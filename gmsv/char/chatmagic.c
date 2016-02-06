#include "version.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <malloc.h>
#include <time.h>
#include <stdlib.h>
#include "common.h"
#include "char.h"
#include "util.h"
#include "buf.h"
#include "npcutil.h"
#include "char_base.h"
#include "object.h"
#include "char_data.h"
#include "handletime.h"
#include "chatmagic.h"
#include "configfile.h"
#include "readmap.h"
#include "map_deal.h"
#include "lssproto_serv.h"
#include "log.h"
#include "battle.h"
#include "pet.h"
#include "enemy.h"
#include "encount.h"
#include "magic_base.h"
#include "magic.h"
#include "pet_skill.h"
#include "pet_event.h"
#include "item_gen.h"
#include "npc_eventaction.h"
#include "map_warppoint.h"
#include "npc_manorsman.h"
#include "net.h"
#include "saacproto_cli.h"
#include "npc_scheduleman.h"
#include "trade.h"
#include "npccreate.h"

#include "char_talk.h"

// WON ADD 修正族长问题
#include "family.h"

#include "petmail.h"

#ifdef _AUTO_PK
#include "npc_autopk.h"
#endif

// WON ADD 重新读取ITEMSET.TXT
#include "item.h"

extern	tagRidePetTable	ridePetTable[296];
extern int *pWorkAttackPower;
extern time_t initTime;
int *pWorkAttackPower = NULL;

struct GMINFO gminfo[GMMAXNUM];


void CHAR_CHAT_DEBUG_warp( int charindex , char *message )
{
	char token[100];
	int fl,x,y;
	easyGetTokenFromString( message , 1 , token, sizeof(token));
	fl = atoi( token );
	easyGetTokenFromString( message , 2 , token , sizeof(token));
	x = atoi( token );
	easyGetTokenFromString( message , 3 , token , sizeof(token));
	y = atoi( token );

	CHAR_warpToSpecificPoint(charindex, fl, x, y);

}


void CHAR_CHAT_DEBUG_hp( int charindex , char *message )
{
	CHAR_setInt( charindex , CHAR_HP,atoi( message ) );
	CHAR_complianceParameter( charindex );
	CHAR_send_P_StatusString( charindex , CHAR_P_STRING_HP);
}
void CHAR_CHAT_DEBUG_setmp( int charindex , char *message )
{
	int i;
	char setmp[15];
	char cdkey[CDKEYLEN];
	char token[128];
	int playernum = CHAR_getPlayerMaxNum();
	easyGetTokenFromString( message , 1 , setmp, sizeof(setmp));
	easyGetTokenFromString( message , 2 , cdkey, sizeof(cdkey));
	if(strlen(cdkey)>0){
		for(i=0;i<playernum;i++){
			if(CHAR_CHECKINDEX(i) == FALSE) continue;
			if(!strcmp( CHAR_getChar( i, CHAR_CDKEY), cdkey))
			{
				break;
			}
		}
		if (i>=playernum)
		{
		 	CHAR_talkToCli( charindex, -1, "此账号不在线~", CHAR_COLORYELLOW );   
			return;
		}
		CHAR_setInt( i , CHAR_MAXMP, atoi( setmp ) );
		CHAR_setInt( i , CHAR_MP, atoi( setmp ) );	
		CHAR_complianceParameter( i );
		CHAR_send_P_StatusString( i , CHAR_P_STRING_MP);
		sprintf( token, "[GM]%s把你的MP设置为%d!", CHAR_getChar( charindex, CHAR_NAME),(int)atoi(setmp)/100);
		CHAR_talkToCli( i, -1, token, CHAR_COLORYELLOW );
		sprintf( token, "玩家%s的MP的设置为%d!", CHAR_getChar( i, CHAR_NAME),(int)atoi(setmp)/100);
		CHAR_talkToCli( charindex, -1,token, CHAR_COLORYELLOW );
	}else{
		CHAR_setInt( charindex , CHAR_MAXMP, atoi( setmp ) );
		CHAR_setInt( charindex , CHAR_MP, atoi( setmp ) );	
		CHAR_complianceParameter( charindex );
		CHAR_send_P_StatusString( charindex , CHAR_P_STRING_MP);
		sprintf( token, "MP设置为%d!",(int)atoi(setmp)/100);
		CHAR_talkToCli( charindex, -1,token, CHAR_COLORYELLOW );
	}
}

void CHAR_CHAT_DEBUG_mp( int charindex , char *message )
{
	int Mymp;
	CHAR_setInt( charindex , CHAR_MP, atoi( message ) );
	Mymp = CHAR_getWorkInt( charindex , CHAR_WORKMAXMP);
	if( Mymp > CHAR_getWorkInt( charindex , CHAR_WORKMAXMP) )
		CHAR_setInt( charindex , CHAR_MP, CHAR_getWorkInt( charindex , CHAR_WORKMAXMP));

	CHAR_complianceParameter( charindex );
	//CHAR_sendStatusString( charindex , "P");
	CHAR_send_P_StatusString( charindex , CHAR_P_STRING_MP);

}

void CHAR_CHAT_DEBUG_str( int charindex , char *message )
{
	int i;
	char str[15];
	char cdkey[CDKEYLEN];
	char token[128];
	int playernum = CHAR_getPlayerMaxNum();
	easyGetTokenFromString( message , 1 , str, sizeof(str));
	easyGetTokenFromString( message , 2 , cdkey, sizeof(cdkey));
	if(strlen(cdkey)>0){
		for(i=0;i<playernum;i++){
			if(CHAR_CHECKINDEX(i) == FALSE) continue;
			if(!strcmp( CHAR_getChar( i, CHAR_CDKEY), cdkey))
			{
				break;
			}
		}
		if (i>=playernum)
		{
		 	CHAR_talkToCli( charindex, -1, "此账号不在线~", CHAR_COLORYELLOW );
			return;
		}
		CHAR_setInt( i , CHAR_STR,atoi( str ) );
		CHAR_complianceParameter( i );
		//CHAR_sendStatusString( charindex , "P");
		CHAR_send_P_StatusString( i ,CHAR_P_STRING_STR|CHAR_P_STRING_MAXHP|CHAR_P_STRING_ATK);
		sprintf( token, "[GM]%s把你的腕力设置为%d!", CHAR_getChar( charindex, CHAR_NAME),(int)atoi(str)/100);
		CHAR_talkToCli( i, -1, token, CHAR_COLORYELLOW );
		sprintf( token, "玩家%s的腕力设置为%d!", CHAR_getChar( i, CHAR_NAME),(int)atoi(str)/100);
		CHAR_talkToCli( charindex, -1,token, CHAR_COLORYELLOW );
	}else{
		CHAR_setInt( charindex , CHAR_STR,atoi( str ) );
		CHAR_complianceParameter( charindex );
		//CHAR_sendStatusString( charindex , "P");
		CHAR_send_P_StatusString( charindex ,CHAR_P_STRING_STR|CHAR_P_STRING_MAXHP|CHAR_P_STRING_ATK);
		sprintf( token, "腕力设置为%d!",(int)atoi(str)/100);
		CHAR_talkToCli( charindex, -1,token, CHAR_COLORYELLOW );
	}
}

void CHAR_CHAT_DEBUG_tgh( int charindex , char *message )
{
	int i;
	char tgh[15];
	char cdkey[CDKEYLEN];
	char token[128];
	int playernum = CHAR_getPlayerMaxNum();
	easyGetTokenFromString( message , 1 , tgh, sizeof(tgh));
	easyGetTokenFromString( message , 2 , cdkey, sizeof(cdkey));
	if(strlen(cdkey)>0){
		for(i=0;i<playernum;i++){
			if(CHAR_CHECKINDEX(i) == FALSE) continue;
			if(!strcmp( CHAR_getChar( i, CHAR_CDKEY), cdkey))
			{
				break;
			}
		}
		if (i>=playernum)
		{
		 	CHAR_talkToCli( charindex, -1, "此账号不在线~", CHAR_COLORYELLOW );
			return;
		}
		CHAR_setInt( i , CHAR_TOUGH,atoi( tgh ) );
		CHAR_complianceParameter( i );
		//CHAR_sendStatusString( charindex , "P");
		CHAR_send_P_StatusString( i ,CHAR_P_STRING_TOUGH|CHAR_P_STRING_MAXHP|CHAR_P_STRING_DEF);
		sprintf( token, "[GM]%s把你的耐力设置为%d!", CHAR_getChar( charindex, CHAR_NAME),(int)atoi(tgh)/100);
		CHAR_talkToCli( i, -1, token, CHAR_COLORYELLOW );
		sprintf( token, "玩家%s的耐力设置为%d!", CHAR_getChar( i, CHAR_NAME),(int)atoi(tgh)/100);
		CHAR_talkToCli( charindex, -1,token, CHAR_COLORYELLOW );
	}else{
		CHAR_setInt( charindex , CHAR_TOUGH,atoi( tgh ) );
		CHAR_complianceParameter( charindex );
		//CHAR_sendStatusString( charindex , "P");
		CHAR_send_P_StatusString( charindex ,CHAR_P_STRING_TOUGH|CHAR_P_STRING_MAXHP|CHAR_P_STRING_DEF);
		sprintf( token, "耐力设置为%d!",(int)atoi(tgh)/100);
		CHAR_talkToCli( charindex, -1,token, CHAR_COLORYELLOW );
	}
}

void CHAR_CHAT_DEBUG_dex( int charindex , char *message )
{
	int i;
	char dex[15];
	char cdkey[CDKEYLEN];
	char token[128];
  int playernum = CHAR_getPlayerMaxNum();
  easyGetTokenFromString( message , 1 , dex, sizeof(dex));
  easyGetTokenFromString( message , 2 , cdkey, sizeof(cdkey));
  if(strlen(cdkey)>0){
		for(i=0;i<playernum;i++)
	  {
	    if(CHAR_CHECKINDEX(i) == FALSE) continue;
	    if(!strcmp( CHAR_getChar( i, CHAR_CDKEY), cdkey))
	      {
	        break;
	      }
	  }
	  if (i>=playernum)
	  {
	    CHAR_talkToCli( charindex, -1, "此账号不在线~", CHAR_COLORYELLOW );   
	    return;
	  }
		CHAR_setInt( i , CHAR_DEX,atoi( dex ) );
		CHAR_complianceParameter( i );
		//CHAR_sendStatusString( charindex , "P");
		CHAR_send_P_StatusString( i ,CHAR_P_STRING_DEX|CHAR_P_STRING_MAXHP|CHAR_P_STRING_ATK|CHAR_P_STRING_QUICK);
		sprintf( token, "[GM]%s把你的敏捷设置为%d!", CHAR_getChar( charindex, CHAR_NAME),(int)atoi(dex)/100);
		CHAR_talkToCli( i, -1, token, CHAR_COLORYELLOW );
		sprintf( token, "玩家%s的敏捷设置为%d!", CHAR_getChar( i, CHAR_NAME),(int)atoi(dex)/100);
		CHAR_talkToCli( charindex, -1,token, CHAR_COLORYELLOW );
	}else{
		CHAR_setInt( charindex , CHAR_DEX,atoi( dex ) );
		CHAR_complianceParameter( charindex );
		//CHAR_sendStatusString( charindex , "P");
		CHAR_send_P_StatusString( charindex ,CHAR_P_STRING_DEX|CHAR_P_STRING_MAXHP|CHAR_P_STRING_ATK|CHAR_P_STRING_QUICK);
		sprintf( token, "敏捷设置为%d!",(int)atoi(dex)/100);
		CHAR_talkToCli( charindex, -1,token, CHAR_COLORYELLOW );
	}
}
void CHAR_CHAT_DEBUG_vital( int charindex , char *message )
{
	int i;
	char vital[15];
	char cdkey[CDKEYLEN];
	char token[128];
	int playernum = CHAR_getPlayerMaxNum();
	easyGetTokenFromString( message , 1 , vital, sizeof(vital));
	easyGetTokenFromString( message , 2 , cdkey, sizeof(cdkey));
	if(strlen(cdkey)>0){
		for(i=0;i<playernum;i++){
			if(CHAR_CHECKINDEX(i) == FALSE) continue;
			if(!strcmp( CHAR_getChar( i, CHAR_CDKEY), cdkey))
			{
				break;
			}
		}
		if (i>=playernum)
		{
		 	CHAR_talkToCli( charindex, -1, "此账号不在线~", CHAR_COLORYELLOW );
			return;
		}
		CHAR_setInt( i , CHAR_VITAL,atoi( vital ) );
		CHAR_complianceParameter( i );
		//CHAR_sendStatusString( charindex , "P");
		CHAR_send_P_StatusString( i ,CHAR_P_STRING_VITAL|CHAR_P_STRING_MAXHP|CHAR_P_STRING_DEF);
		sprintf( token, "[GM]%s把你的体力设置为%d!", CHAR_getChar( charindex, CHAR_NAME),(int)atoi(vital)/100);
		CHAR_talkToCli( i, -1, token, CHAR_COLORYELLOW );
		sprintf( token, "玩家%s的体力设置为%d!", CHAR_getChar( i, CHAR_NAME),(int)atoi(vital)/100);
		CHAR_talkToCli( charindex, -1,token, CHAR_COLORYELLOW );
	}else{
		CHAR_setInt( charindex , CHAR_VITAL,atoi( vital ) );
		CHAR_complianceParameter( charindex );
		//CHAR_sendStatusString( charindex , "P");
		CHAR_send_P_StatusString( charindex ,CHAR_P_STRING_VITAL|CHAR_P_STRING_MAXHP|CHAR_P_STRING_DEF);
		sprintf( token, "体力设置为%d!",(int)atoi(vital)/100);
		CHAR_talkToCli( charindex, -1,token, CHAR_COLORYELLOW );
	}
}
void CHAR_CHAT_DEBUG_luck( int charindex , char *message )
{
	int i;
	char buf[64];
	int luck;
	char cdkey[CDKEYLEN];
	char token[128];
	int playernum = CHAR_getPlayerMaxNum();
	easyGetTokenFromString( message , 1 , buf, sizeof(buf));
	luck=atoi(buf);
	easyGetTokenFromString( message , 2 , cdkey, sizeof(cdkey));
	if(luck<1)
		luck=1;
	else if(luck>5)
		luck=5;
	if(strlen(cdkey)>0){
		for(i=0;i<playernum;i++){
			if(CHAR_CHECKINDEX(i) == FALSE) continue;
			if(!strcmp( CHAR_getChar( i, CHAR_CDKEY), cdkey))
			{
				break;
			}
		}
		if (i>=playernum)
		{
		 	CHAR_talkToCli( charindex, -1, "此账号不在线~", CHAR_COLORYELLOW );
			return;
		}
		CHAR_setInt( i , CHAR_LUCK,luck);
		CHAR_complianceParameter( i );
		//CHAR_sendStatusString( charindex , "P");
		CHAR_send_P_StatusString( i , CHAR_P_STRING_LUCK);
		sprintf( token, "[GM]%s把你的运气设置为%d!", CHAR_getChar( charindex, CHAR_NAME),luck);
		CHAR_talkToCli( i, -1, token, CHAR_COLORYELLOW );
		sprintf( token, "玩家%s的运气设置为%d!", CHAR_getChar( i, CHAR_NAME),luck);
		CHAR_talkToCli( charindex, -1,token, CHAR_COLORYELLOW );
	}else{
		CHAR_setInt( charindex , CHAR_LUCK,luck);
		CHAR_complianceParameter( charindex );
		//CHAR_sendStatusString( charindex , "P");
		CHAR_send_P_StatusString( charindex , CHAR_P_STRING_LUCK);
		sprintf( token, "运气设置为%d!",luck);
		CHAR_talkToCli( charindex, -1,token, CHAR_COLORYELLOW );
	}
}

void CHAR_CHAT_DEBUG_gold( int charindex , char *message )
{
	int i, MaxGold, money;
	char cdkey[CDKEYLEN];
	char buf[64];
	char token[128];
	int playernum = CHAR_getPlayerMaxNum();
	easyGetTokenFromString( message , 1 , buf, sizeof(buf));
	money = atoi( buf );
	easyGetTokenFromString( message , 2 , cdkey, sizeof(cdkey));
	MaxGold = CHAR_getMaxHaveGold( charindex);
	if( money > MaxGold )
		money = MaxGold;
	if(strlen(cdkey)>0){
		for(i=0;i<playernum;i++){
			if(CHAR_CHECKINDEX(i) == FALSE) continue;
			if(!strcmp( CHAR_getChar( i, CHAR_CDKEY), cdkey))
			{
				break;
			}
		}
		if (i>=playernum)
		{
		 	CHAR_talkToCli( charindex, -1, "此账号不在线~", CHAR_COLORYELLOW );
			return;
		}
		CHAR_setInt( i , CHAR_GOLD , money );
		CHAR_complianceParameter( i );
		CHAR_send_P_StatusString( i , CHAR_P_STRING_GOLD);
		sprintf( token, "[GM]%s把你的金钱设置为%d!", CHAR_getChar( charindex, CHAR_NAME),money);
		CHAR_talkToCli( i, -1, token, CHAR_COLORYELLOW );
		sprintf( token, "玩家%s的金钱设置为%d!", CHAR_getChar( i, CHAR_NAME),money);
		CHAR_talkToCli( charindex, -1,token, CHAR_COLORYELLOW );
	}else{
		CHAR_setInt( charindex , CHAR_GOLD , money );
		CHAR_complianceParameter( charindex );
		CHAR_send_P_StatusString( charindex , CHAR_P_STRING_GOLD);
		sprintf( token, "金钱设置为%d!",money);
		CHAR_talkToCli( charindex, -1,token, CHAR_COLORYELLOW );
	}
}

#ifdef _TEST_DROPITEMS
void CHAR_CHAT_DEBUG_dropmypet( int charindex, char *message )
{
	char buf[256];
	int petID, x, y, floor, objindex, petindex, count=0;
	int i, j, k, petarray, cLeve=1;

	if( getStringFromIndexWithDelim( message, " ", 1, buf, sizeof( buf)) == FALSE ) return;
	petID = atoi( buf);
	if( getStringFromIndexWithDelim( message, " ", 2, buf, sizeof( buf)) != FALSE ){
		cLeve = atoi( buf);
	}

	floor = CHAR_getInt( charindex, CHAR_FLOOR);
	x = CHAR_getInt( charindex, CHAR_X);
	y = CHAR_getInt( charindex, CHAR_Y);
	if( getStringFromIndexWithDelim( message, " ", 3, buf, sizeof( buf)) != FALSE ){
		floor = atoi( buf);
	}
	if( getStringFromIndexWithDelim( message, " ", 4, buf, sizeof( buf)) != FALSE ){
		x = atoi( buf);
	}
	if( getStringFromIndexWithDelim( message, " ", 5, buf, sizeof( buf)) != FALSE ){
		y = atoi( buf);
	}
	for( i=(x-3); i<(x+3); i++)	{
		for( j=(y-3); j<(y+3); j++)	{
			petarray = ENEMY_getEnemyArrayFromId( petID);
			petindex = ENEMY_TEST_createPetIndex( petarray );
			if( !CHAR_CHECKINDEX( petindex) ){
				CHAR_talkToCli( charindex, -1, "err Can't create pet", CHAR_COLORYELLOW);
				return;
			}
			CHAR_complianceParameter( petindex );
			objindex = PET_dropPetAbsolute( petindex, floor, i, j, FALSE );
			if( objindex == -1 ){
				CHAR_talkToCli( charindex, -1, "err Can't Drop", CHAR_COLORYELLOW);
				return;
			}
			for( k = 1; k < cLeve; k ++ ){	//升级
				CHAR_PetLevelUp( petindex );
				CHAR_PetAddVariableAi( petindex, AI_FIX_PETLEVELUP );
				CHAR_setInt( petindex, CHAR_LV, CHAR_getInt( petindex, CHAR_LV) +1 );
			}
			CHAR_complianceParameter( petindex );
			CHAR_setInt( petindex , CHAR_HP , CHAR_getWorkInt( petindex , CHAR_WORKMAXHP ) );

			CHAR_setWorkInt( petindex, CHAR_WORKOBJINDEX, objindex );
			CHAR_setInt( petindex, CHAR_FLOOR, floor);
			CHAR_setInt( petindex, CHAR_X, i);
			CHAR_setInt( petindex, CHAR_Y, j);
			CHAR_sendCToArroundCharacter( objindex);
			CHAR_setInt( petindex, CHAR_PUTPETTIME, NowTime.tv_sec);
			CHAR_setChar( petindex, CHAR_OWNERCDKEY, "SYSTEM_WAYI" );
			CHAR_setChar( petindex, CHAR_OWNERCHARANAME, "SYSTEM_WAYI" );
			count++;
		}
	}
	sprintf( buf, "总共丢掷 %d只宠物。", count);
	CHAR_talkToCli( charindex, -1, buf, CHAR_COLORYELLOW);


}

void CHAR_CHAT_DEBUG_dropmyitem( int charindex, char *message )
{
	char buf[256];
	int itemID, SitemID, maxflg=0,emptyitemindexinchara, i, j;
	int floor, x, y, objindex;

	if( getStringFromIndexWithDelim( message, " ", 1, buf, sizeof( buf)) == FALSE ) return;
	SitemID = atoi( buf);
	if( getStringFromIndexWithDelim( message, " ", 2, buf, sizeof( buf)) != FALSE ){
		maxflg = atoi( buf);
	}
	floor = CHAR_getInt( charindex, CHAR_FLOOR);
	x = CHAR_getInt( charindex, CHAR_X);
	y = CHAR_getInt( charindex, CHAR_Y);
	if( getStringFromIndexWithDelim( message, " ", 3, buf, sizeof( buf)) != FALSE ){
		floor = atoi( buf);
	}
	if( getStringFromIndexWithDelim( message, " ", 4, buf, sizeof( buf)) != FALSE ){
		x = atoi( buf);
	}
	if( getStringFromIndexWithDelim( message, " ", 5, buf, sizeof( buf)) != FALSE ){
		y = atoi( buf);
	}
	itemID = SitemID;
	

	for( i=(x-3); i<(x+3); i++)	{
		for( j=(y-3); j<(y+3); j++)	{
			int ret = 0 , count=0;
			int itemindex;
			while( count < 10 ){
				count++;
				itemindex = ITEM_makeItemAndRegist( itemID );
				if( maxflg == 1 ) itemID++;
				if( !ITEM_CHECKINDEX( itemindex)) continue;
				break;
			}
			emptyitemindexinchara = CHAR_findEmptyItemBox( charindex );

			CHAR_setItemIndex( charindex, emptyitemindexinchara, itemindex );
			ITEM_setWorkInt(itemindex, ITEM_WORKOBJINDEX,-1);
			ITEM_setWorkInt(itemindex, ITEM_WORKCHARAINDEX,charindex);
			CHAR_sendItemDataOne( charindex, emptyitemindexinchara);
			//drop
			if( (ret = CHAR_DropItemFXY( charindex, emptyitemindexinchara,
				floor, i, j, &objindex )) != 0 ){
				char buf[256];
				sprintf( buf, "错误%d", ret);
				CHAR_talkToCli( charindex, -1, buf, CHAR_COLORYELLOW);
			}
			ITEM_setInt(itemindex,ITEM_PUTTIME,NowTime.tv_sec);
			CHAR_sendWatchEvent(objindex,CHAR_ACTSTAND,NULL,0,TRUE);
			CHAR_setItemIndex(charindex, emptyitemindexinchara ,-1);
			CHAR_sendItemDataOne(charindex, emptyitemindexinchara);
			CHAR_sendCToArroundCharacter(CHAR_getWorkInt( charindex, CHAR_WORKOBJINDEX));
		}
	}
	{
				char buf[256];
				sprintf( buf, "最後ID%d", itemID-1);
				CHAR_talkToCli( charindex, -1, buf, CHAR_COLORYELLOW);
	}
}
#endif

void CHAR_CHAT_DEBUG_additem( int charindex , char *message )
{

  int emptyitemindexinchara, itemindex;
	int	i;
	int num = 0;
	int itemid;
	char buf[64];
	char	msgbuf[128];
  char cdkey[CDKEYLEN];
	easyGetTokenFromString( message , 1 , buf, sizeof(buf));
	itemid = atoi( buf);
	easyGetTokenFromString( message , 2 , buf, sizeof(buf));
	num = atoi( buf);
	easyGetTokenFromString( message , 3 , cdkey, sizeof(cdkey));

	if( num <= 0 ) num = 1;

	if(strlen(cdkey)>0){
		int playernum = CHAR_getPlayerMaxNum();
		int j;
		for(j=0;j<playernum;j++)
	  {
	     if(CHAR_CHECKINDEX(j) == FALSE) continue;
	     if(!strcmp( CHAR_getChar( j, CHAR_CDKEY), cdkey))
	     {
	        break;
	     }
	  }
	  if (j>=playernum)
	  {
       CHAR_talkToCli( charindex, -1, "此账号不在线~", CHAR_COLORYELLOW );   
       return;
    }
		for( i = 0; i < num; i ++ ) {
		    emptyitemindexinchara = CHAR_findEmptyItemBox( j );
	
			if( emptyitemindexinchara < 0 ){
				sprintf( msgbuf, "玩家%s物品栏位不足。", CHAR_getChar( j, CHAR_NAME ));
				CHAR_talkToCli( charindex, -1, msgbuf, CHAR_COLORYELLOW);
				sprintf( msgbuf, "你的%s物品栏位不足。", CHAR_getChar( j, CHAR_NAME ));
				CHAR_talkToCli( j, -1, msgbuf, CHAR_COLORYELLOW);
				return;
			}

			itemindex = ITEM_makeItemAndRegist( itemid );
				
		    if( itemindex != -1 ){
		        CHAR_setItemIndex( j, emptyitemindexinchara, itemindex );
		        ITEM_setWorkInt(itemindex, ITEM_WORKOBJINDEX,-1);
		        ITEM_setWorkInt(itemindex, ITEM_WORKCHARAINDEX,j);
		       	CHAR_sendItemDataOne( j, emptyitemindexinchara);
				LogItem(
					CHAR_getChar( charindex, CHAR_NAME ),
					CHAR_getChar( charindex, CHAR_CDKEY ),
#ifdef _add_item_log_name  // WON ADD 在item的log中增加item名称
		itemindex,
#else
		atoi( message),
#endif
					"AddItem(制作道具GM)",
			       	CHAR_getInt( charindex,CHAR_FLOOR),
					CHAR_getInt( charindex,CHAR_X ),
		 	      	CHAR_getInt( charindex,CHAR_Y ),
	                ITEM_getChar( itemindex, ITEM_UNIQUECODE),
						ITEM_getChar( itemindex, ITEM_NAME),
						ITEM_getInt( itemindex, ITEM_ID)
	
				);
				snprintf( msgbuf,sizeof( msgbuf),"[GM]%s给你制作%s成功。",
							 CHAR_getChar( charindex, CHAR_NAME ),ITEM_getChar( itemindex, ITEM_NAME));
				CHAR_talkToCli( j, -1,msgbuf, CHAR_COLORYELLOW);
				snprintf( msgbuf, sizeof( msgbuf), "成功为%s制作 %s。",
											CHAR_getChar( j, CHAR_NAME ),ITEM_getChar( itemindex, ITEM_NAME));
				CHAR_talkToCli( charindex, -1,msgbuf, CHAR_COLORYELLOW);
			}else{
				sprintf( msgbuf, "制作道具失败。" );
				CHAR_talkToCli( charindex, -1,msgbuf, CHAR_COLORYELLOW);
			}
		}
	}else{
		for( i = 0; i < num; i ++ ) {
	    emptyitemindexinchara = CHAR_findEmptyItemBox( charindex );

		if( emptyitemindexinchara < 0 ){
			sprintf( msgbuf, "物品栏位不足。" );

			CHAR_talkToCli( charindex, -1, msgbuf, CHAR_COLORYELLOW);

			return;
		}

		itemindex = ITEM_makeItemAndRegist( itemid );

	    if( itemindex != -1 ){
	        CHAR_setItemIndex( charindex, emptyitemindexinchara, itemindex );
	        ITEM_setWorkInt(itemindex, ITEM_WORKOBJINDEX,-1);
	        ITEM_setWorkInt(itemindex, ITEM_WORKCHARAINDEX,charindex);
	       	CHAR_sendItemDataOne( charindex, emptyitemindexinchara);
			LogItem(
				CHAR_getChar( charindex, CHAR_NAME ),
				CHAR_getChar( charindex, CHAR_CDKEY ),
#ifdef _add_item_log_name  // WON ADD 在item的log中增加item名称
				itemindex,
#else
				atoi( message),
#endif
				"AddItem(制作道具GM)",
		       	CHAR_getInt( charindex,CHAR_FLOOR),
				CHAR_getInt( charindex,CHAR_X ),
	 	      	CHAR_getInt( charindex,CHAR_Y ),
                ITEM_getChar( itemindex, ITEM_UNIQUECODE),
					ITEM_getChar( itemindex, ITEM_NAME),
					ITEM_getInt( itemindex, ITEM_ID)

			);
			snprintf( msgbuf, sizeof( msgbuf), "制作%s成功。",
										ITEM_getChar( itemindex, ITEM_NAME));
				CHAR_talkToCli( charindex, -1,msgbuf, CHAR_COLORYELLOW);
			}else{
				sprintf( msgbuf, "制作道具失败。" );
				CHAR_talkToCli( charindex, -1, msgbuf, CHAR_COLORYELLOW);
			}
		}
	}
}


void CHAR_CHAT_DEBUG_metamo( int charindex , char *message )
{
		char metamoid[7];
    char cdkey[CDKEYLEN];
		char token[128];
    int i=0;
    int playernum = CHAR_getPlayerMaxNum();
    easyGetTokenFromString( message , 1 , metamoid, sizeof(metamoid));
    easyGetTokenFromString( message , 2 , cdkey, sizeof(cdkey));
    if(strlen(cdkey)>0){
			for(i=0;i<playernum;i++)
	    {
	        if(CHAR_CHECKINDEX(i) == FALSE) continue;
	        if(!strcmp( CHAR_getChar( i, CHAR_CDKEY), cdkey))
	        {
	          break;
	        }
	    }
	    if (i>=playernum)
	     {
	       	CHAR_talkToCli( charindex, -1, "此账号不在线~", CHAR_COLORYELLOW );   
	        return;
	     }
			CHAR_setInt( i , CHAR_BASEIMAGENUMBER , atoi( metamoid ) );
			CHAR_setInt( i , CHAR_BASEBASEIMAGENUMBER , atoi( metamoid ) );
	
			CHAR_complianceParameter( i );
			CHAR_sendCToArroundCharacter( CHAR_getWorkInt( i , CHAR_WORKOBJINDEX ));
			CHAR_send_P_StatusString( i , CHAR_P_STRING_BASEBASEIMAGENUMBER);
			sprintf( token, "[GM]%s把你的人物形象设置为%d!", CHAR_getChar( charindex, CHAR_NAME),(int)atoi(metamoid));
			CHAR_talkToCli( i, -1, token, CHAR_COLORYELLOW );
			sprintf( token, "玩家%s的人物形象设置为%d!", CHAR_getChar( i, CHAR_NAME),(int)atoi(metamoid));
			CHAR_talkToCli( charindex, -1,token, CHAR_COLORYELLOW );
    }else{
			CHAR_setInt( charindex , CHAR_BASEIMAGENUMBER , atoi( metamoid ) );
			CHAR_setInt( charindex , CHAR_BASEBASEIMAGENUMBER , atoi( metamoid ) );
	
			CHAR_complianceParameter( charindex );
			CHAR_sendCToArroundCharacter( CHAR_getWorkInt( charindex , CHAR_WORKOBJINDEX ));
			CHAR_send_P_StatusString( charindex , CHAR_P_STRING_BASEBASEIMAGENUMBER);
			sprintf( token, "人物形象设置为%d!",(int)atoi(metamoid));
			CHAR_talkToCli( charindex, -1,token, CHAR_COLORYELLOW );
		}
}

void CHAR_CHAT_DEBUG_info( int charindex , char *message )
{
	char line[256];
	int		index;

	if( strlen( message) != 0 ) {
		index = atoi( message);
	}
	else {
		index = charindex;
	}
	if( !CHAR_CHECKINDEX( index)) {
		CHAR_talkToCli( charindex, -1 , "该index无法使用。" , CHAR_COLORWHITE );
		return;
	}

	snprintf( line, sizeof( line),
			  "name:%s i:%d B:%d BB:%d LV:%d EXP:%d MakeSeq:%d",
			  CHAR_getChar( index, CHAR_NAME),
			  charindex,
			  CHAR_getInt( index, CHAR_BASEIMAGENUMBER ),
			  CHAR_getInt( index, CHAR_BASEBASEIMAGENUMBER ),
			  CHAR_getInt( index, CHAR_LV ),
			  CHAR_getInt( index, CHAR_EXP ),
			  CHAR_getCharMakeSequenceNumber( index));

	CHAR_talkToCli( charindex, -1 , line , CHAR_COLORWHITE );

	snprintf( line, sizeof( line),
			  "Gold:%d HP:%d MP/MHP:%d/%d" ,
			  CHAR_getInt( index, CHAR_GOLD ),
			  CHAR_getInt( index, CHAR_HP ),
			  CHAR_getInt( index, CHAR_MP ),
			  CHAR_getInt( index, CHAR_MAXMP ));
	CHAR_talkToCli( charindex, -1 , line , CHAR_COLORWHITE );


	if( CHAR_getInt( index, CHAR_WHICHTYPE ) == CHAR_TYPEPET ){
		snprintf( line, sizeof(line),
			  "体:%d 力:%d 丈:%d 早:%d 耐:%d 攻:%d 守:%d 敏:%d 忠修:%d 忠变:%.2f 编码:%s",
			  CHAR_getInt( index, CHAR_VITAL )/100,
			  CHAR_getInt( index, CHAR_STR )/100,
			  CHAR_getInt( index, CHAR_TOUGH )/100,
			  CHAR_getInt( index, CHAR_DEX )/100,
			  CHAR_getWorkInt( index, CHAR_WORKFIXVITAL ),
			  CHAR_getWorkInt( index, CHAR_WORKATTACKPOWER ),
			  CHAR_getWorkInt( index, CHAR_WORKDEFENCEPOWER ),//CHAR_WORKFIXDEX
			  CHAR_getWorkInt( index, CHAR_WORKQUICK ),
			  CHAR_getInt( index, CHAR_MODAI ),
			  CHAR_getInt( index, CHAR_VARIABLEAI ) * 0.01,			  
			  CHAR_getChar( index, CHAR_UNIQUECODE) );
	}else{
		snprintf( line, sizeof(line),
			  "体:%d 力:%d 丈:%d 早:%d 耐:%d 攻:%d 守:%d 魅:%d 运:%d",
			  CHAR_getInt( index, CHAR_VITAL )/100,
			  CHAR_getInt( index, CHAR_STR )/100,
			  CHAR_getInt( index, CHAR_TOUGH )/100,
			  CHAR_getInt( index, CHAR_DEX )/100,
			  CHAR_getWorkInt( index, CHAR_WORKFIXVITAL ),
			  CHAR_getWorkInt( index, CHAR_WORKATTACKPOWER ),
			  CHAR_getWorkInt( index, CHAR_WORKDEFENCEPOWER ),
			  CHAR_getWorkInt( index, CHAR_WORKFIXCHARM ),
			  CHAR_getWorkInt( index, CHAR_WORKFIXLUCK ));
	}
	CHAR_talkToCli( charindex, -1 , line , CHAR_COLORWHITE );


	snprintf( line, sizeof(line),
			  "土:%d 水:%d 火:%d 风:%d",
			  CHAR_getWorkInt( index, CHAR_WORKFIXEARTHAT ),
			  CHAR_getWorkInt( index, CHAR_WORKFIXWATERAT ),
			  CHAR_getWorkInt( index, CHAR_WORKFIXFIREAT ),
			  CHAR_getWorkInt( index, CHAR_WORKFIXWINDAT ));
	CHAR_talkToCli( charindex, -1 , line , CHAR_COLORWHITE );

	snprintf( line, sizeof( line),
			  "Sk Itm:%d ChV:%d Rad:%d" ,
			  CHAR_MAXITEMHAVE,
			  CHAR_getInt( index, CHAR_CHATVOLUME ),
			  CHAR_getInt( index, CHAR_RADARSTRENGTH ) );
	CHAR_talkToCli( charindex, -1 , line , CHAR_COLORWHITE );


	snprintf( line, sizeof( line),
			  "Status 毒:%d 麻:%d 默:%d 石:%d 暗:%d 乱:%d" ,
			  CHAR_getInt( index , CHAR_POISON ),
			  CHAR_getInt( index , CHAR_PARALYSIS ),
			  CHAR_getInt( index , CHAR_SLEEP ),
			  CHAR_getInt( index , CHAR_STONE ),
			  CHAR_getInt( index , CHAR_DRUNK ),
			  CHAR_getInt( index , CHAR_CONFUSION ) );
	CHAR_talkToCli( charindex, -1 , line , CHAR_COLORWHITE );

#ifdef _PERSONAL_FAME	// Arminius: 家族个人声望
	snprintf( line, sizeof( line),
			  "TitInd:%d FL:%d X:%d Y:%d DIR:%d FAME:%d" ,
			  CHAR_getInt( index , CHAR_INDEXOFEQTITLE ),
			  CHAR_getInt( index , CHAR_FLOOR ),
			  CHAR_getInt( index , CHAR_X ),
			  CHAR_getInt( index , CHAR_Y ),
			  CHAR_getInt( index , CHAR_DIR ),
			  CHAR_getInt( index , CHAR_FAME)
				);
#else
	snprintf( line, sizeof( line),
				  "TitInd:%d FL:%d X:%d Y:%d DIR:%d" ,
			  CHAR_getInt( index , CHAR_INDEXOFEQTITLE ),
			  CHAR_getInt( index , CHAR_FLOOR ),
			  CHAR_getInt( index , CHAR_X ),
			  CHAR_getInt( index , CHAR_Y ),
			  CHAR_getInt( index , CHAR_DIR ) );
#endif
	CHAR_talkToCli( charindex, -1 , line , CHAR_COLORWHITE );


	snprintf( line, sizeof( line),
				  "walk:%d dead:%d ItemMergecnt:%d" ,
			  CHAR_getInt( index , CHAR_WALKCOUNT ) ,
			  CHAR_getInt( index , CHAR_DEADCOUNT ) ,
			  CHAR_getInt( index, CHAR_MERGEITEMCOUNT)
			  );
	CHAR_talkToCli( charindex, -1 , line , CHAR_COLORWHITE );

	if( CHAR_getInt( index, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER) {
		snprintf( line, sizeof( line),
					  "petindex [%d][%d][%d][%d][%d] " ,
				CHAR_getCharPet( index, 0),
				CHAR_getCharPet( index, 1),
				CHAR_getCharPet( index, 2),
				CHAR_getCharPet( index, 3),
				CHAR_getCharPet( index, 4));
	}
	else {
		snprintf( line, sizeof( line),
					  "PetskillIndex [%d][%d][%d][%d][%d][%d][%d] " ,
				CHAR_getPetSkill( index, 0),
				CHAR_getPetSkill( index, 1),
				CHAR_getPetSkill( index, 2),
				CHAR_getPetSkill( index, 3),
				CHAR_getPetSkill( index, 4),
				CHAR_getPetSkill( index, 5),
				CHAR_getPetSkill( index, 6));
	}
	
	CHAR_talkToCli( charindex, -1 , line , CHAR_COLORWHITE );
	if( CHAR_getInt( index, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER) {
		snprintf( line, sizeof( line),
					  "poolpetindex [%d][%d][%d][%d][%d] " ,
				CHAR_getCharPoolPet( index, 0),
				CHAR_getCharPoolPet( index, 1),
				CHAR_getCharPoolPet( index, 2),
				CHAR_getCharPoolPet( index, 3),
				CHAR_getCharPoolPet( index, 4));
		CHAR_talkToCli( charindex, -1 , line , CHAR_COLORWHITE );

		snprintf( line, sizeof( line),
					  "poolpetindex [%d][%d][%d][%d][%d] " ,
				CHAR_getCharPoolPet( index, 5),
				CHAR_getCharPoolPet( index, 6),
				CHAR_getCharPoolPet( index, 7),
				CHAR_getCharPoolPet( index, 8),
				CHAR_getCharPoolPet( index, 9));
		CHAR_talkToCli( charindex, -1 , line , CHAR_COLORWHITE );
	}

	snprintf( line, sizeof( line),
				  "levelup [%d], 累积 %d   赌场积分[%d]" ,
			CHAR_getInt( index, CHAR_LEVELUPPOINT),
			CHAR_getWorkInt( index, 	CHAR_WORKULTIMATE),
			CHAR_getInt( index, CHAR_GAMBLENUM) );
	CHAR_talkToCli( charindex, -1 , line , CHAR_COLORWHITE );

}

void CHAR_CHAT_DEBUG_sysinfo( int charindex , char *message )
{
	char msg[1024],name[256];
	int i;
	time_t new_t;
	int dd,hh,mm,ss;

	int items=0 , chars=0, golds=0, nouses=0;
	int	players=0,pets=0,others=0;
	int	whichtype= -1;
	int objnum = OBJECT_getNum();


	int itemnum = ITEM_getITEM_itemnum();
	int itemuse =0;

	makeEscapeString( getGameserverID(), name, sizeof( name));
	sprintf( msg, "GSID:%s GSNAME:%s", getGameserverID(), name);
	CHAR_talkToCli( charindex, -1, msg, CHAR_COLORYELLOW);


	for( i=0 ; i<objnum ; i++){
		switch( OBJECT_getType( i )){
		case OBJTYPE_NOUSE:
			nouses++;
			break;
		case OBJTYPE_CHARA:
			chars++;
			whichtype =  CHAR_getInt( OBJECT_getIndex( i), CHAR_WHICHTYPE);
			if( whichtype == CHAR_TYPEPLAYER) players++;
			else if( whichtype == CHAR_TYPEPET) pets++;
			else others ++;
			break;
		case OBJTYPE_ITEM:
			items++;
			break;
		case OBJTYPE_GOLD:
			golds++;
			break;
		default:
			break;
		}
	}

	for( i=0 ; i<itemnum ; i++)
		if( ITEM_getITEM_use(i) )
			itemuse++;

	snprintf( msg , sizeof(msg),
			  "Object Char:%d Item:%d Gold:%d Nouse:%d Item use:%d",
			  chars,items,golds, nouses, itemuse );

	CHAR_talkToCli(charindex,-1,msg,CHAR_COLORYELLOW );

	snprintf( msg , sizeof(msg),
			  "player : %d  pet : %d  others  : %d",
			  players, pets, others );

	CHAR_talkToCli(charindex,-1,msg,CHAR_COLORYELLOW );
	
	time(&new_t);
	new_t -= initTime;
        dd=(int) new_t / 86400; new_t=new_t % 86400;
        hh=(int) new_t / 3600;  new_t=new_t % 3600;
        mm=(int) new_t / 60;    new_t=new_t % 60;
        ss=(int) new_t;

	snprintf( msg, sizeof( msg ) ,
          	"start: %d days %d hours %d min %d sec。",dd,hh,mm,ss);
        CHAR_talkToCli( charindex, -1, msg, CHAR_COLORYELLOW );

	{
		showMem( msg);
		CHAR_talkToCli( charindex, -1, msg, CHAR_COLORYELLOW);
	}
	snprintf( msg, sizeof( msg ) ,
			"NPC_createnum:%d。",NPC_createnum);
	CHAR_talkToCli( charindex, -1, msg, CHAR_COLORYELLOW );

}


void CHAR_CHAT_DEBUG_announce(int charindex , char *message )
{
    int     i;
    int     playernum = CHAR_getPlayerMaxNum();

    for( i = 0 ; i < playernum ; i++) {
        if( CHAR_getCharUse(i) != FALSE ) {
			CHAR_talkToCli( i, -1, message, CHAR_COLORRED);
    }
	}
}

void CHAR_CHAT_DEBUG_level( int charaindex, char* message )
{
	int i, level;
	char buf[64];
	char cdkey[CDKEYLEN];
	char token[128];
	int playernum = CHAR_getPlayerMaxNum();
	easyGetTokenFromString( message , 1 , buf, sizeof(buf));
	level=atoi(buf);
	easyGetTokenFromString( message , 2 , cdkey, sizeof(cdkey));
	if(strlen(cdkey)>0){
		for(i=0;i<playernum;i++){
			if(CHAR_CHECKINDEX(i) == FALSE) continue;
			if(!strcmp( CHAR_getChar( i, CHAR_CDKEY), cdkey))
			{
				break;
			}
		}
		if (i>=playernum)
		{
		 	CHAR_talkToCli( charaindex, -1, "此账号不在线~", CHAR_COLORYELLOW );
			return;
		}
		CHAR_setInt(i,CHAR_LV, atoi( message ));
		CHAR_send_P_StatusString( i , CHAR_P_STRING_LV|CHAR_P_STRING_NEXTEXP);
		sprintf( token, "[GM]%s把你的等级设置为%d!", CHAR_getChar( charaindex, CHAR_NAME),level);
		CHAR_talkToCli( i, -1, token, CHAR_COLORYELLOW );
		sprintf( token, "玩家%s的等级设置为%d!", CHAR_getChar( i, CHAR_NAME),level);
		CHAR_talkToCli( charaindex, -1,token, CHAR_COLORYELLOW );
	}else{
		CHAR_setInt(charaindex,CHAR_LV, atoi( message ));
		CHAR_send_P_StatusString( charaindex , CHAR_P_STRING_LV|CHAR_P_STRING_NEXTEXP);
		sprintf( token, "等级设置为%d!",level);
		CHAR_talkToCli( charaindex, -1,token, CHAR_COLORYELLOW );
	}
	
}


void CHAR_CHAT_DEBUG_exp( int charaindex, char* message )
{
	int i;
	char exp[15];
	char cdkey[CDKEYLEN];
	char token[128];
	int playernum = CHAR_getPlayerMaxNum();
	easyGetTokenFromString( message , 1 , exp, sizeof(exp));
	easyGetTokenFromString( message , 2 , cdkey, sizeof(cdkey));
	if(strlen(cdkey)>0){
		for(i=0;i<playernum;i++){
			if(CHAR_CHECKINDEX(i) == FALSE) continue;
			if(!strcmp( CHAR_getChar( i, CHAR_CDKEY), cdkey))
			{
				break;
			}
		}
		if (i>=playernum)
		{
		 	CHAR_talkToCli( charaindex, -1, "此账号不在线~", CHAR_COLORYELLOW );
			return;
		}
		CHAR_setMaxExp( i, atoi( exp ));
		CHAR_send_P_StatusString( i , CHAR_P_STRING_EXP);
		sprintf( token, "[GM]%s把你的经验设置为%d!", CHAR_getChar( charaindex, CHAR_NAME),(int)atoi(exp));
		CHAR_talkToCli( i, -1, token, CHAR_COLORYELLOW );
		sprintf( token, "玩家%s的经验设置为%d!", CHAR_getChar( i, CHAR_NAME),(int)atoi(exp));
		CHAR_talkToCli( charaindex, -1,token, CHAR_COLORYELLOW );
	}else{
		CHAR_setMaxExp( charaindex, atoi( exp ));
		CHAR_send_P_StatusString( charaindex , CHAR_P_STRING_EXP);
		sprintf( token, "经验设置为%d!",(int)atoi(exp)/100);
		CHAR_talkToCli( charaindex, -1,token, CHAR_COLORYELLOW );
	}
}

void CHAR_CHAT_DEBUG_delitem( int charindex , char *message )
{
	int		itemid;

	if( strcmp( message, "all") == 0 ) {
		int		i;
		for( i = CHAR_STARTITEMARRAY; i < CHAR_MAXITEMHAVE; i ++ ) {
		    itemid = CHAR_getItemIndex(charindex, i);
			if( itemid != -1 ) {
				{
					LogItem(
						CHAR_getChar( charindex, CHAR_NAME ),
						CHAR_getChar( charindex, CHAR_CDKEY ),
#ifdef _add_item_log_name  // WON ADD 在item的log中增加item名称
						itemid,
#else
			       		ITEM_getInt( itemid, ITEM_ID ),
#endif
						"DelItem(删除道具GM)",
						CHAR_getInt( charindex,CHAR_FLOOR),
						CHAR_getInt( charindex,CHAR_X ),
 			    	  	CHAR_getInt( charindex,CHAR_Y ),
                        ITEM_getChar( itemid, ITEM_UNIQUECODE),
						ITEM_getChar( itemid, ITEM_NAME),
						ITEM_getInt( itemid, ITEM_ID)
					);
				}
				CHAR_DelItem( charindex, i);
			}
		}
	}else {
		int		equipplace;
		equipplace = atoi( message);
		if( equipplace < CHAR_STARTITEMARRAY || equipplace > CHAR_MAXITEMHAVE -1 ) {
			CHAR_talkToCli( charindex, -1, "号码不正确。",  CHAR_COLORWHITE);
			return;
		}
	    itemid = CHAR_getItemIndex(charindex, equipplace);
		if( itemid == -1 ) return;
		{
			LogItem(
				CHAR_getChar( charindex, CHAR_NAME ),
				CHAR_getChar( charindex, CHAR_CDKEY ),
#ifdef _add_item_log_name  // WON ADD 在item的log中增加item名称
				itemid,
#else
	       		ITEM_getInt( itemid, ITEM_ID ),
#endif
				"DelItem(删除道具GM)",
				CHAR_getInt( charindex,CHAR_FLOOR),
				CHAR_getInt( charindex,CHAR_X ),
 	      		CHAR_getInt( charindex,CHAR_Y ),
                ITEM_getChar( itemid, ITEM_UNIQUECODE),
				ITEM_getChar( itemid, ITEM_NAME),
				ITEM_getInt( itemid, ITEM_ID)
			);
		}
		CHAR_DelItem( charindex, equipplace);
	}
}

#ifdef _MAKE_MAP
void CHAR_CHAT_DelMap( int charaindex, char* message ){

	int   ff, x, y, xx, yy;
  int		tile, obj;

	ff = CHAR_getInt( charaindex, CHAR_FLOOR );
	
	if( MAP_getFloorXY( ff, &xx, &yy) == FALSE )return;

	for(x=0;x<xx;x++){
		for(y=0;y<yy;y++){
			if( MAP_getTileAndObjData( ff, x, y, &tile, &obj) == FALSE )continue;
			printf("\nff:%d,x:%d,y:%d,tile:%d,obj:%d\n", ff, x, y, tile, obj);
			if(atoi(message)==1){
				MAP_setTileAndObjData( ff, x, y, tile, 0 );
			}else if(atoi(message)==2){
				MAP_setTileAndObjData( ff, x, y, 0, obj );
			}else if(atoi(message)==3){
				MAP_setTileAndObjData( ff, x, y, 0, 0 );
			}
		}
	}
}

void CHAR_CHAT_GetMap( int charaindex, char* message ){

	int 	dir, ff, fx, fy;
  int		tile, obj;

	ff = CHAR_getInt( charaindex, CHAR_FLOOR );
	fx = CHAR_getInt( charaindex, CHAR_X );
	fy = CHAR_getInt( charaindex, CHAR_Y );
	dir = CHAR_getInt( charaindex, CHAR_DIR );

	fx += CHAR_getDX(dir);
	fy += CHAR_getDY(dir);

	if( MAP_getTileAndObjData( ff, fx, fy, &tile, &obj)	== FALSE )return;

	{	
		char szBuffer[64];
		snprintf( szBuffer, sizeof( szBuffer ), "图片= %d; 对象= %d",
			tile, obj );
		CHAR_talkToCli( charaindex, -1, szBuffer,  CHAR_COLORWHITE);
	}
}

void CHAR_CHAT_Map( int charaindex, char* message ){

	int 	dir, ff, fx, fy;
  int		tile, obj;
  int		fixtile, fixobj;
  char  buf[16];
  
	getStringFromIndexWithDelim( message, " ", 1, buf, sizeof( buf));
	fixobj=atoi(buf);
	getStringFromIndexWithDelim( message, " ", 2, buf, sizeof( buf));
	fixtile=atoi(buf);
	ff = CHAR_getInt( charaindex, CHAR_FLOOR );
	fx = CHAR_getInt( charaindex, CHAR_X );
	fy = CHAR_getInt( charaindex, CHAR_Y );
	dir = CHAR_getInt( charaindex, CHAR_DIR );

	fx += CHAR_getDX(dir);
	fy += CHAR_getDY(dir);

	if( MAP_getTileAndObjData( ff, fx, fy, &tile, &obj)	== FALSE )return;

	{	
		char szBuffer[64];
		snprintf( szBuffer, sizeof( szBuffer ), "图片：%d => %d",	tile, fixtile );
		CHAR_talkToCli( charaindex, -1, szBuffer,  CHAR_COLORWHITE);
		snprintf( szBuffer, sizeof( szBuffer ), "对象：%d => %d",	obj, fixobj );
		CHAR_talkToCli( charaindex, -1, szBuffer,  CHAR_COLORWHITE);
	}
	
	MAP_setTileAndObjData( ff, fx, fy, fixtile, fixobj );

	{
		char *stringdata;
		RECT seekr , retr;
		seekr.x = fx;
		seekr.y = fy;
		seekr.width = 1;
		seekr.height = 1;
		stringdata = MAP_getdataFromRECT(ff,&seekr,&retr);
		if( stringdata ){

			if( CHAR_getInt( charaindex , CHAR_WHICHTYPE )
			== CHAR_TYPEPLAYER ){
				int fd = getfdFromCharaIndex( charaindex );
				lssproto_M_send(fd,ff,
					retr.x,retr.y,
					retr.x+retr.width,retr.y+retr.height,
					stringdata );
			}
		} else {
		}
	}

    return ;
}

void CHAR_CHAT_Fixtile( int charaindex, char* message ){

	int 	dir, ff, fx, fy;
  int		tile, obj;
  int		fixtile=atoi(message);

	ff = CHAR_getInt( charaindex, CHAR_FLOOR );
	fx = CHAR_getInt( charaindex, CHAR_X );
	fy = CHAR_getInt( charaindex, CHAR_Y );
	dir = CHAR_getInt( charaindex, CHAR_DIR );

	fx += CHAR_getDX(dir);
	fy += CHAR_getDY(dir);

	if( MAP_getTileAndObjData( ff, fx, fy, &tile, &obj)	== FALSE )return;

	{	
		char szBuffer[64];
		snprintf( szBuffer, sizeof( szBuffer ), "图片：%d => %d",	tile, fixtile );
		CHAR_talkToCli( charaindex, -1, szBuffer,  CHAR_COLORWHITE);
		snprintf( szBuffer, sizeof( szBuffer ), "对象：%d => %d",	obj, obj );
		CHAR_talkToCli( charaindex, -1, szBuffer,  CHAR_COLORWHITE);
	}
	
	MAP_setTileAndObjData( ff, fx, fy, fixtile, -1 );

	{
		char *stringdata;
		RECT seekr , retr;
		seekr.x = fx;
		seekr.y = fy;
		seekr.width = 1;
		seekr.height = 1;
		stringdata = MAP_getdataFromRECT(ff,&seekr,&retr);
		if( stringdata ){

			if( CHAR_getInt( charaindex , CHAR_WHICHTYPE )
			== CHAR_TYPEPLAYER ){
				int fd = getfdFromCharaIndex( charaindex );
				lssproto_M_send(fd,ff,
					retr.x,retr.y,
					retr.x+retr.width,retr.y+retr.height,
					stringdata );
			}
		} else {
		}
	}

    return ;
}

void CHAR_CHAT_Fixobj( int charaindex, char* message ){

	int 	dir, ff, fx, fy;
  int		tile, obj;
  int		fixobj=atoi(message);
  
	ff = CHAR_getInt( charaindex, CHAR_FLOOR );
	fx = CHAR_getInt( charaindex, CHAR_X );
	fy = CHAR_getInt( charaindex, CHAR_Y );
	dir = CHAR_getInt( charaindex, CHAR_DIR );

	fx += CHAR_getDX(dir);
	fy += CHAR_getDY(dir);

	if( MAP_getTileAndObjData( ff, fx, fy, &tile, &obj)	== FALSE )return;

	{	
		char szBuffer[64];
		snprintf( szBuffer, sizeof( szBuffer ), "图片：%d => %d",	tile, tile );
		CHAR_talkToCli( charaindex, -1, szBuffer,  CHAR_COLORWHITE);
		snprintf( szBuffer, sizeof( szBuffer ), "对象：%d => %d",	obj, fixobj );
		CHAR_talkToCli( charaindex, -1, szBuffer,  CHAR_COLORWHITE);
	}
	
	MAP_setTileAndObjData( ff, fx, fy, -1, fixobj );

	{
		char *stringdata;
		RECT seekr , retr;
		seekr.x = fx;
		seekr.y = fy;
		seekr.width = 1;
		seekr.height = 1;
		stringdata = MAP_getdataFromRECT(ff,&seekr,&retr);
		if( stringdata ){

			if( CHAR_getInt( charaindex , CHAR_WHICHTYPE )
			== CHAR_TYPEPLAYER ){
				int fd = getfdFromCharaIndex( charaindex );
				lssproto_M_send(fd,ff,
					retr.x,retr.y,
					retr.x+retr.width,retr.y+retr.height,
					stringdata );
			}
		} else {
		}
	}

    return ;
}

void CHAR_CHAT_Fukuwa( int charaindex, char* message ){
	int 	dir, ff, fx, fy, objindex, vs_index, vs_fd, stringlen;
    OBJECT obj;
	char szBuffer[2048];

	stringlen = strlen( message );

    if( message[0] == '[' && message[stringlen-1] == ']' ){
		CHAR_talkToCli( charaindex, -1, "语言没有转换。",  CHAR_COLORWHITE);
		return ;
    }

	ff = CHAR_getInt( charaindex, CHAR_FLOOR );
	fx = CHAR_getInt( charaindex, CHAR_X );
	fy = CHAR_getInt( charaindex, CHAR_Y );
	dir = CHAR_getInt( charaindex, CHAR_DIR );

	fx += CHAR_getDX(dir);
	fy += CHAR_getDY(dir);

	stringlen = strlen(  message  );


	snprintf( szBuffer, sizeof( szBuffer ), "P|%s", message );

    for( obj=MAP_getTopObj( ff, fx, fy ); obj; obj=NEXT_OBJECT(obj) ){
        objindex = GET_OBJINDEX(obj);
        if( OBJECT_getType(objindex) != OBJTYPE_CHARA )continue;
		vs_index = OBJECT_getIndex(objindex);
       	vs_fd = getfdFromCharaIndex( vs_index );
		CHAR_Talk( vs_fd, vs_index,
			szBuffer, CHAR_COLORWHITE, 3 );
		break;
	}

}
#endif

void CHAR_CHAT_DEBUG_superman( int charaindex, char* message )
{
#define		CHAT_CHAT_DEBUG_SUPERMAN_PARAMDEF		100000
	int i;
	char hp[15];
	char cdkey[CDKEYLEN];
	char token[128];
	int playernum = CHAR_getPlayerMaxNum();
	easyGetTokenFromString( message , 1 , hp, sizeof(hp));
	easyGetTokenFromString( message , 2 , cdkey, sizeof(cdkey));
	if(strlen(cdkey)>0){
		for(i=0;i<playernum;i++){
			if(CHAR_CHECKINDEX(i) == FALSE) continue;
			if(!strcmp( CHAR_getChar( i, CHAR_CDKEY), cdkey))
			{
				break;
			}
		}
		if (i>=playernum)
		{
		 	CHAR_talkToCli( charaindex, -1, "此账号不在线~", CHAR_COLORYELLOW );
			return;
		}
		CHAR_setInt(charaindex,CHAR_SKILLUPPOINT, 0);
		CHAR_Skillupsend(charaindex);
		CHAR_setInt( charaindex , CHAR_HP, CHAT_CHAT_DEBUG_SUPERMAN_PARAMDEF);
		CHAR_setInt( charaindex , CHAR_MP,CHAT_CHAT_DEBUG_SUPERMAN_PARAMDEF );
		CHAR_setInt( charaindex , CHAR_MAXMP,CHAT_CHAT_DEBUG_SUPERMAN_PARAMDEF );
		CHAR_setInt( charaindex , CHAR_VITAL,CHAT_CHAT_DEBUG_SUPERMAN_PARAMDEF );
		CHAR_setInt( charaindex , CHAR_STR,CHAT_CHAT_DEBUG_SUPERMAN_PARAMDEF );
		CHAR_setInt( charaindex , CHAR_TOUGH,CHAT_CHAT_DEBUG_SUPERMAN_PARAMDEF );
		CHAR_setInt( charaindex , CHAR_DEX , CHAT_CHAT_DEBUG_SUPERMAN_PARAMDEF );
#ifdef _TRANS_LEVEL_CF
		CHAR_setInt( charaindex,CHAR_LV, getMaxLevel());
#else
		CHAR_setInt( charaindex,CHAR_LV, 140);
#endif
		CHAR_complianceParameter( charaindex );
		CHAR_sendStatusString( charaindex , "P");
		
		sprintf( token, "[GM]%s把你为超人状态!", CHAR_getChar( charaindex, CHAR_NAME));
		CHAR_talkToCli( i, -1, token, CHAR_COLORYELLOW );
		sprintf( token, "玩家%s设置为超人状态!", CHAR_getChar( i, CHAR_NAME));
		CHAR_talkToCli( charaindex, -1,token, CHAR_COLORYELLOW );
	}else{
		CHAR_setInt(charaindex,CHAR_SKILLUPPOINT, 0);
		CHAR_Skillupsend(charaindex);
		CHAR_setInt( charaindex , CHAR_HP, CHAT_CHAT_DEBUG_SUPERMAN_PARAMDEF);
		CHAR_setInt( charaindex , CHAR_MP,CHAT_CHAT_DEBUG_SUPERMAN_PARAMDEF );
		CHAR_setInt( charaindex , CHAR_MAXMP,CHAT_CHAT_DEBUG_SUPERMAN_PARAMDEF );
		CHAR_setInt( charaindex , CHAR_VITAL,CHAT_CHAT_DEBUG_SUPERMAN_PARAMDEF );
		CHAR_setInt( charaindex , CHAR_STR,CHAT_CHAT_DEBUG_SUPERMAN_PARAMDEF );
		CHAR_setInt( charaindex , CHAR_TOUGH,CHAT_CHAT_DEBUG_SUPERMAN_PARAMDEF );
		CHAR_setInt( charaindex , CHAR_DEX , CHAT_CHAT_DEBUG_SUPERMAN_PARAMDEF );
#ifdef _TRANS_LEVEL_CF
		CHAR_setInt( charaindex,CHAR_LV, getMaxLevel());
#else
		CHAR_setInt( charaindex,CHAR_LV, 140);
#endif
		CHAR_complianceParameter( charaindex );
		CHAR_sendStatusString( charaindex , "P");
		
		CHAR_talkToCli( charaindex, -1,"设置超人状态!", CHAR_COLORYELLOW );
	}
#undef	CHAT_CHAT_DEBUG_SUPERMAN_PARAMDEF

}

void CHAR_CHAT_printcount( int charaindex, char* message )
{
	char	msgbuf[512];

	snprintf( msgbuf,sizeof( msgbuf),
			  "你说了 %d 次话,走了 %d 步路,死了 %d 次",
			  CHAR_getInt( charaindex, CHAR_TALKCOUNT),
			  CHAR_getInt( charaindex, CHAR_WALKCOUNT),
			  CHAR_getInt( charaindex, CHAR_DEADCOUNT)
			  );
	CHAR_talkToCli( charaindex, -1, msgbuf,  CHAR_COLORWHITE);

}

void CHAR_CHAT_DEBUG_battlein( int charaindex, char* message )
{
	int fd = getfdFromCharaIndex( charaindex );
	if( fd != -1) {
		lssproto_EN_recv( fd,
			CHAR_getInt( charaindex, CHAR_X),
			CHAR_getInt( charaindex, CHAR_Y));
	}
}

void CHAR_CHAT_DEBUG_battleout( int charaindex, char* message )
{

	BATTLE_WatchStop( charaindex );
}

void CHAR_CHAT_DEBUG_petmake( int charaindex, char* message )
{
	int	ret;
	int	enemynum;
	int	enemyid;
	int	i,j;
	int level=-1;
	char buf[10];
	char cdkey[CDKEYLEN];
	char token[128];
	int playernum = CHAR_getPlayerMaxNum();
	easyGetTokenFromString( message , 1 , buf, sizeof(buf));
	enemyid=atoi(buf);
	easyGetTokenFromString( message , 2 , buf, sizeof(buf));
	level=atoi(buf);
	easyGetTokenFromString( message , 3 , cdkey, sizeof(cdkey));
	if(strlen(cdkey)>0){
		for(j=0;j<playernum;j++){
			if(CHAR_CHECKINDEX(j) == FALSE) continue;
			if(!strcmp( CHAR_getChar( j, CHAR_CDKEY), cdkey))
			{
				break;
			}
		}
		if (j>=playernum)
		{
		 	CHAR_talkToCli( charaindex, -1, "此账号不在线~", CHAR_COLORYELLOW );
			return;
		}
		enemynum = ENEMY_getEnemyNum();
		for( i = 0; i < enemynum; i ++ ) {
			if( ENEMY_getInt( i, ENEMY_ID) == enemyid) {
				break;
			}
		}
		if( i == enemynum )return;
	
		ret = ENEMY_createPetFromEnemyIndex( j, i);
		
		if( !CHAR_CHECKINDEX( ret))return;
		
		sprintf( token, "[GM]%s制作了%s给你!", CHAR_getChar( charaindex, CHAR_NAME),ENEMY_getChar( i, ENEMY_NAME));
		CHAR_talkToCli( j, -1, token, CHAR_COLORYELLOW );
		sprintf( token, "给玩家%s的%s制作成功!", CHAR_getChar( j, CHAR_NAME),ENEMY_getChar( i, ENEMY_NAME));
		CHAR_talkToCli( charaindex, -1,token, CHAR_COLORYELLOW );
		for( i = 0; i < CHAR_MAXPETHAVE; i ++ ){
			if( CHAR_getCharPet( j, i ) == ret )break;
		}
		if( i == CHAR_MAXPETHAVE )return;
	
		if( CHAR_CHECKINDEX( ret ) == TRUE ){
			CHAR_setMaxExpFromLevel( ret, CHAR_getInt( ret, CHAR_LV ));
			CHAR_setInt(ret,CHAR_VARIABLEAI,10000);
		}
		if( level != -1 ){
#ifdef _TRANS_LEVEL_CF
			if( level > getMaxLevel() ) level = getMaxLevel();
#else
			if( level > 140 ) level = 140;
#endif
			if( level < 0 ) level = 0;
			level = level - CHAR_getInt( ret, CHAR_LV );
			if( level > 0 ){
				int k;
				for( k = 1; k <= level; k ++ ){	//升级
					CHAR_PetLevelUp( ret );
					CHAR_PetAddVariableAi( ret, AI_FIX_PETLEVELUP );
					CHAR_setInt( ret, CHAR_LV, CHAR_getInt( ret, CHAR_LV) +1 );
				}
			}
		}
		CHAR_setInt( ret, CHAR_HP, CHAR_getWorkInt( ret, CHAR_WORKMAXHP ));
		CHAR_complianceParameter( ret );
		snprintf( token, sizeof( token ), "K%d", i );
		CHAR_sendStatusString( j, token );
		snprintf( token, sizeof( token ), "W%d", i );
		CHAR_sendStatusString( j, token );
	}else{
		enemynum = ENEMY_getEnemyNum();
		for( i = 0; i < enemynum; i ++ ) {
			if( ENEMY_getInt( i, ENEMY_ID) == enemyid) {
				break;
			}
		}
		if( i == enemynum )return;
	
		ret = ENEMY_createPetFromEnemyIndex(charaindex, i);
		if( !CHAR_CHECKINDEX( ret))return;
		
		snprintf( token,sizeof( token), "%s制作成功!",ENEMY_getChar( i, ENEMY_NAME));
		CHAR_talkToCli( charaindex, -1, token,  CHAR_COLORWHITE);
	
		for( i = 0; i < CHAR_MAXPETHAVE; i ++ ){
			if( CHAR_getCharPet( charaindex, i ) == ret )break;
		}
		if( i == CHAR_MAXPETHAVE )return;
	
		if( CHAR_CHECKINDEX( ret ) == TRUE ){
			CHAR_setMaxExpFromLevel( ret, CHAR_getInt( ret, CHAR_LV ));
			CHAR_setInt(ret,CHAR_VARIABLEAI,10000);
		}
		if( level != -1 ){
			if( level > 200 ) level = 200;
			if( level < 0 ) level = 0;
			level = level - CHAR_getInt( ret, CHAR_LV );
				if( level > 0 ){
				int k;
				for( k = 1; k <= level; k ++ ){	//升级
					CHAR_PetLevelUp( ret );
					CHAR_PetAddVariableAi( ret, AI_FIX_PETLEVELUP );
					CHAR_setInt( ret, CHAR_LV, CHAR_getInt( ret, CHAR_LV) +1 );
				}
			}
		}
		CHAR_setInt( ret, CHAR_HP, CHAR_getWorkInt( ret, CHAR_WORKMAXHP ));
		CHAR_complianceParameter( ret );
		snprintf( token, sizeof( token ), "K%d", i );
		CHAR_sendStatusString( charaindex, token );
		snprintf( token, sizeof( token ), "W%d", i );
		CHAR_sendStatusString( charaindex, token );
	}
}

void CHAR_CHAT_DEBUG_deletepet( int charaindex, char* message )
{
	int i, pindex;
	char category[12];
  
	for( i = 0; i < CHAR_MAXPETHAVE; i ++ ) {
		pindex = CHAR_getCharPet( charaindex, i);
		if( CHAR_CHECKINDEX( pindex)) {
			CHAR_endCharOneArray( pindex );
			CHAR_setCharPet( charaindex, i, -1);
			snprintf( category, sizeof( category), "K%d",i);
			CHAR_sendStatusString( charaindex, category );
			snprintf( category, sizeof( category), "W%d",i);
			CHAR_sendStatusString( charaindex, category );
		}
	}
	
	CHAR_talkToCli( charaindex, -1, "清除身上所有宠物。",  CHAR_COLORWHITE);

}

#define MAXMESS_LISTNUM 7
#define ALLRESET (MAXMESS_LISTNUM - 1)

void CHAR_CHAT_DEBUG_reset( int charaindex, char* message )
{
	int i;
	char mess[MAXMESS_LISTNUM][256]={ "enemy", "encount", "magic", "warppoint", "petskill", "pettalk", "all"};

	for( i=0; i<MAXMESS_LISTNUM; i++){
		if( !strcmp(  mess[i], message) )
			break;
	}
	if( i >= MAXMESS_LISTNUM ){
		CHAR_talkToCli( charaindex, -1, "指令错误",  CHAR_COLORRED);//CHAR_COLORYELLOW
		return;
	}
	switch( i){
	case ALLRESET:
	case 0:
		if( !ENEMYTEMP_reinitEnemy() || !ENEMY_reinitEnemy() ){
			CHAR_talkToCli( charaindex, -1, "重新读取敌人基本资料失败。",  CHAR_COLORRED);
			return;
		}
		if( i != ALLRESET ) break;
	case 1:
		if( !GROUP_reinitGroup() || !ENCOUNT_reinitEncount() ){
			CHAR_talkToCli( charaindex, -1, "重新读取遭遇团队资料失败。",  CHAR_COLORRED);
		}
		if( i != ALLRESET ) break;
	case 2:
		if( !MAGIC_reinitMagic() || !ATTMAGIC_reinitMagic() ) {
			CHAR_talkToCli( charaindex, -1, "重新读取咒术资料失败。",  CHAR_COLORRED);
		}
		if( i != ALLRESET ) break;
	case 3:
		MAPPOINT_resetMapWarpPoint( 1);
		if( !MAPPOINT_loadMapWarpPoint() ){
			CHAR_talkToCli( charaindex, -1, "重新读取跳跃点资料失败。",  CHAR_COLORRED);
		}
		if( i != ALLRESET ) break;
	case 4:
		if( !PETSKILL_reinitPetskill() ) {
			CHAR_talkToCli( charaindex, -1, "重新读取宠物技能资料失败。",  CHAR_COLORRED);
		}
		if( i != ALLRESET ) break;
	case 5:
		LoadPetTalk();
		if( i != ALLRESET ) break;
		break;
	}
	CHAR_talkToCli( charaindex, -1, "重新读取游戏资料。",  CHAR_COLORYELLOW);
}

void CHAR_CHAT_DEBUG_enemyrestart( int charaindex, char* message )
{
	if( ENEMYTEMP_reinitEnemy()) {
		CHAR_talkToCli( charaindex, -1, "再次读取敌人基本资料。",  CHAR_COLORWHITE);
	}else {
		CHAR_talkToCli( charaindex, -1, "再次读取敌人基本资料失败。",  CHAR_COLORWHITE);
	}

	if( ENEMY_reinitEnemy()) {
		CHAR_talkToCli( charaindex, -1, "再次读取敌人资料。",  CHAR_COLORWHITE);
	}else {
		CHAR_talkToCli( charaindex, -1, "再次读取敌人基本资料失败。",  CHAR_COLORWHITE);
	}

	if( GROUP_reinitGroup()) {
		CHAR_talkToCli( charaindex, -1, "再次读取团队资料。",  CHAR_COLORWHITE);
	}else {
		CHAR_talkToCli( charaindex, -1, "再次读取团队资料失败。",  CHAR_COLORWHITE);
	}
	if( ENCOUNT_reinitEncount()) {
		CHAR_talkToCli( charaindex, -1, "再次读取遭遇资料。",  CHAR_COLORWHITE);
	}
	else {
		CHAR_talkToCli( charaindex, -1, "再次读取遭遇资料失败。",  CHAR_COLORWHITE);
	}

	if( MAGIC_reinitMagic()) {
		CHAR_talkToCli( charaindex, -1, "再次读取咒术资料。",  CHAR_COLORWHITE);
	}
	else {
		CHAR_talkToCli( charaindex, -1, "再次读取咒术资料失败。",  CHAR_COLORWHITE);
	}

#ifdef _ATTACK_MAGIC
  if( ATTMAGIC_reinitMagic() )
    CHAR_talkToCli( charaindex , -1 , "再次读取咒术资料。" , CHAR_COLORWHITE );
  else
    CHAR_talkToCli( charaindex , -1 , "再次读取咒术资料失败。" , CHAR_COLORWHITE );
#endif
	if( PETSKILL_reinitPetskill()) {
		CHAR_talkToCli( charaindex, -1, "再次读取宠物技能资料。",  CHAR_COLORWHITE);
	}
	else {
		CHAR_talkToCli( charaindex, -1, "再次读取宠物技能资料失败。",  CHAR_COLORWHITE);
	}
	
	LoadPetTalk();	// Arminius 8.15 pet talk
	CHAR_talkToCli( charaindex, -1, "再次读取宠物讲话资料。", CHAR_COLORWHITE);

}

void CHAR_CHAT_DEBUG_battlewatch( int charaindex, char* message )
{
	int fd = getfdFromCharaIndex( charaindex );
	if( fd != -1) {
		lssproto_LB_recv( fd,
			CHAR_getInt( charaindex, CHAR_X),
			CHAR_getInt( charaindex, CHAR_Y));
	}
}

// shan 2001/12/18 Begin   由於改变过多，故将原先的function注掉
void CHAR_CHAT_DEBUG_eventclean( int charaindex, char* message )
{	
	int event_num = 6;
	int point;
	int shiftbit;
	int eventno=0;
	char token[1024];
	int array;
	int shift;
	char token_cdkey[256]="", token_name[256]="", cdkey[CDKEYLEN], token_flag[256]="";
	unsigned int max_user = 0;
	BOOL find = FALSE;
	BOOL ret1, ret2;
	int  charaindex_tmp = 0;
	int i;
     
    getStringFromIndexWithDelim( message, " ", 1, token_flag, sizeof( token));
	ret1 = getStringFromIndexWithDelim( message, " ", 2, token_cdkey, sizeof( token_cdkey));
    ret2 = getStringFromIndexWithDelim( message, " ", 3, token_name, sizeof( token_name));
	shiftbit = atoi(token_flag);
	eventno = atoi(token_flag);

	if( shiftbit!=-1 && ret1==FALSE && ret2==FALSE ){
		charaindex_tmp = charaindex;
		find=TRUE;
	}else{
		if( shiftbit==-1 || strlen(token_cdkey)==0 || strlen(token_name)==0 ){
			sprintf( token, "%s", "参数不正确。正确格式：[eventsetend 任务旗标 帐号 人物名称]");
			CHAR_talkToCli( charaindex, -1, token, CHAR_COLORWHITE);
			return; 
		}
		max_user = getFdnum();
		for( i=0; i<max_user; i++){
			char szName[256];
			int i_use;			

			i_use = CONNECT_getUse( i);
			if( i_use){
				CONNECT_getCdkey( i, cdkey, sizeof( cdkey));
				CONNECT_getCharname( i, szName, sizeof( szName));
				if( strcmp( cdkey, token_cdkey)==0 && strcmp( szName, token_name)==0){
					charaindex_tmp = CONNECT_getCharaindex( i);					
					find=TRUE;
				}
			}
		}
	}
	
	if( strcmp(token_flag, "all")==0 && find ){
		CHAR_setInt( charaindex_tmp, CHAR_ENDEVENT, 0);
		CHAR_setInt( charaindex_tmp, CHAR_ENDEVENT2, 0);
		CHAR_setInt( charaindex_tmp, CHAR_ENDEVENT3, 0);
#ifdef _NEWEVENT
		CHAR_setInt( charaindex_tmp, CHAR_ENDEVENT4, 0);
		CHAR_setInt( charaindex_tmp, CHAR_ENDEVENT5, 0);
		CHAR_setInt( charaindex_tmp, CHAR_ENDEVENT6, 0);
#endif
		CHAR_setInt( charaindex_tmp, CHAR_NOWEVENT, 0);
		CHAR_setInt( charaindex_tmp, CHAR_NOWEVENT2, 0);
		CHAR_setInt( charaindex_tmp, CHAR_NOWEVENT3, 0);
#ifdef _NEWEVENT
		CHAR_setInt( charaindex_tmp, CHAR_NOWEVENT4, 0);
		CHAR_setInt( charaindex_tmp, CHAR_NOWEVENT5, 0);
		CHAR_setInt( charaindex_tmp, CHAR_NOWEVENT6, 0);
#endif
		sprintf( token, "削除全部的事件旗。" );
		CHAR_talkToCli( charaindex, -1,token, CHAR_COLORWHITE);
		return ;
	}

	if(find){
		array = shiftbit / 32;
		shift = shiftbit % 32;
		
		if( array>=event_num ){
			sprintf( token, "错误！！你所设的任务旗标编号已超过  围(0~%d)。",32*event_num-1);
			CHAR_talkToCli( charaindex, -1, token, CHAR_COLORWHITE);
			return;
		}
		point = CHAR_getInt( charaindex_tmp, CHAR_ENDEVENT+array);
        point = point & ~(1<<shift);
		CHAR_setInt( charaindex_tmp, CHAR_ENDEVENT+array, point);
		point = CHAR_getInt( charaindex_tmp, CHAR_NOWEVENT+array);	
		point = point & ~(1<<shift);	
		CHAR_setInt( charaindex_tmp, CHAR_NOWEVENT+array, point);
   
		sprintf( token, "Success!! Character Name:%s delete eventflag:[%d].", CHAR_getChar( charaindex_tmp, CHAR_NAME), eventno);
		CHAR_talkToCli( charaindex, -1, token, CHAR_COLORWHITE);
		// display
		if( charaindex_tmp==charaindex )
			sprintf( token, "%s", "");
		else
			sprintf( token, "%s %s", token_cdkey, token_name);
		//CHAR_CHAT_DEBUG_watchevent( charaindex, token );
	}else{
		sprintf( token,"%s\n","失败！！帐号与人物名称无法相符。");
		CHAR_talkToCli(charaindex, -1, token, CHAR_COLORWHITE);
	}	
}

// shan 2001/12/15 End

// shan 2001/12/18 Begin   由於改变过多，故将原先的function注掉

void CHAR_CHAT_DEBUG_eventsetnow( int charaindex, char* message )
{
	int event_num = 6;

	int point;
	int shiftbit;
	int eventno=0;
	char token[1024];
	int array;
	int shift;
	char token_cdkey[256]="", token_name[256]="", cdkey[CDKEYLEN];
	unsigned int max_user = 0;
	BOOL find = FALSE;
	BOOL ret1, ret2;
	int  charaindex_tmp = 0;
	int i;
     
    getStringFromIndexWithDelim( message, " ", 1, token, sizeof( token));
	ret1 = getStringFromIndexWithDelim( message, " ", 2, token_cdkey, sizeof( token_cdkey));
    ret2 = getStringFromIndexWithDelim( message, " ", 3, token_name, sizeof( token_name));
	shiftbit = atoi(token);
	eventno = atoi(token);

	if( shiftbit!=-1 && ret1==FALSE && ret2==FALSE ){
		charaindex_tmp = charaindex;
		find=TRUE;
	}
	else{
		if( shiftbit==-1 || strlen(token_cdkey)==0 || strlen(token_name)==0 ){
			sprintf( token, "%s", "参数不正确。正确格式：[eventsetend 任务旗标 帐号 人物名称]");
			CHAR_talkToCli( charaindex, -1, token, CHAR_COLORWHITE);
			return; 
		}
		max_user = getFdnum();
		for( i=0; i<max_user; i++){
			char szName[256];
			int i_use;			

			i_use = CONNECT_getUse( i);
			if( i_use){
				CONNECT_getCdkey( i, cdkey, sizeof( cdkey));
				CONNECT_getCharname( i, szName, sizeof( szName));
				if( strcmp( cdkey, token_cdkey)==0 && strcmp( szName, token_name)==0){
					charaindex_tmp = CONNECT_getCharaindex( i);					
					find=TRUE;
				}
			}
		}
	}

	if( strcmp(token, "all")==0 && find ){
		CHAR_setInt( charaindex_tmp, CHAR_NOWEVENT, -1);
		CHAR_setInt( charaindex_tmp, CHAR_NOWEVENT2, -1);
		CHAR_setInt( charaindex_tmp, CHAR_NOWEVENT3, -1);
#ifdef _NEWEVENT
		CHAR_setInt( charaindex_tmp, CHAR_NOWEVENT4, -1);
		CHAR_setInt( charaindex_tmp, CHAR_NOWEVENT5, -1);
		CHAR_setInt( charaindex_tmp, CHAR_NOWEVENT6, -1);
#endif
		sprintf( token, "Success, set all nowflag.." );
		CHAR_talkToCli( charaindex, -1,token, CHAR_COLORWHITE);
		return ;
	}

	if(find){
		array = shiftbit / 32;
		shift = shiftbit % 32;
		
		if( array>=event_num ){
			sprintf( token, "错误！！你所设的任务旗标编号已超过范围(0~%d)。",32*event_num-1);
			CHAR_talkToCli( charaindex, -1, token, CHAR_COLORWHITE);
			return;
		}		
		point = CHAR_getInt( charaindex_tmp, CHAR_NOWEVENT+array);
		point = point | (1<<shift);
		CHAR_setInt( charaindex_tmp, CHAR_NOWEVENT+array, point);
   
		sprintf( token, "Success!! Character Name:%s eventnow_number:[%d].", CHAR_getChar( charaindex_tmp, CHAR_NAME), eventno);
		CHAR_talkToCli( charaindex, -1, token, CHAR_COLORWHITE);
		// display
		if( charaindex_tmp==charaindex )
			sprintf( token, "%s", "");
		else
			sprintf( token, "%s %s", token_cdkey, token_name);
		CHAR_CHAT_DEBUG_watchevent( charaindex, token );
	}else{
		sprintf( token,"%s\n","失败！！帐号与人物名称无法相符。");
		CHAR_talkToCli(charaindex, -1, token, CHAR_COLORWHITE);
	}
}
/*void CHAR_CHAT_DEBUG_eventsetnow( int charaindex, char* message )
{
	int point;
	int shiftbit;
	int eventno=0;
	char token[256];
	int array;
	int shift;

	shiftbit=atoi(message);
	eventno=atoi(message);

	// -1反白仿弘卞楮溢互卅中奶矛件玄   
	if(shiftbit==-1) return;

	array = shiftbit / 32;
	shift = shiftbit % 32;

	point = CHAR_getInt(charaindex,CHAR_NOWEVENT+array);
	point = point | (1 << shift);
	CHAR_setInt(charaindex,CHAR_NOWEVENT+array,point);

	sprintf( token, "设定事件中%d号旗。",eventno);
	CHAR_talkToCli( charaindex, -1,token, CHAR_COLORWHITE);
}*/
// shan 2001/12/18 End

// shan 2001/12/15 Begin   由於改变过多，故将原先的function注掉

void CHAR_CHAT_DEBUG_eventsetend( int charaindex, char* message )
{
 	int event_num = 6;
	int point;
	int shiftbit;
	int eventno = 0;
	char token[1024];
	int array;
	int shift;
	char token_cdkey[256]="", token_name[256]="", cdkey[CDKEYLEN];
	unsigned int max_user = 0;
	BOOL find = FALSE;
	BOOL ret1, ret2;
	int  charaindex_tmp = 0;
	int i;
	
	getStringFromIndexWithDelim( message, " ", 1, token, sizeof( token));
	ret1 = getStringFromIndexWithDelim( message, " ", 2, token_cdkey, sizeof( token_cdkey));
    ret2 = getStringFromIndexWithDelim( message, " ", 3, token_name, sizeof( token_name));
	shiftbit = atoi(token);
	eventno = atoi(token);

    if( shiftbit!=-1 && ret1==FALSE && ret2==FALSE ){
		charaindex_tmp = charaindex;
		find=TRUE;
	}
	else{
		if( shiftbit==-1 || strlen(token_cdkey)==0 || strlen(token_name)==0 ){
			sprintf( token, "%s", "参数不正确。正确格式：[eventsetend 任务旗标 帐号 人物名称]");
			CHAR_talkToCli( charaindex, -1, token, CHAR_COLORWHITE);
			return; 
		}
		max_user = getFdnum();
		for( i=0; i<max_user; i++){
			char szName[256];
			int i_use;			

			i_use = CONNECT_getUse( i);
			if( i_use){
				CONNECT_getCdkey( i, cdkey, sizeof( cdkey));
				CONNECT_getCharname( i, szName, sizeof( szName));
				if( strcmp( cdkey, token_cdkey)==0 && strcmp( szName, token_name)==0){
					charaindex_tmp = CONNECT_getCharaindex( i);					
					find=TRUE;
				}
			}
		}
	}

	if( strcmp(token, "all")==0 && find ){
		CHAR_setInt( charaindex_tmp, CHAR_ENDEVENT, -1);
		CHAR_setInt( charaindex_tmp, CHAR_ENDEVENT2, -1);
		CHAR_setInt( charaindex_tmp, CHAR_ENDEVENT3, -1);
#ifdef _NEWEVENT
		CHAR_setInt( charaindex_tmp, CHAR_ENDEVENT4, -1);
		CHAR_setInt( charaindex_tmp, CHAR_ENDEVENT5, -1);
		CHAR_setInt( charaindex_tmp, CHAR_ENDEVENT6, -1);
#endif
		sprintf( token, "Success, set all endflag.." );
		CHAR_talkToCli( charaindex, -1,token, CHAR_COLORWHITE);
		return ;
	}

	if(find){
		array = shiftbit / 32;
		shift = shiftbit % 32;
		
		if( array>=event_num ){
			sprintf( token, "错误！！你所设的任务旗标编号已超过  围(0~%d)。",32*event_num-1);
			CHAR_talkToCli( charaindex, -1, token, CHAR_COLORWHITE);
			return;
		}
		point = CHAR_getInt( charaindex_tmp, CHAR_ENDEVENT+array);
		point = point | (1<<shift);
		CHAR_setInt( charaindex_tmp, CHAR_ENDEVENT+array, point);
   
		sprintf( token, "Success!! Character Name:%s eventend_number:[%d].", CHAR_getChar( charaindex_tmp, CHAR_NAME), eventno);
		CHAR_talkToCli( charaindex, -1, token, CHAR_COLORYELLOW);
        // display
		if( charaindex_tmp==charaindex )
			sprintf( token, "%s", "");
		else
			sprintf( token, "%s %s", token_cdkey, token_name);
		CHAR_CHAT_DEBUG_watchevent( charaindex, token );
	}else{
		sprintf( token,"%s\n","失败！！帐号与人物名称无法相符。");
		CHAR_talkToCli(charaindex, -1, token, CHAR_COLORYELLOW);
	}

}

void CHAR_CHAT_DEBUG_debug( int charaindex, char* message )
{
	char	msgbuf[256];
	BOOL	flg = isstring1or0( message);
	if( flg == TRUE ){
		CHAR_setWorkInt( charaindex, CHAR_WORKFLG,
			CHAR_getWorkInt( charaindex, CHAR_WORKFLG ) | WORKFLG_DEBUGMODE );
		CHAR_setWorkInt( charaindex, CHAR_WORKDEBUGMODE, TRUE);
	}else{
		CHAR_setWorkInt( charaindex, CHAR_WORKFLG,
			CHAR_getWorkInt( charaindex, CHAR_WORKFLG ) & ~WORKFLG_DEBUGMODE );
		CHAR_setWorkInt( charaindex, CHAR_WORKDEBUGMODE, FALSE);
	}
	CHAR_send_P_StatusString( charaindex, CHAR_P_STRING_DEBUGMODE);
	snprintf( msgbuf, sizeof( msgbuf), "已设定除错模式为%s。",
		( flg == TRUE) ? "ON" : "OFF");

	CHAR_talkToCli( charaindex, -1, msgbuf, CHAR_COLORYELLOW);

}

void CHAR_CHAT_DEBUG_dp( int charindex , char *message )
{
	int i;
	char dp[15];
	char cdkey[CDKEYLEN];
	char token[128];
	int playernum = CHAR_getPlayerMaxNum();
	easyGetTokenFromString( message , 1 , dp, sizeof(dp));
	easyGetTokenFromString( message , 2 , cdkey, sizeof(cdkey));
	if(strlen(cdkey)>0){
		for(i=0;i<playernum;i++){
			if(CHAR_CHECKINDEX(i) == FALSE) continue;
			if(!strcmp( CHAR_getChar( i, CHAR_CDKEY), cdkey))
			{
				break;
			}
		}
		if (i>=playernum)
		{
		 	CHAR_talkToCli( charindex, -1, "此账号不在线~", CHAR_COLORYELLOW );
			return;
		}
		CHAR_setInt( i , CHAR_DUELPOINT, atoi( dp ) );
		CHAR_send_P_StatusString( i ,CHAR_P_STRING_DUELPOINT );
		sprintf( token, "[GM]%s把你的DP设置为%d!", CHAR_getChar( charindex, CHAR_NAME),(int)atoi(dp));
		CHAR_talkToCli( i, -1, token, CHAR_COLORYELLOW );
		sprintf( token, "玩家%s的DP设置为%d!", CHAR_getChar( i, CHAR_NAME),(int)atoi(dp));
		CHAR_talkToCli( charindex, -1,token, CHAR_COLORYELLOW );
	}else{
		CHAR_setInt( charindex , CHAR_DUELPOINT, atoi( dp ) );
		CHAR_send_P_StatusString( charindex ,CHAR_P_STRING_DUELPOINT );
		sprintf( token, "DP设置为%d!",(int)atoi(dp));
		CHAR_talkToCli( charindex, -1,token, CHAR_COLORYELLOW );
	}
}

#ifdef _EQUIT_SEQUENCE
void CHAR_CHAT_DEBUG_sequence( int charindex , char *message )
{
	CHAR_setWorkInt( charindex, CHAR_WORKFIXSEQUENCE, atoi( message) );
	CHAR_complianceParameter( charindex );
	CHAR_talkToCli( charindex, -1, "设定顺序", CHAR_COLORYELLOW);
}
#endif

#define ITEMRESERVESTRING   "item"
#define POOLITEMRESERVESTRING   "poolitem"

void CHAR_CHAT_DEBUG_setmerge_main( int charaindex, char* message, int mode )
{
	int haveitemindex = atoi( message);
	int itemindex;
	
	if( !CHAR_CHECKITEMINDEX( charaindex, haveitemindex) ){
		CHAR_talkToCli( charaindex, -1, "号码很奇怪。", CHAR_COLORWHITE);
		return;
	}
	itemindex = CHAR_getItemIndex( charaindex, haveitemindex);
	if( !ITEM_CHECKINDEX( itemindex)) {
		CHAR_talkToCli( charaindex, -1, "该处是否没有任何道具,情报亦很奇怪。", CHAR_COLORWHITE);
		return;
	}
	ITEM_setInt( itemindex, ITEM_MERGEFLG, mode);
	
	CHAR_talkToCli( charaindex, -1, "宁岳白仿弘及丑综毛仄引仄凶［", CHAR_COLORWHITE);
	
	CHAR_sendItemDataOne( charaindex, haveitemindex);
}

void CHAR_CHAT_DEBUG_effect( int charaindex, char* message )
{
	int		floorid=-1, effectid=-1, level=0;
	char	buf[ 256]; 
	int		i;
    int     playernum = CHAR_getPlayerMaxNum();
	BOOL	flg = FALSE;
	
	getStringFromIndexWithDelim( message, " ", 1, buf, sizeof( buf));
	if( strcmp( buf, "alloff" ) == 0 ) {
		flg = TRUE;
	}else {
		floorid = atoi( buf);
		if( !getStringFromIndexWithDelim( message, " ", 2, buf, sizeof( buf))) {
			return;
		}
		effectid = atoi( buf);
		if( !getStringFromIndexWithDelim( message, " ", 3, buf, sizeof( buf))) {
			return;
		}
		level = atoi( buf);
	}
	for( i = 0 ; i < playernum; i ++) {
		if( CHAR_CHECKINDEX( i)) {
			if( CHAR_getInt( i, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER){
				if( flg) {
					int ef = CHAR_getWorkInt( i, CHAR_WORKEFFECT);
					if( ef != 0 ) {
						int fd = getfdFromCharaIndex( i);
						lssproto_EF_send( fd, 0, 0, "");
						CHAR_setWorkInt( i, CHAR_WORKEFFECT, 0);
					}
				}
				else if( CHAR_getInt( i, CHAR_FLOOR) == floorid ) {
					int fd = getfdFromCharaIndex( i);
					lssproto_EF_send( fd, effectid, level, "");
					if( level == 0 ) {
						CHAR_setWorkInt( i, CHAR_WORKEFFECT, 
										CHAR_getWorkInt( i, CHAR_WORKEFFECT) &
											~effectid);
					}
					else {
						CHAR_setWorkInt( i, CHAR_WORKEFFECT, 
										CHAR_getWorkInt( i, CHAR_WORKEFFECT) | 
											effectid);
					}
				}
			}
		}
	}
}

void CHAR_CHAT_DEBUG_setTrans( int charaindex, char *message)
{
	int i;
	char setTrans[15];
	char cdkey[CDKEYLEN];
	char token[128];
	int playernum = CHAR_getPlayerMaxNum();
	easyGetTokenFromString( message , 1 , setTrans, sizeof(setTrans));
	easyGetTokenFromString( message , 2 , cdkey, sizeof(cdkey));
	if(strlen(cdkey)>0){
		for(i=0;i<playernum;i++){
			if(CHAR_CHECKINDEX(i) == FALSE) continue;
			if(!strcmp( CHAR_getChar( i, CHAR_CDKEY), cdkey))
			{
				break;
			}
		}
		if (i>=playernum)
		{
		 	CHAR_talkToCli( charaindex, -1, "此账号不在线~", CHAR_COLORYELLOW );
			return;
		}
		CHAR_setInt(i, CHAR_TRANSEQUATION, 0);
		CHAR_setInt(i, CHAR_TRANSMIGRATION, atoi( setTrans ));
		CHAR_sendCToArroundCharacter( CHAR_getWorkInt( i, CHAR_WORKOBJINDEX ));
		sprintf( token, "[GM]%s把你转生设置为%d转!", CHAR_getChar( charaindex, CHAR_NAME),(int)atoi(setTrans));
		CHAR_talkToCli( i, -1, token, CHAR_COLORYELLOW );
		sprintf( token, "玩家%s转生设置为%d转!", CHAR_getChar( i, CHAR_NAME),(int)atoi(setTrans));
		CHAR_talkToCli( charaindex, -1,token, CHAR_COLORYELLOW );
	}else{
		CHAR_setInt(charaindex, CHAR_TRANSEQUATION, 0);
		CHAR_setInt(charaindex, CHAR_TRANSMIGRATION, atoi( setTrans ));
		CHAR_sendCToArroundCharacter( CHAR_getWorkInt( charaindex, CHAR_WORKOBJINDEX ));
		sprintf( token, "转生设置为%d转!",(int)atoi(setTrans));
		CHAR_talkToCli( charaindex, -1,token, CHAR_COLORYELLOW );
	}
}

void CHAR_CHAT_DEBUG_shutup(int charindex ,char *message)
{
     char token[100];
     char token1[100];
     char cdkey[CDKEYLEN];
     char buf[256];
     char line[256];
     unsigned int MAX_USER=0,i;
     BOOL flg;
     easyGetTokenFromString( message , 1 , token, sizeof(token));
     easyGetTokenFromString( message , 2 , token1, sizeof(token1));
     if( strlen(message)==0){
         sprintf(buf,"%s","参数不正确");
         CHAR_talkToCli( charindex, -1,buf, CHAR_COLORYELLOW);
         return;
     }
     if( strlen(token1)==0){
         sprintf(buf,"%s","参数不正确--> ON OFF");
         CHAR_talkToCli( charindex, -1,buf, CHAR_COLORYELLOW);
         return;
     }
     flg = isstring1or0( token1);
     MAX_USER=getFdnum();
     for(i=0;i<MAX_USER;i++){
         int i_use;
         i_use=CONNECT_getUse(i);
         if(i_use){
            CONNECT_getCdkey( i, cdkey, sizeof( cdkey ));
            if(strcmp(token,cdkey)==0){
               int index = CONNECT_getCharaindex( i );
               if(flg){
                  CONNECT_set_shutup(i,TRUE);
                  //print("\n<Set TRUE");
                  snprintf( line, sizeof( line),
                            "将 name:%s account=%s封嘴",
                            CHAR_getChar( index, CHAR_NAME),
                            cdkey);
                  CHAR_talkToCli( charindex, -1,line, CHAR_COLORYELLOW);
               }else{
                  CONNECT_set_shutup(i,FALSE);
                  //print("\n<Set FALSE");
                  snprintf( line, sizeof( line),
                            "将 name:%s account=%s开口",
                            CHAR_getChar( index, CHAR_NAME),
                            cdkey);
                  CHAR_talkToCli( charindex, -1,line, CHAR_COLORYELLOW);
               }
             }
          }//if use
       }//for
}

void CHAR_CHAT_DEBUG_getuser(int charindex ,char *message) //ttom 14/11/2000
{	//  ..NAME|FLOOR|
	char buf1[256], name[256], token[256];
	int i, k, MyFloor=-1, colors = CHAR_COLORWHITE;
	int user[50];
	int MAXCHARA=0;
	BOOL FINDNPC = FALSE;
	BOOL MOREs = FALSE;

	memset( name, 0, sizeof( name) );
	MAXCHARA = CHAR_getPlayerMaxNum();
	if( getStringFromIndexWithDelim( message, " ", 1, name, sizeof( name)) == FALSE )	return;
	if( getStringFromIndexWithDelim( message, " ", 2, buf1, sizeof( buf1)) != FALSE ){
		MyFloor = atoi( buf1);
	}
	if( getStringFromIndexWithDelim( message, " ", 3, buf1, sizeof( buf1)) != FALSE ){
		if( strstr( buf1, "npc") != NULL ){
			FINDNPC = TRUE;
			MAXCHARA = CHAR_getCharNum();
		}
	}
	k = 0;
	for( i=0; i<MAXCHARA && k<50; i++ ){
		if( !CHAR_CHECKINDEX( i) ) continue;
		if( FINDNPC == TRUE ){
			if( CHAR_getInt( i, CHAR_WHICHTYPE ) == CHAR_TYPEPLAYER  ||
				CHAR_getInt( i, CHAR_WHICHTYPE ) == CHAR_TYPEPET ) continue;
		}
		if( MyFloor != -1 && MyFloor != CHAR_getInt( i, CHAR_FLOOR) ) continue;

		if( strstr( CHAR_getChar( i, CHAR_OWNTITLE), name) != NULL ||
			strstr( CHAR_getChar( i, CHAR_NAME), name) != NULL ){
			user[k++] = i;
			if( k >= 10 )MOREs = TRUE;
		}
	}

	memset( token, 0, sizeof(token));
	strcpy( token, "MORE:");
	for( i=0; i<k; i++){//CHAR_COLORYELLOW
		int userindex = user[i];
		if( !CHAR_CHECKINDEX( userindex)) continue;
		if( !strcmp( CHAR_getUseName( userindex), name ) ||
			!strcmp( CHAR_getChar( userindex, CHAR_NAME), name ) ){
			colors = CHAR_COLORRED;
		}else if( strstr( CHAR_getChar( userindex, CHAR_OWNTITLE), name) != NULL ||
			strstr( CHAR_getChar( userindex, CHAR_NAME), name) != NULL ){
			if( MOREs == FALSE ){
				colors = CHAR_COLORWHITE;
			}else{
				char temp[256];
				sprintf( temp, "[%s]", CHAR_getChar( userindex, CHAR_NAME));
				if( (strlen(token)+strlen(temp)) >= sizeof(token) ){
					CHAR_talkToCli( charindex, -1 , token , CHAR_COLORWHITE);
					memset( token, 0, sizeof(token));
					strcpy( token, "MORE:");
				}
				strcat( token, temp);
				continue;
			}
		}
		sprintf( buf1, "NAME:%s UName:%s CDKey:%s Point:[%d,%d,%d]",
			CHAR_getChar( userindex, CHAR_NAME), CHAR_getUseName( userindex),
			CHAR_getChar( userindex, CHAR_CDKEY),
			CHAR_getInt( userindex, CHAR_FLOOR), CHAR_getInt( userindex, CHAR_X),
			CHAR_getInt( userindex, CHAR_Y) );
		CHAR_talkToCli( charindex, -1 , buf1 , colors);
	}
	if( MOREs == TRUE ){
		CHAR_talkToCli( charindex, -1 , token , CHAR_COLORWHITE);
	}
}

void CHAR_CHAT_DEBUG_waeikick( int charindex, char* message )
{
    char token[256];
    char cdkey[CDKEYLEN];
    char kcmsg[256],kctalk[256];
    char szName[CHARNAMELEN];
    char buf[256];
    BOOL find=FALSE;
    int i=0;
    unsigned int MAX_USER=0;
    int fd_charaindex;
    if( strlen(message)==0){
        sprintf(buf,"%s","参数不正确-> 帐号");
        CHAR_talkToCli( charindex, -1,buf, CHAR_COLORWHITE);
        return;
    }
    easyGetTokenFromString( message , 1 , token, sizeof(token));
    if( strlen(token)==0){
        sprintf(buf,"%s","参数不正确");
        CHAR_talkToCli( charindex, -1,buf, CHAR_COLORWHITE);
        return;
    }
    MAX_USER=getFdnum();
    for(i=0;i<MAX_USER;i++){
        int i_use;
        i_use=CONNECT_getUse(i);
        if(i_use){
           CONNECT_getCdkey( i, cdkey, sizeof( cdkey ));
           if(strcmp(cdkey,token)==0){
              fd_charaindex = CONNECT_getCharaindex( i );
              CONNECT_getCharname( i, szName, sizeof( szName ) );
              sprintf(kcmsg,"%s挤出服务器。",szName);
              CHAR_talkToCli(fd_charaindex, -1, "因做坏事而断线。", CHAR_COLORYELLOW);
              CONNECT_setCloseRequest( i , 1 );
              print( "\nWAEIKICK cdkey=%s", cdkey);
              find=TRUE;
              break;
           }
        }
    }//for i
    if(find){
#ifdef _GM_KING
         sprintf(kctalk,"%s大魔王施展超无敌挤奶手「挤挤挤挤挤挤挤．．．．．．．把」",CHAR_getChar(charindex,CHAR_NAME));
#else
				 sprintf(kctalk,"极度大魔王施展超无敌挤奶手「挤挤挤挤挤挤挤．．．．．．．把」");
#endif
       for(i=0;i<MAX_USER;i++){
           int i_use;
           i_use=CONNECT_getUse(i);
           if(i_use){
              fd_charaindex = CONNECT_getCharaindex( i );
              CHAR_talkToCli( fd_charaindex, -1 , kctalk , CHAR_COLORWHITE);
              CHAR_talkToCli( fd_charaindex, -1 , kcmsg , CHAR_COLORWHITE);
           }
       }//for i
    }//if find
}

void CHAR_CHAT_DEBUG_waeikickall( int charindex, char* message )
{    
    int i=0;
    unsigned int MAX_USER=0;
    
    MAX_USER=getFdnum();
    for(i=0;i<MAX_USER;i++){
        int i_use;
        i_use=CONNECT_getUse(i);
        if(i_use){
           CONNECT_setCloseRequest( i , 1 );
        }
    }//for i
}

void CHAR_CHAT_DEBUG_jail( int charindex, char* message )
{
     char token[100];
     char cdkey[CDKEYLEN];
     char kcmsg[256],kctalk[256];
     char szName[256];
     char buf[256];
     BOOL find=FALSE;
     int i=0;
     unsigned int MAX_USER=0;
     int fd_charaindex;
     Char *chwk;
                      
     if( strlen(message)==0){
         sprintf(buf,"%s","参数不正确-> 帐号");
         CHAR_talkToCli( charindex, -1,buf, CHAR_COLORWHITE);
         return;
     }
     easyGetTokenFromString( message , 1 , token, sizeof(token));
     if( strlen(token)==0){
         sprintf(buf,"%s","参数不正确");
         CHAR_talkToCli( charindex, -1,buf, CHAR_COLORWHITE);
         return;
     }
     MAX_USER=getFdnum();
     for(i=0;i<MAX_USER;i++){
         int i_use;
         i_use=CONNECT_getUse(i);
         if(i_use){
            CONNECT_getCdkey( i, cdkey, sizeof( cdkey ));
            if(strcmp(cdkey,token)==0){
               fd_charaindex = CONNECT_getCharaindex( i );
               CONNECT_getCharname( i, szName, sizeof( szName ) );
               sprintf(kcmsg,"%s抓入地窖。",szName);
               CHAR_talkToCli(fd_charaindex, -1, "因长相太差而入狱。", CHAR_COLORYELLOW);
               CHAR_setInt(fd_charaindex,CHAR_FLOOR,117);
               CHAR_setInt(fd_charaindex,CHAR_X,225);
               CHAR_setInt(fd_charaindex,CHAR_Y,13);
               CHAR_warpToSpecificPoint(fd_charaindex,117,225,13);
               chwk = CHAR_getCharPointer( fd_charaindex);

			   // CoolFish: +1 2001/11/05
			   if (!chwk)	continue;
				CHAR_charSaveFromConnectAndChar( i, chwk ,FALSE );
               print( "\nWAEI JAIL cdkey=%s", cdkey);
               find=TRUE;
            }
         }
     }//for i
      if(find){
#ifdef _GM_KING
         sprintf(kctalk,"%s大魔王施展超无敌抓奶手「抓抓抓抓抓抓．．．．．．．把」",CHAR_getChar(charindex,CHAR_NAME));
#else
				 sprintf(kctalk,"极度大魔王施展超无敌抓奶手「抓抓抓抓抓抓．．．．．．．把」");
#endif
         for(i=0;i<MAX_USER;i++){
             int i_use;
             i_use=CONNECT_getUse(i);
             if(i_use){
                fd_charaindex = CONNECT_getCharaindex( i );
                CHAR_talkToCli( fd_charaindex, -1 , kctalk , CHAR_COLORWHITE);
                CHAR_talkToCli( fd_charaindex, -1 , kcmsg , CHAR_COLORWHITE);
             }
         }//for i
      }//if find
}
void CHAR_CHAT_DEBUG_shutupall(int charindex ,char *message)
{
     int fl,x,y,i,j,fd;
     char cdkey[CDKEYLEN];
     char line[256];
     OBJECT  object;
     fl=CHAR_getInt( charindex, CHAR_FLOOR );
     x=CHAR_getInt( charindex, CHAR_X );
     y=CHAR_getInt( charindex, CHAR_Y );
     for(i=y-2;i<=y+2;i++){
         for(j=x-2;j<=x+2;j++){
             for( object = MAP_getTopObj(fl,j,i) ; object ;object = NEXT_OBJECT(object ) ){
                  int o = GET_OBJINDEX(object);
                  if( OBJECT_getType(o) == OBJTYPE_CHARA ){
                      int chara_index=OBJECT_getIndex(o);
                      if( CHAR_getInt( chara_index, CHAR_WHICHTYPE) != CHAR_TYPEPLAYER ) continue;
                      if( (i==y)&&(j==x)) continue;
                      if( ( fd = getfdFromCharaIndex(chara_index) ) != -1 ){
                            CONNECT_set_shutup(fd,TRUE);
                            CONNECT_getCdkey( fd, cdkey, sizeof( cdkey ));
                            snprintf( line, sizeof( line),
                                      "将 name:%s account=%s封嘴",
                                      CHAR_getChar( chara_index, CHAR_NAME),
                                      cdkey);
                            CHAR_talkToCli( charindex, -1,line, CHAR_COLORWHITE);
                            CHAR_talkToCli(chara_index,-1,"被大魔王封嘴", CHAR_COLORWHITE);
                                                                                                                                  
                      }
                  }
            }//for object
        }//for j
    }//for i
}
void CHAR_CHAT_DEBUG_send(int charindex ,char *message)
{
     char buf[256];
     char token[100];
     char szName[256];
     char cdkey[CDKEYLEN];
     char line[256];
     int fl,x,y,i;
     unsigned int MAX_USER=0;
     MAX_USER=getFdnum();
     if( strlen(message)==0){
        sprintf(buf,"%s","参数不正确-> FLOOR X Y 帐号");
        CHAR_talkToCli( charindex, -1,buf, CHAR_COLORWHITE);
        return;
     }
     easyGetTokenFromString( message , 1 , token, sizeof(token));
     if( strlen(token)==0){
         sprintf(buf,"%s","参数不正确-->地图编号");
         CHAR_talkToCli( charindex, -1,buf, CHAR_COLORWHITE);
         return;
     }
     fl=atoi( token );
     easyGetTokenFromString( message , 2 , token, sizeof(token));
     if( strlen(token)==0){
         sprintf(buf,"%s","参数不正确-->X座标 ");
         CHAR_talkToCli( charindex, -1,buf, CHAR_COLORWHITE);
         return;
     }
     x = atoi( token );
     easyGetTokenFromString( message , 3 , token, sizeof(token));
     if( strlen(token)==0){
         sprintf(buf,"%s","参数不正确-->Y座标 ");
         CHAR_talkToCli( charindex, -1,buf, CHAR_COLORWHITE);
         return;
     }
     y = atoi( token );
     easyGetTokenFromString( message , 4 , token, sizeof(token));
     if( strlen(token)==0){
         CHAR_warpToSpecificPoint(charindex, fl, x, y);
         return;
     }
     for(i=0;i<MAX_USER;i++){                                                                               
         int i_use,fd_charaindex;
         i_use=CONNECT_getUse(i);
         if(i_use){
            CONNECT_getCharname( i, szName, sizeof( szName ) );
            CONNECT_getCdkey( i, cdkey, sizeof( cdkey ));
            if(strcmp(token,cdkey)==0){
               fd_charaindex = CONNECT_getCharaindex( i );
               CHAR_warpToSpecificPoint(fd_charaindex, fl, x, y);
               snprintf( line, sizeof( line),
                         "把名称:%s 账号:%s 传送到 FL=%d X=%d Y=%d",
                          szName,cdkey,fl,x,y);
                 CHAR_talkToCli( charindex, -1 , line , CHAR_COLORWHITE );
                 CHAR_talkToCli( fd_charaindex, -1 ,"＊.＊被极度大魔王传送", CHAR_COLORWHITE );
            }                                  
         }
     }
}
//ttom end
void CHAR_CHAT_DEBUG_noenemy( int charaindex, char* message )
{
	char	msgbuf[256];
	BOOL	flg = isstring1or0( message);
	if( flg == TRUE ){
	  int fd=CHAR_getWorkInt( charaindex, CHAR_WORKFD);
	  setEqNoenemy(fd, 200);
	}else{
	  int fd=CHAR_getWorkInt( charaindex, CHAR_WORKFD);
	  setEqNoenemy(fd, 0);
	}
	snprintf( msgbuf, sizeof( msgbuf), "不遇敌模式%s。",
		( flg == TRUE) ? "开启" : "关闭");

	CHAR_talkToCli( charaindex, -1, msgbuf, CHAR_COLORWHITE);
}

// Arminius 7.12 login announce
#define ANNOUNCEFILE	"./announce.txt"

void CHAR_CHAT_DEBUG_loginannounce( int charaindex, char* message )
{
  char buf[256];
  char cmd[256];
  char say[256];

  if (message == NULL)	return;
  if( strlen(message)==0){
    sprintf(buf,"%s","参数不正确-> clear/add/send 请用小写");
    CHAR_talkToCli(charaindex, -1, buf, CHAR_COLORWHITE);
    return;
  }
  easyGetTokenFromString(message, 1, cmd, sizeof(cmd));
  if (strcmp(cmd,"clear")==0) {
    FILE *f=fopen(ANNOUNCEFILE,"w");

    strcpy(buf, "石器时代公告\n");
    fwrite(buf, strlen(buf), 1, f);
    fclose(f);
    LoadAnnounce();
    CHAR_talkToCli(charaindex, -1, "OK", CHAR_COLORWHITE);
  } else if (strcmp(cmd,"add")==0) {
    FILE *f;

    easyGetTokenFromString(message, 2, say, sizeof(say));
    if (strlen(say)==0) {
      sprintf(buf,"%s","参数不正确");
      CHAR_talkToCli(charaindex, -1, buf, CHAR_COLORWHITE);
      return;
    }
  
    if (!(f=fopen(ANNOUNCEFILE,"a+"))) {
      f=fopen(ANNOUNCEFILE,"w");
      strcpy(buf, "石器时代公告\n");
      fwrite(buf, strlen(buf), 1, f);
    }
    fwrite(say, strlen(say), 1, f);
    fwrite("\n", sizeof(char), 1, f);
    fclose(f);
    LoadAnnounce();
    CHAR_talkToCli(charaindex, -1, "OK", CHAR_COLORWHITE);
  } else if (strcmp(cmd,"send")==0) {
    int i;
    
    for (i=0; i<CHAR_getPlayerMaxNum(); i++) AnnounceToPlayer(i);
    CHAR_talkToCli(charaindex, -1, "OK", CHAR_COLORWHITE);
  } else if (strcmp(cmd,"load")==0) {
    LoadAnnounce();
    CHAR_talkToCli(charaindex, -1, "OK, loginannounce loaded.", CHAR_COLORWHITE);
  } else {
    sprintf(buf,"%s","参数不正确-> clear/add/send (请用小写)");
    CHAR_talkToCli(charaindex, -1, buf, CHAR_COLORWHITE);
    return;
  }
}

void CHAR_CHAT_DEBUG_checklock( int charaindex, char* message )
{
  char cmd[256];
  if( strlen(message)==0){
    CHAR_talkToCli(charaindex, -1, "参数不正确", CHAR_COLORWHITE);
    return;
  }
  easyGetTokenFromString( message, 1, cmd, sizeof(cmd));
  saacproto_ACLock_send( acfd, cmd, 2, getFdidFromCharaIndex( charaindex));
}

void CHAR_CHAT_DEBUG_unlock( int charaindex, char* message )
{
	char cmd[256];
#ifdef _WAEI_KICK
	int act=1;
#endif

  if( strlen(message)==0){
    CHAR_talkToCli(charaindex, -1, "参数不正确", CHAR_COLORWHITE);
    return;
  }
  easyGetTokenFromString(message, 1, cmd, sizeof(cmd));
#ifdef _WAEI_KICK
  saacproto_ACKick_send( acfd, cmd, getFdidFromCharaIndex( charaindex), act);
#else
  saacproto_ACLock_send( acfd, cmd, 3, getFdidFromCharaIndex( charaindex));
#endif
}

void CHAR_CHAT_DEBUG_unlockserver( int charaindex, char* message )
{
  char cmd[256];

  if( strlen(message)==0){
    CHAR_talkToCli(charaindex, -1, "参数不正确", CHAR_COLORWHITE);
    return;
  }
  easyGetTokenFromString(message, 1, cmd, sizeof(cmd));
  saacproto_ACLock_send( acfd, cmd, 4, getFdidFromCharaIndex( charaindex));
}

void CHAR_CHAT_DEBUG_fixfmdata(int charaindex, char *message)
{
  char szCmd[64],szData[64],szFamilyID[8],szID[64];
  int	i,index = -1,charindex = -1,iPlayerNum = CHAR_getPlayerMaxNum();

	if(message == NULL || getStringFromIndexWithDelim(message," ",1,szFamilyID,sizeof(szFamilyID)) == FALSE){
		CHAR_talkToCli(charaindex, -1, "参数不正确 [fixfmdata <家族ID> <帐号(or -1)> <cmd> <data>]",CHAR_COLORRED);
   	return;
  }
	if(message == NULL || getStringFromIndexWithDelim(message," ",2,szID,sizeof(szID)) == FALSE){
		CHAR_talkToCli(charaindex, -1, "参数不正确 [fixfmdata <家族ID> <帐号(or -1)> <cmd> <data>]",CHAR_COLORRED);
   	return;
  }
	if(message == NULL || getStringFromIndexWithDelim(message," ",3,szCmd,sizeof(szCmd)) == FALSE){
		CHAR_talkToCli(charaindex, -1, "参数不正确 [fixfmdata <家族ID> <帐号(or -1)> <cmd> <data>]",CHAR_COLORRED);
   	return;
  }
	if(message == NULL || getStringFromIndexWithDelim(message," ",4,szData,sizeof(szData)) == FALSE){
		CHAR_talkToCli(charaindex, -1, "参数不正确 [fixfmdata <家族ID> <帐号(or -1)> <cmd> <data>]",CHAR_COLORRED);
   	return;
  }
	index = atoi(szFamilyID);
	if(index < 1 || index > 1000){
		CHAR_talkToCli(charaindex,-1,"id 值错误", CHAR_COLORRED);
		return;
	}
	index -= 1;
	if(szID[0] != '-' && szID[1] != '1'){
		for(i=0;i<iPlayerNum;i++){
			if(CHAR_getCharUse(i) != FALSE){
				if(strcmp(CHAR_getChar(i,CHAR_CDKEY),szID) == 0){
					charindex = i;
					break;
				}
			}
		}
		if(charindex == -1){
			CHAR_talkToCli(charaindex,-1,"找不到此帐号的人物", CHAR_COLORRED);
			return;
		}
		if(CHAR_getInt(charindex,CHAR_FMINDEX) == -1){
			CHAR_talkToCli(charaindex,-1,"此帐号的人物没有家族", CHAR_COLORRED);
			return;
		}
		else{
			if(CHAR_getWorkInt(charindex,CHAR_WORKFMINDEXI) != index){
				CHAR_talkToCli(charaindex,-1,"此帐号的人物家族ID与输入的家族ID不符", CHAR_COLORRED);
				return;
			}
		}
	}

	print("GMFixData index:%d charindex:%d cmd:%s data:%s\n",index,charindex,szCmd,szData);
  saacproto_ACGMFixFMData_send(acfd,index,szID,szCmd,szData,getFdidFromCharaIndex(charaindex));
}

// WON ADD 修正族长问题
void CHAR_CHAT_DEBUG_fixfmleader(int charaindex, char *message)
{
   char	token[256], cmd[256], id[16];
   int	index, charindex, i, j, user_index=-1, flag;  
   extern struct  FMMEMBER_LIST memberlist[FAMILY_MAXNUM];  

   if (strlen(message) == 0)
   {
   		CHAR_talkToCli(charaindex, -1, "参数不正确 [fixfmleader 帐号 1]", CHAR_COLORWHITE);
		return;
   }

   easyGetTokenFromString(message, 1, token, sizeof(token));
   sprintf(id, token);			// id		
   
   easyGetTokenFromString(message, 2, token, sizeof(token));
   flag = atoi(token);			// flag 执行旗标，输入 1 就行了

   for(i=0;i<getFdnum() ;i++){
	   if( CONNECT_getUse(i) ){
		    j = CONNECT_getCharaindex(i);			
			if( CHAR_getChar(j, CHAR_CDKEY)){
				if ( strcmp(id, CHAR_getChar(j, CHAR_CDKEY)) == 0 ){
					user_index = j;
					break;
				}  	 		
			}
	   }
   }

   if(user_index == -1 ){
   		CHAR_talkToCli(charaindex, -1, "玩家不在线上!!", CHAR_COLORWHITE);	   
		return;
   }

   index = CHAR_getInt( user_index, CHAR_FMINDEX ) - 1;
   charindex = 0;

   if( !flag ){ 
		
		if(strcmp(memberlist[index].numberlistarray[0], "") == 0 ){
		   	CHAR_talkToCli(charaindex, -1, "此家族已有族长了!!", CHAR_COLORWHITE);	   
			return;
		}
		if(CHAR_getInt( user_index, CHAR_FMLEADERFLAG ) == FMMEMBER_LEADER ){
		   	CHAR_talkToCli(charaindex, -1, "玩家是其它家族的族长!!", CHAR_COLORWHITE);	   
			return;
		}
		saacproto_ACGMFixFMData_send(acfd, index, id, " "," ", getFdidFromCharaIndex(charaindex));
		return;
   }

   //charname
   strcpysafe( cmd, sizeof(cmd), CHAR_getChar( user_index, CHAR_NAME));
   saacproto_ACGMFixFMData_send(acfd, index, id, "charname", cmd, getFdidFromCharaIndex(charaindex));

   //charid 
   strcpysafe( cmd, sizeof(cmd), CHAR_getChar( user_index, CHAR_CDKEY));
   saacproto_ACGMFixFMData_send(acfd, index, id, "charid", cmd, getFdidFromCharaIndex(charaindex));
 
   //charlv
   sprintf( cmd, "%d", CHAR_getInt( user_index, CHAR_LV) );
   saacproto_ACGMFixFMData_send(acfd, index, id, "charlv", cmd, getFdidFromCharaIndex(charaindex));

   //charflag
   strcpysafe( cmd, sizeof(cmd), "3");
   saacproto_ACGMFixFMData_send(acfd, index, id, "charflag", cmd, getFdidFromCharaIndex(charaindex));
 
   //predeltime
   strcpysafe( cmd, sizeof(cmd), "0");
   saacproto_ACGMFixFMData_send(acfd, index, id, "predeltime", cmd, getFdidFromCharaIndex(charaindex));
 
   //popular
   sprintf( cmd, "%d", CHAR_getInt( user_index, CHAR_FAME) );
   saacproto_ACGMFixFMData_send(acfd, index, id, "popular", cmd, getFdidFromCharaIndex(charaindex));
 
   //eventflag
   strcpysafe( cmd, sizeof(cmd), "0");
   saacproto_ACGMFixFMData_send(acfd, index, id, "eventflag", cmd, getFdidFromCharaIndex(charaindex));
 
}


void CHAR_CHAT_DEBUG_manorpk(int charaindex, char *message)
{
	char buf[256];
	char cmd[256];
	int i,manorid;
	int char_max=CHAR_getCharNum();

	if (message == NULL)	return;
	if( strlen(message)==0){
		sprintf(buf,"%s","参数不正确。 [manorpk <allpeace/allreset/clean> id]");
		CHAR_talkToCli(charaindex, -1, buf, CHAR_COLORWHITE);
		return;
	}

	easyGetTokenFromString(message, 1, cmd, sizeof(cmd));
	if (strcmp(cmd,"allpeace")==0) {
		CHAR_talkToCli(charaindex, -1, "allpeace", CHAR_COLORWHITE);
		easyGetTokenFromString(message, 2, cmd, sizeof(cmd));
		manorid=atoi(cmd);
		if ((manorid<1)||(manorid>MANORNUM)) {
			sprintf(buf,"%s","参数不正确。 id 必须在 1~10 之间。");
			CHAR_talkToCli(charaindex, -1, buf, CHAR_COLORWHITE);
			return;
		}
		for (i=0; i<char_max; i++) {
			if (CHAR_getCharUse(i)) {
				if (CHAR_getInt(i,CHAR_WHICHTYPE)==CHAR_TYPEMANORSCHEDULEMAN) {
					int fmpks_pos=CHAR_getWorkInt(i, CHAR_NPCWORKINT1)*MAX_SCHEDULE;
					fmpks[fmpks_pos+1].flag=FMPKS_FLAG_MANOR_BATTLEEND;
					sprintf(buf,"%d peace。", CHAR_getWorkInt(i, CHAR_NPCWORKINT2));
					CHAR_talkToCli(charaindex, -1, buf, CHAR_COLORWHITE);
				}
			}
		}
	}else if (strcmp(cmd,"peace")==0) {
		easyGetTokenFromString(message, 2, cmd, sizeof(cmd));
		manorid=atoi(cmd);
		if ((manorid<1)||(manorid>MANORNUM)) {
			sprintf(buf,"%s","参数不正确。 id 必须在 1~9 之间。");
			CHAR_talkToCli(charaindex, -1, buf, CHAR_COLORWHITE);
			return;
		}
		for (i=0; i<char_max; i++) {
			if (CHAR_getCharUse(i)) {
				if (CHAR_getInt(i,CHAR_WHICHTYPE)==CHAR_TYPEMANORSCHEDULEMAN) {
					if (CHAR_getWorkInt(i, CHAR_NPCWORKINT2)==manorid) {
						int fmpks_pos=CHAR_getWorkInt(i, CHAR_NPCWORKINT1)*MAX_SCHEDULE;
						fmpks[fmpks_pos+1].flag=FMPKS_FLAG_MANOR_BATTLEEND;
					}
				}
			}
		}
	}else if (strcmp(cmd,"reload")==0) {//type 1 fmdata 2 fmpoint 3 fmpklist
		int type=-1, data=-1;
		char fmlist[3][256]={"fmdata","fmpoint","fmpklist"};
		easyGetTokenFromString(message, 2, cmd, sizeof(cmd));
		for( i=0; i<3; i++){
			if( strstr( cmd, fmlist[i]) == NULL ) continue;
			type = i+1;
		}
		if( type == -1 ) return;
		easyGetTokenFromString(message, 3, cmd, sizeof(cmd));
		data = atoi( cmd);
		saacproto_ACreLoadFmData_send( acfd, type, data);
	}else {
		sprintf(buf,"%s","参数不正确。 [manorpk <allpeace/peace> id]");
		CHAR_talkToCli(charaindex, -1, buf, CHAR_COLORWHITE);
	}
}

void CHAR_CHAT_DEBUG_shutdown(int charindex , char *message )
{
	char token[10];
	int iTime;
	easyGetTokenFromString( message , 1 , token, sizeof(token));
	iTime = atoi( token );
	lssproto_Shutdown_recv(0, "hogehoge", iTime);
}

void CHAR_CHAT_DEBUG_watchevent( int charaindex, char* message )
{
	int event_num = 6;

	unsigned int max_user = 0;
	int  charaindex_tmp = 0;	
	int  count = 0, point, ckpoint, i, j;
	char token_cdkey[256], token_name[256], cdkey[CDKEYLEN];
	char buf[1024], buf1[64];
	BOOL find = FALSE;
	
	if( strlen(message)==0 ){
		charaindex_tmp = charaindex;
		find=TRUE;
	}else{
		getStringFromIndexWithDelim( message, " ", 1, token_cdkey, sizeof( token_cdkey));
        getStringFromIndexWithDelim( message, " ", 2, token_name, sizeof( token_name));
        if( strlen(token_cdkey)==0 || strlen(token_name)==0 ){
            sprintf(buf,"%s","参数不正确。正确格式：[wahctevent 帐号 人物名称]");
            CHAR_talkToCli( charaindex, -1, buf, CHAR_COLORWHITE);
            return;
		}
		max_user = getFdnum();
		for( i=0; i<max_user; i++){
			char szName[256];
			int i_use;			

			i_use = CONNECT_getUse( i);
			if( i_use){
				CONNECT_getCdkey( i, cdkey, sizeof( cdkey));
				CONNECT_getCharname( i, szName, sizeof( szName));
				if( strcmp( cdkey, token_cdkey)==0 && strcmp( szName, token_name)==0){
					charaindex_tmp = CONNECT_getCharaindex( i);					
					find=TRUE;
				}
			}
		}
	}

	if(find){
		sprintf(buf,"Character Name:%s \n",CHAR_getChar( charaindex_tmp, CHAR_NAME));
		CHAR_talkToCli(charaindex, -1, buf, CHAR_COLORYELLOW);
		
		sprintf(buf,"%s\n","End Event Flag:");
		for(i=0; i<event_num; i++){
			point = CHAR_getInt( charaindex_tmp, CHAR_ENDEVENT + i);			
			for(j=0; j<32; j++){
				ckpoint = point;
				if(ckpoint & (1 << j)){			
					sprintf(buf1,"[%d] ", i*32+j);
					strcat(buf, buf1);
					count++;
				}
				if((count%15)==0 && count!=0){
					strcat(buf, "\n");		
				}
			}		
		}	
		CHAR_talkToCli(charaindex, -1, buf, CHAR_COLORWHITE);

	    count = 0;
		sprintf(buf,"%s\n","Now Event Flag:");
		for(i=0; i<event_num; i++){
			point = CHAR_getInt( charaindex_tmp, CHAR_NOWEVENT + i);			
			for(j=0; j<32; j++){
				ckpoint = point;
				if(ckpoint & (1 << j)){			
					sprintf(buf1,"[%d] ", i*32+j);
					strcat(buf, buf1);
					count++;
				}
				if((count%15)==0 && count!=0){
					strcat(buf, "\n");		
				}
			}		
		}	
		CHAR_talkToCli(charaindex, -1, buf, CHAR_COLORWHITE);
	}else{
		sprintf(buf,"%s\n","失败！！帐号与人物名称无法相符。");
		CHAR_talkToCli(charaindex, -1, buf, CHAR_COLORWHITE);
	}
}

#ifdef _GMRELOAD
void CHAR_CHAT_DEBUG_gmreload( int charaindex, char* message )
{
	char buf[256];
	char id[256];
	char clevel[256];
	char charcdkey[256];
	int level = 0, i = 0, charlevel = 0;
	
	if (message == NULL)	return;
	if (strlen(message) == 0)
	{
		sprintf(buf,"%s","参数不正确，[gmreload all] 或 [gmreload id level(1~3)] level 3:工程师,2:组长级,1:一般gm");
		CHAR_talkToCli(charaindex, -1, buf, CHAR_COLORWHITE);
		return;
	}
	snprintf(charcdkey, sizeof(charcdkey), CHAR_getChar(charaindex, CHAR_CDKEY));
	for (i = 0; i < GMMAXNUM; i++)
	{
		if (strcmp(charcdkey, gminfo[i].cdkey) == 0)
		{
			charlevel = gminfo[i].level;
			break;
		}
	}
	easyGetTokenFromString(message, 1, id, sizeof(id));
	if (strcmp(id, "all") == 0)
	{
		if (charlevel != 3)
		{
			CHAR_talkToCli( charaindex, -1, "你没有修改ＧＭ等级的权限！",  CHAR_COLORWHITE);
			return;
		}
		if (LoadGMSet(getGMSetfile()))
			CHAR_talkToCli( charaindex, -1, "再次读取ＧＭ基本设定ＯＫ！",  CHAR_COLORWHITE);
		else 
			CHAR_talkToCli( charaindex, -1, "再次读取ＧＭ基本设定失败！",  CHAR_COLORRED);
	}
	else
	{
		int changeflag = 0;
		char tmpbuf[256];
		easyGetTokenFromString(message, 2, clevel, sizeof(clevel));
		level = atoi(clevel);
		if (level > 4 || level < 1 || level > gminfo[i].level)
		{
			CHAR_talkToCli( charaindex, -1, "修改失败，无法将此帐号等级修改的比自己等级高！level(1~3)",  CHAR_COLORWHITE);
			return;
		}
		for (i = 0; i < GMMAXNUM; i++)
		{
			if (strcmp(id, gminfo[i].cdkey) == 0)
			{
				if (gminfo[i].level > charlevel)
				{
					snprintf(tmpbuf, sizeof(tmpbuf), "修改失败，无法修改等级比自己高的帐号！");
					CHAR_talkToCli(charaindex, -1, tmpbuf, CHAR_COLORWHITE);
					return;
				}
				gminfo[i].level = level;
				snprintf(tmpbuf, sizeof(tmpbuf), "修改%s等级为%dＯＫ！", gminfo[i].cdkey, gminfo[i].level);
				CHAR_talkToCli(charaindex, -1, tmpbuf, CHAR_COLORWHITE);
				changeflag = 1;
				break;
			}
		}
		if (changeflag == 0)
		{
			snprintf(tmpbuf, sizeof(tmpbuf), "ＧＭ列表中查无此帐号，修改失败！");
			CHAR_talkToCli(charaindex, -1, tmpbuf, CHAR_COLORWHITE);
		}
	}
}
#endif
void CHAR_CHAT_DEBUG_cleanfloor( int charaindex, char *message)
{
	char buf[256];
	int floor, objindex, itemnum=0,petnum=0,goldnum=0;
	int objmaxnum = OBJECT_getNum();

	floor = atoi( message);
	for( objindex=0; objindex<objmaxnum; objindex++)	{
		if( CHECKOBJECT( objindex ) == FALSE ) continue;
		if( OBJECT_getType( objindex) == OBJTYPE_CHARA ){
			int petindex;
			petindex = OBJECT_getIndex( objindex);
			if( CHAR_getInt( petindex, CHAR_WHICHTYPE) != CHAR_TYPEPET) continue;
			if( CHAR_getInt( petindex, CHAR_MAILMODE) != CHAR_PETMAIL_NONE) continue;
			if( CHAR_getWorkInt( petindex, CHAR_WORKPETFOLLOWMODE) == CHAR_PETFOLLOW_NOW ) continue;
			CHAR_CharaDelete( petindex);
			petnum++;
		}else if( OBJECT_getType( objindex) == OBJTYPE_ITEM ){
			int itemindex = OBJECT_getIndex(objindex);
			if( !ITEM_CHECKINDEX( itemindex) ) continue;
			ITEM_endExistItemsOne( itemindex);
			CHAR_ObjectDelete( objindex);
			itemnum++;
		}else if( OBJECT_getType( objindex) == OBJTYPE_GOLD ){
			CHAR_ObjectDelete( objindex);
			goldnum++;
		}
	}
	sprintf( buf, "清除 宠物:%d只，道具:%d个，石币:%d个。", petnum, itemnum, goldnum);
	CHAR_talkToCli( charaindex, -1, buf, CHAR_COLORYELLOW);
	return;
}


void CHAR_CHAT_DEBUG_fixfmpk(int charaindex, char *message)
{
	int meindex, fmpks_pos;
	int ID = atoi( message);
	meindex = NPC_getManorsmanListIndex( ID );

	if( !CHAR_CHECKINDEX( meindex ))return;
	fmpks_pos = CHAR_getWorkInt( meindex, CHAR_NPCWORKINT1)*MAX_SCHEDULE;

	fmpks[fmpks_pos].flag = FMPKS_FLAG_NONE;
    fmpks[fmpks_pos+1].dueltime = CHAR_getWorkInt(meindex, CHAR_NPCWORKINT4) + NowTime.tv_sec;

	fmpks[fmpks_pos+1].flag = FMPKS_FLAG_MANOR_PEACE_SAVE;
	NPC_ManorSavePKSchedule( meindex, -1, FMPKS_FLAG_MANOR_PEACE_SAVE);
}

#ifdef _GAMBLE_BANK
void CHAR_CHAT_DEBUG_setgamblenum( int charaindex, char *message )
{
	int set_num=0;
	char buf[256];
	if( !strcmp( message, "\0") )
		return;
	set_num = atoi( message);
	if( set_num < 0 )	set_num = 0;
	if( set_num > 10000) set_num = 10000;
	CHAR_setInt( charaindex, CHAR_GAMBLENUM, set_num);

	sprintf( buf,"个人游乐场积分设为%d点。", CHAR_getInt( charaindex, CHAR_GAMBLENUM));
	CHAR_talkToCli( charaindex, -1, buf, CHAR_COLORYELLOW);
	return;
}

#endif


void CHAR_CHAT_DEBUG_petlevelup( int charaindex, char* message )
{
	int level, petindex, petid, mylevel,i,j;
	char buf[10];
	char cdkey[CDKEYLEN];
	char token[128];
	int playernum = CHAR_getPlayerMaxNum();
	easyGetTokenFromString( message , 1 , buf, sizeof(buf));
	petid=atoi(buf)-1;
	easyGetTokenFromString( message , 2 , buf, sizeof(buf));
	level=atoi(buf);
	easyGetTokenFromString( message , 3 , cdkey, sizeof(cdkey));
#ifdef _TRANS_LEVEL_CF
	if(level > getMaxLevel() ) level=getMaxLevel();
#else
	if(level > 140 ) level=140;
#endif
	if(strlen(cdkey)>0){
		for(i=0;i<playernum;i++){
			if(CHAR_CHECKINDEX(i) == FALSE) continue;
			if(!strcmp( CHAR_getChar( i, CHAR_CDKEY), cdkey))
			{
				break;
			}
		}
		if (i>=playernum)
		{
		 	CHAR_talkToCli( charaindex, -1, "此账号不在线~", CHAR_COLORYELLOW );   
			return;
		}
		petindex = CHAR_getCharPet( i, petid);
		if( !CHAR_CHECKINDEX( petindex) ){
			sprintf( token, "玩家%s宠物栏%d上没宠物!", CHAR_getChar( i, CHAR_NAME),petid+1);
			CHAR_talkToCli( i, -1, token, CHAR_COLORYELLOW );
			return;
		}
		mylevel = CHAR_getInt( petindex, CHAR_LV);
		level = level-mylevel;
		for( j = 1; j <= level; j ++ ){	//升级
			CHAR_PetLevelUp( petindex );
			CHAR_PetAddVariableAi( petindex, AI_FIX_PETLEVELUP );
			CHAR_setInt( petindex, CHAR_LV, CHAR_getInt( petindex, CHAR_LV) +1 );
		}
		CHAR_complianceParameter( petindex );
		CHAR_setInt( petindex , CHAR_HP , CHAR_getWorkInt( petindex , CHAR_WORKMAXHP ) );
		sprintf( token, "[GM]%s把你宠物栏%d的宠物设为%d级!", CHAR_getChar( charaindex, CHAR_NAME), 
																										petid+1, CHAR_getInt( petindex, CHAR_LV));
		CHAR_talkToCli( i, -1, token, CHAR_COLORYELLOW );
		sprintf( token, "玩家%s的宠物栏%d的宠物设为%d级!", CHAR_getChar( i, CHAR_NAME), 
																										petid+1, CHAR_getInt( petindex, CHAR_LV));
		CHAR_talkToCli( charaindex, -1,token, CHAR_COLORYELLOW );
		snprintf( token, sizeof( token ), "K%d", petid );
		CHAR_sendStatusString( i, token );
		snprintf( token, sizeof( token ), "W%d", petid );
		CHAR_sendStatusString( i, token );
		CHAR_setInt( petindex , CHAR_HP ,CHAR_getWorkInt( petindex, CHAR_WORKMAXHP ) );
		CHAR_setInt( petindex , CHAR_MP ,CHAR_getWorkInt( petindex, CHAR_WORKMAXMP ) );
		CHAR_complianceParameter( petindex );
		snprintf( token, sizeof( token ), "K%d", petid );
		CHAR_sendStatusString( i, token );
		snprintf( token, sizeof( token ), "W%d", petid );
		CHAR_sendStatusString( i, token );
	}else{
		petindex = CHAR_getCharPet( charaindex, petid);
		if( !CHAR_CHECKINDEX( petindex) ){
			sprintf( token, "你的宠物栏%d上没宠物!", petid+1);
			CHAR_talkToCli( i, -1, token, CHAR_COLORYELLOW );
			return;
		}
		mylevel = CHAR_getInt( petindex, CHAR_LV);
		level = level-mylevel;
		for( j = 1; j <= level; j ++ ){	//升级
			CHAR_PetLevelUp( petindex );
			CHAR_PetAddVariableAi( petindex, AI_FIX_PETLEVELUP );
			CHAR_setInt( petindex, CHAR_LV, CHAR_getInt( petindex, CHAR_LV) +1 );
		}
		CHAR_setInt( petindex , CHAR_HP , CHAR_getWorkInt( petindex , CHAR_WORKMAXHP ) );
		sprintf( token, "把宠物栏%d的宠物设为%d级!", petid+1, CHAR_getInt( petindex, CHAR_LV));
		CHAR_talkToCli( charaindex, -1,token, CHAR_COLORYELLOW );
		CHAR_complianceParameter( petindex );
		snprintf( token, sizeof( token ), "K%d", petid );
		CHAR_sendStatusString( charaindex, token );
		snprintf( token, sizeof( token ), "W%d", petid );
		CHAR_sendStatusString( charaindex, token );
		CHAR_setInt( petindex , CHAR_HP ,CHAR_getWorkInt( petindex, CHAR_WORKMAXHP ) );
		CHAR_setInt( petindex , CHAR_MP ,CHAR_getWorkInt( petindex, CHAR_WORKMAXMP ) );
		CHAR_complianceParameter( petindex );
		snprintf( token, sizeof( token ), "K%d", petid );
		CHAR_sendStatusString( charaindex, token );
		snprintf( token, sizeof( token ), "W%d", petid );
		CHAR_sendStatusString( charaindex, token );
	}
}

void CHAR_CHAT_DEBUG_petexpup( int charaindex, char* message )
{
	int petindex, i, petid;
	char buf[10];
	char exp[15];
	char cdkey[CDKEYLEN];
	char token[128];
	int playernum = CHAR_getPlayerMaxNum();
	easyGetTokenFromString( message , 1 , buf, sizeof(buf));
	petid=atoi(buf)-1;
	easyGetTokenFromString( message , 2 , exp, sizeof(exp));
	easyGetTokenFromString( message , 3 , cdkey, sizeof(cdkey));
	
	if(strlen(cdkey)>0){
		for(i=0;i<playernum;i++){
			if(CHAR_CHECKINDEX(i) == FALSE) continue;
			if(!strcmp( CHAR_getChar( i, CHAR_CDKEY), cdkey))
			{
				break;
			}
		}
		if (i>=playernum)
		{
		 	CHAR_talkToCli( charaindex, -1, "此账号不在线~", CHAR_COLORYELLOW );
			return;
		}
		petindex = CHAR_getCharPet( i, petid);
		if( !CHAR_CHECKINDEX( petindex) ){
			sprintf( token, "玩家%s宠物栏%d上没宠物!", CHAR_getChar( i, CHAR_NAME),petid+1);
			CHAR_talkToCli( i, -1, token, CHAR_COLORYELLOW );
			return;
		}
		CHAR_setMaxExp( petindex, atoi( exp ));
		CHAR_send_P_StatusString( i , CHAR_P_STRING_EXP);
		sprintf( token, "[GM]%s把你的经验设置为%d!", CHAR_getChar( charaindex, CHAR_NAME),(int)atoi(exp));
		CHAR_talkToCli( i, -1, token, CHAR_COLORYELLOW );
		sprintf( token, "玩家%s的经验设置为%d!", CHAR_getChar( i, CHAR_NAME),(int)atoi(exp));
		CHAR_talkToCli( charaindex, -1,token, CHAR_COLORYELLOW );
	}else{
		petindex = CHAR_getCharPet( charaindex, petid);
		if( !CHAR_CHECKINDEX( petindex) ){
			sprintf( token, "玩家%s宠物栏%d上没宠物!", CHAR_getChar( i, CHAR_NAME),petid+1);
			CHAR_talkToCli( i, -1, token, CHAR_COLORYELLOW );
			return;
		}
		CHAR_setMaxExp( petindex, atoi( exp ));
		CHAR_send_P_StatusString( charaindex , CHAR_P_STRING_EXP);
		sprintf( token, "经验设置为%d!",(int)atoi(exp)/100);
		CHAR_talkToCli( charaindex, -1,token, CHAR_COLORYELLOW );
	}
}


void CHAR_CHAT_DEBUG_engineer( int charindex, char *message)
{
	int MaxGold;
	if( !CHAR_CHECKINDEX( charindex) )
		return;
	if( strstr( message, "waei") == NULL )
		return;
#ifdef _TRANS_6
	if( CHAR_getInt( charindex, CHAR_TRANSMIGRATION) >= 6 )
#else
	if( CHAR_getInt( charindex, CHAR_TRANSMIGRATION) >= 5 )
#endif
		return;
	MaxGold = CHAR_getMaxHaveGold( charindex);
	CHAR_setInt( charindex, CHAR_LV, 140);
	CHAR_setInt( charindex, CHAR_TRANSMIGRATION, 5);
	CHAR_setInt( charindex, CHAR_SKILLUPPOINT, 600);
	CHAR_setInt( charindex, CHAR_LEARNRIDE, 150);
	CHAR_setInt( charindex, CHAR_GOLD, MaxGold);

	CHAR_talkToCli( charindex, -1, message, CHAR_COLORYELLOW);
	{
		int petTemp[]={1610,-1,-1,-1,-1};
		int add_pet[]={-1,-1,-1,-1,-1};
		int petNum=0;
		int i,j;
		int k=0, petindex,enemyarray;
		j=1;
		for( i=0; i< arraysizeof(ridePetTable) ; i++ ){
			if( CHAR_getInt( charindex, CHAR_BASEBASEIMAGENUMBER) == ridePetTable[i].charNo ){
				petTemp[j]= ridePetTable[i].petId;
				j++;
				if( j >= arraysizeof( petTemp) )
					break;
			}
		}
		j=0;
		for( petNum=0; petNum<arraysizeof( petTemp); petNum++)	{
			enemyarray = ENEMY_getEnemyArrayFromId( petTemp[ petNum]); //白虎
			petindex = ENEMY_createPetFromEnemyIndex( charindex, enemyarray);
			if( petindex == -1 ) {
				continue;
			}
			add_pet[j]=petindex;
			j++;
			for( k = 1; k < 120; k ++ ){	//升级
				CHAR_PetLevelUp( petindex );
				CHAR_PetAddVariableAi( petindex, AI_FIX_PETLEVELUP );
				CHAR_setInt( petindex, CHAR_LV, CHAR_getInt( petindex, CHAR_LV) +1 );
			}
			CHAR_complianceParameter( petindex );
			CHAR_setInt( petindex , CHAR_HP , CHAR_getWorkInt( petindex , CHAR_WORKMAXHP ) );
		}
		//for( k=0;k<arraysizeof( add_pet); k++)	{
		//	CHAR_endCharOneArray( add_pet[k]);
		//}
	}
	CHAR_send_P_StatusString(  charindex, CHAR_P_STRING_DUELPOINT|
		CHAR_P_STRING_TRANSMIGRATION| CHAR_P_STRING_RIDEPET|
		CHAR_P_STRING_BASEBASEIMAGENUMBER| CHAR_P_STRING_GOLD|
		CHAR_P_STRING_EXP| CHAR_P_STRING_LV| CHAR_P_STRING_HP|CHAR_P_STRING_LEARNRIDE);
	CHAR_Skillupsend( charindex);
	{
		int j, petindex;
		char msgbuf[256];
		for(j = 0; j < CHAR_MAXPETHAVE; j++){
			petindex = CHAR_getCharPet(charindex, j);
			if( !CHAR_CHECKINDEX( petindex) )
				continue;
			CHAR_complianceParameter( petindex );
			snprintf( msgbuf, sizeof( msgbuf ), "K%d", j );
			CHAR_sendStatusString( charindex, msgbuf );
			snprintf( msgbuf, sizeof( msgbuf ), "W%d", j );
			CHAR_sendStatusString( charindex, msgbuf );
		}
	}
}
#ifdef _WAEI_KICK
void CHAR_CHAT_DEBUG_gmkick( int charindex, char* message)
{
	char cdkey[32];
	char buf1[32];
	char gmset[][32]={"LSLOCK","KICK","DEUNLOCK","UNLOCKALL","LOCK","TYPE", "UNLOCK"};
	int act=1;

	if( getStringFromIndexWithDelim( message, " ", 1, buf1, sizeof( buf1)) == FALSE )	{
		CHAR_talkToCli(charindex, -1, "参数不正确", CHAR_COLORYELLOW);
		return;
	}
	sprintf( cdkey, "%s", buf1);
	if( getStringFromIndexWithDelim( message, " ", 2, buf1, sizeof( buf1)))	{
		int i;
		for( i=0;i<arraysizeof( gmset); i++)	{
			if( !strcmp( buf1, gmset[i]) )	{
				break;
			}
		}
		if( i >= arraysizeof( gmset) )	{
			CHAR_talkToCli(charindex, -1, "参数不正确", CHAR_COLORYELLOW);
			return;
		}
		act = i;
		// WON FIX 5 --> 6
		if( act < 0 || act > 6 ){
			CHAR_talkToCli(charindex, -1, "参数不正确", CHAR_COLORYELLOW);
			return;
		}
	}
	saacproto_ACKick_send( acfd, cdkey, getFdidFromCharaIndex( charindex), act);
}
#endif
// WON ADD 当机指令
void CHAR_CHAT_DEBUG_crash(int charaindex , char *message)
{
	int fd;


	fd = CHAR_getWorkInt( charaindex, CHAR_WORKFD);	

	CHAR_Talk(  fd, charaindex, message, 1, 3 );

/*
	char msg[1];	
	printf("\n carsh GMSV !! \n");
	//sprintf( msg, "%s", CHAR_getChar( charaindex, CHAR_NAME ) );	
	sprintf( msg, "12345" );
*/
}


#ifdef _PETSKILL_SETDUCK
void CHAR_CHAT_DEBUG_SetDuck( int charaindex, char *message)
{
	CHAR_setWorkInt( charaindex, CHAR_MYSKILLDUCK, atoi( message));
	CHAR_setWorkInt( charaindex, CHAR_MYSKILLDUCKPOWER, 100 );
}
#endif

#ifdef _TYPE_TOXICATION
void CHAR_CHAT_DEBUG_Toxication( int charaindex, char *message)
{
	int fd = getfdFromCharaIndex( charaindex );
	if( strstr( message, "TRUE") != 0){
		CHAR_talkToCli( charaindex, -1, "中毒", CHAR_COLORYELLOW);
		setToxication( fd, 1);
	}else	{
		setToxication( fd, 0);
	}

}
#endif

void CHAR_CHAT_DEBUG_showMem(int charaindex, char *message)
{
	char bufarg[256];
	showMem( bufarg);
	CHAR_talkToCli( charaindex, -1, bufarg, CHAR_COLORRED);
}

void CHAR_CHAT_DEBUG_cleanfreepet(int charaindex, char *message)
{	
	int objindex, delobjnum=0;
	int objmaxnum = OBJECT_getNum();

	for( objindex=0; objindex<objmaxnum; objindex++)	{
		int nums;
		if( CHECKOBJECT( objindex ) == FALSE ) continue;
		nums = PET_CleanPetdeletetime( objindex);
		delobjnum += nums;
	}
	if( delobjnum > 0 ){
		char buf[256];
		sprintf( buf, "清除%d  置宠物。", delobjnum);
		CHAR_talkToCli( charaindex, -1, buf, CHAR_COLORRED);
	}
}

#ifdef _SEND_EFFECT	   	        // WON ADD AC送下雪、下雨等特效
void CHAR_CHAT_DEBUG_sendeffect( int charaindex, char *message)
{
	int i, j, effect, level;
	char buf[256];
	struct tm tm1;
	int playernum = CHAR_getPlayerMaxNum();
	int		player;
	int		efnum = 0;

	struct {
		int floor;
		int effect;
		BOOL on;
		BOOL off;
	} ef[2048];


	memcpy( &tm1, localtime( (time_t *)&NowTime.tv_sec), sizeof( tm1));

	memset( buf, 0, sizeof( buf));

	if( getStringFromIndexWithDelim( message, " ", 1, buf, sizeof( buf)) == FALSE ) return;
	effect = atoi( buf );			
	
	if( getStringFromIndexWithDelim( message, " ", 2, buf, sizeof( buf)) == FALSE ) return;
	level =	atoi( buf );

	for( i=0; i<=CHAR_effectnum; i++){
		if(CHAR_effect[i].floor){
			CHAR_effect[i].effect = effect;
			CHAR_effect[i].level = level;
			CHAR_effect[i].sendflg = FALSE;
			snprintf( CHAR_effect[i].month  , sizeof( CHAR_effect[i].month) , "%d" , tm1.tm_mon+1);
			snprintf( CHAR_effect[i].day    , sizeof( CHAR_effect[i].day)   , "%d" , tm1.tm_mday);
			snprintf( CHAR_effect[i].hour   , sizeof( CHAR_effect[i].hour)  , "%d" , tm1.tm_hour);
			snprintf( CHAR_effect[i].min    , sizeof( CHAR_effect[i].min)   , "%d" , tm1.tm_min);
			snprintf( CHAR_effect[i].expire , sizeof( CHAR_effect[i].expire), "300000" );
		}// if
	}// for i


	memset( &ef, 0, sizeof( ef));

	for( i = 0; i < CHAR_effectnum; i ++){
		if( !CHAR_effect[i].sendflg) {
			for( player = 0 ; player < playernum; player ++ ) {
				if( !CHAR_CHECKINDEX( player ) ) continue;
				if( CHAR_getInt( player, CHAR_FLOOR) == CHAR_effect[i].floor ) {
					int fd = getfdFromCharaIndex( player);
					lssproto_EF_send( fd, CHAR_effect[i].effect, CHAR_effect[i].level, "");
					if( CHAR_effect[i].level == 0 ) {
						CHAR_setWorkInt( player, CHAR_WORKEFFECT, 
										CHAR_getWorkInt( player, CHAR_WORKEFFECT) &
											~CHAR_effect[i].effect);
					}else {					
						CHAR_setWorkInt( player, CHAR_WORKEFFECT, 
										CHAR_getWorkInt( player, CHAR_WORKEFFECT) | 
											CHAR_effect[i].effect);
					}
				}
			}
			CHAR_effect[i].sendflg = TRUE;
		}

		for( j = 0; j < efnum; j ++ ) {
			if( ef[j].floor == CHAR_effect[i].floor && ef[j].effect == CHAR_effect[i].effect) 
			{
				ef[j].on = TRUE;
				break;
			}
		}
		if( j == efnum ) {
			ef[j].floor = CHAR_effect[i].floor;
			ef[j].effect = CHAR_effect[i].effect;
			ef[j].on = TRUE;
			efnum++;
			if( efnum >= arraysizeof( ef)) {
				print( "err buffer over %s:%d\n", __FILE__, __LINE__);
				efnum --;
			} 
		} 
	}   

	for( i = 0; i < efnum; i ++ ) {
		if( ef[i].on == FALSE && ef[i].off == TRUE ) {
			for( player = 0 ; player < playernum; player ++ ) {
				if( !CHAR_CHECKINDEX( player ) ) continue;
				if( CHAR_getInt( player, CHAR_FLOOR) == ef[i].floor ) {
					int fd = getfdFromCharaIndex( player);
					lssproto_EF_send( fd, 0, 0, "");
					CHAR_setWorkInt( player, CHAR_WORKEFFECT, 0);
				}
			}
		}
	}
}
#endif

void CHAR_CHAT_DEBUG_checktime( int charaindex, char *message)
{
	time_t newsec;
	char buf[256];
	char WKday[7][32]={"日","一","二","三","四","五","六"};
	struct tm *p;


	newsec = time( NULL);
	p = localtime( &newsec);
//	newsec = localtime( &newsec);
	sprintf( buf, "今天：%d年%d月%d日 %s %d:%d:%d ...newsec:%ld。",
		(1900+p->tm_year), (1+p->tm_mon), p->tm_mday, WKday[p->tm_wday],
		p->tm_hour, p->tm_min, p->tm_sec, newsec );
	CHAR_talkToCli( charaindex, -1, buf, CHAR_COLORRED);
	//p = (struct tm *)calloc( 1, sizeof( struct tm) );
	//if( p == NULL ) return;
	if( getStringFromIndexWithDelim( message, "/", 1, buf, sizeof( buf)) == FALSE ) return;
	print("ANDY 1.buf:%s\n", buf);
	p->tm_year = atoi( buf)-1900;
	if( getStringFromIndexWithDelim( message, "/", 2, buf, sizeof( buf)) == FALSE ) return;
	print("ANDY 2.buf:%s\n", buf);
	p->tm_mon = atoi( buf)-1;
	if( getStringFromIndexWithDelim( message, "/", 3, buf, sizeof( buf)) == FALSE ) return;
	print("ANDY 3.buf:%s\n", buf);
	p->tm_mday = atoi( buf);
	if( getStringFromIndexWithDelim( message, "/", 4, buf, sizeof( buf)) == FALSE ) return;
	p->tm_wday = atoi( buf);
	if( p->tm_wday < 0 || p->tm_wday >= 7 ) p->tm_wday = 0;
	if( getStringFromIndexWithDelim( message, "/", 5, buf, sizeof( buf)) == FALSE ) return;
	p->tm_hour = atoi( buf);
	if( getStringFromIndexWithDelim( message, "/", 6, buf, sizeof( buf)) == FALSE ) return;
	p->tm_min = atoi( buf);
	if( getStringFromIndexWithDelim( message, "/", 7, buf, sizeof( buf)) == FALSE ) return;
	p->tm_sec =  atoi( buf);

	newsec = mktime( p);
	sprintf( buf, "确认1：%d年%d月%d日 %s %d:%d:%d ...newsec:%ld。",
		(1900+p->tm_year), (1+p->tm_mon), p->tm_mday, WKday[p->tm_wday],
		p->tm_hour, p->tm_min, p->tm_sec, newsec );
	CHAR_talkToCli( charaindex, -1, buf, CHAR_COLORRED);

	p = localtime( &newsec);
	sprintf( buf, "确认2：%d年%d月%d日 %s %d:%d:%d ...newsec:%ld..time:%d。",
		(1900+p->tm_year), (1+p->tm_mon), p->tm_mday, WKday[p->tm_wday],
		p->tm_hour, p->tm_min, p->tm_sec, newsec, (int)time(NULL));
	CHAR_talkToCli( charaindex, -1, buf, CHAR_COLORRED);
}

void CHAR_CHAT_DEBUG_playerspread( int charaindex, char *message)
{
	int i,j;
	int playernum = CHAR_getPlayerMaxNum();
#define CHECKMAPNUMS 100
	struct {
		int floor;
		int nums;
	}MapArray[CHECKMAPNUMS];

	if(strstr( message, "waei") == NULL ) return;
	for( i=0; i<CHECKMAPNUMS; i++ ){
		MapArray[i].floor = -1;
		MapArray[i].nums = 0;
	}
	for( i=0; i<playernum; i++ ){
		int Myfloor, finds=0;
		if( CHAR_getCharUse( i) == FALSE ) continue;
		Myfloor = CHAR_getInt( i, CHAR_FLOOR);
		for( j=0; j<CHECKMAPNUMS; j++){
			if( MapArray[j].floor == -1 ) break;
			if( MapArray[j].floor == Myfloor ){
				MapArray[j].nums++;
				finds=1;
				break;
			}
		}
		if( finds == 0 && j < CHECKMAPNUMS ){
			MapArray[j].floor=Myfloor;
			MapArray[j].nums++;
		}else if( finds == 0 ){
			print( "ANDY MapArray full!!\n");
		}
	}
	print("\n");
	for( i=0; i<CHECKMAPNUMS; i++ ){
		if( MapArray[i].floor == -1 ) break;
		print( "ANDY MapArray[%d:%s]-%d\n",
			MapArray[i].floor,
			MAP_getFloorName( MapArray[i].floor), MapArray[i].nums );
	}
}

void CHAR_CHAT_DEBUG_samecode( int charaindex, char *message)
{
	char buf1[256], buf2[256];
	char cmd[3][256]={
		"pet", "item", "set"
	};
	int type=-1, j, total = 0;
	BOOL checkn = FALSE;
	int MAXPLAYER = CHAR_getPlayerMaxNum();
	int MAXITEM = ITEM_getITEM_itemnum();
	int MAXPET = CHAR_getPetMaxNum();

	if( getStringFromIndexWithDelim( message, " ", 1, buf1, sizeof( buf1) ) == FALSE )return;
	for( j=0; j<3; j++){
		if( strstr( buf1, cmd[j]) != NULL ){
			type = j;
			break;
		}
	}
	if( j >= 3 ){
		CHAR_talkToCli( charaindex, -1, "参数错误！", CHAR_COLORRED);
		return;
	}

	switch( type){
	case 0:
		if( getStringFromIndexWithDelim( message, " ", 2, buf1, sizeof( buf1) ) == TRUE ){
			checkn = TRUE;
		}
		for( j=MAXPLAYER; j<MAXPLAYER+MAXPET; j++ ){
			if( !CHAR_CHECKINDEX( j) ) continue;
			if( CHAR_getWorkInt( j, CHAR_WORKPLAYERINDEX) != 100000 ) continue;
			if( checkn == TRUE && strstr( CHAR_getUseName( j), buf1) != NULL ){
				sprintf( buf2, "Detain PET:%s[%s]-%s LV:%d",
					CHAR_getUseName( j), CHAR_getChar( j, CHAR_NAME),
					CHAR_getChar( j, CHAR_UNIQUECODE), CHAR_getInt( j, CHAR_LV) );
				CHAR_talkToCli( charaindex, -1, buf2, CHAR_COLORYELLOW);
				total++;
			}
		}
		sprintf( buf2, "total DetainPet:%d", total);
		CHAR_talkToCli( charaindex, -1, buf2, CHAR_COLORYELLOW);
		break;
	case 1:
		for( j=0; j<MAXITEM; j++ ){
			if( !ITEM_CHECKINDEX( j) ) continue;
			if( CHAR_getWorkInt( j, ITEM_WORKCHARAINDEX) != 100000 ) continue;

			sprintf( buf2, "Detain Item:%s-%s",
				ITEM_getChar( j, ITEM_NAME), ITEM_getChar( j, ITEM_UNIQUECODE) );
			CHAR_talkToCli( charaindex, -1, buf2, CHAR_COLORYELLOW);
					total++;
		}
		sprintf( buf2, "total DetainItem:%d", total);
		CHAR_talkToCli( charaindex, -1, buf2, CHAR_COLORYELLOW);
		break;
	case 2:
		if( getStringFromIndexWithDelim( message, " ", 2, buf1, sizeof( buf1) ) == TRUE ){
			for( j=0; j<=CHAR_MAXITEMHAVE; j++){
				int itemindex = CHAR_getItemIndex( charaindex , j );
				if( !ITEM_CHECKINDEX( itemindex) ) continue;
				ITEM_setChar( itemindex, ITEM_UNIQUECODE, buf1);
			}
			for( j=0; j<CHAR_MAXPETHAVE; j++){
    			int petindex = CHAR_getCharPet( charaindex, j);
				if( !CHAR_CHECKINDEX( petindex) ) continue;
				CHAR_setChar( petindex, CHAR_UNIQUECODE, buf1);
			}
		}
		break;
	}


}

// Robin 2001/04/30
void CHAR_CHAT_DEBUG_silent(int charindex ,char *message)
{
     char token[100];
     char token1[100];
     char cdkey[CDKEYLEN];
     char buf[256];
     char line[256];
     unsigned int MAX_USER=0,i;
     //BOOL flg;
     int minu;
     easyGetTokenFromString( message , 1 , token, sizeof(token));
     easyGetTokenFromString( message , 2 , token1, sizeof(token1));
     if( (strlen(message)==0) || (strlen(token1)==0) ){
         sprintf(buf,"%s","参数不正确--> 帐号 分钟");
         CHAR_talkToCli( charindex, -1,buf, CHAR_COLORWHITE);
         return;
     }
     //flg = isstring1or0( token1);
     minu = atoi( token1 );
     MAX_USER=getFdnum();
     for(i=0;i<MAX_USER;i++){
         int i_use;
         i_use=CONNECT_getUse(i);
         if(i_use){
             CONNECT_getCdkey( i, cdkey, sizeof( cdkey ));
             //print("\n<shut_up>--token=%s",token);
             if(strcmp(token,cdkey)==0){
             	int index = CONNECT_getCharaindex( i );
                //print("\n<shut_up>--find-->cdkey %s",cdkey);
                
                //CONNECT_set_shutup(i,TRUE);
                
                // Robin
                CHAR_setInt(index,CHAR_SILENT, minu*60);
                //CHAR_setInt(index,CHAR_SILENT, (int)NowTime.tv_sec +(minu*60) );
                {
                CHAR_setWorkInt( index, CHAR_WORKLOGINTIME, (int)NowTime.tv_sec);
                }
                //print(" set_silent:%s:%dmin ", cdkey, minu);
                
                //print("\n<Set TRUE");
#ifdef _GM_KING
                snprintf( line, sizeof( line),
               		"%s大魔王将 %s 封嘴%d分钟。",CHAR_getChar(index,CHAR_NAME),
                	CHAR_getChar( index, CHAR_NAME),
                	minu );
#else
                snprintf( line, sizeof( line),
               		"极度大魔王将 %s 封嘴%d分钟。",
                	CHAR_getChar( index, CHAR_NAME),
                	minu );
#endif
                CHAR_talkToCli( charindex, -1,line, CHAR_COLORWHITE);
                CHAR_talkToCli( index, -1,line, CHAR_COLORWHITE);
             }
          }//if use
       }//for
}

void CHAR_CHAT_DEBUG_help( int charindex, char *message)
{
	char token[256], buf[256];
	char buf1[256], buf2[256]; 
	int i, GmLevel=0, type= 0;
	int MaxMagicNum = CHAR_getChatMagicFuncMaxNum();

	getStringFromIndexWithDelim( message, " ", 1, buf, sizeof( buf) );
	if( ( strlen( message) == 0 ) || ( strlen( buf) == 0 ) ){
		strcpy( buf, "[help 指令/all]");
		CHAR_talkToCli( charindex, -1, buf, CHAR_COLORYELLOW);
		return;
	}

	if( !strcmp( buf, "all") ) type = 1;
	strcpy( token, "可用GM指令:");
	GmLevel = CHAR_getWorkInt( charindex, CHAR_WORKGMLEVEL);

	for( i=0; i<MaxMagicNum; i++){
		if( CHAR_getChatMagicFuncNameAndString( i, buf1, buf2, GmLevel, TRUE) != 1 ) continue;
		if( type == 1 ){
			sprintf( buf2, ", [%s ]", buf1);
			if( strlen( token) >= 220 ){
				CHAR_talkToCli( charindex, -1, token, CHAR_COLORYELLOW);	
				strcpy( token, "可用GM指令:");
			}
			strcat( token, buf2);
			continue;
		}else{
			if( strstr( buf1, buf) != NULL || 
				strstr( buf2, buf) != NULL ){
				sprintf( token, "GM指令:[%s %s]", buf1, buf2);
				CHAR_talkToCli( charindex, -1, token, CHAR_COLORYELLOW);
			}
			continue;
		}
	}
	if( type == 1 )
		CHAR_talkToCli( charindex, -1, token, CHAR_COLORYELLOW);	
}

#ifdef _EQUIT_NEGLECTGUARD
void CHAR_CHAT_DEBUG_setneguard( int charaindex, char* message )
{
	int num = atoi( message);
	CHAR_setWorkInt(  charaindex, CHAR_WORKNEGLECTGUARD, num);
}
#endif

void CHAR_CHAT_DEBUG_itemreload(int charindex ,char *message)
{
	print("Reading itemset File...");
	CHAR_talkToCli( charindex, -1, "重新道具资料...", CHAR_COLORYELLOW);

	if(!ITEM_readItemConfFile( getItemfile()) ) {
		CHAR_talkToCli( charindex, -1, "读取失败", CHAR_COLORYELLOW);
	}
	print("done\n");
	CHAR_talkToCli( charindex, -1, "读取完成", CHAR_COLORYELLOW);
}


void CHAR_CHAT_DEBUG_skywalker(int charaindex ,char *message)
{
	if( !CHAR_CHECKINDEX( charaindex) )
		return;

	if( strstr( message, "on") != NULL )	{
		CHAR_talkToCli( charaindex, -1, "天行者模式启动", CHAR_COLORYELLOW);
		CHAR_setWorkInt( charaindex, CHAR_WORKSKYWALKER, TRUE);
	}
	if( strstr( message, "off") != NULL )	{
		CHAR_talkToCli( charaindex, -1, "天行者模式关闭", CHAR_COLORYELLOW);
		CHAR_setWorkInt( charaindex, CHAR_WORKSKYWALKER, FALSE);
	}

	CHAR_send_P_StatusString( charaindex, CHAR_P_STRING_SKYWALKER);
}

#ifdef _ITEM_ADDEXP // 显示智果效果
void CHAR_CHAT_DEBUG_itemaddexp(int charaindex ,char *message)
{
	char msg[1024];
	int toindex = -1;

	if( !CHAR_CHECKINDEX( charaindex) )
		return;

	if( !strcmp( message, "") ) {
		toindex = charaindex;
	}
	else {
		int maxchara, i;
		maxchara = CHAR_getPlayerMaxNum();
		for( i=0; i<maxchara; i++ ) {
			if( !CHAR_CHECKINDEX( i) ) continue;
			if( CHAR_getInt( i, CHAR_WHICHTYPE ) != CHAR_TYPEPLAYER) continue;

			if( !strcmp( CHAR_getChar( i, CHAR_CDKEY), message) ) {
				toindex = i;
				break;
			}
		}
		if( toindex == -1 ) {
			CHAR_talkToCli( charaindex,-1, "查无此人", CHAR_COLORRED);
			return;
		}
	}

	sprintf( msg, "智慧之果效果  %s　目前分数%d  %d％  时间%d分。",
			CHAR_getChar( toindex, CHAR_NAME),
			CHAR_getWorkInt( toindex, CHAR_WORKITEM_ADDEXP)
					* (int)(CHAR_getWorkInt( toindex, CHAR_WORKITEM_ADDEXPTIME)/60),
			CHAR_getWorkInt( toindex, CHAR_WORKITEM_ADDEXP),
			(int)(CHAR_getWorkInt( toindex, CHAR_WORKITEM_ADDEXPTIME)/60) );

	CHAR_talkToCli( charaindex,-1, msg, CHAR_COLORRED);
}
#endif

#ifdef _DEF_GETYOU 
void CHAR_CHAT_DEBUG_getyou(int charaindex,char *message)
{
	int floor,x,y,i,j,fd,area;
	char cdkey[CDKEYLEN],areabuf[5];
	char szName[256],buf[256];
    OBJECT  object;
	char line[256];
	int o,chara_index;
	int getnum = 0;

	getStringFromIndexWithDelim( message, " ", 1, areabuf , sizeof( areabuf ));
	area = atoi(areabuf);
	if( area <= 0 || area > 3 ){
        sprintf(buf,"%s","没有输入  围或  围超出限制，请输入1~3");
        CHAR_talkToCli( charaindex, -1, buf, CHAR_COLORYELLOW);
        return;
	}
	floor=CHAR_getInt( charaindex, CHAR_FLOOR );
    x=CHAR_getInt( charaindex, CHAR_X );
    y=CHAR_getInt( charaindex, CHAR_Y );
	for(i=y-area;i<=y+area;i++){
		for(j=x-area;j<=x+area;j++){
			for( object = MAP_getTopObj(floor,j,i) ; object ;object = NEXT_OBJECT(object ) ){
				o = GET_OBJINDEX(object);
				if( OBJECT_getType(o) == OBJTYPE_CHARA ){
					chara_index=OBJECT_getIndex(o);
					if( CHAR_getInt( chara_index, CHAR_WHICHTYPE) != CHAR_TYPEPLAYER ) continue;
					if( chara_index == charaindex ) continue;  //找到自己了 
					if( ( fd = getfdFromCharaIndex(chara_index) ) != -1 ){				
						getnum++;
						CONNECT_getCharname( fd, szName, sizeof( szName ) );  //人物名称
						CONNECT_getCdkey( fd, cdkey, sizeof( cdkey ));		  //帐号	
						//CHAR_getChar( chara_index, CHAR_NAME);  
						snprintf( line, sizeof( line), "第%d个找到[%s]的帐号是[%s]",getnum,szName,cdkey);
                        CHAR_talkToCli( charaindex, -1,line, CHAR_COLORRED);
						print("第%d个找到[%s]的帐号是[%s]\n",getnum,szName,cdkey);					
					}
				}
			}
		}
	}
	if(!getnum) CHAR_talkToCli( charaindex, -1, "  围内没有人", CHAR_COLORYELLOW);
}
#endif

#ifdef _DEF_NEWSEND
void CHAR_CHAT_DEBUG_newsend(int charindex ,char *message)
{
    char buf[256];
	char token[100];
	char szName[256];
	char cdkey[CDKEYLEN];
	char line[256];
	char say[128];
	int fl,x,y,i;
	unsigned int MAX_USER=0;
	MAX_USER=getFdnum();
	if( strlen(message)==0){
		sprintf(buf,"%s","参数不正确-> FLOOR X Y 无/要说的话");
		CHAR_talkToCli( charindex, -1,buf, CHAR_COLORWHITE);
		return;
	}
	easyGetTokenFromString( message , 1 , token, sizeof(token));
	if( strlen(token)==0){
		sprintf(buf,"%s","参数不正确-->地图编号");
		CHAR_talkToCli( charindex, -1,buf, CHAR_COLORWHITE);
		return;
	}
	fl=atoi( token );
	easyGetTokenFromString( message , 2 , token, sizeof(token));
	if( strlen(token)==0){
		sprintf(buf,"%s","参数不正确-->X座标 ");
		CHAR_talkToCli( charindex, -1,buf, CHAR_COLORWHITE);
		return;
	}
	x = atoi( token );
	easyGetTokenFromString( message , 3 , token, sizeof(token));
	if( strlen(token)==0){
		sprintf(buf,"%s","参数不正确-->Y座标 ");
		CHAR_talkToCli( charindex, -1,buf, CHAR_COLORWHITE);
		return;
	}
	y = atoi( token );
	easyGetTokenFromString( message , 4 , token, sizeof(token));
	if( strlen(token)==0){
		sprintf(buf,"%s","参数不正确-->帐号");
		CHAR_talkToCli( charindex, -1,buf, CHAR_COLORWHITE);
		return;
	}
	easyGetTokenFromString( message , 5 , say , sizeof(say));

	for(i=0;i<MAX_USER;i++){                                                                               
		int i_use,fd_charaindex;
		i_use=CONNECT_getUse(i);
		if(i_use){
			CONNECT_getCharname( i, szName, sizeof( szName ) );
			CONNECT_getCdkey( i, cdkey, sizeof( cdkey ));
			if(strcmp(token,cdkey)==0){
				fd_charaindex = CONNECT_getCharaindex( i );
				CHAR_warpToSpecificPoint(fd_charaindex, fl, x, y);
				snprintf( line, sizeof( line),
						 "把 name:%s account=%s 传送到 FL=%d X=%d Y=%d say:%s",
						  szName,cdkey,fl,x,y,say);
				CHAR_talkToCli( charindex, -1 , line , CHAR_COLORWHITE );
				if(strlen(say)){ 
					CHAR_talkToCli( fd_charaindex, -1 , say, CHAR_COLORYELLOW );
				}
			}                                  
        }
	}
}	
#endif 

#ifdef _DEF_SUPERSEND
void CHAR_CHAT_DEBUG_supersend(int charaindex ,char *message)
{
    char buf[256];
	char token[100];
	char name[CHARNAMELEN],szName[CHARNAMELEN];
	char cdkey[CDKEYLEN],cdkeytoken[CDKEYLEN];
	char line[256];
	char say[128];
	int tofl,toX,toY,i,j,k,area;
    OBJECT  object;
	int fl,x,y,fd,fd_charaindex; //帐号所在位置
	unsigned int MAX_USER=0;
	int sendnum = 0;
	int o,chara_index;


	if( strlen(message)==0){
		sprintf(buf,"%s","参数不正确-> FLOOR X Y 帐号 格数 无/要说的话");
		CHAR_talkToCli( charaindex, -1,buf, CHAR_COLORWHITE);
		return;
	}
	easyGetTokenFromString( message , 1 , token, sizeof(token));
	if( strlen(token)==0){
		sprintf(buf,"%s","参数不正确-->地图编号");
		CHAR_talkToCli( charaindex, -1,buf, CHAR_COLORWHITE);
		return;
	}
	tofl=atoi( token );
	easyGetTokenFromString( message , 2 , token, sizeof(token));
	if( strlen(token)==0){
		sprintf(buf,"%s","参数不正确-->X座标 ");
		CHAR_talkToCli( charaindex, -1,buf, CHAR_COLORWHITE);
		return;
	}
	toX = atoi( token );
	easyGetTokenFromString( message , 3 , token, sizeof(token));
	if( strlen(token)==0){
		sprintf(buf,"%s","参数不正确-->Y座标 ");
		CHAR_talkToCli( charaindex, -1,buf, CHAR_COLORWHITE);
		return;
	}
	toY = atoi( token );
	easyGetTokenFromString( message , 4 , cdkeytoken , sizeof(cdkeytoken));
	if( strlen(cdkeytoken)==0){
		sprintf(buf,"%s","参数不正确-->帐号");
		CHAR_talkToCli( charaindex, -1,buf, CHAR_COLORWHITE);
		return;
	}
	easyGetTokenFromString( message , 5 , token, sizeof(token));
	area = atoi( token );
	if( strlen(token)==0 ||	area <= 0 || area > 3 ){
		sprintf(buf,"%s","参数不正确-->格数(1~3)");
		CHAR_talkToCli( charaindex, -1,buf, CHAR_COLORWHITE);
		return;
	}
	easyGetTokenFromString( message , 6 , say , sizeof(say));  //说话

	MAX_USER=getFdnum();
    for(k=0;k<MAX_USER;k++){
		int i_use;
        i_use=CONNECT_getUse(k);
        if(i_use){
			CONNECT_getCdkey( k, cdkey, sizeof( cdkey ));
            if(strcmp(cdkey,cdkeytoken)==0){
				fd_charaindex = CONNECT_getCharaindex( k );
				CONNECT_getCharname( k , name, sizeof( name ) );
				fl=CHAR_getInt( fd_charaindex, CHAR_FLOOR );
				x=CHAR_getInt( fd_charaindex, CHAR_X );
				y=CHAR_getInt( fd_charaindex, CHAR_Y );
				for(i=y-area;i<=y+area;i++){
					for(j=x-area;j<=x+area;j++){
						object = MAP_getTopObj(fl,j,i); 
						while(object){
							o = GET_OBJINDEX(object);
							if( OBJECT_getType(o) == OBJTYPE_CHARA ){
								chara_index=OBJECT_getIndex(o);
								if( CHAR_getInt( chara_index, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER ){
									//if( chara_index == charaindex ) continue;  //找到自己了 
									if( ( fd = getfdFromCharaIndex(chara_index) ) != -1 ){				
										sendnum++;
										CONNECT_getCharname( fd, szName, sizeof( szName ) );  //人物名称
										CHAR_warpToSpecificPoint( chara_index, tofl, toX, toY);
										snprintf(line,sizeof( line),"第%d个传送[%s]",sendnum,szName);
										CHAR_talkToCli( charaindex, -1,line, CHAR_COLORWHITE);
										if(strlen(say))
											CHAR_talkToCli( chara_index, -1 , say , CHAR_COLORYELLOW );										
										object = MAP_getTopObj(fl,j,i);
										continue;
									}
								}
							}
							object = NEXT_OBJECT(object);
						}
					}
				}
				if(sendnum<=1) CHAR_talkToCli( charaindex, -1, "  围内没有别的玩家", CHAR_COLORYELLOW);
				snprintf( line, sizeof( line),"把 name:%s account=%s 周围%d格内的玩家共%d个传送到 FL=%d X=%d Y=%d",
													  name,cdkey,area,sendnum,tofl,toX,toY);
				CHAR_talkToCli( charaindex, -1 , line , CHAR_COLORYELLOW );
				break;
			}
		}
	}
}
#endif

#ifdef _CREATE_MM_1_2
void CHAR_CHAT_DEBUG_MM( int charaindex , char *message )
{
	int	ret;
	int	enemynum;
	int	enemyid;
	int index;
	int	i,j;
	int work[4]={50,50,50,50};
	int LevelUpPoint;
	char buf[10];
	char cdkey[CDKEYLEN];
	char token[128];
	int playernum = CHAR_getPlayerMaxNum();
	easyGetTokenFromString( message , 1 , buf, sizeof(buf));
	index=atoi(buf);
	easyGetTokenFromString( message , 2 , cdkey, sizeof(cdkey));
	if(index==1)enemyid=1479;
	else if(index==2)enemyid=2547;
	else{
		CHAR_talkToCli( charaindex, -1, "目前只能制作1/2代玛蕾菲雅!", CHAR_COLORYELLOW );
		return;}
		
	if(strlen(cdkey)>0){
		for(j=0;j<playernum;j++){
			if(CHAR_CHECKINDEX(j) == FALSE) continue;
			if(!strcmp( CHAR_getChar( j, CHAR_CDKEY), cdkey))
			{
				break;
			}
		}
		if (j>=playernum)
		{
		 	CHAR_talkToCli( charaindex, -1, "此账号不在线~", CHAR_COLORYELLOW );
			return;
		}
		enemynum = ENEMY_getEnemyNum();
		for( i = 0; i < enemynum; i ++ ) {
			if( ENEMY_getInt( i, ENEMY_ID) == enemyid) {
				break;
			}
		}
		if( i == enemynum )return;
	
		ret = ENEMY_createPetFromEnemyIndex( j, i);
		if( !CHAR_CHECKINDEX( ret))return;
		snprintf( token,sizeof( token), "制作极品%d代玛蕾菲雅成功!",index);
		CHAR_talkToCli( charaindex, -1, token,  CHAR_COLORWHITE);
		sprintf( token, "[GM]%s制作了只极品%d代玛蕾菲雅给你!", CHAR_getChar( charaindex, CHAR_NAME),index);
		CHAR_talkToCli( j, -1, token, CHAR_COLORYELLOW );
		sprintf( token, "给玩家%s制作了只极品%d代玛蕾菲雅!", CHAR_getChar( j, CHAR_NAME),index);
		CHAR_talkToCli( charaindex, -1,token, CHAR_COLORYELLOW );
		for( i = 0; i < CHAR_MAXPETHAVE; i ++ ){
			if( CHAR_getCharPet( j, i ) == ret )break;
		}
		if( i == CHAR_MAXPETHAVE )return;

		CHAR_setInt(ret,CHAR_VARIABLEAI,10000);
		snprintf( token, sizeof( token ), "K%d", i );
		CHAR_sendStatusString( j, token );
		snprintf( token, sizeof( token ), "W%d", i );
		CHAR_sendStatusString( j, token );
		int k;
		LevelUpPoint = ( work[3]<< 24) + ( work[0]<< 16) + ( work[1]<< 8) + ( work[2]<< 0);
		CHAR_setInt( ret, CHAR_ALLOCPOINT, LevelUpPoint);
		for( k = 1; k < 79; k ++ ){	//升级
			LevelUpPoint = ( work[3]<< 24) + ( work[0]<< 16) + ( work[1]<< 8) + ( work[2]<< 0);
			CHAR_setInt( ret, CHAR_ALLOCPOINT, LevelUpPoint);
			CHAR_PetLevelUp( ret );
			CHAR_PetAddVariableAi( ret, AI_FIX_PETLEVELUP );
			CHAR_setInt( ret , CHAR_HP ,CHAR_getWorkInt( ret, CHAR_WORKMAXHP ) );
			CHAR_setInt( ret , CHAR_MP ,CHAR_getWorkInt( ret, CHAR_WORKMAXMP ) );
			CHAR_complianceParameter( ret );
			snprintf( token, sizeof( token ), "K%d", i );
			CHAR_sendStatusString( j, token );
			snprintf( token, sizeof( token ), "W%d", i );
			CHAR_sendStatusString( j, token );
		}
		CHAR_setInt( ret , CHAR_HP ,CHAR_getWorkInt( ret, CHAR_WORKMAXHP ) );
		CHAR_setInt( ret , CHAR_MP ,CHAR_getWorkInt( ret, CHAR_WORKMAXMP ) );
		CHAR_complianceParameter( ret );
		snprintf( token, sizeof( token ), "K%d", i );
		CHAR_sendStatusString( j, token );
		snprintf( token, sizeof( token ), "W%d", i );
		CHAR_sendStatusString( j, token );
	}else{
		enemynum = ENEMY_getEnemyNum();
		for( i = 0; i < enemynum; i ++ ) {
			if( ENEMY_getInt( i, ENEMY_ID) == enemyid) {
				break;
			}
		}
		if( i == enemynum )return;
	
		ret = ENEMY_createPetFromEnemyIndex(charaindex, i);
		if( !CHAR_CHECKINDEX( ret))return;
		snprintf( token,sizeof( token), "制作极品%d代玛蕾菲雅成功!",index);
		CHAR_talkToCli( charaindex, -1, token,  CHAR_COLORWHITE);
	
		for( i = 0; i < CHAR_MAXPETHAVE; i ++ ){
			if( CHAR_getCharPet( charaindex, i ) == ret )break;
		}
		if( i == CHAR_MAXPETHAVE )return;

		CHAR_setInt(ret,CHAR_VARIABLEAI,10000);
		int k;
		LevelUpPoint = ( work[3]<< 24) + ( work[0]<< 16) + ( work[1]<< 8) + ( work[2]<< 0);
		CHAR_setInt( ret, CHAR_ALLOCPOINT, LevelUpPoint);
		for( k = 1; k < 79; k ++ ){	//升级
			LevelUpPoint = ( work[3]<< 24) + ( work[0]<< 16) + ( work[1]<< 8) + ( work[2]<< 0);
			CHAR_setInt( ret, CHAR_ALLOCPOINT, LevelUpPoint);
			CHAR_PetLevelUp( ret );
			CHAR_PetAddVariableAi( ret, AI_FIX_PETLEVELUP );
			CHAR_setInt( ret, CHAR_LV, CHAR_getInt( ret, CHAR_LV) +1 );
			CHAR_setInt( ret , CHAR_HP ,CHAR_getWorkInt( ret, CHAR_WORKMAXHP ) );
			CHAR_setInt( ret , CHAR_MP ,CHAR_getWorkInt( ret, CHAR_WORKMAXMP ) );
			CHAR_complianceParameter( ret );
			snprintf( token, sizeof( token ), "K%d", i );
			CHAR_sendStatusString( charaindex, token );
			snprintf( token, sizeof( token ), "W%d", i );
			CHAR_sendStatusString( charaindex, token );
		}
		CHAR_setInt( ret , CHAR_HP ,CHAR_getWorkInt( ret, CHAR_WORKMAXHP ) );
		CHAR_setInt( ret , CHAR_MP ,CHAR_getWorkInt( ret, CHAR_WORKMAXMP ) );
		CHAR_complianceParameter( ret );
		snprintf( token, sizeof( token ), "K%d", i );
		CHAR_sendStatusString( charaindex, token );
		snprintf( token, sizeof( token ), "W%d", i );
		CHAR_sendStatusString( charaindex, token );
	}
	printf("%d\n",CHAR_getInt( ret, CHAR_HP ));
}
#endif

#ifdef _SendTo
void CHAR_CHAT_DEBUG_Sendto( int charaindex , char *message )
{
	char cdkey[CDKEYLEN];
	int i;
	int playernum = CHAR_getPlayerMaxNum();
	easyGetTokenFromString( message , 1 , cdkey, sizeof(cdkey));
	if(strlen(cdkey)>0){
		for(i=0;i<playernum;i++){
			if(CHAR_CHECKINDEX(i) == FALSE) continue;
			if(!strcmp( CHAR_getChar( i, CHAR_CDKEY), cdkey))
			{
				break;
			}
		}
		if (i>=playernum)
		{
		 	CHAR_talkToCli( charaindex, -1, "此账号不在线~", CHAR_COLORYELLOW );
			return;
		}
    if(!strcmp(CHAR_getChar(i,CHAR_CDKEY),cdkey))
    {
       CHAR_DischargePartyNoMsg( charaindex);//解散团队
       int floor, x, y;
       char token[256];
       floor = CHAR_getInt( i, CHAR_FLOOR);
       x = CHAR_getInt( i, CHAR_X);
       y = CHAR_getInt( i, CHAR_Y);

       CHAR_warpToSpecificPoint( charaindex, floor, x, y );
       sprintf( token, "已传送到玩家%s身边！", CHAR_getChar( i, CHAR_NAME));
       CHAR_talkToCli( charaindex, -1, token, CHAR_COLORYELLOW );
       return;
     }
   }
}
#endif

#ifdef _GM_ITEM
void CHAR_CHAT_DEBUG_GMFUNCTION( int charindex , char *message )
{
	int i;
	char gmtime[4];
	char gmfunction[16];
	char cdkey[CDKEYLEN];
	char token[128];
	int playernum = CHAR_getPlayerMaxNum();
	easyGetTokenFromString( message , 1 , gmfunction, sizeof(gmfunction));
	easyGetTokenFromString( message , 2 , gmtime, sizeof(gmtime));
	easyGetTokenFromString( message , 3 , cdkey, sizeof(cdkey));
	if(strlen(cdkey)>0){
		for(i=0;i<playernum;i++){
			if(CHAR_CHECKINDEX(i) == FALSE) continue;
			if(!strcmp( CHAR_getChar( i, CHAR_CDKEY), cdkey))
			{
				break;
			}
		}
		if (i>=playernum)
		{
		 	CHAR_talkToCli( charindex, -1, "此账号不在线~", CHAR_COLORYELLOW );
			return;
		}
		CHAR_setChar( i , CHAR_GMFUNCTION, gmfunction );
		CHAR_setInt( i , CHAR_GMTIME, atoi( gmtime ) );
		sprintf( token, "[GM]%s给你使用%s权限%d次!",CHAR_getChar( charindex, CHAR_NAME), gmfunction, atoi( gmtime ));
		CHAR_talkToCli( i, -1, token, CHAR_COLORYELLOW );
		sprintf( token, "相关使用请查看[help %s]无需GM密码!",CHAR_getChar( charindex, CHAR_GMFUNCTION));
		CHAR_talkToCli( i, -1, token, CHAR_COLORYELLOW );
		sprintf( token, "给玩家%s使用%s权限%d次!", CHAR_getChar( i, CHAR_NAME), gmfunction, atoi( gmtime ));
		CHAR_talkToCli( charindex, -1,token, CHAR_COLORYELLOW );
	}else{
		CHAR_setChar( charindex , CHAR_GMFUNCTION, gmfunction );
		CHAR_setInt( charindex , CHAR_GMTIME, atoi( gmtime ) );
		sprintf( token, "设置使用%s权限%d次!", gmfunction,atoi( gmtime ));
		CHAR_talkToCli( charindex, -1,token, CHAR_COLORYELLOW );
	}
}
#endif

#ifdef _GM_RIDE
void CHAR_CHAT_DEBUG_SETRIDE( int charindex , char *message )
{
	int i,rideindex;
	char buf[16];
	char cdkey[CDKEYLEN];
	char token[128];
	int playernum = CHAR_getPlayerMaxNum();
	easyGetTokenFromString( message , 1 , buf, sizeof(buf));
	rideindex=atoi(buf);
	easyGetTokenFromString( message , 2 , cdkey, sizeof(cdkey));
	if(rideindex<1 || rideindex>12)
	{
		CHAR_talkToCli( charindex, -1,"骑宠编号必须为1~12", CHAR_COLORRED );
		return;
	}
	typedef struct{
		char name[CHARNAMELEN];
		int Code;
	}tagNewRideCode;
	tagNewRideCode NewRides[12]={
		{ "帖拉所伊朵", RIDE_PET0},
		{ "玛恩摩洛斯", RIDE_PET1}, 
		{ "朵拉比斯", RIDE_PET2},
		{ "拉奇鲁哥", RIDE_PET3}, 
		{ "扬奇洛斯", RIDE_PET4}, 
		{ "卡达鲁卡斯", RIDE_PET5},
		{ "卡卡金宝", RIDE_PET6}, 
		{ "格尔格", RIDE_PET7}, 
		{ "左迪洛斯", RIDE_PET8},
		{ "巴朵兰恩", RIDE_PET9}, 
		{ "史卡鲁", RIDE_PET10}, 
		{ "罗多克雷", RIDE_PET11}
	};
	if(strlen(cdkey)>0){
		for(i=0;i<playernum;i++){
			if(CHAR_CHECKINDEX(i) == FALSE) continue;
			if(!strcmp( CHAR_getChar( i, CHAR_CDKEY), cdkey))
			{
				break;
			}
		}
		if (i>=playernum)
		{
		 	CHAR_talkToCli( charindex, -1, "此账号不在线~", CHAR_COLORYELLOW );
			return;
		}
		int LRCode = CHAR_getInt( i, CHAR_LOWRIDEPETS);
		
		LRCode = LRCode|NewRides[rideindex-1].Code;
		CHAR_setInt( i, CHAR_LOWRIDEPETS, LRCode);
		
		sprintf( token, "[GM]%s给你骑乘骑宠%s的资格!",CHAR_getChar( charindex, CHAR_NAME), NewRides[rideindex-1].name);
		CHAR_talkToCli( i, -1, token, CHAR_COLORYELLOW );
		sprintf( token, "给玩家%s骑乘骑宠%s的资格!", CHAR_getChar( i, CHAR_NAME), NewRides[rideindex-1].name);
		CHAR_talkToCli( charindex, -1,token, CHAR_COLORYELLOW );
	}else{
		int LRCode = CHAR_getInt( charindex, CHAR_LOWRIDEPETS);
		LRCode = LRCode|NewRides[rideindex-1].Code;
		CHAR_setInt( charindex, CHAR_LOWRIDEPETS, LRCode);
		
		sprintf( token, "设置骑乘骑宠%s的资格!", NewRides[rideindex-1].name);
		CHAR_talkToCli( charindex, -1,token, CHAR_COLORYELLOW );
	}
	CHAR_sendStatusString( charindex, "x");
}

void CHAR_CHAT_DEBUG_MVRIDE( int charindex , char *message )
{
	int i,rideindex;
	char buf[16];
	char cdkey[CDKEYLEN];
	char token[128];
	int playernum = CHAR_getPlayerMaxNum();
	easyGetTokenFromString( message , 1 , buf, sizeof(buf));
	rideindex=atoi(buf);
	easyGetTokenFromString( message , 2 , cdkey, sizeof(cdkey));
	if(rideindex<1 || rideindex>12)
	{
		CHAR_talkToCli( charindex, -1,"骑宠编号必须为1~12", CHAR_COLORRED );
		return;
	}
	typedef struct{
		char name[CHARNAMELEN];
		int Code;
	}tagNewRideCode;
	tagNewRideCode NewRides[]={
		{ "帖拉所伊朵", RIDE_PET0},
		{ "玛恩摩洛斯", RIDE_PET1}, 
		{ "朵拉比斯", RIDE_PET2},
		{ "拉奇鲁哥", RIDE_PET3}, 
		{ "扬奇洛斯", RIDE_PET4}, 
		{ "卡达鲁卡斯", RIDE_PET5},
		{ "卡卡金宝", RIDE_PET6}, 
		{ "格尔格", RIDE_PET7}, 
		{ "左迪洛斯", RIDE_PET8},
		{ "巴朵兰恩", RIDE_PET9}, 
		{ "史卡鲁", RIDE_PET10}, 
		{ "罗多克雷", RIDE_PET11}
	};
	if(strlen(cdkey)>0){
		for(i=0;i<playernum;i++){
			if(CHAR_CHECKINDEX(i) == FALSE) continue;
			if(!strcmp( CHAR_getChar( i, CHAR_CDKEY), cdkey))
			{
				break;
			}
		}
		if (i>=playernum)
		{
		 	CHAR_talkToCli( charindex, -1, "此账号不在线~", CHAR_COLORYELLOW );
			return;
		}
		int LRCode = CHAR_getInt( i, CHAR_LOWRIDEPETS);
		
		LRCode = LRCode^NewRides[rideindex-1].Code;
		CHAR_setInt( i, CHAR_LOWRIDEPETS, LRCode);
		
		sprintf( token, "[GM]%s移除你骑乘骑宠%s的资格!",CHAR_getChar( charindex, CHAR_NAME), NewRides[rideindex-1].name);
		CHAR_talkToCli( i, -1, token, CHAR_COLORYELLOW );
		sprintf( token, "移除玩家%s骑乘骑宠%s的资格!!", CHAR_getChar( i, CHAR_NAME), NewRides[rideindex-1].name);
		CHAR_talkToCli( charindex, -1,token, CHAR_COLORYELLOW );
	}else{
		int LRCode = CHAR_getInt( charindex, CHAR_LOWRIDEPETS);
		LRCode = LRCode^NewRides[rideindex-1].Code;
		CHAR_setInt( charindex, CHAR_LOWRIDEPETS, LRCode);
		
		sprintf( token, "移除骑乘骑宠%s的资格!", NewRides[rideindex-1].name);
		CHAR_talkToCli( charindex, -1,token, CHAR_COLORYELLOW );
	}
	CHAR_sendStatusString( charindex, "x");
}
#endif

#ifdef _LOCK_IP
void CHAR_CHAT_DEBUG_ADD_LOCK( int charindex , char *message )
{
	int flag;
	char buf[16];
	char cdkey[16]="";
	char IP[16]="";
	easyGetTokenFromString( message , 1 , buf, sizeof(buf));
	flag=atoi(buf);
	if(flag<=1){
		easyGetTokenFromString( message , 2 , cdkey, sizeof(cdkey));
	}else if(flag==2){
		easyGetTokenFromString( message , 2 , IP, sizeof(IP));
	}else{
		easyGetTokenFromString( message , 2 , cdkey, sizeof(cdkey));
		easyGetTokenFromString( message , 3 , IP, sizeof(IP));
	}
	saacproto_LockLogin_send(acfd, cdkey, IP, 1);
}

void CHAR_CHAT_DEBUG_DEL_LOCK( int charindex , char *message )
{
	int flag;
	char buf[16];
	char cdkey[16]="";
	char IP[16]="";
	easyGetTokenFromString( message , 1 , buf, sizeof(buf));
	flag=atoi(buf);
	if(flag<=1){
		easyGetTokenFromString( message , 2 , cdkey, sizeof(cdkey));
	}else if(flag==2){
		easyGetTokenFromString( message , 2 , IP, sizeof(IP));
	}else{
		easyGetTokenFromString( message , 2 , cdkey, sizeof(cdkey));
		easyGetTokenFromString( message , 3 , IP, sizeof(IP));
	}
	saacproto_LockLogin_send(acfd, cdkey, IP, 0);
}

void CHAR_CHAT_DEBUG_SHOWIP( int charindex , char *message )
{
	int MAX_USER = getFdnum();
	int i;
	char token[64];
	char cdkey[CDKEYLEN];
	if(strlen(message)>0){
		for(i=0;i<MAX_USER;i++){
      int i_use;
      i_use=CONNECT_getUse(i);
      if(i_use){
				CONNECT_getCdkey( i, cdkey, sizeof( cdkey ));
				if(!strcmp( cdkey, message))
				{
					break;
				}
			}
		}
		if (i>=MAX_USER)
		{
		 	CHAR_talkToCli( charindex, -1, "此账号不在线~", CHAR_COLORYELLOW );
			return;
		}

		unsigned long ip;
		int a,b,c,d;
		 // Arminius: limit ip
	  ip=CONNECT_get_userip(i);
	   
	  a=(ip % 0x100); ip=ip / 0x100;
	  b=(ip % 0x100); ip=ip / 0x100;
	  c=(ip % 0x100); ip=ip / 0x100;
	  d=(ip % 0x100);
		sprintf( token, "账号%s的IP是 %d.%d.%d.%d",message, a,b,c,d);
		CHAR_talkToCli( charindex, -1,token, CHAR_COLORYELLOW );
	}
}
#endif

void CHAR_CHAT_DEBUG_SET_FAME( int charaindex, char* message )
{
	char szMsg[32],szBuf[16],buf[128],cdkey[CDKEYLEN];
	int i,id,fd,iPlayerNum = CHAR_getPlayerMaxNum();

	getStringFromIndexWithDelim(message," ",1,cdkey,sizeof(cdkey));
	getStringFromIndexWithDelim(message," ",2,szBuf,sizeof(szBuf));
	id = atoi(szBuf);
	for(i=0;i<iPlayerNum;i++){
		if(CHAR_getCharUse(i) != FALSE){
			if(!strcmp(CHAR_getChar(i,CHAR_CDKEY),cdkey)) break;
		}
	}
	if(i >= iPlayerNum){
		CHAR_talkToCli(charaindex,-1,"找不到这名玩家",CHAR_COLORYELLOW);
		return;
	}
	CHAR_setInt(i,CHAR_FAME,id);
	sprintf(szMsg,"目前声望值: %d",CHAR_getInt(i,CHAR_FAME)/100);
	CHAR_talkToCli(charaindex,-1,szMsg,CHAR_COLORYELLOW);
	sprintf(buf,"%d",id);
	fd = getfdFromCharaIndex(i);
	saacproto_ACFixFMData_send(acfd,
		CHAR_getChar(i,CHAR_FMNAME),CHAR_getInt(i,CHAR_FMINDEX),CHAR_getWorkInt(i,CHAR_WORKFMINDEXI),
		FM_FIX_FMFEED,buf,"",CHAR_getWorkInt(i,CHAR_WORKFMCHARINDEX),CONNECT_getFdid(fd));
	return;
}

#ifdef _AUTO_PK
void CHAR_CHAT_DEBUG_SET_AUTOPK( int charaindex, char* message )
{
	char szMsg[256];
	sprintf(szMsg,"已设置PK时间为%d分钟!",atoi(message));
	CHAR_talkToCli(charaindex,-1,szMsg,CHAR_COLORYELLOW);
	AutoPk_PKTimeSet(atoi(message));
	AutoPk_GetAwardStr();
}
#endif

#ifdef _PLAYER_NUM
void CHAR_CHAT_DEBUG_SET_PLAYERNUM( int charaindex, char* message )
{
	setPlayerNum(atoi(message));
}
#endif

#ifdef _RELOAD_CF
void CHAR_CHAT_DEBUG_SET_RELOADCF( int charaindex, char* message )
{
	if(readconfigfile(getConfigfilename()))
		CHAR_talkToCli(charaindex,-1,"OK",CHAR_COLORYELLOW);
}
#endif

#ifdef _TRANS
void CHAR_CHAT_DEBUG_Trans( int charaindex, char *message)
{
	int i;
	int Trans;
	char cdkey[CDKEYLEN];
	char token[128];
	int vital, str, tgh, dex;
	int quest , level ,equ;
	int t_quest, t_level;
	int total,ans;
	int work[10];
	float tmp,lvup;
	int playernum = CHAR_getPlayerMaxNum();
	easyGetTokenFromString( message , 1 , cdkey, sizeof(cdkey));
	if(strlen(cdkey)>0){
		for(i=0;i<playernum;i++){
			if(CHAR_CHECKINDEX(i) == FALSE) continue;
			if(!strcmp( CHAR_getChar( i, CHAR_CDKEY), cdkey))
			{
				break;
			}
		}
		if (i>=playernum)
		{
		 	CHAR_talkToCli( charaindex, -1, "此账号不在线~", CHAR_COLORYELLOW );
			return;
		}
		Trans=CHAR_getInt(i,CHAR_TRANSMIGRATION)+1;
		if(Trans>7 || CHAR_getInt(i,CHAR_LV)<80){
			sprintf( token, "由于你已七转或等级小于80级，所以[GM]%s为你转生失败!", CHAR_getChar( charaindex, CHAR_NAME));
			CHAR_talkToCli( i, -1, token, CHAR_COLORYELLOW );
			sprintf( token, "由于玩家%s已七转或等级小于80级，所以%d转生失败!", CHAR_getChar( i, CHAR_NAME),Trans);
			CHAR_talkToCli( charaindex, -1,token, CHAR_COLORYELLOW );
			return;
		}else{
			CHAR_sendCToArroundCharacter( CHAR_getWorkInt( i, CHAR_WORKOBJINDEX ));
			sprintf( token, "[GM]%s为你转生%d转!", CHAR_getChar( charaindex, CHAR_NAME),Trans);
			CHAR_talkToCli( i, -1, token, CHAR_COLORYELLOW );
			sprintf( token, "为玩家%s转生为%d转!", CHAR_getChar( i, CHAR_NAME),Trans);
			CHAR_talkToCli( charaindex, -1,token, CHAR_COLORYELLOW );
			charaindex=i;
		}
	}else{
		Trans=CHAR_getInt(charaindex,CHAR_TRANSMIGRATION)+1;
		if(Trans>7 || CHAR_getInt(charaindex,CHAR_LV)<80){
			sprintf( token, "由于你已七转或等级小于80级，所以转生失败!", CHAR_getChar( charaindex, CHAR_NAME));
			CHAR_talkToCli( charaindex, -1, token, CHAR_COLORYELLOW );
			return;
		}else{
			CHAR_sendCToArroundCharacter( CHAR_getWorkInt( charaindex, CHAR_WORKOBJINDEX ));
			sprintf( token, "为自己转生为%d转!",Trans);
			CHAR_talkToCli( charaindex, -1,token, CHAR_COLORYELLOW );
		}
	}
#ifdef _ADD_POOL_ITEM			   // WON ADD 增加可寄放的道具	
	int tran_pool_item[5] = { 4, 4, 4, 4, 4 };		// 增加的道具寄放数
	int tran_pool_pet[5] = { 2, 2, 2, 2, 2};		// 增加的宠物寄放数
	int j,item_sum=0, pet_sum=0;

	for(j=0; j<5 ;j++){
		item_sum += tran_pool_item[j] ;			
		pet_sum += tran_pool_pet[j];		
	}

	item_sum += 10;								// 限制最大的寄道具数
	pet_sum  += 5;							    // 限制最大的寄宠数

#endif
	NPC_TransmigrationStatus(charaindex, charaindex, work);
	NPC_TransmigrationFlg_CLS(charaindex, charaindex);
	CHAR_setInt(charaindex ,CHAR_TRANSMIGRATION, Trans);
	CHAR_setInt(charaindex, CHAR_LV ,1);
	CHAR_setMaxExp( charaindex, 0);
  CHAR_setInt( charaindex,CHAR_SKILLUPPOINT,CHAR_getInt( charaindex, CHAR_TRANSMIGRATION)*10);
  CHAR_Skillupsend( charaindex );
  CHAR_setInt( charaindex,CHAR_RIDEPET, -1 );
  CHAR_setInt( charaindex , CHAR_BASEIMAGENUMBER , CHAR_getInt( charaindex , CHAR_BASEBASEIMAGENUMBER) );
	CHAR_sendStatusString( charaindex , "P");
}
#endif

#ifdef _MAKE_PET_CF
void CHAR_CHAT_DEBUG_petmakecf( int charaindex, char* message )
{
	int	enemynum;
	int	enemyid;
	int	i,j;
	int level=-1;
	int trans=0;
	float chengzhang=0.0;
	char buf[10],buf1[10];
	char cdkey[CDKEYLEN];
	char token[128];
	int playernum = CHAR_getPlayerMaxNum();
	easyGetTokenFromString( message , 1 , buf, sizeof(buf));
	enemyid=atoi(buf);
	easyGetTokenFromString( message , 2 , buf, sizeof(buf));
	getStringFromIndexWithDelim(buf,"/",1,buf1,sizeof(buf1));
	chengzhang=atof(buf1)-1.0;
	if(chengzhang>9.0)chengzhang=9.0;
	getStringFromIndexWithDelim(buf,"/",2,buf1,sizeof(buf1));
	level=atoi(buf1);
	getStringFromIndexWithDelim(buf,"/",3,buf1,sizeof(buf1));
	trans=atoi(buf1);
	easyGetTokenFromString( message , 3 , cdkey, sizeof(cdkey));
	
	if(strlen(cdkey)>0){
		for(j=0;j<playernum;j++){
			if(CHAR_CHECKINDEX(j) == FALSE) continue;
			if(!strcmp( CHAR_getChar( j, CHAR_CDKEY), cdkey))
			{
				break;
			}
		}
		if (j>=playernum)
		{
		 	CHAR_talkToCli( charaindex, -1, "此账号不在线~", CHAR_COLORYELLOW );
			return;
		}
		enemynum = ENEMY_getEnemyNum();
		for( i = 0; i < enemynum; i ++ ) {
			if( ENEMY_getInt( i, ENEMY_ID) == enemyid) {
				break;
			}
		}
		if( i == enemynum )return;
	
		int petindex = ENEMY_createPetFromEnemyIndex( j, i);
		if( !CHAR_CHECKINDEX( petindex))return;
		CHAR_setInt(petindex,CHAR_VARIABLEAI,10000);
		sprintf( token, "[GM]%s制作了%s给你!", CHAR_getChar( charaindex, CHAR_NAME),ENEMY_getChar( i, ENEMY_NAME));
		CHAR_talkToCli( j, -1, token, CHAR_COLORYELLOW );
		sprintf( token, "给玩家%s的%s制作成功!", CHAR_getChar( j, CHAR_NAME),ENEMY_getChar( i, ENEMY_NAME));
		CHAR_talkToCli( charaindex, -1,token, CHAR_COLORYELLOW );
		for( i = 0; i < CHAR_MAXPETHAVE; i ++ ){
			if( CHAR_getCharPet( j, i ) == petindex )break;
		}
		if( i == CHAR_MAXPETHAVE )return;
	
		if( CHAR_CHECKINDEX( petindex ) == TRUE ){
			CHAR_setMaxExpFromLevel( petindex, CHAR_getInt(petindex, CHAR_LV ));
		}
		
		
		{
			
			int vital=CHAR_getInt( petindex, CHAR_VITAL);
			int str=CHAR_getInt( petindex, CHAR_STR);
			int tough=CHAR_getInt( petindex, CHAR_TOUGH);
			int dex=CHAR_getInt( petindex, CHAR_DEX);
			int lv=CHAR_getInt( petindex, CHAR_LV);
			int LevelUpPoint = CHAR_getInt( petindex, CHAR_ALLOCPOINT );
			int work[4];
			float fRand, cvitl, cstr, ctough, cdex;
			float rnd=(rand()%5+95)/100.0;
			float Param[4] = { 0.0, 0.0, 0.0, 0.0 };
			work[3] =(( LevelUpPoint >> 24 ) & 0xFF);
			work[0] = (( LevelUpPoint >> 16 ) & 0xFF);
			work[1] = (( LevelUpPoint >> 8 ) & 0xFF);
			work[2] = (( LevelUpPoint >> 0 ) & 0xFF);
			struct _RankRandTbl{
				int min;
				int max;
			}RankRandTbl[] = {
				{ 450, 500 },
				{ 470, 520 },
				{ 490, 540 },
				{ 510, 560 },
				{ 530, 580 },
				{ 550, 600 },
			};
			int petrank = CHAR_getInt( petindex, CHAR_PETRANK );
			if( petrank < 0 || petrank > 5 ) petrank = 0;
				
			int i;
			for(i=0;i<160;i++){
				fRand = (float)RAND( RankRandTbl[petrank].min, RankRandTbl[petrank].max ) * 0.01;
				cvitl += (float)work[3] * fRand + Param[0] * fRand;
				cstr += (float)work[0] * fRand + Param[1] * fRand;
				ctough += (float)work[1] * fRand + Param[2] * fRand;
				cdex += (float)work[2] * fRand + Param[3] * fRand;
			}

			rnd=((cstr-str)/80.0/160.0+(ctough-tough)/80.0/160.0+(cdex-dex)/100.0/160.0);

			work[3]*=(chengzhang/rnd);
			work[0]*=(chengzhang/rnd);
			work[1]*=(chengzhang/rnd);
			work[2]*=(chengzhang/rnd);
	
			LevelUpPoint = ( work[3]<< 24) + ( work[0]<< 16) + ( work[1]<< 8) + ( work[2]<< 0);
			CHAR_setInt( petindex, CHAR_ALLOCPOINT, LevelUpPoint);
			
			CHAR_setInt( petindex, CHAR_VITAL, vital);
			CHAR_setInt( petindex, CHAR_STR, str);
			CHAR_setInt( petindex, CHAR_TOUGH, tough);
			CHAR_setInt( petindex, CHAR_DEX, dex);
			
		}
		
		if( level != -1 ){
#ifdef _TRANS_LEVEL_CF
			if( level > getMaxLevel() ) level = getMaxLevel();
#else
			if( level > 140 ) level = 140;
#endif
			if( level < 0 ) level = 0;
			level = level - CHAR_getInt( petindex, CHAR_LV );
			if( level > 0 ){
				int k;
				for( k = 1; k <= level; k ++ ){	//升级
					CHAR_PetLevelUp( petindex );
					CHAR_PetAddVariableAi( petindex, AI_FIX_PETLEVELUP );
					CHAR_setInt( petindex, CHAR_LV, CHAR_getInt( petindex, CHAR_LV) +1 );
				}
			}
		}
		CHAR_setInt( i, CHAR_HP, CHAR_getWorkInt( i, CHAR_WORKMAXHP ));
		CHAR_complianceParameter( petindex );
		snprintf( token, sizeof( token ), "K%d", i );
		CHAR_sendStatusString( j, token );
		snprintf( token, sizeof( token ), "W%d", i );
		CHAR_sendStatusString( j, token );
	}else{
		enemynum = ENEMY_getEnemyNum();
		for( i = 0; i < enemynum; i ++ ) {
			if( ENEMY_getInt( i, ENEMY_ID) == enemyid) {
				break;
			}
		}
		if( i == enemynum )return;
	
		int petindex = ENEMY_createPetFromEnemyIndex(charaindex, i);
		if( !CHAR_CHECKINDEX( petindex))return;
		CHAR_setInt(petindex,CHAR_VARIABLEAI,10000);
		snprintf( token,sizeof( token), "%s制作成功!",ENEMY_getChar( i, ENEMY_NAME));
		CHAR_talkToCli( charaindex, -1, token,  CHAR_COLORWHITE);
	
		for( i = 0; i < CHAR_MAXPETHAVE; i ++ ){
			if( CHAR_getCharPet( charaindex, i ) == petindex )break;
		}
		if( i == CHAR_MAXPETHAVE )return;
	
		if( CHAR_CHECKINDEX( petindex ) == TRUE ){
			CHAR_setMaxExpFromLevel( petindex, CHAR_getInt( petindex, CHAR_LV ));
		}
		
		{
			
			int vital=CHAR_getInt( petindex, CHAR_VITAL);
			int str=CHAR_getInt( petindex, CHAR_STR);
			int tough=CHAR_getInt( petindex, CHAR_TOUGH);
			int dex=CHAR_getInt( petindex, CHAR_DEX);
			int lv=CHAR_getInt( petindex, CHAR_LV);
			int LevelUpPoint = CHAR_getInt( petindex, CHAR_ALLOCPOINT );
			int work[4];
			float fRand=0.0, cvitl=0.0, cstr=0.0, ctough=0.0, cdex=0.0;
			float rnd=0.0;
			float Param[4] = { 0.0, 0.0, 0.0, 0.0 };
			work[3] =(( LevelUpPoint >> 24 ) & 0xFF);
			work[0] = (( LevelUpPoint >> 16 ) & 0xFF);
			work[1] = (( LevelUpPoint >> 8 ) & 0xFF);
			work[2] = (( LevelUpPoint >> 0 ) & 0xFF);
			struct _RankRandTbl{
				int min;
				int max;
			}RankRandTbl[] = {
				{ 450, 500 },
				{ 470, 520 },
				{ 490, 540 },
				{ 510, 560 },
				{ 530, 580 },
				{ 550, 600 },
			};
			int petrank = CHAR_getInt( petindex, CHAR_PETRANK );
			if( petrank < 0 || petrank > 5 ) petrank = 0;
				
			int i;
			for(i=0;i<160;i++){
				fRand = (float)RAND( RankRandTbl[petrank].min, RankRandTbl[petrank].max ) * 0.01;
				cvitl += (float)work[3] * fRand + Param[0] * fRand;
				cstr += (float)work[0] * fRand + Param[1] * fRand;
				ctough += (float)work[1] * fRand + Param[2] * fRand;
				cdex += (float)work[2] * fRand + Param[3] * fRand;
			}

			rnd=((cstr-(float)str)/80.0/160.0+(ctough-(float)tough)/80.0/160.0+(cdex-(float)dex)/100.0/160.0);

			printf("chengzhang:%f,chengzhang:%f\n",chengzhang,rnd);

			work[3]*=(chengzhang/rnd);
			work[0]*=(chengzhang/rnd);
			work[1]*=(chengzhang/rnd);
			work[2]*=(chengzhang/rnd);
	
			LevelUpPoint = ( work[3]<< 24) + ( work[0]<< 16) + ( work[1]<< 8) + ( work[2]<< 0);
			CHAR_setInt( petindex, CHAR_ALLOCPOINT, LevelUpPoint);
			
			CHAR_setInt( petindex, CHAR_VITAL, vital);
			CHAR_setInt( petindex, CHAR_STR, str);
			CHAR_setInt( petindex, CHAR_TOUGH, tough);
			CHAR_setInt( petindex, CHAR_DEX, dex);
			
		}
		if( level != -1 ){
			if( level > 200 )level = 200;
			if( level < 0 ) level = 0;
			level = level - CHAR_getInt( petindex, CHAR_LV );
				if( level > 0 ){
				int k;
				for( k = 1; k <= level; k ++ ){	//升级
					CHAR_PetLevelUp( petindex );
					CHAR_PetAddVariableAi( petindex, AI_FIX_PETLEVELUP );
					CHAR_setInt( petindex, CHAR_LV, CHAR_getInt( petindex, CHAR_LV) +1 );
				}
			}		
		}
		CHAR_setInt( petindex, CHAR_TRANSMIGRATION, trans);
		CHAR_setInt( petindex, CHAR_HP, CHAR_getWorkInt( petindex, CHAR_WORKMAXHP ));
		CHAR_complianceParameter( petindex );
		snprintf( token, sizeof( token ), "K%d", i );
		CHAR_sendStatusString( charaindex, token );
		snprintf( token, sizeof( token ), "W%d", i );
		CHAR_sendStatusString( charaindex, token );
	}
}
#endif

#ifdef _MAKE_PET_ABILITY
void CHAR_CHAT_DEBUG_petmakeabi( int charaindex, char* message )
{
	int	ret;
	int	enemynum;
	int	enemyid;
	int	i,j;
	char buf[10];
	int vital,str,tough,dex;
	char token[128];
	int playernum = CHAR_getPlayerMaxNum();
	easyGetTokenFromString( message , 1 , buf, sizeof(buf));
	enemyid=atoi(buf);
	easyGetTokenFromString( message , 2 , buf, sizeof(buf));
	vital=atoi(buf);
	easyGetTokenFromString( message , 3 , buf, sizeof(buf));
	str=atoi(buf);
	easyGetTokenFromString( message , 4 , buf, sizeof(buf));
	tough=atoi(buf);
	easyGetTokenFromString( message , 5 , buf, sizeof(buf));
	dex=atoi(buf);
	
	enemynum = ENEMY_getEnemyNum();
	for( i = 0; i < enemynum; i ++ ) {
		if( ENEMY_getInt( i, ENEMY_ID) == enemyid) {
			break;
		}
	}
	if( i == enemynum )return;

	ret = ENEMY_createPetFromEnemyIndex(charaindex, i);
	if( !CHAR_CHECKINDEX( ret))return;
	
	snprintf( token,sizeof( token), "%s制作成功!",ENEMY_getChar( i, ENEMY_NAME));
	CHAR_talkToCli( charaindex, -1, token,  CHAR_COLORWHITE);

	for( i = 0; i < CHAR_MAXPETHAVE; i ++ ){
		if( CHAR_getCharPet( charaindex, i ) == ret )break;
	}
	if( i == CHAR_MAXPETHAVE )return;

	if( CHAR_CHECKINDEX( ret ) == TRUE ){
		CHAR_setInt( ret, CHAR_VITAL, vital);
		CHAR_setInt( ret, CHAR_STR, str*80);
		CHAR_setInt( ret, CHAR_TOUGH, tough*80);
		CHAR_setInt( ret, CHAR_DEX, dex*100);
		CHAR_setMaxExpFromLevel( ret, CHAR_getInt( ret, CHAR_LV ));
		CHAR_setInt(ret,CHAR_VARIABLEAI,10000);
	}
	CHAR_setInt( ret, CHAR_HP, CHAR_getWorkInt( ret, CHAR_WORKMAXHP ));
	CHAR_complianceParameter( ret );
	snprintf( token, sizeof( token ), "K%d", i );
	CHAR_sendStatusString( charaindex, token );
	snprintf( token, sizeof( token ), "W%d", i );
	CHAR_sendStatusString( charaindex, token );
}

#endif

