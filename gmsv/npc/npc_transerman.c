#include "version.h"
#include <string.h>
#include "char.h"
#include "object.h"
#include "char_base.h"
#include "npcutil.h"
#include "npc_transerman.h"
#include "lssproto_serv.h"
#include "saacproto_cli.h"
#include "readmap.h"
#include "battle.h"
#include "log.h"
#include "enemy.h"
#include "handletime.h"
#include "npc_eventaction.h"

//特地从天堂高薪挖角来的"传送师"
#ifdef _TRANSER_MAN
enum {
	WINDOW_START=1,
	WINDOW_SELECT,
	WINDOW_WARP,
	WINDOW_END,
};
enum {
	NPC_WORK_CURRENTTIME = CHAR_NPCWORKINT1,
/*
	NPC_WORK_ROUTETOY = CHAR_NPCWORKINT2,
	NPC_WORK_ROUTEPOINT = CHAR_NPCWORKINT3,
	NPC_WORK_ROUNDTRIP = CHAR_NPCWORKINT4,
	NPC_WORK_MODE = CHAR_NPCWORKINT5,
	NPC_WORK_CURRENTROUTE = CHAR_NPCWORKINT6, 
	NPC_WORK_ROUTEMAX = CHAR_NPCWORKINT7,
	NPC_WORK_WAITTIME = CHAR_NPCWORKINT8,
	NPC_WORK_CURRENTTIME = CHAR_NPCWORKINT9,
	NPC_WORK_SEFLG = CHAR_NPCWORKINT10,
*/
};

#define STANDBYTIME 50

static void NPC_TranserMan_selectWindow( int meindex, int toindex, int num,int select);
BOOL TranserMan_GetMenuStr( int meindex, int toindex, char *npcarg, char *token);

BOOL NPC_TranserManInit( int meindex )
{
	char npcarg[NPC_UTIL_GETARGSTR_BUFSIZE];
		
	if(NPC_Util_GetArgStr( meindex, npcarg, sizeof(npcarg))==NULL){
		print("TRANSER_MAN: GetArgStrErr!!");
		return FALSE;
	}

   	CHAR_setInt( meindex, CHAR_WHICHTYPE, CHAR_TRANSERMANS);

	//CHAR_setInt( meindex, CHAR_LOOPINTERVAL, NEWNPCMAN_STANDBY);
	CHAR_setWorkInt( meindex, NPC_WORK_CURRENTTIME, NowTime.tv_sec);
	return TRUE;
}
//CHAR_setWorkInt(talkerindex,CHAR_WORKSHOPRELEVANT,1);
void NPC_TranserManTalked( int meindex, int talkerindex, char *msg, int color )
{
	if( CHAR_getInt( talkerindex , CHAR_WHICHTYPE ) != CHAR_TYPEPLAYER )
    		return;
	if(NPC_Util_isFaceToFace( meindex ,talkerindex , 2) == FALSE) {
		if(NPC_Util_isFaceToChara( talkerindex, meindex, 1) == FALSE)
			return;
	}
#ifdef _ITEM_CHECKWARES
	if( CHAR_CheckInItemForWares( talkerindex, 0) == FALSE ){
		CHAR_talkToCli( talkerindex, meindex, "根据传送师工会规定，不能服务携带货物的人！", CHAR_COLORYELLOW);
		return;
	}
#endif
// Syu ADD 天才传送师判断组队状态
	if( CHAR_getWorkInt ( talkerindex , CHAR_WORKPARTYMODE) != CHAR_PARTY_NONE)
	{
		CHAR_talkToCli( talkerindex, meindex, "根据传送师工会规定，不能帮组队中的人传送！",CHAR_COLORYELLOW);
		return;
	}

	CHAR_setWorkInt( talkerindex, CHAR_WORKSHOPRELEVANT, 0);
	NPC_TranserMan_selectWindow( meindex, talkerindex, WINDOW_START, 0);
}

void NPC_TranserManWindowTalked( int meindex, int talkerindex, int seqno,
								int select, char *data)
{
	if( select == WINDOW_BUTTONTYPE_CANCEL || select == WINDOW_BUTTONTYPE_NO)
		return;
	switch( seqno)	{
	case NPC_TRANSERMAN_START:
		break;
	case NPC_TRANSERMAN_SELECT:
		NPC_TranserMan_selectWindow( meindex, talkerindex, WINDOW_SELECT, atoi( data));
		break;
	case NPC_TRANSERMAN_WARP:
		if( select == WINDOW_BUTTONTYPE_CANCEL )
			return;
		NPC_TranserMan_selectWindow( meindex, talkerindex, WINDOW_WARP, atoi( data));
		break;
	case NPC_TRANSERMAN_END:
		break;
	}

}

static void NPC_TranserMan_selectWindow( int meindex, int toindex, int num,int select)
{
	char npcarg[NPC_UTIL_GETARGSTR_BUFSIZE];
	char token[256];

	int buttontype = 0, windowtype = 0, windowno = 0;
	int fd = getfdFromCharaIndex( toindex);

	windowtype = WINDOW_MESSAGETYPE_MESSAGE;

	if(NPC_Util_isFaceToFace( meindex ,toindex , 2) == FALSE) {
		if(NPC_Util_isFaceToChara( toindex, meindex, 1) == FALSE)
			return;
	}

	memset( npcarg, 0, sizeof( npcarg));
	if(NPC_Util_GetArgStr( meindex, npcarg, sizeof(npcarg))==NULL){
		print("TRANSER_MAN: GetArgStrErr!!");
		return;
	}
	memset( token, 0, sizeof( token));

	switch( num)	{
	case WINDOW_START:
		if( NPC_Util_GetStrFromStrWithDelim( npcarg, "START_MSG", token, sizeof( token) ) == NULL) {
			print("TranserMan Get START_MSG ERROR !");
			return;
		}
		CHAR_setWorkInt( toindex, CHAR_WORKSHOPRELEVANT, WINDOW_START);
		buttontype = WINDOW_BUTTONTYPE_YESNO;
		windowno = NPC_TRANSERMAN_SELECT;
		break;
	case WINDOW_SELECT:
		if( TranserMan_GetMenuStr( meindex, toindex, npcarg, token) == FALSE )	{
			print( "TranserMan GetMenu ERROR !!");
			return;
		}
		windowtype = WINDOW_MESSAGETYPE_SELECT;
		buttontype = WINDOW_BUTTONTYPE_CANCEL;
		windowno = NPC_TRANSERMAN_WARP;
		break;
	case WINDOW_WARP:
		if( ActionNpc_CheckMenuFree( meindex, toindex, npcarg, select) == FALSE ){
			memset( token, 0, sizeof( token));
			if( NPC_Util_GetStrFromStrWithDelim( npcarg, "FAIL_MSG", token, sizeof( token) ) != NULL) {
				CHAR_talkToCli( toindex, meindex, token, CHAR_COLORYELLOW);
			}
		}
		return;

	case WINDOW_END:
		break;
	}

	lssproto_WN_send( fd, windowtype, buttontype, windowno,
		CHAR_getWorkInt( meindex, CHAR_WORKOBJINDEX), token);
}


void NPC_TranserManLoop( int meindex)
{

}

BOOL TranserMan_GetMenuStr( int meindex, int toindex, char *npcarg, char *token)
{
	char buf1[1024], buf2[256], buf3[256];
	BOOL FINDS=FALSE;
	int talkNo=1;
	if( npcarg == NULL ) return FALSE;
	
	if( NPC_Util_GetStrFromStrWithDelim( npcarg, "MenuHead", buf1, sizeof( buf1) ) == NULL){
		print("TranserMan Get MenuHead ERROR!");
		return FALSE;
	}
	sprintf( token, "%s\n", buf1);

	while( getStringFromIndexWithDelim( npcarg,"}",talkNo, buf1, sizeof( buf1)) != FALSE )	{
		talkNo++;
		memset( buf2, 0, sizeof( buf2));
		if( NPC_Util_GetStrFromStrWithDelim( buf1, "MenuStr", buf2, sizeof( buf2)) == NULL  )
			continue;
		FINDS = TRUE;

		sprintf( buf3, "          %s\n", buf2);
		strcat( token, buf3);
	}

	return FINDS;
}

#endif


