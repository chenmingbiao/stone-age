#include "version.h"
#include <string.h>
#include "char.h"
#include "object.h"
#include "char_base.h"
#include "npcutil.h"
#include "npc_npcenemy.h"
#include "battle.h"
#include "enemy.h"
#include "readmap.h"
#include "encount.h"
#include "lssproto_serv.h"
#include "configfile.h"
#include "anim_tbl.h"
#include "handletime.h"
#include "npc_eventaction.h"

enum {
	CHAR_WORK_ENCOUNTTYPE	= CHAR_NPCWORKINT1,	/* ¾Þ¼þÊÐËü¼þÐþ¼°¿á   */
	CHAR_WORK_DIEACT		= CHAR_NPCWORKINT2,	/* ÉØÊÏ·ÖÁÝÉýµ¤ÔÊÔÂ¾® */
	CHAR_WORK_WARP_FLOOR	= CHAR_NPCWORKINT3,	/* ·¥¡õÃóÔÊÔÂ°×·òÊ§*/
	CHAR_WORK_WARP_X		= CHAR_NPCWORKINT4,	/* ·¥¡õÃóÔÊÔÂX */
	CHAR_WORK_WARP_Y		= CHAR_NPCWORKINT5,	/* ·¥¡õÃóÔÊÔÂY */
	CHAR_WORK_ONEBATTLE 	= CHAR_NPCWORKINT6,	/* Óò¹´ØÆ¾®¾ô  ØÆØ¦ÖÐ */
	NPC_TIME_MODE			= CHAR_NPCWORKINT7, 
	CHAR_WORK_BASEIMGBAK	= CHAR_NPCWORKINT8, /* ÊÖÎç¼°  ì«  Ä¯ */
	CHAR_WORK_DIETIME		= CHAR_NPCWORKINT9, /* ÉØÊÏ·ÖÁÝÃÞ */
	CHAR_WORK_REVIVALTIME	= CHAR_NPCWORKINT10, /* Ï·Îåß¯ÔÂÁÝÃÞ */
	CHAR_WORK_BATTLETYPE	= CHAR_NPCWORKINT11,
};

#define		NPC_ENEMY_REVIVALTIMA_DEFAULT	120

static int gymbody[] = {
	SPR_001em,SPR_011em,SPR_021em,SPR_031em,SPR_041em,SPR_051em,
	SPR_061em,SPR_071em,SPR_081em,SPR_091em,SPR_101em,SPR_111em,

	SPR_002em,SPR_012em,SPR_022em,SPR_032em,SPR_042em,SPR_052em,
	SPR_062em,SPR_072em,SPR_082em,SPR_092em,SPR_102em,SPR_112em,

	SPR_003em,SPR_013em,SPR_023em,SPR_033em,SPR_043em,SPR_053em,
	SPR_063em,SPR_073em,SPR_083em,SPR_093em,SPR_103em,SPR_113em,

	SPR_004em,SPR_014em,SPR_024em,SPR_034em,SPR_044em,SPR_054em,
	SPR_064em,SPR_074em,SPR_084em,SPR_094em,SPR_104em,SPR_114em,
};

#ifdef _NEW_WARPMAN
static BOOL NPC_NPCEnemy_CheckFree( int  meindex, int toindex, BOOL *Party);
BOOL NPC_WarpAllMsg(int meindex,int toindex );
BOOL NPCEnemy_CheckFree( int meindex, int talker, char *buf);
BOOL NPCEnemy_BSCheck(int meindex,int talker,char* buf);
BOOL NPCEnemy_FreeIfCheck(int meindex,int talker,char* buf,int kosuu,int flg, int temp);
BOOL NPCEnemy_WarpManReduce(int meindex,int talker,char *buf);
BOOL NPCEnemy_BigSmallLastCheck(int point1,int mypoint,int flg);
BOOL NPCEnemy_CheckTrans(int meindex,int talker,int trans,int flg);
BOOL NPCEnemy_LevelCheck(int meindex,int talker,int level,int flg);
BOOL NPCEnemy_CheckMyPet( int meindex, int talker, int petLv, int flg, int petid);
BOOL NPCEnemy_ItemCheck(int meindex,int talker,int itemNo,int flg);
#endif
#ifdef _NPC_REPLACEMENT
void Check_EnemyWarpMe( int meindex, char *args );
#endif
static int NPC_NPCEnemy_StealItem( char *argstr, int meindex, int charaindex);

BOOL NPC_NPCEnemyInit( int meindex )
{
	char	argstr[NPC_UTIL_GETARGSTR_BUFSIZE];
	char	buf[64];
	int		tmp, gym;


	if( NPC_Util_GetArgStr( meindex, argstr, sizeof( argstr)) == NULL )	{
		print("Can't GetArgStr !!");
		return FALSE;
	}
	if( NPC_Util_GetStrFromStrWithDelim( argstr, "sktype", buf, sizeof( buf)) != NULL ){
		CHAR_setWorkInt( meindex, CHAR_WORK_BATTLETYPE, atoi( buf));
	}else{
		CHAR_setWorkInt( meindex, CHAR_WORK_BATTLETYPE, 0);
	}

	if( NPC_Util_GetStrFromStrWithDelim( argstr, "enemyno", buf, sizeof( buf)) == NULL ){
		char filename[256];
		NPC_Util_CheckAssignArgFile( meindex, filename);
		print( "NPCEnemy: nothing parameter [enemyno]:\n%s\n", filename);
		return FALSE;
	}else {
		int		i;
		for( i = 0; i < NPC_ENEMY_ENEMYNUMBER; i ++ ) {
			int		curEnemy;
			char	data[128];
			int		ret;
			ret = getStringFromIndexWithDelim( buf,",", i + 1, data, sizeof(data));
			if( ret == FALSE)
				break;
			curEnemy = ENEMY_getEnemyArrayFromId( atoi( data));
			if( !ENEMY_CHECKINDEX( curEnemy) ) {
				char filename[256];
				NPC_Util_CheckAssignArgFile( meindex, filename);
				print( "NPCEnemy: invalid param [enemyno:%d data:%s curEnemy:%d]\nfile:%s\n",i+1, data, curEnemy, filename );
				return FALSE;
			}
		}
	}
	
	if( NPC_Util_GetStrFromStrWithDelim( argstr, "steal", buf, sizeof( buf)) != NULL ) {
		if( NPC_Util_GetStrFromStrWithDelim( argstr, "item", buf, sizeof( buf)) == NULL ) {
			print( "NPCEnemy:ÓÐÖ¸¶¨steal¶øÃ»ÓÐÖ¸¶¨item£¬ºÜÆæ¹Ö\n");
			return FALSE;
		}
	}

	gym = NPC_Util_GetNumFromStrWithDelim( argstr, "gym");

	if( gym > 0 ){
		CHAR_setInt( meindex, CHAR_BASEBASEIMAGENUMBER,
			gymbody[RAND( 0, arraysizeof( gymbody ) - 1)] );
		CHAR_setInt( meindex, CHAR_BASEIMAGENUMBER,
			CHAR_getInt( meindex, CHAR_BASEBASEIMAGENUMBER ) );
		CHAR_setInt( meindex, CHAR_LV, gym );
	}
	tmp = NPC_Util_GetNumFromStrWithDelim( argstr, "dieact");
	if( tmp != 1 ) 
		tmp = 0;
	CHAR_setWorkInt( meindex, CHAR_WORK_DIEACT, tmp);
	if( strstr( argstr, "NEWNPCENEMY") != 0 )	{
		CHAR_setWorkInt( meindex, CHAR_WORK_WARP_FLOOR, 0 );
		CHAR_setWorkInt( meindex, CHAR_WORK_WARP_Y, 0 );
		CHAR_setWorkInt( meindex, CHAR_WORK_WARP_X, 0 );
	}else	{
		if( CHAR_getWorkInt( meindex, CHAR_WORK_DIEACT) == 1 ) {
			char buff1[256];
			if( NPC_Util_GetStrFromStrWithDelim( argstr, "warpfl", buff1, sizeof( buff1) ) == NULL) {
				print( "NPCENEMY: nothing warp_floor %s\n", buff1);
				return FALSE;
			}
			tmp = atoi( buff1);
			CHAR_setWorkInt( meindex, CHAR_WORK_WARP_FLOOR, tmp);
			if( NPC_Util_GetStrFromStrWithDelim( argstr, "warpx", buff1, sizeof( buff1) ) == NULL) {
				print( "NPCENEMY: nothing warp_x\n");
				return FALSE;
			}
			tmp = atoi( buff1);
			CHAR_setWorkInt( meindex, CHAR_WORK_WARP_X, tmp);
			if( NPC_Util_GetStrFromStrWithDelim( argstr, "warpy", buff1, sizeof( buff1) ) == NULL) {
				print( "NPCENEMY: nothing warp_y\n");
				return FALSE;
			}
			tmp = atoi( buff1);
			CHAR_setWorkInt( meindex, CHAR_WORK_WARP_Y, tmp);
			if( MAP_IsValidCoordinate( CHAR_getWorkInt( meindex, CHAR_WORK_WARP_FLOOR),
								CHAR_getWorkInt( meindex, CHAR_WORK_WARP_X),
								CHAR_getWorkInt( meindex, CHAR_WORK_WARP_Y) ) == FALSE ){
				char filename[256];
				NPC_Util_CheckAssignArgFile( meindex, filename);
				print( "NPCENEMY: %s: .\n %s.\nInvalid fl=%d,x=%d,y=%d\n", filename, argstr,
					CHAR_getWorkInt( meindex, CHAR_WORK_WARP_FLOOR),
					CHAR_getWorkInt( meindex, CHAR_WORK_WARP_X),
					CHAR_getWorkInt( meindex, CHAR_WORK_WARP_Y) );
				return FALSE;
			}
		}
	}
	//andy_end
	tmp = NPC_Util_GetNumFromStrWithDelim( argstr, "entype");
	if( tmp != 1 && tmp != 2)
		tmp = 0;
	CHAR_setWorkInt( meindex, CHAR_WORK_ENCOUNTTYPE, tmp);

	tmp = NPC_Util_GetNumFromStrWithDelim( argstr, "onebattle");
	if( tmp != 1 )
		tmp = 0;
	CHAR_setWorkInt( meindex, CHAR_WORK_ONEBATTLE, tmp);

	tmp = NPC_Util_GetNumFromStrWithDelim( argstr, "time");
	if( tmp == -1 )
		tmp = NPC_ENEMY_REVIVALTIMA_DEFAULT;
	CHAR_setWorkInt( meindex, CHAR_WORK_REVIVALTIME, tmp);

	CHAR_setInt( meindex , CHAR_WHICHTYPE , CHAR_TYPENPCENEMY );
	CHAR_setWorkInt( meindex, CHAR_WORKEVENTTYPE, CHAR_EVENT_ENEMY);
	CHAR_setWorkInt( meindex, CHAR_WORK_BASEIMGBAK,
						CHAR_getInt( meindex, CHAR_BASEIMAGENUMBER));
	return TRUE;
}

void NPC_NPCEnemyTalked( int meindex , int talkerindex , char *szMes ,
					 int color )
{
	char	npcarg[NPC_UTIL_GETARGSTR_BUFSIZE];
	char	buf[256];

	if( CHAR_getInt( talkerindex , CHAR_WHICHTYPE ) != CHAR_TYPEPLAYER ) {
		return;
	}
	if( NPC_Util_GetArgStr( meindex, npcarg, sizeof( npcarg)) == NULL )	{
		print("Can't GetArgStr !!");
		return;
	}
	if( NPC_Util_GetStrFromStrWithDelim( npcarg, "NEWTIME", buf, sizeof( buf)) != NULL ){
		if( CHAR_getWorkInt( meindex, NPC_TIME_MODE) <= 0 )	{
			if( NPC_Util_GetStrFromStrWithDelim( npcarg, "Time_Msg", buf, sizeof( buf) ) != NULL)	{
				CHAR_talkToCli( talkerindex, meindex, buf, CHAR_COLORYELLOW);
			}
			return;
		}
	}
	if( !NPC_Util_charIsInFrontOfChar( talkerindex, meindex, 1 ))
		return; 
	if( CHAR_getWorkInt( meindex, CHAR_WORK_ENCOUNTTYPE) >= 1 ) {
		NPC_NPCEnemy_Encount( meindex, talkerindex, 1);
	}
}

int NPC_NPCEnemy_Encount( int meindex, int charaindex, int mode)
{
	char	argstr[NPC_UTIL_GETARGSTR_BUFSIZE];
	char	buf[512];
	BOOL	flg = TRUE;
	int		battlemax = getBattlenum();
	int		i;
	if( CHAR_getInt( meindex, CHAR_BASEIMAGENUMBER) == 0 ) {
		return FALSE;
	}
	if( mode == 0 ) {
		if( CHAR_getWorkInt( meindex, CHAR_WORK_ENCOUNTTYPE) == 1 ) {
			flg = FALSE;
		}
	}else if( mode == 1 ) {
		if( CHAR_getWorkInt( meindex, CHAR_WORK_ENCOUNTTYPE) == 0 ) {
			flg = FALSE;
		}
	}

	NPC_Util_GetArgStr( meindex, argstr, sizeof( argstr));

	if( !flg) {
		if( NPC_Util_GetStrFromStrWithDelim( argstr, "deniedmsg", buf, sizeof( buf)) != NULL )	{
			CHAR_talkToCliAndParty( charaindex, meindex ,buf , CHAR_COLORYELLOW );
		}
		return FALSE;
	}
	flg = FALSE;
	while( 1 ) {
		char	buf[1024];
		if( NPC_Util_GetStrFromStrWithDelim( argstr, "item", buf, sizeof( buf)) != NULL ) {
			char	data[128];
			BOOL	found = FALSE;

			for( i = 0; i < CHAR_MAXITEMHAVE; i ++ ) {
				int		j;
				int		itemid;
				int ret = getStringFromIndexWithDelim( buf,",", i + 1,data, sizeof(data));
				if( ret == FALSE)
					break;
				found = FALSE;
				itemid = atoi( data);
				for( j = 0; j < CHAR_MAXITEMHAVE; j ++ ) {
					int itemindex = CHAR_getItemIndex( charaindex, j);
					if( ITEM_CHECKINDEX( itemindex)) {
						if( ITEM_getInt( itemindex, ITEM_ID) == itemid ) {
							found = TRUE;
							break;
						}
					}
				}
				if( !found )
					break;
			}
			if( !found )
				break;
		}
#ifdef _ADD_NOITEM_BATTLE
		if(NPC_Util_GetStrFromStrWithDelim(argstr,"noitem",buf,sizeof(buf)) != NULL){
			char data[128];
			BOOL found = FALSE;

			for(i=0;i<CHAR_MAXITEMHAVE;i++){
				int j;
				int itemid;
				int ret = getStringFromIndexWithDelim(buf,",",i + 1,data,sizeof(data));
				if(ret == FALSE) break;
				found = TRUE;
				itemid = atoi(data);
				for(j=0;j<CHAR_MAXITEMHAVE;j++){
					int itemindex = CHAR_getItemIndex(charaindex,j);
					if(ITEM_CHECKINDEX( itemindex)){
						if(ITEM_getInt(itemindex,ITEM_ID) == itemid){
							found = FALSE;
							break;
						}
					}
				}
				if(!found) break;
			}
			if(!found) break;
		}
#endif

		{
			char data[256];

			BOOL Evflg=TRUE;
			if( NPC_Util_GetStrFromStrWithDelim( argstr, "B_evend", buf, sizeof( buf)) != NULL ) {
				int i=1, event=-1;;
				while( getStringFromIndexWithDelim( buf,",", i, data, sizeof( data)) ){
					i++;
					event=atoi( data);
					if( NPC_EventCheckFlg( charaindex, event) == FALSE )	{
						Evflg = FALSE;
						break;
					}
				}
			}
			if( Evflg == FALSE )
				break;
			i=1;
			if( NPC_Util_GetStrFromStrWithDelim( argstr, "B_evnow", buf, sizeof( buf)) != NULL ) {
				int i=1, event=-1;;
				while( getStringFromIndexWithDelim( buf,",", i, data, sizeof( data)) ){
					i++;
					event=atoi( data);
					if( NPC_NowEventCheckFlg( charaindex, event) == FALSE )	{
						Evflg = FALSE;
						break;
					}
				}
			}
			if( Evflg == FALSE )
				break;
		}

		if( CHAR_getWorkInt( meindex, CHAR_WORK_ONEBATTLE ) == 1 ) {
			for( i = 0; i < battlemax; i ++ ) {
				if( BattleArray[i].use ) {
					if( BattleArray[i].createindex == meindex ) {
						break;
					}
				}
			}
			if( i != battlemax ) {
				if( NPC_Util_GetStrFromStrWithDelim( argstr, "alreadymsg", buf, sizeof( buf))!= NULL )
				{
					CHAR_talkToCliAndParty( charaindex, meindex ,buf , CHAR_COLORYELLOW );
				}
				return FALSE;
			}
		}
		flg = TRUE;
		break;
	}

	if( flg ) {
		/* ºÒ¼°ÁÝ·´  ÷»ÔÊÔÂ */
		if( CHAR_getWorkInt( charaindex, CHAR_WORKPARTYMODE) != CHAR_PARTY_CLIENT) {
			char	config[32];
			snprintf( config, sizeof( config), "askbattlemsg1");
			/* ¾ô  »¥ï§ÒýÔÂó¡±åß¤  ¶ªÓÀ±¾¡õ³â */
			
			
			if( NPC_Util_GetStrFromStrWithDelim( argstr, config,  buf, sizeof( buf))!= NULL )
			{
				int	len = strlen( buf);
				char	escapebuf[1024];
				int fd = getfdFromCharaIndex( charaindex);
				char	buf2[256];
				/* askbattlemsg? »¥Ø¤Ä¾ÈÉ  ÐÑµæ±åÔÊÔÂ  "\n"Îì¡õÓñÃ«  Ä¾ÔÂ)*/
				for( i = 2; i < 7; i ++ ) {
					snprintf( config, sizeof( config), "askbattlemsg%d", i);
					if( NPC_Util_GetStrFromStrWithDelim( argstr, config, buf2, sizeof( buf2))!= NULL )
					{
						strcpysafe( &buf[len], sizeof( buf)-len, "\n" );
						len++;
						strcpysafe( &buf[len], sizeof( buf)-len, buf2 );
						len += strlen( buf2);
						if( len >= sizeof( buf) ) {
							print( "buffer over\n");
							return FALSE;
						}
					}
					else {
						strcpysafe( &buf[len], sizeof( buf)-len, "\n" );
						len++;
						if( len >= sizeof( buf) ) {
							print( "buffer over\n");
							return FALSE;
						}
					}
				}

				if( fd != -1 ) {
					lssproto_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,
									WINDOW_BUTTONTYPE_YESNO,
									CHAR_WINDOWTYPE_NPCENEMY_START,
									CHAR_getWorkInt( meindex, CHAR_WORKOBJINDEX),
									makeEscapeString( buf, escapebuf,sizeof(escapebuf)));
				}
				/* ³ð³ð·ÖØê    ±åØøÕý¡õ¼þ */
				return FALSE;
			}else {
				/* ¾ô  ±å  ÈÕÁùÔÂ */
				flg = NPC_NPCEnemy_BattleIn( meindex, charaindex);
			}
		}
	}
	if( !flg) {
		if( NPC_Util_GetStrFromStrWithDelim( argstr, "deniedmsg", buf, sizeof( buf))!= NULL )
		{
			CHAR_talkToCliAndParty( charaindex, meindex ,buf , CHAR_COLORYELLOW );
		}
	}
	return flg;
}

int NPC_NPCEnemy_Dying( int battleindex, int meindex )
{
	char	argstr[NPC_UTIL_GETARGSTR_BUFSIZE];
	char	buf[64];
	int	i,toindex=-1;
	int tmp=-1;

	if( !CHAR_CHECKINDEX( meindex)) {
		return FALSE;
	}
	NPC_Util_GetArgStr( meindex, argstr, sizeof( argstr));

#ifdef _NEW_WARPMAN
	if( strstr( argstr, "NEWNPCENEMY") )    {
	}else	{
#endif
	/* Ó¬    ¼°ÚïÌ« */
	if( NPC_Util_GetStrFromStrWithDelim( argstr, "endmsg", buf, sizeof( buf)) != NULL ) {
		for( i = 0; i < 5; i ++ ) {
			/* ¾ô  ±å¸¨µÑØÆ»¯ÖÐÔÂòå¹»±å¶ªÓÀ±¾¡õ³â */
			int toindex = BattleArray[battleindex].Side[0].Entry[i].charaindex;
			if( CHAR_CHECKINDEX( toindex) )	{
				CHAR_talkToCli( toindex, meindex ,buf , CHAR_COLORYELLOW );
			}
		}

	}
#ifdef _NEW_WARPMAN
	}
#endif
	if( NPC_Util_GetStrFromStrWithDelim( argstr, "steal", buf, sizeof( buf)) != NULL ) {
		print("\n steal TRUE !!");
		if( atoi( buf) == 1 ) {
			int charaindex = BattleArray[battleindex].Side[0].Entry[0].charaindex;
			NPC_NPCEnemy_StealItem( argstr, meindex, charaindex);
		}
		
	}
#ifdef _ALLDOMAN // (²»¿É¿ª) Syu ADD ÅÅÐÐ°ñNPC
	if( NPC_Util_GetStrFromStrWithDelim( argstr, "herobattlefield", buf, sizeof( buf)) != NULL ) {
			int charaindex = BattleArray[battleindex].Side[0].Entry[0].charaindex;
			CHAR_setWorkInt ( charaindex , CHAR_WORKHEROFLOOR , atoi ( buf ) ) ;
			// Syu ADD 
			if ( atoi ( buf ) > CHAR_getInt ( charaindex , CHAR_HEROFLOOR ) )
				CHAR_setInt ( charaindex , CHAR_HEROFLOOR , atoi( buf ) ) ; 
	}

#endif
	tmp = NPC_Util_GetNumFromStrWithDelim( argstr, "dieact");
	if( tmp != 1 ) 
		tmp = 0;
	CHAR_setWorkInt( meindex, CHAR_WORK_DIEACT, tmp);
	if( CHAR_getWorkInt( meindex, CHAR_WORK_DIEACT) == 0 ) {
	    Char 	*ch;
		CHAR_setInt( meindex, CHAR_BASEIMAGENUMBER, 0);
		print("bbi---->bi10");
		CHAR_sendCToArroundCharacter( CHAR_getWorkInt( meindex, CHAR_WORKOBJINDEX));

		CHAR_setWorkInt( meindex, CHAR_WORKEVENTTYPE, CHAR_EVENT_ALTERRATIVE);
		MAP_sendAroundMapdata( CHAR_getInt( meindex, CHAR_FLOOR),
								CHAR_getInt( meindex, CHAR_X),
								CHAR_getInt( meindex, CHAR_Y));
	    ch  = CHAR_getCharPointer( meindex);
	    if( ch != NULL ) {
			strcpysafe( ch->charfunctable[CHAR_LOOPFUNC].string,
						sizeof( ch->charfunctable[CHAR_LOOPFUNC]),
						"NPCEnemyLoop");
			CHAR_constructFunctable( meindex);
			CHAR_setInt( meindex, CHAR_LOOPINTERVAL, 5000);
			CHAR_setWorkInt( meindex, CHAR_WORK_DIETIME, NowTime.tv_sec);
		}else {
			print( "NPCENEMY:err\n");
		}
	}else {
		for( i = 0; i < 5; i ++ ) {
			toindex = BattleArray[battleindex].Side[0].Entry[i].charaindex;
			if( CHAR_CHECKINDEX( toindex) ) {
#ifdef _NEW_WARPMAN
				if( strstr( argstr, "NEWNPCENEMY") )    {
					BOOL Party=TRUE;
					if( NPC_NPCEnemy_CheckFree( meindex, toindex , &Party) == FALSE )       {
						continue;
					}
					if( Party == TRUE )	{
					}else	{
						if( CHAR_getWorkInt( toindex, CHAR_WORKPARTYMODE ) == CHAR_PARTY_LEADER )	{
							if( NPC_WarpAllMsg( meindex, toindex ) == TRUE )	{
								return TRUE;
							}
						}
						continue;
					}
				}
				CHAR_DischargeParty( toindex, 0); // ²ð¶Ó
				CHAR_warpToSpecificPoint( toindex,
					CHAR_getWorkInt( meindex, CHAR_WORK_WARP_FLOOR),
					CHAR_getWorkInt( meindex, CHAR_WORK_WARP_X),
					CHAR_getWorkInt( meindex, CHAR_WORK_WARP_Y)
					);

#else
				CHAR_warpToSpecificPoint( toindex,
								CHAR_getWorkInt( meindex, CHAR_WORK_WARP_FLOOR),
								CHAR_getWorkInt( meindex, CHAR_WORK_WARP_X),
								CHAR_getWorkInt( meindex, CHAR_WORK_WARP_Y)
								);
#endif
#ifdef _NPC_REPLACEMENT
				Check_EnemyWarpMe( meindex, argstr);
#endif
			}
		}
	}
	return TRUE;
}

void NPC_NPCEnemyLoop( int meindex )
{

	if( NowTime.tv_sec > CHAR_getWorkInt( meindex, CHAR_WORK_DIETIME) +
						 CHAR_getWorkInt( meindex, CHAR_WORK_REVIVALTIME)){
		Char	*ch;
		CHAR_setInt( meindex, CHAR_BASEIMAGENUMBER,	CHAR_getWorkInt( meindex, CHAR_WORK_BASEIMGBAK));
		print("bbi---->bi11");

		CHAR_sendCToArroundCharacter( CHAR_getWorkInt( meindex, CHAR_WORKOBJINDEX));
		CHAR_setWorkInt( meindex, CHAR_WORKEVENTTYPE, CHAR_EVENT_ENEMY);
		MAP_sendAroundMapdata( CHAR_getInt( meindex, CHAR_FLOOR),
								CHAR_getInt( meindex, CHAR_X),
								CHAR_getInt( meindex, CHAR_Y));
	    ch  = CHAR_getCharPointer( meindex);
	    if( ch != NULL ) {
			strcpysafe( ch->charfunctable[CHAR_LOOPFUNC].string,
						sizeof( ch->charfunctable[CHAR_LOOPFUNC]),	"");
			CHAR_constructFunctable( meindex);
//			CHAR_setInt( meindex, CHAR_LOOPINTERVAL, 0);
		}
	}
}


BOOL NPC_NPCEnemy_BattleIn(  int meindex, int charaindex)
{
	int		gym;
	int		ret;
	char	buf[256];
	char	argstr[NPC_UTIL_GETARGSTR_BUFSIZE];

	memset( argstr, 0, sizeof( argstr));
	NPC_Util_GetArgStr( meindex, argstr, sizeof( argstr));
	gym = NPC_Util_GetNumFromStrWithDelim( argstr, "gym");

	if( gym > 0 ){
		ret = BATTLE_CreateVsEnemy( charaindex, 2, meindex);
	}else{
		ret = BATTLE_CreateVsEnemy( charaindex, 1, meindex);
	}
	if( ret == 0 ) {
		BattleArray[CHAR_getWorkInt( charaindex, CHAR_WORKBATTLEINDEX)].WinFunc	= NPC_NPCEnemy_Dying;
		if( NPC_Util_GetStrFromStrWithDelim( argstr, "startmsg", buf, sizeof( buf))	!= NULL ){
			CHAR_talkToCliAndParty( charaindex, meindex ,buf , CHAR_COLORYELLOW );
		}
		if( NPC_Util_GetStrFromStrWithDelim( argstr, "steal", buf, sizeof( buf))!= NULL ){
			if( atoi( buf) == 0 ) {
				NPC_NPCEnemy_StealItem( argstr, meindex, charaindex);
			}
		}
	}
	return ( ret == 0 ) ?TRUE:FALSE ;
}

void NPC_NPCEnemyWindowTalked( int meindex, int talkerindex, int seqno, int select, char *data)
{
	if( seqno == CHAR_WINDOWTYPE_NPCENEMY_START) {
		if( select == WINDOW_BUTTONTYPE_YES) {
			NPC_NPCEnemy_BattleIn( meindex, talkerindex);
		}
	}
}

static int NPC_NPCEnemy_StealItem( char *argstr, int meindex, int charaindex)
{
	char	itembuf[1024];
	if( NPC_Util_GetStrFromStrWithDelim( argstr, "item", itembuf, sizeof( itembuf))!= NULL )
	{
		char	data[128];
		int		delitemgroup[CHAR_MAXITEMHAVE];
		int		found = 0;
		int		i;
		for( i = 0; i < CHAR_MAXITEMHAVE; i ++ ) {
			int		j;
			int		itemid;
			int ret = getStringFromIndexWithDelim( itembuf,",", i + 1,data, sizeof(data));
			if( ret == FALSE) {
				break;
			}
			itemid = atoi( data);
			for( j = 0; j < CHAR_MAXITEMHAVE; j ++ ) {
				int itemindex = CHAR_getItemIndex( charaindex, j);
				if( ITEM_CHECKINDEX( itemindex)){
					if( ITEM_getInt( itemindex, ITEM_ID) == itemid ){
						CHAR_setItemIndex( charaindex, j, -1);
						ITEM_endExistItemsOne( itemindex);
						delitemgroup[found] = j;
						found ++;
						break;
					}
				}
			}
			if( !found ) break;
		}
		CHAR_sendItemData( charaindex, delitemgroup, found);
	}
	else {
		print( "NPCENEMY:Ææ¹Ö\n");
		return FALSE;
	}
	return TRUE;
}

#ifdef _NEW_WARPMAN
static BOOL NPC_NPCEnemy_CheckFree( int  meindex, int toindex, BOOL *Party)
{
  char npcarg[NPC_UTIL_GETARGSTR_BUFSIZE];
  char buf1[1024],buf2[256],buf3[256],buf4[256];
  char token[256];
  char temp1[16];
  int talkNo = 1;
  int point=1;

  int i=0,type=0;

  struct tagWarpPoint {
	int floor;
	int x;
	int y;
  }AllWarPoint[]={
	{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},
	{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},
	{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},
  };

  if(NPC_Util_GetArgStr( meindex, npcarg, sizeof(npcarg))==NULL)	{
	print("GetArgStrErr");
	return FALSE;
  }
  if( (strstr( npcarg,"NEWNPCENEMY") == NULL) && (strstr( npcarg,"OVER") == NULL) )	{
	return FALSE;
  }else	{
  }
	
  while( getStringFromIndexWithDelim( npcarg,"OVER",talkNo,buf1,sizeof( buf1)) != FALSE )	{
	
	talkNo++;
	sprintf(temp1,"NEWEVENT");
	if( strstr( buf1, temp1) != NULL)	{
	  if( NPC_Util_GetStrFromStrWithDelim( buf1, "FREE", buf2, sizeof( buf2) ) == FALSE )	{
	  	continue;
	  }
		if( NPC_ActionPassCheck( meindex, toindex, buf2) == FALSE )	{
			continue;
		}

	  if( NPC_Util_GetStrFromStrWithDelim( buf1, "endmsg", token, sizeof( token)) != NULL ) {
		CHAR_talkToCli( toindex, meindex ,token , CHAR_COLORYELLOW );
	  }
#ifdef _ALLDOMAN // (²»¿É¿ª) Syu ADD ÅÅÐÐ°ñNPC
	  if( NPC_Util_GetStrFromStrWithDelim( buf1, "herobattlefield", token, sizeof( token)) != NULL ) {
		  CHAR_setWorkInt ( toindex , CHAR_WORKHEROFLOOR , atoi ( token ) ) ;
		  if ( atoi ( token ) > CHAR_getInt ( toindex , CHAR_HEROFLOOR ) )
				CHAR_setInt ( toindex , CHAR_HEROFLOOR , atoi( token ) ) ; 

	  }
#endif

	  if( NPC_Util_GetStrFromStrWithDelim( buf1, "WARP", buf2, sizeof( buf2) ) == FALSE ){
	  	continue;
	  }else	{//RAND	arraysizeof	AllWarPoint
		  	while( getStringFromIndexWithDelim( buf2, ";", point, buf3,sizeof( buf3)) != FALSE )	{
				point++;
				getStringFromIndexWithDelim( buf3, ",", 1, buf4, sizeof( buf4));
				AllWarPoint[i].floor = atoi( buf4);
				getStringFromIndexWithDelim( buf3, ",", 2, buf4, sizeof( buf4));
				AllWarPoint[i].x = atoi( buf4);
				getStringFromIndexWithDelim( buf3, ",", 3, buf4, sizeof( buf4));
				AllWarPoint[i].y = atoi( buf4);
				i++;
				if( i >= arraysizeof( AllWarPoint)  )
					  break;
			}
			type = RAND( 0, i-1);
			if( AllWarPoint[type].floor <= 0 || type > arraysizeof( AllWarPoint)-1)	{
				type = 0;
			}
		  CHAR_setWorkInt( meindex, CHAR_WORK_WARP_FLOOR, AllWarPoint[type].floor );
		  CHAR_setWorkInt( meindex, CHAR_WORK_WARP_X, AllWarPoint[type].x );
		  CHAR_setWorkInt( meindex, CHAR_WORK_WARP_Y, AllWarPoint[type].y );
		  
	  }
	  if( NPC_Util_GetStrFromStrWithDelim( buf1, "CHECKPARTY", buf4, sizeof( buf4)) != NULL ) {
		  if( strstr( buf4, "FALSE") != NULL )	{
				*Party = FALSE;
		  }
	  }
	  if( *Party == TRUE )	{
		  Action_RunDoEventAction( meindex, toindex, buf1);
	  }
	  return TRUE;
	}
  }
  return FALSE;  
}


BOOL NPCEnemy_CheckFree( int meindex, int talker, char *buf)
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
				if(NPCEnemy_BSCheck(meindex,talker,buff3)==FALSE)	{
					loop=1;
					break;
				}
			}
			if(loop==0) {
				CHAR_setWorkInt( talker, CHAR_WORKWARPCHECK, TRUE );
				return TRUE;
			}
		}else{
			if( NPCEnemy_BSCheck( meindex, talker, buff2) == TRUE ){
				CHAR_setWorkInt( talker, CHAR_WORKWARPCHECK, TRUE );
				return TRUE;
			}
		}
	}
	CHAR_setWorkInt( talker, CHAR_WORKWARPCHECK, FALSE );
	return FALSE;
}

BOOL NPCEnemy_BSCheck(int meindex,int talker,char* buf)
{
	char buff2[128];
	int kosuu,temp=-1,flg=0;
	char buff1[128],buff3[128];
	if(strstr( buf, "-") != NULL)	{
		//buff3Îª×¥³èÎïID
		getStringFromIndexWithDelim( buf, "-", 2, buff3, sizeof(buff3));
		temp = atoi( buff3);
		getStringFromIndexWithDelim( buf, "-", 1, buff1, sizeof(buff1));
		strcpy( buf, buff1);
        }                         

	if(strstr( buf, "<") != NULL){
		getStringFromIndexWithDelim( buf, "<", 2, buff2, sizeof(buff2));
		kosuu = atoi( buff2);
		getStringFromIndexWithDelim( buf, "<", 1, buff2, sizeof(buff2));

		if(NPCEnemy_FreeIfCheck( meindex, talker, buff2, kosuu, 1, temp)==TRUE){
			return TRUE;
		}
	}else if(strstr( buf, ">") != NULL){
		getStringFromIndexWithDelim( buf, ">", 2, buff2, sizeof(buff2));
		kosuu = atoi(buff2);
		getStringFromIndexWithDelim( buf, ">" ,1, buff2, sizeof(buff2));

		if(NPCEnemy_FreeIfCheck( meindex, talker, buff2, kosuu, 2, temp)==TRUE){
			return TRUE;
		}

	}else if(strstr( buf, "!" ) != NULL){
		getStringFromIndexWithDelim( buf, "!=", 2, buff2, sizeof(buff2));
		kosuu = atoi( buff2);
		getStringFromIndexWithDelim( buf, "!=", 1, buff2, sizeof(buff2));
		if(NPCEnemy_FreeIfCheck( meindex, talker, buff2, kosuu, 0, temp) == TRUE){
			return FALSE;
		}else{
			return TRUE;
		}

	}else if(strstr( buf, "=") != NULL){
	
		getStringFromIndexWithDelim( buf, "=", 2, buff2, sizeof(buff2));
		kosuu = atoi( buff2);
		getStringFromIndexWithDelim( buf, "=", 1, buff2, sizeof(buff2));

		if( strstr( buf, "PET"))	{
			flg = 3;
		}
		if(strstr( buf, "*") != NULL){
			if( NPCEnemy_WarpManReduce( meindex, talker, buf)==TRUE){
				return TRUE;
			}
		}else if(NPCEnemy_FreeIfCheck( meindex, talker, buff2, kosuu, flg, temp)==TRUE){
			return TRUE;
		}
	}
	return FALSE;
}

BOOL NPCEnemy_FreeIfCheck(int meindex,int talker,char* buf,int kosuu,int flg, int temp)
{
	if(strcmp(buf,"LV")==0){
		if(NPCEnemy_LevelCheck(meindex,talker,kosuu,flg)==TRUE){
				return TRUE;
		}
	}
	
	if( strcmp(buf,"TRANS") == 0 )	{
		if( NPCEnemy_CheckTrans( meindex, talker, kosuu, flg) == TRUE )
			return TRUE;
	}
	if( strcmp( buf, "PET") == 0 )	{
		if( temp > 0 )	{
			if( NPCEnemy_CheckMyPet( meindex, talker, kosuu, flg, temp) == TRUE )
				return TRUE;
		}else
			return FALSE;
	}	

	if(strcmp(buf,"ITEM")==0){
		if(NPCEnemy_ItemCheck(meindex,talker,kosuu,flg)==TRUE){
				return TRUE;
		}
	}
	if(strcmp(buf,"ENDEV")==0){
		if(NPC_EventCheckFlg( talker, kosuu)==TRUE){
				return TRUE;
		}
	}
	if(strcmp(buf,"NOWEV")==0){
		if(NPC_NowEventCheckFlg( talker, kosuu)==TRUE){
				return TRUE;
		}
	}
	return FALSE;

}

BOOL NPCEnemy_WarpManReduce(int meindex,int talker,char *buf)
{
	char buf2[512];
	char buf3[256];
	int id=0;
	int i;
	int itemindex;
	int itemno;
	int kosuu;
	int cnt=0;
	
	getStringFromIndexWithDelim(buf,"=",2,buf2,sizeof(buf2));
	getStringFromIndexWithDelim(buf2,"*",1,buf3,sizeof(buf3));
	itemno = atoi(buf3);
	getStringFromIndexWithDelim(buf2,"*",2,buf3,sizeof(buf3));
	kosuu = atoi(buf3);
	
	for( i=0 ; i < CHAR_MAXITEMHAVE;i++ ){
		itemindex=CHAR_getItemIndex( talker , i );
		if( ITEM_CHECKINDEX(itemindex) ){
			id=ITEM_getInt(itemindex ,ITEM_ID );
			if(itemno==id){
				cnt++;
				if(cnt==kosuu){
					return TRUE;
				}
			}
		}
	}

	return FALSE;

}

BOOL NPCEnemy_BigSmallLastCheck(int point1,int mypoint,int flg)
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

BOOL NPCEnemy_LevelCheck(int meindex,int talker,int level,int flg)
{
	int mylevel;
	mylevel=CHAR_getInt(talker,CHAR_LV);
	if(NPCEnemy_BigSmallLastCheck(level,mylevel,flg)==TRUE)	{
		return TRUE;
	}
	return FALSE;
}

BOOL NPCEnemy_CheckTrans(int meindex,int talker,int trans,int flg)
{
	int myTrans;
	myTrans=CHAR_getInt(talker, CHAR_TRANSMIGRATION);
	if( NPCEnemy_BigSmallLastCheck( trans, myTrans, flg ) == TRUE )	{
             return TRUE;
        }                   
	return FALSE;
}

BOOL NPCEnemy_CheckMyPet( int meindex, int talker, int petLv, int flg, int petid)
{
	int petsel,petindex=-1;	
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
	}else	{	//ÕÒµ½Ìõ¼þ³è
		switch( flg)	{
		case 0:
			if( CHAR_getInt( petindex, CHAR_LV) != petLv )
				return TRUE;
		break;
		case 1:
			if( CHAR_getInt( petindex, CHAR_LV) < petLv )
				return TRUE;
		break;
		case 2:
			if( CHAR_getInt( petindex, CHAR_LV) > petLv )
				return TRUE;
		break;
		case 3:
			if( CHAR_getInt( petindex, CHAR_LV) == petLv )
				return TRUE;
		break;
		}
		return FALSE;
	}
	return FALSE;                                             
}

BOOL NPCEnemy_ItemCheck(int meindex,int talker,int itemNo,int flg)
{
	int i;
	int itemindex=-1;
	int id;

	for( i=0;i<CHAR_MAXITEMHAVE;i++ ){

		itemindex = CHAR_getItemIndex( talker , i );
		if( ITEM_CHECKINDEX( itemindex) )	{
			id=ITEM_getInt(itemindex ,ITEM_ID );
			
			if( NPCEnemy_BigSmallLastCheck(itemNo,id,flg) == TRUE )
				return TRUE;
		}
	}
	return FALSE;

}

BOOL NPC_WarpAllMsg(int meindex,int toindex )
{
	int fl,x,y;
	int subindex=-1;
	int i;
	fl = CHAR_getWorkInt( meindex, CHAR_WORK_WARP_FLOOR );
	x = CHAR_getWorkInt( meindex, CHAR_WORK_WARP_X );
	y = CHAR_getWorkInt( meindex, CHAR_WORK_WARP_Y );

	if( CHAR_getWorkInt( toindex, CHAR_WORKPARTYMODE ) != 1 )
		return FALSE;

	for( i = 0; i < CHAR_PARTYMAX; i ++ ){
		subindex = CHAR_getWorkInt( toindex, CHAR_WORKPARTYINDEX1+i );
		if( CHAR_CHECKINDEX( subindex ) == FALSE )continue;
		if( fl==0 && x==0 && y==0 )	{
		}else	{
			CHAR_warpToSpecificPoint( subindex, fl, x, y );
		}
	}

	return TRUE;
}
#endif



void NPC_NPCEnemyWatch( int meobjindex, int objindex, CHAR_ACTION act,
                    int x,int y,int dir, int* opt,int optlen )
{
	int born, dead;
	int meindex, index;
	char npcarg[NPC_UTIL_GETARGSTR_BUFSIZE];

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
	if( !strstr( npcarg,"NEWNPCENEMY") )
		return;
	if( NPC_getTimeData( meindex, npcarg, &born, &dead, 200) == FALSE )
		return;
	
	NPC_TimeDefineDo( meindex, born, dead, NPC_TIME_MODE);

	CHAR_sendCToArroundCharacter( CHAR_getWorkInt( meindex, CHAR_WORKOBJINDEX ) );

}

#ifdef _NPC_REPLACEMENT
void Check_EnemyWarpMe( int meindex, char *args )
{

	char buf1[256];
	int objindex,index;
	int nfl, nx, ny;
	OBJECT obj;

	memset( buf1, 0, sizeof( buf1));
	if( NPC_Util_GetStrFromStrWithDelim( args, "REPLACEMENT", buf1, sizeof( buf1)) == NULL ) {
		return;
	}
	GetRand_WarpPoint( buf1, &nfl, &nx, &ny );

	if( MAP_IsValidCoordinate( nfl, nx, ny ) == FALSE ){
		char filename[256];
		NPC_Util_CheckAssignArgFile( meindex, filename);
		print( "NPCENEMY REPLACEMENT: %s .Invalid POINT( %d, %d, %d)\n", filename, nfl, nx, ny );
		return;
	}
	{
		int ofl, ox, oy;
		ofl = CHAR_getInt( meindex, CHAR_FLOOR );
		ox = CHAR_getInt( meindex, CHAR_X );
		oy = CHAR_getInt( meindex, CHAR_Y );

		for ( obj=MAP_getTopObj( nfl, nx, ny); obj; obj=NEXT_OBJECT( obj) ) {
			objindex = GET_OBJINDEX( obj);
			if (OBJECT_getType( objindex) != OBJTYPE_CHARA) continue;
			index = OBJECT_getIndex( objindex);
			if( CHAR_getInt( index, CHAR_WHICHTYPE) != CHAR_TYPEPLAYER &&
				CHAR_getWorkInt( index, CHAR_WORKEVENTTYPE) != CHAR_TYPEPET )	{
				CHAR_warpToSpecificPoint( index, ofl, ox, oy);
				CHAR_sendCToArroundCharacter( objindex);
				break;
			}
		}
	}
	objindex = CHAR_getWorkInt( meindex, CHAR_WORKOBJINDEX);
	CHAR_warpToSpecificPoint( meindex, nfl, nx, ny);
	CHAR_sendCToArroundCharacter( objindex);
}
#endif
