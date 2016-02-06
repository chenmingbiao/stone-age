#include "version.h"

#include "char.h"
#include "lssproto_serv.h"
#include "npcutil.h"
#include "npc_poolitemshop.h"
#include "log.h"
#include "handletime.h"

#define		NPC_POOLITEMSHOP_DEFAULT_COST		200

enum {
	NPC_WORK_COST = CHAR_NPCWORKINT1,
	NPC_WORK_CURRENTTIME = CHAR_NPCWORKINT9,
};

enum {
	NPC_POOLITEMSHOP_MSG_MAIN=0,
	NPC_POOLITEMSHOP_MSG_POOL,
	NPC_POOLITEMSHOP_MSG_DRAW,
	NPC_POOLITEMSHOP_MSG_REALY,
	NPC_POOLITEMSHOP_MSG_STONE,
	NPC_POOLITEMSHOP_MSG_POOLFULL,
	NPC_POOLITEMSHOP_MSG_ITEMFULL,

	CHAR_WINDOWTYPE_POOLITEMSHOP_START = 290,
	CHAR_WINDOWTYPE_POOLITEMSHOP_POOL_MSG,
	CHAR_WINDOWTYPE_POOLITEMSHOP_DRAW_MSG,
	CHAR_WINDOWTYPE_POOLITEMSHOP_FULL_MSG,
	CHAR_WINDOWTYPE_POOLITEMSHOP_HAVEITEMFULL_MSG,
	CHAR_WINDOWTYPE_POOLITEMSHOP_END,

#ifdef _NPC_DEPOTITEM
	CHAR_WINDOWTYPE_DEPOTITEMSHOP_MENU = 310,
	CHAR_WINDOWTYPE_DEPOTITEMSHOP_HANDLE,
	CHAR_WINDOWTYPE_DEPOTITEMSHOP_ADD,
	CHAR_WINDOWTYPE_DEPOTITEMSHOP_GET,
#endif
};

typedef struct {
	char	option[32];
	char	defaultmsg[128];
}NPC_POOLITEMSHOP_MSG;

NPC_POOLITEMSHOP_MSG		poolshopmsg[] = {
	{ "main_msg",		"欢迎"},
	{ "pool_main",		"要寄放什麽呢"},
	{ "draw_main",		"要领养什麽呢"},
	{ "realy_msg",		"真的要那个吗"},
	{ "stone_msg",		"钱不够喔!"},
	{ "poolfull_msg",	"超过的无法处理喔"},
	{ "itemfull_msg",	"项目有很多"}
};

static BOOL NPC_PoolItemShop_DrawItem( int meindex, int talkerindex, int num);
static BOOL NPC_PoolItemShop_PoolItem( int meindex, int talkerindex, int num);
static void NPC_PoolItemShop_MakeItemString_Draw( int meindex, int talkerindex, 
								char *retstring,int retstringlen);
static void NPC_PoolItemShop_MakeItemString_Pool( int meindex, int talkerindex, 
								char *retstring,int retstringlen);
static char *NPC_PoolItemShop_getMsg_noarg( int tablenum, 
								char *argstr, char *retstring, int retstringlen);
static void NPC_PoolItemShop_printWindow_Draw( int meindex, int talkerindex);
static void NPC_PoolItemShop_printWindow_Pool( int meindex, int talkerindex);
static void NPC_PoolItemShop_printWindow_Full( int meindex, int talkerindex);
void NPC_PoolItemShop_printWindow_Start( int meindex, int talkerindex);
static void NPC_PoolItemShop_printWindow_HaveItemFull( int meindex, int talkerindex);

#ifdef _NPC_DEPOTITEM
void NPC_PoolItemShop_DepotItem_Menu( int meindex, int talkerindex);
void NPC_DepotItem_Item_printWindow( int meindex, int talkerindex);
void NPC_DepotItem_Depot_printWindow( int meindex, int talkerindex);
BOOL NPC_DepotItem_InsertItem( int meindex, int talkerindex, int num);
BOOL NPC_DepotItem_gettItem( int meindex, int talkerindex, int num);
#endif

#define NPCPOOLITEMLOOP 1000*20
int othertime=0;
BOOL NPC_PoolItemShopInit( int meindex)
{
	char	argstr[NPC_UTIL_GETARGSTR_BUFSIZE - 1024 * 20];
	int cost;
	NPC_Util_GetArgStr( meindex, argstr, sizeof( argstr));
	cost = NPC_Util_GetNumFromStrWithDelim( argstr, "cost");
	if( cost == -1 ) cost = NPC_POOLITEMSHOP_DEFAULT_COST;
	CHAR_setWorkInt( meindex, NPC_WORK_COST, cost);

	return TRUE;
}

void NPC_PoolItemShopLoop( int meindex)
{
}

void NPC_PoolItemShopTalked( int meindex , int talkerindex , 
							char *szMes ,int color )
{
	if( CHAR_getInt( talkerindex , CHAR_WHICHTYPE ) != CHAR_TYPEPLAYER ) {
		return;
	}
	CHAR_setWorkInt( talkerindex, CHAR_WORKSHOPRELEVANT, 0);
	if( !NPC_Util_isFaceToFace( meindex, talkerindex, 2)) {
		if( NPC_Util_CharDistance( talkerindex, meindex ) > 1) return;
	}
	NPC_PoolItemShop_printWindow_Start( meindex, talkerindex);
}

void NPC_PoolItemShopWindowTalked( int meindex, int talkerindex, 
								int seqno, int select, char *data)
{
	if( NPC_Util_CharDistance( talkerindex, meindex ) > 2) return;

	//print("\n NPC_PoolItemShopWindowTalked: seq:%d sel:%d data:%s", seqno, select, data);

	switch( seqno ) {
	  case CHAR_WINDOWTYPE_POOLITEMSHOP_START:
		switch( atoi( data)) {
		  case 1:
			if( CHAR_getCharPoolItemIndexElement( talkerindex) != -1 ) {
				NPC_PoolItemShop_printWindow_Pool( meindex, talkerindex);
			}else {
				NPC_PoolItemShop_printWindow_Full( meindex, talkerindex);
			}
			break;
		  case 2:
			if( CHAR_findEmptyItemBox( talkerindex) != -1 ) {
				NPC_PoolItemShop_printWindow_Draw( meindex, talkerindex);
			}else {
				NPC_PoolItemShop_printWindow_HaveItemFull( meindex, talkerindex);
			}
			break;
#ifdef _NPC_DEPOTITEM
		  case 3:
			if( !CHAR_CheckDepotItem( talkerindex) ){
				CHAR_GetDepotItem( meindex, talkerindex);
				CHAR_talkToCli( talkerindex, -1, "取得道具，请稍後！", CHAR_COLORYELLOW);
			}else{
				NPC_PoolItemShop_DepotItem_Menu( meindex, talkerindex);
			}
			break;
#endif

		}
		break;
	  case CHAR_WINDOWTYPE_POOLITEMSHOP_POOL_MSG:
	  	if( atoi( data) == 0){
			NPC_PoolItemShop_printWindow_Start( meindex, talkerindex);
	  	}else {
			NPC_PoolItemShop_PoolItem( meindex, talkerindex, atoi(data)-1);
		}
		break;
	  case CHAR_WINDOWTYPE_POOLITEMSHOP_DRAW_MSG:
	  	if( atoi( data) == 0){
			NPC_PoolItemShop_printWindow_Start( meindex, talkerindex);
	  	}else {
		  	NPC_PoolItemShop_DrawItem( meindex, talkerindex, atoi(data)-1);
		}
	  	break;
	  case CHAR_WINDOWTYPE_POOLITEMSHOP_FULL_MSG:
		NPC_PoolItemShop_printWindow_Start( meindex, talkerindex);
	  	break;
	  case CHAR_WINDOWTYPE_POOLITEMSHOP_HAVEITEMFULL_MSG:
		NPC_PoolItemShop_printWindow_Start( meindex, talkerindex);
	  	break;

#ifdef _NPC_DEPOTITEM
	  case CHAR_WINDOWTYPE_DEPOTITEMSHOP_MENU:
			if( !CHAR_CheckDepotItem( talkerindex) ){
				CHAR_GetDepotItem( meindex, talkerindex);
				return;
			}else{
				NPC_PoolItemShop_DepotItem_Menu( meindex, talkerindex);
			}
		  break;
	  case CHAR_WINDOWTYPE_DEPOTITEMSHOP_HANDLE:
		  if( !CHAR_CheckDepotItem( talkerindex) ) return ;
		  switch( atoi( data)) {
		  case 1://放入
				if( CHAR_findEmptyDepotItem( talkerindex) == -1 ){
					CHAR_talkToCli( talkerindex, -1, "仓库已满！", CHAR_COLORYELLOW);
					return;
				}
				NPC_DepotItem_Item_printWindow( meindex, talkerindex);
			  break;
		  case 2:
				if( CHAR_findEmptyItemBox( talkerindex) == -1 ) {
					CHAR_talkToCli( talkerindex, -1, "身上道具栏位已满！", CHAR_COLORYELLOW);
					return;
				}
				NPC_DepotItem_Depot_printWindow( meindex, talkerindex);
			  break;
		  }
		  break;
	  case CHAR_WINDOWTYPE_DEPOTITEMSHOP_ADD://放入
			if( !CHAR_CheckDepotItem( talkerindex) ) return ;
			if( atoi( data) == 0){
				NPC_PoolItemShop_DepotItem_Menu( meindex, talkerindex);
			}else {
				if( NPC_DepotItem_InsertItem( meindex, talkerindex, atoi( data)-1) == FALSE ){
					NPC_PoolItemShop_DepotItem_Menu( meindex, talkerindex);
					CHAR_talkToCli( talkerindex, -1, "存放道具失败，请稍後再试！", CHAR_COLORYELLOW);
				}
			}
		  break;
	  case CHAR_WINDOWTYPE_DEPOTITEMSHOP_GET:
			if( !CHAR_CheckDepotItem( talkerindex) ) return ;
			if( atoi( data) == 0){
				NPC_PoolItemShop_DepotItem_Menu( meindex, talkerindex);
			}else {
				if( NPC_DepotItem_gettItem( meindex, talkerindex, atoi( data)-1) == FALSE ){
					NPC_PoolItemShop_DepotItem_Menu( meindex, talkerindex);
					CHAR_talkToCli( talkerindex, -1, "取出道具失败，请稍後再试！", CHAR_COLORYELLOW);
				}
			}
		  break;
#endif
  
	  default:
		break;
	}
}

void NPC_PoolItemShop_printWindow_Start( int meindex, int talkerindex)
{
	int fd;
	
	fd = getfdFromCharaIndex( talkerindex);
	if( fd != -1 ) {
		char	message[1024];
		char	buf[2048];

#ifdef _NPC_DEPOTITEM
		strcpy( message, 
			"3\n\n"
			"            欢迎光临\n\n"
			"          ＜寄放道具＞\n"
			"          ＜取回道具＞\n"
			"          ＜使用仓库＞\n\n"
			"          ＜  离开  ＞"
		);
#else
		strcpy( message, 
			"3\n\n"
			"            欢迎光临\n\n"
			"          ＜寄放道具＞\n"
			"          ＜取回道具＞\n\n\n"
			"          ＜  离开  ＞"
		);
#endif

		lssproto_WN_send( fd, WINDOW_MESSAGETYPE_SELECT, 
						WINDOW_BUTTONTYPE_NONE,
						CHAR_WINDOWTYPE_POOLITEMSHOP_START,
						CHAR_getWorkInt( meindex, CHAR_WORKOBJINDEX),
						makeEscapeString( message, buf, sizeof(buf)));
	}
}

#ifdef _NPC_DEPOTITEM
void NPC_PoolItemShop_DepotItem_Menu( int meindex, int talkerindex)
{
	int fd;
	
	fd = getfdFromCharaIndex( talkerindex);
	if( fd != -1 ) {
		char	message[1024];
		char	buf[2048];

		strcpy( message, 
			"3\n\n"
			"          使用道具仓库\n\n"
			"          ＜存放道具＞\n"
			"          ＜取回道具＞\n"
		);

		lssproto_WN_send( fd, WINDOW_MESSAGETYPE_SELECT, 
						WINDOW_BUTTONTYPE_CANCEL,
						CHAR_WINDOWTYPE_DEPOTITEMSHOP_HANDLE,
						CHAR_getWorkInt( meindex, CHAR_WORKOBJINDEX),
						makeEscapeString( message, buf, sizeof(buf)));
	}
}

void NPC_DepotItem_MakeItemString( int meindex, int talkerindex, char *retstring,int retstringlen)
{
	int		i;
	int		itemindex;
	int		pos = 0;
	char	buff[1024];
		
	retstring[0] = '\0';
	for( i = CHAR_STARTITEMARRAY; i < CHAR_MAXITEMHAVE; i ++ ) {
		int poolflg = FALSE;
		itemindex = CHAR_getItemIndex( talkerindex, i);
		if( !ITEM_CHECKINDEX( itemindex) ) continue;
			
		if( ITEM_getInt( itemindex, ITEM_DROPATLOGOUT) || ITEM_getInt( itemindex, ITEM_VANISHATDROP) || 
			!ITEM_getInt( itemindex, ITEM_CANPETMAIL) ) {
			poolflg = TRUE;
		}
		snprintf( buff, sizeof( buff), 
			"%s|%d|%d|%d|%s|%d|",
			ITEM_getChar( itemindex, ITEM_SECRETNAME),
			poolflg,CHAR_getWorkInt( meindex, NPC_WORK_COST),
			ITEM_getInt( itemindex, ITEM_BASEIMAGENUMBER),
			ITEM_getChar( itemindex, ITEM_EFFECTSTRING),
			i + 1 );

		if( pos +strlen( buff)>= retstringlen) {
			fprint( "buffer over err\n");
			break;
		}
		strcpy( &retstring[pos], buff);
		pos += strlen( buff);
	}
}

void NPC_DepotItem_Item_printWindow( int meindex, int talkerindex)
{
	char	itemstring[NPC_UTIL_GETARGSTR_BUFSIZE - 1024 * 20];
	char	sendstring[NPC_UTIL_GETARGSTR_BUFSIZE - 1024 * 20];
	char	argstr[NPC_UTIL_GETARGSTR_BUFSIZE - 1024 * 20];
	char	buff2[1024];
	char	buff3[1024];
	char	buff4[1024];
	int fd;
	
	if( (fd = getfdFromCharaIndex( talkerindex)) == -1 ){
		fprint( "err\n");
		return;
	}
	NPC_Util_GetArgStr( meindex, argstr, sizeof( argstr));
	snprintf( sendstring, sizeof( sendstring), "0|%d|%s|%s|%s|%s|",
			CHAR_getfindEmptyDepotItem( talkerindex),
			CHAR_getChar( meindex, CHAR_NAME),
			NPC_PoolItemShop_getMsg_noarg( NPC_POOLITEMSHOP_MSG_POOL, argstr, buff2, sizeof( buff2)),
			NPC_PoolItemShop_getMsg_noarg( NPC_POOLITEMSHOP_MSG_POOLFULL, argstr, buff3, sizeof( buff3)),
			NPC_PoolItemShop_getMsg_noarg( NPC_POOLITEMSHOP_MSG_REALY, argstr, buff4, sizeof( buff4))
			);

	NPC_DepotItem_MakeItemString( meindex, talkerindex, itemstring, sizeof( itemstring));
	strcat( sendstring, itemstring);
	lssproto_WN_send( fd, WINDOW_MESSAGETYPE_POOLITEMSHOPMAIN,
				WINDOW_BUTTONTYPE_NONE, 
				CHAR_WINDOWTYPE_DEPOTITEMSHOP_ADD,
				CHAR_getWorkInt( meindex, CHAR_WORKOBJINDEX), sendstring);
	
}

void NPC_DepotItem_MakeDepotString( int meindex, int talkerindex, char *retstring,int retstringlen)
{
	int		i;
	int		itemindex;
	int		pos = 0;
	char	buff[1024];

	retstring[0] = '\0';
	for( i = 0; i < /*60*/CHAR_MAXDEPOTITEMHAVE; i ++ ) {
		int poolflg = FALSE;
		itemindex = CHAR_getDepotItemIndex( talkerindex, i);
		if( !ITEM_CHECKINDEX( itemindex)) continue;
		snprintf( buff, sizeof( buff),
			"%s|%d|%d|%d|%d|%s|",
			ITEM_getChar( itemindex, ITEM_SECRETNAME),
			poolflg,
			ITEM_getInt( itemindex, ITEM_LEVEL),
			ITEM_getInt( itemindex, ITEM_COST),
			ITEM_getInt( itemindex, ITEM_BASEIMAGENUMBER),
			ITEM_getChar( itemindex, ITEM_EFFECTSTRING)
			);
		if( pos +strlen( buff)>= retstringlen) {
			fprint( "buffer over err\n");
			break;
		}
		strcpy( &retstring[pos], buff);
		pos += strlen( buff);
	}

}

void NPC_DepotItem_Depot_printWindow( int meindex, int talkerindex)
{
	char	itemstring[NPC_UTIL_GETARGSTR_BUFSIZE - 1024 * 20];
	char	sendstring[NPC_UTIL_GETARGSTR_BUFSIZE - 1024 * 20];
	char	argstr[NPC_UTIL_GETARGSTR_BUFSIZE - 1024 * 20];
	char	buff[1024];
	char	buff2[1024];
	char	buff3[1024];
	int fd;
	
	if( (fd = getfdFromCharaIndex( talkerindex)) == -1 )return;
	NPC_Util_GetArgStr( meindex, argstr, sizeof( argstr));
	snprintf( sendstring, sizeof( sendstring), "1|%s|%s|%s|%s|",
			CHAR_getChar( meindex, CHAR_NAME),
			NPC_PoolItemShop_getMsg_noarg( NPC_POOLITEMSHOP_MSG_DRAW, argstr, buff2, sizeof( buff2)),
			NPC_PoolItemShop_getMsg_noarg( NPC_POOLITEMSHOP_MSG_ITEMFULL, argstr, buff, sizeof( buff)),
			NPC_PoolItemShop_getMsg_noarg( NPC_POOLITEMSHOP_MSG_REALY, argstr, buff3, sizeof( buff3))
			);
	NPC_DepotItem_MakeDepotString( meindex, talkerindex, itemstring, sizeof( itemstring));
	strcat( sendstring, itemstring);
	lssproto_WN_send( fd, WINDOW_MESSAGETYPE_POOLITEMSHOPMAIN,
				WINDOW_BUTTONTYPE_NONE, 
				CHAR_WINDOWTYPE_DEPOTITEMSHOP_GET,
				CHAR_getWorkInt( meindex, CHAR_WORKOBJINDEX),
				sendstring);
	
	//print("\n NPC_DepotItem_Depot_printWindow:%s size:%d ", sendstring, strlen(sendstring));
}

void NPC_DepotItem_CheckRepeat_Del( int charaindex, int ti, int itemindex)
{
	char token[256];
	sprintf( token, "仓库道具%s，编码重复！(系统清除)",
					ITEM_getChar( itemindex, ITEM_NAME));
	CHAR_talkToCli( charaindex, -1, token, CHAR_COLORYELLOW);
	LogItem(
			CHAR_getChar( charaindex, CHAR_NAME ),
			CHAR_getChar( charaindex, CHAR_CDKEY ),
#ifdef _add_item_log_name
			itemindex,
#else
			ITEM_getInt( itemindex, ITEM_ID),
#endif
			"repeat(仓库道具重复)",
			0, 0, 0,
			ITEM_getChar( itemindex, ITEM_UNIQUECODE),
			ITEM_getChar( itemindex, ITEM_NAME),
			ITEM_getInt( itemindex, ITEM_ID) );

	CHAR_setDepotItemIndex( charaindex, ti, -1);
	ITEM_endExistItemsOne( itemindex);
}

void NPC_DepotItem_CheckRepeat( int talkerindex)
{
	int i, j, cnt=0, itemindex, itemindex1;

	for( i=0; i<(CHAR_MAXDEPOTITEMHAVE-1); i++){
		itemindex = CHAR_getDepotItemIndex( talkerindex, i);
		if( !ITEM_CHECKINDEX( itemindex)) continue;
		cnt=0;
		for( j=(i+1); j<CHAR_MAXDEPOTITEMHAVE; j++ ){
			itemindex1 = CHAR_getDepotItemIndex( talkerindex, j);
			if( itemindex==itemindex1) continue;
			if( !ITEM_CHECKINDEX( itemindex1)) continue;
			if( !strcmp( ITEM_getChar( itemindex, ITEM_UNIQUECODE),
				ITEM_getChar( itemindex1, ITEM_UNIQUECODE) ) ){
				NPC_DepotItem_CheckRepeat_Del( talkerindex, j, itemindex1);
				cnt++;
			}
		}
		if( cnt != 0 ){
			NPC_DepotItem_CheckRepeat_Del( talkerindex, i, itemindex);
		}
	}
}

BOOL NPC_DepotItem_InsertItem( int meindex, int talkerindex, int num)
{
	int emptyindex;
	int itemindex;
	int cost = CHAR_getWorkInt( meindex, NPC_WORK_COST);

	if( !CHAR_CheckDepotItem( talkerindex) ) return FALSE;
	if( (emptyindex=CHAR_findEmptyDepotItem( talkerindex) ) == -1 ) return FALSE;
	itemindex = CHAR_getItemIndex( talkerindex, num);
	if( !ITEM_CHECKINDEX( itemindex) )return FALSE;
#if 1 // 共同仓库不可存的物品
	if( ITEM_getInt( itemindex, ITEM_DROPATLOGOUT) || // 登出後消失
			ITEM_getInt( itemindex, ITEM_VANISHATDROP) || // 丢弃後消失
			!ITEM_getInt( itemindex, ITEM_CANPETMAIL)) { // 不可宠邮寄
		print("\n 改封包!!非法存放道具:%s ", CHAR_getChar( talkerindex, CHAR_CDKEY) );
		return FALSE;
	}
#endif
	if( CHAR_DelGold( talkerindex, cost ) == 0 ) return FALSE;

	CHAR_setItemIndex( talkerindex, num, -1);
	CHAR_sendItemDataOne( talkerindex, num);
	CHAR_setDepotItemIndex( talkerindex, emptyindex, itemindex);

//	CHAR_DelGold( talkerindex, cost );
	CHAR_send_P_StatusString( talkerindex, CHAR_P_STRING_GOLD);

	LogItem(
		CHAR_getChar( talkerindex, CHAR_NAME ),
		CHAR_getChar( talkerindex, CHAR_CDKEY ),
#ifdef _add_item_log_name
		itemindex,
#else
		ITEM_getInt( itemindex, ITEM_ID),
#endif
		"Depot(存放道具)",
		CHAR_getInt( talkerindex,CHAR_FLOOR),
		CHAR_getInt( talkerindex,CHAR_X ),
 		CHAR_getInt( talkerindex,CHAR_Y ),
		ITEM_getChar( itemindex, ITEM_UNIQUECODE),
		ITEM_getChar( itemindex, ITEM_NAME),
		ITEM_getInt( itemindex, ITEM_ID) );

	NPC_DepotItem_CheckRepeat( talkerindex);

	return TRUE;
}

BOOL NPC_DepotItem_gettItem( int meindex, int talkerindex, int num)
{
	int emptyindex;
	int itemindex;

	if( !CHAR_CheckDepotItem( talkerindex) ) return FALSE;

	if( (emptyindex=CHAR_findEmptyItemBox( talkerindex)) == -1 ) return FALSE;
	itemindex = CHAR_getDepotItemIndex( talkerindex, num);
	if( !ITEM_CHECKINDEX( itemindex) )return FALSE;

	CHAR_setDepotItemIndex( talkerindex, num, -1);
	CHAR_setItemIndex( talkerindex, emptyindex, itemindex);
	CHAR_sendItemDataOne( talkerindex, emptyindex);
	
	CHAR_send_P_StatusString( talkerindex, CHAR_P_STRING_GOLD);

	LogItem(
		CHAR_getChar( talkerindex, CHAR_NAME ),
		CHAR_getChar( talkerindex, CHAR_CDKEY ),
#ifdef _add_item_log_name
		itemindex,
#else
		ITEM_getInt( itemindex, ITEM_ID),
#endif
		"Depot(取出道具)",
		CHAR_getInt( talkerindex,CHAR_FLOOR),
		CHAR_getInt( talkerindex,CHAR_X ),
 		CHAR_getInt( talkerindex,CHAR_Y ),
		ITEM_getChar( itemindex, ITEM_UNIQUECODE),
		ITEM_getChar( itemindex, ITEM_NAME),
		ITEM_getInt( itemindex, ITEM_ID) );

	{
		int i, cnt=0;
		int work[CHAR_MAXDEPOTITEMHAVE];
		for( i = 0; i < CHAR_MAXDEPOTITEMHAVE; i ++ ) {
			work[i] = -1;
		}
		for( i = 0; i < CHAR_MAXDEPOTITEMHAVE; i ++ ) {
			itemindex = CHAR_getDepotItemIndex( talkerindex, i);
			if( !ITEM_CHECKINDEX( itemindex)) continue;
			work[ cnt++] = itemindex;
		}
		for( i = 0; i < CHAR_MAXDEPOTITEMHAVE; i ++ ) {
			CHAR_setDepotItemIndex( talkerindex, i, work[i]);
		}
	}


	return TRUE;
}
#endif

static void NPC_PoolItemShop_printWindow_Full( int meindex, int talkerindex)
{
	int fd;
	
	fd = getfdFromCharaIndex( talkerindex);
	if( fd != -1 ) {
		char	message[1024];
		char	buf[2048];
		
		strcpy( message, 
			"\n\n    超过的无法处理喔"
				);
		lssproto_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE, 
						WINDOW_BUTTONTYPE_OK,
						CHAR_WINDOWTYPE_POOLITEMSHOP_FULL_MSG,
						CHAR_getWorkInt( meindex, CHAR_WORKOBJINDEX),
						makeEscapeString( message, buf, sizeof(buf)));
	}
	
}

static void NPC_PoolItemShop_printWindow_HaveItemFull( int meindex, int talkerindex)
{
	int fd;
	
	fd = getfdFromCharaIndex( talkerindex);
	if( fd != -1 ) {
		char	message[1024];
		char	buf[2048];
		
		strcpy( message, 
			"\n\n    道具不是已经满了吗"
				);
		lssproto_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE, 
						WINDOW_BUTTONTYPE_OK,
						CHAR_WINDOWTYPE_POOLITEMSHOP_HAVEITEMFULL_MSG,
						CHAR_getWorkInt( meindex, CHAR_WORKOBJINDEX),
						makeEscapeString( message, buf, sizeof(buf)));
	}
}

static void NPC_PoolItemShop_printWindow_Pool( int meindex, int talkerindex)
{
	char	itemstring[NPC_UTIL_GETARGSTR_BUFSIZE - 1024 * 20];
	char	sendstring[NPC_UTIL_GETARGSTR_BUFSIZE - 1024 * 20];
	char	argstr[NPC_UTIL_GETARGSTR_BUFSIZE - 1024 * 20];
	char	buff2[1024];
	char	buff3[1024];
	char	buff4[1024];
	int fd;
	
	fd = getfdFromCharaIndex( talkerindex);
	if( fd == -1 ) {
		fprint( "err\n");
		return;
	}

	NPC_Util_GetArgStr( meindex, argstr, sizeof( argstr));
	
	snprintf( sendstring, sizeof( sendstring), "0|%d|%s|%s|%s|%s|",
			CHAR_getEmptyCharPoolItemIndexNum( talkerindex),
			CHAR_getChar( meindex, CHAR_NAME),
			NPC_PoolItemShop_getMsg_noarg( NPC_POOLITEMSHOP_MSG_POOL, 
										argstr, buff2, sizeof( buff2)),
			NPC_PoolItemShop_getMsg_noarg( NPC_POOLITEMSHOP_MSG_POOLFULL, 
										argstr, buff3, sizeof( buff3)),
			NPC_PoolItemShop_getMsg_noarg( NPC_POOLITEMSHOP_MSG_REALY, 
										argstr, buff4, sizeof( buff4))
			);
	
	NPC_PoolItemShop_MakeItemString_Pool( meindex, talkerindex, itemstring, sizeof( itemstring));
	strcat( sendstring, itemstring);
	lssproto_WN_send( fd, WINDOW_MESSAGETYPE_POOLITEMSHOPMAIN,
				WINDOW_BUTTONTYPE_NONE, 
				CHAR_WINDOWTYPE_POOLITEMSHOP_POOL_MSG,
				CHAR_getWorkInt( meindex, CHAR_WORKOBJINDEX),
				sendstring);
	
}

static void NPC_PoolItemShop_printWindow_Draw( int meindex, int talkerindex)
{
	char	itemstring[NPC_UTIL_GETARGSTR_BUFSIZE - 1024 * 20];
	char	sendstring[NPC_UTIL_GETARGSTR_BUFSIZE - 1024 * 20];
	char	argstr[NPC_UTIL_GETARGSTR_BUFSIZE - 1024 * 20];
	char	buff[1024];
	char	buff2[1024];
	char	buff3[1024];
	int fd;
	
	fd = getfdFromCharaIndex( talkerindex);
	if( fd == -1 ) {
		fprint( "err\n");
		return;
	}

	NPC_Util_GetArgStr( meindex, argstr, sizeof( argstr));
	
	snprintf( sendstring, sizeof( sendstring), "1|%s|%s|%s|%s|",
			CHAR_getChar( meindex, CHAR_NAME),
			NPC_PoolItemShop_getMsg_noarg( NPC_POOLITEMSHOP_MSG_DRAW, 
										argstr, buff2, sizeof( buff2)),
			NPC_PoolItemShop_getMsg_noarg( NPC_POOLITEMSHOP_MSG_ITEMFULL, 
										argstr, buff, sizeof( buff)),
			NPC_PoolItemShop_getMsg_noarg( NPC_POOLITEMSHOP_MSG_REALY, 
										argstr, buff3, sizeof( buff3))
			);
	NPC_PoolItemShop_MakeItemString_Draw( meindex, talkerindex, 
									itemstring, sizeof( itemstring));
	strcat( sendstring, itemstring);
	lssproto_WN_send( fd, WINDOW_MESSAGETYPE_POOLITEMSHOPMAIN,
				WINDOW_BUTTONTYPE_NONE, 
				CHAR_WINDOWTYPE_POOLITEMSHOP_DRAW_MSG,
				CHAR_getWorkInt( meindex, CHAR_WORKOBJINDEX),
				sendstring);
	
}

static char *NPC_PoolItemShop_getMsg_noarg( int tablenum, 
								char *argstr, char *retstring, int retstringlen)
{
	char	buf[1024];
	if( tablenum < 0 || tablenum >= arraysizeof( poolshopmsg)) return "\0";
	
	if( NPC_Util_GetStrFromStrWithDelim( argstr, poolshopmsg[tablenum].option, buf, sizeof( buf)) != NULL ) {
		strcpysafe( retstring, retstringlen, buf);
	}else {
		strcpysafe( retstring, retstringlen, poolshopmsg[tablenum].defaultmsg);
	}
	return retstring;
}

static void NPC_PoolItemShop_MakeItemString_Pool( int meindex, int talkerindex, 
								char *retstring,int retstringlen)
{
	int		i;
	int		itemindex;
	int		pos = 0;
	char	buff[1024];
		
	retstring[0] = '\0';
	for( i = CHAR_STARTITEMARRAY; i < CHAR_MAXITEMHAVE; i ++ ) {
		itemindex = CHAR_getItemIndex( talkerindex, i);
		if( ITEM_CHECKINDEX( itemindex)) {
			int poolflg = FALSE;
			if( ITEM_getInt( itemindex, ITEM_DROPATLOGOUT) || 
				ITEM_getInt( itemindex, ITEM_VANISHATDROP) ||
				!ITEM_getInt( itemindex, ITEM_CANPETMAIL)){
				poolflg = TRUE;
			}
			snprintf( buff, sizeof( buff), 
						"%s|%d|%d|%d|%s|%d|",
						ITEM_getChar( itemindex, ITEM_SECRETNAME),
						poolflg,CHAR_getWorkInt( meindex, NPC_WORK_COST),
						ITEM_getInt( itemindex, ITEM_BASEIMAGENUMBER),
						ITEM_getChar( itemindex, ITEM_EFFECTSTRING),
						i + 1 );
			if( pos +strlen( buff)>= retstringlen) {
				fprint( "buffer over err\n");
				break;
			}
			strcpy( &retstring[pos], buff);
			pos += strlen( buff);
		}
	}
}

static void NPC_PoolItemShop_MakeItemString_Draw( int meindex, int talkerindex, 
								char *retstring,int retstringlen)
{
	int		i;
	int		itemindex;
	int		pos = 0;
	char	buff[1024];
		
	retstring[0] = '\0';
	for( i = 0; i < CHAR_MAXPOOLITEMHAVE; i ++ ) {
		itemindex = CHAR_getPoolItemIndex( talkerindex, i);
		if( ITEM_CHECKINDEX( itemindex)) {
			int poolflg = FALSE;
			snprintf( buff, sizeof( buff),
						"%s|%d|%d|%d|%d|%s|",
						ITEM_getChar( itemindex, ITEM_SECRETNAME),
						poolflg,
						ITEM_getInt( itemindex, ITEM_LEVEL),
						ITEM_getInt( itemindex, ITEM_COST),
						ITEM_getInt( itemindex, ITEM_BASEIMAGENUMBER),
						ITEM_getChar( itemindex, ITEM_EFFECTSTRING)
						);
			if( pos +strlen( buff)>= retstringlen) {
				fprint( "buffer over err\n");
				break;
			}
			strcpy( &retstring[pos], buff);
			pos += strlen( buff);
		}
	}
}

static BOOL NPC_PoolItemShop_PoolItem( int meindex, int talkerindex, int num)
{
	int emptyindex;
	int itemindex;
	int cost = CHAR_getWorkInt( meindex, NPC_WORK_COST);

	emptyindex = CHAR_getCharPoolItemIndexElement( talkerindex);
	if( emptyindex == -1 ) {
		return FALSE;
	}
	                                                                                                  
	itemindex = CHAR_getItemIndex( talkerindex, num);
	if( !ITEM_CHECKINDEX( itemindex)) {
		fprint( "err");
		return FALSE;
	}
	CHAR_DelGold( talkerindex, cost );

	CHAR_setPoolItemIndex( talkerindex, emptyindex, itemindex);

	CHAR_setItemIndex( talkerindex, num, -1);
	CHAR_sendItemDataOne( talkerindex, num);

	CHAR_send_P_StatusString( talkerindex, CHAR_P_STRING_GOLD);
	LogItem(
		CHAR_getChar( talkerindex, CHAR_NAME ), /* 平乓仿   */
		CHAR_getChar( talkerindex, CHAR_CDKEY ),
#ifdef _add_item_log_name  // WON ADD 在item的log中增加item名称
		itemindex,
#else
		ITEM_getInt( itemindex, ITEM_ID),  /* 失奶  丞  寞 */
#endif
		"pool(寄放道具)",
		CHAR_getInt( talkerindex,CHAR_FLOOR),
		CHAR_getInt( talkerindex,CHAR_X ),
 		CHAR_getInt( talkerindex,CHAR_Y ),
		ITEM_getChar( itemindex, ITEM_UNIQUECODE),
				ITEM_getChar( itemindex, ITEM_NAME),
				ITEM_getInt( itemindex, ITEM_ID)

	);
	return TRUE;
}

static BOOL NPC_PoolItemShop_DrawItem( int meindex, int talkerindex, int num)
{
	int emptyindex;
	int itemindex;
	int i;
	int cnt;
	int work[CHAR_MAXPOOLITEMHAVE];

	emptyindex = CHAR_findEmptyItemBox( talkerindex);
	if( emptyindex == -1 ) return FALSE;
	
	itemindex = CHAR_getPoolItemIndex( talkerindex, num);
	if( !ITEM_CHECKINDEX( itemindex)) {
		fprint( "err\n");
		return FALSE;
	}
	CHAR_setItemIndex( talkerindex, emptyindex, itemindex);
	CHAR_setPoolItemIndex( talkerindex, num, -1);
	CHAR_sendItemDataOne( talkerindex, emptyindex);
	for( i = 0; i < CHAR_MAXPOOLITEMHAVE; i ++ ) {
		work[i] = -1;
	}
	cnt = 0;
	for( i = 0; i < CHAR_MAXPOOLITEMHAVE; i ++ ) {
		itemindex = CHAR_getPoolItemIndex( talkerindex, i);
		if( ITEM_CHECKINDEX( itemindex)) {
			work[ cnt++] = itemindex;
		}
	}
	for( i = 0; i < CHAR_MAXPOOLITEMHAVE; i ++ ) {
		CHAR_setPoolItemIndex( talkerindex, i, work[i]);
	}
	LogItem(
		CHAR_getChar( talkerindex, CHAR_NAME ),
		CHAR_getChar( talkerindex, CHAR_CDKEY ),
#ifdef _add_item_log_name  // WON ADD 在item的log中增加item名称
		itemindex,
#else
		ITEM_getInt( itemindex, ITEM_ID),
#endif
		"draw(领取道具)",
		CHAR_getInt( talkerindex,CHAR_FLOOR),
		CHAR_getInt( talkerindex,CHAR_X ),
 		CHAR_getInt( talkerindex,CHAR_Y ),
		ITEM_getChar( itemindex, ITEM_UNIQUECODE),
		ITEM_getChar( itemindex, ITEM_NAME),
		ITEM_getInt( itemindex, ITEM_ID)
	);
	return TRUE;
}
