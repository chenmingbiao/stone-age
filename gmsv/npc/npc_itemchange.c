#include "version.h"
#include <string.h>
#include "char.h"
#include "object.h"
#include "char_base.h"
#include "npcutil.h"
#include "lssproto_serv.h"
#include "saacproto_cli.h"
#include "readmap.h"
#include "battle.h"
#include "log.h"
#include "enemy.h"
#include "handletime.h"
#include "npc_eventaction.h"
#include "npc_itemchange.h"

#ifdef _ITEM_NPCCHANGE
enum {
	ITEMCHANGE_START=1,
	ITEMCHANGE_SELECT,
	ITEMCHANGE_MESSAGE,
	ITEMCHANGE_END,
	ITEMCHANGE_ERROR,

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

static void NPC_ItemchangeMan_selectWindow( int meindex, int toindex, int num,int select);
BOOL ItemchangeMan_GetNeedStr( int meindex, int toindex, char *npcarg, char *token, int num );
BOOL ItemchangeMan_GetMenuStr( int meindex, int toindex, char *npcarg, char *token);

BOOL NPC_ItemchangeManInit( int meindex )
{
	char npcarg[NPC_UTIL_GETARGSTR_BUFSIZE];
		
	if(NPC_Util_GetArgStr( meindex, npcarg, sizeof(npcarg))==NULL){
		print("TRANSER_MAN: GetArgStrErr!!");
		return FALSE;
	}

   	CHAR_setInt( meindex, CHAR_WHICHTYPE, CHAR_ITEMCHANGENPC);

	//CHAR_setInt( meindex, CHAR_LOOPINTERVAL, NEWNPCMAN_STANDBY);
	CHAR_setWorkInt( meindex, NPC_WORK_CURRENTTIME, NowTime.tv_sec);

	return TRUE;
}
//CHAR_setWorkInt(talkerindex,CHAR_WORKSHOPRELEVANT,1);
void NPC_ItemchangeManTalked( int meindex, int talkerindex, char *msg, int color )
{
	if( CHAR_getInt( talkerindex , CHAR_WHICHTYPE ) != CHAR_TYPEPLAYER )
    		return;
	if( NPC_Util_isFaceToFace( meindex ,talkerindex , 2) == FALSE) {
		if( NPC_Util_isFaceToChara( talkerindex, meindex, 1) == FALSE)
			return;
	}   
	CHAR_setWorkInt( talkerindex, CHAR_WORKSHOPRELEVANT, 0);
	NPC_ItemchangeMan_selectWindow( meindex, talkerindex, ITEMCHANGE_START, -1);

}

void NPC_ItemchangeManWindowTalked( int meindex, int talkerindex, int seqno, int select, char *data)
{
	int flg=-1;
	if( select == WINDOW_BUTTONTYPE_NO || select == WINDOW_BUTTONTYPE_CANCEL)
		return;
	switch( seqno)	{
	case NPC_ITEMCHANGE_START:
		break;
	case NPC_ITEMCHANGE_SELECT:
		NPC_ItemchangeMan_selectWindow( meindex, talkerindex, ITEMCHANGE_SELECT, flg);
		break;
	case NPC_ITEMCHANGE_MESSAGE:
		flg = atoi( data);
		CHAR_setWorkInt( talkerindex, CHAR_WORKSHOPRELEVANT, flg);
		NPC_ItemchangeMan_selectWindow( meindex, talkerindex, ITEMCHANGE_MESSAGE, flg);
		break;
	case NPC_ITEMCHANGE_END:
		flg = CHAR_getWorkInt( talkerindex, CHAR_WORKSHOPRELEVANT);
		NPC_ItemchangeMan_selectWindow( meindex, talkerindex, ITEMCHANGE_END , flg);
		break;
	}

}

static void NPC_ItemchangeMan_selectWindow( int meindex, int toindex, int num,int select)
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
	case ITEMCHANGE_START:
		if( NPC_Util_GetStrFromStrWithDelim( npcarg, "START_MSG", token, sizeof( token) ) == NULL) {
			print("TranserMan Get START_MSG ERROR !");
			return;
		}
		CHAR_setWorkInt( toindex, CHAR_WORKSHOPRELEVANT, ITEMCHANGE_START);
		buttontype = WINDOW_BUTTONTYPE_YESNO;
		windowno = NPC_ITEMCHANGE_SELECT;
		break;
	case ITEMCHANGE_SELECT:
		if( ItemchangeMan_GetMenuStr( meindex, toindex, npcarg, token) == FALSE )
			return;
		CHAR_setWorkInt( toindex, CHAR_WORKSHOPRELEVANT, ITEMCHANGE_SELECT);
		windowtype = WINDOW_MESSAGETYPE_SELECT;
		buttontype = WINDOW_BUTTONTYPE_CANCEL;
		windowno = NPC_ITEMCHANGE_MESSAGE;
		break;
	case ITEMCHANGE_MESSAGE:
		if( ItemchangeMan_GetNeedStr( meindex, toindex, npcarg, token, select ) == FALSE )
			return;
		buttontype = WINDOW_BUTTONTYPE_YESNO;
		windowno = NPC_ITEMCHANGE_END;
		break;
	case ITEMCHANGE_END:
		if( ActionNpc_CheckMenuFree( meindex, toindex, npcarg, select ) == FALSE ){
			memset( token, 0, sizeof( token));
			if( NPC_Util_GetStrFromStrWithDelim( npcarg, "FAIL_MSG", token, sizeof( token) ) != NULL) {
				CHAR_talkToCli( toindex, meindex, token, CHAR_COLORYELLOW);
			}
		}
		return;
		break;
	case ITEMCHANGE_ERROR:
		break;
	}

	lssproto_WN_send( fd, windowtype, buttontype, windowno,
		CHAR_getWorkInt( meindex, CHAR_WORKOBJINDEX), token);
}

void NPC_ItemchangeManLoop( int meindex)
{
	
}

BOOL ItemchangeMan_GetMenuStr( int meindex, int toindex, char *npcarg, char *token)
{
	char buf1[1024], buf2[256], buf3[256];
	char NullName[256] = {"未知的物品"};
	BOOL FINDS=FALSE;
	int talkNo=1;
	int addID;
	if( npcarg == NULL ) return FALSE;
	
	if( NPC_Util_GetStrFromStrWithDelim( npcarg, "MenuHead", buf1, sizeof( buf1) ) == NULL){
		print("ItemchangeMan Get MenuHead ERROR!");
		return FALSE;
	}

	sprintf( token, "%s\n", buf1);

	while( getStringFromIndexWithDelim( npcarg,"}", talkNo, buf1, sizeof( buf1)) != FALSE )	{
		char *ItemName;
		talkNo++;

		if( NPC_Util_GetStrFromStrWithDelim( buf1, "CHANGEITEM", buf2, sizeof( buf2)) != NULL  ){
			addID = atoi( buf2);
			if( (ItemName = ITEM_getNameFromNumber( addID)) == "\0" ){
				sprintf( buf3, "   %s\n", NullName);
			}else{
				sprintf( buf3, "   %s\n", ItemName);
			}
		//	continue;
		}else	{
			memset( buf3, 0, sizeof( buf3));
			if( NPC_Util_GetStrFromStrWithDelim( buf1, "CHANGEMSG", buf3, sizeof( buf3)) == NULL  )
				continue;
			strcat( buf3, "\n");
		}
		FINDS = TRUE;
		strcat( token, buf3);
	}

	return FINDS;
}

BOOL ItemchangeMan_GetNeedStr( int meindex, int toindex, char *npcarg, char *token, int num )
{
	char buf1[1024], buf2[256], buf3[256];
	char NullName[256] = {"未知的物品"};
	int talkNo=1, addID, i=0;
	BOOL FINDS = FALSE;
	if( npcarg == NULL ) return FALSE;
	if( NPC_Util_GetStrFromStrWithDelim( npcarg, "NeedHead", buf1, sizeof( buf1) ) == NULL){
		print("ItemchangeMan Get NeedHead ERROR!\n%s", npcarg);
		return FALSE;
	}
	sprintf( token, "%s\n", buf1);
	if( getStringFromIndexWithDelim( npcarg,"}", num, buf1, sizeof( buf1)) == FALSE )	{
		printf("Get NeedMess error !!\n");
		return FALSE;
	}

	if( NPC_Util_GetStrFromStrWithDelim( buf1, "NeedItem", buf2, sizeof( buf2) ) != NULL){
		while( getStringFromIndexWithDelim( buf2, ",", talkNo, buf1, sizeof( buf1)) != FALSE )	{
			char *ItemName=NULL;
			talkNo++;

			addID = atoi( buf1);
			FINDS = TRUE;
			if( (ItemName = ITEM_getNameFromNumber( addID)) == "\0" ){
				sprintf( buf3, "%s  ", NullName);
			}else{
				sprintf( buf3, "%s  ", ItemName);
			}
			strcat( token, buf3);
			if( i%3 == 0 )
				strcat( token, "\n");
			i++;
		}
	}else	{
		if( NPC_Util_GetStrFromStrWithDelim( buf1, "NeedMsg", buf2, sizeof( buf2) ) == NULL)
			return FALSE;
		sprintf( token, "%s  ", buf2);
		FINDS = TRUE;
	}

	if( NPC_Util_GetStrFromStrWithDelim( buf2, "DelGold", buf1, sizeof( buf1)) != NULL )	{
		char goldmess[256];
		int gold = atoi( buf1);
		if( gold > 0 )	{
			sprintf( goldmess, "石币:%d \n", gold);
			strcat( token, goldmess);
		}
	}

	return FINDS;
}
//void ItemchangeMan_CheckMenuFree( int meindex, int toindex, char *npcarg, int nums)
#endif


