#include "version.h"
#include <string.h>
#include <math.h>
#include "char.h"
#include "object.h"
#include "char_base.h"
#include "npcutil.h"
#include "lssproto_serv.h"
#include "enemy.h"
#include "log.h"
#include "battle.h"
#include "readmap.h"
#include "handletime.h"
#include "configfile.h"
#ifdef _GAMBLE_BANK
#include "npc_gamblebank.h"

static void NPC_GambleBank_selectWindow( int meindex, int toindex, int num, int flg);
int NPC_GambleBank_DoGold( int meindex, int toindex, int Gold, int flg);
BOOL NPC_GambleBank_AddItem( int meindex, int toindex, int itemId, int count);

enum	{
	GAMBLE_START = 0,
	GAMBLE_SELET,
	GAMBLE_MAN_BANK,
	GAMBLE_MAN_CHANG1,
	GAMBLE_MAN_CHANG2,
	GAMBLE_END,
};

enum {
	NPC_WORK_INDEX = CHAR_NPCWORKINT1,
	NPC_WORK_WORKTYPE = CHAR_NPCWORKINT2,	// 0,-1 = NULL 2 = 银行 3 = 换币 4 = BOTH
	NPC_WORK_CURRENTTIME = CHAR_NPCWORKINT3,
	NPC_WORK_PAGE = CHAR_NPCWORKINT4,
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
#define _GAMBLEBANK_U_NOLOCK	//同时对多人

#define GAMBLEBANK_LOOPTIME 80
#define GAMBLEBANK_STANDBY	5000 
#define GAMBLEBANK_DEF		3	//手续费 / 100
#define GAMBLEBANK_GETMAX	1000000
#define GAMBLEBANK_GETMIN	100
#define LIST_PAGE 7
BOOL NPC_GambleBankInit( int meindex )
{
	char npcarg[NPC_UTIL_GETARGSTR_BUFSIZE];
	char buf1[256];
    CHAR_setInt( meindex , CHAR_WHICHTYPE , CHAR_GAMBLEBANK );

#ifdef _GAMBLEBANK_U_NOLOCK
	CHAR_setInt( meindex, CHAR_LOOPINTERVAL, GAMBLEBANK_STANDBY);
	CHAR_setWorkInt( meindex, NPC_WORK_CURRENTTIME, NowTime.tv_sec);
#endif
	CHAR_setWorkInt( meindex , NPC_WORK_INDEX, -1);

	if(NPC_Util_GetArgStr( meindex, npcarg, sizeof(npcarg)) == NULL)	{
		print("GambleBank:GetArgStrErr");
		return FALSE;
	}

	if(NPC_Util_GetStrFromStrWithDelim( npcarg,"GAMBLE_TYPE", buf1,sizeof( buf1) ) == NULL) {
		print("GAMBLE_TYPE err !");
		return FALSE;
	}
	CHAR_setWorkInt( meindex, NPC_WORK_WORKTYPE, atoi( buf1) );

    return TRUE;
}
//CHAR_send_P_StatusString( charaindex, CHAR_P_STRING_GOLD);
void NPC_GambleBankLoop( int meindex)
{
#ifdef _GAMBLEBANK_U_NOLOCK

	int fulltime = GAMBLEBANK_LOOPTIME;
	if( ( CHAR_getWorkInt( meindex, NPC_WORK_CURRENTTIME) + fulltime) >= NowTime.tv_sec )
		return;
	CHAR_setWorkInt( meindex, NPC_WORK_CURRENTTIME, NowTime.tv_sec);
	CHAR_setWorkInt( meindex , NPC_WORK_INDEX, -1);
#endif
	return;
}

void NPC_GambleBankTalked( int meindex , int talkerindex , char *szMes ,int color )
{
	char buf1[256];
	char npcarg[NPC_UTIL_GETARGSTR_BUFSIZE];
	int work_type;

	if( !CHAR_CHECKINDEX( talkerindex) || !CHAR_CHECKINDEX( meindex) )
		return;
	if( CHAR_getInt( talkerindex , CHAR_WHICHTYPE ) != CHAR_TYPEPLAYER ) {
    		return;
	}
	if(NPC_Util_isFaceToFace( meindex ,talkerindex , 2) == FALSE) {
		if(NPC_Util_isFaceToChara( talkerindex, meindex, 1) == FALSE)	{
			return;
		}
	}    
	if(NPC_Util_GetArgStr( meindex, npcarg, sizeof(npcarg)) == NULL)	{
		print("GambleBank:GetArgStrErr");
		return;
	}
#ifdef _FIX_GAMBLENUM
	if( CHAR_getInt( talkerindex, CHAR_GAMBLENUM) < 0 )	{
		CHAR_setInt( talkerindex, CHAR_GAMBLENUM, 0);
	}
#endif
	if(NPC_Util_GetStrFromStrWithDelim( npcarg,"GAMBLE_TYPE", buf1,sizeof( buf1) ) == NULL) {
		print("GAMBLE_TYPE err !");
		return;
	}

	CHAR_setWorkInt( meindex, NPC_WORK_WORKTYPE, atoi( buf1) );
	work_type = CHAR_getWorkInt( meindex, NPC_WORK_WORKTYPE);


	if( work_type < 1 || work_type > 4 )	{	// 0 or -1
		strcpy( buf1,"暂时停止服务。");
		CHAR_talkToCli( talkerindex, meindex, buf1,  CHAR_COLORYELLOW);
		CHAR_setWorkInt( meindex , NPC_WORK_INDEX, -1);
		return;
	}
#ifdef _GAMBLEBANK_U_NOLOCK

#else
	if( CHAR_getWorkInt( meindex , NPC_WORK_INDEX) >= 0 )	{
			strcpy( buf1,"我正忙着！");
			CHAR_talkToCli( talkerindex, meindex, buf1,  CHAR_COLORYELLOW);
		return;
	}else	{
		CHAR_setWorkInt( meindex , NPC_WORK_INDEX, talkerindex);
		CHAR_setWorkInt( meindex, NPC_WORK_CURRENTTIME, NowTime.tv_sec);
	}
#endif
	CHAR_setWorkInt( talkerindex, CHAR_WORKSHOPRELEVANT, 1);
	NPC_GambleBank_selectWindow( meindex, talkerindex, GAMBLE_START, 1);
}

static void NPC_GambleBank_selectWindow( int meindex, int toindex, int num, int flg)
{
	char token[256];
	char buf1[256],buf2[256];
	char npcarg[NPC_UTIL_GETARGSTR_BUFSIZE];
	int buttontype = 0, windowtype = 0, windowno = 0;
	int fd = getfdFromCharaIndex( toindex);
	int i;
	int page=-1;
	//flg <= 0 错误 1 正常 flg = 2 钱不够  3 = 个人存款将满
	//		4 = 个人银行将满 5 = 提领上限 6 = 提领下限
	char Gamble_End[][56] = {
		"error_msg", "end_msg", "money_msg", "full_msg1",
		"full_msg2","getmax_msg","getmin_msg",
	};

	if(NPC_Util_GetArgStr( meindex, npcarg, sizeof(npcarg))==NULL){
		print("GambleBank:GetArgStrErr");
		return;
	}

  	windowtype = WINDOW_MESSAGETYPE_MESSAGE;
	switch( num)	{
	case GAMBLE_START:
		if( NPC_Util_GetStrFromStrWithDelim( npcarg,"gamble_start", token, sizeof( token) ) == NULL) {
			print("gamble_start msg err !");
			return;
		}
		buttontype = WINDOW_BUTTONTYPE_YESNO;
		windowno = NPC_GambleBank_START;
	break;
		
	case GAMBLE_SELET:
		i = 1;
		strcpy( token, "\0");
		sprintf( buf2,"%s%d", "gamble_msg0", i);
		while( NPC_Util_GetStrFromStrWithDelim( npcarg, buf2, buf1, sizeof( buf1) ) != NULL )	{
			if( strstr( buf1, "NULL" ) == NULL )	{
				strcat( token , buf1);
				strcat( token, "\n");
			}else	{
				strcat( token, "\n");
			}
			i++;
			sprintf( buf2,"%s%d", "gamble_msg0", i);
		}
	  	windowtype=WINDOW_MESSAGETYPE_SELECT;
	  	windowno = NPC_GambleBank_SELECT; 
		break;
	
	case GAMBLE_MAN_BANK:	//银行
		sprintf( token,"%d", CHAR_getInt( toindex, CHAR_PERSONAGOLD ) );
		windowtype = WINDOW_MESSAGETYPE_BANK;
		windowno = NPC_GambleBank_BANK;
	break;

	case GAMBLE_MAN_CHANG1:	//换物	
		{
			char snum[256];
			page = CHAR_getWorkInt( toindex, CHAR_WORKSHOPRELEVANT);
			strcpy( token, "换物");
			sprintf( token, "你有%d积分，想换哪一个奖品呢？\n", CHAR_getInt( toindex, CHAR_GAMBLENUM));
			for( i=(page*LIST_PAGE);i<(page*LIST_PAGE+LIST_PAGE);i++)	{
				if( i>=arraysizeof( GB_ITEMS))	{
					break;
				}
				if( !strcmp( GB_ITEMS[i].name,"NEXT") )	{
					strcat( token, "               下一页");
					break;
				}
				if( !strcmp( GB_ITEMS[i].name,"END") )	{
					strcat( token, "               取消\n");
					break;
				}

				sprintf( snum,"%s%d\t%s",
								"积分：", GB_ITEMS[i].Gnum,
								GB_ITEMS[i].name);
				snum[36] = 0;
				strcat( token, snum);
				strcat( token,"\n");
				strcpy( snum, "\0");
			}

			windowtype = WINDOW_MESSAGETYPE_SELECT;
			buttontype = WINDOW_BUTTONTYPE_NONE;
	  		windowno = NPC_GambleBank_CHANG1;
		}
	break;
	case GAMBLE_END:
		CHAR_send_P_StatusString( toindex, CHAR_P_STRING_GOLD);
		if( flg < 0  || flg > 6)	{
			flg = 0;
		}
		if( NPC_Util_GetStrFromStrWithDelim( npcarg, Gamble_End[flg], token, sizeof( token) ) == NULL) {
			print("Gamble_End[%d] noe found !", flg);
			return;
		}
		CHAR_setWorkInt( toindex, CHAR_WORKSHOPRELEVANT, -1);
#ifdef _GAMBLEBANK_U_NOLOCK
#else
		CHAR_setWorkInt( meindex , NPC_WORK_INDEX, -1);
#endif
		windowno = NPC_GambleBank_END;
		buttontype = WINDOW_BUTTONTYPE_OK;
	break;
	}

	lssproto_WN_send( fd, windowtype, 
					buttontype, 
					windowno,
					CHAR_getWorkInt( meindex, CHAR_WORKOBJINDEX),
					token);
}

void NPC_GambleBankWindowTalked ( int meindex, int talkerindex, int seqno, int select, char *data)
{
	int stone_gold = 0;
	int flg=1;
	int work_type=-1;
	int type;
	type = atoi( data);

	if( !CHAR_CHECKINDEX( talkerindex) || !CHAR_CHECKINDEX( meindex) )
		return;

	work_type = CHAR_getWorkInt( meindex, NPC_WORK_WORKTYPE );
	if( work_type < 1 || work_type > 4 )	{
		return;
	}
	if( CHAR_getInt( talkerindex , CHAR_WHICHTYPE ) != CHAR_TYPEPLAYER ) {
    		return;
	}
#ifdef _GAMBLEBANK_U_NOLOCK
#else
	{
		char buf1[256];
		if( CHAR_getWorkInt( meindex , NPC_WORK_INDEX) != talkerindex )	{
			strcpy( buf1,"我正忙着呢！");
			CHAR_talkToCli( talkerindex, meindex, buf1,  CHAR_COLORYELLOW);
			CHAR_setWorkInt( talkerindex, CHAR_WORKSHOPRELEVANT, -1);
			return;
		}
	}
#endif

	if(NPC_Util_isFaceToFace( meindex ,talkerindex , 2) == FALSE) {
		if(NPC_Util_isFaceToChara( talkerindex, meindex, 1) == FALSE) return;
	}    
	if( NPC_Util_CharDistance( talkerindex, meindex ) > 2) {
		return;
	}
	CHAR_setWorkInt( meindex, NPC_WORK_CURRENTTIME, NowTime.tv_sec);
	switch( seqno) {
	  case NPC_GambleBank_START:
		  if( select == WINDOW_BUTTONTYPE_YES )	{
			NPC_GambleBank_selectWindow( meindex, talkerindex, GAMBLE_SELET, flg);
		  }else	{
			  NPC_GambleBank_selectWindow( meindex, talkerindex, GAMBLE_END, flg);
		  }
	  	break;
	  case NPC_GambleBank_SELECT:
		  if( type == 1 )	{	//银行
			CHAR_setWorkInt( talkerindex, CHAR_WORKSHOPRELEVANT, 2);
			NPC_GambleBank_selectWindow( meindex, talkerindex, GAMBLE_MAN_BANK, flg);
		  }else if( type == 2 )	{	//换物
			CHAR_setWorkInt( talkerindex, CHAR_WORKSHOPRELEVANT, 0);
			NPC_GambleBank_selectWindow( meindex, talkerindex, GAMBLE_MAN_CHANG1, flg);
		  }else	{
			  NPC_GambleBank_selectWindow( meindex, talkerindex, GAMBLE_END, flg);
		  }
	  break;
	  case NPC_GambleBank_BANK:	//2
			if( CHAR_getWorkInt( talkerindex, CHAR_WORKSHOPRELEVANT) != 2 )	//错误程序
				return;
			if( work_type != 2 && work_type != 4 )	{
				CHAR_setWorkInt( meindex , NPC_WORK_INDEX, -1);
				return;
			}
			if( select == 4 && atoi( data) != 0 )	{
				stone_gold = atoi( data);
				flg = NPC_GambleBank_DoGold( meindex, talkerindex, stone_gold, select);
				//flg = 1 正常 flg = 2 钱不够 flg <= 0 错误
				if( flg == 1 )	{
					CHAR_setWorkInt( talkerindex, CHAR_WORKSHOPRELEVANT, -1);
					CHAR_setWorkInt( meindex , NPC_WORK_INDEX, -1);
					return;
				}
			}
			NPC_GambleBank_selectWindow( meindex, talkerindex, GAMBLE_END, flg);
		break;
	  case NPC_GambleBank_CHANG1:	//换物
		  {
			  int page,ItemID;
			  int count;
			  type -=1;
			  page = CHAR_getWorkInt( talkerindex, CHAR_WORKSHOPRELEVANT)*LIST_PAGE;
			  if( work_type != 3 && work_type != 4 )	{
					CHAR_setWorkInt( meindex , NPC_WORK_INDEX, -1);
					return;
			  }

			  if( page < 0 )	//错误对话程序
				  return;
			  if( (page+type) < 0 || (page+type) >= arraysizeof( GB_ITEMS) )	{
				NPC_GambleBank_selectWindow( meindex, talkerindex, GAMBLE_END, flg);
			  }else	if( !strcmp( GB_ITEMS[page+type].name,"NEXT") ){
				  CHAR_setWorkInt( talkerindex, CHAR_WORKSHOPRELEVANT,
						CHAR_getWorkInt( talkerindex, CHAR_WORKSHOPRELEVANT) + 1);
				  NPC_GambleBank_selectWindow( meindex, talkerindex, GAMBLE_MAN_CHANG1, flg);
			  }else	if( !strcmp( GB_ITEMS[page+type].name,"END") ){
					CHAR_setWorkInt( meindex , NPC_WORK_INDEX, -1);
					CHAR_setWorkInt( talkerindex, CHAR_WORKSHOPRELEVANT, -1);
					return;
			  }else	{
				  if( type < 7 && type >= 0 )	{
						  if( GB_ITEMS[page+type].name == NULL ||
							  !strcmp( GB_ITEMS[page+type].name, "\0") )	{
						  }else	{
							  ItemID = GB_ITEMS[page+type].ItemId;	//ID
							  count = GB_ITEMS[page+type].Gnum;	//积分
							  NPC_GambleBank_AddItem( meindex, talkerindex, ItemID, count);
						  }
						  CHAR_setWorkInt( meindex , NPC_WORK_INDEX, -1);
				  }
				  CHAR_setWorkInt( talkerindex, CHAR_WORKSHOPRELEVANT, -1);
			  }
		  }
		break;
	  case NPC_GambleBank_END:
		  CHAR_setWorkInt( talkerindex, CHAR_WORKSHOPRELEVANT, -1);
		break;
	}
}


int NPC_GambleBank_DoGold( int meindex, int toindex, int Gold, int flg)
{
	//flg = 1 正常 flg = 2 钱不够 flg <= 0 错误 3 = 个人存款将满 4 = 个人银行将满 5 = 单次最高金额
	char buf1[256];
	int player_gold = CHAR_getInt( toindex, CHAR_GOLD );
	int stone_def = 0; //手续费
//	int def = GAMBLEBANK_DEF;
	//不作无意义的存取款
	if( flg != 4 )
		return 0;
	if( Gold == 0 )	{
		return 0;
	}
	
	if( Gold < 0 )	{	//取款
		Gold *=-1;
		//stone_def = (Gold * def) /100;
		stone_def = 300;
		if( Gold > GAMBLEBANK_GETMAX )	{
			return 5;
		}else if( Gold < GAMBLEBANK_GETMIN )	{
			return 6;
		}
		if( ( player_gold + Gold ) > CHAR_getMaxHaveGold(toindex) )	{
			return 3;	//取款後将超过个人金额
		}else	if( (Gold + stone_def)> CHAR_getInt( toindex, CHAR_PERSONAGOLD ) )	{	//银行存款不够
			return 2;
		}
		
		CHAR_setInt( toindex, CHAR_PERSONAGOLD, (CHAR_getInt( toindex, CHAR_PERSONAGOLD )-(Gold+stone_def)));

		CHAR_AddGold( toindex, Gold);

		sprintf( buf1,"取款：%d ，手续费：%d，银行剩余：%d 。", Gold, stone_def, CHAR_getInt( toindex, CHAR_PERSONAGOLD ));
		CHAR_talkToCli( toindex, meindex, buf1,  CHAR_COLORYELLOW);
		LogBankStone(    CHAR_getChar( toindex, CHAR_NAME ), CHAR_getChar( toindex, CHAR_CDKEY ),
					toindex, Gold, 
					"GB_Bank_Get(银行取款)", 
					CHAR_getInt( toindex, CHAR_FLOOR),
		            CHAR_getInt( toindex, CHAR_X ), CHAR_getInt( toindex, CHAR_Y ) ,
					CHAR_getInt( toindex, CHAR_GOLD ),
					CHAR_getInt( toindex, CHAR_PERSONAGOLD )
					);
		return 1;
	}else if( Gold > 0 )	{	//存款
		if( Gold > player_gold )	{
			return 2;
		}else if( (Gold + CHAR_getInt( toindex, CHAR_PERSONAGOLD ) ) > CHAR_MAXPERSONAGOLD )  {
			return 4;
		}

		CHAR_DelGold( toindex, Gold );

		CHAR_setInt( toindex, CHAR_PERSONAGOLD, (CHAR_getInt( toindex, CHAR_PERSONAGOLD ) + Gold ) );
		sprintf( buf1,"存款：%d ，银行剩余：%d 。", Gold, CHAR_getInt( toindex, CHAR_PERSONAGOLD ));
		CHAR_talkToCli( toindex, meindex, buf1,  CHAR_COLORYELLOW);
		LogBankStone(    CHAR_getChar( toindex, CHAR_NAME ), CHAR_getChar( toindex, CHAR_CDKEY ),
					toindex, Gold, 
					"GB_Bank_save(银行存款)", 
					CHAR_getInt( toindex, CHAR_FLOOR),
		            CHAR_getInt( toindex, CHAR_X ), CHAR_getInt( toindex, CHAR_Y ),
					CHAR_getInt( toindex, CHAR_GOLD ),
					CHAR_getInt( toindex, CHAR_PERSONAGOLD )
					);
		return 1;
	}
	return 0;
}

BOOL NPC_GambleBank_AddItem( int meindex, int toindex, int itemId, int count)
{
	int i=-1,itemindex=-1;
	int ret=-1;
	char token[256];
 	strcpy( token,"\0");
	if( count < 0 || itemId < 0 )
		return FALSE;

	if( CHAR_getInt( toindex, CHAR_GAMBLENUM) < count )	{
		sprintf( token,"游乐场积分不足！");
		CHAR_talkToCli( toindex, -1,token,CHAR_COLORWHITE);
		return FALSE;
	}
	for( i = 0 ; i < CHAR_MAXITEMHAVE ; i++ ){
		itemindex=CHAR_getItemIndex( toindex , i );
		if( itemindex == -1 )	{
			break;
		}
	}
	if( i == CHAR_MAXITEMHAVE )	{
		snprintf( token,sizeof( token), "物品栏空间不足！！");
		CHAR_talkToCli( toindex, -1, token,  CHAR_COLORWHITE);
		return FALSE;
	}
	itemindex = ITEM_makeItemAndRegist( itemId);
	if(itemindex == -1)
		return FALSE;
	ret = CHAR_addItemSpecificItemIndex( toindex, itemindex);
	if( ret < 0 || ret >= CHAR_MAXITEMHAVE ) {
		ITEM_endExistItemsOne( itemindex);
		return FALSE;
	}
		LogItem(
				CHAR_getChar( toindex, CHAR_NAME ),
				CHAR_getChar( toindex, CHAR_CDKEY ),
#ifdef _add_item_log_name  // WON ADD 在item的log中增加item名称
				itemindex,
#else
				ITEM_getInt( itemindex, ITEM_ID ),
#endif
				"G_BANK(游乐场积分兑换道具)",
				CHAR_getInt( toindex,CHAR_FLOOR),
				CHAR_getInt( toindex,CHAR_X ),
				CHAR_getInt( toindex,CHAR_Y ),
				ITEM_getChar( itemindex, ITEM_UNIQUECODE),
				ITEM_getChar( itemindex, ITEM_NAME),
				ITEM_getInt( itemindex, ITEM_ID)
		);
	sprintf( token,"拿到%s",ITEM_getChar( itemindex, ITEM_NAME));
	CHAR_talkToCli( toindex, -1,token,CHAR_COLORWHITE);
	CHAR_sendItemDataOne( toindex, ret);
	CHAR_setInt( toindex, CHAR_GAMBLENUM, CHAR_getInt( toindex, CHAR_GAMBLENUM) - count);
	sprintf( token,"游乐场积分剩余： %d", CHAR_getInt( toindex, CHAR_GAMBLENUM));
	CHAR_talkToCli( toindex, -1,token,CHAR_COLORWHITE);
	return TRUE;
}

#endif





