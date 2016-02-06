#include "version.h"
#include <time.h>
#include "char.h"
#include "object.h"
#include "lssproto_serv.h"
#include "npcutil.h"
#include "handletime.h"
#include "npc_scheduleman.h"
#include "family.h"

/*
 * 家族 PK 登记员
 *
 * 这个 npc 显示并且登记家族 pk 排程
 * pk 排程以一小时一场 pk 为原则排定 pk 场的赛程, 这个赛程由登记员
 * 登记与设定，主要的设定事项在 FamilyPKSchedule
 *
 * 这个登记员同时会将这个 schedule 写入磁碟，如果 Game Server 重新
 * 启动的话，会先读入这一份对战排程表，同时进行排程。
 * 排程只排从现在开始的 24 小时之内的赛程
 * 排程表档案的格式为:
 *
 * time|host_index|host_name|guest_index|guest_name|prepare_time|max_player|flag|win
 *
 * 档案的命名: 登记员楼层号码_登记员X座标_登记员Y座标
 *
 */

// 全部的家族 pk 赛程
FamilyPKSchedule fmpks[MAX_SCHEDULE*MAX_SCHEDULEMAN];

extern  int     familyNumTotal;

enum {
	NPC_WORK_ID = CHAR_NPCWORKINT1,		// 登记员 ID, 从 0 开始
	NPC_WORK_CHALLENGETIMEOUT = CHAR_NPCWORKINT2,	// 同意挑战的 timeout
	NPC_WORK_SETTINGTIMEOUT = CHAR_NPCWORKINT3,	// 设定挑战的 timeout
	NPC_WORK_PREVIOUSCHECKTIME = CHAR_NPCWORKINT4,	// 上一次检查的时间
	NPC_WORK_FIGHTINTERVAL = CHAR_NPCWORKINT5,	// PK 场次的间隔 (单位: 分)
};

enum {
	CHAR_WORK_PAGE = CHAR_WORKSHOPRELEVANT,		// 换页用
	CHAR_WORK_DUELTIME = CHAR_WORKSHOPRELEVANTSEC,	// 所选择的 pk 时间
};

#define SCHEDULEFILEDIR		"./Schedule/"

void NPC_LoadPKSchedule(int meindex);	// Load schedule from disk
void NPC_SavePKSchedule(int meindex);	// save schedule to disk
void NPC_RemoveExpiredBattle(int meindex);	// 移除过期的战斗
void NPC_ProcessTimeout(int meindex);	// 处理 timeout
BOOL NPC_AlreadyScheduled(int meindex, int talkerindex);	// 检查, 一个家族只能安排一场
// 产生排程表的 data
void NPC_LIST_gendata(int meindex, int talkerindex, int page, char *buf, int size);
// 产生选择家族的 data
void NPC_SELECT_gendata(int meindex, int talkerindex, int page, char *buf, int size);
// 产生排程详细的 data
void NPC_DETAIL_gendata(int meindex, char *buf, int size, int dueltime);

BOOL NPC_SchedulemanInit( int meindex )
{
  char argstr[NPC_UTIL_GETARGSTR_BUFSIZE];
  int meid;
  int interval;

  CHAR_setInt( meindex, CHAR_WHICHTYPE, CHAR_TYPEFMSCHEDULEMAN );
  CHAR_setWorkInt ( meindex, NPC_WORK_PREVIOUSCHECKTIME, -1);

  // 参数
  NPC_Util_GetArgStr(meindex, argstr, sizeof(argstr));
  meid = NPC_Util_GetNumFromStrWithDelim(argstr, "id" );
  if ((meid<0) || (meid>=MAX_SCHEDULEMAN)) {
    print("SCHEDULEMAN init error: invalid ID(%d)\n",meid);
    meid=0;
  }
  CHAR_setWorkInt(meindex, NPC_WORK_ID, meid);

  interval = NPC_Util_GetNumFromStrWithDelim(argstr, "loopinterval" );
  if ((interval<100) || (interval>10000)) interval=1000;
  CHAR_setInt(meindex, CHAR_LOOPINTERVAL, interval);

  interval = NPC_Util_GetNumFromStrWithDelim(argstr, "challengetimeout");
  if ((interval<60) || (interval>60*60)) interval=20*60;
  CHAR_setWorkInt(meindex, NPC_WORK_CHALLENGETIMEOUT, interval);
  
  interval = NPC_Util_GetNumFromStrWithDelim(argstr, "settingtimeout");
  if ((interval<10) || (interval>60*60)) interval=60;
  CHAR_setWorkInt(meindex, NPC_WORK_SETTINGTIMEOUT, interval);

  interval = NPC_Util_GetNumFromStrWithDelim(argstr, "fightinterval");
  if ((interval<5) || (interval>60)) interval = 50;
  CHAR_setWorkInt(meindex, NPC_WORK_FIGHTINTERVAL, interval);


  NPC_LoadPKSchedule(meindex);

  return TRUE;
}

void NPC_SchedulemanTalked(int meindex, int talkerindex, char *msg, int color)
{
  char buf[4096];
  int fd;
  if (NPC_Util_CharDistance(meindex, talkerindex) > 2) return;
  if (!NPC_Util_isFaceToFace(meindex, talkerindex, 2)) return;
  fd = getfdFromCharaIndex(talkerindex);
  if (fd == -1) return;

  CHAR_setWorkInt(talkerindex, CHAR_WORK_PAGE, 0);
  NPC_LIST_gendata(meindex, talkerindex, 0, buf, sizeof(buf));
  lssproto_WN_send(fd, WINDOW_MESSAGETYPE_PKSCHEDULELIST,
        		WINDOW_BUTTONTYPE_NEXT | WINDOW_BUTTONTYPE_CANCEL,
        		CHAR_WINDOWTYPE_SCHEDULEMAN_START,
			CHAR_getWorkInt(meindex, CHAR_WORKOBJINDEX),
			buf);
}

void NPC_SchedulemanWindowTalked(int meindex, int talkerindex,
				int seqno, int select, char *data)
{
  int page;
  int buttontype;
  char buf[4096],token[256];
  int fd,i,dt,a;
  int fmpks_pos;

  if (NPC_Util_CharDistance(meindex,talkerindex) > 2) return;
  if (!NPC_Util_isFaceToFace(meindex, talkerindex, 2)) return;
  fd = getfdFromCharaIndex(talkerindex);
  if (fd == -1) return;
  fmpks_pos = CHAR_getWorkInt(meindex, NPC_WORK_ID)*MAX_SCHEDULE;

//  print("seqno=%d select=%d data=%s\n", seqno,select,data);

  switch (seqno) {
  case CHAR_WINDOWTYPE_SCHEDULEMAN_START:
    switch (select) {
    case WINDOW_BUTTONTYPE_NEXT:
    case WINDOW_BUTTONTYPE_PREV:
      page=CHAR_getWorkInt(talkerindex, CHAR_WORK_PAGE)
           + MAXSCHEDULEINONEWINDOW
             * (( select == WINDOW_BUTTONTYPE_NEXT) ? 1 : -1);
      buttontype=WINDOW_BUTTONTYPE_NEXT | WINDOW_BUTTONTYPE_PREV | WINDOW_BUTTONTYPE_CANCEL;
      if (page<0) {
        page=0;
        buttontype=WINDOW_BUTTONTYPE_NEXT | WINDOW_BUTTONTYPE_CANCEL;
      }
      if (page>MAX_SCHEDULE-MAXSCHEDULEINONEWINDOW) {
        page=MAX_SCHEDULE-MAXSCHEDULEINONEWINDOW;
        buttontype=WINDOW_BUTTONTYPE_PREV | WINDOW_BUTTONTYPE_CANCEL;
      }
      CHAR_setWorkInt(talkerindex, CHAR_WORK_PAGE, page);
      // Send WN
      NPC_LIST_gendata(meindex, talkerindex, page, buf, sizeof(buf));
      lssproto_WN_send(fd, WINDOW_MESSAGETYPE_PKSCHEDULELIST,
        		   buttontype,
        		   CHAR_WINDOWTYPE_SCHEDULEMAN_START,
			   CHAR_getWorkInt(meindex, CHAR_WORKOBJINDEX),
			   buf);
      break;
    case WINDOW_BUTTONTYPE_OK:

#ifdef _FMVER21    
      if ((CHAR_getInt(talkerindex,CHAR_FMLEADERFLAG)==FMMEMBER_LEADER) &&
#else
      if ((CHAR_getInt(talkerindex,CHAR_FMLEADERFLAG)==1) &&
#endif     
		  
        (CHAR_getWorkInt(talkerindex, CHAR_WORKFMSETUPFLAG)==1)) {
        // 只有已成立家族的族长可以使用进一步的功能 (设定、更改、同意)

        // decide: send family list or detail or accept
        dt=atoi(data);
        for (i=0; i<MAX_SCHEDULE; i++) {
          if (fmpks[fmpks_pos+i].dueltime==dt) {
            if (i==0) {
              CHAR_talkToCli(talkerindex, meindex,
                "这个时间无法进行预约。", CHAR_COLORWHITE);
              break;
            }

            switch (fmpks[fmpks_pos+i].flag) {
            case FMPKS_FLAG_NONE:
              if (NPC_AlreadyScheduled(meindex, talkerindex)) {
                // 同一个家族只能有一次排程
                CHAR_talkToCli(talkerindex, meindex,
                  "你已经安排过战斗，将机会留给其他家族吧。", CHAR_COLORWHITE);
              } else {
				
                fmpks[fmpks_pos+i].host_index=CHAR_getWorkInt(talkerindex, CHAR_WORKFMINDEXI);
                strcpy(fmpks[fmpks_pos+i].host_name,makeStringFromEscaped(CHAR_getChar(talkerindex, CHAR_FMNAME)));
                
//                print("CHAR_FMNAME: %s\n",fmpks[fmpks_pos+i].host_name);
                
                fmpks[fmpks_pos+i].guest_index=-1;
                strcpy(fmpks[fmpks_pos+i].guest_name,"");
                fmpks[fmpks_pos+i].prepare_time=5;	// default
                fmpks[fmpks_pos+i].max_player=50;	// default
                fmpks[fmpks_pos+i].win=0;	// default
                fmpks[fmpks_pos+i].flag=FMPKS_FLAG_SETTING;
                fmpks[fmpks_pos+i].setting_timeout=
                    CHAR_getWorkInt(meindex, NPC_WORK_SETTINGTIMEOUT);
                // 送出选择家族的列表
                CHAR_setWorkInt(talkerindex, CHAR_WORK_PAGE, 1); // page 1
                CHAR_setWorkInt(talkerindex, CHAR_WORK_DUELTIME, dt);
                NPC_SELECT_gendata(meindex, talkerindex, 1, buf, sizeof(buf));
                buttontype=0;
                if (familyNumTotal>MAXFAMILYINONEWINDOW)
                  buttontype |= WINDOW_BUTTONTYPE_NEXT;
                lssproto_WN_send(fd, WINDOW_MESSAGETYPE_PKSCHEDULESELECTFAMILY,
        			   buttontype,
        			   CHAR_WINDOWTYPE_SCHEDULEMAN_SELECT,
				   CHAR_getWorkInt(meindex, CHAR_WORKOBJINDEX),
				   buf);

              }
              break;
            case FMPKS_FLAG_CHALLENGE:
              { // 主队族长可以重设条件，客队则是同意 pk，其他人滚蛋
                int tkfmindex=CHAR_getWorkInt(talkerindex, CHAR_WORKFMINDEXI);
                if (tkfmindex==fmpks[fmpks_pos+i].host_index) {
                  fmpks[fmpks_pos+i].flag=FMPKS_FLAG_SETTING;
                  fmpks[fmpks_pos+i].setting_timeout=
                    CHAR_getWorkInt(meindex, NPC_WORK_SETTINGTIMEOUT);
                  // 送出细部调整的列表
                  CHAR_setWorkInt(talkerindex, CHAR_WORK_DUELTIME, dt);
                  NPC_DETAIL_gendata(meindex, buf, sizeof(buf), dt);
                  lssproto_WN_send(fd, WINDOW_MESSAGETYPE_PKSCHEDULEDETAIL,
        	  		   WINDOW_BUTTONTYPE_OK | WINDOW_BUTTONTYPE_CANCEL,
        	 		   CHAR_WINDOWTYPE_SCHEDULEMAN_DETAIL,
				   CHAR_getWorkInt(meindex, CHAR_WORKOBJINDEX),
				   buf);
                } else if (tkfmindex==fmpks[fmpks_pos+i].guest_index) {
					fmpks[fmpks_pos+i].flag=FMPKS_FLAG_SCHEDULED;
  					page=CHAR_getWorkInt(talkerindex, CHAR_WORK_PAGE);
   					buttontype=WINDOW_BUTTONTYPE_NEXT | WINDOW_BUTTONTYPE_PREV | WINDOW_BUTTONTYPE_OK;
					if (page<0) {
						page=0;
						buttontype=WINDOW_BUTTONTYPE_NEXT | WINDOW_BUTTONTYPE_OK;
					}
					if (page>MAX_SCHEDULE-MAXSCHEDULEINONEWINDOW) {
 						page=MAX_SCHEDULE-MAXSCHEDULEINONEWINDOW;
						buttontype=WINDOW_BUTTONTYPE_PREV | WINDOW_BUTTONTYPE_OK;
					} 
                    CHAR_talkToCli(talkerindex, meindex,
						"你的家族已经接受挑战。", CHAR_COLORWHITE);
                    // 重新送出 list
      				NPC_LIST_gendata(meindex, talkerindex, page, buf, sizeof(buf));
					lssproto_WN_send(fd, WINDOW_MESSAGETYPE_PKSCHEDULELIST,
        			   buttontype,
        			   CHAR_WINDOWTYPE_SCHEDULEMAN_START,
				    CHAR_getWorkInt(meindex, CHAR_WORKOBJINDEX),
				    buf);
				}
              } 
              break;
            case FMPKS_FLAG_SETTING:
              { // 主队族长可以重设条件
                int tkfmindex=CHAR_getWorkInt(talkerindex, CHAR_WORKFMINDEXI);
                if (tkfmindex==fmpks[fmpks_pos+i].host_index) {
                  fmpks[fmpks_pos+i].flag=FMPKS_FLAG_SETTING;
                  fmpks[fmpks_pos+i].setting_timeout=
                    CHAR_getWorkInt(meindex, NPC_WORK_SETTINGTIMEOUT);
                  // 送出细部调整的列表
                  CHAR_setWorkInt(talkerindex, CHAR_WORK_DUELTIME, dt);
                  NPC_DETAIL_gendata(meindex, buf, sizeof(buf), dt);
                  lssproto_WN_send(fd, WINDOW_MESSAGETYPE_PKSCHEDULEDETAIL,
        	  		   WINDOW_BUTTONTYPE_OK | WINDOW_BUTTONTYPE_CANCEL,
        	 		   CHAR_WINDOWTYPE_SCHEDULEMAN_DETAIL,
				   CHAR_getWorkInt(meindex, CHAR_WORKOBJINDEX),
				   buf);
				}
              }    
              // WON ADD 修正家族pk场的约战问题
			  break;
			}

          }
        }
        NPC_SavePKSchedule(meindex);
	} else {
#ifdef _FMVER21      
        if (CHAR_getInt(talkerindex,CHAR_FMLEADERFLAG)!=FMMEMBER_LEADER) {
#else
        if (CHAR_getInt(talkerindex,CHAR_FMLEADERFLAG)!=1) {
#endif         
          CHAR_talkToCli(talkerindex, meindex,
            "只有族长才能预约家族ＰＫ喔。", CHAR_COLORWHITE);
		}else if (CHAR_getWorkInt(talkerindex, CHAR_WORKFMSETUPFLAG)!=1) {
          CHAR_talkToCli(talkerindex, meindex,
            "你的家族还没有正式成立喔。", CHAR_COLORWHITE);
        }
      }
      break;
    }
    break;
  case CHAR_WINDOWTYPE_SCHEDULEMAN_SELECT:
    switch (select) {
    case WINDOW_BUTTONTYPE_PREV:
    case WINDOW_BUTTONTYPE_NEXT:
      page=CHAR_getWorkInt(talkerindex, CHAR_WORK_PAGE)
           + MAXFAMILYINONEWINDOW
             * (( select == WINDOW_BUTTONTYPE_NEXT) ? 1 : -1);
      buttontype=WINDOW_BUTTONTYPE_NEXT | WINDOW_BUTTONTYPE_PREV;
      if (page<1) {
        page=1;
        buttontype=WINDOW_BUTTONTYPE_NEXT;
      }
      if (page>familyNumTotal-MAXFAMILYINONEWINDOW+1) {
        page=familyNumTotal-MAXFAMILYINONEWINDOW+1;
        buttontype=WINDOW_BUTTONTYPE_PREV;
      }
      CHAR_setWorkInt(talkerindex, CHAR_WORK_PAGE, page);
      // Send WN
      NPC_SELECT_gendata(meindex, talkerindex, page, buf, sizeof(buf));
      lssproto_WN_send(fd, WINDOW_MESSAGETYPE_PKSCHEDULESELECTFAMILY,
        		   buttontype,
        		   CHAR_WINDOWTYPE_SCHEDULEMAN_SELECT,
			   CHAR_getWorkInt(meindex, CHAR_WORKOBJINDEX),
			   buf);
      break;
    case WINDOW_BUTTONTYPE_OK:

#ifdef _FMVER21    
      if ((CHAR_getInt(talkerindex,CHAR_FMLEADERFLAG)==FMMEMBER_LEADER) &&
#else
      if ((CHAR_getInt(talkerindex,CHAR_FMLEADERFLAG)==1) &&
#endif       
          (CHAR_getWorkInt(talkerindex, CHAR_WORKFMSETUPFLAG)==1)) {

        dt=CHAR_getWorkInt(talkerindex, CHAR_WORK_DUELTIME);
        for (i=0; i<MAX_SCHEDULE; i++) {
          if (fmpks[fmpks_pos+i].dueltime==dt) {
            print("host_index: %d %d\n",fmpks[fmpks_pos+i].host_index,CHAR_getWorkInt(talkerindex, CHAR_WORKFMINDEXI));
            if (fmpks[fmpks_pos+i].host_index==CHAR_getWorkInt(talkerindex, CHAR_WORKFMINDEXI)) {
              print("data:%s\n",data);
              if (getStringFromIndexWithDelim(data,"|",1,token,sizeof(token))) {
                a=atoi(token);
                if (a!=fmpks[fmpks_pos+i].host_index) {
                  fmpks[fmpks_pos+i].guest_index=a;
                  // 必须要再检查是否有这个客队家族 /**/
                  if (getStringFromIndexWithDelim(data,"|",2,token,sizeof(token))) {
                    strcpy(fmpks[fmpks_pos+i].guest_name, makeStringFromEscaped(token));
                    // 送出 detail 编辑窗
                    NPC_DETAIL_gendata(meindex, buf, sizeof(buf), dt);
                    lssproto_WN_send(fd, WINDOW_MESSAGETYPE_PKSCHEDULEDETAIL,
        	  		   WINDOW_BUTTONTYPE_OK | WINDOW_BUTTONTYPE_CANCEL,
        	 		   CHAR_WINDOWTYPE_SCHEDULEMAN_DETAIL,
				   CHAR_getWorkInt(meindex, CHAR_WORKOBJINDEX),
				   buf);
                  } else fmpks[fmpks_pos+i].flag=FMPKS_FLAG_NONE;
                } else fmpks[fmpks_pos+i].flag=FMPKS_FLAG_NONE;
              } else fmpks[fmpks_pos+i].flag=FMPKS_FLAG_NONE;
            }
          }
        }
      }
      break;
    }
    break;
  case CHAR_WINDOWTYPE_SCHEDULEMAN_DETAIL:

#ifdef _FMVER21  
    if ((CHAR_getInt(talkerindex,CHAR_FMLEADERFLAG)==FMMEMBER_LEADER) &&
#else
    if ((CHAR_getInt(talkerindex,CHAR_FMLEADERFLAG)==1) &&
#endif    
        (CHAR_getWorkInt(talkerindex, CHAR_WORKFMSETUPFLAG)==1)) {

      dt=CHAR_getWorkInt(talkerindex, CHAR_WORK_DUELTIME);
      for (i=0; i<MAX_SCHEDULE; i++) {
        if (fmpks[fmpks_pos+i].dueltime==dt) {
          if (fmpks[fmpks_pos+i].host_index==CHAR_getWorkInt(talkerindex, CHAR_WORKFMINDEXI)) {
            switch (select) {
            case WINDOW_BUTTONTYPE_OK:
              {
                // 准备时间
                if (getStringFromIndexWithDelim(data,"|",4,token,sizeof(token))) {
                  a=atoi(token);
                  if ((a>0) && (a<=40)) fmpks[fmpks_pos+i].prepare_time=a;
                }
                // 最大人数
                if (getStringFromIndexWithDelim(data,"|",5,token,sizeof(token))) {
                  a=atoi(token);
                  if ((a>0) && (a<=50)) fmpks[fmpks_pos+i].max_player=a;
                }
                // 胜利条件
                if (getStringFromIndexWithDelim(data,"|",6,token,sizeof(token))) {
                  a=atoi(token);
                  if ((a>=0) && (a<=1)) fmpks[fmpks_pos+i].win = a;
                }
                fmpks[fmpks_pos+i].flag=FMPKS_FLAG_CHALLENGE;
                fmpks[fmpks_pos+i].challenge_timeout=
                    CHAR_getWorkInt(meindex, NPC_WORK_CHALLENGETIMEOUT);
                CHAR_talkToCli(talkerindex, meindex,
                    "家族挑战设定完成。", CHAR_COLORWHITE);
              }
              break;
            case WINDOW_BUTTONTYPE_CANCEL:
              fmpks[fmpks_pos+i].flag=FMPKS_FLAG_NONE;
              fmpks[fmpks_pos+i].host_index=-1;
              strcpy(fmpks[fmpks_pos+i].host_name,"");
              fmpks[fmpks_pos+i].guest_index=-1;
              strcpy(fmpks[fmpks_pos+i].guest_name,"");
              CHAR_talkToCli(talkerindex, meindex,
                  "清除家族挑战。", CHAR_COLORWHITE);
              break;
            }
            NPC_SavePKSchedule(meindex);
          }
        }
      }
    }
    break;
  }
}

void NPC_SchedulemanLoop(int meindex)
{
//  int fmpks_pos = CHAR_getWorkInt(meindex, NPC_WORK_ID)*MAX_SCHEDULE;

/*
  print("Scheduleman: fmpks_pos=%d dueltime=%d host=%s guest=%s\n",
       fmpks_pos, fmpks[fmpks_pos].dueltime, fmpks[fmpks_pos].host_name, fmpks[fmpks_pos].guest_name);
*/

  NPC_RemoveExpiredBattle(meindex);
  NPC_ProcessTimeout(meindex);
}

// 移除过期的战斗
void NPC_RemoveExpiredBattle(int meindex)
{
  struct tm tm1;
  int keeptime;
  int i,expired=-1;
  int fmpks_pos = CHAR_getWorkInt(meindex, NPC_WORK_ID)*MAX_SCHEDULE;
  int prevckt = CHAR_getWorkInt(meindex, NPC_WORK_PREVIOUSCHECKTIME);
  int fin = CHAR_getWorkInt(meindex, NPC_WORK_FIGHTINTERVAL);
  int h,d;

  // 以目前的时刻当标准去移除过期的排程
  memcpy( &tm1, localtime( (time_t *)&NowTime.tv_sec), sizeof( tm1));

  if (tm1.tm_min<fin)
    keeptime = (tm1.tm_hour-1)*100 + 60 - (fin - tm1.tm_min);
  else
    keeptime = tm1.tm_hour*100 + tm1.tm_min - fin;

  CHAR_setWorkInt(meindex, NPC_WORK_PREVIOUSCHECKTIME, keeptime);
  if (keeptime<prevckt) keeptime+=10000;	// 隔日

  // 计算哪些是过期的
  i=0;
  while ((i<MAX_SCHEDULE) && (fmpks[fmpks_pos+i].dueltime<=keeptime)) {
    expired=i;
    i++;
  }
  
  // 如果第一项排程与目前时间差异太大, 则整个排程全部无效
  if (expired==-1) {
    for (i=0; i<3; i++) {	  // tolerance 是 3 个 fight interval
      keeptime += fin;
      if ((keeptime % 100)>=60) keeptime = keeptime + 100 - 60;
    }
    if (fmpks[fmpks_pos].dueltime > keeptime) expired=23;
  }

  if (expired>=0) {
    // 移动後面的排程来取代已经失效的排程
    for (i=expired+1; i<MAX_SCHEDULE; i++) {
      memcpy(&fmpks[fmpks_pos+i-expired-1],
             &fmpks[fmpks_pos+i],sizeof(FamilyPKSchedule));
    }

    // 将後面的排程设成未排程
    for (i=MAX_SCHEDULE-expired-1; i<MAX_SCHEDULE; i++) {
      memset(&fmpks[fmpks_pos+i], 0, sizeof(FamilyPKSchedule));
      fmpks[fmpks_pos+i].flag=-1;
// Terry add 2004/06/10 要把host_index 和 guest_index 设定为 -1
			fmpks[fmpks_pos+i].host_index = -1;
			fmpks[fmpks_pos+i].guest_index = -1;
// end
    }

    // 重新定义时刻
    if (expired==23)
      fmpks[fmpks_pos].dueltime = tm1.tm_hour*100 + ((int)(tm1.tm_min/fin))*fin;

    h=(fmpks[fmpks_pos].dueltime%10000);
    d=0;
    for (i=1; i<MAX_SCHEDULE; i++) {
      h=h+fin;
      if ( (h%100) >=60 ) h=h+100-60;
      if (h>=2400) {
        h = h-2400;
        d = 10000;
      }
      fmpks[fmpks_pos+i].dueltime=d+h;
    }
    
    // 检查第一项排程, 如果不在排定的状况下则设为『无排程』
    if (fmpks[fmpks_pos].flag < FMPKS_FLAG_SCHEDULED) {
      fmpks[fmpks_pos].flag = FMPKS_FLAG_NONE;
      fmpks[fmpks_pos+i].host_index=-1;
      strcpy(fmpks[fmpks_pos+i].host_name,"");
      fmpks[fmpks_pos+i].guest_index=-1;
      strcpy(fmpks[fmpks_pos+i].guest_name,"");
    }
    
  }
}

// 处理 timeout
void NPC_ProcessTimeout(int meindex)
{
  int i;
  int fmpks_pos = CHAR_getWorkInt(meindex, NPC_WORK_ID)*MAX_SCHEDULE;

  for (i=0; i<MAX_SCHEDULE; i++) {
    if (fmpks[fmpks_pos+i].flag==FMPKS_FLAG_CHALLENGE) {
      if (fmpks[fmpks_pos+i].challenge_timeout>=0) {
        fmpks[fmpks_pos+i].challenge_timeout--;
        if (fmpks[fmpks_pos+i].challenge_timeout<0) {
	  fmpks[fmpks_pos+i].host_index = -1;
	  strcpy(fmpks[fmpks_pos+i].host_name, "-1");
	  fmpks[fmpks_pos+i].guest_index = -1;
	  strcpy(fmpks[fmpks_pos+i].guest_name, "-1");
	  fmpks[fmpks_pos+i].prepare_time = -1;
	  fmpks[fmpks_pos+i].max_player = -1;
          fmpks[fmpks_pos+i].flag=-1;
          fmpks[fmpks_pos+i].win=-1;
        }
      }
    }
    if (fmpks[fmpks_pos+i].flag==FMPKS_FLAG_SETTING) {
      if (fmpks[fmpks_pos+i].setting_timeout>=0) {
        fmpks[fmpks_pos+i].setting_timeout--;
        if (fmpks[fmpks_pos+i].setting_timeout<0) {
	  fmpks[fmpks_pos+i].host_index = -1;
	  strcpy(fmpks[fmpks_pos+i].host_name, "-1");
	  fmpks[fmpks_pos+i].guest_index = -1;
	  strcpy(fmpks[fmpks_pos+i].guest_name, "-1");
	  fmpks[fmpks_pos+i].prepare_time = -1;
	  fmpks[fmpks_pos+i].max_player = -1;
          fmpks[fmpks_pos+i].flag=-1;
          fmpks[fmpks_pos+i].win=-1;
        }
      }
    }
  }
}

// 检查, 一个家族只能安排一场战斗
BOOL NPC_AlreadyScheduled(int meindex, int talkerindex)
{
  int i;
  int fmpks_pos = CHAR_getWorkInt(meindex, NPC_WORK_ID)*MAX_SCHEDULE;
  int tkfmindex = CHAR_getWorkInt(talkerindex, CHAR_WORKFMINDEXI);
  int flag=1;

  for (i=0; i<MAX_SCHEDULE; i++) {
    if (fmpks[fmpks_pos+i].flag>=FMPKS_FLAG_SCHEDULED) {
      if (fmpks[fmpks_pos+i].host_index==tkfmindex) return TRUE;
    } else if (fmpks[fmpks_pos+i].flag!=FMPKS_FLAG_NONE) {
      if (flag==1) flag=0; else return TRUE;
    }
  }
  return FALSE;
}

// 读取 schedule 档案
void NPC_LoadPKSchedule(int meindex)
{
  char filename[256],tmp[4096],token[256];
  FILE *f;
  int i;
  int fmpks_pos = CHAR_getWorkInt(meindex, NPC_WORK_ID)*MAX_SCHEDULE;

  snprintf(filename,sizeof(filename), "%s%d_%d_%d",
           SCHEDULEFILEDIR, CHAR_getInt(meindex, CHAR_FLOOR),
           CHAR_getInt(meindex, CHAR_X),
           CHAR_getInt(meindex, CHAR_Y) );

  if( ! (f=fopen( filename, "r" )) ){	// create new schedule file
    f = fopen( filename, "w" );
    if( !f ){
      print( "ERROR:Can't create Schedule file %s!\n",filename );
      return;
    }
    for (i=0; i<MAX_SCHEDULE; i++){
      sprintf(tmp,"%d|-1|-1|-1|-1|-1|-1|-1\n",i);
      fwrite( tmp, strlen(tmp), 1, f);
    }
  }

  fseek(f, 0, SEEK_SET);	// 档案开头
  for( i = 0; i < MAX_SCHEDULE; i++ ){
    fgets(tmp, sizeof(tmp), f);
    fmpks[fmpks_pos+i].flag=-1;

    // 时间
    if (getStringFromIndexWithDelim(tmp,"|",1,token,sizeof(token))) {
      fmpks[fmpks_pos+i].dueltime=atoi(token);
    } else continue;
    // 主队 familyindex
    if (getStringFromIndexWithDelim(tmp,"|",2,token,sizeof(token))) {
      fmpks[fmpks_pos+i].host_index=atoi(token);
    } else continue;
    // 主队 家族名
    if (getStringFromIndexWithDelim(tmp,"|",3,token,sizeof(token))) {
      strcpy(fmpks[fmpks_pos+i].host_name,makeStringFromEscaped(token));
    } else continue;
    // 客队 familyindex
    if (getStringFromIndexWithDelim(tmp,"|",4,token,sizeof(token))) {
      fmpks[fmpks_pos+i].guest_index=atoi(token);
    } else continue;
    // 客队 家族名
    if (getStringFromIndexWithDelim(tmp,"|",5,token,sizeof(token))) {
      strcpy(fmpks[fmpks_pos+i].guest_name,makeStringFromEscaped(token));
    } else continue;
    // 准备时间
    if (getStringFromIndexWithDelim(tmp,"|",6,token,sizeof(token))) {
      fmpks[fmpks_pos+i].prepare_time=atoi(token);
    } else continue;
    // 最大人数
    if (getStringFromIndexWithDelim(tmp,"|",7,token,sizeof(token))) {
		//andy_reEdit 2003/06/17
		int maxnum = atoi(token);
		if( maxnum < 50 ) maxnum = 50;
		fmpks[fmpks_pos+i].max_player = maxnum;
    } else continue;
    // 旗标
    if (getStringFromIndexWithDelim(tmp,"|",8,token,sizeof(token))) {
      fmpks[fmpks_pos+i].flag=atoi(token);
    } else continue;
    // 胜利条件
    if (getStringFromIndexWithDelim(tmp,"|",9,token,sizeof(token))) {
      fmpks[fmpks_pos+i].win=atoi(token);
    } else continue;
  }
  fclose(f);

  // 第 0 笔预约不算
  fmpks[fmpks_pos].host_index=-1;
  strcpy(fmpks[fmpks_pos].host_name,"");
  fmpks[fmpks_pos].guest_index=-1;
  strcpy(fmpks[fmpks_pos].guest_name,"");
  fmpks[fmpks_pos].prepare_time=-1;
  fmpks[fmpks_pos].max_player=-1;
  fmpks[fmpks_pos].flag=-1;
  fmpks[fmpks_pos].win=-1;
  
  NPC_RemoveExpiredBattle(meindex);
}

void NPC_SavePKSchedule(int meindex)
{
  char filename[256],tmp[4096],n1[256],n2[256];
  FILE *f;
  int i;
  int fmpks_pos = CHAR_getWorkInt(meindex, NPC_WORK_ID)*MAX_SCHEDULE;

  snprintf(filename,sizeof(filename), "%s%d_%d_%d",
           SCHEDULEFILEDIR, CHAR_getInt(meindex, CHAR_FLOOR),
           CHAR_getInt(meindex, CHAR_X),
           CHAR_getInt(meindex, CHAR_Y) );
           
  f=fopen(filename, "w");	// create new

  for( i = 0; i < MAX_SCHEDULE; i++ ){
    makeEscapeString(fmpks[fmpks_pos+i].host_name,n1,sizeof(n1));
    makeEscapeString(fmpks[fmpks_pos+i].guest_name,n2,sizeof(n2));
/*  
    print("SavePK:%s %s %s %s\n",fmpks[fmpks_pos+i].host_name,n1,
                                 fmpks[fmpks_pos+i].guest_name,n2);
*/
    sprintf(tmp, "%d|%d|%s|%d|%s|%d|%d|%d|%d\n",
    	fmpks[fmpks_pos+i].dueltime,
    	fmpks[fmpks_pos+i].host_index,
    	n1,
    	fmpks[fmpks_pos+i].guest_index,
    	n2,
    	fmpks[fmpks_pos+i].prepare_time,
    	fmpks[fmpks_pos+i].max_player,
    	fmpks[fmpks_pos+i].flag,
    	fmpks[fmpks_pos+i].win);
  
    fwrite(tmp, strlen(tmp), 1, f);
  }
  fclose(f);
}

// 产生 WN_PKSCHEDULELIST 的 data
void NPC_LIST_gendata(int meindex, int talkerindex, int page, char *buf, int size)
{
  char tmp[4096], n1[256], n2[256];
  int i,flag;
  int fmpks_pos = CHAR_getWorkInt(meindex, NPC_WORK_ID)*MAX_SCHEDULE;
  int tkfmindex = CHAR_getWorkInt(talkerindex, CHAR_WORKFMINDEXI);
  int tkfmleader = CHAR_getInt(talkerindex, CHAR_FMLEADERFLAG);
  struct tm tm1;

  memset(buf, 0, size);		// clear buffer
  if ((page<0)||(page>=MAX_SCHEDULE)) return;

  // 记录目前的时间
  memcpy( &tm1, localtime( (time_t *)&NowTime.tv_sec), sizeof( tm1));
  sprintf(buf, "%d|", tm1.tm_hour*100 + tm1.tm_min);
  
  for (i=page; i<MAX_SCHEDULE && i<page+4; i++) {
    if (i!=page) strcat(buf, "|");
    makeEscapeString(fmpks[fmpks_pos+i].host_name,n1,sizeof(n1));
    makeEscapeString(fmpks[fmpks_pos+i].guest_name,n2,sizeof(n2));
    flag=-1;
    switch (fmpks[fmpks_pos+i].flag) {
    case FMPKS_FLAG_CHALLENGE:
      if (tkfmindex==fmpks[fmpks_pos+i].host_index) {
#ifdef _FMVER21      
        if (tkfmleader==FMMEMBER_LEADER) flag=FLAG_MODIFY; else flag=FLAG_CONFIRM;
#else
        if (tkfmleader==1) flag=FLAG_MODIFY; else flag=FLAG_CONFIRM;
#endif        
      } else if (tkfmindex==fmpks[fmpks_pos+i].guest_index) {
#ifdef _FMVER21      
        if (tkfmleader==FMMEMBER_LEADER) flag=FLAG_ACCEPT; else flag=FLAG_CONFIRM;
#else
        if (tkfmleader==1) flag=FLAG_ACCEPT; else flag=FLAG_CONFIRM;
#endif        
      } else flag=FLAG_CONFIRM;
      break;
    case FMPKS_FLAG_SETTING:
      flag=FLAG_SETTING;
      break;
    case FMPKS_FLAG_CONFIRMING:
      flag=FLAG_CONFIRM;
      break;
    case FMPKS_FLAG_SCHEDULED:
    case FMPKS_FLAG_DUEL:
    case FMPKS_FLAG_HOSTWIN:
    case FMPKS_FLAG_GUESTWIN:
      flag=FLAG_SCHEDULED;
      break;
    }

    sprintf(tmp, "%d|%s|%s|%d|%d|%d|%d",
    	fmpks[fmpks_pos+i].dueltime,
    	n1,
    	n2,
    	fmpks[fmpks_pos+i].prepare_time,
    	fmpks[fmpks_pos+i].max_player,
    	flag,
    	fmpks[fmpks_pos+i].win);
    if (strlen(buf)+strlen(tmp)<size) {
      strcat(buf, tmp);
    } else {
      return;
    }
  }
}

// 产生 WN_PKSCHEDULESELECTFAMILY 的 data
void NPC_SELECT_gendata(int meindex, int talkerindex, int page, char *buf, int size)
{
  int i,j,cnt=0;
  char token[4096];
  char fmindex[256];
  char fmname[256];
  int tkfmindex = CHAR_getWorkInt(talkerindex, CHAR_WORKFMINDEXI);
  
  memset(buf, 0, size);		// clear buffer
  strcpy(buf,"0");
  for (i=0,j=0; i<MAXFAMILYINONEWINDOW; i++,j++) {
    if (getStringFromIndexWithDelim(familyListBuf,"|",page+j,token,sizeof(token))) {
      if ( (getStringFromIndexWithDelim(token," ",1,fmindex,sizeof(fmindex))) &&
           (getStringFromIndexWithDelim(token," ",2,fmname,sizeof(fmname))) ) {
        if (tkfmindex!=atoi(fmindex)-1) {
          sprintf(token,"|%d|%s",(atoi(fmindex)-1),fmname);
          if (strlen(buf)+strlen(token)<size) {
            strcat(buf, token);
            cnt++;
            buf[0]=cnt+'0';
          } else return;
        } else i=i-1;
      }
    }
  }
}

// 产生 WN_PKSCHEDULEDETAIL 的 data
void NPC_DETAIL_gendata(int meindex, char *buf, int size, int dueltime)
{
  char n1[256], n2[256];
  int i=0;
  int fmpks_pos = CHAR_getWorkInt(meindex, NPC_WORK_ID)*MAX_SCHEDULE;
  
//  print("DETAIL: dueltime=%d\n",dueltime);
  
  while ((i<MAX_SCHEDULE)&&(fmpks[fmpks_pos+i].dueltime!=dueltime))
	  i++;

  if (fmpks[fmpks_pos+i].dueltime==dueltime) {
    makeEscapeString(fmpks[fmpks_pos+i].host_name,n1,sizeof(n1));
    makeEscapeString(fmpks[fmpks_pos+i].guest_name,n2,sizeof(n2));
    sprintf(buf, "%d|%s|%s|%d|%d|%d",
    	fmpks[fmpks_pos+i].dueltime,
    	n1,
    	n2,
    	fmpks[fmpks_pos+i].prepare_time,
    	fmpks[fmpks_pos+i].max_player,
    	fmpks[fmpks_pos+i].win);
  } else {
    strcpy(buf, "-1|??|??|0|0");
    print("scheduleman: gen DETAIL error (%d).",dueltime);
  }
}
