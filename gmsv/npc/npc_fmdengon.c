#include "version.h"
#include <time.h>
#include <string.h>
#include "char.h"
#include "object.h"
#include "lssproto_serv.h"
#include "saacproto_cli.h"
#include "npcutil.h"
#include "handletime.h"
#include "npc_fmdengon.h"
#include "family.h"

#define DENGONFILELINENUM      35     // 整个公布栏的资料笔数
#define FMSDENGONFILELINENUM   140    // 家族间的留言板资料笔数
#define DENGONFILEENTRYSIZE    128    // 本文大小
#define MESSAGEINONEWINDOW     7      // 每页所显示的笔数
#define FMMAXNUM               1000   // 家族数量的最大值
#define FM_MEMBERLIST          2      // 家族成员列表    (主功能表的按键)
#define FM_MEMBERMEMO          3      // 家族留言        (主功能表的按键)
#define FM_FMMEMO              4      // 家族之间留言板  (主功能表的按键)
#define FM_FMPOINT             5      // 申请家族据点    (主功能表的按键)
#define FM_FMDPTOP             6      // 家族间强者表    (主功能表的按键)
#define FM_WAITTIME            (3*60)
#define FMSDENGON_SN           10000  // 家族之间的留言板的识别码

extern struct  FMMEMBER_LIST memberlist[FMMAXNUM];         // 接收 AC 成员列表资料的 ARRAY 
extern struct  FMS_MEMO fmsmemo;                           // 家族之间的留言板
extern struct  FM_POINTLIST fmpointlist;                   // 家族据点
extern struct  FMS_DPTOP fmdptop;                          // 家族强者表
extern int leaderdengonindex;                              // 777 家族公布栏 index
char NPC_sendbuf[DENGONFILEENTRYSIZE*MESSAGEINONEWINDOW];  // 一整页的大小
char enlistbuf[4096];                                        // 家族成员召募 BUF(显示用的)

unsigned long READTIME1 = 0,
              READTIME2 = 0,
              READTIME3 = 0,
              READTIME4 = 0;

#ifdef _FIX_FM_FMPOINT
void ApplyFamilyPoint( int meindex, int toindex, int select);
#endif

// 公布栏的初始化(when gmsv start)
BOOL NPC_FmDengonInit( int meindex)
{
    int fmindex=CHAR_getInt(meindex, CHAR_FMINDEX);
    CHAR_setInt( meindex, CHAR_WHICHTYPE, CHAR_TYPEDENGON);
    if(fmindex>0){
	    
	    
//    if( CHAR_getInt(meindex, CHAR_FLOOR) == 777 ){
//        leaderdengonindex = meindex;
//    }
	    
	    if( READTIME1 == 0 || READTIME2 == 0 || READTIME3 == 0 || READTIME4 == 0 ){
	        READTIME1 = NowTime.tv_sec+FM_WAITTIME,
	        READTIME2 = NowTime.tv_sec+FM_WAITTIME,
	        READTIME3 = NowTime.tv_sec+FM_WAITTIME,
	        READTIME4 = NowTime.tv_sec+FM_WAITTIME;
	       
	        // 取得家族的成员列表(memberlist struct)，以及家族的留言板

	        saacproto_ACShowMemberList_send( acfd, fmindex);
	        saacproto_ACFMReadMemo_send( acfd, fmindex);

	        // 家族之间的留言板所传的值预设为 FMSDENGON_SN
	        saacproto_ACFMReadMemo_send( acfd, FMSDENGON_SN);
	        saacproto_ACFMPointList_send(acfd);
	        saacproto_ACShowTopFMList_send(acfd, FM_TOP_INTEGRATE);
	        saacproto_ACShowTopFMList_send(acfd, FM_TOP_ADV);    
	        saacproto_ACShowTopFMList_send(acfd, FM_TOP_FEED);
	        saacproto_ACShowTopFMList_send(acfd, FM_TOP_SYNTHESIZE);
	        saacproto_ACShowTopFMList_send(acfd, FM_TOP_DEALFOOD);
	        saacproto_ACShowTopFMList_send(acfd, FM_TOP_PK);
	    }
	    
	 }
	 return TRUE;
}

// Select Event
void NPC_FmDengonWindowTalked( int index, int talker, int seqno, int select, char *data)
{
//    char buf[DENGONFILEENTRYSIZE*MESSAGEINONEWINDOW*2];
    char buf[4096];
    int  buttonevent;
    int  buttontype = 0;
    struct timeval recvtime;
    if (!CHAR_CHECKINDEX(talker)) return;
    
    CONNECT_getLastrecvtime_D( getfdFromCharaIndex( talker), &recvtime);
    if( time_diff( NowTime, recvtime) < 0.5 ){
        return;
    }
    
    CONNECT_setLastrecvtime_D( getfdFromCharaIndex( talker), &NowTime);
#ifndef _FM_MODIFY
    // 距离超出 DENGONDISTANCE 的  围内时，即取消动作
#define DENGONDISTANCE 3	
    if( CHAR_getInt(index, CHAR_FLOOR) != 777 )
        if(NPC_Util_CharDistance( index, talker) > DENGONDISTANCE) return;
#endif
    
    // 家族留言板
    if( seqno == CHAR_WINDOWTYPE_FM_DENGON)
    {
			int dengonindex;
			int fmindex_wk;
			char tmp_buffer[4096],tmp[4096];
			
			getStringFromIndexWithDelim(data,"|",1,tmp_buffer,sizeof(tmp_buffer));
			dengonindex = atoi(tmp_buffer);
			
			fmindex_wk = CHAR_getWorkInt( talker, CHAR_WORKFMINDEXI);
			
			if( fmindex_wk < 0 || fmindex_wk >= FMMAXNUM) return;
			
			switch( select){
			case WINDOW_BUTTONTYPE_NEXT:
			case WINDOW_BUTTONTYPE_PREV:
				{
					int fd,i;
					fd = getfdFromCharaIndex( talker);
					if( fd == -1) return;
					
					dengonindex += 7 * (( select == WINDOW_BUTTONTYPE_NEXT) ? 1 : -1);
					if( dengonindex > memberlist[fmindex_wk].memoindex && memberlist[fmindex_wk].memonum < DENGONFILELINENUM)
						dengonindex = memberlist[fmindex_wk].memoindex;
					else if( dengonindex < 6 && memberlist[fmindex_wk].memonum < DENGONFILELINENUM) 
						dengonindex = 6;
					else if( dengonindex < 1 && memberlist[fmindex_wk].memonum >= DENGONFILELINENUM)
						dengonindex = memberlist[fmindex_wk].memonum+dengonindex;
					else if( dengonindex > memberlist[fmindex_wk].memonum && memberlist[fmindex_wk].memonum >= DENGONFILELINENUM)
						dengonindex -= memberlist[fmindex_wk].memonum;
					
					buttontype = WINDOW_BUTTONTYPE_OKCANCEL;
					if( dengonindex==memberlist[fmindex_wk].memoindex && memberlist[fmindex_wk].memonum >= DENGONFILELINENUM) 
						buttontype |= WINDOW_BUTTONTYPE_PREV;
					else if( (dengonindex - 7)<=memberlist[fmindex_wk].memoindex && (dengonindex - 7)>=(memberlist[fmindex_wk].memoindex - 7) &&
						memberlist[fmindex_wk].memonum >= DENGONFILELINENUM) 
						buttontype |= WINDOW_BUTTONTYPE_NEXT;
					else if( dengonindex==memberlist[fmindex_wk].memoindex) 
						buttontype |= WINDOW_BUTTONTYPE_PREV;    
					else if( dengonindex == 6) buttontype |= WINDOW_BUTTONTYPE_NEXT;
					else{
						buttontype |= WINDOW_BUTTONTYPE_PREV;
						buttontype |= WINDOW_BUTTONTYPE_NEXT;
					}
					if(dengonindex >= 6){
						strcpy( NPC_sendbuf, memberlist[fmindex_wk].memo[dengonindex - 6]);
						strcat( NPC_sendbuf, "\n");
						for( i=(dengonindex - 5); i<=dengonindex; i++){
							strcat( NPC_sendbuf, memberlist[fmindex_wk].memo[i]);
							strcat( NPC_sendbuf, "\n");
						}
						sprintf(tmp, "%d\n", dengonindex);
						strcat( NPC_sendbuf, tmp);
					}
					if(dengonindex < 6){
						strcpy( NPC_sendbuf, memberlist[fmindex_wk].memo[memberlist[fmindex_wk].memonum+(dengonindex - 6)]);
						strcat( NPC_sendbuf, "\n");
						for( i=memberlist[fmindex_wk].memonum+(dengonindex - 5); i<memberlist[fmindex_wk].memonum; i++){
							strcat( NPC_sendbuf, memberlist[fmindex_wk].memo[i]);
							strcat( NPC_sendbuf, "\n");
						}
						for( i=0; i<=dengonindex; i++){
							strcat( NPC_sendbuf, memberlist[fmindex_wk].memo[i]);
							strcat( NPC_sendbuf, "\n");
						}
						sprintf(tmp, "%d\n", dengonindex);
						strcat( NPC_sendbuf, tmp);
					}
					
					lssproto_WN_send( fd, WINDOW_FMMESSAGETYPE_DENGON,
						buttontype,
						CHAR_WINDOWTYPE_FM_DENGON,
#ifndef _FM_MODIFY
						CHAR_getWorkInt( index, CHAR_WORKOBJINDEX),
#else
						-1,
#endif
						makeEscapeString( NPC_sendbuf, buf, sizeof(buf)));
				}
				break;
			case WINDOW_BUTTONTYPE_OK:
				{
					int    fd,i;
					struct tm tm1;
					char   m_buf[4096];
					
					fd = getfdFromCharaIndex( talker);
					if( fd == -1 ) return;
					if( strlen(data) == 0 ) break;
					
					memberlist[fmindex_wk].memonum++;
					if( memberlist[fmindex_wk].memonum > DENGONFILELINENUM) 
						memberlist[fmindex_wk].memonum = DENGONFILELINENUM;
					
					memberlist[fmindex_wk].memoindex++;
					if( memberlist[fmindex_wk].memoindex >= DENGONFILELINENUM) 
						memberlist[fmindex_wk].memoindex=0;
					
					dengonindex = memberlist[fmindex_wk].memoindex;
					
					if( dengonindex < 6 && memberlist[fmindex_wk].memonum<DENGONFILELINENUM)
						dengonindex = 6;
					
					memcpy( &tm1, localtime( (time_t *)&NowTime.tv_sec), sizeof(struct tm));
					getStringFromIndexWithDelim(data,"|",2,tmp_buffer,sizeof(tmp_buffer));
					sprintf( m_buf,"%s|%2d/%02d %2d:%02d %s",
						tmp_buffer,
						tm1.tm_mon +1, tm1.tm_mday, tm1.tm_hour, tm1.tm_min,
						CHAR_getChar( talker, CHAR_NAME));
					strcpy(memberlist[fmindex_wk].memo[memberlist[fmindex_wk].memoindex], m_buf);
					
					// send acsv 
					saacproto_ACFMWriteMemo_send( acfd, CHAR_getChar( talker, CHAR_FMNAME), 
						CHAR_getInt(talker, CHAR_FMINDEX),
						makeEscapeString( memberlist[fmindex_wk].memo[memberlist[fmindex_wk].memoindex], buf, sizeof(buf)),
						fmindex_wk);
					
					if( dengonindex >= 6){
						strcpy( NPC_sendbuf, memberlist[fmindex_wk].memo[dengonindex - 6]);
						strcat( NPC_sendbuf, "\n");
						for( i=(dengonindex-5); i<=dengonindex; i++){
							strcat( NPC_sendbuf, memberlist[fmindex_wk].memo[i]);
							strcat( NPC_sendbuf, "\n");
						}
						sprintf(tmp, "%d\n", dengonindex);
						strcat( NPC_sendbuf, tmp);
					}
					if( dengonindex < 6){
						strcpy( NPC_sendbuf, memberlist[fmindex_wk].memo[memberlist[fmindex_wk].memonum+(dengonindex - 6)]);
						strcat( NPC_sendbuf, "\n");
						for( i=memberlist[fmindex_wk].memonum+(dengonindex - 5); i<memberlist[fmindex_wk].memonum; i++){
							strcat( NPC_sendbuf, memberlist[fmindex_wk].memo[i]);
							strcat( NPC_sendbuf, "\n");
						}
						for( i=0; i<=dengonindex; i++){
							strcat( NPC_sendbuf, memberlist[fmindex_wk].memo[i]);
							strcat( NPC_sendbuf, "\n");
						}
						sprintf(tmp, "%d\n", dengonindex);
						strcat( NPC_sendbuf, tmp);
					}
					lssproto_WN_send( fd, WINDOW_FMMESSAGETYPE_DENGON,
						WINDOW_BUTTONTYPE_OKCANCEL|
						WINDOW_BUTTONTYPE_PREV,
						CHAR_WINDOWTYPE_FM_DENGON,
#ifndef _FM_MODIFY
						CHAR_getWorkInt( index, CHAR_WORKOBJINDEX),
#else
						-1,
#endif
						makeEscapeString( NPC_sendbuf, buf, sizeof(buf)));
				}
				break;
			default:
				break;
      }  // Switch End
    }  // If End
        
    // 家族之间留言板
    else if(seqno == CHAR_WINDOWTYPE_FM_FMSDENGON)
    {
			int dengonindex;
			char tmp_buffer[4096],tmp[4096];
			getStringFromIndexWithDelim(data,"|",1,tmp_buffer,sizeof(tmp_buffer));
			dengonindex = atoi(tmp_buffer);
			
			switch( select ){
			case WINDOW_BUTTONTYPE_NEXT:
			case WINDOW_BUTTONTYPE_PREV:
				{
					int fd,i;
					fd = getfdFromCharaIndex( talker);
					if( fd == -1 ) return;
					
					dengonindex += 7 * (( select == WINDOW_BUTTONTYPE_NEXT) ? 1 : -1);
					if( dengonindex > fmsmemo.memoindex && fmsmemo.memonum < FMSDENGONFILELINENUM)
						dengonindex = fmsmemo.memoindex;
					else if( dengonindex < 6 && fmsmemo.memonum < FMSDENGONFILELINENUM) 
						dengonindex = 6;
					else if( dengonindex < 1 && fmsmemo.memonum >= FMSDENGONFILELINENUM)
						dengonindex = fmsmemo.memonum+dengonindex;
					else if( dengonindex > fmsmemo.memonum && fmsmemo.memonum >= FMSDENGONFILELINENUM)
						dengonindex -= fmsmemo.memonum;
					
					buttontype = WINDOW_BUTTONTYPE_OKCANCEL;
					if( dengonindex==fmsmemo.memoindex && fmsmemo.memonum >= FMSDENGONFILELINENUM) 
						buttontype |= WINDOW_BUTTONTYPE_PREV;
					else if( (dengonindex-7)<=fmsmemo.memoindex && (dengonindex - 7)>=(fmsmemo.memoindex - 7) &&
						fmsmemo.memonum >= FMSDENGONFILELINENUM) 
						buttontype |= WINDOW_BUTTONTYPE_NEXT;
					else if( dengonindex == fmsmemo.memoindex) 
						buttontype |= WINDOW_BUTTONTYPE_PREV;    
					else if( dengonindex == 6 ) 
						buttontype |= WINDOW_BUTTONTYPE_NEXT;
					else{
						buttontype |= WINDOW_BUTTONTYPE_PREV;
						buttontype |= WINDOW_BUTTONTYPE_NEXT;
					}
					
					if( dengonindex >= 6 ){
						if( (dengonindex - 6)  >= 140 ) return;
						strcpy( NPC_sendbuf, fmsmemo.memo[dengonindex - 6]);
						strcat( NPC_sendbuf, "\n");
						for( i=(dengonindex - 5); i<=dengonindex; i++){
							strcat( NPC_sendbuf, fmsmemo.memo[i]);
							strcat( NPC_sendbuf, "\n");
						}
						sprintf(tmp, "%d\n", dengonindex);
						strcat( NPC_sendbuf, tmp);
					}
					if(dengonindex < 6){
						if( (fmsmemo.memonum+(dengonindex - 6))  >= 140 ||
							(fmsmemo.memonum+(dengonindex - 6))  < 0 ) return;
						
						strcpy( NPC_sendbuf, fmsmemo.memo[fmsmemo.memonum+(dengonindex - 6)]);
						
						strcat( NPC_sendbuf, "\n");
						for( i=fmsmemo.memonum+(dengonindex - 5); i<fmsmemo.memonum; i++){
							strcat( NPC_sendbuf, fmsmemo.memo[i]);
							strcat( NPC_sendbuf, "\n");
						}
						for( i=0; i<=dengonindex; i++){
							strcat( NPC_sendbuf, fmsmemo.memo[i]);
							strcat( NPC_sendbuf, "\n");
						}
						sprintf(tmp, "%d\n", dengonindex);
						strcat( NPC_sendbuf, tmp);
					}
					lssproto_WN_send( fd, WINDOW_FMMESSAGETYPE_FMSDENGON,
						buttontype,
						CHAR_WINDOWTYPE_FM_FMSDENGON,
#ifndef _FM_MODIFY
						CHAR_getWorkInt( index, CHAR_WORKOBJINDEX),
#else
						-1,
#endif
						makeEscapeString( NPC_sendbuf, buf, sizeof(buf)));
				}
				break;
			case WINDOW_BUTTONTYPE_OK:
				{
					int    fd,i;
					struct tm tm1;
					char   m_buf[4096];
					
					fd = getfdFromCharaIndex( talker );
					if( fd == -1 )  return;
					
#ifdef _FMVER21  
					if( CHAR_getInt( talker, CHAR_FMLEADERFLAG) != FMMEMBER_LEADER){              
#else
					if( CHAR_getInt( talker, CHAR_FMLEADERFLAG) != 1){              
#endif              
							sprintf( NPC_sendbuf, "              『警       告』\n 抱歉！你不是族长，所以仅能查看。");
							lssproto_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,
								WINDOW_BUTTONTYPE_OK,
								-1,
								-1,
								makeEscapeString( NPC_sendbuf, buf, sizeof(buf)));
							return;
						}
						
						if( strlen( data) == 0 )  break;
						
						fmsmemo.memonum++;
						if( fmsmemo.memonum > FMSDENGONFILELINENUM) 
							fmsmemo.memonum = FMSDENGONFILELINENUM;
            
						fmsmemo.memoindex++;
						if( fmsmemo.memoindex >= FMSDENGONFILELINENUM) 
							fmsmemo.memoindex = 0;
						
						dengonindex = fmsmemo.memoindex;
						if( dengonindex < 6 && fmsmemo.memonum<FMSDENGONFILELINENUM)
							dengonindex = 6;
						
						memcpy( &tm1, localtime( (time_t *)&NowTime.tv_sec), sizeof( tm1));
						getStringFromIndexWithDelim(data,"|",2,tmp_buffer,sizeof(tmp_buffer));
						sprintf( m_buf,"%s|%2d/%02d %2d:%02d %s",
							tmp_buffer,
							tm1.tm_mon +1, tm1.tm_mday, tm1.tm_hour, tm1.tm_min,
							CHAR_getChar( talker, CHAR_NAME));
						strcpy( fmsmemo.memo[fmsmemo.memoindex], m_buf);
						
						// send acsv 
						saacproto_ACFMWriteMemo_send( acfd, "FMS", 
							FMSDENGON_SN,
							makeEscapeString( fmsmemo.memo[fmsmemo.memoindex], buf, sizeof(buf)),
							FMSDENGON_SN);
						
						if( dengonindex >= 6){
							strcpy( NPC_sendbuf, fmsmemo.memo[dengonindex - 6]);
							strcat( NPC_sendbuf, "\n");
							for( i=(dengonindex - 5); i<=dengonindex; i++){
								strcat( NPC_sendbuf, fmsmemo.memo[i]);
								strcat( NPC_sendbuf, "\n");
							}
							sprintf(tmp, "%d\n", dengonindex);
							strcat( NPC_sendbuf, tmp);
						}
						if(dengonindex < 6){
							strcpy( NPC_sendbuf, fmsmemo.memo[fmsmemo.memonum+(dengonindex - 6)]);
							strcat( NPC_sendbuf, "\n");
							for( i=fmsmemo.memonum+(dengonindex - 5); i<fmsmemo.memonum; i++){
								strcat( NPC_sendbuf, fmsmemo.memo[i]);
								strcat( NPC_sendbuf, "\n");
							}
							for( i=0; i<=dengonindex; i++){
								strcat( NPC_sendbuf, fmsmemo.memo[i]);
								strcat( NPC_sendbuf, "\n");
							}
							sprintf(tmp, "%d\n", dengonindex);
							strcat( NPC_sendbuf, tmp);
						}
						lssproto_WN_send( fd, WINDOW_FMMESSAGETYPE_FMSDENGON,
							WINDOW_BUTTONTYPE_OKCANCEL|
							WINDOW_BUTTONTYPE_PREV,
							CHAR_WINDOWTYPE_FM_FMSDENGON,
#ifndef _FM_MODIFY
							CHAR_getWorkInt( index, CHAR_WORKOBJINDEX),
#else
							-1,
#endif
							makeEscapeString( NPC_sendbuf, buf, sizeof(buf)));
					}
					break;
				default: break;
      }  // Switch End
    }  // If End
        
    // 说明视窗(家族据点)
    else if( seqno == CHAR_WINDOWTYPE_FM_MESSAGE1)
    {
			int fd;
			char pointbuf[1024];
			strcpy( pointbuf, "");
			fd = getfdFromCharaIndex( talker );
			if( fd == -1 )  return;

			switch( select ){
			case WINDOW_BUTTONTYPE_OK:
				{
#ifdef _FIX_FM_FMPOINT
					char x[4];
					char y[4];
					char fmindex[4];
					char name[32];
					int windowtype = WINDOW_MESSAGETYPE_MESSAGE;
					if( CHAR_getInt( talker, CHAR_FMLEADERFLAG) == FMMEMBER_LEADER &&
						          CHAR_getWorkInt(talker, CHAR_WORKFMSETUPFLAG)==1){
						CHAR_setInt( talker, CHAR_LISTPAGE,0);
						int tkfmindex = CHAR_getWorkInt(talker, CHAR_WORKFMINDEXI);
						int i,check=TRUE;
						char fmindex[4];
						for(i=0; i<MANORNUM; i++){	// 10个庄园
						  getStringFromIndexWithDelim(fmpointlist.pointlistarray[i], "|", 5, fmindex, sizeof(fmindex));
						  if(tkfmindex==atoi(fmindex)-1 ){
						  	 check=FALSE;
						   	 break;
						  }
						}
					  if(check==TRUE){
							for (i=CHAR_getInt( talker, CHAR_LISTPAGE); i<CHAR_getInt( talker, CHAR_LISTPAGE)+5; i++){	// 10个庄园
							  getStringFromIndexWithDelim(fmpointlist.pointlistarray[i], "|", 5, fmindex, sizeof(fmindex));
						 	  if(atoi(fmindex)<=0 ){
						   		check=TRUE;
						    	break;
						  	}
						  }
						}
					  if(check==TRUE){
					   	windowtype = WINDOW_MESSAGETYPE_SELECT;
							strcpy(pointbuf,"3\n        　    “家族据地列表”\n\n");
					  }else{
					  	windowtype = WINDOW_MESSAGETYPE_MESSAGE;
					   	strcpy(pointbuf,"        　    “家族据地列表”\n\n");
							
						}
					}else{
						strcpy(pointbuf,"        　    “家族据地列表”\n\n");
					}
					strcat(pointbuf," “地　点”“东”“南”“状　　态”\n");
					getStringFromIndexWithDelim( fmpointlist.pointlistarray[0],"|",3, x, sizeof( x));
					getStringFromIndexWithDelim( fmpointlist.pointlistarray[0],"|",4, y, sizeof( y));
					getStringFromIndexWithDelim( fmpointlist.pointlistarray[0],"|",5, fmindex, sizeof( fmindex));
					getStringFromIndexWithDelim( fmpointlist.pointlistarray[0],"|",6, name, sizeof( name));
					if(atoi(fmindex)<=0)strcpy(name,"  未占领");
					sprintf(pointbuf,"%s  萨姆吉尔  %3s   %3s   %s\n", pointbuf, x, y, name);
					getStringFromIndexWithDelim( fmpointlist.pointlistarray[1],"|",3, x, sizeof( x));
					getStringFromIndexWithDelim( fmpointlist.pointlistarray[1],"|",4, y, sizeof( y));
					getStringFromIndexWithDelim( fmpointlist.pointlistarray[1],"|",5, fmindex, sizeof( fmindex));
					getStringFromIndexWithDelim( fmpointlist.pointlistarray[1],"|",6, name, sizeof( name));
					if(atoi(fmindex)<=0)strcpy(name,"  未占领");
					sprintf(pointbuf,"%s  玛丽娜斯  %3s   %3s   %s\n", pointbuf, x, y, name);
					getStringFromIndexWithDelim( fmpointlist.pointlistarray[2],"|",3, x, sizeof( x));
					getStringFromIndexWithDelim( fmpointlist.pointlistarray[2],"|",4, y, sizeof( y));
					getStringFromIndexWithDelim( fmpointlist.pointlistarray[2],"|",5, fmindex, sizeof( fmindex));
					getStringFromIndexWithDelim( fmpointlist.pointlistarray[2],"|",6, name, sizeof( name));
					if(atoi(fmindex)<=0)strcpy(name,"  未占领");
					sprintf(pointbuf,"%s  加　　加  %3s   %3s   %s\n", pointbuf, x, y, name);
					getStringFromIndexWithDelim( fmpointlist.pointlistarray[3],"|",3, x, sizeof( x));
					getStringFromIndexWithDelim( fmpointlist.pointlistarray[3],"|",4, y, sizeof( y));
					getStringFromIndexWithDelim( fmpointlist.pointlistarray[3],"|",5, fmindex, sizeof( fmindex));
					getStringFromIndexWithDelim( fmpointlist.pointlistarray[3],"|",6, name, sizeof( name));
					if(atoi(fmindex)<=0)strcpy(name,"  未占领");
					sprintf(pointbuf,"%s  卡鲁他那  %3s   %3s   %s\n", pointbuf, x, y, name);
					getStringFromIndexWithDelim( fmpointlist.pointlistarray[4],"|",3, x, sizeof( x));
					getStringFromIndexWithDelim( fmpointlist.pointlistarray[4],"|",4, y, sizeof( y));
					getStringFromIndexWithDelim( fmpointlist.pointlistarray[4],"|",5, fmindex, sizeof( fmindex));
					getStringFromIndexWithDelim( fmpointlist.pointlistarray[4],"|",6, name, sizeof( name));
					if(atoi(fmindex)<=0)strcpy(name,"  未占领");
					sprintf(pointbuf,"%s  伊　　甸  %3s   %3s   %s\n", pointbuf, x, y, name);
#else
					int i;
					strcpy( pointbuf, fmpointlist.pointlistarray[0]);
					strcat( pointbuf, "\n");
					for( i=1; i<=FMPOINTNUM; i++){
						strcat( pointbuf, fmpointlist.pointlistarray[i]);
						strcat( pointbuf, "\n");
					}
#endif

#ifdef _FIX_FM_FMPOINT
					lssproto_WN_send( fd, windowtype,
						WINDOW_BUTTONTYPE_OK|WINDOW_BUTTONTYPE_NEXT,
						CHAR_WINDOWTYPE_FM_POINTLIST,
#else
					lssproto_WN_send( fd, WINDOW_FMMESSAGETYPE_POINTLIST,
						WINDOW_BUTTONTYPE_OK,
						CHAR_WINDOWTYPE_FM_POINTLIST,
#endif
#ifndef _FM_MODIFY
						CHAR_getWorkInt( index, CHAR_WORKOBJINDEX),
#else
						-1,
#endif

#ifdef _FIX_FM_FMPOINT
					pointbuf);
#else
					makeEscapeString( pointbuf, buf, sizeof(buf)));
#endif
				}
				break;
			default:
				break;
			}
    }        

    // 说明视窗(成员列表)
    else if( seqno == CHAR_WINDOWTYPE_FM_MESSAGE2)
    {
			int fd,i;
			char numberlistbuf[4096];
			int fmindex_wk;
			fmindex_wk = CHAR_getWorkInt( talker, CHAR_WORKFMINDEXI);
			
			if( fmindex_wk < 0 || fmindex_wk >= FMMAXNUM) return;
			
			fd = getfdFromCharaIndex( talker );
			if( fd == -1 )  return;
			
			switch( select ){
			case WINDOW_BUTTONTYPE_OK:
				{
					strcpy( numberlistbuf, memberlist[fmindex_wk].numberlistarray[0]);
					strcat( numberlistbuf, "\n");
					for( i=1; i<10; i++){
						strcat( numberlistbuf, memberlist[fmindex_wk].numberlistarray[i]);
						strcat( numberlistbuf, "\n");
					}
					// 增加程式码(向AC要召募人员的值)
					sprintf(enlistbuf, "是否继续召募家族人员|0|%d",memberlist[fmindex_wk].accept);
					strcat( numberlistbuf, enlistbuf);
					strcat( numberlistbuf, "\n");
					lssproto_WN_send( fd, WINDOW_FMMESSAGETYPE_SELECT,
						WINDOW_BUTTONTYPE_OK|
						WINDOW_BUTTONTYPE_NEXT,
						CHAR_WINDOWTYPE_FM_MEMBERLIST,
#ifndef _FM_MODIFY
						CHAR_getWorkInt( index, CHAR_WORKOBJINDEX),
#else
						-1,
#endif
						makeEscapeString( numberlistbuf, buf, sizeof(buf)));
				}
				break;
			default:
				break;
			}
    }        

    // 强者表的选项视窗
    else if( seqno == CHAR_WINDOWTYPE_FM_DPSELECT)
    {
			int fmindex_wk;
			fmindex_wk = CHAR_getWorkInt( talker, CHAR_WORKFMINDEXI);
			
			if( CHAR_getInt(talker, CHAR_FMINDEX) > 0 ){
				if( fmindex_wk < 0 || fmindex_wk >= FMMAXNUM){
					print("FamilyNumber Data Error!!");
					return;
				}
			}
			
			buttonevent = atoi(data);
			switch( buttonevent ){
			case 1:				// 前叁十大家族综合声望列表
				{
					int  fd,i;
					char listbuf[4096];
					fd = getfdFromCharaIndex( talker );
					if( fd == -1 )  return;
					
					strcpy( listbuf, fmdptop.topmemo[0]);
					strcat( listbuf, "\n");
					for( i=1; i<10; i++){
						strcat( listbuf, fmdptop.topmemo[i]);
						strcat( listbuf, "\n");
					}
					strcat( listbuf, "0\n");
#ifdef _FMVER21              
					lssproto_WN_send( fd, WINDOW_FMMESSAGETYPE_TOP30DP,
#else
					lssproto_WN_send( fd, WINDOW_FMMESSAGETYPE_DP,
#endif              
						WINDOW_BUTTONTYPE_OK|
						WINDOW_BUTTONTYPE_NEXT,
						CHAR_WINDOWTYPE_FM_DPTOP,
#ifndef _FM_MODIFY
						CHAR_getWorkInt( index, CHAR_WORKOBJINDEX),
#else
						-1,
#endif
						makeEscapeString( listbuf, buf, sizeof(buf)));
				}
				break;
			case 2:				// 前十大家族冒险列表
				{
					int  fd,i;
					char listbuf[4096];
					fd = getfdFromCharaIndex( talker );
					if( fd == -1 )  return;
					
					strcpy( listbuf, fmdptop.adv_topmemo[0]);
					strcat( listbuf, "\n");
					for( i=1; i<10; i++){
						strcat( listbuf, fmdptop.adv_topmemo[i]);
						strcat( listbuf, "\n");
					}
					
					lssproto_WN_send( fd, WINDOW_FMMESSAGETYPE_DP,
						WINDOW_BUTTONTYPE_OK|
						WINDOW_BUTTONTYPE_PREV,
						CHAR_WINDOWTYPE_FM_DPME,
#ifndef _FM_MODIFY
						CHAR_getWorkInt( index, CHAR_WORKOBJINDEX),
#else
						-1,
#endif
						makeEscapeString( listbuf, buf, sizeof(buf)));
				}
				break;
			case 3:				// 前十大家族伺育列表
				{
					int  fd,i;
					char listbuf[4096];
					fd = getfdFromCharaIndex( talker );
					if( fd == -1 )  return;
					
					strcpy( listbuf, fmdptop.feed_topmemo[0]);
					strcat( listbuf, "\n");
					for( i=1; i<10; i++){
						strcat( listbuf, fmdptop.feed_topmemo[i]);
						strcat( listbuf, "\n");
					}
					
					lssproto_WN_send( fd, WINDOW_FMMESSAGETYPE_DP,
						WINDOW_BUTTONTYPE_OK|
						WINDOW_BUTTONTYPE_PREV,
						CHAR_WINDOWTYPE_FM_DPME,
#ifndef _FM_MODIFY
						CHAR_getWorkInt( index, CHAR_WORKOBJINDEX),
#else
						-1,
#endif
						makeEscapeString( listbuf, buf, sizeof(buf)));
				}
				break;
			case 4:				// 前十大家族合成列表
				{
					int  fd,i;
					char listbuf[4096];
					fd = getfdFromCharaIndex( talker );
					if( fd == -1 )  return;
					
					strcpy( listbuf, fmdptop.syn_topmemo[0]);
					strcat( listbuf, "\n");
					for( i=1; i<10; i++){
						strcat( listbuf, fmdptop.syn_topmemo[i]);
						strcat( listbuf, "\n");
					}
					
					lssproto_WN_send( fd, WINDOW_FMMESSAGETYPE_DP,
						WINDOW_BUTTONTYPE_OK|
						WINDOW_BUTTONTYPE_PREV,
						CHAR_WINDOWTYPE_FM_DPME,
#ifndef _FM_MODIFY
						CHAR_getWorkInt( index, CHAR_WORKOBJINDEX),
#else
						-1,
#endif
						makeEscapeString( listbuf, buf, sizeof(buf)));
				}
				break;
			case 5:				// 前十大家族料理列表
				{
					int  fd,i;
					char listbuf[4096];
					fd = getfdFromCharaIndex( talker );
					if( fd == -1 )  return;
					
					strcpy( listbuf, fmdptop.food_topmemo[0]);
					strcat( listbuf, "\n");
					for( i=1; i<10; i++){
						strcat( listbuf, fmdptop.food_topmemo[i]);
						strcat( listbuf, "\n");
					}
					
					lssproto_WN_send( fd, WINDOW_FMMESSAGETYPE_DP,
						WINDOW_BUTTONTYPE_OK|
						WINDOW_BUTTONTYPE_PREV,
						CHAR_WINDOWTYPE_FM_DPME,
#ifndef _FM_MODIFY
						CHAR_getWorkInt( index, CHAR_WORKOBJINDEX),
#else
						-1,
#endif
						makeEscapeString( listbuf, buf, sizeof(buf)));
				}
				break;
			case 6:				// 前十大家族ＰＫ列表
				{
					int  fd,i;
					char listbuf[4096];
					fd = getfdFromCharaIndex( talker );
					if( fd == -1 )  return;
					
					strcpy( listbuf, fmdptop.pk_topmemo[0]);
					strcat( listbuf, "\n");
					for( i=1; i<10; i++){
						strcat( listbuf, fmdptop.pk_topmemo[i]);
						strcat( listbuf, "\n");
					}
					
					lssproto_WN_send( fd, WINDOW_FMMESSAGETYPE_DP,
						WINDOW_BUTTONTYPE_OK|
						WINDOW_BUTTONTYPE_PREV,
						CHAR_WINDOWTYPE_FM_DPME,
#ifndef _FM_MODIFY
						CHAR_getWorkInt( index, CHAR_WORKOBJINDEX),
#else
						-1,
#endif
						makeEscapeString( listbuf, buf, sizeof(buf)));
				}
				break;
			case 7:				// 自己家族声望排行榜
				{
					int  fd,i,h,k,fmid;
					char listbuf[4096];
					
					fd = getfdFromCharaIndex( talker );
					if( fd == -1 )  return;
					
					fmid = CHAR_getWorkInt(talker, CHAR_WORKFMINDEXI);
					if( fmid < 0 ){
						sprintf( NPC_sendbuf, "              『警       告』\n 抱歉！你不是家族人员，无法查看。");
						lssproto_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE, WINDOW_BUTTONTYPE_OK,
							-1,
							-1,
							makeEscapeString( NPC_sendbuf, buf, sizeof(buf)));
						return;
					}
					
					for( h=0; h<FMMAXNUM; h++)
						if( fmdptop.fmtopid[h] == fmid ) 
							break;
						
						k = h;                 // 标示颜色用(多传一个1，以供Client之用)
						if(h <= 4) h = 0;
						else if(h >= 994 ) h = 990;
						else h -= 4;
						
						strcpy( listbuf, fmdptop.topmemo[h]);
						if( k == h ) strcat( listbuf, "|1");
						strcat( listbuf, "\n");
						for( i = h + 1; i < h + 10; i++){
							strcat( listbuf, fmdptop.topmemo[i]);
							if(i == k) strcat( listbuf, "|1");
							strcat( listbuf, "\n");
						}
#ifdef _FMVER21              
						lssproto_WN_send( fd, WINDOW_FMMESSAGETYPE_TOP30DP,
#else
            lssproto_WN_send( fd, WINDOW_FMMESSAGETYPE_DP,
#endif              
							WINDOW_BUTTONTYPE_OK|
							WINDOW_BUTTONTYPE_PREV,
							CHAR_WINDOWTYPE_FM_DPME,
#ifndef _FM_MODIFY
							CHAR_getWorkInt( index, CHAR_WORKOBJINDEX),
#else
							-1,
#endif
							makeEscapeString( listbuf, buf, sizeof(buf)));
				}
				break;
			default:
				break;
        }
    }
        
    // 选项视窗
    else if( seqno == CHAR_WINDOWTYPE_FM_SELECT)
    {
			int fmindex_wk;
			fmindex_wk = CHAR_getWorkInt( talker, CHAR_WORKFMINDEXI);
			
			if( CHAR_getInt(talker, CHAR_FMINDEX) > 0 ){
				if( fmindex_wk < 0 || fmindex_wk >= FMMAXNUM){
					print("FamilyNumber Data Error!!");
					return;
				}
			}
			
			buttonevent = atoi(data);
			
			switch( buttonevent ){
			case FM_MEMBERLIST:
				{
					int fd;
					
					fd = getfdFromCharaIndex( talker );
					if( fd == -1 )  return;
					
					if( CHAR_getInt(talker, CHAR_FMINDEX) <= 0){  
						sprintf( NPC_sendbuf, "              『警       告』\n 抱歉！你不是家族人员，不得使用公布栏。");
						lssproto_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE, WINDOW_BUTTONTYPE_OK,
							-1, -1, makeEscapeString( NPC_sendbuf, buf, sizeof(buf)));
						return;
					}
					/*
					#ifdef _FMVER21                   
					if( CHAR_getInt( talker, CHAR_FMLEADERFLAG ) == FMMEMBER_LEADER ||
					CHAR_getInt( talker, CHAR_FMLEADERFLAG ) == FMMEMBER_ELDER ){
					#else
					if( CHAR_getInt( talker, CHAR_FMLEADERFLAG) == 1){
					#endif              
					saacproto_ACShowMemberList_send( acfd, fmindex_wk);
					READTIME1 = NowTime.tv_sec+FM_WAITTIME;
					}else
					*/
					if( NowTime.tv_sec > READTIME1 ){
						saacproto_ACShowMemberList_send( acfd, fmindex_wk);
						READTIME1 = NowTime.tv_sec+FM_WAITTIME;
					}
					
#ifdef _FMVER21                   
					//              if( CHAR_getInt( talker, CHAR_FMLEADERFLAG ) == FMMEMBER_LEADER || 
					//                  CHAR_getInt( talker, CHAR_FMLEADERFLAG ) == FMMEMBER_ELDER ||
					//                  CHAR_getInt( talker, CHAR_FMLEADERFLAG ) == FMMEMBER_VICELEADER ){
					if( CHAR_getInt( talker, CHAR_FMLEADERFLAG ) == FMMEMBER_LEADER || 
						CHAR_getInt( talker, CHAR_FMLEADERFLAG ) == FMMEMBER_ELDER ){
#else
						if( CHAR_getInt( talker, CHAR_FMLEADERFLAG) == 1 ){
#endif              
							sprintf( NPC_sendbuf, "               『族 长 需 知』\n请小心处理族员的资料，一经修改後就无法回复原态，敬请小心。");
						}else{
							sprintf( NPC_sendbuf, "               『列 表 需 知』\n 此表族长可作修改，族员仅能查看。");
						}
						
						lssproto_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,
							WINDOW_BUTTONTYPE_OK,
							CHAR_WINDOWTYPE_FM_MESSAGE2,
#ifndef _FM_MODIFY
							CHAR_getWorkInt( index, CHAR_WORKOBJINDEX),
#else
							-1,
#endif
							makeEscapeString( NPC_sendbuf, buf, sizeof(buf)));
          }
          break;
			case FM_FMPOINT:
				{
					int fd;
					
					fd = getfdFromCharaIndex( talker );
					if( fd == -1 )  return;
					
#ifdef _FMVER21                   
					if( CHAR_getInt( talker, CHAR_FMLEADERFLAG) == FMMEMBER_LEADER ){
#else
						if( CHAR_getInt( talker, CHAR_FMLEADERFLAG) == 1 ){
#endif              
							saacproto_ACFMPointList_send(acfd);
							sprintf( NPC_sendbuf, "               『族 长 需 知』\n请小心慎选所申请的据点，一但选取据点後就无法回复原态，敬请小心。");
							READTIME4 = NowTime.tv_sec+FM_WAITTIME;
						}
						else{
							sprintf( NPC_sendbuf, "               『列 表 需 知』\n此表族长可以申请，其馀仅能查看。");
						}
						
						if( NowTime.tv_sec > READTIME4 ){
							saacproto_ACFMPointList_send(acfd);
							READTIME4 = NowTime.tv_sec+FM_WAITTIME;
						}
						
						lssproto_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,
							WINDOW_BUTTONTYPE_OK,
							CHAR_WINDOWTYPE_FM_MESSAGE1,
#ifndef _FM_MODIFY
							CHAR_getWorkInt( index, CHAR_WORKOBJINDEX),
#else
							-1,
#endif
							makeEscapeString( NPC_sendbuf, buf, sizeof(buf)));
          }
          break;
			case FM_FMDPTOP:
				{
					int  fd;
					
					fd = getfdFromCharaIndex( talker );
					if( fd == -1 )  return;
					
					if( NowTime.tv_sec > READTIME3 ){
						saacproto_ACShowTopFMList_send( acfd, FM_TOP_INTEGRATE );
						saacproto_ACShowTopFMList_send( acfd, FM_TOP_ADV );    
						saacproto_ACShowTopFMList_send( acfd, FM_TOP_FEED );
						saacproto_ACShowTopFMList_send( acfd, FM_TOP_SYNTHESIZE );
						saacproto_ACShowTopFMList_send( acfd, FM_TOP_DEALFOOD );
						saacproto_ACShowTopFMList_send( acfd, FM_TOP_PK );                           
						READTIME3 = NowTime.tv_sec+FM_WAITTIME;
					}
					memset(NPC_sendbuf,0,sizeof(NPC_sendbuf));
					strcpy( NPC_sendbuf, "\n              三十大家族声望列表\n");
					strcat( NPC_sendbuf, "              十大冒险家族\n");
					strcat( NPC_sendbuf, "              十大饲育家族\n");
					strcat( NPC_sendbuf, "              十大合成家族\n");
					strcat( NPC_sendbuf, "              十大料理家族\n");
					strcat( NPC_sendbuf, "              十大战斗家族\n");
					strcat( NPC_sendbuf, "              自己家族声望列表\n");			
						
					lssproto_WN_send( fd, WINDOW_MESSAGETYPE_SELECT,
						WINDOW_BUTTONTYPE_NONE,
						CHAR_WINDOWTYPE_FM_DPSELECT,
#ifndef _FM_MODIFY
						CHAR_getWorkInt( index, CHAR_WORKOBJINDEX),
#else
						-1,
#endif
						makeEscapeString( NPC_sendbuf, buf, sizeof(buf)));
					
				}
				break;
			case FM_MEMBERMEMO:
				{
					int fd,i,dengonindex;
					char tmp[4096];
					fd = getfdFromCharaIndex( talker );
					
					if( fd == -1 )  return;
					
					if( CHAR_getInt(talker, CHAR_FMINDEX) <= 0){  
						sprintf( NPC_sendbuf, "              『警       告』\n 抱歉！你不是家族人员，不得使用公布栏。");
						
						lssproto_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,
							WINDOW_BUTTONTYPE_OK,
							-1,
							-1,
							makeEscapeString( NPC_sendbuf, buf, sizeof(buf)));
						return;
					}
					
					if( NowTime.tv_sec > READTIME2 ){
						saacproto_ACFMReadMemo_send( acfd, fmindex_wk);
						READTIME2 = NowTime.tv_sec+FM_WAITTIME;
					}
					
					dengonindex = memberlist[fmindex_wk].memoindex;
					if( memberlist[fmindex_wk].memoindex < 6 && memberlist[fmindex_wk].memonum < DENGONFILELINENUM ){
						dengonindex = 6;
					}    
					
					if( dengonindex >= 6 ){
						strcpy( NPC_sendbuf, memberlist[fmindex_wk].memo[dengonindex - 6]);
						strcat( NPC_sendbuf, "\n");
						for( i=(dengonindex - 5); i<=dengonindex; i++){
							strcat( NPC_sendbuf, memberlist[fmindex_wk].memo[i]);
							strcat( NPC_sendbuf, "\n");
						}
						sprintf(tmp, "%d\n", dengonindex);
						strcat( NPC_sendbuf, tmp);
					}
					if( dengonindex < 6 ){
						strcpy( NPC_sendbuf,
							memberlist[fmindex_wk].memo[memberlist[fmindex_wk].memonum + (dengonindex - 6)]);
						strcat( NPC_sendbuf, "\n");
						for( i=memberlist[fmindex_wk].memonum + (dengonindex - 5); i<memberlist[fmindex_wk].memonum; i++){
							strcat( NPC_sendbuf, memberlist[fmindex_wk].memo[i]);
							strcat( NPC_sendbuf, "\n");
						}
						for( i=0; i<=dengonindex; i++){
							strcat( NPC_sendbuf, memberlist[fmindex_wk].memo[i]);
							strcat( NPC_sendbuf, "\n");
						}
						sprintf(tmp, "%d\n", dengonindex);
						strcat( NPC_sendbuf, tmp);
					}
					
					lssproto_WN_send( fd, WINDOW_FMMESSAGETYPE_DENGON,
						WINDOW_BUTTONTYPE_OKCANCEL|
						WINDOW_BUTTONTYPE_PREV,
						CHAR_WINDOWTYPE_FM_DENGON,
#ifndef _FM_MODIFY
						CHAR_getWorkInt( index, CHAR_WORKOBJINDEX),
#else
						-1,
#endif
						makeEscapeString( NPC_sendbuf, buf, sizeof(buf)));
				}
				break;
			case FM_FMMEMO:
				{
					int fd,i,dengonindex;
					char tmp[4096];
					fd = getfdFromCharaIndex( talker );
					
					if( fd == -1 )  return;
					
					if( NowTime.tv_sec > READTIME3 ){
						saacproto_ACFMReadMemo_send( acfd, FMSDENGON_SN);
						READTIME3 = NowTime.tv_sec+FM_WAITTIME;
					}
					dengonindex = fmsmemo.memoindex;
					if( fmsmemo.memoindex<6 || fmsmemo.memonum>FMSDENGONFILELINENUM ){
						dengonindex = 6; 
					}
					if( dengonindex >= 6 ){
						strcpy( NPC_sendbuf, fmsmemo.memo[dengonindex - 6]);
						strcat( NPC_sendbuf, "\n");
						for( i=(dengonindex - 5); i<=dengonindex; i++){
							strcat( NPC_sendbuf, fmsmemo.memo[i]);
							strcat( NPC_sendbuf, "\n");
						}
						sprintf(tmp, "%d\n", dengonindex);
						strcat( NPC_sendbuf, tmp);
					}
					if( dengonindex < 6 ){
						if( (fmsmemo.memonum + (dengonindex - 6)) < 0 || (fmsmemo.memonum + (dengonindex - 6)) >= 140 )
							return;
						strcpy( NPC_sendbuf, fmsmemo.memo[fmsmemo.memonum + (dengonindex - 6)]);
						strcat( NPC_sendbuf, "\n");
						for( i=fmsmemo.memonum + (dengonindex - 5); i<fmsmemo.memonum; i++){
							strcat( NPC_sendbuf, fmsmemo.memo[i]);
							strcat( NPC_sendbuf, "\n");
						}
						for( i=0; i<=dengonindex; i++){
							strcat( NPC_sendbuf, fmsmemo.memo[i]);
							strcat( NPC_sendbuf, "\n");
						}
						sprintf(tmp, "%d\n", dengonindex);
						strcat( NPC_sendbuf, tmp);
					}
					
					lssproto_WN_send( fd, WINDOW_FMMESSAGETYPE_FMSDENGON,
						WINDOW_BUTTONTYPE_OKCANCEL|
						WINDOW_BUTTONTYPE_PREV,
						CHAR_WINDOWTYPE_FM_FMSDENGON,
#ifndef _FM_MODIFY
						CHAR_getWorkInt( index, CHAR_WORKOBJINDEX),
#else
						-1,
#endif
						makeEscapeString( NPC_sendbuf, buf, sizeof(buf)));
				}   
				break;
			default:
				break;
        }
    }
        
    // 成员列表
    else if( seqno == CHAR_WINDOWTYPE_FM_MEMBERLIST)
    {
			char numberlistbuf[4096],tmp_buffer[4096],dutybuf[64];
			int  numberlistindex;
			int  fmindex_wk;
			fmindex_wk = CHAR_getWorkInt( talker, CHAR_WORKFMINDEXI);
			if( fmindex_wk < 0 || fmindex_wk >= FMMAXNUM) return;
			
			getStringFromIndexWithDelim(data,"|",1,tmp_buffer,sizeof(tmp_buffer));
			numberlistindex = atoi(tmp_buffer);
			getStringFromIndexWithDelim(data,"|",2,tmp_buffer,sizeof(tmp_buffer));
			buttonevent = atoi(tmp_buffer);
			getStringFromIndexWithDelim(data,"|",3,dutybuf,sizeof(dutybuf));
			
#ifdef _FMVER21
			//        if( buttonevent>=1 && buttonevent<=11 && 
			//            ( CHAR_getInt( talker, CHAR_FMLEADERFLAG ) == FMMEMBER_LEADER || 
			//              CHAR_getInt( talker, CHAR_FMLEADERFLAG ) == FMMEMBER_ELDER ||
			//              CHAR_getInt( talker, CHAR_FMLEADERFLAG ) == FMMEMBER_VICELEADER ))
			if( buttonevent>=1 && buttonevent<=11 && 
				( CHAR_getInt( talker, CHAR_FMLEADERFLAG ) == FMMEMBER_LEADER || 
				CHAR_getInt( talker, CHAR_FMLEADERFLAG ) == FMMEMBER_ELDER ))
#else
        if( buttonevent>=1 && buttonevent<=11 && CHAR_getInt( talker, CHAR_FMLEADERFLAG) == 1 )
#endif        
        {
					int fd,i;
					int int_status;
					char getstatus[4096];
					
					fd = getfdFromCharaIndex( talker );
					if( fd == -1 )  return;
					
					strcpy( getstatus, memberlist[fmindex_wk].numberlistarray[numberlistindex+buttonevent - 1]
						+ (strlen( memberlist[fmindex_wk].numberlistarray[numberlistindex+buttonevent - 1]) - 1));
					
					int_status = atoi(getstatus);
					
					// 家族的加入、退出、申请等选项
					if( buttonevent!=11 )
#ifdef _FMVER21            
						strcpy( memberlist[fmindex_wk].numberlistarray[numberlistindex+buttonevent - 1]
						+ (strlen(memberlist[fmindex_wk].numberlistarray[numberlistindex+buttonevent - 1]) - 1), dutybuf);                			    
#else
					switch( int_status ){
					case 1:
					case 3:
						strcpy( memberlist[fmindex_wk].numberlistarray[numberlistindex+buttonevent - 1]
							+ (strlen(memberlist[fmindex_wk].numberlistarray[numberlistindex+buttonevent - 1]) - 1), "4");                      
						break;
					case 2:
						strcpy( memberlist[fmindex_wk].numberlistarray[numberlistindex+buttonevent - 1]
							+ (strlen(memberlist[fmindex_wk].numberlistarray[numberlistindex+buttonevent - 1]) - 1), "1"); 					  
						break;
					default:
						break;
					}              
#endif                 
					// 家族的召募选项
					if( buttonevent == 11 )
					{
						strcpy( getstatus, enlistbuf + (strlen(enlistbuf) - 1));
						int_status = atoi(getstatus);
						
						switch( int_status ){
						case 1:
							memberlist[fmindex_wk].accept = 0;
							sprintf(enlistbuf, "是否继续召募家族人员|%d|%d",numberlistindex,memberlist[fmindex_wk].accept);
							break;
						case 0:
							memberlist[fmindex_wk].accept = 1;
							sprintf(enlistbuf, "是否继续召募家族人员|%d|%d",numberlistindex,memberlist[fmindex_wk].accept);
							break;
						default:
							break;    
						}
					}
					
					strcpy( numberlistbuf, memberlist[fmindex_wk].numberlistarray[numberlistindex]);
					strcat( numberlistbuf, "\n");
					for( i=(numberlistindex + 1); i<numberlistindex + 10; i++){
						strcat( numberlistbuf, memberlist[fmindex_wk].numberlistarray[i]);
						strcat( numberlistbuf, "\n");
					}
					sprintf(enlistbuf, "是否继续召募家族人员|%d|%d",numberlistindex,memberlist[fmindex_wk].accept);
					strcat( numberlistbuf, enlistbuf);
					strcat( numberlistbuf, "\n");
					
					buttontype = WINDOW_BUTTONTYPE_OK;
					if( (numberlistindex + 10) > memberlist[fmindex_wk].fmnum) 
						buttontype |= WINDOW_BUTTONTYPE_PREV;
					else if( numberlistindex == 0 ) 
						buttontype |= WINDOW_BUTTONTYPE_NEXT;
					else{
						buttontype |= WINDOW_BUTTONTYPE_PREV;
						buttontype |= WINDOW_BUTTONTYPE_NEXT;
					}
					
					lssproto_WN_send( fd, WINDOW_FMMESSAGETYPE_SELECT,
						buttontype,
						CHAR_WINDOWTYPE_FM_MEMBERLIST,
#ifndef _FM_MODIFY
						CHAR_getWorkInt( index, CHAR_WORKOBJINDEX),
#else
						-1,
#endif
						makeEscapeString( numberlistbuf, buf, sizeof(buf)));
        } // end if
        switch( select ){
				case WINDOW_BUTTONTYPE_NEXT:
				case WINDOW_BUTTONTYPE_PREV:
          {
						int fd,i;
						
						fd = getfdFromCharaIndex( talker );
						if( fd == -1 )  return;
						
						numberlistindex += 10 * (( select == WINDOW_BUTTONTYPE_NEXT) ? 1 : -1);
						
						if( numberlistindex >= memberlist[fmindex_wk].fmnum) 
							numberlistindex -= 10;
						else if( numberlistindex < 1 ) 
							numberlistindex = 0;
						
						buttontype = WINDOW_BUTTONTYPE_OK;
						if( (numberlistindex + 10) >= memberlist[fmindex_wk].fmnum) 
							buttontype |= WINDOW_BUTTONTYPE_PREV;
						else if( numberlistindex==0 )
							buttontype |= WINDOW_BUTTONTYPE_NEXT;
						else{
							buttontype |= WINDOW_BUTTONTYPE_PREV;
							buttontype |= WINDOW_BUTTONTYPE_NEXT;
						}
						
						strcpy( numberlistbuf, memberlist[fmindex_wk].numberlistarray[numberlistindex]);
						strcat( numberlistbuf, "\n");
						for( i=(numberlistindex+1); i<numberlistindex+10; i++){
							strcat( numberlistbuf, memberlist[fmindex_wk].numberlistarray[i]);
							strcat( numberlistbuf, "\n");
						}
						sprintf(enlistbuf, "是否继续召募家族人员|%d|%d",numberlistindex,memberlist[fmindex_wk].accept);
						strcat( numberlistbuf, enlistbuf);
						strcat( numberlistbuf, "\n");
						lssproto_WN_send( fd, WINDOW_FMMESSAGETYPE_SELECT,
							buttontype,
							CHAR_WINDOWTYPE_FM_MEMBERLIST,
#ifndef _FM_MODIFY
							CHAR_getWorkInt( index, CHAR_WORKOBJINDEX),
#else
							-1,
#endif
							makeEscapeString( numberlistbuf, buf, sizeof(buf)));
          }
          break;
				case WINDOW_BUTTONTYPE_OK:
          break;
				default:
          break;
        }
    }
    
    // 家族强者表(前叁十)
    else if( seqno == CHAR_WINDOWTYPE_FM_DPTOP)
    {
			char listbuf[4096],tmp_buffer[4096];
			int  listindex;
			getStringFromIndexWithDelim(data,"|",1,tmp_buffer,sizeof(tmp_buffer));
			listindex = atoi(tmp_buffer);
			
			switch( select ){
			case WINDOW_BUTTONTYPE_NEXT:
			case WINDOW_BUTTONTYPE_PREV:
				{
					int fd,i;
					fd = getfdFromCharaIndex( talker );
					if( fd == -1 )  return;
					
					listindex += 10 * (( select == WINDOW_BUTTONTYPE_NEXT) ? 1 : -1);
					
					if( listindex >= 30) 
						return;
					//listindex = 20;
					//listindex -= 10;
					//else if( listindex < 1 ) 
					//    listindex = 0;
					if (listindex < 0) return;
					
					buttontype = WINDOW_BUTTONTYPE_OK;
					if( (listindex + 10) >= 30) 
						buttontype |= WINDOW_BUTTONTYPE_PREV;
					else if( listindex==0 )
						buttontype |= WINDOW_BUTTONTYPE_NEXT;
					else{
						buttontype |= WINDOW_BUTTONTYPE_PREV;
						buttontype |= WINDOW_BUTTONTYPE_NEXT;
					}
					
					strcpy( listbuf, fmdptop.topmemo[listindex]);
					strcat( listbuf, "\n");
					for( i=(listindex+1); i<listindex+10; i++){
						strcat( listbuf, fmdptop.topmemo[i]);
						strcat( listbuf, "\n");
					}
					sprintf(tmp_buffer, "%d\n", listindex);
					strcat( listbuf, tmp_buffer);
					
#ifdef _FMVER21              
					lssproto_WN_send( fd, WINDOW_FMMESSAGETYPE_TOP30DP,
#else
					lssproto_WN_send( fd, WINDOW_FMMESSAGETYPE_DP,
#endif              
						buttontype,
						CHAR_WINDOWTYPE_FM_DPTOP,
#ifndef _FM_MODIFY
						CHAR_getWorkInt( index, CHAR_WORKOBJINDEX),
#else
						-1,
#endif
						makeEscapeString( listbuf, buf, sizeof(buf)));
				}
				break;
			case WINDOW_BUTTONTYPE_OK:
				break;
			default:
				break;
			}
    }

    // 据点列表
    else if( seqno == CHAR_WINDOWTYPE_FM_POINTLIST)
    {
			char pointbuf[1024];
			int  pointlistindex;
			strcpy( pointbuf, "");
			pointlistindex = 0;
			buttonevent = atoi(data);
			
			switch( select ){
			case WINDOW_BUTTONTYPE_NEXT:
			case WINDOW_BUTTONTYPE_PREV:
				{
					int fd;
					
					fd = getfdFromCharaIndex( talker );
					if( fd == -1 )  return;
#ifdef _FIX_FM_FMPOINT
					char x[4];
					char y[4];
					char fmindex[4];
					char name[32];
					int windowtype = WINDOW_MESSAGETYPE_MESSAGE;
					pointlistindex += 5 * (( select == WINDOW_BUTTONTYPE_NEXT) ? 1 : -1);
					
					if( pointlistindex > 5) 
						pointlistindex -= 5;
					else if( pointlistindex < 1 ) 
						pointlistindex = 0;
						
					buttontype = WINDOW_BUTTONTYPE_OK;
					if( CHAR_getInt( talker, CHAR_FMLEADERFLAG) == FMMEMBER_LEADER &&
						          CHAR_getWorkInt(talker, CHAR_WORKFMSETUPFLAG)==1){
						if(select==WINDOW_BUTTONTYPE_NEXT)
							CHAR_setInt( talker, CHAR_LISTPAGE,1);
						else if(select==WINDOW_BUTTONTYPE_PREV)
							CHAR_setInt( talker, CHAR_LISTPAGE,0);
						int tkfmindex = CHAR_getWorkInt(talker, CHAR_WORKFMINDEXI);
						int i,check=TRUE;
						char fmindex[4];
						for(i=0; i<MANORNUM; i++){	// 10个庄园
						  getStringFromIndexWithDelim(fmpointlist.pointlistarray[i], "|", 5, fmindex, sizeof(fmindex));
						  if(tkfmindex==atoi(fmindex)-1 ){
						  	 check=FALSE;
						   	 break;
						  }
						}
					  if(check==TRUE){
							for (i=CHAR_getInt( talker, CHAR_LISTPAGE); i<CHAR_getInt( talker, CHAR_LISTPAGE)+5; i++){	// 10个庄园
							  getStringFromIndexWithDelim(fmpointlist.pointlistarray[i], "|", 5, fmindex, sizeof(fmindex));
						 	  if(atoi(fmindex)<=0 ){
						   		check=TRUE;
						    	break;
						  	}
						  }
						}
					  if(check==TRUE){
					   	windowtype = WINDOW_MESSAGETYPE_SELECT;
							strcpy(pointbuf,"3\n        　    “家族据地列表”\n\n");
					  }else{
					  	windowtype = WINDOW_MESSAGETYPE_MESSAGE;
					   	strcpy(pointbuf,"        　    “家族据地列表”\n\n");
							
						}
					}else{
						strcpy(pointbuf,"        　    “家族据地列表”\n\n");
					}
					strcat(pointbuf," “地　点”“东”“南”“状　　态”\n");
					if( (pointlistindex + 5) > 5){
							getStringFromIndexWithDelim( fmpointlist.pointlistarray[5],"|",3, x, sizeof( x));
							getStringFromIndexWithDelim( fmpointlist.pointlistarray[5],"|",4, y, sizeof( y));
							getStringFromIndexWithDelim( fmpointlist.pointlistarray[5],"|",5, fmindex, sizeof( fmindex));
							getStringFromIndexWithDelim( fmpointlist.pointlistarray[5],"|",6, name, sizeof( name));
							if(atoi(fmindex)<=0)strcpy(name,"  未占领");
							sprintf(pointbuf,"%s  塔 尔 塔  %3s   %3s   %s\n", pointbuf, x, y, name);
							getStringFromIndexWithDelim( fmpointlist.pointlistarray[6],"|",3, x, sizeof( x));
							getStringFromIndexWithDelim( fmpointlist.pointlistarray[6],"|",4, y, sizeof( y));
							getStringFromIndexWithDelim( fmpointlist.pointlistarray[6],"|",5, fmindex, sizeof( fmindex));
							getStringFromIndexWithDelim( fmpointlist.pointlistarray[6],"|",6, name, sizeof( name));
							if(atoi(fmindex)<=0)strcpy(name,"  未占领");
							sprintf(pointbuf,"%s  尼 克 斯  %3s   %3s   %s\n", pointbuf, x, y, name);
							getStringFromIndexWithDelim( fmpointlist.pointlistarray[7],"|",3, x, sizeof( x));
							getStringFromIndexWithDelim( fmpointlist.pointlistarray[7],"|",4, y, sizeof( y));
							getStringFromIndexWithDelim( fmpointlist.pointlistarray[7],"|",5, fmindex, sizeof( fmindex));
							getStringFromIndexWithDelim( fmpointlist.pointlistarray[7],"|",6, name, sizeof( name));
							if(atoi(fmindex)<=0)strcpy(name,"  未占领");
							sprintf(pointbuf,"%s  弗 列 顿  %3s   %3s   %s\n", pointbuf, x, y, name);
							getStringFromIndexWithDelim( fmpointlist.pointlistarray[8],"|",3, x, sizeof( x));
							getStringFromIndexWithDelim( fmpointlist.pointlistarray[8],"|",4, y, sizeof( y));
							getStringFromIndexWithDelim( fmpointlist.pointlistarray[8],"|",5, fmindex, sizeof( fmindex));
							getStringFromIndexWithDelim( fmpointlist.pointlistarray[8],"|",6, name, sizeof( name));
							if(atoi(fmindex)<=0)strcpy(name,"  未占领");
							sprintf(pointbuf,"%s  亚 伊 欧  %3s   %3s   %s\n", pointbuf, x, y, name);
							getStringFromIndexWithDelim( fmpointlist.pointlistarray[9],"|",3, x, sizeof( x));
							getStringFromIndexWithDelim( fmpointlist.pointlistarray[9],"|",4, y, sizeof( y));
							getStringFromIndexWithDelim( fmpointlist.pointlistarray[9],"|",5, fmindex, sizeof( fmindex));
							getStringFromIndexWithDelim( fmpointlist.pointlistarray[9],"|",6, name, sizeof( name));
							if(atoi(fmindex)<=0)strcpy(name,"  未占领");
							sprintf(pointbuf,"%s  瑞尔亚斯  %3s   %3s   %s\n", pointbuf, x, y, name);
							buttontype |= WINDOW_BUTTONTYPE_PREV;
					}else if( pointlistindex==0 ){
							getStringFromIndexWithDelim( fmpointlist.pointlistarray[0],"|",3, x, sizeof( x));
							getStringFromIndexWithDelim( fmpointlist.pointlistarray[0],"|",4, y, sizeof( y));
							getStringFromIndexWithDelim( fmpointlist.pointlistarray[0],"|",5, fmindex, sizeof( fmindex));
							getStringFromIndexWithDelim( fmpointlist.pointlistarray[0],"|",6, name, sizeof( name));
							if(atoi(fmindex)<=0)strcpy(name,"  未占领");
							sprintf(pointbuf,"%s  萨姆吉尔  %3s   %3s   %s\n", pointbuf, x, y, name);
							getStringFromIndexWithDelim( fmpointlist.pointlistarray[1],"|",3, x, sizeof( x));
							getStringFromIndexWithDelim( fmpointlist.pointlistarray[1],"|",4, y, sizeof( y));
							getStringFromIndexWithDelim( fmpointlist.pointlistarray[1],"|",5, fmindex, sizeof( fmindex));
							getStringFromIndexWithDelim( fmpointlist.pointlistarray[1],"|",6, name, sizeof( name));
							if(atoi(fmindex)<=0)strcpy(name,"  未占领");
							sprintf(pointbuf,"%s  玛丽娜斯  %3s   %3s   %s\n", pointbuf, x, y, name);
							getStringFromIndexWithDelim( fmpointlist.pointlistarray[2],"|",3, x, sizeof( x));
							getStringFromIndexWithDelim( fmpointlist.pointlistarray[2],"|",4, y, sizeof( y));
							getStringFromIndexWithDelim( fmpointlist.pointlistarray[2],"|",5, fmindex, sizeof( fmindex));
							getStringFromIndexWithDelim( fmpointlist.pointlistarray[2],"|",6, name, sizeof( name));
							if(atoi(fmindex)<=0)strcpy(name,"  未占领");
							sprintf(pointbuf,"%s  加　　加  %3s   %3s   %s\n", pointbuf, x, y, name);
							getStringFromIndexWithDelim( fmpointlist.pointlistarray[3],"|",3, x, sizeof( x));
							getStringFromIndexWithDelim( fmpointlist.pointlistarray[3],"|",4, y, sizeof( y));
							getStringFromIndexWithDelim( fmpointlist.pointlistarray[3],"|",5, fmindex, sizeof( fmindex));
							getStringFromIndexWithDelim( fmpointlist.pointlistarray[3],"|",6, name, sizeof( name));
							if(atoi(fmindex)<=0)strcpy(name,"  未占领");
							sprintf(pointbuf,"%s  卡鲁他那  %3s   %3s   %s\n", pointbuf, x, y, name);
							getStringFromIndexWithDelim( fmpointlist.pointlistarray[4],"|",3, x, sizeof( x));
							getStringFromIndexWithDelim( fmpointlist.pointlistarray[4],"|",4, y, sizeof( y));
							getStringFromIndexWithDelim( fmpointlist.pointlistarray[4],"|",5, fmindex, sizeof( fmindex));
							getStringFromIndexWithDelim( fmpointlist.pointlistarray[4],"|",6, name, sizeof( name));
							if(atoi(fmindex)<=0)strcpy(name,"  未占领");
							sprintf(pointbuf,"%s  伊　　甸  %3s   %3s   %s\n", pointbuf, x, y, name);
							buttontype |= WINDOW_BUTTONTYPE_NEXT;
					}else{
							buttontype |= WINDOW_BUTTONTYPE_PREV;
							buttontype |= WINDOW_BUTTONTYPE_NEXT;
					}
#else
					int i;
					pointlistindex += 10 * (( select == WINDOW_BUTTONTYPE_NEXT) ? 1 : -1);
					
					if( pointlistindex > FMPOINTNUM) 
						pointlistindex -= 10;
					else if( pointlistindex < 1 ) 
						pointlistindex = 0;
					
					buttontype = WINDOW_BUTTONTYPE_OK;
					if( (pointlistindex + 10) > FMPOINTNUM) 
						buttontype |= WINDOW_BUTTONTYPE_PREV;
					else if( pointlistindex==0 )
						buttontype |= WINDOW_BUTTONTYPE_NEXT;
					else{
						buttontype |= WINDOW_BUTTONTYPE_PREV;
						buttontype |= WINDOW_BUTTONTYPE_NEXT;
					}
					
					strcpy( pointbuf, fmpointlist.pointlistarray[pointlistindex]);
					strcat( pointbuf, "\n");
					for( i=(pointlistindex+1); i<pointlistindex+10; i++){
						strcat( pointbuf, fmpointlist.pointlistarray[i]);
						strcat( pointbuf, "\n");
					}
#endif
#ifdef _FIX_FM_FMPOINT
					lssproto_WN_send( fd, windowtype,
#else
					lssproto_WN_send( fd, WINDOW_FMMESSAGETYPE_POINTLIST,
#endif
						buttontype,
						CHAR_WINDOWTYPE_FM_POINTLIST,
#ifndef _FM_MODIFY
						CHAR_getWorkInt( index, CHAR_WORKOBJINDEX),
#else
						-1,
#endif
#ifndef _FIX_FM_FMPOINT
					pointbuf);	
#else
					makeEscapeString( pointbuf, buf, sizeof(buf)));
#endif
				}
				break;
			case WINDOW_BUTTONTYPE_OK:
				break;
			default:
#ifdef _FIX_FM_FMPOINT
					ApplyFamilyPoint( index, talker, atoi(data)+CHAR_getInt( talker, CHAR_LISTPAGE)*5);
#endif
				break;
			}
    }        

    // 家族强者表(自己及前十大)
    else if( seqno == CHAR_WINDOWTYPE_FM_DPME )
    {
			switch( select ){
			case WINDOW_BUTTONTYPE_PREV:
				{
					int  fd;
					
					fd = getfdFromCharaIndex( talker );
					if( fd == -1 )  return;
					
					if( NowTime.tv_sec > READTIME3 ){
						saacproto_ACShowTopFMList_send(acfd, FM_TOP_INTEGRATE);
						saacproto_ACShowTopFMList_send(acfd, FM_TOP_ADV);    
						saacproto_ACShowTopFMList_send(acfd, FM_TOP_FEED);
						saacproto_ACShowTopFMList_send(acfd, FM_TOP_SYNTHESIZE);
						saacproto_ACShowTopFMList_send(acfd, FM_TOP_DEALFOOD);
						saacproto_ACShowTopFMList_send(acfd, FM_TOP_PK);                           
						READTIME3 = NowTime.tv_sec+FM_WAITTIME;
					}
					
					strcpy( NPC_sendbuf, "\n              三十大家族声望列表\n");
					strcat( NPC_sendbuf, "              十大冒险家族\n");
					strcat( NPC_sendbuf, "              十大饲育家族\n");
					strcat( NPC_sendbuf, "              十大合成家族\n");
					strcat( NPC_sendbuf, "              十大料理家族\n");
					strcat( NPC_sendbuf, "              十大战斗家族\n");
					strcat( NPC_sendbuf, "              自己家族声望列表\n");		
					lssproto_WN_send( fd, WINDOW_MESSAGETYPE_SELECT,
						WINDOW_BUTTONTYPE_NONE,
						CHAR_WINDOWTYPE_FM_DPSELECT,
#ifndef _FM_MODIFY
						CHAR_getWorkInt( index, CHAR_WORKOBJINDEX),
#else
						-1,
#endif
						makeEscapeString( NPC_sendbuf, buf, sizeof(buf)));                
				}
				break;
			case WINDOW_BUTTONTYPE_OK:
				break;
			default:
				break;
			}
			
    }  
}

// call FmDengon NPC event
#ifndef _FM_MODIFY
void NPC_FmDengonLooked( int meindex, int lookedindex )
{
    char buf[DENGONFILEENTRYSIZE*MESSAGEINONEWINDOW*2];
    char menubuf[4096];
    int  fd;
    
    if (!CHAR_CHECKINDEX(lookedindex)) return;
    
    fd = getfdFromCharaIndex( lookedindex );
    if( fd == -1 )  return;
    
    // 必须站在布告栏的前面一格
    if( NPC_Util_CharDistance( lookedindex, meindex ) > 1) return;
    // 空白处请勿更动
    strcpy( menubuf, "                『家族布告栏』\n\n");
	strcat( menubuf, "                 家族成员列表\n");
	strcat( menubuf, "                   家族留言\n");
	strcat( menubuf, "                家族之间留言板\n");
	strcat( menubuf, "                 申请家族据点\n");
	strcat( menubuf, "                家族之间强者表");
        
    lssproto_WN_send(fd, WINDOW_MESSAGETYPE_SELECT,
        	     WINDOW_BUTTONTYPE_CANCEL,
                     CHAR_WINDOWTYPE_FM_SELECT,
 		     CHAR_getWorkInt( meindex, CHAR_WORKOBJINDEX),
		     makeEscapeString( menubuf, buf, sizeof(buf)));        
}
#else
void NPC_FmDengonLooked( int meindex, int lookedindex )
{
    char buf[DENGONFILEENTRYSIZE*MESSAGEINONEWINDOW*2];
    char menubuf[4096];
    int  fd;
    
    if (!CHAR_CHECKINDEX(lookedindex)) return;
    
    fd = getfdFromCharaIndex( lookedindex );
    if( fd == -1 )  return;
    
    // 空白处请勿更动
    strcpy( menubuf, "                『家族布告栏』\n\n");
		strcat( menubuf, "                 家族成员列表\n");
		strcat( menubuf, "                   家族留言\n");
		strcat( menubuf, "                家族之间留言板\n");
		strcat( menubuf, "                 申请家族据点\n");
		strcat( menubuf, "                家族之间强者表");
        
    lssproto_WN_send(fd,
										 WINDOW_MESSAGETYPE_SELECT,
        						 WINDOW_BUTTONTYPE_CANCEL,
                     CHAR_WINDOWTYPE_FM_SELECT,
 										 -1,
										 makeEscapeString( menubuf, buf, sizeof(buf)));        
}
#endif

#ifdef _FIX_FM_FMPOINT
void ApplyFamilyPoint( int meindex, int toindex, int select)
{
		int fd = getfdFromCharaIndex(toindex);
		if (fd == -1) return;
		char fmindex[4];
		char buf[64];
		int tkfmindex = CHAR_getWorkInt(toindex, CHAR_WORKFMINDEXI);
		int i,check=0;
		for (i=0; i<=MANORNUM-1; i++) {	// 10个庄园
       getStringFromIndexWithDelim(fmpointlist.pointlistarray[i], "|", 5, fmindex, sizeof(fmindex));
       if (tkfmindex==atoi(fmindex)-1){
       	 return;
       }
    }
    if(memberlist[tkfmindex].fmjoinnum<30)
    	check=1;
    else if(CHAR_getInt( toindex, CHAR_FAME)<300000)
    	check=2;
		getStringFromIndexWithDelim( fmpointlist.pointlistarray[select-1],"|",5, fmindex, sizeof( fmindex));
		if(atoi(fmindex)<=0 && check==0){
			saacproto_ACFixFMPoint_send(acfd,CHAR_getChar(toindex, CHAR_FMNAME),tkfmindex+1,tkfmindex,
																		CHAR_getChar(toindex, CHAR_FMNAME),tkfmindex+1,tkfmindex,select);
			sprintf(buf, "恭喜你！\n    该庄园已经是你的了。");
			lssproto_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,	WINDOW_BUTTONTYPE_OK,
	    												0,	CHAR_getWorkInt(meindex, CHAR_WORKOBJINDEX),	buf);
	  }else{
	  	if(check==0)
					sprintf(buf, "该庄园据点已有人占领了，请到该庄园的踢馆管理员处申请踢馆抢夺庄园吧！");
			else if(check==1)
					sprintf(buf, "很抱歉，申请庄园据点家族人必数需满30人！");
			else if(check==2)
					sprintf(buf, "很抱歉，申请庄园据点家族声望必需3000以上！");
			lssproto_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,	WINDOW_BUTTONTYPE_OK,
	    												0,	CHAR_getWorkInt(meindex, CHAR_WORKOBJINDEX),	buf);
	  }
}
#endif

