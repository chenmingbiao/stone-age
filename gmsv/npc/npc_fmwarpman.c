#include "version.h"
#include <time.h>
#include "char.h"
#include "object.h"
#include "char_base.h"
#include "npcutil.h"
#include "lssproto_serv.h"
#include "saacproto_cli.h"
#include "npc_fmwarpman.h"
#include "npc_scheduleman.h"
#include "readmap.h"
#include "log.h"
#include "battle.h"
#include "handletime.h"
#include "family.h"
#include "errno.h"
#include "configfile.h"

#ifdef _FM_POINT_PK
extern struct  FM_POINTLIST fmpointlist;                   // 家族据点
#endif

#define FMWARPMAN_INIT_LOOPTIME		600	// 0.1秒
#define FMWARPMAN_FREE_LOOPTIME		9000	// 1.5秒
#define FMWARPMAN_BUSY_LOOPTIME		3000	// 0.5秒
#define FMWARPMAN_WAIT_LOOPTIME		18000	// 3秒
#define FMWARPMAN_CLEANPLACE		59
//#define FMWARPMAN_CLEANPLACE		9
#define	TRUE	1
#define FALSE	0
// CoolFish Rem 2002/2/25
// #define MANOR	4

enum {
	NPC_WORK_MODEFLAG =  CHAR_NPCWORKINT1,
	NPC_WORK_CURRENTTIME = CHAR_NPCWORKINT2,
	NPC_WORK_WARPFLOOR = CHAR_NPCWORKINT3,
	NPC_WORK_FMNUMI = CHAR_NPCWORKINT4,
	NPC_WORK_FMNUMII = CHAR_NPCWORKINT5,
	NPC_WORK_ID = CHAR_NPCWORKINT6,
	NPC_WORK_TIMEFLAG = CHAR_NPCWORKINT9,
	NPC_WORK_TALKFLAG = CHAR_NPCWORKINT10,
	NPC_WORK_CleanTime = CHAR_NPCWORKINT11,
};

enum {
	NPC_STATEINIT,
	NPC_STATEFREE,
	NPC_STATEBUSY,
	NPC_STATEWAIT,
};

enum {
	NPC_WORK_WINFMNAME = CHAR_NPCWORKCHAR1,
};

void NPC_ERR_FMDiSP(int meindex, int talker, int errNO);
static void NPC_FMWarpMan_selectWindow(int meindex, int toindex, int num, int select);
void NPC_FMBATTLESET(int floor, int index1, int index2, int flag);
void NPC_WarpFamily(int floor, int index1, int index2, int fl, int x, int y);
void NPC_BattleOut(int fmindex, int fmindex1);
void NPC_CleanPK(int floor, int meindex);
int NPC_FMFloorUse(int floor);
void NPC_talkToFloor(int floor, int index1, int index2, char *data);
void CheckLeavePK(int npcindex, int floor, int index1, int index2);

void CHECK_FMPknumInFloor( int meindex)
{
	int fmnum1 = 0, fmnum2 = 0;
	int fmpks_pos;

	fmpks_pos = CHAR_getWorkInt(meindex, NPC_WORK_ID) * MAX_SCHEDULE;
	// Nuke 20040920: Bug fix 庄园人数满时要检查总人数
	//print("<<%d %d>>",fmpks_pos,MAX_SCHEDULEMAN);
	//if( fmpks_pos < 0 || fmpks_pos >= MAX_SCHEDULEMAN ) return;
	if( fmpks_pos < 0 || fmpks_pos >= MAX_SCHEDULEMAN * MAX_SCHEDULE ) return;
	NPC_GetPKFMNum(CHAR_getWorkInt(meindex, NPC_WORK_WARPFLOOR),
		         		fmpks[fmpks_pos].host_index,
		         		fmpks[fmpks_pos].guest_index,
		         		&fmnum1, &fmnum2 );

	CHAR_setWorkInt( meindex, NPC_WORK_FMNUMI , fmnum1);
	CHAR_setWorkInt( meindex, NPC_WORK_FMNUMII, fmnum2);
}

extern int familyMemberIndex[FAMILY_MAXNUM][FAMILY_MAXMEMBER];

BOOL NPC_FMWarpManInit( int meindex )
{
	char npcarg[NPC_UTIL_GETARGSTR_BUFSIZE];
	char buff2[256];
	char buf[1024];
	int fl, x, y, meid;
	// shan
	int fl1, x1, y1;

	if(NPC_Util_GetArgStr( meindex, npcarg, sizeof(npcarg))==NULL){
		print("FMWarpMan:GetArgStrErr");
		return FALSE;
	}

	if(NPC_Util_GetStrFromStrWithDelim( npcarg, "WARP1", buf, sizeof( buf))==NULL){
	        print("FMWarpman Err is %s",npcarg);
		print("FMWarpman Err");
		return FALSE;
	}

	getStringFromIndexWithDelim(buf,",",1,buff2,sizeof(buff2));
	fl=atoi(buff2);
	CHAR_setWorkInt(meindex, NPC_WORK_WARPFLOOR, fl);
	getStringFromIndexWithDelim(buf,",",2,buff2,sizeof(buff2));
	x=atoi(buff2);
	getStringFromIndexWithDelim(buf,",",3,buff2,sizeof(buff2));
	y=atoi(buff2);

	if( MAP_IsValidCoordinate( fl,x,y )== FALSE ){
		print( "FMWarp NPC:Invalid warpman ERR" );
		return FALSE;
	}

    // shan begin
	if(NPC_Util_GetStrFromStrWithDelim( npcarg, "WARP2", buf, sizeof( buf))==NULL){
	        print("FMWarpman Err is %s",npcarg);
		print("FMWarpman Err");
		return FALSE;
	}
	getStringFromIndexWithDelim(buf,",",1,buff2,sizeof(buff2));
	fl1=atoi(buff2);
	CHAR_setWorkInt(meindex, NPC_WORK_WARPFLOOR, fl);
	getStringFromIndexWithDelim(buf,",",2,buff2,sizeof(buff2));
	x1=atoi(buff2);
	getStringFromIndexWithDelim(buf,",",3,buff2,sizeof(buff2));
	y1=atoi(buff2);
	if( MAP_IsValidCoordinate( fl1,x1,y1 )== FALSE ){
		print( "FMWarp NPC:Invalid warpman ERR" );
		return FALSE;
	}
	// shan end
	
	meid = NPC_Util_GetNumFromStrWithDelim(npcarg, "ID");
	if ((meid < 0) || (meid >= MAX_SCHEDULEMAN))
	{
		print("FMWARP NPC: Init error invalid ID:%d\n", meid);
		return FALSE;
	}

	/*--正奶皿涩烂--*/
	CHAR_setInt(meindex, CHAR_LOOPINTERVAL, FMWARPMAN_INIT_LOOPTIME);
   	CHAR_setInt( meindex , CHAR_WHICHTYPE , CHAR_TYPEWARPMAN );
	CHAR_setWorkInt(meindex, NPC_WORK_MODEFLAG, NPC_STATEINIT);
	CHAR_setWorkInt(meindex, NPC_WORK_CURRENTTIME, NowTime.tv_sec);
	CHAR_setWorkInt(meindex, NPC_WORK_TIMEFLAG, 0);
	CHAR_setWorkInt(meindex, NPC_WORK_TALKFLAG, -1);
	CHAR_setWorkInt(meindex, NPC_WORK_ID, meid);
	CHAR_setWorkInt(meindex, NPC_WORK_FMNUMI, 0);
	CHAR_setWorkInt(meindex, NPC_WORK_FMNUMII, 0);

	CHAR_setWorkInt( meindex, NPC_WORK_CleanTime, 6*10);
	
    return TRUE;
}

void NPC_FMWarpManTalked(int meindex, int talkerindex, char *szMes, int color)
{
	char npcarg[NPC_UTIL_GETARGSTR_BUFSIZE];
	
	CHAR_setWorkInt( talkerindex, CHAR_WORKWARPCHECK, FALSE );
	
    if( CHAR_getInt( talkerindex , CHAR_WHICHTYPE ) != CHAR_TYPEPLAYER ) {
    	return;
    }

	if(NPC_Util_isFaceToFace( meindex ,talkerindex ,2)==FALSE){
		if(NPC_Util_isFaceToChara(talkerindex,meindex,1 )==FALSE) return;
	}

	if(NPC_Util_GetArgStr( meindex, npcarg, sizeof(npcarg))==NULL){
		print("GetArgStrErr");
		return ;
	}

	if(CHAR_getWorkInt(talkerindex, CHAR_WORKPARTYMODE) != CHAR_PARTY_NONE){
		NPC_ERR_FMDiSP( meindex, talkerindex, 1);
	}

	CHAR_setWorkInt( talkerindex , CHAR_WORKSHOPRELEVANTTRD , 0 );
	NPC_FMWarpMan_selectWindow( meindex, talkerindex, 0, -1);
}

void NPC_FMWarpManLoop(int meindex)
{
	struct tm tm1;
	struct tm *tm2;
	int fmpks_pos = CHAR_getWorkInt(meindex, NPC_WORK_ID) * MAX_SCHEDULE;
	int floor = CHAR_getWorkInt(meindex, NPC_WORK_WARPFLOOR);
	// WON ADD 修正snprintf会导致当机的bug
	if( (tm2=localtime((time_t *)&NowTime.tv_sec) ) == NULL ){
		print("\n won ==> time err !! ");
		return;
	}
	memcpy(&tm1, localtime((time_t *)&NowTime.tv_sec), sizeof(tm1));

	if (tm1.tm_min == 0)
   	CHAR_setWorkInt(meindex, NPC_WORK_TIMEFLAG, tm1.tm_min);
	if (CHAR_getWorkInt(meindex, NPC_WORK_MODEFLAG) == NPC_STATEINIT)
	{
		if (tm1.tm_sec == 0)
		{
			CHAR_setWorkInt(meindex, NPC_WORK_MODEFLAG, NPC_STATEFREE);
			CHAR_setInt(meindex, CHAR_LOOPINTERVAL, FMWARPMAN_FREE_LOOPTIME);
		}
	}
	if (CHAR_getWorkInt(meindex, NPC_WORK_MODEFLAG) == NPC_STATEFREE)
	{
		// 取得目前系统时间，若目前时间 >= 准备时间则设定ＢＵＳＹ状态
		
		if (tm1.tm_min > CHAR_getWorkInt(meindex, NPC_WORK_TALKFLAG)
			&& tm1.tm_min == 0)
		{
			// 通知成员进场
			if (fmpks[fmpks_pos].flag == FMPKS_FLAG_SCHEDULED)
				NPC_talkToFloor(CHAR_getInt(meindex, CHAR_FLOOR),
				fmpks[fmpks_pos].host_index,
				fmpks[fmpks_pos].guest_index, "可以准备进场了！");
			CHAR_setWorkInt(meindex, NPC_WORK_TALKFLAG, tm1.tm_min);
		}
		if ((fmpks[fmpks_pos].prepare_time > 0) && (fmpks[fmpks_pos].flag == FMPKS_FLAG_SCHEDULED))
		{
			int clock = 0;
			
			NPC_FMBATTLESET(floor, fmpks[fmpks_pos].host_index,
				fmpks[fmpks_pos].guest_index, -1);
			if (tm1.tm_hour - (fmpks[fmpks_pos].dueltime / 100) < 0)
				clock = (fmpks[fmpks_pos].dueltime % 100) + fmpks[fmpks_pos].prepare_time - 60;
			else
				clock = fmpks[fmpks_pos].dueltime - (tm1.tm_hour * 100) + fmpks[fmpks_pos].prepare_time;
			
			if ((tm1.tm_min >= clock) && (fmpks[fmpks_pos].prepare_time > 0))
			{
				
				NPC_talkToFloor(floor, fmpks[fmpks_pos].host_index,
					fmpks[fmpks_pos].guest_index, "开战罗～！！");
				CHAR_setWorkInt(meindex, NPC_WORK_MODEFLAG, NPC_STATEBUSY);
				CHAR_setInt(meindex, CHAR_LOOPINTERVAL, FMWARPMAN_BUSY_LOOPTIME);
				NPC_FMBATTLESET(floor, fmpks[fmpks_pos].host_index,
					fmpks[fmpks_pos].guest_index, 1);
				
//				print("\n won ==> set fmwaperman state busy !!");
			}
			else if (tm1.tm_min > CHAR_getWorkInt(meindex, NPC_WORK_TIMEFLAG))
			{
				char buf[256];
				int clock = 0;
				CHAR_setWorkInt(meindex, NPC_WORK_TIMEFLAG, tm1.tm_min);
				clock = ((fmpks[fmpks_pos].dueltime % 100) + fmpks[fmpks_pos].prepare_time) - tm1.tm_min;
				if (clock >= 60)	clock = clock - 60;
				sprintf(buf, "距离对战时间还剩下：%4d分钟", clock);
				NPC_talkToFloor(floor, fmpks[fmpks_pos].host_index,
					fmpks[fmpks_pos].guest_index, buf);
			}
		}
	}else if (CHAR_getWorkInt(meindex, NPC_WORK_MODEFLAG) == NPC_STATEBUSY){
		// 检查场内人数、判断胜负
		int num1 = 0, num2 = 0;
		int winflag = 0;
		int floor = CHAR_getWorkInt(meindex, NPC_WORK_WARPFLOOR);
		int meid = CHAR_getWorkInt(meindex, NPC_WORK_ID);
		
//		print("\n won ==> check_winner : npc_meid(%d)", meid );
		
		NPC_GetPKFMNum(floor, fmpks[fmpks_pos].host_index,
			fmpks[fmpks_pos].guest_index, &num1, &num2);
		NPC_FMBATTLESET(floor, fmpks[fmpks_pos].host_index,
			fmpks[fmpks_pos].guest_index, 1);
		CheckLeavePK(meindex, floor, fmpks[fmpks_pos].host_index,
			fmpks[fmpks_pos].guest_index);
		
		if (meid > MANORNUM){
			if (tm1.tm_min == FMWARPMAN_CLEANPLACE){
				// 判断胜负、WARP离场、设定ＮＰＣ状态
				NPC_BattleOut(fmpks[fmpks_pos].host_index, fmpks[fmpks_pos].guest_index);
				NPC_FMBATTLESET(floor, fmpks[fmpks_pos].host_index,
					fmpks[fmpks_pos].guest_index, -1);
				if (fmpks[fmpks_pos].win == 0){// 生存战
					if (num1 > num2)
						winflag = 1;
					else if(num1 < num2)
						winflag = 2;
					else
						winflag = 3;
				}else if (fmpks[fmpks_pos].win == 1){ // 打飞战
					if ((CHAR_getWorkInt(meindex, NPC_WORK_FMNUMI) - num1) < (CHAR_getWorkInt(meindex, NPC_WORK_FMNUMII) - num2))
						winflag = 1;
					else if ((CHAR_getWorkInt(meindex, NPC_WORK_FMNUMI) - num1) > (CHAR_getWorkInt(meindex, NPC_WORK_FMNUMII) - num2))
						winflag = 2;
					else
						winflag = 3;
				}
			}
		}
		if ((num1 == 0) && (num2 != 0)){
			winflag = 2;
		}else if ((num2 == 0) && (num1 != 0)){
			winflag = 1;
		}else if ((num1 == 0) && (num2 == 0)){
			winflag = 3;
		}      
		if (winflag > 0 && winflag < 3){
#ifdef _MANOR_PKRULE      
			if(meid > MANORNUM){
#endif
				// 流浪家族pk 过声望
				if (winflag == 1){	 		 
					saacproto_ACFixFMPK_send(acfd,
						fmpks[fmpks_pos].host_name,
						fmpks[fmpks_pos].host_index + 1,
						fmpks[fmpks_pos].host_index,
						fmpks[fmpks_pos].guest_name,
						fmpks[fmpks_pos].guest_index + 1,
						fmpks[fmpks_pos].guest_index);
				} else {			 
					saacproto_ACFixFMPK_send(acfd,
						fmpks[fmpks_pos].guest_name,
						fmpks[fmpks_pos].guest_index + 1,
						fmpks[fmpks_pos].guest_index,
						fmpks[fmpks_pos].host_name,
						fmpks[fmpks_pos].host_index + 1,
						fmpks[fmpks_pos].host_index);
				}  
#ifdef _MANOR_PKRULE      
			}
#endif
			
			{
				if (meid > 0 && meid <= MANORNUM){// CoolFish 2002/2/25 Change MANOR -> MANORNUM
					fmpks[fmpks_pos + 1].flag = FMPKS_FLAG_MANOR_BATTLEEND;
					if (winflag == 1){
						char token[256];
						sprintf( token, " (%d:%d) %d/%d/%d",
							tm1.tm_hour, tm1.tm_min,
							tm1.tm_year+1900, tm1.tm_mon+1, tm1.tm_mday);
						saacproto_ACFixFMPoint_send(acfd, 
							fmpks[fmpks_pos].host_name,
							fmpks[fmpks_pos].host_index + 1,
							fmpks[fmpks_pos].host_index,
							fmpks[fmpks_pos].guest_name,
							fmpks[fmpks_pos].guest_index + 1,
							fmpks[fmpks_pos].guest_index, meid);
						//Syu 增加庄园战胜负Log
						Logfmpk(
							fmpks[fmpks_pos].host_name,
							fmpks[fmpks_pos].host_index,
							num1,
							fmpks[fmpks_pos].guest_name,
							fmpks[fmpks_pos].guest_index,
							num2, token, "", "", 2);
					} 
					else if (winflag == 2){	
						char token[256];
						sprintf( token, " (%d:%d) %d/%d/%d",
							tm1.tm_hour, tm1.tm_min,
							tm1.tm_year+1900, tm1.tm_mon+1, tm1.tm_mday);
#ifdef _FM_POINT_PK
						int fmid;
						char fmindex[4];
						for (fmid=0; fmid<MANORNUM; fmid++){	// 10个庄园
				       getStringFromIndexWithDelim(fmpointlist.pointlistarray[fmid], "|", 5, fmindex, sizeof(fmindex));
				       if (fmpks[fmpks_pos].guest_index==atoi(fmindex)-1)
				       	 break;
				    }
#endif
						saacproto_ACFixFMPoint_send(acfd, 
							fmpks[fmpks_pos].guest_name,
							fmpks[fmpks_pos].guest_index + 1,
							fmpks[fmpks_pos].guest_index,
							fmpks[fmpks_pos].host_name,
							fmpks[fmpks_pos].host_index + 1,
							fmpks[fmpks_pos].host_index, meid);
						//Syu 增加庄园战胜负Log
						Logfmpk(
							fmpks[fmpks_pos].guest_name,
							fmpks[fmpks_pos].guest_index,
							num2,
							fmpks[fmpks_pos].host_name,
							fmpks[fmpks_pos].host_index,
							num1, token, "", "", 2);
#ifdef _FM_POINT_PK
						if(fmid>=0 && fmid<MANORNUM){
							saacproto_ACFixFMPoint_send(acfd, 
							fmpks[fmpks_pos].host_name,
							fmpks[fmpks_pos].host_index + 1,
							fmpks[fmpks_pos].host_index,
							fmpks[fmpks_pos].host_name,
							fmpks[fmpks_pos].host_index + 1,
							fmpks[fmpks_pos].host_index, fmid+1);
						}
#endif
					}
					CHAR_setWorkInt(meindex, NPC_WORK_MODEFLAG, NPC_STATEFREE);
					CHAR_setInt(meindex, CHAR_LOOPINTERVAL, FMWARPMAN_FREE_LOOPTIME);
					print("FMWarpMan State:%d\n", CHAR_getWorkInt(meindex, NPC_WORK_MODEFLAG));
				}
			}
			if (winflag == 1)
			{
				CHAR_setWorkChar(meindex, NPC_WORK_WINFMNAME, fmpks[fmpks_pos].host_name);
			}
			else if (winflag == 2)
			{
				CHAR_setWorkChar(meindex, NPC_WORK_WINFMNAME, fmpks[fmpks_pos].guest_name);
			}
			NPC_WarpFamily(floor, fmpks[fmpks_pos].host_index,
				fmpks[fmpks_pos].guest_index,
				CHAR_getInt(meindex, CHAR_FLOOR),
				CHAR_getInt(meindex, CHAR_X),
				CHAR_getInt(meindex, CHAR_Y));
			if (meid > MANORNUM){// CoolFish 2002/2/25 Change MANOR -> MANORNUM
				CHAR_setWorkInt(meindex, NPC_WORK_MODEFLAG, NPC_STATEWAIT);
				CHAR_setInt(meindex, CHAR_LOOPINTERVAL, FMWARPMAN_WAIT_LOOPTIME);
			}
      }else if( winflag == 3 ){
				if (meid > 0 && meid <= MANORNUM) // CoolFish 2002/2/25 Change MANOR -> MANORNUM
					
					fmpks[fmpks_pos + 1].flag = FMPKS_FLAG_MANOR_BATTLEEND;
				NPC_talkToFloor(floor, fmpks[fmpks_pos].host_index,
         	fmpks[fmpks_pos].guest_index, "双方平手，不分胜负！！");
				NPC_WarpFamily(floor, fmpks[fmpks_pos].host_index,
         	fmpks[fmpks_pos].guest_index,
         	CHAR_getInt(meindex, CHAR_FLOOR),
         	CHAR_getInt(meindex, CHAR_X),
         	CHAR_getInt(meindex, CHAR_Y));
				CHAR_setWorkInt(meindex, NPC_WORK_MODEFLAG, NPC_STATEWAIT);
				CHAR_setInt(meindex, CHAR_LOOPINTERVAL, FMWARPMAN_WAIT_LOOPTIME);  
      }
   }else if (CHAR_getWorkInt(meindex, NPC_WORK_MODEFLAG) == NPC_STATEWAIT){
		 
		 //	andy_edit 2002/07/29
		 if (tm1.tm_min == FMWARPMAN_CLEANPLACE)
			 CHAR_setInt(meindex, CHAR_LOOPINTERVAL, FMWARPMAN_FREE_LOOPTIME);
		 
		 NPC_CleanPK(floor, meindex);
		 
		 if (tm1.tm_min == 0){
			 CHAR_setWorkInt(meindex, NPC_WORK_MODEFLAG, NPC_STATEFREE);
			 // shan add
			 NPC_FMBATTLESET(floor, fmpks[fmpks_pos].host_index, fmpks[fmpks_pos].guest_index, -1);
			 CHAR_setInt(meindex, CHAR_LOOPINTERVAL, FMWARPMAN_FREE_LOOPTIME);
			 // 将 fmwarpman 初始化
			 CHAR_setWorkInt(meindex, NPC_WORK_FMNUMI, 0);
			 CHAR_setWorkInt(meindex, NPC_WORK_FMNUMII, 0);
			 CHAR_setWorkInt(meindex, NPC_WORK_TIMEFLAG, 0);
			 CHAR_setWorkInt(meindex, NPC_WORK_TALKFLAG, -1);
			 CHAR_setWorkChar(meindex, NPC_WORK_WINFMNAME, "");
		 }
   }
}

void NPC_FMWarpManWindowTalked( int meindex, int talkerindex, 
		int seqno, int select, char *data)
{
	char npcarg[NPC_UTIL_GETARGSTR_BUFSIZE];
	char buf[1024], buff2[256], tmpbuf[256];
	int fl, x, y, fmpks_pos, fd;

	if( !CHAR_CHECKINDEX( talkerindex) )return;
	fd = getfdFromCharaIndex(talkerindex);
	if (fd == -1)	return;

	if( NPC_Util_CharDistance( talkerindex, meindex ) > 2) {
		return;
	}
	if(select==WINDOW_BUTTONTYPE_OK){
		return;
	}
   	if(NPC_Util_GetArgStr( meindex, npcarg, sizeof(npcarg))==NULL){
		print("GetArgStrErr");
		return ;
	}
	// shan add
	fmpks_pos = CHAR_getWorkInt(meindex, NPC_WORK_ID) * MAX_SCHEDULE;
   
	if (CHAR_getWorkInt(talkerindex, CHAR_WORKFMINDEXI) == fmpks[fmpks_pos].host_index){
		if( NPC_Util_GetStrFromStrWithDelim( npcarg, "WARP1", buf, sizeof( buf)) == NULL ){
			print("FM WARPMAN Can't Read WARP1!\n");
			return;
		}
	}else if (CHAR_getWorkInt(talkerindex, CHAR_WORKFMINDEXI) == fmpks[fmpks_pos].guest_index){
		if( NPC_Util_GetStrFromStrWithDelim( npcarg, "WARP2", buf, sizeof( buf)) == NULL ){
			print("FM WARPMAN Can't Read WARP2!\n");
			return;
		}
	}else{
		CHAR_talkToCli( talkerindex, meindex, "你并非对战双方所属任一家族成员！", CHAR_COLORYELLOW);
		return;
	}
	getStringFromIndexWithDelim(buf,",",1,buff2,sizeof(buff2));
	fl=atoi(buff2);
	getStringFromIndexWithDelim(buf,",",2,buff2,sizeof(buff2));
	x=atoi(buff2);
	getStringFromIndexWithDelim(buf,",",3,buff2,sizeof(buff2));
	y=atoi(buff2);


	switch (seqno){
	case CHAR_WINDOWTYPE_WINDOWWARPMAN_MAIN:
		if(select==WINDOW_BUTTONTYPE_YES){
			if( CHAR_getWorkInt( talkerindex, CHAR_WORKBATTLEMODE) != BATTLE_CHARMODE_NONE) return;
			if(CHAR_getWorkInt(talkerindex, CHAR_WORKPARTYMODE) != CHAR_PARTY_NONE){
				CHAR_talkToCli( talkerindex, meindex, "请解散团队！", CHAR_COLORYELLOW);
				return;
			}

			if(CHAR_getWorkInt( talkerindex , CHAR_WORKSHOPRELEVANTTRD) <= 0){
				if( CHAR_getWorkInt( talkerindex, CHAR_WORKWARPCHECK ) == TRUE )  {
					if (CHAR_getWorkInt(meindex, NPC_WORK_MODEFLAG) == NPC_STATEFREE){
         		if (CHAR_getWorkInt(talkerindex, CHAR_WORKFMINDEXI) == fmpks[fmpks_pos].host_index){
         			int tmpnum1 = CHAR_getWorkInt(meindex, NPC_WORK_FMNUMI) + 1;
	         		CHAR_setWorkInt(meindex, NPC_WORK_FMNUMI, tmpnum1);
		       	}else if (CHAR_getWorkInt(talkerindex, CHAR_WORKFMINDEXI) == fmpks[fmpks_pos].guest_index){
			     		int tmpnum2 = CHAR_getWorkInt(meindex, NPC_WORK_FMNUMII) + 1;
				   		CHAR_setWorkInt(meindex, NPC_WORK_FMNUMII, tmpnum2);
						}else{
							CHAR_talkToCli( talkerindex, meindex, "你并非对战双方所属任一家族成员！", CHAR_COLORYELLOW);
						}
	         	CHAR_setWorkInt(talkerindex, CHAR_WORKFMPKFLAG, 1);
		       	CHAR_setWorkInt(talkerindex, CHAR_WORKFMMANINDEX, meindex);
			     	CHAR_setWorkInt( talkerindex, CHAR_WORKWARPCHECK, FALSE );
				   	CHAR_warpToSpecificPoint(talkerindex, fl, x,y);
/*						{
							FILE *fp;
							struct tm tm1;
							char szFileName[128];

							memcpy(&tm1, localtime((time_t *)&NowTime.tv_sec), sizeof(tm1));
							memset(szFileName,0,sizeof(szFileName));
							sprintf(szFileName,"FMPkWarp.%d%d%d.log",tm1.tm_mon,tm1.tm_hour,tm1.tm_min);
							fp = fopen(szFileName,"a+");
							if(fp != NULL){
								fprintf(fp,"FMName:%s\tName:%s\tWarpHost:%d\tWarpGuest:%d\tFloor:%d,%d,%d\tMaxPlayer:%d\n",
														CHAR_getChar(talkerindex,CHAR_FMNAME),CHAR_getChar(talkerindex,CHAR_NAME),
														CHAR_getWorkInt(meindex,NPC_WORK_FMNUMI),
														CHAR_getWorkInt(meindex,NPC_WORK_FMNUMII),
														fl,x,y,
														fmpks[fmpks_pos].max_player);
								fclose(fp);
							}
							else printf("%s(errno:%x)\n",sys_errlist[errno],errno);
						}
*/
					}
					else return;
					{
						struct tm tm1;
						int clock = 0;
						int fmpks_pos = CHAR_getWorkInt(meindex, NPC_WORK_ID) * MAX_SCHEDULE;
						memcpy(&tm1, localtime((time_t *)&NowTime.tv_sec), sizeof(tm1));
						clock = ((fmpks[fmpks_pos].dueltime % 100) + fmpks[fmpks_pos].prepare_time) - tm1.tm_min;
						if (clock >= 60)	clock = clock - 60;
						sprintf(tmpbuf, "\n请等候双方准备完毕後再开战！"
							"\n战斗时请不要随意登出或离开。"
							"\n赢得胜利的家族也请先稍後一下，"
							"\n等待裁判的宣布後再离场！"
							"\n谢谢您的合作！"
							"\n距离对战时间还剩下：%4d分钟",
							clock);
						lssproto_WN_send(fd, 
			         	WINDOW_MESSAGETYPE_MESSAGE, WINDOW_BUTTONTYPE_OK,
								-1, -1,
								makeEscapeString(tmpbuf, buff2, sizeof(buff2)));
					}
				}
			}
			else{
				if(CHAR_getInt(talkerindex,CHAR_GOLD) < CHAR_getWorkInt(talkerindex,CHAR_WORKSHOPRELEVANTTRD)){
					NPC_ERR_FMDiSP( meindex, talkerindex, 2);
					return ;
				}
				CHAR_DelGold( talkerindex, CHAR_getWorkInt(talkerindex,CHAR_WORKSHOPRELEVANTTRD ));
				CHAR_setWorkInt( talkerindex, CHAR_WORKWARPCHECK, FALSE );
				CHAR_warpToSpecificPoint(talkerindex, fl, x,y);
			}
		}
		break;
	}
}

void NPC_ERR_FMDiSP(int meindex,int talker,int errNO)
{
	
	char token[1024];
	int i=0;
	int otherindex;
	int fd = getfdFromCharaIndex( talker);
	char npcarg[NPC_UTIL_GETARGSTR_BUFSIZE];

	if(NPC_Util_GetArgStr( meindex, npcarg, sizeof(npcarg))==NULL){
		print("GetArgStrErr");
		return ;
	}
	
	if(errNO==1){
		/*--由□  奴□匹  仄井仃凶--*/
		if(NPC_Util_GetStrFromStrWithDelim( npcarg, "PartyMsg",token, sizeof( token))==NULL) {
			/*--涩烂今木化卅仃木壬｝票及丢永本□斥毛银丹--*/
			sprintf(token, "\n\n无法加入团队，请先解散团队！");
		}

		if(CHAR_getWorkInt(talker,CHAR_WORKPARTYMODE)==CHAR_PARTY_CLIENT){
					
		}else{
			
		/*--伉□母□互  仄井仃凶桦宁及心--*/
		/*--由□  奴□蝈够卞丢永本□斥毛请允--*/		
		for( i=0 ; i < CHAR_PARTYMAX ;i++)
		{
			otherindex=CHAR_getWorkInt(talker,CHAR_WORKPARTYINDEX1+i);

			if(otherindex != -1){
				 fd = getfdFromCharaIndex( otherindex);

				/*-仇仇匹霜耨允月--*/
				lssproto_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,
							WINDOW_BUTTONTYPE_OK,
							CHAR_WINDOWTYPE_WINDOWWARPMAN_ERR, 
							CHAR_getWorkInt( meindex, CHAR_WORKOBJINDEX),
							token);
				}
			}
			return ;
		}
	
	}else if (errNO==2){
		/*--嗯互凶曰卅中匹允［--*/
		if(NPC_Util_GetStrFromStrWithDelim( npcarg, "MoneyMsg", token, sizeof( token))==NULL){
			/*--云嗯互箫曰卅中及卞｝涩烂今木化卅井匀凶日｝票及丢永本□斥毛  歹曰卞请允--*/
			sprintf(token,"\n\n金钱似乎不足唷，请存好钱後再过来。");
			}
		}

		/*-仇仇匹霜耨允月--*/
		lssproto_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,
						WINDOW_BUTTONTYPE_OK,
						CHAR_WINDOWTYPE_WINDOWWARPMAN_ERR, 
						CHAR_getWorkInt( meindex, CHAR_WORKOBJINDEX),
						token);
}

static void NPC_FMWarpMan_selectWindow( int meindex, int toindex, int num,int select)
{
	struct tm tm1;
	char npcarg[NPC_UTIL_GETARGSTR_BUFSIZE];
	char token[1024];
	char buf[256];
	char buf2[1024];
	char buf3[256];
	int fl = 0, num1 = 0, num2 = 0;
	int fmpks_pos = CHAR_getWorkInt(meindex, NPC_WORK_ID) * MAX_SCHEDULE;
	int floor = CHAR_getWorkInt(meindex, NPC_WORK_WARPFLOOR);
	int fd = getfdFromCharaIndex( toindex);	
	if( fd == -1 ) {
		fprint( "getfd err\n");
		return;
	}
	if(NPC_Util_GetArgStr( meindex, npcarg, sizeof(npcarg))==NULL){
		print("GetArgStrErr");
		return ;
	}
	/*--涩烂白央奶伙及  卞白夫失谛醒互隙烂今木化中月井今木化中木壬白夫失谛醒及喃曰请仄*/
	if(strstr(npcarg,"%4d")!=NULL){
		NPC_Util_GetStrFromStrWithDelim( npcarg, "WARP1", buf, sizeof( buf));
		getStringFromIndexWithDelim(buf,",",1,buf3,sizeof(buf3));
		fl = NPC_FMFloorUse(atoi(buf3));
	}
	CHAR_setWorkInt(toindex, CHAR_WORKWARPCHECK, TRUE);
	CHAR_setWorkInt(toindex, NPC_WORK_WARPFLOOR, fl);
	memcpy(&tm1, localtime((time_t *)&NowTime.tv_sec), sizeof(tm1));
	
	if (CHAR_getWorkInt(meindex, NPC_WORK_MODEFLAG) == NPC_STATEFREE)
	{
		if (((CHAR_getWorkInt(toindex, CHAR_WORKFMINDEXI) == fmpks[fmpks_pos].host_index)
			&& (strcmp(CHAR_getChar(toindex, CHAR_FMNAME), fmpks[fmpks_pos].host_name) == 0))
			|| ((CHAR_getWorkInt(toindex, CHAR_WORKFMINDEXI) == fmpks[fmpks_pos].guest_index)
			&& (strcmp(CHAR_getChar(toindex, CHAR_FMNAME), fmpks[fmpks_pos].guest_name) == 0)))
		{
			if ((fmpks[fmpks_pos].host_index != -1)
		      && (CHAR_getWorkInt(toindex, CHAR_WORKFMINDEXI) == fmpks[fmpks_pos].host_index))
			{
				// WON ADD 修正家族pk场的约战问题
				
				if(fmpks[fmpks_pos].flag == -1)		return;
				
				if (fmpks[fmpks_pos].flag != FMPKS_FLAG_SCHEDULED)
					
				{
					lssproto_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,
						WINDOW_BUTTONTYPE_OK, -1, -1,
						makeEscapeString("\n这场战斗对方好像还没同意呢！\n下次请记得先邀约对方後再来登记吧～", token, sizeof(token)));
					return;
				}
#ifdef _FMVER21		   	
				if (CHAR_getInt(toindex, CHAR_FMLEADERFLAG) == FMMEMBER_APPLY)
#else
				if (CHAR_getInt(toindex, CHAR_FMLEADERFLAG) == 0)
#endif		   	
				{	
					char	tmpbuf[256];
					sprintf(tmpbuf, "\n你还没有正式加入%s家族，所以不能进场！", fmpks[fmpks_pos].host_name);
					lssproto_WN_send(fd, WINDOW_MESSAGETYPE_MESSAGE,
						WINDOW_BUTTONTYPE_OK, -1, -1,
						makeEscapeString(tmpbuf, token, sizeof(token)));
					return;
				}
				//andy_add 2003/06/17
				CHECK_FMPknumInFloor( meindex);
				if (CHAR_getWorkInt(meindex, NPC_WORK_FMNUMI) > (fmpks[fmpks_pos].max_player - 1)){
					lssproto_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,
						WINDOW_BUTTONTYPE_OK, -1, -1, makeEscapeString("\n已经不能再进去罗～！\n家族进场人数已经到达设定上限了！", token, sizeof(token)));
					return;
				}
			}
			else if ((fmpks[fmpks_pos].guest_index != -1)
		      && (CHAR_getWorkInt(toindex, CHAR_WORKFMINDEXI) == fmpks[fmpks_pos].guest_index))
			{
				// WON ADD 修正家族pk场的约战问题
				
				if(fmpks[fmpks_pos].flag == -1)		return;
				
				if (fmpks[fmpks_pos].flag != FMPKS_FLAG_SCHEDULED)
					
				{
					lssproto_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,
						WINDOW_BUTTONTYPE_OK, -1, -1,
						makeEscapeString("\n这场战斗你好像还没同意呢！\n下次请记得提早来跟我确认～", token, sizeof(token)));
					return;
				}
#ifdef _FMVER21		   	
				if (CHAR_getInt(toindex, CHAR_FMLEADERFLAG) == FMMEMBER_APPLY)
#else
					if (CHAR_getInt(toindex, CHAR_FMLEADERFLAG) == 0)
#endif		   	
				{
					char	tmpbuf[256];
					sprintf(tmpbuf, "\n你还没有正式加入%s家族，所以不能进场！", fmpks[fmpks_pos].guest_name);
					lssproto_WN_send(fd, WINDOW_MESSAGETYPE_MESSAGE,
						WINDOW_BUTTONTYPE_OK, -1, -1,
						makeEscapeString(tmpbuf, token, sizeof(token)));
					return;
				}
				//andy_add 2003/06/17
				CHECK_FMPknumInFloor( meindex);
				if( CHAR_getWorkInt( meindex, NPC_WORK_FMNUMII) > (fmpks[fmpks_pos].max_player - 1)){
					lssproto_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,
						WINDOW_BUTTONTYPE_OK, -1, -1,
						makeEscapeString("已经不能再进去罗～！\n家族进场人数已经到达设定上限了！", token, sizeof(token)));
					return;
				}
			}
			if(NPC_Util_GetStrFromStrWithDelim( npcarg, "FreeMsg", buf2, sizeof( buf2)) == NULL) return;
			CONNECT_set_pass(fd, TRUE);
			CONNECT_set_first_warp(fd, TRUE);
			sprintf(token, buf2, fmpks[fmpks_pos].guest_name, fmpks[fmpks_pos].host_name,
				tm1.tm_hour, tm1.tm_min);
			CHAR_setWorkInt( toindex , CHAR_WORKFMMANINDEX, meindex );
			CHAR_setWorkInt( toindex , CHAR_WORKSHOPRELEVANTTRD , -1 );
		} else if ((((CHAR_getWorkInt(toindex, CHAR_WORKFMINDEXI) == fmpks[fmpks_pos + 1].host_index)
			&& (strcmp(CHAR_getChar(toindex, CHAR_FMNAME), fmpks[fmpks_pos + 1].host_name) == 0))
			|| ((CHAR_getWorkInt(toindex, CHAR_WORKFMINDEXI) == fmpks[fmpks_pos + 1].guest_index)
			&& (strcmp(CHAR_getChar(toindex, CHAR_FMNAME), fmpks[fmpks_pos + 1].guest_name) == 0)))
			&& (((fmpks[fmpks_pos + 1].flag == FMPKS_FLAG_SCHEDULED)
			   || (fmpks[fmpks_pos + 1].flag == FMPKS_FLAG_MANOR_PREPARE)))
				 )
		{
			if(NPC_Util_GetStrFromStrWithDelim( npcarg, "NextMsg", buf2, sizeof( buf2)) == NULL) return;
			sprintf(token, buf2, fl, tm1.tm_hour, tm1.tm_min);
			CONNECT_set_pass(fd, FALSE);
			CONNECT_set_first_warp(fd, FALSE);
		}
		else if ((strcmp(fmpks[fmpks_pos].host_name, "") ==  0) || (strcmp(fmpks[fmpks_pos].guest_name, "") == 0))
		{
			if(NPC_Util_GetStrFromStrWithDelim( npcarg, "TalkMsg", buf2, sizeof( buf2)) == NULL) return;
			sprintf(token, buf2);
			CONNECT_set_pass(fd, FALSE);
			CONNECT_set_first_warp(fd, FALSE);
		}
		else
		{
			if(NPC_Util_GetStrFromStrWithDelim( npcarg, "AskMsg", buf2, sizeof( buf2)) == NULL) return;
			CONNECT_set_pass(fd, FALSE);
			CONNECT_set_first_warp(fd, FALSE);
			sprintf(token, buf2, fmpks[fmpks_pos].guest_name, fmpks[fmpks_pos].host_name);
		}
	}
	else if (CHAR_getWorkInt(meindex, NPC_WORK_MODEFLAG) == NPC_STATEBUSY) // 两家族战斗中
	{
		if (((CHAR_getWorkInt(toindex, CHAR_WORKFMINDEXI) == fmpks[fmpks_pos].host_index)
			&& (strcmp(CHAR_getChar(toindex, CHAR_FMNAME), fmpks[fmpks_pos].host_name) == 0))
			|| ((CHAR_getWorkInt(toindex, CHAR_WORKFMINDEXI) == fmpks[fmpks_pos].guest_index)
			&& (strcmp(CHAR_getChar(toindex, CHAR_FMNAME), fmpks[fmpks_pos].guest_name) == 0)))
		{
			if(NPC_Util_GetStrFromStrWithDelim( npcarg, "BusyMsg", buf2, sizeof( buf2)) == NULL){
				print("\nGet BusyMsg Message Error");
				return;
			}
			NPC_GetPKFMNum(floor,
				fmpks[fmpks_pos].host_index,
				fmpks[fmpks_pos].guest_index,
				&num1, &num2);
			sprintf(token, buf2, 
				fmpks[fmpks_pos].host_name, num1,
				fmpks[fmpks_pos].guest_name, num2,
				tm1.tm_hour, tm1.tm_min);
		} else if ((((CHAR_getWorkInt(toindex, CHAR_WORKFMINDEXI) == fmpks[fmpks_pos + 1].host_index)
			&& (strcmp(CHAR_getChar(toindex, CHAR_FMNAME), fmpks[fmpks_pos + 1].host_name) == 0))
			|| ((CHAR_getWorkInt(toindex, CHAR_WORKFMINDEXI) == fmpks[fmpks_pos + 1].guest_index)
			&& (strcmp(CHAR_getChar(toindex, CHAR_FMNAME), fmpks[fmpks_pos + 1].guest_name) == 0)))
			&& (((fmpks[fmpks_pos + 1].flag == FMPKS_FLAG_SCHEDULED)
			   || (fmpks[fmpks_pos + 1].flag == FMPKS_FLAG_MANOR_PREPARE)))
				 )
		{
			if(NPC_Util_GetStrFromStrWithDelim( npcarg, "NextMsg", buf2, sizeof( buf2)) == NULL) return;
			sprintf(token, buf2, fl, tm1.tm_hour, tm1.tm_min);
		} else {
			if(NPC_Util_GetStrFromStrWithDelim( npcarg, "AskMsg", buf2, sizeof( buf2)) == NULL) return;
			sprintf(token, buf2, fmpks[fmpks_pos].guest_name, fmpks[fmpks_pos].host_name);
		}
		CONNECT_set_pass(fd, FALSE);
		CONNECT_set_first_warp(fd, FALSE);
	}
	else if (CHAR_getWorkInt(meindex, NPC_WORK_MODEFLAG) == NPC_STATEWAIT) // 两家族战斗结束
	{
		if (((CHAR_getWorkInt(toindex, CHAR_WORKFMINDEXI) == fmpks[fmpks_pos].host_index)
			&& (strcmp(CHAR_getChar(toindex, CHAR_FMNAME), fmpks[fmpks_pos].host_name) == 0))
			|| ((CHAR_getWorkInt(toindex, CHAR_WORKFMINDEXI) == fmpks[fmpks_pos].guest_index)
			&& (strcmp(CHAR_getChar(toindex, CHAR_FMNAME), fmpks[fmpks_pos].guest_name) == 0)))
		{
			char tmpbuf[256];
			if(NPC_Util_GetStrFromStrWithDelim( npcarg, "EndMsg", buf2, sizeof( buf2)) == NULL){
				print("\nGet EndMsg Message Error");
				return;
			}
			NPC_GetPKFMNum(floor,
				fmpks[fmpks_pos].host_index,
				fmpks[fmpks_pos].guest_index,
				&num1, &num2);
			if (strcmp(CHAR_getWorkChar(meindex, NPC_WORK_WINFMNAME), "") == 0)
				sprintf(tmpbuf, "双方平手！");
			else sprintf(tmpbuf, "%s获胜了！！", CHAR_getWorkChar(meindex, NPC_WORK_WINFMNAME));
			sprintf(token, buf2,  fmpks[fmpks_pos].guest_name,
				fmpks[fmpks_pos].host_name, tmpbuf);
		} else if ((((CHAR_getWorkInt(toindex, CHAR_WORKFMINDEXI) == fmpks[fmpks_pos + 1].host_index)
			&& (strcmp(CHAR_getChar(toindex, CHAR_FMNAME), fmpks[fmpks_pos + 1].host_name) == 0))
			|| ((CHAR_getWorkInt(toindex, CHAR_WORKFMINDEXI) == fmpks[fmpks_pos + 1].guest_index)
			&& (strcmp(CHAR_getChar(toindex, CHAR_FMNAME), fmpks[fmpks_pos + 1].guest_name) == 0)))
			&& (((fmpks[fmpks_pos + 1].flag == FMPKS_FLAG_SCHEDULED)
			   || (fmpks[fmpks_pos + 1].flag == FMPKS_FLAG_MANOR_PREPARE)))
				 )
		{
			if(NPC_Util_GetStrFromStrWithDelim( npcarg, "NextMsg", buf2, sizeof( buf2)) == NULL) return;
			sprintf(token, buf2, fl, tm1.tm_hour, tm1.tm_min);
		} else {
			if(NPC_Util_GetStrFromStrWithDelim( npcarg, "AskMsg", buf2, sizeof( buf2)) == NULL) return;
			sprintf(token, buf2, fmpks[fmpks_pos].guest_name, fmpks[fmpks_pos].host_name);
		}
		CONNECT_set_pass(fd, FALSE);
		CONNECT_set_first_warp(fd, FALSE);
	}
	/*-仇仇匹霜耨允月--*/
	lssproto_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,
		WINDOW_BUTTONTYPE_YESNO,
		CHAR_WINDOWTYPE_WINDOWWARPMAN_MAIN, 
		CHAR_getWorkInt( meindex, CHAR_WORKOBJINDEX),
		token);
	
}

int NPC_FMFloorUse(int floor)
{
	int	i;
	int	players = 0;
	int     playernum = CHAR_getPlayerMaxNum();

	/* 皿伊奶乩□民尼永弁 */
	for( i=0 ; i< playernum ; i++ ){
		if( CHAR_getCharUse(i) == FALSE )continue;
			if(CHAR_getInt(i,CHAR_FLOOR)==floor){
				players++;
			}
	}
	return players;
}

void NPC_GetPKFMNum(int floor, int index1, int index2, int *num1, int *num2)
{
	int	i = 0, charindex;

	*num1 = 0; *num2 = 0;

	/* 皿伊奶乩□民尼永弁 */
	for (i = 0 ; i < FAMILY_MAXMEMBER; i++ ){
		charindex = familyMemberIndex[ index1][i];
		if( CHAR_getCharUse( charindex) ){
			if (CHAR_getInt(charindex, CHAR_FLOOR) == floor)
				*num1 = *num1 + 1;
		}else
			familyMemberIndex[ index1][i] = -1;

		charindex = familyMemberIndex[index2][i];
		if (CHAR_getCharUse(charindex)){
			if (CHAR_getInt(charindex, CHAR_FLOOR) == floor)
				*num2 = *num2 + 1;
		}else
			familyMemberIndex[ index2][i] = -1;
	}

}

void NPC_FMBATTLESET(int floor, int index1, int index2, int flag)
{
	int	i = 0, charindex;

	for (i = 0; i < FAMILY_MAXMEMBER; i++)
	{
		charindex = familyMemberIndex[index1][i];
		if (CHAR_getCharUse(charindex))
		{
			if (CHAR_getInt(charindex ,CHAR_FLOOR) == floor)
			{
			        CHAR_setWorkInt( charindex, CHAR_WORKBATTLEFLAG, flag);
			        if(flag==1)
    				    CHAR_setFlg(charindex, CHAR_ISDUEL, 1);
		        }
		}
		else
			familyMemberIndex[index1][i] = -1;
		charindex = familyMemberIndex[index2][i];
		if (CHAR_getCharUse(charindex))
		{
			if (CHAR_getInt(charindex ,CHAR_FLOOR) == floor)
			{
			        CHAR_setWorkInt( charindex, CHAR_WORKBATTLEFLAG, flag);
			        if(flag==1)
			            CHAR_setFlg(charindex, CHAR_ISDUEL, 1);
			}
		}
		else
			familyMemberIndex[index2][i] = -1;
	}
}

void NPC_CleanPK(int floor, int meindex)
{
	int	i;
	int     playernum = CHAR_getPlayerMaxNum();
	int	fl = CHAR_getInt(meindex, CHAR_FLOOR);
	int	x = CHAR_getInt(meindex, CHAR_X);
	int	y = CHAR_getInt(meindex, CHAR_Y);

	for( i=0 ; i< playernum ; i++ )
	{
	   if( CHAR_getCharUse(i) == FALSE )continue;
	   	if(CHAR_getInt(i,CHAR_FLOOR)==floor)
	   		CHAR_warpToSpecificPoint(i, fl, x, y);
	}
}

void NPC_talkToFloor(int floor, int index1, int index2, char *data)
{
	int 	i = 0, charindex;

	for (i = 0; i < FAMILY_MAXMEMBER; i++)
	{
		charindex = familyMemberIndex[index1][i];
		if (CHAR_getCharUse(charindex))
		{
//			print("charname:%s fmname:%s\n", 
//				CHAR_getChar(charindex, CHAR_NAME),
//				CHAR_getChar(charindex, CHAR_FMNAME));
			if (CHAR_getInt(charindex, CHAR_FLOOR) == floor)
				CHAR_talkToCli(charindex, -1, data, CHAR_COLORYELLOW);
		}
		else
			familyMemberIndex[index1][i] = -1;
		charindex = familyMemberIndex[index2][i];
		if (CHAR_getCharUse(charindex))
		{
//			print("charname:%s fmname:%s\n", 
//				CHAR_getChar(i, CHAR_NAME),
//				CHAR_getChar(i, CHAR_FMNAME));
			if (CHAR_getInt(charindex, CHAR_FLOOR) == floor)
				CHAR_talkToCli(charindex, -1, data, CHAR_COLORRED);
		}
		else
			familyMemberIndex[index2][i] = -1;
	}
}

void NPC_WarpFamily(int floor, int index1, int index2, int fl, int x, int y)
{
        int     i, charindex1, charindex2;

        for (i = 0; i < FAMILY_MAXMEMBER; i++){
                charindex1 = familyMemberIndex[index1][i];
                charindex2 = familyMemberIndex[index2][i];
                if (CHAR_getCharUse(charindex1))
                {
                   if (CHAR_getInt(charindex1, CHAR_FLOOR) == floor)
                   {
                   	CHAR_setWorkInt(charindex1, CHAR_WORKBATTLEFLAG, FALSE);
                   	CHAR_warpToSpecificPoint(charindex1, fl, x, y);
                   }
                }
                else
                         familyMemberIndex[index1][i] = -1;
                if (CHAR_getCharUse(charindex2))
                {
                   if (CHAR_getInt(charindex2, CHAR_FLOOR) == floor)
                   {
                   	CHAR_setWorkInt(charindex2, CHAR_WORKBATTLEFLAG, FALSE);
                   	CHAR_warpToSpecificPoint(charindex2, fl, x, y);
                   }
                }
                else
                         familyMemberIndex[index1][i] = -1;
        }
}

void NPC_BattleOut(int index1, int index2)
{
        int i, charindex1, charindex2;

        for (i = 0; i < FAMILY_MAXMEMBER; i++)
        {
                charindex1 = familyMemberIndex[index1][i];
                charindex2 = familyMemberIndex[index2][i];
                if(CHAR_getCharUse(charindex1)){
                         if(CHAR_getWorkInt( charindex1, CHAR_WORKBATTLEMODE) != BATTLE_CHARMODE_NONE)
                                 BATTLE_WatchStop(charindex1);
                }
                else
                         familyMemberIndex[index1][i] = -1;

                if(CHAR_getCharUse(charindex2)){
                         if(CHAR_getWorkInt( charindex2, CHAR_WORKBATTLEMODE) != BATTLE_CHARMODE_NONE)
                                 BATTLE_WatchStop(charindex2);
                }
                else
                         familyMemberIndex[index2][i] = -1;
        }   
}

void CheckLeavePK(int npcindex, int floor, int index1, int index2)
{
	int 	i = 0, charindex, fl = 0, x = 0, y = 0;
	fl = CHAR_getInt(npcindex, CHAR_FLOOR);
	x = CHAR_getInt(npcindex, CHAR_X);
	y = CHAR_getInt(npcindex, CHAR_Y);

	for (i = 0; i < FAMILY_MAXMEMBER; i++)
	{
	   charindex = familyMemberIndex[index1][i];
	   if (CHAR_getCharUse(charindex)){
	      if (CHAR_getInt(charindex, CHAR_FLOOR) == floor){
	         if (CHAR_getWorkInt(charindex, CHAR_WORKFMPKFLAG) < 0){
	            if (CHAR_getWorkInt(charindex, CHAR_WORKPARTYMODE) != CHAR_PARTY_NONE)
	            	CHAR_DischargeParty( charindex, 0);
	            CHAR_setWorkInt(charindex, CHAR_WORKWARPCHECK, FALSE);
	            CHAR_setWorkInt(charindex, CHAR_WORKBATTLEFLAG, FALSE);
	            CHAR_warpToSpecificPoint(charindex, fl, x, y);
	            CHAR_talkToCli(charindex, -1, "战败离场", CHAR_COLORRED);
	         }
	      }
	   }else
	      familyMemberIndex[index1][i] = -1;
	   charindex = familyMemberIndex[index2][i];
	   if (CHAR_getCharUse(charindex)){
	      if (CHAR_getInt(charindex, CHAR_FLOOR) == floor){
	         if (CHAR_getWorkInt(charindex, CHAR_WORKFMPKFLAG) < 0){
	            if (CHAR_getWorkInt(charindex, CHAR_WORKPARTYMODE) != CHAR_PARTY_NONE)
	            	CHAR_DischargeParty(charindex, 0);
	            CHAR_setWorkInt(charindex, CHAR_WORKWARPCHECK, FALSE);
	            CHAR_setWorkInt(charindex, CHAR_WORKBATTLEFLAG, FALSE);
	            CHAR_warpToSpecificPoint(charindex, fl, x, y);
	            CHAR_talkToCli(charindex, -1, "战败离场", CHAR_COLORRED);
	         }
	      }
	   }else
	      familyMemberIndex[index2][i] = -1;
	}
}
