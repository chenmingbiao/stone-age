#include "version.h"
#include <string.h>
#include "char.h"
#include "object.h"
#include "char_base.h"
#include "npcutil.h"
#include "lssproto_serv.h"
#include "saacproto_cli.h"
#include "npc_warpman.h"
#include "map_deal.h"
#include "readmap.h"
#include "battle.h"
#include "npc_exchangeman.h"
#include "log.h"
#include "enemy.h"
#include "handletime.h"
#include "npc_eventaction.h"

static void NPC_WarpMan_selectWindow( int meindex, int toindex, int num,int select);
//ANDY_END
BOOL NPC_GetDuelPointCheck(int meindex,int talker);
BOOL NPC_PARTY_CHAECK( int meindex, int talkerindex);
void NPC_ERR_DiSP(int meindex,int talker,int errNO);

BOOL NPC_BigSmallLastCheck(int point1,int mypoint,int flg);

//BOOL NPC_ItemCheck(int meindex,int talker,int itemNo,int flg);
int NPC_FloorUse(int talker,int floor);
BOOL NPC_WarpMsg(int meindex,int talker,char *buf);
BOOL NPC_NpcWarpMsg(int meindex,int talker,char *arg);
int NPC_FloorUseOtherFloor(int charaindex, char *buf);

#define WARPMAN_STANDBY 3000
#define WARPMAN_WAIT 150
//ANDY_ADD
int CheckWarpMsg( int meindex, int talkerindex,char *npcarg ,char *TalkStr);

#ifdef _TREASURE_BOX
BOOL NPC_TreasureEventRunMsg( int meindex);
#endif

BOOL NPC_TreasureRandItemGet(int meidex,int talker,int rand_j,char *buf);

enum {
	NPC_WORK_CURRENTTIME = CHAR_NPCWORKINT1,
	NPC_TIME_BORN = CHAR_NPCWORKINT2,
	NPC_TIME_DEAD = CHAR_NPCWORKINT3,
	NPC_TIME_MODE = CHAR_NPCWORKINT4,

	NPC_TIME_EVENTMODE = CHAR_NPCWORKINT6,// 1
	NPC_TIME_EVENTTIME = CHAR_NPCWORKINT7,
	NPC_TIME_EVENONBBI = CHAR_NPCWORKINT8,
	NPC_TIME_EVENOFFDBBI = CHAR_NPCWORKINT9,
	NPC_TIME_EVENTNUM = CHAR_NPCWORKINT10,
};

enum{
	NPC_EVENTMODE_NONE,
	NPC_EVENTMODE_OPEN,
	NPC_EVENTMODE_EVENT,
};

#ifdef _NEW_WARPMAN
static void NPC_NewWarpMan_selectWindow( int meindex, int toindex, int num,int select,char *TalkStr);

enum	{
	WARP_MAN=0,
	NEW_WARPMAN,
	NEW_WARPMAN_END
	};
#endif
#define TIMECHECKLOOP 20
// Robin 0518
//BOOL checkend = FALSE;


BOOL NPC_WarpManInit( int meindex )
{
	char npcarg[NPC_UTIL_GETARGSTR_BUFSIZE - 1024 * 20];
	char buff2[256];
	char buf[1024];
	int fl,x,y;
		
	if(NPC_Util_GetArgStr( meindex, npcarg, sizeof(npcarg))==NULL){
		print("WarpMan:GetArgStrErr");
		return FALSE;
	}
   	CHAR_setInt( meindex , CHAR_WHICHTYPE , CHAR_TYPEWARPMAN );
#ifdef _NEW_WARPMAN
	if( strstr( npcarg,"NEWWARPMAN") )      {
			//timeman 功能
			CHAR_setWorkInt( meindex, NPC_TIME_MODE, 1);
			if(NPC_Util_GetStrFromStrWithDelim( npcarg, "NEWTIME", buff2, sizeof( buff2)) != NULL ){
				CHAR_setInt( meindex, CHAR_LOOPINTERVAL, 60*1000);
			}
#ifdef _TREASURE_BOX
			CHAR_setWorkInt( meindex, NPC_TIME_EVENTMODE, NPC_EVENTMODE_NONE);
			if(NPC_Util_GetStrFromStrWithDelim( npcarg, "TREASURE_BOX", buff2, sizeof( buff2)) != NULL ){
				if( strstr( buff2, "ON") != NULL ){
					CHAR_setWorkInt( meindex, NPC_TIME_EVENTMODE, NPC_EVENTMODE_EVENT);
					CHAR_setWorkInt( meindex, NPC_TIME_EVENTTIME, NowTime.tv_sec);
					if(NPC_Util_GetStrFromStrWithDelim( npcarg, "TREASURE_ONBBI", buff2, sizeof( buff2)) != NULL )
						CHAR_setWorkInt( meindex, NPC_TIME_EVENONBBI, atoi( buff2));
					if(NPC_Util_GetStrFromStrWithDelim( npcarg, "TREASURE_OFFDBBI", buff2, sizeof( buff2)) != NULL )
						CHAR_setWorkInt( meindex, NPC_TIME_EVENOFFDBBI, atoi( buff2));
					CHAR_setWorkInt( meindex, NPC_TIME_EVENTNUM, 0 );
				}
			}

#endif
			return TRUE;
	}else{
#endif
		if(NPC_Util_GetStrFromStrWithDelim( npcarg, "WARP", buf, sizeof( buf))==NULL){
			char filename[256];
			 if( NPC_Util_CheckAssignArgFile( meindex, filename) != NULL )
				 print("\n Warpman Err -> file:%s", filename);
			return FALSE;
		}
		getStringFromIndexWithDelim(buf,",",1,buff2,sizeof(buff2));
		fl=atoi(buff2);
		getStringFromIndexWithDelim(buf,",",2,buff2,sizeof(buff2));
		x=atoi(buff2);
		getStringFromIndexWithDelim(buf,",",3,buff2,sizeof(buff2));
		y=atoi(buff2);
		if( MAP_IsValidCoordinate( fl,x,y )== FALSE ){
			char filename[256];
			NPC_Util_CheckAssignArgFile( meindex, filename);
			print( "\nWarp NPC: filename:%s\n 3.Invalid warpman ERR %d %d %d" , filename, fl , x , y );
			return FALSE;
		}
#ifdef _NEW_WARPMAN
	}
#endif
	return TRUE;
}

void NPC_WarpManTalked( int meindex , int talkerindex , char *szMes ,int color )
{
	char npcarg[NPC_UTIL_GETARGSTR_BUFSIZE - 1024 * 20];
	char token[256];
	int RunType=-1;
	
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

#ifdef _NEW_WARPMAN
	if( strstr( npcarg,"NEWWARPMAN") )      {
#ifdef _TREASURE_BOX
		if( CHAR_getWorkInt( meindex, NPC_TIME_EVENTMODE) == NPC_EVENTMODE_OPEN )
			return;
#endif
		NPC_NewWarpMan_selectWindow( meindex ,talkerindex, 0, -1, szMes);
		return;
	}else	{
		if(NPC_PARTY_CHAECK( meindex, talkerindex) == FALSE)	{//有组队
			NPC_ERR_DiSP( meindex, talkerindex, 1);
			return;
		}
		
		
		RunType = CheckWarpMsg( meindex, talkerindex, npcarg , szMes);
		if( RunType !=0 )	{
			if( RunType == 1 )	{
				if( NPC_Util_GetStrFromStrWithDelim( npcarg, "nomal_msg", token, sizeof(token)) != NULL)	{
					CHAR_talkToCli( talkerindex, meindex,token,CHAR_COLORWHITE);
				}
			}
			return;
		}
	}
#else
	
	
	if(NPC_PARTY_CHAECK( meindex, talkerindex) == FALSE)	{//有组队
		NPC_ERR_DiSP( meindex, talkerindex, 1);
		return;
	}
	RunType = CheckWarpMsg( meindex, talkerindex, npcarg , szMes);
	if( RunType !=0 )	{
		if( RunType == 1 )	{
			if( NPC_Util_GetStrFromStrWithDelim( npcarg, "nomal_msg", token, sizeof(token)) != NULL)	{
				CHAR_talkToCli( talkerindex, meindex,token,CHAR_COLORWHITE);
			}
		}
		return;
	}
#endif
	
	CHAR_setWorkInt( talkerindex, CHAR_WORKSHOPRELEVANT, 0);
	CHAR_setWorkInt( talkerindex , CHAR_WORKSHOPRELEVANTTRD , 0 );
	if(strstr(npcarg,"DR")!=NULL)	{
		CHAR_setWorkInt( talkerindex , CHAR_WORKSHOPRELEVANTSEC , -1 );
		if(NPC_GetDuelPointCheck(meindex,talkerindex)==FALSE)	return;
	}else{
		NPC_WarpMan_selectWindow( meindex, talkerindex,0,-1);
	}
}

static void NPC_WarpMan_selectWindow( int meindex, int toindex, int num,int select)
{
	
	char npcarg[NPC_UTIL_GETARGSTR_BUFSIZE - 1024 * 20];
	char token[1024];
	char buf[256];
	char buf2[1024];
	char buf3[256];
	int fl=0	;
	int money=100	;
	int fd = getfdFromCharaIndex( toindex);
	char tmp[64];

	if(NPC_Util_GetArgStr( meindex, npcarg, sizeof(npcarg))==NULL){
		print("GetArgStrErr");
		CHAR_setWorkInt( toindex, CHAR_WORKSHOPRELEVANT, 0);
		return ;
	}

        if(NPC_PARTY_CHAECK( meindex, toindex) == FALSE)	{
		    NPC_ERR_DiSP( meindex, toindex, 1);
		    return;
		}


	if( CHAR_getWorkInt( toindex, CHAR_WORKSHOPRELEVANT) != 0 )	{
		return;
	}else	{
		CHAR_setWorkInt( toindex, CHAR_WORKSHOPRELEVANT, 1);
	}

	if(strstr(npcarg,"%4d")!=NULL){
		int flwork = 0;
		int work;
		NPC_Util_GetStrFromStrWithDelim( npcarg, "WARP", buf, sizeof( buf));
		getStringFromIndexWithDelim(buf,",",1,buf3,sizeof(buf3));
		work = atoi( buf3);
		fl = NPC_FloorUse( toindex, atoi(buf3));		

		if(NPC_Util_GetStrFromStrWithDelim( npcarg, "Floor", buf3, sizeof( buf3)) != NULL){
			flwork = NPC_FloorUseOtherFloor(work, buf3);
		}
		fl = fl +flwork;
	}

	NPC_Util_GetStrFromStrWithDelim( npcarg, "FREE", buf, sizeof( buf));

	if( (NPC_ActionPassCheck(meindex,toindex,buf)==TRUE) || (strstr(buf,"ALLFREE")!=NULL) ){
		if(NPC_Util_GetStrFromStrWithDelim( npcarg, "FreeMsg", buf2, sizeof( buf2))==NULL) return;
		      CONNECT_set_pass(fd,TRUE);
		      CONNECT_set_first_warp(fd,TRUE);
		if(strstr(buf2,"%4d") != NULL){
			sprintf(token,buf2,fl);
		}else{
			strcpy(token,buf2);
		}
		CHAR_setWorkInt( toindex , CHAR_WORKSHOPRELEVANTTRD , -1 );
		CHAR_setWorkInt( toindex, CHAR_WORKSHOPRELEVANT, 2);
	}else	{
		if(NPC_Util_GetStrFromStrWithDelim( npcarg, "PayMsg", buf2, sizeof( buf2)) != NULL){
		       CONNECT_set_pass(fd,FALSE);
		       CONNECT_set_first_warp(fd,FALSE);
			if(strstr(buf2,"%8d") != NULL){
				NPC_Util_GetStrFromStrWithDelim( npcarg, "MONEY", tmp, sizeof( tmp));
				if(strstr(tmp,"LV")!=NULL){
					int level;
					char buff2[32];
						
					level = CHAR_getInt(toindex,CHAR_LV);
					getStringFromIndexWithDelim(tmp,"*",2,buff2,sizeof(buff2));
					money = level*atoi(buff2);
				}else{
					money=atoi(tmp);
				}
				if(strstr(buf2,"%4d") != NULL){
					sprintf(token,buf2,fl,money);
				}else{
					sprintf(token,buf2,money);
				}
			}else if(strstr(buf2,"%4d") != NULL){
				sprintf(token,buf2,fl);
				NPC_Util_GetStrFromStrWithDelim( npcarg, "MONEY", tmp, sizeof( tmp));
				if( strstr( tmp,"-" ) != NULL){
					money=10000001;
					lssproto_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,
								WINDOW_BUTTONTYPE_OK,
								CHAR_WINDOWTYPE_WINDOWWARPMAN_MAIN, 
								CHAR_getWorkInt( meindex, CHAR_WORKOBJINDEX),
								token);
					return ;
				}
			}else{
				strcpy(token,buf2);
				NPC_Util_GetStrFromStrWithDelim( npcarg, "MONEY", tmp, sizeof( tmp));
				if( strstr( tmp,"-" ) != NULL){
					money=10000001;
					lssproto_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,
								WINDOW_BUTTONTYPE_OK,
								CHAR_WINDOWTYPE_WINDOWWARPMAN_MAIN, 
								CHAR_getWorkInt( meindex, CHAR_WORKOBJINDEX),
								token);
					return ;
				}else{
					money = atoi(tmp);
				}
			}
			if(CHAR_getInt(toindex,CHAR_FLOOR)==117){
			   money=CHAR_getInt(toindex,CHAR_GOLD);
			   if (money>=10) money*=0.9; 
			   else if( money == 0 ) {
				   CHAR_warpToSpecificPoint(toindex,117,225,13);
				   return;
			   }
			}
			CHAR_setWorkInt( toindex , CHAR_WORKSHOPRELEVANTTRD , money );
			CONNECT_set_pass(fd,TRUE);
			CONNECT_set_first_warp(fd,TRUE);
		
		}else if(NPC_Util_GetStrFromStrWithDelim( npcarg, "NomalMsg", token,sizeof(token)) !=NULL){
			CHAR_setWorkInt( toindex , CHAR_WORKSHOPRELEVANTTRD , -2 );
			CHAR_setWorkInt( toindex, CHAR_WORKSHOPRELEVANT, -1);
			lssproto_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,
								WINDOW_BUTTONTYPE_OK,
								CHAR_WINDOWTYPE_WINDOWWARPMAN_MAIN, 
								CHAR_getWorkInt( meindex, CHAR_WORKOBJINDEX),
								token);
		}
	}
	lssproto_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,
						WINDOW_BUTTONTYPE_YESNO,
						CHAR_WINDOWTYPE_WINDOWWARPMAN_MAIN, 
						CHAR_getWorkInt( meindex, CHAR_WORKOBJINDEX),
						token);
}

void NPC_WarpManWindowTalked( int meindex, int talkerindex, 
								int seqno, int select, char *data)
{
	char npcarg[NPC_UTIL_GETARGSTR_BUFSIZE - 1024 * 20];
	char buf[NPC_UTIL_GETARGSTR_BUFSIZE - 1024 * 20];
#ifdef _NEW_WARPMAN
	BOOL checkp=TRUE;
	char buf1[1024];
	int talkNo = 1;
	BOOL tenflg=FALSE;
#else
	int fl,x,y;
#endif
	char buff2[256];
	int fd = getfdFromCharaIndex( talkerindex );
	if( NPC_Util_CharDistance( talkerindex, meindex ) > 2)
		return;
	if(NPC_Util_GetArgStr( meindex, npcarg, sizeof(npcarg))==NULL){
		print("GetArgStrErr");
		return ;
	}
	if( CHAR_getWorkInt( talkerindex, CHAR_WORKSHOPRELEVANT) < 1 )	{
		CHAR_setWorkInt( talkerindex, CHAR_WORKSHOPRELEVANT, -1);
		return;
	}

#ifdef _NEW_WARPMAN
	if( seqno == NEW_WARPMAN )    {
		int eqnum=0;
		eqnum = CHAR_getWorkInt( talkerindex, CHAR_WORKSHOPRELEVANT);
		sprintf(buf1,"%s", "NOFREE");
		if( eqnum >= 0 )	{
			sprintf(buf1,"TALKEVENT%d", eqnum);
		}
		CHAR_setWorkInt( talkerindex, CHAR_WORKSHOPRELEVANT, 0);
		while( getStringFromIndexWithDelim( npcarg,"OVER",talkNo,buf,sizeof( buf)) != FALSE){
			if( strstr(buf, buf1) != NULL)	{
				strcpy( npcarg , buf);
				tenflg = TRUE;
				break;
			}
			talkNo++;
		}
		if( tenflg == FALSE )	{
			CHAR_setWorkInt( talkerindex, CHAR_WORKSHOPRELEVANT, -1);
			return;
		}
	}else {
#endif    
		if( select != WINDOW_BUTTONTYPE_YES )	{
			CHAR_setWorkInt( talkerindex, CHAR_WORKSHOPRELEVANT, -1);
			return;
		}
		    if(NPC_PARTY_CHAECK( meindex, talkerindex) == FALSE){
			    NPC_ERR_DiSP( meindex, talkerindex, 1);
			    return;
			}
#ifdef _NEW_WARPMAN	
	}
#else
	NPC_Util_GetStrFromStrWithDelim( npcarg, "WARP", buf, sizeof( buf));
	GetRand_WarpPoint( buf, &fl, &x, &y );
#endif

#ifdef _NEW_WARPMAN	
  switch( seqno)	{
  case WARP_MAN:
	if( select != WINDOW_BUTTONTYPE_YES )
		return;
	    if( NPC_PARTY_CHAECK( meindex, talkerindex) == FALSE ){
		    NPC_ERR_DiSP( meindex, talkerindex, 1);
		    return;
		}		
#else
	if( select != WINDOW_BUTTONTYPE_YES) return;
	    if( NPC_PARTY_CHAECK( meindex, talkerindex) == FALSE)
		    return;
#endif

#ifdef _NEW_WARPMAN
  break;
  case NEW_WARPMAN:
		if( select != WINDOW_BUTTONTYPE_YES ){
			CHAR_setWorkInt( talkerindex, CHAR_WORKSHOPRELEVANT, -1);
			if( NPC_Util_GetStrFromStrWithDelim( npcarg, "CancelMsg", buff2, sizeof( buff2) ) != NULL)	{
				CHAR_talkToCli( talkerindex, meindex, buff2, CHAR_COLORYELLOW);
			}
			return;
		}
	  checkp=TRUE;
	  if( NPC_Util_GetStrFromStrWithDelim( npcarg, "CHECKPARTY", buff2, sizeof( buff2) ) != NULL)	{
		  if( strstr( buff2, "FALSE") != NULL )	{
			checkp = FALSE;
		}
	  }
	  if( checkp==TRUE && NPC_PARTY_CHAECK( meindex, talkerindex) == FALSE )	{
		      NPC_ERR_DiSP( meindex, talkerindex, 1);
		      CHAR_setWorkInt( talkerindex, CHAR_WORKSHOPRELEVANT, -1);
		      return;
	  }		
  break;
  case NEW_WARPMAN_END:
	  return;
  break;
  }
#endif

	if( Action_RunDoEventAction( meindex, talkerindex, npcarg) == FALSE ){
		CHAR_setWorkInt( talkerindex, CHAR_WORKSHOPRELEVANT, -1);
		return;
	}

		CHAR_complianceParameter( talkerindex );
		CHAR_send_P_StatusString( talkerindex ,
						CHAR_P_STRING_ATK|CHAR_P_STRING_DEF|
						CHAR_P_STRING_CHARM|CHAR_P_STRING_QUICK|
						CHAR_P_STRING_WATER|CHAR_P_STRING_FIRE|
						CHAR_P_STRING_WIND|CHAR_P_STRING_EARTH
						);
		if( CHAR_getWorkInt( CONNECT_getCharaindex(fd), CHAR_WORKBATTLEMODE) != BATTLE_CHARMODE_NONE){
	            return ;
		}
		if(CHAR_getWorkInt( talkerindex , CHAR_WORKSHOPRELEVANTTRD) <= 0){
				CHAR_setWorkInt( talkerindex, CHAR_WORKWARPCHECK, FALSE );
#ifdef _NEW_WARPMAN	
				NPC_WarpMsg( meindex, talkerindex, npcarg);
#else
				if( fl == 0 && x == 0 && y == 0 )		{
				}else	{
					CHAR_warpToSpecificPoint(talkerindex, fl, x,y);
				}
#endif
		}else	{
			if(CHAR_getInt(talkerindex,CHAR_GOLD) <
	                  CHAR_getWorkInt(talkerindex,CHAR_WORKSHOPRELEVANTTRD)){
				NPC_ERR_DiSP( meindex, talkerindex, 2);
				return ;
			}
			CHAR_DelGold( talkerindex, CHAR_getWorkInt(talkerindex,CHAR_WORKSHOPRELEVANTTRD ));

				CHAR_setWorkInt( talkerindex, CHAR_WORKWARPCHECK, FALSE );
#ifdef _NEW_WARPMAN	
				NPC_WarpMsg( meindex, talkerindex, npcarg);
#else
				if( fl == 0 && x == 0 && y == 0 )		{
				}else	{
                        CHAR_warpToSpecificPoint(talkerindex, fl, x,y);
				}
#endif

		}
#ifdef _NEW_WARPMAN
		NPC_NpcWarpMsg( meindex, talkerindex, npcarg);
#else
	}
#endif
}

BOOL NPC_BigSmallLastCheck(int point1,int mypoint,int flg)
{
	if(flg==0){
		if(point1==mypoint) {
			return TRUE;
		}
		
	}else if(flg==1){
		if(mypoint < point1) {
			return TRUE;
		}
		
	}else if(flg==2){
		if(mypoint > point1) {
			return TRUE;
		}
	}

	return FALSE;

}

BOOL NPC_GetDuelPointCheck(int meindex,int talker)
{

	int fdid = getFdidFromCharaIndex( talker);
	char dbkey[256];
	
	if( fdid == -1 ) return FALSE;

	CHAR_makeDBKey( talker, dbkey, sizeof( dbkey));
	saacproto_DBGetEntryRank_send( acfd, DB_DUELPOINT, dbkey, fdid,
							CHAR_getWorkInt( meindex, CHAR_WORKOBJINDEX));
	
	return TRUE;

}

void NPC_GetDuelRank(int rank,int fdid,int objindex)
{

	int talker;
	int meindex;


	rank++;
   	talker = getCharindexFromFdid( fdid);
	if( talker == -1 ) return;

	meindex= OBJECT_getIndex( objindex);
	if( !CHAR_CHECKINDEX( meindex)) return;

	CHAR_setWorkInt(talker,CHAR_WORKSHOPRELEVANTSEC,rank);

	NPC_WarpMan_selectWindow( meindex, talker,0,-1);

}

BOOL NPC_PARTY_CHAECK(int meindex,int talker)
{
	if(CHAR_getWorkInt(talker,CHAR_WORKPARTYMODE) != CHAR_PARTY_NONE){
		return FALSE;
	}
	return TRUE;
}

void NPC_ERR_DiSP(int meindex,int talker,int errNO)
{
	char token[1024];
	int i=0;
	int otherindex;
	int fd = getfdFromCharaIndex( talker);
	char npcarg[NPC_UTIL_GETARGSTR_BUFSIZE - 1024 * 20];
	
	if(NPC_Util_GetArgStr( meindex, npcarg, sizeof(npcarg))==NULL){
		print("GetArgStrErr");
		return ;
	}	
	if(errNO==1){
		if(NPC_Util_GetStrFromStrWithDelim( npcarg, "PartyMsg",token, sizeof( token))==NULL) {
			sprintf(token,"\n\n　　　　无法加入团队。　　"
				"\n\n　　　 请解散团队 。　");
		}
		if(CHAR_getWorkInt(talker,CHAR_WORKPARTYMODE)==CHAR_PARTY_CLIENT){			
		}else{	
			
			for( i=0 ; i < CHAR_PARTYMAX ;i++){
				otherindex=CHAR_getWorkInt(talker,CHAR_WORKPARTYINDEX1+i);
				if(otherindex != -1){
					fd = getfdFromCharaIndex( otherindex);
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
		if(NPC_Util_GetStrFromStrWithDelim( npcarg, "MoneyMsg", token, sizeof( token))==NULL){
			sprintf(token,"\n\n　　　　似乎金钱不足唷。　　"
				"\n\n　请存好钱後，再过来。");
		}
	}
	lssproto_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,
		WINDOW_BUTTONTYPE_OK,
		CHAR_WINDOWTYPE_WINDOWWARPMAN_ERR, 
		CHAR_getWorkInt( meindex, CHAR_WORKOBJINDEX),
		token);
}

int NPC_FloorUse(int charaindex,int floor)
{
	int		i;
	int		players=0;
	int     playernum = CHAR_getPlayerMaxNum();
	for( i=0 ; i< playernum ; i++ ){
		if( CHAR_getCharUse(i) == FALSE )continue;
			if(CHAR_getInt(i,CHAR_FLOOR)==floor){
				players++;
			}
	}
	return players;
}

int NPC_FloorUseOtherFloor(int warp, char *buf)
{
	int		i,j=1;
	int		players=0;
	int     playernum = CHAR_getPlayerMaxNum();
	char buf2[32];
	int floor;

	for( i=0 ; i< playernum ; i++ ){
		if( CHAR_getCharUse(i) == FALSE )continue;
		j=1;
		while(getStringFromIndexWithDelim(buf,",",j,buf2,sizeof(buf2))!= FALSE){
			j++;
			floor = atoi(buf2);
			if(CHAR_getInt(i,CHAR_FLOOR) == floor && warp != floor) {
				players++;
			}
		}
	}
	return players;
}

BOOL NPC_WarpMsg(int meindex,int talker,char *arg)
{
	char buf[256];	
	int fl=0,x=0,y=0;
	int parent=-1;
	int pmode;
	int subindex;
	int i;

	if( NPC_Util_GetStrFromStrWithDelim( arg, "WARP", buf, sizeof( buf)) == NULL )
		return FALSE;
	GetRand_WarpPoint( buf, &fl, &x, &y );
	if( fl == 0 && x == 0 && y == 0 )	{
		return TRUE;
	}else	{
		if( MAP_IsValidCoordinate( fl,x,y )== FALSE ){
			print( "Warp NPC:1.Invalid warpman ERR" );
			return FALSE;
		}
	}
#ifdef _TIME_TICKET
	if( check_TimeTicketMap( fl) ) {
		int havei, itemi;
		CHAR_setWorkInt( talker, CHAR_WORKTICKETTIME, 0);
		CHAR_setWorkInt( talker, CHAR_WORKTICKETTIMESTART, 0);
		for( havei = CHAR_STARTITEMARRAY ; havei < CHAR_MAXITEMHAVE ; havei++ ){
			itemi = CHAR_getItemIndex( talker, havei);
			if( !ITEM_CHECKINDEX( itemi) ) continue;
			if( strcmp( ITEM_getChar( itemi, ITEM_USEFUNC), "ITEM_timeticket") )	continue;
			//if( ITEM_getInt( itemi ,ITEM_ID) != 20646 )	continue;
			CHAR_talkToCli( talker, -1, "自动使用门票。", CHAR_COLORYELLOW);
			ITEM_timeticketEx( talker, talker, havei, 1);
			break;
		}
		if( havei == CHAR_MAXITEMHAVE ) {
			CHAR_talkToCli( talker, -1, "请先准备专用门票才可进入。", CHAR_COLORYELLOW);
			return FALSE;
		}
	}
#endif
	pmode = CHAR_getWorkInt( talker, CHAR_WORKPARTYMODE );
	switch( pmode ){
		case 1: 
			parent = talker;
			break;
		case 2:
			parent = CHAR_getWorkInt( talker, CHAR_WORKPARTYINDEX1 );
			break;
		default:
			CHAR_warpToSpecificPoint(talker, fl, x,y);
			return TRUE;
	}
	if( parent < 0 )
		return TRUE;
	for( i = 0; i < CHAR_PARTYMAX; i ++ ){
		subindex = CHAR_getWorkInt( parent, CHAR_WORKPARTYINDEX1+i );
		if( CHAR_CHECKINDEX( subindex ) == FALSE )continue;
		if( fl==0 && x==0 && y==0 )	{
		}else	{
			CHAR_warpToSpecificPoint( subindex, fl, x, y );
		}
	}
	return TRUE;
}

#ifdef _NEW_WARPMAN
static void NPC_NewWarpMan_selectWindow( int meindex, int toindex, int num,int select,char *TalkStr)
{	
	char npcarg[NPC_UTIL_GETARGSTR_BUFSIZE - 1024 * 20];
	char token[1024];
	int buttontype = 0, windowtype = 0, windowno = 0;
	char buf[NPC_UTIL_GETARGSTR_BUFSIZE - 1024 * 20],buf1[256];
	int fd = getfdFromCharaIndex( toindex);
	BOOL tenflg =FALSE;
	int talkNo=1,RunType=-1;

	if(NPC_Util_GetArgStr( meindex, npcarg, sizeof(npcarg))==NULL){
		print("GetArgStrErr");
		return ;
	}

	if( NPC_Util_GetStrFromStrWithDelim( npcarg, "NEWTIME", buf1, sizeof( buf1)) != NULL ){
		if( CHAR_getWorkInt( meindex, NPC_TIME_MODE) <= 0 )	{
			if( NPC_Util_GetStrFromStrWithDelim( npcarg, "Time_Msg", token, sizeof( token) ) != NULL)	{
				CHAR_talkToCli( toindex, meindex, token, CHAR_COLORYELLOW);
			}
			return;
		}
	}

	windowtype = WINDOW_MESSAGETYPE_MESSAGE;
	sprintf( buf1,"TALKEVENT");
	//寻找多种条件 FREE
	while(getStringFromIndexWithDelim( npcarg,"OVER",talkNo,buf,sizeof( buf)) != FALSE)	{ 
	  if(strstr(buf, buf1) != NULL) {
	    //检查对话  暗语
		RunType = CheckWarpMsg( meindex, toindex, buf , TalkStr);
		if( RunType > 1 )	{
			return;
		}else if( RunType != 0 )	{
			talkNo++;
			continue;
		}
	    if( NPC_Util_GetStrFromStrWithDelim( buf, "FREE", token, sizeof( token) ) == NULL)	{
	      print("msgErr");
	      return;
	    }

	    //检查玩家是否附合条件		
	    if((NPC_ActionPassCheck(meindex,toindex,token)==TRUE) || (strstr( token, "ALLFREE")!=NULL) )	{
		  CHAR_setWorkInt( toindex, CHAR_WORKSHOPRELEVANT, talkNo-1);

	      //若附合条件 取出 FreeMsg 字串
	      if(NPC_Util_GetStrFromStrWithDelim( buf, "FreeMsg", token, sizeof( token))==NULL)
	        return;
	      CONNECT_set_pass(fd,TRUE);
	      CONNECT_set_first_warp(fd,TRUE);
	      buttontype = WINDOW_BUTTONTYPE_YESNO;
	      windowno = NEW_WARPMAN;
	      tenflg = TRUE;
	      CHAR_setWorkInt( toindex , CHAR_WORKSHOPRELEVANTTRD , -1 );
	      break;									
	    }
	  }
	  talkNo++;
	}

	//无任何条件成立
	if( tenflg == FALSE )	{
		if( getStringFromIndexWithDelim( npcarg,"OVER",1,buf,sizeof( buf)) == FALSE )
			return;
		if( strstr( buf, "NOFREE") == NULL )	{
			return;
		}else	{
			strcpy( npcarg, buf);
		}
		if(NPC_Util_GetStrFromStrWithDelim( npcarg, "NomalMsg", token,sizeof(token)) != NULL)	{
			CHAR_talkToCli( toindex, meindex, token,CHAR_COLORWHITE);
			return;
		}
	}

	//送讯息给 CLI						
	lssproto_WN_send( fd, windowtype, buttontype, windowno,
			CHAR_getWorkInt( meindex, CHAR_WORKOBJINDEX), token	);																				
}

#endif
//ANDY_RE
int CheckWarpMsg( int meindex, int talkerindex,char *npcarg ,char *TalkStr)
{
  char buf[1024];

  if( NPC_Util_GetStrFromStrWithDelim( npcarg, "warp_msg", buf, sizeof( buf) ) == NULL)
	  return 0;
  if( strcmp( buf, TalkStr) )
	  return 1;

  if( NPC_Util_GetStrFromStrWithDelim( npcarg, "CHECKPARTY", buf, sizeof( buf) ) != NULL)	{
	  if( strstr( buf, "FALSE") != NULL )	{
	  }else {
	  if( NPC_PARTY_CHAECK( meindex, talkerindex) == FALSE )	{
		  NPC_ERR_DiSP( meindex, talkerindex, 1);
		  return 2;
	  }		
	}
  }

   if( NPC_Util_GetStrFromStrWithDelim( npcarg, "FREE", buf, sizeof( buf)) == NULL )
	   return 2;
   if( ( NPC_ActionPassCheck( meindex, talkerindex, buf ) == FALSE) )	{
	   	if(NPC_Util_GetStrFromStrWithDelim( npcarg, "NomalMsg", buf,sizeof(buf)) != NULL)	{
			CHAR_talkToCli( talkerindex, meindex,buf,CHAR_COLORWHITE);
		}
		return	2;
   }else   {
		if( Action_RunDoEventAction( meindex, talkerindex, npcarg) == FALSE ){

			CHAR_setWorkInt( talkerindex, CHAR_WORKSHOPRELEVANT, -1);
			return 3;
		}
		CHAR_complianceParameter( talkerindex );
		CHAR_send_P_StatusString( talkerindex ,
			CHAR_P_STRING_ATK|CHAR_P_STRING_DEF|
			CHAR_P_STRING_CHARM|CHAR_P_STRING_QUICK|
			CHAR_P_STRING_WATER|CHAR_P_STRING_FIRE|
			CHAR_P_STRING_WIND|CHAR_P_STRING_EARTH	);

		if(NPC_Util_GetStrFromStrWithDelim( npcarg, "FreeMsg", buf, sizeof( buf)) != NULL ){
			CHAR_talkToCli( talkerindex, meindex, buf,  CHAR_COLORWHITE);
		}

		NPC_WarpMsg( meindex, talkerindex, npcarg);
		NPC_NpcWarpMsg( meindex, talkerindex, npcarg);
		return 4;
	}
  return 0;
}

BOOL NPC_NpcWarpMsg(int meindex,int talker,char *arg)
{
	char buf[256];	
	int fl=0,x=0,y=0;
	
	if( CHAR_CHECKINDEX( meindex ) == FALSE )
		return FALSE;

	if( NPC_Util_GetStrFromStrWithDelim( arg, "NPCPOINT", buf, sizeof( buf)) == NULL )
		return FALSE;
	GetRand_WarpPoint( buf, &fl, &x, &y );

	if( MAP_IsValidCoordinate( fl,x,y )== FALSE ){
		print( "Warp NPC:2.Invalid warpman ERR" );
		return FALSE;
	}

	CHAR_warpToSpecificPoint( meindex, fl, x, y );
	
	return TRUE;
}

void NPC_WarpManLoop( int meindex)
{
	int born, dead;
	char npcarg[NPC_UTIL_GETARGSTR_BUFSIZE - 1024 * 20];
	if( !CHAR_CHECKINDEX( meindex) ) return;

	if( NPC_Util_GetArgStr( meindex, npcarg, sizeof(npcarg)) == NULL ){
		print("WarpMan:GetArgStrErr");
		return;
	}
	if( !strstr( npcarg,"NEWWARPMAN") )
		return;

	if( NPC_getTimeData( meindex, npcarg, &born, &dead, 200) != FALSE ){
		NPC_TimeDefineDo( meindex, born, dead, NPC_TIME_MODE);
		CHAR_sendCToArroundCharacter( CHAR_getWorkInt( meindex, CHAR_WORKOBJINDEX ) );
	}
}

void NPC_WarpManWatch( int meobjindex, int objindex, CHAR_ACTION act,
					  int x,int y,int dir, int* opt,int optlen )
{
	int born, dead;
	int meindex, index;

	char npcarg[NPC_UTIL_GETARGSTR_BUFSIZE - 1024 * 20];

	if( OBJECT_getType( objindex) != OBJTYPE_CHARA) return;
	index = OBJECT_getIndex( objindex);
	if( !CHAR_CHECKINDEX( index) ) return;
	if( CHAR_getInt( index, CHAR_WHICHTYPE) != CHAR_TYPEPLAYER) return;
    meindex = OBJECT_getIndex( meobjindex);
	if( !CHAR_CHECKINDEX( meindex) ) return;

	if( NPC_Util_GetArgStr( meindex, npcarg, sizeof(npcarg)) == NULL ){
		print("WarpMan:GetArgStrErr");
		return;
	}
	if( !strstr( npcarg,"NEWWARPMAN") )
		return;
#ifdef _TREASURE_BOX
	if( CHAR_getWorkInt( meindex, NPC_TIME_EVENTMODE) == NPC_EVENTMODE_OPEN ){
		if( CHAR_getWorkInt( meindex, NPC_TIME_EVENTTIME) < NowTime.tv_sec ){
			//变回宝箱
			CHAR_setWorkInt( meindex, NPC_TIME_EVENTMODE, NPC_EVENTMODE_EVENT);
			//变图
			CHAR_setInt( meindex, CHAR_BASEBASEIMAGENUMBER,
				CHAR_getWorkInt( meindex, NPC_TIME_EVENOFFDBBI));
			CHAR_setInt( meindex, CHAR_BASEIMAGENUMBER,
				CHAR_getWorkInt( meindex, NPC_TIME_EVENOFFDBBI));
			NPC_TreasureEventRunMsg( meindex);	//是否warp
			CHAR_sendCToArroundCharacter( CHAR_getWorkInt( meindex, CHAR_WORKOBJINDEX ) );
		}
	}
#endif
	if( NPC_getTimeData( meindex, npcarg, &born, &dead, 200) != FALSE ){
		NPC_TimeDefineDo( meindex, born, dead, NPC_TIME_MODE);
		CHAR_sendCToArroundCharacter( CHAR_getWorkInt( meindex, CHAR_WORKOBJINDEX ) );
	}
}

#ifdef _TREASURE_BOX
BOOL NPC_TreasureEventRunMsg( int meindex)
{
	char buf[256];	
	char npcarg[NPC_UTIL_GETARGSTR_BUFSIZE - 1024 * 20];

	int fl,x,y;
	
	if( !CHAR_CHECKINDEX( meindex ))
		return FALSE;


	if(NPC_Util_GetArgStr( meindex, npcarg, sizeof(npcarg))==NULL){
		print("WarpMan:GetArgStrErr");
		return FALSE;
	}	

	if( NPC_Util_GetStrFromStrWithDelim( npcarg, "TREASURE_NUM", buf, sizeof( buf)) == NULL )
		return FALSE;

	if(  CHAR_getWorkInt( meindex, NPC_TIME_EVENTNUM) < atoi( buf) ) return FALSE;
	CHAR_setWorkInt( meindex, NPC_TIME_EVENTNUM, 0);
	
	if( NPC_Util_GetStrFromStrWithDelim( npcarg, "TREASURE_POINT", buf, sizeof( buf)) == NULL )
		return FALSE;
	GetRand_WarpPoint( buf, &fl, &x, &y );

	if( MAP_IsValidCoordinate( fl,x,y )== FALSE ){
		print( "Warp NPC:2.Invalid warpman ERR" );
		return FALSE;
	}

	CHAR_warpToSpecificPoint( meindex, fl, x, y );
	return TRUE;
}

#endif

BOOL NPC_TreasureRandItemGet(int meidex,int talker,int rand_j,char *buf)
{
	char buff2[64];
	int randitem;
	int ret;
	int itemindex;
	char token[128];

	if(rand_j == 0) {
		print("Event:由於０的介入，出现错误。");
 		return FALSE;
 	}
 	
	randitem = rand()%rand_j;
	if(randitem == 0) randitem = rand_j;

	getStringFromIndexWithDelim(buf , "," , randitem, buff2, sizeof(buff2)) ;

	itemindex = ITEM_makeItemAndRegist( atoi( buff2));

	if(itemindex == -1) return FALSE;
	
	/*失奶  丞及馨笛(  涛失奶  丞  卞中木化仄引丹  */
	ret = CHAR_addItemSpecificItemIndex( talker, itemindex);
	if( !CHAR_CHECKITEMINDEX( talker, ret) ){
		print( "npc_exchangeman.c: additem error itemindex[%d]\n", itemindex);
		ITEM_endExistItemsOne( itemindex);
		return FALSE;
	}

	if(itemindex != -1) {
		LogItem(
			CHAR_getChar( talker, CHAR_NAME ), /* 平乓仿   */
			CHAR_getChar( talker, CHAR_CDKEY ),
#ifdef _add_item_log_name  // WON ADD 在item的log中增加item名称
			itemindex,
#else
			ITEM_getInt( itemindex, ITEM_ID),  /* 失奶  丞  寞 */
#endif
			"EventAddItem(任务需求所得到的道具)",
			CHAR_getInt( talker,CHAR_FLOOR),
			CHAR_getInt( talker,CHAR_X ),
 			CHAR_getInt( talker,CHAR_Y ),
            ITEM_getChar( itemindex, ITEM_UNIQUECODE),
			ITEM_getChar( itemindex, ITEM_NAME),
			ITEM_getInt( itemindex, ITEM_ID)
		);
	}
					
	sprintf(token,"收下了%s",ITEM_getChar( itemindex, ITEM_NAME));
	CHAR_talkToCli( talker, -1, token, CHAR_COLORWHITE);

	CHAR_sendItemDataOne( talker, ret);
	return TRUE;

}
