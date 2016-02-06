#include "version.h"
#include "correct_bug.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "readmap.h"
#include "object.h"
#include "char.h"
#include "char_base.h"
#include "lssproto_serv.h"
#include "npcutil.h"
#include "trade.h"
#include "log.h"
#include "handletime.h"
#include "buf.h"
#include "net.h"
#include "char_base.h"
#include "battle.h"
#include "npc_bus.h"
#include "char_talk.h"

#define TRADE_WAIT		"请稍候，连络%s中···"
#define TRADE_NONE		"前方什麽人也没有！"
#define	TRADE_OVERPLAYER	"前方不只一位玩家喔！"
#define	TRADE_NOWILL		"真是抱歉，对方不愿意跟你交易！"
#define	TRADE_TRADING		"%s正在与其他人交易···"
#define	TRADE_VANISHITEM	"丢在地上会消失的物品无法交易！"
#define TRADE_PETFULL		"%s宠物栏已满，交易取消！"
#define	TRADE_ITEMFULL		"%s物品栏已满，请整理後再交易～"
#define TRADE_CANCEL		"%s将交易取消了！"
#define TRADE_POOR		"%s钱不够，无法交易！"
#define	TRADE_RICH		"%s金钱交易请勿超过一百万！"
#define TRADE_TOORICH		"%s金钱将会超过一百万！"
#define TRADE_POORLV		"%s等级不够，无法照顾交易後的宠物！"
#define	TRADE_LOCK		"交易锁定···"
#define	TRADE_SUCCESS		"交易ＯＫ！"
#define	TRADE_FAILED		"交易失败！"
#define TRADE_FMPET		"家族守护兽无法交易！"

#define TRADE_RDCANCEL	"交易取消！"

enum{
	TRADE_ITEM=0,
	TRADE_PET,
	TRADE_GOLD,
};

#define MAX_TRADELISTNUM 100
typedef struct _tagTradeList{
	int charaindex;
	int fd;
	int PetTi[5];
	int ItemTi[15];
	int ItemNum[15];
	int Golds;
	int use;
}STradeList;

STradeList TradeList[MAX_TRADELISTNUM][2];

BOOL TRADE_CheckTradeList( int meindex, STradeList *temp1, int toindex, STradeList *temp2);

void TRADE_ShowItem(int fd, int meindex, char* message);
void TRADE_Close(int fd, int meindex, char* message);

void TRADE_SwapItem(int meindex, int toindex, char* message, int fd, char* mycharaname, int tofd, char* tocharaname);

int TRADE_CheckItembuf(int fd, int meindex, int toindex, int tofd, char* mycharaname, char* tocharaname);
BOOL TRADE_ChangeItem(int meindex, int toindex, char *a, char *b, int item1, int item2, int itemindex1, int itemindex2);
BOOL TRADE_HandleItem( int meindex, int showindex, char *message, char *outmess);
BOOL TRADE_HandleGold( int meindex, int showindex, char *message, char *outmess);
BOOL TRADE_HandlePet( int meindex, int showindex, char *message, char *outmess);



void CHAR_Trade(int fd, int index, char* message)
{
   char		firstToken[64];
   // shan 以下注掉是不必要的动作  2002/03/05
   //char		messageeraseescape[512];
   char*	messagebody;
   {   		
	   	if (*message == 0) 	return;
	    if (!CHAR_CHECKINDEX(index))	return;
	   	CHAR_getMessageBody( message, firstToken, sizeof(firstToken), &messagebody);
			switch( tolower( firstToken[0]) ){
			  case 'd':	// 寻找前方玩家
					TRADE_Search(fd, index, message);
			   	break;  
				case 't':	// 显示交易的物品、金钱、宠物
			   	TRADE_ShowItem(fd, index, message);
			   	break;
			  case 'w':	// 关闭交易
					TRADE_Close(fd, index, message);
			   	break;
			  default:
			   	break;
			}
   }
}

BOOL TRADE_Search(int fd, int meindex, char* message)
{
   int		objbuf[16];
   int		front_x, front_y, i, found_count;
   BOOL		found =  FALSE, searchflg = FALSE;
   int 		cnt = 0, tofd = -1, checkfd = -1;
   char		msgbuf[1024], mycharaname[256], tocharaname[256];
   char		token[256];

   if (!CHAR_CHECKINDEX(meindex))	return FALSE;

   // 若玩家状态为交易中或交易锁定中则不予处理
   if (CHAR_getWorkInt(meindex, CHAR_WORKTRADEMODE) == CHAR_TRADE_TRADING
   	|| CHAR_getWorkInt(meindex, CHAR_WORKTRADEMODE) == CHAR_TRADE_LOCK)
   		return FALSE;

   // 若玩家状态为组队或战斗中则不予处理
   if ((CHAR_getWorkInt(meindex, CHAR_WORKPARTYMODE) != CHAR_PARTY_NONE)
   	|| (CHAR_getWorkInt(meindex, CHAR_WORKBATTLEMODE) != BATTLE_CHARMODE_NONE))
   		return FALSE;
   strcpy(mycharaname, CHAR_getChar(meindex, CHAR_NAME));

   if (getStringFromIndexWithDelim(message, "|", 2, token,
	sizeof(token)) == FALSE)	return FALSE;
   // 设定状态交易请求中
//   CHAR_setWorkInt(meindex, CHAR_WORKTRADEMODE, CHAR_TRADE_SENDING);
   
	for (i = 0; i < CONNECT_WINDOWBUFSIZE; i++)
		CONNECT_setTradecharaindex(fd, i, -1);
   CHAR_getCoordinationDir(CHAR_getInt(meindex, CHAR_DIR),
   		CHAR_getInt(meindex, CHAR_X),
   		CHAR_getInt(meindex, CHAR_Y),
   		1, &front_x, &front_y);

	found_count = CHAR_getSameCoordinateObjects( objbuf, arraysizeof(objbuf),
							CHAR_getInt(meindex, CHAR_FLOOR),front_x, front_y);
	// 前方没有玩家或对方关闭交易选项
	if (found_count == 0){
		CHAR_talkToCli(meindex, -1, TRADE_NONE, CHAR_COLORYELLOW);
		CHAR_setWorkInt(meindex, CHAR_WORKTRADEMODE, CHAR_TRADE_FREE);
		return FALSE;
	}
   
	for (i = 0; i < found_count; i++){
		int	objindex = objbuf[i];
		int	index = OBJECT_getIndex(objindex);

		if (OBJECT_getType(objindex) != OBJTYPE_CHARA)	continue;
		if (CHAR_getInt(index, CHAR_WHICHTYPE) != CHAR_TYPEPLAYER)	continue;
		if (index == meindex)	continue;
		found = TRUE;
		if (CHAR_getWorkInt(index, CHAR_WORKBATTLEMODE) != BATTLE_CHARMODE_NONE)	continue;
		if (CHAR_getWorkInt(index, CHAR_WORKPARTYMODE) != CHAR_PARTY_NONE)	continue;
		if (CHAR_getFlg(index, CHAR_ISTRADE) == FALSE)	continue;
		if (CHAR_getWorkInt(index, CHAR_WORKTRADEMODE) != CHAR_TRADE_FREE)	continue;
		strcpy(tocharaname, CHAR_getChar(index, CHAR_NAME));
		tofd = getfdFromCharaIndex(index);
		if (tofd == -1){
			CHAR_setWorkInt(meindex, CHAR_WORKTRADEMODE, CHAR_TRADE_FREE);
			return	FALSE;
		}	
		if (searchflg){
			if (tofd != checkfd)	continue;
			if (strcmp(token, tocharaname) != 0)	continue;
		}
		CONNECT_setTradecharaindex(fd, cnt, index);
		cnt ++;
		if (cnt == CONNECT_WINDOWBUFSIZE)	break;
	}

	if (cnt == 0){
		// 前方没有玩家或对方关闭交易选项
		if (found){
			CHAR_talkToCli(meindex, -1, TRADE_NOWILL, CHAR_COLORYELLOW);
		}else
			CHAR_talkToCli(meindex, -1, TRADE_NONE, CHAR_COLORYELLOW);

		sprintf(msgbuf, "C|%d|%s|0", tofd, tocharaname);
		CHAR_setWorkInt(meindex, CHAR_WORKTRADEMODE, CHAR_TRADE_FREE);
		return FALSE;
	}
	// 前方有一位玩家
	if (cnt == 1){
		int toindex;
		toindex = CONNECT_getCharaindex(tofd);
		if( !CHAR_CHECKINDEX(toindex))       return FALSE;
		snprintf(msgbuf, sizeof(msgbuf), TRADE_WAIT, tocharaname);
   		CHAR_talkToCli(meindex, -1, msgbuf, CHAR_COLORYELLOW);

   		sprintf(msgbuf, "C|%d|%s|1", fd, mycharaname);
   		lssproto_TD_send( tofd, -1, msgbuf);
   		sprintf(msgbuf, "C|%d|%s|1", tofd, tocharaname);
   		lssproto_TD_send(fd, -1, msgbuf);

   		CHAR_setWorkInt(meindex, CHAR_WORKTRADEMODE, CHAR_TRADE_TRADING);
   		CHAR_setWorkInt(toindex, CHAR_WORKTRADEMODE, CHAR_TRADE_TRADING);

		CONNECT_set_confirm(fd, FALSE);
		CONNECT_set_confirm(tofd, FALSE);

        CHAR_sendTradeEffect(meindex, 1);
        CHAR_sendTradeEffect(toindex, 1);
        return TRUE;
   }else if (cnt > 1){	// 前方不只一位玩家
		CHAR_talkToCli(meindex, -1, TRADE_OVERPLAYER, CHAR_COLORYELLOW);
//		sprintf(msgbuf, "C|%d|%s|0", tofd, tocharaname);
		CHAR_setWorkInt(meindex, CHAR_WORKTRADEMODE, CHAR_TRADE_FREE);
   		return FALSE;
    }

	CHAR_setWorkInt(meindex, CHAR_WORKTRADEMODE, CHAR_TRADE_FREE);
	return FALSE;
}

void TRADE_Close(int fd, int meindex, char* message)
{
	char msg[128], mycharaname[256];
	int tofd=-1, toindex=-1;
	int j;
	char token[256];
	if (!CHAR_CHECKINDEX(meindex))	return;
	if (*message == 0)	return;
	strcpy(mycharaname, CHAR_getChar(meindex, CHAR_NAME));

   if (getStringFromIndexWithDelim(message, "|", 2, token, sizeof(token)) == FALSE)	return;
   tofd = atoi(token);
   toindex = CONNECT_getCharaindex(tofd);
   if (!CHAR_CHECKINDEX(toindex))       return;
   if (getStringFromIndexWithDelim(message, "|", 3, token, sizeof(token)) == FALSE)	return;

   
   snprintf(msg, sizeof(msg), TRADE_CANCEL, mycharaname);
   CHAR_talkToCli(toindex, -1, msg, CHAR_COLORYELLOW);
   snprintf( msg, sizeof(msg), "W|%d|%s", fd, mycharaname);
   lssproto_TD_send( tofd, -1, msg);

   CHAR_setWorkInt(toindex, CHAR_WORKTRADEMODE, CHAR_TRADE_FREE);
   CONNECT_setTradeTmp(tofd, "");
   CHAR_sendStatusString( CONNECT_getCharaindex(tofd), "i");
   CHAR_sendStatusString( CONNECT_getCharaindex(tofd), "P");
   CHAR_sendTradeEffect( toindex, 0);
   CONNECT_set_confirm(tofd, FALSE);

	if( !CHAR_CHECKINDEX( toindex) ){
		char buf[256];
		if( getStringFromIndexWithDelim( message, "|", 2, buf, sizeof(buf)) == FALSE)	return;
		tofd = atoi( buf);
		toindex = CONNECT_getCharaindex( tofd);
		if (!CHAR_CHECKINDEX(toindex) )return;
	}


	snprintf( msg, sizeof(msg), TRADE_RDCANCEL);
	CHAR_talkToCli(meindex, -1, msg, CHAR_COLORYELLOW);
	snprintf( msg, sizeof(msg), "W|%d|%s", tofd, CHAR_getChar( toindex, CHAR_NAME) );
	lssproto_TD_send( fd, -1, msg);

	CHAR_setWorkInt( meindex, CHAR_WORKTRADEMODE, CHAR_TRADE_FREE);
	CONNECT_setTradeTmp(fd, "");
	CHAR_sendStatusString(meindex, "i");
	CHAR_sendStatusString(meindex, "P");
	CHAR_sendTradeEffect( meindex, 0);
	CONNECT_set_confirm(fd, FALSE);
// end
	for(j = 0; j < CHAR_MAXPETHAVE; j++){
		char msgbuf[256];
		int petindex = CHAR_getCharPet( meindex, j);
		if( !CHAR_CHECKINDEX( petindex) ) continue;
		CHAR_complianceParameter( petindex );
		snprintf( msgbuf, sizeof( msgbuf ), "K%d", j );
		CHAR_sendStatusString( meindex, msgbuf );
		snprintf( msgbuf, sizeof( msgbuf ), "W%d", j );
		CHAR_sendStatusString( meindex, msgbuf );
	}

}

void TRADE_ShowItem(int fd, int meindex, char* message)
{
   char token[128], mycharaname[256], tocharaname[256], msg[2048];
   int tofd, showindex, toindex;// itemindex, gold, tmpgold, item;

   int MaxGold;

   MaxGold = CHAR_getMaxHaveGold( meindex);
   strcpy( mycharaname, CHAR_getChar(meindex, CHAR_NAME));

   if (getStringFromIndexWithDelim(message, "|", 2, token, sizeof(token)) == FALSE)	return;
   tofd = atoi(token);
   toindex = CONNECT_getCharaindex(tofd);

   if (!CHAR_CHECKINDEX(toindex))       return;
   // 若玩家状态为组队或战斗中则不予处理
   if ((CHAR_getWorkInt(meindex, CHAR_WORKPARTYMODE) != CHAR_PARTY_NONE)
   	|| (CHAR_getWorkInt(meindex, CHAR_WORKBATTLEMODE) != BATTLE_CHARMODE_NONE))
   		return;

   if ((CHAR_getWorkInt(toindex, CHAR_WORKPARTYMODE) != CHAR_PARTY_NONE)
   	|| (CHAR_getWorkInt(toindex, CHAR_WORKBATTLEMODE) != BATTLE_CHARMODE_NONE))
   		return;

	if (getStringFromIndexWithDelim(message, "|", 3, tocharaname, sizeof(tocharaname)) == FALSE) return;
	if (getStringFromIndexWithDelim(message, "|", 5, token, sizeof(token)) == FALSE) return;
	showindex = atoi(token);
	
	if (getStringFromIndexWithDelim(message, "|", 4, token, sizeof(token)) == FALSE) return; 
		
	strcpy( tocharaname, CHAR_getChar(toindex, CHAR_NAME));

	switch(tolower(token[0])){
		case 'i':
		{
			char buf1[2048];
			if( TRADE_HandleItem( meindex, showindex, message, buf1) == FALSE ){
				print(" TRADE_Close_Err3:%d,%d,%s,%s ", meindex, showindex, message, buf1);
				TRADE_Close(fd, meindex, message);
				return;
			}else{
				snprintf(msg, sizeof(msg), "T|%d|%s|I|%d|%s", fd, mycharaname, showindex, buf1 );
				lssproto_TD_send(tofd, -1, msg);
			}
		}
		break;
   	case 'g':
		{
			char buf1[2048];
			if( TRADE_HandleGold( meindex, showindex, message, buf1) == FALSE ){
				print(" TRADE_Close_Err4:%d,%d,%s,%s ", meindex, showindex, message, buf1);
				TRADE_Close(fd, meindex, message);
				return;
			}else{// 将欲交易之金钱传给对方
				snprintf(msg, sizeof(msg), "T|%d|%s|G|%d|%s", fd, mycharaname, showindex, buf1);
				lssproto_TD_send( tofd, -1, msg);
			}
		}
		break;
		case 'p':
		{
			char buf1[2048];
			if( TRADE_HandlePet( meindex, showindex, message, buf1) == FALSE ){
				print(" TRADE_Close_Err5:%d,%d,%s,%s ", meindex, showindex, message, buf1);
				TRADE_Close(fd, meindex, message);
				return;
			}else{
				snprintf(msg, sizeof(msg), "T|%d|%s|P|%d|%s",
					fd, mycharaname, showindex, buf1);
				lssproto_TD_send(tofd, -1, msg);
			}
		}
		break;
   	case 'k':
   	{
	    if((CONNECT_get_confirm(fd)==TRUE) &&
						(CONNECT_get_confirm(tofd)==TRUE) ){//当双方都按下 lock
				if( CHAR_getWorkInt(meindex, CHAR_WORKTRADEMODE) == CHAR_TRADE_LOCK ){
					//andy_log
					print("ANDY err 防止第二次进入!!\n");
					return;//防止第二次进入
				}
				CHAR_setWorkInt(meindex, CHAR_WORKTRADEMODE, CHAR_TRADE_LOCK);
				//检查对方是否按下 ok
//				if( CHAR_getWorkInt( toindex, CHAR_WORKTRADEMODE) != CHAR_TRADE_LOCK )return;
			
				snprintf(msg, sizeof(msg), "T|%d|%s|A", fd, mycharaname);
				lssproto_TD_send( tofd, -1, msg);
				TRADE_SwapItem(meindex, toindex, message, fd, mycharaname, tofd, tocharaname);
			}else{
				if( CONNECT_get_confirm( fd) == TRUE ) return;
				CONNECT_set_confirm(fd, TRUE);
				snprintf( msg, sizeof(msg), "T|%d|%s|C", fd, mycharaname);
				lssproto_TD_send( tofd, -1, msg);
			}
		}
   	break;
		case 'c':
		{
	    if( CONNECT_get_confirm( fd) == TRUE ) return;
			CONNECT_set_confirm(fd, TRUE);
			snprintf( msg, sizeof(msg), "T|%d|%s|C", fd, mycharaname);
			lssproto_TD_send( tofd, -1, msg);
		}
		break;
  }
	return;
}

void TRADE_SwapItem( int meindex, int toindex, char* message, int fd, char* mycharaname, int tofd, char* tocharaname)
{
   int result;
   char msg[128], tmpmsg[128];

   CONNECT_setTradeTmp(fd, message);
   result = TRADE_CheckItembuf(fd, meindex, toindex, tofd, mycharaname, tocharaname);
   toindex = CONNECT_getCharaindex(tofd);

   if (!CHAR_CHECKINDEX(toindex))       return;
   if (result == -1)
   	strcpy(msg, TRADE_FAILED);
   else if (result == -2)
   	sprintf(msg, TRADE_ITEMFULL, mycharaname);
   else if (result == -3)
   	sprintf(msg, TRADE_ITEMFULL, tocharaname);
   else if (result == -4)
   	sprintf(msg, TRADE_RICH, mycharaname);
   else if (result == -5)
   	sprintf(msg, TRADE_POOR, mycharaname);
   else if (result == -6)
   	sprintf(msg, TRADE_RICH, tocharaname);
   else if (result == -7)
   	sprintf(msg, TRADE_POOR, tocharaname);
   else if (result == -8)
   	sprintf(msg, TRADE_TOORICH, mycharaname);
   else if (result == -9)
   	sprintf(msg, TRADE_TOORICH, tocharaname);
   else if (result == -10)
   	sprintf(msg, TRADE_PETFULL, mycharaname);
   else if (result == -11)
   	sprintf(msg, TRADE_PETFULL, tocharaname);
   else if (result == -12)
   	strcpy(msg, TRADE_VANISHITEM);
   else if (result == -13)
   	sprintf(msg, TRADE_POORLV, mycharaname);
   else if (result == -14)
   	sprintf(msg, TRADE_POORLV, tocharaname);
   else if (result == -15)
   	strcpy(msg, TRADE_FMPET);
   else if (result == 1)
   	strcpy(msg, TRADE_SUCCESS);
   else if (result == 2)
   	strcpy(msg, TRADE_LOCK);

   if (result != 2){
   	snprintf(tmpmsg, sizeof(tmpmsg), "W|%d|%s", fd, mycharaname);
   	lssproto_TD_send(tofd, -1, tmpmsg);
   	snprintf(tmpmsg, sizeof(tmpmsg), "W|%d|%s", tofd, tocharaname);
   	lssproto_TD_send(fd, -1, tmpmsg);
   	
	CHAR_setWorkInt(meindex, CHAR_WORKTRADEMODE, CHAR_TRADE_FREE);
	CHAR_setWorkInt(toindex, CHAR_WORKTRADEMODE, CHAR_TRADE_FREE);
	CONNECT_setTradeTmp(fd, "");
	CONNECT_setTradeTmp(tofd, "");
	CHAR_sendTradeEffect( meindex, 0);
	CHAR_sendTradeEffect( toindex, 0);   	
   }
	CHAR_sendStatusString(meindex, "i");
	CHAR_sendStatusString(toindex, "i");
	CHAR_sendStatusString(meindex, "P");
	CHAR_sendStatusString(toindex, "P");
	{
		int j, petindex;
		char msgbuf[256];
		for(j = 0; j < CHAR_MAXPETHAVE; j++){
			petindex = CHAR_getCharPet( meindex, j);
			if( !CHAR_CHECKINDEX( petindex) ) continue;
			CHAR_complianceParameter( petindex );
			snprintf( msgbuf, sizeof( msgbuf ), "K%d", j );
			CHAR_sendStatusString( meindex, msgbuf );
			snprintf( msgbuf, sizeof( msgbuf ), "W%d", j );
			CHAR_sendStatusString( meindex, msgbuf );
		}
		for(j = 0; j < CHAR_MAXPETHAVE; j++){
			petindex = CHAR_getCharPet( toindex, j);
			if( !CHAR_CHECKINDEX( petindex) ) continue;
			CHAR_complianceParameter( petindex );
			snprintf( msgbuf, sizeof( msgbuf ), "K%d", j );
			CHAR_sendStatusString( toindex, msgbuf );
			snprintf( msgbuf, sizeof( msgbuf ), "W%d", j );
			CHAR_sendStatusString( toindex, msgbuf );
		}
	}

   CHAR_talkToCli(meindex, -1, msg, CHAR_COLORYELLOW);
   CHAR_talkToCli(toindex, -1, msg, CHAR_COLORYELLOW);   
}

int TRADE_CheckItembuf(int fd, int meindex, int toindex, int tofd, char* mycharaname, char* tocharaname)
{
   char a[16], b[16], c[16], d[16], e[16], f[16];
   char g[16], h[16], i[16], j[16], k[16], l[16], token[16];
   int itemindex1 = 0, itemindex2 = 0, itemindex3 = 0;
   int itemindex4 = 0, itemindex5 = 0, itemindex6 = 0;
   int toitemindex1 = 0, toitemindex2 = 0, toitemindex3 = 0;
   int toitemindex4 = 0, toitemindex5 = 0, toitemindex6 = 0;
   int item1 = 0, item2 = 0, item4 = 0, item5 = 0, pet3 = 0, pet6 = 0;
   int pet1 = 0, pet2 = 0;
   int swapitem1 = 0, swapitem2 = 0, gold1 = 0, gold2 = 0;
   char itembuf[256], toitembuf[256], buf[4048];
   
   CONNECT_getTradeTmp(fd, itembuf, sizeof(itembuf));
   CONNECT_getTradeTmp(tofd, toitembuf, sizeof(toitembuf));

   // 比对双方之最後交易协定
   if (getStringFromIndexWithDelim(itembuf, "|", 5, token, sizeof(token)) == TRUE)
	   	strcpy(a, token);
   if (getStringFromIndexWithDelim(itembuf, "|", 6, token, sizeof(token)) == TRUE)
		itemindex1 = atoi(token);
   if (getStringFromIndexWithDelim(itembuf, "|", 7, token, sizeof(token)) == TRUE)
	   	strcpy(b, token);
   if (getStringFromIndexWithDelim(itembuf, "|", 8, token, sizeof(token)) == TRUE)
		itemindex2 = atoi(token);
   if (getStringFromIndexWithDelim(itembuf, "|", 9, token, sizeof(token)) == TRUE)
	   	strcpy(c, token);
   if (getStringFromIndexWithDelim(itembuf, "|", 10, token, sizeof(token)) == TRUE)
		itemindex3 = atoi(token);
   if (getStringFromIndexWithDelim(itembuf, "|", 11, token, sizeof(token)) == TRUE)
	   	strcpy(d, token);
   if (getStringFromIndexWithDelim(itembuf, "|", 12, token, sizeof(token)) == TRUE)
		itemindex4 = atoi(token);
   if (getStringFromIndexWithDelim(itembuf, "|", 13, token, sizeof(token)) == TRUE)
	   	strcpy(e, token);
   if (getStringFromIndexWithDelim(itembuf, "|", 14, token, sizeof(token)) == TRUE)
		itemindex5 = atoi(token);
   if (getStringFromIndexWithDelim(itembuf, "|", 15, token, sizeof(token)) == TRUE)
		strcpy(f, token);
   if (getStringFromIndexWithDelim(itembuf, "|", 16, token, sizeof(token)) == TRUE)
		itemindex6 = atoi(token);

   if (getStringFromIndexWithDelim(toitembuf, "|", 5, token, sizeof(token)) == TRUE)
	   	strcpy(g, token);
   if (getStringFromIndexWithDelim(toitembuf, "|", 6, token, sizeof(token)) == TRUE)
		toitemindex1 = atoi(token);
   if (getStringFromIndexWithDelim(toitembuf, "|", 7, token, sizeof(token)) == TRUE)
	   	strcpy(h, token);
   if (getStringFromIndexWithDelim(toitembuf, "|", 8, token, sizeof(token)) == TRUE)
		toitemindex2 = atoi(token);
   if (getStringFromIndexWithDelim(toitembuf, "|", 9, token, sizeof(token)) == TRUE)
	   	strcpy(i, token);
   if (getStringFromIndexWithDelim(toitembuf, "|", 10, token, sizeof(token)) == TRUE)
		toitemindex3 = atoi(token);
   if (getStringFromIndexWithDelim(toitembuf, "|", 11, token, sizeof(token)) == TRUE)
	   	strcpy(j, token);
   if (getStringFromIndexWithDelim(toitembuf, "|", 12, token, sizeof(token)) == TRUE)
		toitemindex4 = atoi(token);
   if (getStringFromIndexWithDelim(toitembuf, "|", 13, token, sizeof(token)) == TRUE)
	   	strcpy(k, token);
   if (getStringFromIndexWithDelim(toitembuf, "|", 14, token, sizeof(token)) == TRUE)
		toitemindex5 = atoi(token);
   if (getStringFromIndexWithDelim(toitembuf, "|", 15, token, sizeof(token)) == TRUE)
	   	strcpy(l, token);
   if (getStringFromIndexWithDelim(toitembuf, "|", 16, token, sizeof(token)) == TRUE)
		toitemindex6 = atoi(token);

   // shan hjj add 洗道具
   if (itemindex1==itemindex2){
        if (itemindex1!=-1){
            return -1;
        }
   }
   if (itemindex4==itemindex5){
        if (itemindex4!=-1){  
            return -1;
        }
   }
   // End
   if ((strcmp(a, j) == 0) && (strcmp(b, k) == 0) && (strcmp(c,l) == 0)
   	&& (strcmp(d, g) == 0) && (strcmp(e, h) == 0) && (strcmp(f, i) == 0)
   	&& (itemindex1 == toitemindex4) && (itemindex2 == toitemindex5)
   	&& (itemindex3 == toitemindex6) && (itemindex4 == toitemindex1)
   	&& (itemindex5 == toitemindex2) && (itemindex6 == toitemindex3))
   {
   	// 检验双方物品栏是否有空位
	if ((strcmp(a, "I") == 0) && (itemindex1 != -1)){
		swapitem1--;
		swapitem2++;
	}
	if ((strcmp(b, "I") == 0) && (itemindex2 != -1)){
		swapitem1--;
		swapitem2++;
	}
	if ((strcmp(d, "I") == 0) && (itemindex4 != -1)){
		swapitem1++;
		swapitem2--;
	}
	if ((strcmp(e, "I") == 0) && (itemindex5 != -1)){
		swapitem1++;
		swapitem2--;
	}
	if (swapitem1 > CHAR_findTotalEmptyItem(meindex))
		return	-2;
	toindex = CONNECT_getCharaindex(tofd);
	if (!CHAR_CHECKINDEX(toindex))       return -1;
	if (swapitem2 > CHAR_findTotalEmptyItem(toindex))
		return	-3;
	// 检查双方金钱
	if ((strcmp(a, "G") == 0) && (itemindex1 != -1))	gold1 += itemindex1;
	if ((strcmp(b, "G") == 0) && (itemindex2 != -1))	gold1 += itemindex2;
	if ((strcmp(d, "G") == 0) && (itemindex4 != -1))	gold2 += itemindex4;
	if ((strcmp(e, "G") == 0) && (itemindex5 != -1))	gold2 += itemindex5;
	if (gold1 > CHAR_getMaxHaveGold( meindex) )	return	-4;
	if (gold1 > CHAR_getInt(meindex, CHAR_GOLD))	return	-5;
	if (gold2 > CHAR_getMaxHaveGold( toindex))	return	-6;
	if (gold2 > CHAR_getInt(toindex, CHAR_GOLD))	return	-7;
	if ((gold2 + CHAR_getInt(meindex, CHAR_GOLD) - gold1) > CHAR_getMaxHaveGold( meindex) )	return	-8;
	if ((gold1 + CHAR_getInt(toindex, CHAR_GOLD) - gold2) > CHAR_getMaxHaveGold( toindex))	return	-9;
	// 检查双方宠物	
	if ((strcmp(c, "P") == 0) && (itemindex3 != -1))
	{
		pet1--;	pet2++;
	}
	if ((strcmp(f, "P") == 0) && (itemindex6 != -1))
	{
		pet1++;	pet2--;
	}
	if ((pet1 > 1) || (pet2 > 1))	return	-10;
	if ((pet1 == 1) && (CHAR_getCharPetElement( meindex) == -1))	return -10;
	if ((pet2 == 1) && (CHAR_getCharPetElement( toindex) == -1))	return -11;
	if ((strcmp(a, "I") == 0) && (itemindex1 != -1))
		item1 = CHAR_getItemIndex(meindex, itemindex1);
	else	item1 = -1;
	if ((strcmp(b, "I") == 0) && (itemindex2 != -1))
		item2 = CHAR_getItemIndex(meindex, itemindex2);
	else	item2 = -1;
	if ((strcmp(d, "I") == 0) && (itemindex4 != -1))
		item4 = CHAR_getItemIndex(toindex, itemindex4);
	else	item4 = -1;
	if ((strcmp(e, "I") == 0) && (itemindex5 != -1))
		item5 = CHAR_getItemIndex(toindex, itemindex5);
	else	item5 = -1;
	if (itemindex3 != -1)
	{
		if( CHAR_getInt( meindex, CHAR_RIDEPET) == itemindex3 )
			return -1;
		pet3 = CHAR_getCharPet(meindex, itemindex3);
	}
	else 	pet3 = -1;
	if (itemindex6 != -1)
	{
		if( CHAR_getInt( toindex, CHAR_RIDEPET) == itemindex6 )
			return -1;
		pet6 = CHAR_getCharPet(toindex, itemindex6);
	}
	else	pet6 = -1;
	if ((ITEM_getInt(item1, ITEM_VANISHATDROP) == 1)||
		(ITEM_getInt(item2, ITEM_VANISHATDROP) == 1)||
		(ITEM_getInt(item4, ITEM_VANISHATDROP) == 1)||
		(ITEM_getInt(item5, ITEM_VANISHATDROP) ==1))
			return	-12;
	if( CHAR_getWorkInt( meindex, CHAR_PickAllPet) != TRUE )	{
		if ((pet6 != -1) && (CHAR_getInt(pet6, CHAR_LV)
			 - CHAR_getInt(meindex, CHAR_LV) > 5)
			 && (CHAR_getInt(meindex,CHAR_TRANSMIGRATION) <= 0))
		 		return -13;

	}
	if( CHAR_getWorkInt( toindex, CHAR_PickAllPet) != TRUE )	{

		if ((pet3 != -1) && (CHAR_getInt(pet3, CHAR_LV)
			 - CHAR_getInt(toindex, CHAR_LV) > 5)
			 && (CHAR_getInt(toindex,CHAR_TRANSMIGRATION) <= 0))
		 		return -14;
	}
	if ((pet6 != -1) && (CHAR_getInt(pet6, CHAR_PETFAMILY) == 1))
		 	return -15;
	if ((pet3 != -1) && (CHAR_getInt(pet3, CHAR_PETFAMILY) == 1))
		 	return -15;
	TRADE_ChangeItem(meindex, toindex, a, d, item1, item4,
		itemindex1, itemindex4);
	TRADE_ChangeItem(meindex, toindex, b, e, item2, item5,
		itemindex2, itemindex5);
	TRADE_ChangeItem(meindex, toindex, c, f, pet3, pet6,
		itemindex3, itemindex6);

	// 写入 trade.log
	{
		char petname1[256], petname2[256];
		int pet1lv, pet2lv;
		int logitem1 = 0, logitem2 = 0, logitem4 = 0, logitem5 = 0;

		if ((strcmp(a, "I") == 0) && (item1 != -1))	logitem1 = ITEM_getInt(item1, ITEM_ID);
		if ((strcmp(b, "I") == 0) && (item2 != -1))	logitem2 = ITEM_getInt(item2, ITEM_ID);
		if ((strcmp(a, "G") == 0) && (item1 == -1))	logitem1 = itemindex1;
		if ((strcmp(b, "G") == 0) && (item2 == -1))	logitem2 = itemindex2;
		if ((strcmp(c, "P") == 0) && (pet3 == -1))
		{
			sprintf(petname1, "NONE");
			pet1lv = 0;
		}
		else
		{
			sprintf(petname1, "%s", CHAR_getChar(pet3, CHAR_NAME));
			pet1lv = CHAR_getInt(pet3, CHAR_LV);
		}
		if ((strcmp(f, "P") == 0) && (pet6 == -1))
		{
			sprintf(petname2, "NONE");
			pet2lv = 0;
		}
		else
		{
			sprintf(petname2, "%s", CHAR_getChar(pet6, CHAR_NAME));
			pet2lv = CHAR_getInt(pet6, CHAR_LV);
		}
		if ((strcmp(d, "I") == 0) && (item4 != -1))	logitem4 = ITEM_getInt(item4, ITEM_ID);
		if ((strcmp(e, "I") == 0) && (item5 != -1))	logitem5 = ITEM_getInt(item5, ITEM_ID);
		if ((strcmp(d, "G") == 0) && (item4 == -1))	logitem4 = itemindex4;
		if ((strcmp(e, "G") == 0) && (item5 == -1))	logitem5 = itemindex5;
	        sprintf(buf, "%s\t%s\t(%s[%d,%s],%s[%d,%s],%s[%s,%d,%s]) <-> %s\t%s\t(%s[%d,%s],%s[%d,%s],%s[%s,%d,%s])",
                        CHAR_getChar(meindex, CHAR_CDKEY), mycharaname,
                        a, logitem1, ITEM_getChar( item1, ITEM_UNIQUECODE), b, logitem2, ITEM_getChar( item2, ITEM_UNIQUECODE),
						c, petname1, pet1lv, CHAR_getChar( pet3, CHAR_UNIQUECODE),
                        CHAR_getChar(toindex, CHAR_CDKEY), tocharaname,
                        d, logitem4, ITEM_getChar( item4, ITEM_UNIQUECODE), e, logitem5, ITEM_getChar( item5, ITEM_UNIQUECODE), 
						f, petname2, pet2lv, CHAR_getChar( pet6, CHAR_UNIQUECODE));
		LogTrade(buf);
	}
	
        return 1;
   }
	return 2;
}

BOOL TRADE_ChangeItem(int meindex, int toindex, char *a, char *b,
	int item1, int item2, int itemindex1, int itemindex2)
{
   int gold1 = 0, gold2 = 0, eptitem = -1;
   if (CHAR_CHECKINDEX(meindex) == FALSE)	return FALSE;
   if (CHAR_CHECKINDEX(toindex) == FALSE)	return FALSE;
   // 道具 <-> 道具
   if ((strcmp(a, "I") == 0) && (strcmp(b, "I") == 0))
   {
   	// 无 <-> 道具
   	if ((itemindex1 == -1) && (itemindex2 > 0))
   	{
   	   if (ITEM_CHECKINDEX(item2) == FALSE)	return FALSE;
   	   eptitem = CHAR_findEmptyItemBox(meindex);
   	   if (eptitem < 0)	return FALSE;
   	   CHAR_setItemIndex(meindex, eptitem, item2);
   	   ITEM_setWorkInt(item2, ITEM_WORKCHARAINDEX, meindex);
   	   ITEM_setWorkInt(item2, ITEM_WORKOBJINDEX, -1);
   	   CHAR_setItemIndex(toindex, itemindex2, -1);
   	}
   	// 道具 <-> 无
   	else if ((itemindex1 > 0) && (itemindex2 == -1))
   	{
   	   if (ITEM_CHECKINDEX(item1) == FALSE)	return FALSE;
   	   eptitem = CHAR_findEmptyItemBox(toindex);
   	   if (eptitem < 0)	return FALSE;
   	   CHAR_setItemIndex(toindex, eptitem, item1);
   	   ITEM_setWorkInt(item1, ITEM_WORKCHARAINDEX, toindex);
   	   ITEM_setWorkInt(item1, ITEM_WORKOBJINDEX, -1);
   	   CHAR_setItemIndex(meindex, itemindex1, -1);
   	}
   	// 道具 <-> 道具
   	else if (itemindex1 > 0 && itemindex2 > 0)
   	{
   	   if (ITEM_CHECKINDEX(item1) == FALSE)	return FALSE;
   	   if (ITEM_CHECKINDEX(item2) == FALSE)	return FALSE;
   	   ITEM_setWorkInt(item1, ITEM_WORKCHARAINDEX, toindex);
   	   ITEM_setWorkInt(item1, ITEM_WORKOBJINDEX, -1);
   	   ITEM_setWorkInt(item2, ITEM_WORKCHARAINDEX, meindex);
   	   ITEM_setWorkInt(item2, ITEM_WORKOBJINDEX, -1);
   	   CHAR_setItemIndex(toindex, itemindex2, item1);
   	   CHAR_setItemIndex(meindex, itemindex1, item2);
   	}
   }
   // 道具 <-> 金钱
   else if ((strcmp(a, "I") == 0) && (strcmp(b, "G") == 0))
   {
   	if (itemindex1 > 0)
   	{
      	   if (ITEM_CHECKINDEX(item1) == FALSE)	return FALSE;
      	   eptitem = CHAR_findEmptyItemBox(toindex);
      	   if (eptitem < 0)	return FALSE;
      	   ITEM_setWorkInt(item1, ITEM_WORKCHARAINDEX, toindex);
      	   ITEM_setWorkInt(item1, ITEM_WORKOBJINDEX, -1);
      	   CHAR_setItemIndex(toindex, eptitem, item1);
      	   CHAR_setItemIndex(meindex, itemindex1, -1);
   	}
	if (itemindex2 < 0)	itemindex2 = 0;
   	gold1 = CHAR_getInt(meindex, CHAR_GOLD);
   	gold1 += itemindex2;
   	CHAR_setInt(meindex, CHAR_GOLD, gold1);
   	gold2 = CHAR_getInt(toindex, CHAR_GOLD);
   	gold2 -= itemindex2;
   	CHAR_setInt(toindex, CHAR_GOLD, gold2);
   }
   // 金钱 <-> 道具
   else if ((strcmp(a, "G") == 0) && (strcmp(b, "I") == 0))
   {
   	if (itemindex2 > 0)
   	{
      	   if (ITEM_CHECKINDEX(item2) == FALSE)	return FALSE;
      	   eptitem = CHAR_findEmptyItemBox(meindex);
      	   if (eptitem < 0)	return FALSE;
      	   ITEM_setWorkInt(item2, ITEM_WORKCHARAINDEX, meindex);
      	   ITEM_setWorkInt(item2, ITEM_WORKOBJINDEX, -1);
      	   CHAR_setItemIndex(meindex, eptitem, item2);
      	   CHAR_setItemIndex(toindex, itemindex2, -1);
   	}
   	if (itemindex1 < 0)	itemindex1 = 0;
   	gold1 = CHAR_getInt(meindex, CHAR_GOLD);
   	gold1 -= itemindex1;
   	CHAR_setInt(meindex, CHAR_GOLD, gold1);
   	gold2 = CHAR_getInt(toindex, CHAR_GOLD);
   	gold2 += itemindex1;
   	CHAR_setInt(toindex, CHAR_GOLD, gold2);
   }
   // 金钱 <-> 金钱
   else if ((strcmp(a, "G") == 0) && (strcmp(b, "G") == 0))
   {
   	if (itemindex1 < 0)	itemindex1 = 0;
   	if (itemindex2 < 0)	itemindex2 = 0;
   	gold1 = CHAR_getInt(toindex, CHAR_GOLD);
   	gold1 = gold1 + itemindex1 - itemindex2;
   	CHAR_setInt(toindex, CHAR_GOLD, gold1);
   	gold2 = CHAR_getInt(meindex, CHAR_GOLD);
   	gold2 = gold2 + itemindex2 - itemindex1;
   	CHAR_setInt(meindex, CHAR_GOLD, gold2);
   }
   // 宠物 <-> 宠物
   else if ((strcmp(a, "P") == 0) && (strcmp(b, "P") == 0))
   {
	char category[8];
   	if ((item1 != -1) && (CHAR_CHECKINDEX(item1) == FALSE))	return FALSE;
   	if ((item2 != -1) && (CHAR_CHECKINDEX(item2) == FALSE))	return FALSE;

   	if ((itemindex1 != -1) && (itemindex1 == CHAR_getInt(meindex, CHAR_DEFAULTPET)))
   	{
   		int fd;
   		CHAR_setInt(meindex, CHAR_DEFAULTPET, -1);
   		fd = getfdFromCharaIndex(meindex);
		if (fd != -1){
			CHAR_setWorkInt( meindex, CHAR_WORK_PET0_STAT+itemindex1-1, 0);
	   		lssproto_KS_send(fd, itemindex1, 0);
		}
   	}
   	if ((itemindex2 != -1)&& (itemindex2 == CHAR_getInt(toindex, CHAR_DEFAULTPET)))
   	{
   		int tofd;
   		CHAR_setInt(toindex, CHAR_DEFAULTPET, -1);
   		tofd = getfdFromCharaIndex(toindex);
		if (tofd != -1){
			CHAR_setWorkInt( toindex, CHAR_WORK_PET0_STAT+itemindex1-1, 0);			
	   		lssproto_KS_send(tofd, itemindex2, 0);
		}
   	}

	if (item1 != -1)
		CHAR_setWorkInt(item1, CHAR_WORKPLAYERINDEX, toindex);
	if (item2 != -1)
		CHAR_setWorkInt(item2, CHAR_WORKPLAYERINDEX, meindex);
	if (itemindex1 == -1)
		itemindex1 = CHAR_getCharPetElement( meindex);
	if (itemindex2 == -1)
		itemindex2 = CHAR_getCharPetElement( toindex);
	CHAR_setCharPet(meindex, itemindex1, item2);
	CHAR_setCharPet(toindex, itemindex2, item1);
	if ((itemindex1 != -1) && (item1 != -1))	{

			CHAR_setChar(item1, CHAR_OWNERCDKEY,
				CHAR_getChar(toindex, CHAR_CDKEY));
			CHAR_setChar(item1, CHAR_OWNERCHARANAME,
				CHAR_getChar(toindex, CHAR_NAME));

		CHAR_complianceParameter(item1);
		CHAR_setInt(item1, CHAR_PUTPETTIME, 0);
	}
	if ((itemindex2 != -1) && (item2 != -1))	{

		CHAR_setChar(item2, CHAR_OWNERCDKEY,
			CHAR_getChar(meindex, CHAR_CDKEY));
		CHAR_setChar(item2, CHAR_OWNERCHARANAME,
			CHAR_getChar(meindex, CHAR_NAME));

		CHAR_complianceParameter(item2);
		CHAR_setInt(item2, CHAR_PUTPETTIME, 0);
	}
	snprintf(category, sizeof(category), "K%d", itemindex1);
	CHAR_sendStatusString(meindex, category);
	snprintf(category, sizeof(category), "W%d", itemindex1);
	CHAR_sendStatusString(meindex, category);
	snprintf(category, sizeof(category), "K%d", itemindex2);
	CHAR_sendStatusString(toindex, category);
	snprintf(category, sizeof(category), "W%d", itemindex2);
	CHAR_sendStatusString(toindex, category);
   }
   return	TRUE;
}

BOOL TRADE_HandleItem( int meindex, int showindex, char *message, char *outmess)
{
	char token[256];
	int item, itemindex;
	int fd = getfdFromCharaIndex( meindex );

	if( CONNECT_get_confirm( fd) == TRUE ) return FALSE;
	if( CHAR_getWorkInt( meindex, CHAR_WORKTRADEMODE) != CHAR_TRADE_TRADING
		|| CHAR_getWorkInt( meindex, CHAR_WORKTRADEMODE) == CHAR_TRADE_LOCK )
		return FALSE;
	if( getStringFromIndexWithDelim( message, "|", 6, token, sizeof( token)) == FALSE) return FALSE;
	item = atoi( token);
	if( item < CHAR_STARTITEMARRAY  || item > CHAR_MAXITEMHAVE ){
		return FALSE;
	}
	itemindex = CHAR_getItemIndex(meindex, item);
	if( !ITEM_CHECKINDEX( itemindex))	return FALSE;
	{
		char itemname[256], tmpbuf[256], tmpbuf1[256];
		int crushe;

		strcpy( itemname, ITEM_getChar(itemindex, ITEM_SECRETNAME));//ITEM_NAME
		crushe	= ITEM_getItemDamageCrusheED( itemindex);
		sprintf( token, "不会损坏");
		if( crushe >= 0 ) snprintf( token, sizeof(token), "%d%%", crushe );
		sprintf( outmess, "%d|%s|%s|%d|%s",
	   					ITEM_getInt( itemindex, ITEM_BASEIMAGENUMBER),
						makeEscapeString( ITEM_getChar( itemindex, ITEM_NAME), tmpbuf1, sizeof(tmpbuf1)),
						ITEM_getChar(itemindex,ITEM_EFFECTSTRING), item, token
						);
	}
	return TRUE;
}

BOOL TRADE_HandleGold( int meindex, int showindex, char *message, char *outmess)
{
	int gold, tmpgold;
	char token[256];
	int fd = getfdFromCharaIndex( meindex );
	if(CONNECT_get_confirm( fd)==TRUE)return FALSE;
	if (CHAR_getWorkInt(meindex, CHAR_WORKTRADEMODE) != CHAR_TRADE_TRADING
		|| CHAR_getWorkInt(meindex, CHAR_WORKTRADEMODE) == CHAR_TRADE_LOCK)
		return FALSE;
	tmpgold = CHAR_getInt(meindex, CHAR_GOLD);
	if (getStringFromIndexWithDelim(message, "|", 6, token, sizeof(token)) == FALSE) return FALSE;
	if( (gold = atoi(token)) < 0 ) return FALSE;
	if( gold > tmpgold )return FALSE;
	sprintf( outmess, "%d", gold);
	return TRUE;
}

BOOL TRADE_HandlePet( int meindex, int showindex, char *message, char *outmess)
{
	int havepetindex, petindex;
	char token[256], buf[256];
	int fd = getfdFromCharaIndex( meindex );
	if(CONNECT_get_confirm( fd)==TRUE)return FALSE;
	if (CHAR_getWorkInt(meindex, CHAR_WORKTRADEMODE) != CHAR_TRADE_TRADING
		|| CHAR_getWorkInt(meindex, CHAR_WORKTRADEMODE) == CHAR_TRADE_LOCK)
		return FALSE;
	if (getStringFromIndexWithDelim(message, "|", 6, token, sizeof(token)) == FALSE) return FALSE;
	if( (havepetindex = atoi(token)) < 0 ) return FALSE;
	petindex = CHAR_getCharPet(meindex, havepetindex);
	if( !CHAR_CHECKINDEX(petindex))	return FALSE;

	sprintf( token, "%d|%s|%d|%d|%d|%d|%d|%d",
			CHAR_getInt( petindex, CHAR_BASEIMAGENUMBER),
			makeEscapeString( CHAR_getUseName(petindex), buf, sizeof(buf)),
			CHAR_getInt( petindex, CHAR_LV),
			CHAR_getWorkInt( petindex, CHAR_WORKATTACKPOWER),
			CHAR_getWorkInt( petindex, CHAR_WORKDEFENCEPOWER),
			CHAR_getWorkInt( petindex, CHAR_WORKQUICK),
			havepetindex, 
			CHAR_getInt( petindex , CHAR_TRANSMIGRATION));
	sprintf( outmess , "%s|%d", token , CHAR_getWorkInt(petindex , CHAR_WORKMAXHP) );
	return TRUE;
}

