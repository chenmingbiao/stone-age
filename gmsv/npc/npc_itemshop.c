#include "version.h"
#include <string.h>
#include "char.h"
#include "object.h"
#include "char_base.h"
#include "npcutil.h"
#include "lssproto_serv.h"
#include "pet_skill.h"
#include "npc_eventaction.h"
#include "readmap.h"
#include "log.h"
#include "family.h"

#define MAXSHOPITEM 33
static void NPC_ItemShop_selectWindow( int meindex, int talker, int num,int select);
void NPC_ItemShop_BuyMain(int meindex,int talker,int before );
void NPC_GetItemList(char *argstr,char * argtoken2);
void NPC_ItemStrStr(int itemID,double rate,char *name,char *token2);

BOOL NPC_SetNewItem(int meindex,int talker,char *data);
BOOL NPC_SellNewItem(int meindex,int talker,char *data);

void NPC_ItemShop_Menu(int meindex,int talker);

int NPC_GetLimtItemList(int talker,char *argstr,char *token2,int sell);

void NPC_ItemShop_SellMain(int meindex,int talker,int select);
int NPC_GetSellItemList(int itemindex,int flg,char *argstr,char *argtoken,int select,int sell);
BOOL NPC_AddItemBuy(int meindex, int talker,int itemID,int kosuu,double rate);
int NPC_SellItemstrsStr(int itemindex,int flg,double rate,char *argtoken,int select,int sell);
void NPC_LimitItemShop(int meindex,int talker,int select);
void NPC_ExpressmanCheck(int meindex,int talker);

enum{
	NPC_SHOP_WORK_NO 		= CHAR_NPCWORKINT1,
	NPC_SHOP_WORK_EV 		= CHAR_NPCWORKINT2,
	NPC_SHOP_WORK_EXPRESS	= CHAR_NPCWORKINT3,
};

		  

typedef struct {
	char	arg[32];
	int		type;
}NPC_Shop;


static NPC_Shop		TypeTable[] = {
	{ "FIST",		0 }, //拳
	{ "AXE",		1 }, //斧
	{ "CLUB",		2 }, //棍棒
	{ "SPEAR",		3},  //矛
	{ "BOW",		4},  //弓
	{ "SHIELD",		5},  //盾
	{ "HELM",		6 }, //头盔
	{ "ARMOUR",		7 }, //盔甲
	{ "BRACELET",	8},  //手镯
	{ "ANCLET",		9 }, //踝饰
	{ "NECKLACE",	10}, //项链
	{ "RING",		11}, //戒指
	{ "BELT",		12}, //腰带
	{ "EARRING",	13}, //耳环
	{ "NOSERING",	14}, //鼻环
	{ "AMULET",		15}, //护身符
	{ "OTHER",		16}, 
	{ "BOOMERANG",	17}, //回力标
	{ "BOUNDTHROW",	18}, 
	{ "BREAKTHROW",	19}, //投掷物
	{ "ACCESSORY",	30}, //配件
	{ "OFFENCE",	40}, //攻击
	{ "DEFENCE",	50}, //防御

};

BOOL NPC_ItemShopInit( int meindex )
{

	char	argstr[NPC_UTIL_GETARGSTR_BUFSIZE];

	CHAR_setInt( meindex , CHAR_WHICHTYPE , CHAR_TYPEITEMSHOP );
	if(NPC_Util_GetArgStr( meindex, argstr, sizeof(argstr)) == NULL) {
		print("GetArgStrErr");
		return FALSE;
	}
	if(strstr(argstr,"LIMITSHOP") != NULL) {
		CHAR_setWorkInt( meindex, NPC_SHOP_WORK_NO, 1);
	}else{
		CHAR_setWorkInt( meindex, NPC_SHOP_WORK_NO, 0);
	}
	if(strstr( argstr, "EVENT") != NULL) {
		CHAR_setWorkInt( meindex, NPC_SHOP_WORK_EV, 1);
	}else{
		CHAR_setWorkInt( meindex, NPC_SHOP_WORK_EV, 0);
	}
	if(strstr( argstr, "EXPRESS") != NULL) {
		CHAR_setWorkInt( meindex, NPC_SHOP_WORK_EXPRESS, 1);
	}else{
		CHAR_setWorkInt( meindex, NPC_SHOP_WORK_EXPRESS, 0);
	}
	
	return TRUE;
}

void NPC_ItemShopTalked( int meindex , int talker , char *szMes ,int color )
{
	char	argstr[NPC_UTIL_GETARGSTR_BUFSIZE];
	char	buff[1024];
	char	buf2[256];
	char 	token[1024];
	int 	i = 1;
	BOOL	sellonlyflg = FALSE;
	char	sellmsg[1024];

    if( CHAR_getInt( talker , CHAR_WHICHTYPE ) != CHAR_TYPEPLAYER ) {
    	return;
    }

	if(NPC_Util_isFaceToFace( meindex, talker, 2) == FALSE) {//交谈时检查是否面对面
		if( NPC_Util_CharDistance( talker, meindex ) > 1) return; //若距离大於1跳出
	}

	//取得npc买卖设定档内的内容,若为NULL,则错误
    if(NPC_Util_GetArgStr( meindex, argstr, sizeof( argstr)) == NULL){
    	print("itemshopGetArgStrErr");
    	return;
    }

	//取得NPC只能卖的讯息
	if( NPC_Util_GetStrFromStrWithDelim( argstr, "sellonly_msg", token, sizeof( token))	!= NULL){
		sellonlyflg = TRUE; //NPC不能买玩家的东西
		strcpysafe(sellmsg, sizeof( sellmsg), token);//例如:sellmsg的值可能是->并不是专门收买东西的店。
	}

    //取得玩家买东西的指令. buff为一串买东西指令的字串,例如:买,购买,感谢您,kau,buy,menu,谢谢,买东西,当玩家打出这些字句时,就可以买东西了
	if( NPC_Util_GetStrFromStrWithDelim( argstr, "buy_msg",	buff, sizeof( buff)) != NULL ){
	    while(getStringFromIndexWithDelim(buff,",",i,buf2,sizeof(buf2)) != FALSE ){
			i++; //若买东西的指令有8个,i就会加到8
			if( strstr( szMes, buf2) != NULL) {
				if( CHAR_getWorkInt( meindex, NPC_SHOP_WORK_EV) == 0) {
					if( CHAR_getWorkInt( meindex, NPC_SHOP_WORK_NO) == 1) {
						if( sellonlyflg ) {
							NPC_ItemShop_selectWindow( meindex, talker, 3, -1);
							return;
						}
					}else{
						NPC_ItemShop_selectWindow( meindex, talker, 1, -1);
						return;
					}
				}else{
					if(CHAR_getWorkInt( meindex, NPC_SHOP_WORK_NO) == 1) {
						if( sellonlyflg) {
							NPC_ItemShop_selectWindow( meindex, talker, 3, -1);
							return;
						}
					}else{
						NPC_ItemShop_selectWindow( meindex, talker, 1, -1);
						return;
					}
		 			return;
		 	 	}
			}
		}
	}
	i=1;

    //取得玩家卖东西的指令. buff为一串卖东西指令的字串,例如:卖东西,卖,sell,uru  当玩家打出这些字句时,就可以卖东西了
	if( NPC_Util_GetStrFromStrWithDelim( argstr, "sell_msg", buff, sizeof( buff)) != NULL ){
	    while( getStringFromIndexWithDelim(buff,",", i,buf2,sizeof(buf2)) != FALSE ){
			i++;
			if(strstr(szMes,buf2) != NULL) {
				NPC_ItemShop_selectWindow( meindex, talker, 2, -1);
				return;
			}
		}
	}
	i = 1;

	//其它讯息
	if( NPC_Util_GetStrFromStrWithDelim( argstr, "other_msg", buff, sizeof( buff)) != NULL ){
	    while(getStringFromIndexWithDelim( buff, ",", i, buf2, sizeof( buf2)) != FALSE ){
			i++;
			if(strstr(szMes,buf2) != NULL) {
				if(NPC_Util_GetStrFromStrWithDelim( argstr, "hint_msg", 
				token, sizeof( token)) != NULL)
				{
					CHAR_talkToCli( talker, meindex, token, CHAR_COLORWHITE);
					return;
				}
			}
		}	
	}

	if(CHAR_getWorkInt( meindex, NPC_SHOP_WORK_NO) == 1) {
		if(CHAR_getWorkInt( meindex, NPC_SHOP_WORK_EV) == 1) {
			if( sellonlyflg) {
				CHAR_talkToCli( talker, meindex, sellmsg, CHAR_COLORWHITE);
				return;
			}
		}else{
			if( sellonlyflg) {
				NPC_ItemShop_selectWindow( meindex, talker, 3, -1);
				return;
			}
		}
	}else{
		if(CHAR_getWorkInt( meindex, NPC_SHOP_WORK_EV) == 1) {
			if( sellonlyflg) {
				CHAR_talkToCli( talker, meindex, sellmsg, CHAR_COLORWHITE);
				return;
			}
		}else{			
			if(CHAR_getWorkInt( meindex, NPC_SHOP_WORK_EXPRESS) == 1) {
				NPC_ExpressmanCheck( meindex, talker);
			}else{
				NPC_ItemShop_selectWindow( meindex, talker, 0, -1);
			}
		}
	}
				
}

static void NPC_ItemShop_selectWindow( int meindex, int talker, int num,int select)
{
	switch( num) {
	  case 0:
		CHAR_send_P_StatusString( talker, CHAR_P_STRING_GOLD);//传送金钱,若100元 送出去的资料格式可能为 P8Nz2|100|
		
		if(CHAR_getWorkInt( meindex, NPC_SHOP_WORK_EXPRESS) == 1 ) {
			if(CHAR_getWorkInt( meindex, NPC_SHOP_WORK_NO) ==0 ) {
				NPC_ExpressmanCheck( meindex, talker);//长毛象快递 
			}
		}else if(CHAR_getWorkInt( meindex, NPC_SHOP_WORK_NO) == 1) {
		
		}else{
		  	NPC_ItemShop_Menu( meindex, talker);//选择(买,卖,离开)的小视窗	
		}
	  	break;

	  case 1://进入买视窗
		CHAR_sendStatusString( talker,"I");//传送玩家身上所有的道具给Client
	  	NPC_ItemShop_BuyMain( meindex, talker, select);
	  	break;

	  case 2://进入卖视窗
		CHAR_sendStatusString( talker,"I");
	  	NPC_ItemShop_SellMain( meindex, talker, select);
	  	break;

	  case 3:
	  	NPC_LimitItemShop( meindex, talker, select);
	  	break;

	}
}

void NPC_ItemShopWindowTalked( int meindex, int talkerindex, 
								int seqno, int select, char *data)
{
	if( NPC_Util_CharDistance( talkerindex, meindex ) > 3) {
		CHAR_send_P_StatusString( talkerindex, CHAR_P_STRING_GOLD);
		return;
	}

	makeStringFromEscaped( data);

	switch( seqno){

	  case CHAR_WINDOWTYPE_WINDOWITEMSHOP_STARTMSG:
		if(atoi( data) == 1 )	NPC_ItemShop_selectWindow(meindex, talkerindex, 1, -1);
		if(atoi( data) == 2)	NPC_ItemShop_selectWindow(meindex, talkerindex, 2, -1);
		if(atoi( data) == 3)	return;/*--窒手仄卅中--*/
		break;
	  case CHAR_WINDOWTYPE_WINDOWITEMSHOP_BUY_MSG:
		if(NPC_SetNewItem(meindex , talkerindex, data) == TRUE) {
			NPC_ItemShop_selectWindow( meindex, talkerindex, 1, 0);
		}else{
			NPC_ItemShop_selectWindow( meindex, talkerindex ,0, -1);
		}

		break;
	  case CHAR_WINDOWTYPE_WINDOWITEMSHOP_SELL_MSG:
		if(NPC_SellNewItem(meindex , talkerindex, data) == TRUE) {
			NPC_ItemShop_selectWindow( meindex, talkerindex, 2, 0);

		}else{
			NPC_ItemShop_selectWindow( meindex,  talkerindex, 0, -1);
		}

		break;
			
	  case CHAR_WINDOWTYPE_WINDOWITEMSHOP_LIMIT:
		 if(select == WINDOW_BUTTONTYPE_YES) {
			NPC_ItemShop_selectWindow( meindex, talkerindex ,2, -1);

		}else  if(select == WINDOW_BUTTONTYPE_NO) {
			return;
		}else if(select == WINDOW_BUTTONTYPE_OK) {
				NPC_ItemShop_selectWindow( meindex, talkerindex, 2, -1);
		}
		break;
	
	  case CHAR_WINDOWTYPE_WINDOWITEMSHOP_EXPRESS:
		if(atoi(data) == 2) {
			NPC_ItemShop_selectWindow( meindex, talkerindex, 1, -1);
		}else if(atoi( data) == 4) {
			NPC_ItemShop_selectWindow( meindex, talkerindex, 2, -1);
		}
	}
}

void NPC_ItemShop_BuyMain(int meindex,int talker,int before )
{      
	char argstr[NPC_UTIL_GETARGSTR_BUFSIZE];
	char token[NPC_UTIL_GETARGSTR_BUFSIZE];
	int fd = getfdFromCharaIndex( talker);
    
	char token2[NPC_UTIL_GETARGSTR_BUFSIZE];
	char buff2[256];
  char buff[256];
	if(NPC_Util_GetArgStr( meindex, argstr, sizeof(argstr)) == NULL) {
       	print("itemshop_GetArgStr_Err");
       	return;
    }
	if(before != -1) {
		sprintf(token,"0|0");

		lssproto_WN_send( fd, WINDOW_MESSAGETYPE_ITEMSHOPMAIN, 
				WINDOW_BUTTONTYPE_NONE, 
				CHAR_WINDOWTYPE_WINDOWITEMSHOP_BUY_MSG,
				CHAR_getWorkInt( meindex, CHAR_WORKOBJINDEX),
				token);
	}else{

		NPC_Util_GetStrFromStrWithDelim( argstr, "main_msg", buff, sizeof( buff));
		NPC_Util_GetStrFromStrWithDelim( argstr, "buy_main", buff2, sizeof( buff2));

		sprintf(token,"0|1|%d|%s|%s|%s|", CHAR_WINDOWTYPE_WINDOWITEMSHOP_STARTMSG,
				CHAR_getChar( meindex, CHAR_NAME), buff, buff2);

		NPC_Util_GetStrFromStrWithDelim( argstr, "what_msg", buff, sizeof( buff));
		NPC_Util_GetStrFromStrWithDelim( argstr, "level_msg", buff2, sizeof( buff));

		snprintf( token2, sizeof( token2), "%s|%s", buff, buff2);
	
	    strncat( token, token2, sizeof( token));
			
		NPC_Util_GetStrFromStrWithDelim( argstr, "realy_msg", buff, sizeof( buff));
		NPC_Util_GetStrFromStrWithDelim( argstr, "itemfull_msg", buff2, sizeof( buff2));

		sprintf( token2, "|%s|%s", buff, buff2);
		strncat(token , token2,sizeof(token));
		strcpy(token2, "|");

		NPC_GetItemList( argstr, token2);
		strncat( token, token2, sizeof( token));
	}

	lssproto_WN_send( fd, WINDOW_MESSAGETYPE_ITEMSHOPMAIN, 
				WINDOW_BUTTONTYPE_NONE, 
				CHAR_WINDOWTYPE_WINDOWITEMSHOP_BUY_MSG,
				CHAR_getWorkInt( meindex, CHAR_WORKOBJINDEX),
				token);

}
void NPC_GetItemList(char *argstr,char *argtoken)
{
	int i = 1;
	int tmp;
	char *name ="\0";
	char buff2[256];
	char buff[NPC_UTIL_GETARGSTR_LINEMAX];
	char token2[NPC_UTIL_GETARGSTR_BUFSIZE];
	double rate = 1.0;
	int loopcnt = 0;

	if(NPC_Util_GetStrFromStrWithDelim( argstr, "buy_rate", buff2, sizeof( buff2))
	 != NULL){
		rate = atof( buff2);
	}
	if( NPC_Util_GetStrFromStrWithDelim( argstr, "ItemList", buff, sizeof( buff)) != NULL ){
	    while( getStringFromIndexWithDelim(buff,",",i,buff2,sizeof(buff2)) !=FALSE ) {
			i++;
			if(strstr( buff2, "-") == NULL) {
			 	name = ITEM_getNameFromNumber( atoi(buff2));
				if(name == "\0") continue;
				loopcnt++;
				if(loopcnt == MAXSHOPITEM) break;
				NPC_ItemStrStr( atoi( buff2), rate, name, token2);
	    		strncat( argtoken, token2, sizeof(token2));
			}else{
				int start;
				int end;
				getStringFromIndexWithDelim( buff2, "-", 1, token2, sizeof(token2));
				start = atoi( token2);
				getStringFromIndexWithDelim( buff2, "-", 2 ,token2, sizeof(token2));
				end = atoi( token2);
				if(start > end){
					tmp = start;
					start = end;
					end = tmp;
				}
				end++;
				for(; start < end ; start++ ) {
					/*--引内抩蟆???--*/

				 	name = ITEM_getNameFromNumber( start );
					if(name == "\0") continue;
					loopcnt++;
					if(loopcnt == MAXSHOPITEM) break;
					NPC_ItemStrStr( start, rate, name, token2);
		    		strncat( argtoken, token2, sizeof(token2));
				}
			}
		}
	}
}

void NPC_ItemStrStr(int itemID,double rate,char *name,char *token2)
{

	int gold;
	int level;
	int graNo;
	char info[1024];
	char escape[256];	
	gold  = ITEM_getcostFromITEMtabl( itemID);
	level = ITEM_getlevelFromITEMtabl( itemID);
	graNo = ITEM_getgraNoFromITEMtabl( itemID);
	strcpy(escape,ITEM_getItemInfoFromNumber( itemID));
	gold=(int)(gold * rate);

	makeEscapeString( escape, info, sizeof( info));
	makeEscapeString( name, escape, sizeof( escape));
	sprintf( token2, "%s|0|%d|%d|%d|%s|", escape, level, gold, graNo, info);
}

BOOL NPC_SetNewItem(int meindex,int talker,char *data)
{

	char buf[1024];
	char buff2[128];
	int i = 1, j = 1;
	int select;
	int kosuu = 0;
	char argstr[NPC_UTIL_GETARGSTR_BUFSIZE];
	double rate = 1.0;
	int gold = 0;		
	int kosuucnt = 0;
	int itemindex;

	/*--忒匀化五凶犯□正毛本伊弁玄午蜊醒卞坌荸--*/
	getStringFromIndexWithDelim( data, "|", 1, buf, sizeof( buf));
	select = atoi(buf);
	if(select == 0) return FALSE;
	getStringFromIndexWithDelim( data, "|", 2, buf, sizeof( buf));
	kosuu = atoi(buf);
	if( kosuu <= 0 ) return FALSE;
	
	/*--蜊醒及民尼永弁  癫卞蝈    月井＂-*/
	for( i = CHAR_STARTITEMARRAY ; i < CHAR_MAXITEMHAVE ; i++ ) {
		itemindex = CHAR_getItemIndex( talker , i );
		if( !ITEM_CHECKINDEX( itemindex) ) {
			kosuucnt++;
		}
	}

	/*--忒匀化  凶蜊醒及  互  端及蜊醒  扔□田础  方曰聂中午云井仄中及匹--*/
	/*--扔□田□础及  毛  木月--*/
	if( kosuucnt < kosuu) kosuu = kosuucnt;
		
	/*--未夫及桦宁反巨仿□--*/
	if(kosuucnt == 0 ) return FALSE;

	i = 1;

	/*--云饕及白央奶伙  中坭反白央奶伙互钒仃卅井匀凶午五反蔽  --*/
	if(NPC_Util_GetArgStr( meindex, argstr, sizeof(argstr)) == NULL) {
   	print("shop_GetArgStr_Err");
   	return FALSE;
	}

	/*---伊□玄毛潸    卅仃木壬1.0)-*/
	if(NPC_Util_GetStrFromStrWithDelim( argstr, "buy_rate", buf, sizeof( buf)) != NULL) {
		rate= atof( buf);
	}

	/*--失奶  丞及馨笛毛垫丹午仇欠-*/
	if( NPC_Util_GetStrFromStrWithDelim( argstr, "ItemList", buf, sizeof( buf)) != NULL ){
		while(getStringFromIndexWithDelim(buf , "," , j, buff2, sizeof(buff2)) != FALSE ){
			j++;
			/*--  "-"互殖引木化中月井升丹井--*/
			if(strstr( buff2, "-") == NULL) {
				if( ITEM_getcostFromITEMtabl(atoi(buff2)) !=-1) {
					if ( i == select) {
						/*---失奶  丞及综岳---*/
						/*--蜊醒坌综岳--*/
						if(NPC_AddItemBuy(meindex, talker,atoi(buff2),kosuu,rate) != TRUE)
						{
							return FALSE;
						}
						return TRUE;
					}
					i++;		
				}
			}else{
				/*--失奶  丞互  15-25  及溥匹霜日木凶桦宁--*/
				int start;
				int end;

				/* "-"匹嗉濠日木凶铵户及醒袄午  及醒袄毛潸  --*/
				getStringFromIndexWithDelim( buff2, "-", 1, argstr, sizeof(argstr));
				start = atoi( argstr);
				getStringFromIndexWithDelim( buff2, "-", 2 ,argstr, sizeof(argstr));
				end = atoi( argstr);
				end++;

				/*--  寞互菅卞卅匀化中凶日｝  木赘尹月**/
				if(start > end){
					gold = start;
					start = end;
					end = gold;
				}

				/*--"-"匹嗉濠日木凶坌及失奶  丞毛树  毛  月--*/
				for(; start < end ; start++ ) {
					if( ITEM_getcostFromITEMtabl( start) != -1) {
						if ( i == select) {
							/*---失奶  丞及综岳---*/
							/*--蜊醒坌综岳--*/
							if(NPC_AddItemBuy(meindex, talker, start, kosuu, rate) != TRUE)
							{
								return FALSE;
							}
							return TRUE;
						}
						i++;
					}
				}
			}
		}
	}

	return FALSE;

}

/*---------------------------------------------
 *失奶  丞及馨笛毛垫丹
 *--------------------------------------------*/
BOOL NPC_AddItemBuy(int meindex, int talker,int itemID,int kosuu,double rate)
{

	int itemindex;
	int i;
	int gold;
	int ret;
	int maxgold;
	gold = ITEM_getcostFromITEMtabl( itemID);
	gold = (int)(gold * rate);

	maxgold = gold * kosuu;
	if(CHAR_getInt( talker, CHAR_GOLD) < maxgold ) return FALSE;

	if( addNpcFamilyTax( meindex, talker, maxgold*0.4 ) )
		print(" FamilyTaxDone! ");
	else
		print(" FamilyTaxError!");
	for(i = 0 ; i < kosuu ; i++){
		itemindex = ITEM_makeItemAndRegist( itemID);
		
		if(itemindex == -1) return FALSE;
		ret = CHAR_addItemSpecificItemIndex( talker, itemindex);
		if( ret < 0 || ret >= CHAR_MAXITEMHAVE ) {
			print( "npc_itemshop.c: additem error itemindex[%d]\n", itemindex);
			ITEM_endExistItemsOne( itemindex);
			return FALSE;
		}
		CHAR_sendItemDataOne( talker, ret);
	}
	CHAR_DelGold( talker, maxgold);
	//CHAR_send_P_StatusString( talker, CHAR_P_STRING_GOLD);
	return TRUE;

}

//选择 (买,卖,出去) 的小视窗
void NPC_ItemShop_Menu(int meindex,int talker)
{	
	char	argstr[NPC_UTIL_GETARGSTR_BUFSIZE];
	char	token[NPC_UTIL_GETARGSTR_LINEMAX];
	char buff[256];
	int fd = getfdFromCharaIndex( talker);

    //argstr取得整个设定档的讯息: 例如->buy_rate:1.0|sell_rate:0.2|buy_msg:买,购买,感谢您,kau,buy............. (中间的分格号是读入时加入的)
    if(NPC_Util_GetArgStr( meindex, argstr, sizeof(argstr)) == NULL) {
		print("shop_GetArgStr_Err");
       	return;
    }
    
	//token为视窗上面的title文字  例如: 萨姆吉尔的防具店|欢迎光临
    NPC_Util_GetStrFromStrWithDelim( argstr, "main_msg", buff, sizeof( buff));
	snprintf(token, sizeof(token),"%s|%s",CHAR_getChar( meindex, CHAR_NAME), buff);

	lssproto_WN_send( fd, WINDOW_MESSAGETYPE_ITEMSHOPMENU, 
				WINDOW_BUTTONTYPE_NONE, 
				CHAR_WINDOWTYPE_WINDOWITEMSHOP_STARTMSG,
				CHAR_getWorkInt( meindex, CHAR_WORKOBJINDEX),
				token);

}

void NPC_ItemShop_SellMain(int meindex,int talker,int before)
{
	char	argstr[NPC_UTIL_GETARGSTR_BUFSIZE];
	char	token[NPC_UTIL_GETARGSTR_BUFSIZE];
	int fd = getfdFromCharaIndex( talker);

	//取得npc设定资料
    if(NPC_Util_GetArgStr( meindex, argstr, sizeof(argstr)) == NULL) {
       	print("shop_GetArgStr_Err");
       	return;
    }

	if(before != -1) {
		sprintf(token,"1|0");
		lssproto_WN_send( fd, WINDOW_MESSAGETYPE_ITEMSHOPMAIN
							+CHAR_getWorkInt(meindex,NPC_SHOP_WORK_NO), 
					WINDOW_BUTTONTYPE_NONE, 
					CHAR_WINDOWTYPE_WINDOWITEMSHOP_SELL_MSG,
					CHAR_getWorkInt( meindex, CHAR_WORKOBJINDEX),
					token);
	
	}else{

		char token2[NPC_UTIL_GETARGSTR_BUFSIZE];
		char buff2[256];
	   	char buff[256];

		NPC_Util_GetStrFromStrWithDelim( argstr, "main_msg", buff, sizeof( buff));
		NPC_Util_GetStrFromStrWithDelim( argstr, "sell_main", buff2, sizeof( buff));
		sprintf( token, "1|1|%d|%s|%s|%s|", CHAR_WINDOWTYPE_WINDOWITEMSHOP_STARTMSG,
				CHAR_getChar( meindex, CHAR_NAME), buff, buff2);

		NPC_Util_GetStrFromStrWithDelim( argstr, "stone_msg", buff, sizeof( buff));

		if(CHAR_getWorkInt(meindex,NPC_SHOP_WORK_EXPRESS) == 1 ) {
			NPC_Util_GetStrFromStrWithDelim( argstr, "exrealy_msg", buff2, sizeof(buff2));
		}else{
			NPC_Util_GetStrFromStrWithDelim( argstr, "realy_msg", buff2, sizeof( buff2));
		}
		sprintf( token2,"%s|%s|", buff, buff2);
		NPC_GetLimtItemList( talker,argstr, token2, -1);//详细玩家要卖出的道具资料
		strncat( token, token2, sizeof( token));

		lssproto_WN_send( fd, WINDOW_MESSAGETYPE_ITEMSHOPMAIN+
					CHAR_getWorkInt(meindex,NPC_SHOP_WORK_NO), 
					WINDOW_BUTTONTYPE_NONE, 
					CHAR_WINDOWTYPE_WINDOWITEMSHOP_SELL_MSG,
					CHAR_getWorkInt( meindex, CHAR_WORKOBJINDEX),
					token);

	}
}
int NPC_GetLimtItemList(int talker, char *argstr, char* token2,int sell)
{

	char token[NPC_UTIL_GETARGSTR_LINEMAX];
	char buff[NPC_UTIL_GETARGSTR_LINEMAX];
	char token3[NPC_UTIL_GETARGSTR_LINEMAX];
	int k = 0 , i = 1 , j = 0;
	int imax;
	int itemtype = 0;
	int itemindex;
	int okflg = 0;
	char buf[256];
	int flg=0;
	int cost;

	if(sell == -1 ){
		i = CHAR_STARTITEMARRAY;
		imax = CHAR_MAXITEMHAVE;
		flg = -1;
	}else{
		i= sell;
		imax= sell + 1;
		flg = 1;
	}	

	for( ; i < imax ; i++ ){
		okflg=0;
		itemindex = CHAR_getItemIndex( talker , i );
		
		if( ITEM_CHECKINDEX( itemindex) ){
			if( NPC_Util_GetStrFromStrWithDelim( argstr,"LimitItemType", buff, sizeof( buff)) != NULL ){
				k = 1;
				while(getStringFromIndexWithDelim(buff , "," , k, token, sizeof(token)) != FALSE ){
					k++;
					for(j = 0 ; j < ITEM_CATEGORYNUM+3 ; j++){
						if(strcmp( TypeTable[ j].arg  , token) == 0 ) {
							itemtype = TypeTable[ j].type;
							if(ITEM_getInt(itemindex,ITEM_TYPE) == itemtype) {
								cost = NPC_GetSellItemList(itemindex,0,argstr,token3,i,sell);
								if(cost != -1) return cost;
								strncat( token2, token3, sizeof( token3));
								okflg = 1;
							}else if(itemtype == 30){
								if( 8 <= ITEM_getInt(itemindex,ITEM_TYPE) 
									&& (ITEM_getInt(itemindex,ITEM_TYPE) <= 15) ){
									cost = NPC_GetSellItemList(itemindex,0,argstr,token3,i,sell);
									if(cost != -1) return cost;
									strncat(token2,token3,sizeof(token3));
									okflg = 1;
								}
							}else if(itemtype == 40){
								if(( 0 <= ITEM_getInt(itemindex,ITEM_TYPE) 
									&& (ITEM_getInt(itemindex,ITEM_TYPE) <= 4)) 
									|| (17 <= ITEM_getInt(itemindex,ITEM_TYPE) 
									&& (ITEM_getInt(itemindex,ITEM_TYPE) <= 19))) {
									cost = NPC_GetSellItemList(itemindex,0,argstr,token3,i,sell);
									if(cost != -1) return cost;
									strncat(token2,token3,sizeof(token3));
									okflg = 1;
								}
							}else if(itemtype == 50){
								if( 5 <= ITEM_getInt(itemindex,ITEM_TYPE) 
									&& (ITEM_getInt(itemindex,ITEM_TYPE) <= 7) ){
									cost = NPC_GetSellItemList(itemindex,0,argstr,token3,i,sell);
									if(cost != -1) return cost;
									strncat(token2,token3,sizeof(token3));
									okflg = 1;
								}
							}
							break;
						}
					}
					
					if(okflg == 1) break;
				}
			}
			if( (NPC_Util_GetStrFromStrWithDelim( argstr, "LimitItemNo", buff,sizeof( buff))!= NULL) && okflg == 0 ){
				k = 1;
				while(getStringFromIndexWithDelim(buff , "," , k, token, sizeof(token)) != FALSE ){
					k++;
					if(strstr( token, "-")==NULL && strcmp(token,"") != 0) {
						if(ITEM_getInt(itemindex,ITEM_ID) == atoi(token)) {
							cost = NPC_GetSellItemList(itemindex,0,argstr,token3,i,sell);
							if(cost != -1) return cost;
							strncat(token2,token3,sizeof(token3));
							okflg=1;
						}
					}else if (strstr( token, "-") != NULL){
						int start, end;
						int work;
						
						if( getStringFromIndexWithDelim( token, "-", 1, buf, sizeof(buf)) == FALSE )
							return -1;
						start = atoi( buf);
						if( getStringFromIndexWithDelim( token, "-", 2 ,buf, sizeof(buf)) == FALSE )
							return -1;
						end = atoi( buf);
						
						if(start > end){
							work = start;
							start = end;
							end = work;
						}
						
						end++;
						if( (start <= ITEM_getInt(itemindex,ITEM_ID)) && (ITEM_getInt(itemindex,ITEM_ID) < end) ){
							cost = NPC_GetSellItemList(itemindex,0,argstr,token3,i,sell);
							if(cost != -1) return cost;
							strncat(token2,token3,sizeof(token3));
							okflg = 1;
						}
					}
				}
			}
			
			if(okflg == 0) {
				cost = NPC_GetSellItemList(itemindex, 1, argstr, token3, i, sell);
				if(sell != -1) return -1;
				strncat( token2, token3, sizeof( token3));
			}
			
		}
	}
	return -1;
}



/*----------------------------------------------------------

	弁仿奶失件玄卞霜耨允月皿夫玄戊伙及综岳

 *----------------------------------------------------------*/
int NPC_GetSellItemList(int itemindex,int flg, char *argstr,char *argtoken,int select,int sell)
{
	char buff[256];
	double rate = 0.2;
	char buff2[256];
	char buff3[64];
	int k = 1;
	int cost = -1;

	if(NPC_Util_GetStrFromStrWithDelim( argstr,"special_item",buff, sizeof( buff)) != NULL){
		if(NPC_Util_GetStrFromStrWithDelim( argstr,"special_rate",buff2, sizeof( buff2)) != NULL ){
			rate = atof(buff2);
		}else{
			rate = 1.2;
		}
		
		while(getStringFromIndexWithDelim(buff , "," , k, buff2, sizeof(buff2)) !=FALSE ){
			k++;
			if(strstr( buff2, "-") == NULL && strcmp(buff2,"") != 0) {
				if(ITEM_getInt(itemindex,ITEM_ID) == atoi(buff2)){
					cost = NPC_SellItemstrsStr( itemindex,0, rate, argtoken,select,sell);
					return cost;
				}
			}else if (strstr( buff2, "-") != NULL){
				int start;
				int end;
				int work;
				getStringFromIndexWithDelim( buff2, "-", 1, buff3, sizeof(buff3));
				start = atoi( buff3);
				getStringFromIndexWithDelim( buff2, "-", 2 ,buff3, sizeof(buff3));
				end = atoi( buff3);

				if(start > end){
					work = start;
					start = end;
					end = work;
				}
				end++;
				if( (start <= ITEM_getInt(itemindex,ITEM_ID)) && (ITEM_getInt(itemindex,ITEM_ID) < end)	){
					cost = NPC_SellItemstrsStr( itemindex,0, rate, argtoken,select,sell);
					return cost;
				}
			}
		}
	}
	if( NPC_Util_GetStrFromStrWithDelim( argstr,"sell_rate",buff, sizeof( buff)) != NULL ){
		rate = atof(buff);
		cost = NPC_SellItemstrsStr( itemindex, flg ,rate, argtoken,select,sell);
		return cost;
	}

	return cost;
}

int NPC_SellItemstrsStr(int itemindex,int flg,double rate,char *argtoken,int select,int sell)
{
	int cost;
	char escapedname[256];
	char name[256];	
	char *eff;
	

	cost = ITEM_getInt( itemindex, ITEM_COST);
	cost = (int)(cost * rate);

	if(sell != -1) return cost;

	strcpy( escapedname, ITEM_getChar( itemindex, ITEM_SECRETNAME));
	makeEscapeString( escapedname, name, sizeof( name));
	eff=ITEM_getChar(itemindex, ITEM_EFFECTSTRING);
	makeEscapeString( eff, escapedname, sizeof(escapedname));



	sprintf( argtoken,
			"%s|%d|%d|%d|%s|%d|",
			name, flg, cost,
			ITEM_getInt( itemindex, ITEM_BASEIMAGENUMBER),
			escapedname, select
	);

	return -1;

}

BOOL NPC_SellNewItem(int meindex,int talker,char *data)
{
	char argstr[NPC_UTIL_GETARGSTR_BUFSIZE];
	char token[256], token2[256];
	int cost, k, select, itemindex;
	int MyGold, MaxGold, sellnum=1;

	MaxGold = CHAR_getMaxHaveGold( talker);
	MyGold = CHAR_getInt( talker, CHAR_GOLD);

	if(NPC_Util_GetArgStr( meindex, argstr, sizeof(argstr)) == NULL) {
		print("GetArgStrErr");
		return FALSE;
	}
	getStringFromIndexWithDelim(data , "|" ,1, token, sizeof( token));
	select = atoi(token);

	if( select < CHAR_STARTITEMARRAY || select >= CHAR_MAXITEMHAVE ) return FALSE;
	k = select;
	itemindex = CHAR_getItemIndex( talker , k);
	cost = NPC_GetLimtItemList( talker,argstr, token2,select);
	if( cost == -1 || (cost*sellnum)+MyGold >= MaxGold || !ITEM_CHECKINDEX( itemindex) ){
		int fd = getfdFromCharaIndex( talker);
		sprintf(token,"\n\n哎呀!对不起" "\n\n对不起啊 ! 可不可以再选一次呢？" );	
		lssproto_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE, 
				WINDOW_BUTTONTYPE_OK, 
				CHAR_WINDOWTYPE_WINDOWITEMSHOP_LIMIT,
				CHAR_getWorkInt( meindex, CHAR_WORKOBJINDEX),
				token);
		return FALSE;
	}
	{
		LogItem(
			CHAR_getChar( talker, CHAR_NAME ),
			CHAR_getChar( talker, CHAR_CDKEY ),
#ifdef _add_item_log_name  // WON ADD 在item的log中增加item名称
			itemindex,
#else
	   		ITEM_getInt( itemindex, ITEM_ID ),
#endif
			"Sell(卖道具)",
			CHAR_getInt( talker,CHAR_FLOOR),
			CHAR_getInt( talker,CHAR_X ),
			CHAR_getInt( talker,CHAR_Y ),
			ITEM_getChar( itemindex, ITEM_UNIQUECODE),
			ITEM_getChar( itemindex, ITEM_NAME),
			ITEM_getInt( itemindex, ITEM_ID)

		);
	}
	CHAR_DelItem( talker, k);
	CHAR_AddGold( talker, cost*sellnum);
	CHAR_send_P_StatusString( talker, CHAR_P_STRING_GOLD);
	return TRUE;
}



void NPC_LimitItemShop(int meindex,int talker,int select)
{

	int fd = getfdFromCharaIndex( talker);
	char token[NPC_UTIL_GETARGSTR_LINEMAX];
	char argstr[NPC_UTIL_GETARGSTR_BUFSIZE];
	char buf[1024];

	if( NPC_Util_GetArgStr( meindex, argstr, sizeof(argstr)) == NULL) {
       	print("shop_GetArgStr_Err");
       	return;
	}

	if(NPC_Util_GetStrFromStrWithDelim( argstr, "sellonly_msg", buf, sizeof( buf))!=NULL){
		sprintf(token,"\n\n%s", buf);
		lssproto_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE, 
				WINDOW_BUTTONTYPE_YESNO, 
				CHAR_WINDOWTYPE_WINDOWITEMSHOP_LIMIT,
				CHAR_getWorkInt( meindex, CHAR_WORKOBJINDEX),
				token);

	}else{
		CHAR_talkToCli( talker, meindex, "这是买卖专门店。",CHAR_COLORWHITE);
	}
	return;
}

void NPC_ExpressmanCheck(int meindex,int talker)
{
	int fd = getfdFromCharaIndex( talker);
	char token[1024];
	char argstr[NPC_UTIL_GETARGSTR_BUFSIZE];
	char buf[1024];

	if( NPC_Util_GetArgStr( meindex, argstr, sizeof(argstr)) == NULL) {
       	print("shop_GetArgStr_Err");
       	return;
	}
	NPC_Util_GetStrFromStrWithDelim( argstr, "main_msg", buf, sizeof( buf));
	sprintf(token,"4\n　　　　　　　%s\n\n%s"
					"\n\n　　　　　＜  打工  ＞　　　"
				  "\n\n　　　　  ＜交付行李＞"
					,CHAR_getChar(meindex,CHAR_NAME),buf);
	lssproto_WN_send( fd, WINDOW_MESSAGETYPE_SELECT, 
			WINDOW_BUTTONTYPE_CANCEL, 
			CHAR_WINDOWTYPE_WINDOWITEMSHOP_EXPRESS,
			CHAR_getWorkInt( meindex, CHAR_WORKOBJINDEX),
			token);

	return;


}

