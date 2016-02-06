#include "version.h"
#include <string.h>
#include "common.h"
#include "char_base.h"
#include "npc_janken.h"
#include "npcutil.h"
#include "char.h"
#include "lssproto_serv.h"
#include "buf.h"
#include "function.h"
#include "readmap.h"
#include "object.h"
#include "log.h"

/*
 *元扎氏仃氏
 */
static void NPC_Janken_selectWindow( int meindex, int talker, int num);
void NPC_JnakenJudge(int meindex,int talker,int sel);
BOOL NPC_JankenEntryItemCheck(int talker,char *buf);
BOOL NPC_JankenEntryItemDel(int talker,char *buf);
void NPC_WarpPointGet(int meindex,int talker,int *fl,int *x,int *y,int judge);


/*********************************
* 赓渝质  
*********************************/
BOOL NPC_JankenInit( int meindex )
{

	//正奶皿毛本永玄
    CHAR_setInt( meindex , CHAR_WHICHTYPE , CHAR_TYPEJANKEN );
	
	return TRUE;

}


/*********************************
*   仄井仃日木凶凛及质  
*********************************/
void NPC_JankenTalked( int meindex , int talkerindex , char *szMes ,
					 int color )
{
	/* 皿伊奶乩□卞覆仄化分仃  杀允月 */
	if( CHAR_getInt( talkerindex , CHAR_WHICHTYPE ) != CHAR_TYPEPLAYER ) {
		return;
	}
	
	/* ㄠ弘伉永玉动  及心 */
	if( NPC_Util_CharDistance( talkerindex, meindex ) > 1) return;

	NPC_Janken_selectWindow(meindex, talkerindex, 0);

	
	

}



/******************************
 *	公木冗木及质  卞坌仃月
 ******************************/
static void NPC_Janken_selectWindow( int meindex, int talker, int num)
{

	char token[1024];
	char buf[32];
	int fd = getfdFromCharaIndex( talker);
	int buttontype=0;
	int windowtype=0;
	int windowno=0;
   	char argstr[NPC_UTIL_GETARGSTR_BUFSIZE];
	

	/*--它奶件玉它正奶皿丢永本□斥互云云中及匹燮卞涩烂--*/
  	windowtype=WINDOW_MESSAGETYPE_MESSAGE;

	/*--涩烂白央奶伙毛  心  心--*/
	if(NPC_Util_GetArgStr( meindex, argstr, sizeof(argstr))==NULL){
		print("NPC_janken Init: GetArgStrErr");
		return ;
	}


	switch( num)
	{
	  case 0:

	  	/*-- 旦正□玄及丢永本□斥互  仃木壬巨仿□  憎 --*/
		if(NPC_Util_GetStrFromStrWithDelim( argstr, "MainMsg", token, sizeof( token))	== NULL)
		{
			print("Janken:MainMsg:%s", CHAR_getChar( meindex, CHAR_NAME));
			return ;
		}

  		/*--蓟      --*/
	  	buttontype = WINDOW_BUTTONTYPE_YESNO;
	  	windowtype = WINDOW_MESSAGETYPE_MESSAGE;
	  	windowno = CHAR_WINDOWTYPE_JANKEN_START; 
	  	break;
	
	
	case 1:
		
		//失奶  丞及民尼永弁
		if(NPC_Util_GetStrFromStrWithDelim( argstr, "EntryItem", buf, sizeof( buf))!= NULL) {
	
			if(NPC_JankenEntryItemCheck(talker,buf) == FALSE)
			{
				NPC_Janken_selectWindow(meindex, talker, 3);
			}

			//巨件玄伉迕及失奶  丞毛绰轮
			if(NPC_JankenEntryItemDel(talker,buf) == FALSE){
				print("元扎氏仃氏ERR:失奶  丞壅仄卞仄匀天中");
			}
		}


		/*--蓟      --*/
		sprintf(token,"              剪刀石头布！！\n"
			      "\n\n               《  石头  》"
			      "\n\n               《  剪刀  》"
				  "\n\n               《  布  》"
		);

		buttontype=WINDOW_BUTTONTYPE_NONE;
		windowtype=WINDOW_MESSAGETYPE_SELECT;
	  	windowno=CHAR_WINDOWTYPE_JANKEN_MAIN; 
	  break;

	case 2:
		/*--丐中仇蓟      --*/
		//sprintf(token,"              丐中仇匹″″\n"
		sprintf(token,"              平手再来！！\n"
			      "\n\n               《  石头  》"
			      "\n\n               《  剪刀  》"
				  "\n\n               《  布  》"
		);

	  	buttontype=WINDOW_BUTTONTYPE_NONE;
	  	windowtype=WINDOW_MESSAGETYPE_SELECT;
 		windowno=CHAR_WINDOWTYPE_JANKEN_MAIN; 
	  	break;

  	case 3:
	  	/*-- 旦正□玄及丢永本□斥互  仃木壬巨仿□  憎 --*/
		NPC_Util_GetStrFromStrWithDelim( argstr, "NoItem", token, sizeof( token));

	  	buttontype=WINDOW_BUTTONTYPE_OK;
	  	windowtype=WINDOW_MESSAGETYPE_MESSAGE;
  		windowno=CHAR_WINDOWTYPE_JANKEN_END; 

  	break;

	}


//	makeEscapeString( token, escapedname, sizeof(escapedname));
	/*-仇仇匹霜耨允月--*/
	lssproto_WN_send( fd, windowtype, 
					buttontype, 
					windowno,
					CHAR_getWorkInt( meindex, CHAR_WORKOBJINDEX),
					token);



}



/*********************************
* 它奴件玉它丢永本□斥卞覆允月质  
*********************************/
void NPC_JankenWindowTalked( int meindex, int talkerindex, 
								int seqno, int select, char *data)
{
	/* ㄠ弘伉永玉动  及心 */
	if( NPC_Util_CharDistance( talkerindex, meindex ) > 1) return;

	switch( seqno){
	  case CHAR_WINDOWTYPE_JANKEN_START:
	
	  	if(select==WINDOW_BUTTONTYPE_YES){
			NPC_Janken_selectWindow(meindex, talkerindex, 1);
		}
		break;	
	
	case CHAR_WINDOWTYPE_JANKEN_MAIN:
		if (atoi(data) >= 3){
			NPC_JnakenJudge(meindex,talkerindex,atoi(data) );
		}
		break;
	}
}


/*
 *斥乓件弗件及  烂
 */
void NPC_JnakenJudge(int meindex,int talker,int sel)
{
	int player=-1;
	int jankenman;
	char j_char[3][8]={" 石头 "," 剪刀 ","  布  "};
	char token[1024];
	int shouhai = 0;
	int fd = getfdFromCharaIndex( talker);
	int fl=0,x=0,y=0;
	
	
	if(sel == 3) player = 0; //弘□
	if(sel == 5) player = 1; //民亦平
	if(sel == 7) player = 2; //由□

	jankenman = rand()%3;

	switch(jankenman){
		case 0:
			if(player == 2){
				shouhai = 1;
			}else if(player == 1){
				shouhai = 2;
			}
		break;
		
		case 1:
			if(player == 0){
				shouhai = 1;
			}else if(player == 2){
				shouhai = 2;
			}
		break;
		
		case 2:
			if(player == 1){
				shouhai = 1;
			}else if(player == 0){
				shouhai = 2;
			}
		break;
	}

	if(shouhai == 1){
		//Change add 玩家猜胜了给的道具
		NPC_JankenItemGet( meindex, talker, "WinItem" );

		 NPC_WarpPointGet(meindex, talker, &fl, &x, &y, 0);
		
		snprintf( token, sizeof( token ) ,
						"              ～　结  果　～      \n\n"
						"  %16s      %-16s\n"
						"        [%s]    VS    [%s]\n\n\n"
						"      %-16s 胜利",
						 CHAR_getChar(meindex,CHAR_NAME),CHAR_getChar(talker,CHAR_NAME),
						 j_char[jankenman],j_char[player],
						 CHAR_getChar(talker,CHAR_NAME)
						);

//			    CHAR_talkToCli( talker , -1 ,token , CHAR_COLORCYAN );

				//伐□皿
			    CHAR_warpToSpecificPoint(talker, fl, x, y);
			    
			    //  匀凶午五反＞减少＝及失弁扑亦件毛允月［
			    CHAR_sendWatchEvent( CHAR_getWorkInt( talker, CHAR_WORKOBJINDEX),	CHAR_ACTPLEASURE,NULL,0,TRUE);
				CHAR_setWorkInt( talker, CHAR_WORKACTION, CHAR_ACTPLEASURE);

	}else if(shouhai == 2){

		//Change add 玩家猜输了给的道具
		NPC_JankenItemGet( meindex, talker, "LoseItem" );

		 NPC_WarpPointGet(meindex, talker, &fl, &x, &y, 1);
		snprintf( token, sizeof( token ) ,
						"              ～　结  果　～      \n\n"
						"  %16s      %-16s\n"
						"        [%s]    VS    [%s]\n\n\n"
						"      %-16s 输了",
						 CHAR_getChar(meindex,CHAR_NAME),CHAR_getChar(talker,CHAR_NAME),
						 j_char[jankenman],j_char[player],
						 CHAR_getChar(talker,CHAR_NAME)
						);
//			    CHAR_talkToCli( talker , -1 ,token , CHAR_COLORCYAN );

				//伐□皿
				CHAR_warpToSpecificPoint(talker, fl, x, y);

				//  仃凶午五反＞  仄中＝失弁扑亦件毛允月
			    CHAR_sendWatchEvent( CHAR_getWorkInt( talker, CHAR_WORKOBJINDEX),	CHAR_ACTSAD,NULL,0,TRUE);
				CHAR_setWorkInt( talker, CHAR_WORKACTION, CHAR_ACTSAD);


	}else{
		//仇仇卞仁月午丐中仇
		NPC_Janken_selectWindow( meindex, talker, 2);
		return;
	}
	
	//霜耨
	lssproto_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE, 
					WINDOW_BUTTONTYPE_OK, 
					CHAR_WINDOWTYPE_JANKEN_END,
					CHAR_getWorkInt( meindex, CHAR_WORKOBJINDEX),
					token);


	
}


/*
 *伐□皿燮毛GET允月［
 */
void NPC_WarpPointGet(int meindex,int talker,int *fl,int *x,int *y,int judge)
{
   	char argstr[NPC_UTIL_GETARGSTR_BUFSIZE];
	char *strbuf[2] = {"WinWarp","LoseWarp"};
	char buf[64];
	char buf2[32];

	/*--涩烂白央奶伙毛  心  心--*/
	if(NPC_Util_GetArgStr( meindex, argstr, sizeof(argstr))==NULL){
		print("NPC_janken Init: GetArgStrErr");
		return ;
	}


	/*--伐□皿燮毛潸  --*/
	NPC_Util_GetStrFromStrWithDelim( argstr, strbuf[judge], buf, sizeof( buf));
	getStringFromIndexWithDelim(buf,",",1,buf2,sizeof(buf2));
	*fl=atoi(buf2);
	getStringFromIndexWithDelim(buf,",",2,buf2,sizeof(buf2));
	*x=atoi(buf2);
	getStringFromIndexWithDelim(buf,",",3,buf2,sizeof(buf2));
	*y=atoi(buf2);
	
	
}

/*
 *--失奶  丞及民尼永弁分仃毛垫丹
 */
BOOL NPC_JankenEntryItemCheck(int talker,char *buf)
{

	char buf2[512];
	char buf3[256];
	int id=0;
	BOOL flg = FALSE;
	int i;
	int itemindex;
	int itemno;
	int kosuu;
	int cnt=0;
	int k=1;
	
	while(getStringFromIndexWithDelim(buf , "," , k, buf2, sizeof(buf2))
	 !=FALSE )
	{
		flg = FALSE;
		k++;
		
		if(strstr(buf2,"*") != NULL){
			cnt = 0;
			getStringFromIndexWithDelim(buf2,"*",1,buf3,sizeof(buf3));
			itemno = atoi(buf3);
			getStringFromIndexWithDelim(buf2,"*",2,buf3,sizeof(buf3));
			kosuu = atoi(buf3);
		
			for( i=0 ; i < CHAR_MAXITEMHAVE;i++ ){
				itemindex = CHAR_getItemIndex( talker , i );
				if( ITEM_CHECKINDEX(itemindex) ){
					id = ITEM_getInt(itemindex ,ITEM_ID );
					if(itemno == id){
						cnt++;
						if(cnt == kosuu){
							flg = TRUE;
							break;
						}
					}
				}
			}
			if(flg == FALSE)
			{
				return FALSE;	
			}
		}else{
			itemno = atoi(buf2);
			
			for( i=0 ; i < CHAR_MAXITEMHAVE;i++ ){
				itemindex = CHAR_getItemIndex( talker , i );
				if( ITEM_CHECKINDEX(itemindex) ){
					id = ITEM_getInt(itemindex ,ITEM_ID );
					if(itemno == id){
						flg = TRUE;
						break;
					}
				}
			}
	
			if(flg == FALSE)
			{
				return FALSE;
			}
		}
	}

	return TRUE;

}

BOOL NPC_JankenEntryItemDel(int talker,char *buf)
{

	int i = 1, j = 1,k = 1;
	char buff3[128];
	char buf2[32];
	int itemindex;

	while(getStringFromIndexWithDelim(buf , "," , k, buff3, sizeof(buff3)) !=FALSE ){
		k++;
		if(strstr(buff3, "*") !=NULL){
			int itemno;
			int kosuu;
			int id;
			int cnt=0;
			getStringFromIndexWithDelim(buff3,"*",1,buf2,sizeof(buf2));
			itemno = atoi(buf2);
			getStringFromIndexWithDelim(buff3,"*",2,buf2,sizeof(buf2));
			kosuu = atoi(buf2);
				for( i =0 ; i < CHAR_MAXITEMHAVE ; i++ ){
				itemindex = CHAR_getItemIndex( talker , i );
				if( ITEM_CHECKINDEX(itemindex) ){
					id=ITEM_getInt(itemindex ,ITEM_ID );
					if(itemno==id){
						cnt++;
						LogItem(
							CHAR_getChar( talker, CHAR_NAME ), /* 平乓仿   */
							CHAR_getChar( talker, CHAR_CDKEY ),
#ifdef _add_item_log_name  // WON ADD 在item的log中增加item名称
							itemindex,
#else
							ITEM_getInt( itemindex, ITEM_ID),  /* 失奶  丞  寞 */
#endif
							"QuizDelItem(猜谜->交出道具)",
							CHAR_getInt( talker, CHAR_FLOOR),
							CHAR_getInt( talker, CHAR_X ),
 							CHAR_getInt( talker, CHAR_Y ),
							ITEM_getChar( itemindex, ITEM_UNIQUECODE),
							ITEM_getChar( itemindex, ITEM_NAME),
							ITEM_getInt( itemindex, ITEM_ID)
						);
						CHAR_DelItem( talker, i);
						if(cnt == kosuu){
							break;
						}
					}
				}
			}		
		}else{
			/*--蓟氏分  寞及失奶  丞毛蓟请---*/
			for( j = 0 ;  j < CHAR_MAXITEMHAVE ; j++){
				itemindex = CHAR_getItemIndex( talker ,j);

				if( ITEM_CHECKINDEX(itemindex) ){
					if( atoi( buff3) == ITEM_getInt(itemindex,ITEM_ID)){
						LogItem(
							CHAR_getChar( talker, CHAR_NAME ), /* 平乓仿   */
							CHAR_getChar( talker, CHAR_CDKEY ),
#ifdef _add_item_log_name  // WON ADD 在item的log中增加item名称
							itemindex,
#else
							ITEM_getInt( itemindex, ITEM_ID),  /* 失奶  丞  寞 */
#endif
							"QuizDelItem(猜谜->交出道具)",
							CHAR_getInt( talker,CHAR_FLOOR),
							CHAR_getInt( talker,CHAR_X ),
							CHAR_getInt( talker,CHAR_Y ),
							ITEM_getChar( itemindex, ITEM_UNIQUECODE),
							ITEM_getChar( itemindex, ITEM_NAME),
							ITEM_getInt( itemindex, ITEM_ID)
						);
						CHAR_DelItem( talker, j);
					}
				}
			}
		}
	}

	return TRUE;
}

BOOL NPC_JankenItemGet(int meindex,int talker, char *wl)
{
	char argstr[NPC_UTIL_GETARGSTR_BUFSIZE];
	char buf[64];

	/*--涩烂白央奶伙毛  心  心--*/
	if(NPC_Util_GetArgStr( meindex, argstr, sizeof(argstr))==NULL){
		print("NPC_janken ItemGet: GetArgStrErr");
		return FALSE;
	}

	if(NPC_Util_GetStrFromStrWithDelim( argstr, wl, buf, sizeof( buf) ) !=NULL){
			NPC_EventAddItem( meindex, talker, buf);
	}
}

