#include "version.h"
#include <string.h>
#include "char.h"
#include "object.h"
#include "char_base.h"
#include "npcutil.h"
#include "lssproto_serv.h"
#include "saacproto_cli.h"
#include "map_deal.h"
#include "readmap.h"
#include "battle.h"
#include "log.h"
#include "enemy.h"
#include "handletime.h"
#include "npc_eventaction.h"
#include "family.h"

enum{
	NPC_EVENTMODE_NONE,
	NPC_EVENTMODE_OPEN,
	NPC_EVENTMODE_EVENT,
};

typedef struct {
	char	arg[32];
	int		born;
	int 	dead;
}NPC_TimeMan;

#define YOAKE 700
#define NICHIBOTU 300
#define SHOUGO 125
#define YONAKA 500

static NPC_TimeMan	TimeTble[] = {
	{"ALLNIGHT",	NICHIBOTU+1,	YOAKE},
	{"ALLNOON",		YOAKE+1,		NICHIBOTU},
	{"AM",			YONAKA+1,		SHOUGO},
	{"PM",			SHOUGO+1,		YONAKA},
	{"FORE",		YOAKE+1,		SHOUGO},
	{"AFTER",		SHOUGO+1,		NICHIBOTU},
	{"EVNING",		NICHIBOTU+1,	YONAKA},
	{"MORNING",		YONAKA+1,		YOAKE},
	{"FREE",		0,				1024},
};

extern void BATTLE_changeRideImage( int index );


#define MAXNPCPOINT 10

BOOL ActionNpc_WarpPoint(int meindex,int talker,char *npcarg)
{
	char data[1024], buf1[256], buf2[256];
	int i=1, j=0;
	BOOL EvFlg=FALSE;

	struct	{
		int FLOOR;
		int X;
		int Y;
	}Points[MAXNPCPOINT];
	
	for( i=0;i<MAXNPCPOINT;i++)	{
		Points[i].FLOOR = -1;
		Points[i].X = -1;
		Points[i].Y = -1;
	}
	//NPC_POINT
	if( NPC_Util_GetStrFromStrWithDelim( npcarg, "NPC_POINT", data, sizeof( data)) != NULL ) {
		int where=0;
		i=1;
		j=0;
		while( getStringFromIndexWithDelim( data, ";", i, buf1, sizeof( buf1)) != FALSE )	{
			i++;
			getStringFromIndexWithDelim( buf1, ",", 1, buf2, sizeof( buf2));
			Points[j].FLOOR = atoi( buf2);
			getStringFromIndexWithDelim( buf1, ",", 2, buf2, sizeof( buf2));
			Points[j].X = atoi( buf2);
			getStringFromIndexWithDelim( buf1, ",", 3, buf2, sizeof( buf2));
			Points[j].Y = atoi( buf2);
			j++;
			if( j >= MAXNPCPOINT )
				break;
		}
		where = RAND( 0, j-1);
		if( MAP_IsValidCoordinate( Points[where].FLOOR,	Points[where].X, Points[where].Y ) == TRUE ){
			CHAR_warpToSpecificPoint( meindex, Points[where].FLOOR,	Points[where].X, Points[where].Y);
			EvFlg = TRUE;
		}else	{
			for( i=0; i<10; i++)	{
				print("%d.[%d,%d,%d]\n", i, Points[i].FLOOR, Points[i].X, Points[i].Y);
			}
		}
	}

	if( NPC_Util_GetStrFromStrWithDelim( npcarg, "WARPPOINT", data, sizeof( data)) != NULL ) {
		int where=0;
		i=1;
		j=0;
		while( getStringFromIndexWithDelim( data, ";", i, buf1, sizeof( buf1)) != FALSE )	{
			i++;
			getStringFromIndexWithDelim( buf1, ",", 1, buf2, sizeof( buf2));
			Points[j].FLOOR = atoi( buf2);
			getStringFromIndexWithDelim( buf1, ",", 2, buf2, sizeof( buf2));
			Points[j].X = atoi( buf2);
			getStringFromIndexWithDelim( buf1, ",", 3, buf2, sizeof( buf2));
			Points[j].Y = atoi( buf2);
			j++;
			if( j >= MAXNPCPOINT )
				break;
		}
		where = RAND( 0, j-1);
		if( MAP_IsValidCoordinate( Points[where].FLOOR,	Points[where].X, Points[where].Y ) == TRUE ){
			CHAR_warpToSpecificPoint( talker, Points[where].FLOOR,	Points[where].X, Points[where].Y);
			EvFlg = TRUE;
		}else	{
			for( i=0; i<10; i++)	{
				print("%d.[%d,%d,%d]\n", i, Points[i].FLOOR, Points[i].X, Points[i].Y);
			}
		}

	}

	return EvFlg;
}

BOOL ActionNpc_CheckMenuFree( int meindex, int toindex, char *npcarg, int nums)
{
	char buf1[1024], buf2[256];
	if( nums < 1 ) return FALSE;

	if( NPC_Util_GetStrFromStrWithDelim( npcarg, "CHECKPARTY", buf1, sizeof( buf1) ) != NULL){
		if( strstr( buf1, "TRUE") != 0 )	{
			if( Action_PartyCheck( meindex, toindex) == FALSE)	{
				CHAR_talkToCli( toindex, meindex, "请一个一个来！", CHAR_COLORYELLOW);
				return FALSE;
			}
		}
	}
	if( getStringFromIndexWithDelim( npcarg,"}", nums, buf1, sizeof( buf1)) == FALSE )
		return FALSE;

	if( NPC_Util_GetStrFromStrWithDelim( buf1, "FREE", buf2, sizeof( buf2) ) == NULL)
#ifdef _VIP_SHOP
		if( NPC_Util_GetStrFromStrWithDelim( buf1, "VipItem", buf2, sizeof( buf2) ) == NULL)
			if(NPC_Util_GetStrFromStrWithDelim( buf1, "VipPet", buf2, sizeof( buf2) ) == NULL)
#endif
			return FALSE;


	if( NPC_ActionPassCheck( meindex, toindex, buf2) == FALSE )	{
#ifdef _VIP_SHOP
		if( NPC_Util_GetStrFromStrWithDelim( buf1, "VipItem", buf2, sizeof( buf2) ) == NULL)
			  if( NPC_Util_GetStrFromStrWithDelim( buf1, "VipPet", buf2, sizeof( buf2) ) == NULL)
#endif
		{
			CHAR_talkToCli( toindex, meindex, "条件不足！", CHAR_COLORYELLOW);
			return FALSE;
		}
	}
	return Action_RunDoEventAction( meindex, toindex, buf1);
}

BOOL ActionNpc_CheckFree( int meindex, int toindex, char *npcarg)
{
	char buf1[1024], buf2[256];
	if( NPC_Util_GetStrFromStrWithDelim( npcarg, "CHECKPARTY", buf1, sizeof( buf1) ) != NULL){
		if( strstr( buf1, "TRUE") != 0 )	{
			if( Action_PartyCheck( meindex, toindex) == FALSE)	{
				CHAR_talkToCli( toindex, meindex, "请一个一个来！", CHAR_COLORYELLOW);
				return FALSE;
			}
		}
	}
	if( NPC_Util_GetStrFromStrWithDelim( npcarg, "FREE", buf2, sizeof( buf2) ) == NULL)
#ifdef _VIP_SHOP
		if( NPC_Util_GetStrFromStrWithDelim( buf1, "VipItem", buf2, sizeof( buf2) ) == NULL)
			if( NPC_Util_GetStrFromStrWithDelim( buf1, "VipPet", buf2, sizeof( buf2) ) == NULL)
#endif
			return FALSE;

	if( NPC_ActionPassCheck( meindex, toindex, buf2) == FALSE )	{
#ifdef _VIP_SHOP
		if( NPC_Util_GetStrFromStrWithDelim( buf1, "VipItem", buf2, sizeof( buf2) ) == NULL)
			if( NPC_Util_GetStrFromStrWithDelim( buf1, "VipPet", buf2, sizeof( buf2) ) == NULL)
#endif
		{
			CHAR_talkToCli( toindex, meindex, "条件不足！", CHAR_COLORYELLOW);
			return FALSE;
		}
	}
	return Action_RunDoEventAction( meindex, toindex, buf1);
}

BOOL Action_RunDoEventAction( int meindex, int toindex, char *buf1)
{
	char buf2[1256];

#if 1 // Robin NPC执行动作前再一次检查FREE条件
	if( NPC_Util_GetStrFromStrWithDelim( buf1, "FREE", buf2, sizeof( buf2)) != NULL ) {
		if( NPC_ActionPassCheck( meindex, toindex, buf2 ) == FALSE && buf2[0] == '\n' )	{
			print("\n 改封包!?跳过NPC的FREE检查!!:%s:%d,%d,%d ",
					CHAR_getChar(toindex, CHAR_CDKEY),
					CHAR_getInt(toindex, CHAR_FLOOR),
					CHAR_getInt(toindex, CHAR_X),
					CHAR_getInt(toindex, CHAR_Y) );
			CHAR_warpToSpecificPoint(toindex,117,225,13);
			return FALSE;
		}
	}
#endif


	if( NPC_Util_GetStrFromStrWithDelim( buf1, "AddGold", buf2, sizeof( buf2)) != NULL ) {
		if( NPC_ActionAddGold2( toindex, buf2) == FALSE )
			return FALSE;
	}

	if( NPC_Util_GetStrFromStrWithDelim( buf1, "DelGold", buf2, sizeof( buf2)) != NULL ) {
		if( NPC_ActionDelGold( toindex, buf2) == FALSE )
			return FALSE;
	}
#ifdef _DP_NPC
	if( NPC_Util_GetStrFromStrWithDelim( buf1, "DelDP", buf2, sizeof( buf2)) != NULL ) {
		if( NPC_ActionDP( toindex, buf2) == FALSE )
			return FALSE;
	}
#endif
	if( NPC_Util_GetStrFromStrWithDelim( buf1, "DelItem", buf2, sizeof( buf2)) != NULL ) {
		//print("\n DelItem:%s:%d ", __FILE__, __LINE__);
		if( NPC_ActionDelItem( toindex, buf2) == FALSE ) {
			//print("\n 改封包??NPC_ActionDelItem:%s:%s ", CHAR_getChar(toindex, CHAR_CDKEY), buf2);
			//CHAR_warpToSpecificPoint(toindex,117,225,13);
			return FALSE;
		}
	}

	//条件成立後npc所给的道具
	if( NPC_Util_GetStrFromStrWithDelim( buf1, "AddItem", buf2, sizeof( buf2)) != NULL ) {
		if( NPC_ActionAddItem( toindex, buf2) == FALSE )
			return FALSE;
	}
#ifdef _VIP_SHOP
	if( NPC_Util_GetStrFromStrWithDelim( buf1, "VipItem", buf2, sizeof( buf2)) != NULL ) {
		if( NPC_ActionVipItem( toindex, buf2) == FALSE )
			return FALSE;
	}
	
	if( NPC_Util_GetStrFromStrWithDelim( buf1, "VipPet", buf2, sizeof( buf2)) != NULL ) {
		if( NPC_ActionVipPet( toindex, buf2) == FALSE )
			return FALSE;
	}
#endif	
#ifdef _TRANS_7_NPC
	if( NPC_Util_GetStrFromStrWithDelim( buf1, "TRANS7", buf2, sizeof( buf2)) != NULL ) {
		if( NPC_ActionTrans7( toindex, buf2) == FALSE )
			return FALSE;
	}
#endif
	if( NPC_Util_GetStrFromStrWithDelim( buf1, "DelPet", buf2, sizeof( buf2)) != NULL ) {
		if( NPC_ActionDelPet( toindex, buf2) == FALSE )
			return FALSE;
	}

#ifdef _NPC_NewDelPet
	if( NPC_Util_GetStrFromStrWithDelim( buf1, "NewDelPet", buf2, sizeof( buf2)) != NULL ) {
        if( NPC_ActionNewDelPet( toindex, buf2) == FALSE )
			return FALSE;
	}
#endif

	if( NPC_Util_GetStrFromStrWithDelim( buf1, "AddPet", buf2, sizeof( buf2)) != NULL ) {
		if( NPC_ActionAddPet( toindex, buf2) == FALSE )
			return FALSE;
	}
	if( NPC_Util_GetStrFromStrWithDelim( buf1, "EvEnd", buf2, sizeof( buf2)) != NULL ) {
		if( NPC_ActionSetEend( toindex, buf2) == FALSE )
			return FALSE;
	}
	if( NPC_Util_GetStrFromStrWithDelim( buf1, "EvNow", buf2, sizeof( buf2)) != NULL ) {
		if( NPC_ActionSetNow( toindex, buf2) == FALSE )
			return FALSE;
	}

	if( NPC_Util_GetStrFromStrWithDelim( buf1, "Event_End", buf2, sizeof( buf2) ) != NULL) {
		if( NPC_ActionSetEend( toindex, buf2) == FALSE )	{
			return FALSE;
		}
	}

	if( NPC_Util_GetStrFromStrWithDelim( buf1, "Event_Now", buf2, sizeof( buf2) ) != NULL) {
		if( NPC_ActionSetNow( toindex, buf2) == FALSE )
			return FALSE;
	}


	if( NPC_Util_GetStrFromStrWithDelim( buf1, "EvClr", buf2, sizeof( buf2) ) != NULL) {
		if( NPC_ActionClearEvent( toindex, buf2) == FALSE )
			return FALSE;
	}

#ifdef _NPCCHANGE_PLAYERIMG
	if( NPC_Util_GetStrFromStrWithDelim( buf1, "CHANGEBBI", buf2, sizeof( buf2) ) != NULL) {
		if( NPC_ActionChangePlayerBBI( meindex, toindex, buf2) == FALSE )
			return FALSE;
	}
#endif

	if( NPC_Util_GetStrFromStrWithDelim( buf1, "SetLastTalkelder", buf2, sizeof( buf2) ) != NULL) {
		CHAR_setInt( toindex, CHAR_LASTTALKELDER, atoi( buf2));
	}

#ifdef _TYPE_TOXICATION
	if( NPC_Util_GetStrFromStrWithDelim( buf1, "TOXICATION", buf2, sizeof( buf2) ) != NULL) {
		int fd = getfdFromCharaIndex( toindex );
		if( strstr( buf2, "TRUE") != 0 ){
			setToxication( fd, 1);
			CHAR_setWorkInt( toindex, CHAR_WORKTOXICATION, 0 );
			if( NPC_Util_GetStrFromStrWithDelim( buf1, "TOXICATIONMSG", buf2, sizeof( buf2) ) != NULL) {
				CHAR_talkToCli( toindex, -1, buf1,  CHAR_COLORYELLOW);
			}
		}else	{
			CHAR_setWorkInt( toindex, CHAR_WORKTOXICATION, 0 );
			setToxication( fd, 0);
			if( NPC_Util_GetStrFromStrWithDelim( buf1, "DETOXIFYMSG", buf2, sizeof( buf2) ) != NULL) {
				CHAR_talkToCli( toindex, -1, buf2,  CHAR_COLORYELLOW);
			}
		}
	}
#endif

	if(NPC_Util_GetStrFromStrWithDelim( buf1, "GetRandItem", buf2, sizeof( buf2) ) != NULL ){
		int j=1, rand_cnt=1;
		char buf3[256];
		while( getStringFromIndexWithDelim( buf2 , "," , j, buf3, sizeof( buf3)) != FALSE ){
			j++;
			rand_cnt++;
		}
		NPC_ActionTreasureRandItemGet( meindex, toindex, rand_cnt, buf2);
	}
#ifdef _TREASURE_BOX
	if( NPC_Util_GetStrFromStrWithDelim( buf1, "TREASURE_EVENT", buf2, sizeof( buf2) ) != NULL) {
		if( strstr( buf2, "OPEN") != NULL){
			if( NPC_Util_GetStrFromStrWithDelim( buf1, "TREASURE_TIME", buf2, sizeof( buf2) ) == NULL)
				return FALSE;
			//NPC_TIME_EVENTMODE = CHAR_NPCWORKINT6,// 1
			//NPC_TIME_EVENTTIME = CHAR_NPCWORKINT7,
			CHAR_setWorkInt( meindex, CHAR_NPCWORKINT6, NPC_EVENTMODE_OPEN );
			CHAR_setWorkInt( meindex, CHAR_NPCWORKINT7, NowTime.tv_sec+atoi( buf2) );
			//变图
			//NPC_TIME_EVENONBBI = CHAR_NPCWORKINT8,
			CHAR_setInt( meindex, CHAR_BASEBASEIMAGENUMBER,
				CHAR_getWorkInt( meindex, CHAR_NPCWORKINT8));

			CHAR_setInt( meindex, CHAR_BASEIMAGENUMBER,
				CHAR_getWorkInt( meindex, CHAR_NPCWORKINT8));
			CHAR_sendCToArroundCharacter( CHAR_getWorkInt( meindex, CHAR_WORKOBJINDEX ) );
		}
	}
#endif

#ifdef _NPC_ADDWARPMAN1
    if( NPC_Util_GetStrFromStrWithDelim( buf1, "PartyCount", buf2, sizeof( buf2) ) != NULL) {
	    //取得组队人数
		int partys=0;
	    int leaderindex=-1;
        int flg=0; //0:等於 1:大於等於 2:小於等於 3:大於 4:小於 5:不等於
	    char Head[32],buf3[32];
		leaderindex = toindex;
	    if( CHAR_getWorkInt( toindex, CHAR_WORKPARTYMODE) == CHAR_PARTY_NONE ){
		    partys = 0;
		}else if( CHAR_getWorkInt( toindex, CHAR_WORKPARTYMODE) == CHAR_PARTY_CLIENT ){
		    leaderindex = CHAR_getWorkInt( toindex, CHAR_WORKPARTYINDEX1);
		
	        if( CHAR_getWorkInt( leaderindex, CHAR_WORKPARTYMODE) == CHAR_PARTY_LEADER ){
		        int i;
		        for( i=0; i<5; i++)	{
			        int pindex = CHAR_getWorkInt( leaderindex, CHAR_WORKPARTYINDEX1+i);
			        if( !CHAR_CHECKINDEX( pindex) )
				        partys++;
				}
			}
		}
	   
		memset( Head, 0, sizeof( Head));
		memset( buf3, 0, sizeof( buf3));
	    if( strstr( buf2, "=" ) != NULL ){
		    strcpy( Head, "=");
		    flg = 0;
		    if( strstr( buf2, ">") != NULL ){
			    strcat( Head, ">");
			    flg = 1;
			}else if( strstr( buf2, "<") != NULL ){
			    strcat( Head, "<");
			    flg = 2;
			}else if( strstr( buf2, "!") != NULL )	{
			    strcat( Head, "!");
			    flg = 5;
			}
		}else if(strstr( buf2, "<") != NULL){
		    strcpy( Head, "<");
		    flg = 4;
		}else if(strstr( buf2, ">") != NULL){
		    strcpy( Head, ">");
		    flg = 3;
		}
		Head[ strlen( Head)+1] = 0;
		if( NPC_Util_GetStrFromStrWithDelim( buf2, Head, buf3, sizeof( buf3) ) != NULL) {
		    if( flg == 0 )
                if( atoi(buf3) != partys ) return FALSE;  
            else if( flg == 1 )
                if( atoi(buf3) < partys ) return FALSE;
		    else if( flg == 2 )
			    if( atoi(buf3) > partys ) return FALSE;
            else if( flg == 3 )
			    if( atoi(buf3) <= partys ) return FALSE;
		    else if( flg == 4 )
			    if( atoi(buf3) >= partys ) return FALSE;
		    else if( flg == 5 )
			    if( atoi(buf3) == partys ) return FALSE;
		}
	}
#endif

	ActionNpc_WarpPoint( meindex, toindex, buf1);
	return TRUE;
}
BOOL Action_PartyCheck( int meindex, int talker)
{
	if(CHAR_getWorkInt(talker,CHAR_WORKPARTYMODE) != CHAR_PARTY_NONE){
		return FALSE;
	}
	return TRUE;
}

BOOL NPC_ActionAddGold( int talker, int Golds)
{
	CHAR_AddGold( talker, Golds);
	return TRUE;
}

BOOL NPC_ActionDelGold( int talker,char *buf)
{
	int Golds=0;
	int PGold=0;
	if( buf == NULL ) return FALSE;
	Golds = atoi( buf);
	PGold = CHAR_getInt( talker, CHAR_GOLD);
	if( PGold < Golds )	{
		CHAR_talkToCli( talker, -1, "你身上的钱不够喔！", CHAR_COLORYELLOW);
		return FALSE;
	}else	{
		char buf1[256];
		sprintf( buf1, "交出%d石币", Golds);
		CHAR_talkToCli( talker, -1, buf1, CHAR_COLORYELLOW);
	}
	CHAR_setInt( talker, CHAR_GOLD, PGold-Golds);
	CHAR_send_P_StatusString( talker , CHAR_P_STRING_GOLD);
	return TRUE;
}
#ifdef _DP_NPC
BOOL NPC_ActionDP( int talker,char *buf)
{
	int DPs=0;
	int PDP=0;
	if( buf == NULL ) return FALSE;
	DPs = atoi( buf);
	PDP = CHAR_getInt( talker, CHAR_DUELPOINT);
	if( PDP < DPs )	{
		CHAR_talkToCli( talker, -1, "你的DP不够哦！", CHAR_COLORYELLOW);
		return FALSE;
	}else	{
		char buf1[256];
		sprintf( buf1, "扣除DP%d", DPs);
		CHAR_talkToCli( talker, -1, buf1, CHAR_COLORYELLOW);
	}
	CHAR_setInt( talker, CHAR_DUELPOINT, PDP-DPs);
	CHAR_send_P_StatusString( talker ,CHAR_P_STRING_DUELPOINT );
	return TRUE;
}
#endif
// WON ADD
BOOL NPC_ActionAddGold2( int talker,char *buf)
{
	int Golds=0;
	int PGold=0;
	int MaxGold = 0;
	int total_gold = 0;
	if( buf == NULL ) return FALSE;
	Golds = atoi( buf);
	PGold = CHAR_getInt( talker, CHAR_GOLD);
	MaxGold = CHAR_getMaxHaveGold( talker );

	total_gold = PGold + Golds;
	if( total_gold > MaxGold ) total_gold = MaxGold;
	
	CHAR_setInt( talker, CHAR_GOLD, total_gold );
	CHAR_send_P_StatusString( talker , CHAR_P_STRING_GOLD);
	return TRUE;
}

BOOL NPC_ActionDelPet( int talker, char *buf)
{
	int petsel;	 
	int petindex=0, k=1;
	int fd = getfdFromCharaIndex( talker );
	char msgbuf[64], szPet[128],buff2[256],buff3[256];
	char buf2[256],buff1[256];
	int defpet;
	int petno,kNum,find=0,petLv=0,flg=-1;

	while( getStringFromIndexWithDelim(buf , "," , k, buff1, sizeof( buff1)) !=FALSE )	{ 
		k++;
    	if( strstr( buff1,"-") != NULL )	{
		getStringFromIndexWithDelim(buff1,"-",2,buff3,sizeof(buff3));
    		getStringFromIndexWithDelim(buff1,"-",1,buf2,sizeof(buf2));		

			if( strstr( buf2,"!") != NULL )	{
				flg = 0;
				getStringFromIndexWithDelim(buf2,"!=",2,buff2,sizeof(buff2));
			}else if( strstr( buf,"<") != NULL )	{
				flg = 1;
				getStringFromIndexWithDelim(buf2,"<",2,buff2,sizeof(buff2));
			}else if( strstr( buf,">") != NULL )	{
				flg = 2;
				getStringFromIndexWithDelim(buf2,">",2,buff2,sizeof(buff2));
			}else if( strstr( buf,"=") != NULL)	{
				flg = 3;
				getStringFromIndexWithDelim(buf2,"=",2,buff2,sizeof(buff2));
			}else
				return FALSE;
			petLv = atoi( buff2);
    	}
		continue;

		if(strstr(buff3,"*")!=NULL){									
			getStringFromIndexWithDelim(buff3,"*",1,buf2,sizeof(buf2));
			petno = atoi(buf2);
			getStringFromIndexWithDelim(buff3,"*",2,buf2,sizeof(buf2));
			kNum = atoi(buf2);
		}else	{
			petno = atoi( buff3);
			kNum = 30;
		}
		//andy_log
		print("petno:%d kNum:%d \n ", petno, kNum);

		find =0;

		for(petsel=0 ; petsel < CHAR_MAXPETHAVE ; petsel++)	{
			petindex = CHAR_getCharPet( talker, petsel);
			if( !CHAR_CHECKINDEX(petindex))	continue;
			if(CHAR_getInt( petindex, CHAR_PETID) !=  petno )continue;

	  		switch( flg)	{
	  		case 0:
	  			if( CHAR_getInt( petindex, CHAR_LV )  != petLv )	{
	  			}else	{
	  				continue;
	  			}
	  		break;
	  		case 1:
				if( CHAR_getInt( petindex, CHAR_LV )  <  petLv )	{
				}else	{
					continue;
				}
			break;
	  		case 2:
				if( CHAR_getInt( petindex, CHAR_LV )  > petLv )		{
				}else	{
					continue;
				}
	  		break;
	  		case 3:
				if( CHAR_getInt( petindex, CHAR_LV ) != petLv )continue;
				break;
	  		}
	  		
 			petindex = CHAR_getCharPet( talker, petsel);
			if( !CHAR_CHECKINDEX(petindex) )
				continue;
			if( CHAR_getWorkInt( CONNECT_getCharaindex( fd), CHAR_WORKBATTLEMODE) != BATTLE_CHARMODE_NONE )
				continue;
			defpet = CHAR_getInt( talker, CHAR_DEFAULTPET);
			
			if(defpet == petsel)	{
				CHAR_setInt( talker, CHAR_DEFAULTPET, -1);
				lssproto_KS_send( fd, -1, TRUE);
			}

			snprintf( msgbuf,sizeof( msgbuf), "交出%s。", CHAR_getChar( petindex, CHAR_NAME));
			CHAR_talkToCli( talker, -1, msgbuf,  CHAR_COLORYELLOW);
				LogPet(			
					CHAR_getChar( talker, CHAR_NAME ),
					CHAR_getChar( talker, CHAR_CDKEY ),
					CHAR_getChar( petindex, CHAR_NAME),
					CHAR_getInt( petindex, CHAR_LV),
					"TenseiDel",
					CHAR_getInt( talker,CHAR_FLOOR),
					CHAR_getInt( talker,CHAR_X ),
					CHAR_getInt( talker,CHAR_Y ),
					CHAR_getChar( petindex, CHAR_UNIQUECODE)   // shan 2001/12/14
				);
			CHAR_setCharPet( talker, petsel, -1);
			CHAR_endCharOneArray( petindex );
			snprintf( szPet, sizeof( szPet ), "K%d", petsel);
			CHAR_sendStatusString( talker, szPet );

			find++;
			if( find >= kNum )
				break;	//条件数目皆删除完
		}
	}

    return TRUE;
}

#ifdef _NPC_NewDelPet
BOOL NPC_ActionNewDelPet( int toindex, char *msg)
{
   		int i,petindex=0,petlev,petnum,petcount,mypetlevel,petid;
		char buf[32],buf2[32];
		int flg=0,count=0;
		
		//比较方式
		if ( strstr( msg, "=" ) )
			flg = 0;
		else if( strstr( msg, "<" ) )
			flg = 1;
		else
			flg = 2;

		//等级
		if ( getStringFromIndexWithDelim( msg, "-", 1, buf, sizeof( buf )) ){
			char cmp[12];
			if( flg == 0 )
				strcpy(cmp,"=");
			else if( flg == 1 )
				strcpy(cmp,"<");
			else 
				strcpy(cmp,">");
			if ( getStringFromIndexWithDelim( buf, cmp, 2, buf2, sizeof( buf2 )) ){
			    petlev = atoi(buf2);
			}
			else
			    return FALSE;
		}
		else
            return FALSE;

		//宠物编号
        if ( getStringFromIndexWithDelim( msg, "-", 2, buf, sizeof( buf )) ){
			if ( getStringFromIndexWithDelim( buf, "*", 1, buf2, sizeof( buf2 )) ){
                petnum = atoi( buf2 );
			}
			else
				return FALSE;
		}
		else
			return FALSE;

		//数量
        if ( getStringFromIndexWithDelim( msg, "-", 2, buf, sizeof( buf )) ){
			if ( getStringFromIndexWithDelim( buf, "*", 2, buf2, sizeof( buf2 )) ){
			    petcount = atoi( buf2 );
			}
			else
				return FALSE;
		}
		else
			return FALSE;

	    for( i = 0; i < CHAR_MAXPETHAVE; i ++ ){
			BOOL cmp=FALSE;
			petindex = CHAR_getCharPet( toindex, i);
			petid = CHAR_getInt( petindex, CHAR_PETID);
			mypetlevel = CHAR_getInt( petindex, CHAR_LV);
        		
			if( flg == 0 )
				if( mypetlevel == petlev ) cmp = TRUE;
			if( flg == 1 )
				if( mypetlevel < petlev ) cmp = TRUE;
			if( flg == 2 )
				if( mypetlevel > petlev ) cmp = TRUE;

			if( petnum == petid && cmp ){//删除宠物
	            char szPet[128];
	            int defpet;
	            char msgbuf[64];
	            int fd = getfdFromCharaIndex( toindex );

                if( !CHAR_CHECKINDEX(petindex) ) return FALSE;

	            if( CHAR_getInt( toindex, CHAR_RIDEPET) == i ) {

		//CHAR_talkToCli( talker, -1, "骑乘中的宠物无法交出！", CHAR_COLORYELLOW );
    	//return	FALSE;

		            CHAR_setInt( toindex, CHAR_RIDEPET, -1);
		            CHAR_send_P_StatusString( toindex, CHAR_P_STRING_RIDEPET );
		            CHAR_complianceParameter( toindex );
		            CHAR_sendCToArroundCharacter( CHAR_getWorkInt( toindex , CHAR_WORKOBJINDEX ));
				}

	            //公及平乓仿互爵    卅日｝矢永玄毛丹日内蔽  
	            if( CHAR_getWorkInt( CONNECT_getCharaindex( fd ), CHAR_WORKBATTLEMODE) != BATTLE_CHARMODE_NONE) 
								return FALSE;
	            //及矢永玄互田玄伙井升丹井及民尼永弁
	            defpet = CHAR_getInt( toindex, CHAR_DEFAULTPET);
	            if(defpet == i){
		            CHAR_setInt( toindex, CHAR_DEFAULTPET, -1);
		            lssproto_KS_send( fd, -1, TRUE);
				}

	            snprintf( msgbuf,sizeof( msgbuf), "交出%s。",
						  CHAR_getChar( petindex, CHAR_NAME));
	            CHAR_talkToCli( toindex, -1, msgbuf,  CHAR_COLORWHITE);

	            // 矢永玄毛澎  仄凶夫弘
	            LogPet( CHAR_getChar( toindex, CHAR_NAME ), // 平乓仿   
		                CHAR_getChar( toindex, CHAR_CDKEY ),
		                CHAR_getChar( petindex, CHAR_NAME),
		                CHAR_getInt( petindex, CHAR_LV),
		                "EvnetDell(任务删除)",
		                CHAR_getInt( toindex,CHAR_FLOOR),
		                CHAR_getInt( toindex,CHAR_X ),
		                CHAR_getInt( toindex,CHAR_Y ),
		                CHAR_getChar( petindex, CHAR_UNIQUECODE)   // shan 2001/12/14
	            );


	            //   切  井日壅允 
	            CHAR_setCharPet( toindex, i, -1);

	            CHAR_endCharOneArray( petindex );

	            snprintf( szPet, sizeof( szPet ), "K%d", i);
	            // 蕙仄中矢永玄旦  □正旦霜曰勾仃月
	            CHAR_sendStatusString( toindex, szPet );

				++count;
                if( count >= petcount )
				    break;
			}
		}

    return TRUE;
}
#endif

BOOL NPC_ActionAddPet( int talker, char *buf)
{
	int	ret;
	char msgbuf[64], buff3[64];
	int	enemynum;
	int	i,j,k=1;
	int petindex, petindex2, petid;

  k=1;
  while( getStringFromIndexWithDelim(buf , "," , k, buff3, sizeof(buff3)) !=FALSE ){
  	k++;	
	//检查宠物栏是否有空位
	for( i = 0 ;i < CHAR_MAXPETHAVE ; i++) {
		petindex = CHAR_getCharPet( talker, i);
		if( petindex == -1  )
			break;
	}

  if( i == CHAR_MAXPETHAVE ){
		snprintf( msgbuf,sizeof( msgbuf), "宠物已满！！");
		CHAR_talkToCli( talker, -1, msgbuf,  CHAR_COLORYELLOW);
		return FALSE;
	}
#ifdef _PET_MM
	if(strstr(buff3,"MM1") || strstr(buff3,"MM3"))
		petid = 1479;
	else if(strstr(buff3,"MM2") || strstr(buff3,"MM4"))
		petid = 2547;
	else
		petid = atoi( buff3);
#else
	petid = atoi( buff3);
#endif
	enemynum = ENEMY_getEnemyNum();
	for( i = 0; i < enemynum; i ++ ) {
		if( ENEMY_getInt( i, ENEMY_ID ) == petid )
		//if( ENEMY_getInt( i, ENEMY_TEMPNO ) == petid )
			break;
	}
	
	if( i == enemynum )
		continue;
		
	ret = ENEMY_createPetFromEnemyIndex( talker, i);
	for( i = 0; i < CHAR_MAXPETHAVE; i ++ )	{
		if( CHAR_getCharPet( talker, i ) == ret )
			break;
	}
	if( i == CHAR_MAXPETHAVE )
		i = 0;	

	if( CHAR_CHECKINDEX( ret ) == TRUE ){
#ifdef _PET_MM
		if(strstr(buff3,"MM1") || strstr(buff3,"MM2")){
			int LevelUpPoint;
			LevelUpPoint = ( 50<< 24) + ( 50<< 16) + ( 50<< 8) + ( 50<< 0);
			CHAR_setInt( ret, CHAR_ALLOCPOINT, LevelUpPoint);
		}else if(strstr(buff3,"MM3") || strstr(buff3,"MM4")){
			while(CHAR_getInt( ret, CHAR_LV)<79){	//升级
				int LevelUpPoint;
				LevelUpPoint = ( 50<< 24) + ( 50<< 16) + ( 50<< 8) + ( 50<< 0);
				CHAR_setInt( ret, CHAR_ALLOCPOINT, LevelUpPoint);
				CHAR_PetLevelUp( ret );
				CHAR_PetAddVariableAi( ret, AI_FIX_PETLEVELUP );
				CHAR_setInt( ret, CHAR_LV, CHAR_getInt( ret, CHAR_LV) +1 );
			}
		}
#endif
		CHAR_setInt( ret, CHAR_HP, CHAR_getWorkInt( ret, CHAR_WORKMAXHP ));
		CHAR_setMaxExpFromLevel( ret, CHAR_getInt( ret, CHAR_LV ));
		CHAR_setInt(ret,CHAR_VARIABLEAI,10000);
	}
	
	petindex2 = CHAR_getCharPet(talker, i);
	if( !CHAR_CHECKINDEX( petindex2) )
		continue;
	snprintf( msgbuf,sizeof( msgbuf), "拿到%s。", CHAR_getChar(petindex2,CHAR_NAME));
	CHAR_talkToCli( talker, -1, msgbuf,  CHAR_COLORYELLOW);
	for(j = 0; j < CHAR_MAXPETHAVE; j++){
		petindex = CHAR_getCharPet(talker, j);
		if( !CHAR_CHECKINDEX( petindex) )
			continue;
		CHAR_complianceParameter( petindex );
		snprintf( msgbuf, sizeof( msgbuf ), "K%d", j );
		CHAR_sendStatusString( talker, msgbuf );
		snprintf( msgbuf, sizeof( msgbuf ), "W%d", j );
		CHAR_sendStatusString( talker, msgbuf );
	}

	LogPet(
		CHAR_getChar( talker, CHAR_NAME ),
		CHAR_getChar( talker, CHAR_CDKEY ),
		CHAR_getChar( petindex2, CHAR_NAME),
		CHAR_getInt( petindex2, CHAR_LV),
		"TenseiGet",
		CHAR_getInt( talker,CHAR_FLOOR),
		CHAR_getInt( talker,CHAR_X ),
		CHAR_getInt( talker,CHAR_Y ),
		CHAR_getChar( petindex2, CHAR_UNIQUECODE)   // shan 2001/12/14
	);
  }
	return TRUE;
}

BOOL NPC_ActionDelItem( int talker, char *buf)
{
	int i = 1, j = 1,k = 1;
	char buff3[128];
	char buf2[32];
	int itemindex;
	char token[256];
	
	if( !CHAR_CHECKINDEX( talker) ) return FALSE;
	while( getStringFromIndexWithDelim(buf , "," , k, buff3, sizeof(buff3)) !=FALSE )	{
		k++;
		if(strstr(buff3,"*")!=NULL){
			int itemno, kosuu=0, id, cnt=0;
			struct _tagItemArray {
				int index;
				int itemindex;
			};
			struct _tagItemArray ItemArray[20];
			for( i=0; i<20 ; i++){
				ItemArray[i].itemindex = -1;
				ItemArray[i].index = -1;
			}
			//取得字串
			getStringFromIndexWithDelim(buff3,"*",1,buf2,sizeof(buf2));
			itemno = atoi(buf2);
			getStringFromIndexWithDelim(buff3,"*",2,buf2,sizeof(buf2));
			kosuu = atoi(buf2);
			//取得 item index
			for( i = CHAR_STARTITEMARRAY ; i < CHAR_MAXITEMHAVE ; i++ ){
				itemindex=CHAR_getItemIndex( talker , i );
				if( ITEM_CHECKINDEX(itemindex) ){
					id=ITEM_getInt(itemindex ,ITEM_ID );
					if(itemno==id){
						ItemArray[cnt].itemindex = itemindex;
						ItemArray[cnt].index = i;
						cnt++;
					}
				}
				if(cnt == kosuu)
					break;
			}
			if(cnt != kosuu){
				CHAR_talkToCli( talker, -1,"条件不足", CHAR_COLORYELLOW);
				return FALSE;
			}
			//收取物品
			for( i=0; i<20; i++)	{
				itemindex = ItemArray[i].itemindex;
				if( !ITEM_CHECKINDEX(itemindex) ) break;
				sprintf(token,"交出%s",ITEM_getChar( itemindex, ITEM_NAME));
				CHAR_talkToCli( talker, -1, token, CHAR_COLORYELLOW);
				CHAR_setItemIndex( talker, ItemArray[i].index ,-1);
				ITEM_endExistItemsOne( itemindex);
				CHAR_sendItemDataOne( talker, ItemArray[i].index);
				LogItem(
					CHAR_getChar( talker, CHAR_NAME ), CHAR_getChar( talker, CHAR_CDKEY ),
#ifdef _add_item_log_name  // WON ADD 在item的log中增加item名称
					itemindex,
#else
					ITEM_getInt( itemindex, ITEM_ID),
#endif
					"WarpManDelItem",
					CHAR_getInt( talker, CHAR_FLOOR), CHAR_getInt( talker, CHAR_X ),
					CHAR_getInt( talker, CHAR_Y ), ITEM_getChar( itemindex, ITEM_UNIQUECODE),
					ITEM_getChar( itemindex, ITEM_NAME),
					ITEM_getInt( itemindex, ITEM_ID)
					);
			}
		}else{
			for( j = CHAR_STARTITEMARRAY ;  j < CHAR_MAXITEMHAVE ; j++){
				itemindex = CHAR_getItemIndex( talker ,j);
				if( ITEM_CHECKINDEX(itemindex) ){
					if( atoi( buff3) == ITEM_getInt(itemindex,ITEM_ID)){
						LogItem(
							CHAR_getChar( talker, CHAR_NAME ),
							CHAR_getChar( talker, CHAR_CDKEY ),
#ifdef _add_item_log_name  // WON ADD 在item的log中增加item名称
							itemindex,
#else
							ITEM_getInt( itemindex, ITEM_ID),
#endif
							"WarpManDelItem",
							CHAR_getInt( talker,CHAR_FLOOR),
							CHAR_getInt( talker,CHAR_X ),
							CHAR_getInt( talker,CHAR_Y ),
							ITEM_getChar( itemindex, ITEM_UNIQUECODE),
							ITEM_getChar( itemindex, ITEM_NAME),
							ITEM_getInt( itemindex, ITEM_ID)

						);
						sprintf( token, "交出%s。", 
										ITEM_getChar( itemindex, ITEM_NAME));
						CHAR_talkToCli( talker, -1, token, CHAR_COLORYELLOW);
						CHAR_DelItem( talker, j);
					}
				}
			}
		}
	}
	return TRUE;
}

#ifdef _VIP_SHOP
BOOL NPC_ActionVipItem( int talker, char *buf)
{
  int itemID,itemindex=-1;
  int pVipPoint, VipPoints;
  char buff[256], token[256];
  int ret;

	if(getStringFromIndexWithDelim(buf , "," , 1, buff, sizeof(buff)) !=FALSE)
		itemID = atoi(buf);

  itemindex = CHAR_findEmptyItemBox( talker );
	if( itemindex < 0 )	{
		CHAR_talkToCli( talker, -1, "物品栏空间不足！！",  CHAR_COLORYELLOW);
		return FALSE;
	}
	
	if(getStringFromIndexWithDelim(buf , "," , 2, buff, sizeof(buff)) !=FALSE)
		VipPoints=atoi(buff);
	if(VipPoints<0)VipPoints=0;

  pVipPoint = CHAR_getInt( talker, CHAR_AMPOINT);
	if( pVipPoint < VipPoints )	{
		CHAR_talkToCli( talker, -1, "你身上的点数不够哦", CHAR_COLORYELLOW);
		return FALSE;
	}
	
	itemindex = ITEM_makeItemAndRegist( itemID );
  if( itemindex != -1 ){
		ret = CHAR_addItemSpecificItemIndex( talker, itemindex);
		if( ret < 0 || ret >= CHAR_MAXITEMHAVE ) {
			ITEM_endExistItemsOne( itemindex);
			print ("\n ret error!!");
			return FALSE;
		}

		sprintf( token,"拿到%s",ITEM_getChar( itemindex, ITEM_NAME));
		CHAR_talkToCli( talker, -1, token, CHAR_COLORYELLOW );
		
		CHAR_setInt( talker, CHAR_AMPOINT, pVipPoint-VipPoints);
		sprintf( token, "扣除会员点数%d", VipPoints);
		CHAR_talkToCli( talker, -1, token, CHAR_COLORYELLOW);

		CHAR_sendItemDataOne( talker, ret);
  }

	return TRUE;	                                                                                                                                                                      
}

BOOL NPC_ActionVipPet( int talker, char *buf)
{
	int	ret;
	char msgbuf[64], buf1[8], buf2[10];
	int	enemynum;
	int	i,j;
	int petindex, petindex2, petid;
  int pVipPoint, VipPoints;
		
	if(getStringFromIndexWithDelim(buf , "," , 1, buf1, sizeof(buf1)) !=FALSE)
#ifdef _PET_MM
	if(strstr(buf1,"MM1") || strstr(buf1,"MM3"))
		petid = 1479;
	else if(strstr(buf1,"MM2") || strstr(buf1,"MM4"))
		petid = 2547;
	else
		petid = atoi( buf1 );
#else
	petid = atoi( buf1 );
#endif
		
	if(getStringFromIndexWithDelim(buf , "," , 2, buf2, sizeof(buf2)) !=FALSE)
		VipPoints=atoi(buf2);
	if(VipPoints<0)VipPoints=0;

  pVipPoint = CHAR_getInt( talker, CHAR_AMPOINT);
	if( pVipPoint < VipPoints )	{
		CHAR_talkToCli( talker, -1, "你身上的点数不够哦", CHAR_COLORYELLOW);
		return FALSE;
	}

	for( i = 0 ;i < CHAR_MAXPETHAVE ; i++) {
		petindex = CHAR_getCharPet( talker, i);
		if( petindex == -1  )
			break;
	}

  if( i == CHAR_MAXPETHAVE )      {
		snprintf( msgbuf,sizeof( msgbuf), "宠物已满！！");
		CHAR_talkToCli( talker, -1, msgbuf,  CHAR_COLORYELLOW);
		return FALSE;
	}

	enemynum = ENEMY_getEnemyNum();
	for( i = 0; i < enemynum; i ++ ) {
		if( ENEMY_getInt( i, ENEMY_ID) == petid) {
				break;
			}
	}

	if( i == enemynum )
		return FALSE;

	ret = ENEMY_createPetFromEnemyIndex( talker, i);
	for( i = 0; i < CHAR_MAXPETHAVE; i ++ )	{
		if( CHAR_getCharPet( talker, i ) == ret )
			break;
	}
	if( i == CHAR_MAXPETHAVE )
		CHAR_talkToCli( talker, -1, "领取宠物失败！", CHAR_COLORYELLOW);

	if( CHAR_CHECKINDEX( ret ) == TRUE ){
#ifdef _PET_MM
		if(strstr(buf1,"MM1") || strstr(buf1,"MM2")){
			int LevelUpPoint;
			LevelUpPoint = ( 50<< 24) + ( 50<< 16) + ( 50<< 8) + ( 50<< 0);
			CHAR_setInt( ret, CHAR_ALLOCPOINT, LevelUpPoint);
		}else if(strstr(buf1,"MM3") || strstr(buf1,"MM4")){
			while(CHAR_getInt( ret, CHAR_LV)<79){	//升级
				int LevelUpPoint;
				LevelUpPoint = ( 50<< 24) + ( 50<< 16) + ( 50<< 8) + ( 50<< 0);
				CHAR_setInt( ret, CHAR_ALLOCPOINT, LevelUpPoint);
				CHAR_PetLevelUp( ret );
				CHAR_PetAddVariableAi( ret, AI_FIX_PETLEVELUP );
				CHAR_setInt( ret, CHAR_LV, CHAR_getInt( ret, CHAR_LV) +1 );
			}
		}
#endif
		CHAR_setInt( ret, CHAR_HP, CHAR_getWorkInt( ret, CHAR_WORKMAXHP ));
		CHAR_setMaxExpFromLevel( ret, CHAR_getInt( ret, CHAR_LV ));
		CHAR_setInt(ret,CHAR_VARIABLEAI,10000);
	}
	
	petindex2 = CHAR_getCharPet(talker, i);
	if( !CHAR_CHECKINDEX( petindex2) )
		return FALSE;
	snprintf( msgbuf,sizeof( msgbuf), "拿到%s。", CHAR_getChar(petindex2,CHAR_NAME));
	CHAR_talkToCli( talker, -1, msgbuf,  CHAR_COLORYELLOW);
	CHAR_setInt( talker, CHAR_AMPOINT, pVipPoint-VipPoints);
	snprintf( msgbuf,sizeof( msgbuf), "扣除会员点数%d", VipPoints);
	CHAR_talkToCli( talker, -1, msgbuf, CHAR_COLORYELLOW);
	for(j = 0; j < CHAR_MAXPETHAVE; j++){
		petindex = CHAR_getCharPet(talker, j);
		if( !CHAR_CHECKINDEX( petindex) )
			continue;
		CHAR_complianceParameter( petindex );
		snprintf( msgbuf, sizeof( msgbuf ), "K%d", j );
		CHAR_sendStatusString( talker, msgbuf );
		snprintf( msgbuf, sizeof( msgbuf ), "W%d", j );
		CHAR_sendStatusString( talker, msgbuf );
	}
	return TRUE;
}
#endif

#ifdef _TRANS_7_NPC
BOOL NPC_ActionTrans7( int talker, char *buf)
{
	char token[128];
	int work[10];
	int Trans=CHAR_getInt(talker,CHAR_TRANSMIGRATION)+1;
#ifdef _ADD_POOL_ITEM			   // WON ADD 增加可寄放的道具	
	int tran_pool_item[5] = { 4, 4, 4, 4, 4 };		// 增加的道具寄放数
	int tran_pool_pet[5] = { 2, 2, 2, 2, 2};		// 增加的宠物寄放数
	int j,item_sum=0, pet_sum=0;
	
	if(Trans>7 || CHAR_getInt(talker,CHAR_LV)<80){
		sprintf( token, "由于你已七转或等级小于80级，所以转生失败!", CHAR_getChar( talker, CHAR_NAME));
		CHAR_talkToCli( talker, -1, token, CHAR_COLORYELLOW );
		return;
	}
	
	for(j=0; j<5 ;j++){
		item_sum += tran_pool_item[j] ;			
		pet_sum += tran_pool_pet[j];		
	}

	item_sum += 10;								// 限制最大的寄道具数
	pet_sum  += 5;							    // 限制最大的寄宠数

#endif
	NPC_TransmigrationStatus(talker, talker, work);
	NPC_TransmigrationFlg_CLS(talker, talker);
	CHAR_setInt(talker ,CHAR_TRANSMIGRATION, Trans);
	CHAR_setInt(talker, CHAR_LV ,1);
	CHAR_setMaxExp( talker, 0);
  CHAR_setInt( talker,CHAR_SKILLUPPOINT,CHAR_getInt( talker, CHAR_TRANSMIGRATION)*10);
  CHAR_Skillupsend( talker );
  CHAR_setInt( talker,CHAR_RIDEPET, -1 );
  CHAR_setInt( talker , CHAR_BASEIMAGENUMBER , CHAR_getInt( talker , CHAR_BASEBASEIMAGENUMBER) );
	CHAR_sendStatusString( talker , "P");
}
#endif

BOOL NPC_ActionAddItem( int talker, char *buf)
{
  int itemID,k=0,itemindex=-1;
  int spaceNum=5,i;
  char buff3[256], msgbuf[64], token[256];
  int ret;

  k=1;
  while( getStringFromIndexWithDelim(buf , "," , k, buff3, sizeof(buff3)) !=FALSE ){
	k++;
		for( i = spaceNum ; i < CHAR_MAXITEMHAVE ; i++ ){
			itemindex=CHAR_getItemIndex( talker , i );
			if( itemindex == -1 )	{
				spaceNum = i+1;
				break;
			}
		}                                                                                                                                                                                                                                                                                                                                      
		if( i == CHAR_MAXITEMHAVE )	{
			snprintf( msgbuf,sizeof( msgbuf), "物品栏空间不足！！");
			CHAR_talkToCli( talker, -1, msgbuf,  CHAR_COLORYELLOW);
			return FALSE;
		}
  }
  k=1;
  while( getStringFromIndexWithDelim(buf , "," , k, buff3, sizeof(buff3)) !=FALSE ){
	k++;
        itemID = atoi( buff3);
        if( itemID  )
		itemindex = ITEM_makeItemAndRegist( itemID);
		if(itemindex == -1)
			continue;
		ret = CHAR_addItemSpecificItemIndex( talker, itemindex);
		if( ret < 0 || ret >= CHAR_MAXITEMHAVE ) {
			ITEM_endExistItemsOne( itemindex);
			print ("\n ret error!!");
			return FALSE;
		}

		sprintf( token,"拿到%s",ITEM_getChar( itemindex, ITEM_NAME));
		CHAR_talkToCli( talker, -1, token, CHAR_COLORYELLOW );
		CHAR_sendItemDataOne( talker, ret);
  }

	return TRUE;	                                                                                                                                                                      
}

BOOL NPC_ActionSetEend( int talkerindex, char * buf)
{
	int eveno;
	int talkNo=1, iflg=0;
	char buf1[256];

	while( getStringFromIndexWithDelim( buf,",",talkNo,buf1,sizeof( buf1)) != FALSE)	{ 
		talkNo++;
		eveno = atoi( buf);
		NPC_EventSetFlg( talkerindex, eveno);
		if( ++iflg > 20 ) break;
	}
	return TRUE;
}

BOOL NPC_ActionClearEvent( int toindex, char * buf)
{
	int eveno;
	int talkNo=1, iflg=0;
	char buf1[256];

	while( getStringFromIndexWithDelim( buf,",",talkNo,buf1,sizeof( buf1)) != FALSE)	{ 
		talkNo++;
		eveno = atoi( buf);
		NPC_NowEndEventSetFlgCls( toindex, eveno);
		if( ++iflg > 20 ) break;
	}
	return TRUE;
}

BOOL NPC_ActionSetNow( int toindex, char * buf)
{
	int eveno;
	int talkNo=1, iflg=0;
	char buf1[256];

	while( getStringFromIndexWithDelim( buf,",",talkNo,buf1,sizeof( buf1)) != FALSE)	{ 
		talkNo++;
		eveno = atoi( buf);
		NPC_NowEventSetFlg( toindex, eveno);
		if( ++iflg > 20 ) break;
	}
	return TRUE;
}


BOOL NPC_ActionPassCheck( int meindex, int talker, char *buf)
{

	char buff2[256];
	char buff3[128];
	int i=1,j=1;
	int loop=0;
	while( getStringFromIndexWithDelim(buf,",",i,buff2,sizeof(buff2)) !=FALSE )	{	
		i++;
		if(strstr(buff2,"&")!=NULL){
			j=1;
			loop=0;
			while( getStringFromIndexWithDelim(buff2,"&",j,buff3,sizeof(buff3)) !=FALSE )	{
				j++;
				if(NPC_ActionBigSmallCheck( meindex, talker, buff3)==FALSE)	{
					loop=1;
					break;
				}
			}
			if(loop==0) {
				CHAR_setWorkInt( talker, CHAR_WORKWARPCHECK, TRUE );
				return TRUE;
			}
		}else{
			if( NPC_ActionBigSmallCheck( meindex, talker, buff2) == TRUE){
				CHAR_setWorkInt( talker, CHAR_WORKWARPCHECK, TRUE );
				return TRUE;

			}
		}
	}
	CHAR_setWorkInt( talker, CHAR_WORKWARPCHECK, FALSE );
	return FALSE;

}

BOOL NPC_ActionBigSmallCheck(int meindex,int talker,char* buf)
{

	char buff2[128], buff3[128];
	int kosuu,temp=-1,flg=0;

#ifdef _NEW_WARPMAN
	char buff1[128];
	if(strstr( buf, "-") != NULL)	{
		getStringFromIndexWithDelim( buf, "-", 2, buff3, sizeof(buff3));
		temp = atoi( buff3);
		getStringFromIndexWithDelim( buf, "-", 1, buff1, sizeof(buff1));
		strcpy( buf, buff1);
	}                         
#endif

	if( strstr( buf, "<") != NULL ){
		getStringFromIndexWithDelim( buf, "<", 2, buff3, sizeof(buff2));
		kosuu = atoi( buff3);
		getStringFromIndexWithDelim( buf, "<", 1, buff2, sizeof(buff2));

		if( NPC_ActionFreeIfCheck( meindex, talker, buff2, buff3, kosuu, 1, temp)==TRUE){
			return TRUE;
		}
	}else if(strstr( buf, ">") != NULL){
		getStringFromIndexWithDelim( buf, ">", 2, buff3, sizeof(buff2));
		kosuu = atoi(buff3);
		getStringFromIndexWithDelim( buf, ">" ,1, buff2, sizeof(buff2));

		if( NPC_ActionFreeIfCheck( meindex, talker, buff2, buff3, kosuu, 2, temp) == TRUE ){
			return TRUE;
		}

	}else if( strstr( buf, "!" ) != NULL){
		getStringFromIndexWithDelim( buf, "!=", 2, buff3, sizeof(buff2));
		kosuu = atoi( buff3);
		getStringFromIndexWithDelim( buf, "!=", 1, buff2, sizeof(buff2));
		if( NPC_ActionFreeIfCheck( meindex, talker, buff2, buff3, kosuu, 0, temp) == TRUE ){
			return FALSE;
		}else{
			return TRUE;
		}
	}else if( strstr( buf, "=") != NULL){
		getStringFromIndexWithDelim( buf, "=", 2, buff3, sizeof(buff2));
		kosuu = atoi( buff3);
		getStringFromIndexWithDelim( buf, "=", 1, buff2, sizeof(buff2));
		flg = 0;
		if( strstr( buf, "*") != NULL){
			if( NPC_ActionWarpManReduce( meindex, talker, buf)==TRUE){
				return TRUE;
			}
		}
#ifdef _NPC_ActionFreeCmp
		else if( strstr( buf, "^") != NULL){
		    if( NPC_ActionWarpManReduce2( meindex, talker, buf)==TRUE){
				return TRUE;
			}
		}
#endif
		else if( NPC_ActionFreeIfCheck( meindex, talker, buff2, buff3, kosuu, flg, temp) == TRUE )	{
			return TRUE;
		}
	}
	return FALSE;
}

#ifdef _NPC_ActionFreeCmp 
//其实功能和原本的NPC_ActionWarpManReduce差不多,但为了不影响以前的设定,所以不去修改原本的
BOOL NPC_ActionWarpManReduce2(int meindex,int talker,char *buf)
{
	char buf2[512];
	char buf3[256];
	int i, itemindex, itemno, kosuu;
	int id=0;
	int nums = 0;

	getStringFromIndexWithDelim(buf,"=",2,buf2,sizeof(buf2));
	getStringFromIndexWithDelim(buf2,"^",1,buf3,sizeof(buf3));
	itemno = atoi(buf3);
	getStringFromIndexWithDelim(buf2,"^",2,buf3,sizeof(buf3));
	kosuu = atoi(buf3);

	for( i=CHAR_STARTITEMARRAY ; i < CHAR_MAXITEMHAVE;i++ ){
		itemindex=CHAR_getItemIndex( talker , i );
		if( ITEM_CHECKINDEX(itemindex) ){
			id=ITEM_getInt(itemindex ,ITEM_ID );
			if( itemno == id ){
				nums += ITEM_getInt( itemindex, ITEM_USEPILENUMS);//数量
			}
		}
	}

	if( nums == kosuu ) return TRUE;

	return FALSE;
}
#endif

BOOL NPC_ActionFreeIfCheck(int meindex,int talker, char* buf, char *opt, int kosuu,int flg, int temp)
{
#ifdef _VIP_SHOP
	if(strcmp(buf,"VIPPOINT")==0){
		if(NPC_ActionVipPointCheck(meindex,talker,kosuu,flg)==TRUE)
				return TRUE;
	}
#endif
#ifdef _VIP_RIDE
	if(strcmp(buf,"VIP")==0){
		if(NPC_ActionVipCheck(meindex,talker,kosuu,flg)==TRUE)
				return TRUE;
	}
#endif

	if(strcmp(buf,"LV")==0){
		if(NPC_ActionLevelCheck(meindex,talker,kosuu,flg)==TRUE)
				return TRUE;
	}
	if(strcmp(buf,"GOLD")==0){
		if(NPC_ActionGoldCheck(meindex,talker,kosuu,flg)==TRUE)
				return TRUE;
	}
	
	if( strcmp(buf,"TRANS") == 0 )	{
		if( NPC_ActionCheckTrans( meindex, talker, kosuu, flg) == TRUE )
			return TRUE;
	}

	if( !strcmp( buf, "GTIME") ){
		if( NPC_CheckTimeDefine( opt) == TRUE )
			return TRUE;
	}

	if(strcmp( buf, "TIME") == 0) {
		if(NPC_TiemCheck( meindex, talker, kosuu, flg) == TRUE)
			return TRUE;
	}

	if( strcmp( buf, "PET") == 0 )	{
		if( temp > 0 )	{
			if( ActionCheckMyPet( meindex, talker, kosuu, flg, temp) == TRUE )
				return TRUE;
		}else
			return FALSE;
	}
	if(strcmp(buf,"ITEM")==0){
		if(NPC_ActionItemCheck(meindex,talker,kosuu, flg)==TRUE)
				return TRUE;
	}

	if(strcmp(buf,"EQUIT")==0){
		if(NPC_ActioneEquitCheck(meindex,talker,kosuu, flg)==TRUE)
				return TRUE;
	}

	if( !strcmp(buf,"ENDEV") || !strcmp(buf,"EVEND") ){
		if(NPC_EventCheckFlg( talker, kosuu)==TRUE)
				return TRUE;
	}
	if( !strcmp(buf,"NOWEV") || !strcmp(buf,"EVNOW") ){
		if(NPC_NowEventCheckFlg( talker, kosuu)==TRUE)
				return TRUE;
	}

	if(strcmp(buf,"PARTY")==0){
		if(NPC_ActionPartyCheck( talker, kosuu, flg)==TRUE)
				return TRUE;
	}
	if(strcmp(buf,"reITEM")==0){
		if( NPC_ActionReItemCheck( talker, kosuu, flg)==TRUE)
				return TRUE;
	}
	if(strcmp(buf,"rePET")==0){
		if( NPC_ActionRePetCheck( talker, kosuu, flg)==TRUE)
				return TRUE;
	}

#ifdef _NPCCHANGE_PLAYERIMG
	if(strcmp( buf, "BBI")==0)	{
		if( NPC_CheckPlayerBBI( meindex, talker, kosuu, flg)== TRUE)	{
			return TRUE;
		}
	}
#endif
	if( strcmp( buf, "FM") == 0 ){
		if( CHAR_getInt( talker, CHAR_FMLEADERFLAG ) > 0 && 
			CHAR_getInt( talker, CHAR_FMLEADERFLAG ) != FMMEMBER_APPLY)
		if( CHAR_getWorkInt( talker, CHAR_WORKFMFLOOR) == kosuu ){
			return TRUE;
		}
	}
	if(strcmp(buf,"DR")==0){
		if(NPC_ActionDuelRankCheck(meindex,talker,kosuu,flg)==TRUE){
				return TRUE;
		}
	}
	if(strcmp(buf,"DP")==0){
		if(NPC_ActionDuelPointCheck(meindex,talker,kosuu,flg)==TRUE){
				return TRUE;
		}
	}

#ifdef _TREASURE_BOX
	if( strcmp( buf, "BOX") == 0 ){
		if( NPC_ActionTreasureTypeCheck( meindex, kosuu,
			//NPC_TIME_EVENTMODE = CHAR_NPCWORKINT6,// 1
			CHAR_getWorkInt( meindex, CHAR_NPCWORKINT6) ,flg)==TRUE)
				return TRUE;
	}
#endif

#ifdef _NPC_ADDWARPMAN1
    if( strcmp( buf, "PARTYCOUNT") == 0){
	    if(NPC_ActionPartyCountCheck( talker, kosuu, flg)==TRUE)
			return TRUE;
	}
	if( strcmp( buf, "MANCOUNT" ) == 0 ){
        if(NPC_ActionManCountCheck( talker, kosuu, flg ) == TRUE )
			return TRUE;
	}
	if( strcmp( buf, "WOMANCOUNT" ) == 0 ){
        if(NPC_ActionWomanCountCheck( talker, kosuu, flg ) == TRUE )
			return TRUE;
	}
#endif

	return FALSE;
}

BOOL NPC_ActioneEquitCheck(int meindex,int talker,int itemNo, int flg)
{
	int i;
	int itemindex=-1;
	int id;
	for( i=0;i<CHAR_STARTITEMARRAY;i++ ){
		itemindex = CHAR_getItemIndex( talker , i );
		if( ITEM_CHECKINDEX( itemindex) )	{
			id=ITEM_getInt(itemindex ,ITEM_ID );
			if( NPC_ActionBigSmallLastCheck(itemNo,id,flg) == TRUE )
				return TRUE;
		}
	}

	return FALSE;
}

BOOL NPC_ActionItemCheck(int meindex,int talker,int itemNo, int flg)
{
	int i;
	int itemindex=-1;
	int id;
	for( i=CHAR_STARTITEMARRAY;i<CHAR_MAXITEMHAVE;i++ ){
		itemindex = CHAR_getItemIndex( talker , i );
		if( ITEM_CHECKINDEX( itemindex) )	{
			id=ITEM_getInt(itemindex ,ITEM_ID );
			if( NPC_ActionBigSmallLastCheck(itemNo,id,flg) == TRUE )
				return TRUE;
		}
	}

	return FALSE;
}

BOOL NPC_ActionBigSmallLastCheck(int point1,int mypoint,int flg)
{
	if(flg==0){
		if(point1==mypoint)
			return TRUE;
	}else if(flg==1){
		if(mypoint < point1)
			return TRUE;
	}else if(flg==2){
		if(mypoint > point1)
			return TRUE;
	}

	return FALSE;
}

BOOL ActionCheckMyPet( int meindex, int talker, int petLv, int flg, int petid)
{
	int petsel,petindex=-1;	
	int lv=-1;
	for(petsel=0 ; petsel < CHAR_MAXPETHAVE ; petsel++)     {
		petindex = CHAR_getCharPet( talker, petsel);
		if( petindex == -1  )
			continue;
	        if(CHAR_getInt( petindex, CHAR_PETID) !=  petid )
			continue;
		break;
	}
	if( petsel  == CHAR_MAXPETHAVE )	{
		return FALSE;
	}else	{	//找到条件宠
		lv = CHAR_getInt( petindex, CHAR_LV);
		if( NPC_ActionBigSmallLastCheck( petLv, lv, flg) == TRUE)	{
			return TRUE;
		}else	{
			return FALSE;
		}
	}
	return FALSE;                                             
}


BOOL NPC_ActionCheckTrans(int meindex,int talker,int trans,int flg)
{
	int myTrans;
	myTrans=CHAR_getInt(talker, CHAR_TRANSMIGRATION);
	if( NPC_ActionBigSmallLastCheck( trans, myTrans, flg ) == TRUE )	{
             return TRUE;
        }                   
	return FALSE;
}

#ifdef _VIP_SHOP
BOOL NPC_ActionVipPointCheck(int meindex,int talker,int point,int flg)
{
	int mypoint;
	mypoint=CHAR_getInt(talker,CHAR_AMPOINT);
	if(NPC_ActionBigSmallLastCheck( point, mypoint, flg) == TRUE){
		return TRUE;
	}
	return FALSE;
}
#endif
#ifdef _VIP_RIDE
BOOL NPC_ActionVipCheck(int meindex,int talker,int vip,int flg)
{
	if(NPC_ActionBigSmallLastCheck( CHAR_getInt(talker,CHAR_VIPRIDE), vip, flg) == TRUE){
		return TRUE;
	}
	return FALSE;
}
#endif

BOOL NPC_ActionLevelCheck(int meindex,int talker,int level,int flg)
{
	int mylevel;
	mylevel=CHAR_getInt(talker,CHAR_LV);
	if(NPC_ActionBigSmallLastCheck( level, mylevel, flg) == TRUE){
		return TRUE;
	}
	return FALSE;
}

BOOL NPC_ActionPartyCheck( int toindex, int nums, int flg)
{
	int partys=0;
	int leaderindex=-1;
	if( CHAR_getWorkInt( toindex, CHAR_WORKPARTYMODE) == CHAR_PARTY_NONE ){
		partys = 0;
		leaderindex = toindex;
	}else if( CHAR_getWorkInt( toindex, CHAR_WORKPARTYMODE) == CHAR_PARTY_CLIENT ){
		leaderindex = CHAR_getWorkInt( toindex, CHAR_WORKPARTYINDEX1);
	}

	if( CHAR_getWorkInt( leaderindex, CHAR_WORKPARTYMODE) == CHAR_PARTY_LEADER ){
		int i;
		for( i=0; i<5; i++)	{
			int pindex = CHAR_getWorkInt( leaderindex, CHAR_WORKPARTYINDEX1+i);
			if( !CHAR_CHECKINDEX( pindex) ) continue;
				partys++;
		}
	}

	if(NPC_ActionBigSmallLastCheck( nums, partys, flg) == TRUE){
		return TRUE;
	}
	return FALSE;
}

BOOL NPC_ActionReItemCheck( int toindex, int nums, int flg)
{
	int remnants=0;
	remnants = CHAR_findSurplusItemBox( toindex);
	if(NPC_ActionBigSmallLastCheck( nums, remnants, flg) == TRUE){
		return TRUE;
	}
	return FALSE;
}
BOOL NPC_ActionRePetCheck( int toindex, int nums, int flg)
{
	int remnants=0;
	remnants = CHAR_findSurplusPetBox( toindex);
	if(NPC_ActionBigSmallLastCheck( nums, remnants, flg) == TRUE){
		return TRUE;
	}
	return FALSE;
}

BOOL NPC_ActionGoldCheck(int meindex,int talker,int Golds,int flg)
{
	int myGold;
	myGold=CHAR_getInt(talker,CHAR_GOLD);
	if(NPC_ActionBigSmallLastCheck( Golds, myGold, flg) == TRUE){
		return TRUE;
	}
	return FALSE;
}

BOOL NPC_ActionWarpManReduce(int meindex,int talker,char *buf)
{
	char buf2[512];
	char buf3[256];
	int i, itemindex, itemno, kosuu;
	int id=0;
	int cnt =0;
	getStringFromIndexWithDelim(buf,"=",2,buf2,sizeof(buf2));
	getStringFromIndexWithDelim(buf2,"*",1,buf3,sizeof(buf3));
	itemno = atoi(buf3);
	getStringFromIndexWithDelim(buf2,"*",2,buf3,sizeof(buf3));
	kosuu = atoi(buf3);

	for( i=CHAR_STARTITEMARRAY ; i < CHAR_MAXITEMHAVE;i++ ){
		itemindex=CHAR_getItemIndex( talker , i );
		if( ITEM_CHECKINDEX(itemindex) ){
			id=ITEM_getInt(itemindex ,ITEM_ID );
			if( itemno == id ){
				cnt++;
				if(cnt==kosuu){
					return TRUE;
				}
			}
		}
	}

	return FALSE;

}

#ifdef _NPCCHANGE_PLAYERIMG	//andy
BOOL NPC_ActionChangePlayerBBI( int meindex, int charindex, char *Img)
{
	int cBBI;
	char buf1[256];
	memset( buf1, 0, sizeof( buf1));
	if( !CHAR_CHECKINDEX( charindex) )
		return FALSE;
	if( !CHAR_CHECKINDEX( meindex) )
		return FALSE;
#ifdef _FIXBUG_ATTACKBOW
	{
		int armtype;
		int itemindex = CHAR_getItemIndex( charindex, CHAR_ARM);
		if( ITEM_CHECKINDEX(itemindex) ){
			armtype = ITEM_getInt( itemindex, ITEM_TYPE );
			if( armtype == ITEM_BOW || armtype == ITEM_BOUNDTHROW ||
				armtype == ITEM_BREAKTHROW || armtype == ITEM_BOOMERANG)	{
				CHAR_talkToCli( charindex, -1, "使用远距离武器无法变身！", CHAR_COLORYELLOW );
				return FALSE;
			}
		}

	}
#endif
#ifdef _PETSKILL_BECOMEPIG
    if( CHAR_getInt( charindex, CHAR_BECOMEPIG) > -1 ){//变成乌力了
		CHAR_talkToCli( charindex, -1, "无法变身，乌力化中不能变身！", CHAR_COLORYELLOW );
	    return FALSE;
	}
#endif
	if( CHAR_getInt( charindex, CHAR_RIDEPET) != -1 )	{//骑宠不换图
		CHAR_setInt( charindex, CHAR_RIDEPET, -1 );
		BATTLE_changeRideImage( charindex );
		CHAR_talkToCli( charindex, -1, "取消骑宠状态。", CHAR_COLORYELLOW);
		
		//change -----> 修改bug(落马後,宠物选项的状态依然为"骑乘",这里修正过来 
		CHAR_complianceParameter( charindex );
		CHAR_send_P_StatusString( charindex, CHAR_P_STRING_RIDEPET);
		
		//return FALSE;
	}
#ifdef _ITEM_METAMO
	CHAR_setWorkInt( charindex, CHAR_WORKITEMMETAMO, 0);
#endif

	if( Img == NULL || !strcmp( Img, "\0"))
		return FALSE;
//=======================
	CHAR_setWorkInt( charindex, CHAR_WORKNPCMETAMO, meindex);
	if( strstr( Img, "NPCBBI") != NULL )	{
		cBBI = CHAR_getInt( meindex, CHAR_BASEBASEIMAGENUMBER);
		sprintf( buf1, "你化身成了%s的样子。", CHAR_getChar( meindex, CHAR_NAME));
	}else if( strstr( Img, "MYBBI") != NULL )	{
		cBBI = CHAR_getInt( charindex, CHAR_BASEBASEIMAGENUMBER);
		CHAR_setWorkInt( charindex, CHAR_WORKNPCMETAMO, 0);
		sprintf( buf1, "变回了原来的样子。");
	}else if( strstr( Img, ",") != NULL )	{
		int imgindex=1, maxImg=0;
		while( getStringFromIndexWithDelim( Img, ",", imgindex, buf1, sizeof( buf1)) != FALSE )	{
			imgindex++;
			maxImg++;
		}
		if( (imgindex = (rand()%maxImg)) > maxImg) imgindex = 1;
		if( imgindex < 1 ) imgindex = 1;

		if( getStringFromIndexWithDelim( Img, ",", imgindex, buf1, sizeof( buf1)) == FALSE )
			return FALSE;
		cBBI = atoi( buf1);
		sprintf( buf1, "你样子改变了。");
	}else	{
		cBBI = atoi( Img);
		sprintf( buf1, "你样子改变了。");
	}
//=======================
	if( cBBI < 0 )	{
		return FALSE;
	}
	CHAR_talkToCli( charindex, -1, buf1, CHAR_COLORYELLOW);
	//CHAR_BASEBASEIMAGENUMBER	CHAR_BASEIMAGENUMBER
	CHAR_setInt( charindex, CHAR_BASEIMAGENUMBER, cBBI);
	CHAR_complianceParameter( charindex );
	CHAR_sendCToArroundCharacter( CHAR_getWorkInt( charindex , CHAR_WORKOBJINDEX ));
	CHAR_send_P_StatusString( charindex , CHAR_P_STRING_BASEBASEIMAGENUMBER);
	//CHAR_K_STRING_BASEIMAGENUMBER
	//CHAR_P_STRING_BASEBASEIMAGENUMBER
	return TRUE;
}
#endif

#ifdef _NPCCHANGE_PLAYERIMG
BOOL NPC_CheckPlayerBBI( int meindex, int charindex, int BBI, int flg)
{
	int MyBBI;
	if( !CHAR_CHECKINDEX( charindex))
		return FALSE;
	if( BBI < 0 )
		return FALSE;
	MyBBI = CHAR_getInt( charindex, CHAR_BASEIMAGENUMBER);
	return NPC_ActionBigSmallLastCheck( BBI, MyBBI,flg);
}
#endif

BOOL showString( int meindex, char *showstr,int flg)
{	//广播
	char buf1[256];
	char argstr[NPC_UTIL_GETARGSTR_BUFSIZE];
	int toindex = -1;
	int floor;
	int i;
	if( NPC_Util_GetArgStr( meindex, argstr, sizeof( argstr)) == NULL )	{
		print("\n not found argstr ");
		return FALSE;
	}
	if( NPC_Util_GetStrFromStrWithDelim( argstr, showstr,buf1, sizeof(buf1)) == NULL ) {
		strcpy( buf1, showstr);
	}
	toindex = -1;
	floor = CHAR_getInt( meindex , CHAR_FLOOR);
	while( toindex < 1000)	{
		toindex ++;
		if( !CHAR_CHECKINDEX( toindex) )
			continue;
		if( CHAR_getInt( toindex , CHAR_WHICHTYPE ) != CHAR_TYPEPLAYER )
			continue;
		if( CHAR_getInt( toindex , CHAR_FLOOR) != floor )
			continue;
		CHAR_talkToCli( toindex, meindex, buf1,  CHAR_COLORYELLOW);
		if( flg == 1 )	{
			CHAR_setWorkInt( toindex, CHAR_WORKSTAKEFLAG, FALSE );
			for( i=0; i<5; i++)	{
				CHAR_setWorkInt( toindex, i+CHAR_WORKSTAKETYPE1, 0);
			}
		}
	}
	return TRUE;
}

BOOL NPC_getTimeXYPoint( int meindex, int Mode)
{
	char npcarg[NPC_UTIL_GETARGSTR_BUFSIZE];
	char buf[256], buf1[256], buf2[256];
	int fl=-1, x, y;
	int GraNo = -1;
	int points = 1;	//条件成立为1
	if( Mode <= 0 )	{
		points = 2;
	}
	memset( npcarg, 0, sizeof( npcarg));
	if( !CHAR_CHECKINDEX( meindex) ) return FALSE;
	if( NPC_Util_GetArgStr( meindex, npcarg, sizeof(npcarg)) == NULL ){
		print("WarpMan:GetArgStrErr");
		return FALSE;
	}
	memset( buf, 0, sizeof( buf));
	if( NPC_Util_GetStrFromStrWithDelim( npcarg, "TimeXYPoint", buf, sizeof( buf)) != NULL ){
		if( getStringFromIndexWithDelim(buf , "&" , points, buf1, sizeof( buf1)) != FALSE ){
			memset( buf2, 0, sizeof( buf2));
			if( getStringFromIndexWithDelim(buf1 , "," , 1, buf2, sizeof( buf2)) == FALSE ){
				return FALSE;
			}
			fl = atoi( buf2);
			if( getStringFromIndexWithDelim(buf1 , "," , 2, buf2, sizeof( buf2)) == FALSE ){
				return FALSE;
			}
			x = atoi( buf2);
			if( getStringFromIndexWithDelim(buf1 , "," , 3, buf2, sizeof( buf2)) == FALSE ){
				return FALSE;
			}
			y = atoi( buf2);
			if( MAP_IsValidCoordinate( fl, x, y ) == FALSE ){
				char filename[256];
				NPC_Util_CheckAssignArgFile( meindex, filename);
				return FALSE;
			}
			if( CHAR_getInt( meindex, CHAR_FLOOR) != fl ||
				CHAR_getInt( meindex, CHAR_X) != x ||
				CHAR_getInt( meindex, CHAR_Y) != y ){
				CHAR_warpToSpecificPoint( meindex, fl , x , y);
			}
		}
	}

	if( NPC_Util_GetStrFromStrWithDelim( npcarg, "GraNo", buf, sizeof( buf)) != NULL ){
		if( getStringFromIndexWithDelim(buf , "&" , points, buf1, sizeof( buf1)) != FALSE ){
			GraNo = atoi( buf1);
			CHAR_setInt( meindex, CHAR_BASEIMAGENUMBER, GraNo);
		}
	}
	return TRUE;
}

BOOL NPC_getTimeData( int meindex, char *npcarg, int *born, int *dead, int loopspeed)
{
	int i;
	char buf[256];
	if( NPC_Util_GetStrFromStrWithDelim( npcarg, "NEWTIME", buf, sizeof( buf)) != NULL ){
		for(i=0 ; i<9 ; i++){
			if( strstr( buf, TimeTble[i].arg ) != NULL){
				*born = TimeTble[i].born;
				*dead = TimeTble[i].dead;
				return TRUE;
			}
		}
	}
	return FALSE;
}

BOOL NPC_CheckTimeDefine( char *timetype)
{
	LSTIME nowlstime;
	int born=-1, dead=-1, finds=0, i;

	RealTimeToLSTime( NowTime.tv_sec, &nowlstime);

	for(i=0 ; i<9 ; i++){
		if( strstr( timetype, TimeTble[i].arg ) != NULL){
			born = TimeTble[i].born;
			dead = TimeTble[i].dead;
			finds = 1;
			break;
		}
	}
	if( finds == 0 ) return FALSE;

	if( born < dead ){
		if( (born < nowlstime.hour) && (dead > nowlstime.hour) ){
			return TRUE;
		}
	}else{
		if( (born < nowlstime.hour && 1024 > nowlstime.hour) ||
			( 0 < nowlstime.hour && dead > nowlstime.hour)){
			return TRUE;
		}
	}

	return FALSE;
}

void NPC_TimeDefineDo( int meindex, int born, int dead, int typeMode)
{
	LSTIME nowlstime;
	RealTimeToLSTime( NowTime.tv_sec, &nowlstime);
	if(born < dead){
		if( (born < nowlstime.hour) && (dead > nowlstime.hour) ){
			CHAR_setWorkInt(meindex,typeMode,0);		
		}else{
			CHAR_setWorkInt(meindex,typeMode,1);
		}
	}else{
		if( (born < nowlstime.hour && 1024 > nowlstime.hour) || ( 0 < nowlstime.hour && dead > nowlstime.hour)){
			CHAR_setWorkInt(meindex,typeMode,0);
		}else{
			CHAR_setWorkInt(meindex,typeMode,1);
		}
	}
	if( NPC_getTimeXYPoint( meindex, CHAR_getWorkInt( meindex, typeMode)) == FALSE )
		return;
}


void GetRand_WarpPoint( char *buf, int *fl, int *x, int *y )	
{
	char buff1[256],buff2[256];
	int i=0,point=1;
	struct {
		int m_fl;
		int m_x;
		int m_y;
	} RandPoint[]={
		{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},
		{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},
		{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},
		{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},
	};
		while( getStringFromIndexWithDelim( buf,";",point,buff1,sizeof( buff1)) != FALSE) {
			point++;
			getStringFromIndexWithDelim(buff1,",",1,buff2,sizeof(buff2));
			RandPoint[i].m_fl = atoi(buff2);
			getStringFromIndexWithDelim(buff1,",",2,buff2,sizeof(buff2));
			RandPoint[i].m_x = atoi(buff2);
			getStringFromIndexWithDelim(buff1,",",3,buff2,sizeof(buff2));
			RandPoint[i].m_y = atoi(buff2);
			i++;
			if( i >= arraysizeof( RandPoint) )
				break;
		}
		
		point = RAND( 0, i);
		if( RandPoint[point].m_fl <= 0 || point > arraysizeof( RandPoint) -1)	{
			point = 0;
		}
		*fl = RandPoint[point].m_fl;
		*x = RandPoint[point].m_x;
		*y = RandPoint[point].m_y;

	return;
}

BOOL NPC_ActionDuelRankCheck(int meindex,int talker,int rank,int flg)
{
	int myrank;
	myrank=CHAR_getWorkInt(talker,CHAR_WORKSHOPRELEVANTSEC);
	if( NPC_ActionBigSmallLastCheck( rank, myrank, flg)==TRUE){
		return TRUE;
	}
	
	return FALSE;
}

BOOL NPC_ActionDuelPointCheck(int meindex,int talker,int rank,int flg)
{
	int myduel;
	myduel = CHAR_getInt( talker, CHAR_DUELPOINT);
	if( NPC_ActionBigSmallLastCheck( rank, myduel, flg)==TRUE){
		return TRUE;
	}
	
	return FALSE;
}

BOOL NPC_ActionTreasureTypeCheck(int meindex,int level, int type, int flg)
{
	int mylevel;

//	if( CHAR_getWorkInt( meindex, NPC_TIME_EVENTMODE) == NPC_EVENTMODE_EVENT ){
	mylevel = (type == NPC_EVENTMODE_EVENT)?1:0;
	if( NPC_ActionBigSmallLastCheck( level, mylevel, flg)==TRUE)
		return TRUE;
	return FALSE;
}

BOOL NPC_ActionTreasureRandItemGet(int meidex,int talker,int rand_j,char *buf)
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
	if( ret < 0 || ret >= CHAR_MAXITEMHAVE ) {
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

#ifdef _NPC_ADDWARPMAN1
BOOL NPC_ActionPartyCountCheck( int toindex, int nums, int flg)
{
	int partys=0;
	int i;

	partys = 0;
	for( i=0 ; i < CHAR_PARTYMAX ;i++)
		if(CHAR_getWorkInt(toindex,CHAR_WORKPARTYINDEX1+i) != -1)
			partys++;

	if(NPC_ActionBigSmallLastCheck( nums, partys, flg) == TRUE)
		return TRUE;

	return FALSE;
}
//检查男生人数
BOOL NPC_ActionManCountCheck( int toindex, int nums, int flg)
{
	int i,tempcount = 0;
	if( CHAR_getWorkInt( toindex, CHAR_WORKPARTYMODE) == CHAR_PARTY_NONE ){
	    if( IsMale(toindex) == 1 )
		    tempcount++;
	}
	else{
	    for( i=0 ; i < CHAR_PARTYMAX ;i++)
	        if(CHAR_getWorkInt(toindex,CHAR_WORKPARTYINDEX1+i) != -1)
			    if( IsMale( CHAR_getWorkInt(toindex,CHAR_WORKPARTYINDEX1+i) ) == 1 )
				    tempcount++;
	}

	print("\n检查男生人数:%d",tempcount);
	if(NPC_ActionBigSmallLastCheck( nums, tempcount, flg) == TRUE)
		return TRUE;

	return FALSE;
}
//检查女生人数
BOOL NPC_ActionWomanCountCheck( int toindex, int nums, int flg)
{
	int i,tempcount = 0;
	if( CHAR_getWorkInt( toindex, CHAR_WORKPARTYMODE) == CHAR_PARTY_NONE ){
	    if( IsFemale(toindex) == 1 )
		    tempcount++;
	}
    else{
	    for( i=0 ; i < CHAR_PARTYMAX ;i++)
	        if(CHAR_getWorkInt(toindex,CHAR_WORKPARTYINDEX1+i) != -1)
		        if( IsFemale( CHAR_getWorkInt(toindex,CHAR_WORKPARTYINDEX1+i) ) == 1 )
				    tempcount++;
	}
    print("\n检查女生人数:%d",tempcount);
	if(NPC_ActionBigSmallLastCheck( nums, tempcount, flg) == TRUE)
		return TRUE;

	return FALSE;
}
#endif

