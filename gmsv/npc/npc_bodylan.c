#include "version.h"
#include <string.h>
#include "char.h"
#include "object.h"
#include "char_base.h"
#include "npcutil.h"
#include "lssproto_serv.h"
#include "npc_windowhealer.h"




/*
 *迭｛示犯奴仿件必□斥毛银尹壬  杀允月NPC
 * 引内  仄井仃日木凶日｝公及皿伊奶乩□卞
 * 仇及NPC及INDEX毛忡绣允月［
 *
 *迫｛NPC及璃曰匹失弁扑亦件互丐匀凶日｝失弁扑亦件毛粟仇仄凶皿伊奶乩□互
 *NPC及INDEX毛忡  仄化中月井民尼永弁
 *民尼永弁    卅日戚反失弁扑亦件及扑□弗件旦民尼永弁允月
 *涡侗及失弁扑亦件毛仄化中凶日皿伊奶乩□及市它件玄毛失永皿今六月［
 *市它件玄失永皿  ｝蝈扑□弗件旦    卅日隙烂及桦赭尺伐□皿今六月
 *
 */

enum {
	BODYLAN_E_COMMANDNUM = CHAR_NPCWORKINT1,	// 戊穴件玉及赢今
};


// 它奶件玉它乒□玉
enum{
	BODYLAN_WIN_FIRST,
	BODYLAN_WIN_LAST_GOOD,
	BODYLAN_WIN_LAST_NG,
	BODYLAN_WIN_GOOD_NO,
	BODYLAN_WIN_ALREADY,
	BODYLAN_WIN_NOT_PREEVENT,
	BODYLAN_WIN_END
};

static void NPC_BodyLan_Profit( int meindex, int playerindex );

static void NPC_BodyLan_Window(
	int meindex,
	int talkerindex,
	int mode
);


/*********************************
* 赓渝质  
*********************************/
BOOL NPC_BodyLanInit( int meindex )
{

	char szP[256], szArg[4096];
	char buf[256];
	int i, needSeq;

    CHAR_setInt( meindex , CHAR_WHICHTYPE , CHAR_TYPEEVENT );

    if( NPC_Util_GetArgStr( meindex, szArg, sizeof( szArg ) ) == NULL ){
    	print( "npc_bodylan.c:没有引数(%s)\n",
    		CHAR_getChar(meindex,CHAR_NPCARGUMENT) );
    	return FALSE;
    }


	// 扑□弗件旦毛茧允
	if( NPC_Util_GetStrFromStrWithDelim( szArg, "Act", szP, sizeof( szP ) ) == NULL ){
		print( "npc_bodylan:动作文字列尚未设定(%s)\n",	szArg );
		return FALSE;
	}

	for( i = 0 ; ; i ++ ){
		//   邰卅赐  
		if( getStringFromIndexWithDelim( szP, ",", i, buf, sizeof( buf)) != FALSE ){
			needSeq = atoi(buf);
			// 穴奶瓜旦互  凶日仇仇引匹
			if( needSeq < 0 ){
				if( i <= 0 ){
					print( "npc_bodylan:动作列尚未设定(%s)\n", szArg );
				}
				//     醒毛本永玄
				CHAR_setWorkInt( meindex, BODYLAN_E_COMMANDNUM, i );
				break;
			}else{
				// 伙□皿仄化醒尹月
			}
		}else{
			if( i <= 0 ){
				print( "npc_bodylan:动作列尚未设定(%s)\n", szArg );
			}
			//     醒毛本永玄
			CHAR_setWorkInt( meindex, BODYLAN_E_COMMANDNUM, i );
			break;
		}
	}



    return TRUE;

}




/*********************************
*   仄井仃日木凶凛及质  
*********************************/
void NPC_BodyLanTalked( int meindex , int talkerindex , char *szMes ,int color )
{
	char szP[256], szArg[4096];
	int EventNo = -1,Pre_Event = -1;

    /* 皿伊奶乩□卞覆仄化分仃  杀允月 */
    if( CHAR_getInt( talkerindex , CHAR_WHICHTYPE ) != CHAR_TYPEPLAYER ) {
    	return;
    }

	/* ㄠ弘伉永玉动  及心 */
	if( NPC_Util_CharDistance( talkerindex, meindex ) > 2 )
	{
		return;
	}

	// 娄醒  侬  
    if( NPC_Util_GetArgStr( meindex, szArg, sizeof( szArg ) ) == NULL ){
    	print( "npc_bodylan.c:没有引数(%s)\n",
    		CHAR_getChar(meindex,CHAR_NPCARGUMENT) );
    	return ;
    }else{
		// 奶矛件玄  寞潸  
		if( NPC_Util_GetStrFromStrWithDelim( szArg, "EventNo", szP, sizeof( szP ) ) != NULL ){
			EventNo = atoi(szP);
		}
		// 仪蟆  邰奶矛件玄  寞潸  
		if( NPC_Util_GetStrFromStrWithDelim( szArg, "Pre_Event", szP, sizeof( szP ) ) != NULL ){
			Pre_Event = atoi(szP);
		}
	}

	// 仪蟆卞  邰奶矛件玄  寞互丐木壬
	if( Pre_Event >= 0 ){
		// 奶矛件玄卞覆仄化升丹仄化中月井民尼永弁
		if( NPC_EventCheckFlg( talkerindex, Pre_Event ) == FALSE ){
			//   邰奶矛件玄毛弁伉失仄化中卅井匀凶日仇及本伉白
			NPC_BodyLan_Window( meindex, talkerindex, BODYLAN_WIN_NOT_PREEVENT );
			return;
		}
	}
	// 奶矛件玄  寞互丐木壬
	if( EventNo >= 0 ){
		// 奶矛件玄卞覆仄化升丹仄化中月井民尼永弁
		if( NPC_EventCheckFlg( talkerindex, EventNo ) == TRUE ){
			//   匀化中凶日仇及本伉白
			NPC_BodyLan_Window( meindex, talkerindex, BODYLAN_WIN_ALREADY );
			return;
		}
	}

	// 蟆荚仇及皿伊奶乩□反愤坌午韵匀化中凶井
	if( CHAR_getWorkInt( talkerindex, CHAR_WORKTRADER ) == meindex ){

		// 今日卞戊穴件玉互蝈  岳  仄化中凶日
		if( CHAR_getWorkInt( talkerindex, CHAR_WORKSHOPRELEVANT )
		 >= CHAR_getWorkInt( meindex, BODYLAN_E_COMMANDNUM )
		){
			// 岳  ″″仍    井＂
			NPC_BodyLan_Window( meindex, talkerindex, BODYLAN_WIN_LAST_GOOD );
			return;
		}else{
			// 撩  
			NPC_BodyLan_Window( meindex, talkerindex, BODYLAN_WIN_LAST_NG );
			return;
		}
	}else{
		// 赓户化卅及匹创尹月
		// 皿伊奶乩□卞愤坌及奶件犯永弁旦毛忡绣今六月
		CHAR_setWorkInt( talkerindex, CHAR_WORKTRADER, meindex );
		// 扑□弗件旦反  赓井日卞赓渝祭
		CHAR_setWorkInt( talkerindex, CHAR_WORKSHOPRELEVANT, 1 );

		// 公及端本伉白午井韵月屯五井卅丐’’’
		NPC_BodyLan_Window( meindex, talkerindex, BODYLAN_WIN_FIRST );
		return;
	}
}


/*=======================================
 * watch 质  
 *======================================*/
void NPC_BodyLanWatch(
	int objmeindex,
	int objmoveindex,
    CHAR_ACTION act,
    int x,
    int y,
    int dir,
    int* opt,
    int optlen
)
{
	char szP[256], szArg[4096];
	char buf[256];
	int actindex;
	int meindex;
	int seqNo, needSeq;

	// 平乓仿弁正动陆反伉正□件
	if( OBJECT_getType(objmoveindex) != OBJTYPE_CHARA ) return;
	actindex = OBJECT_getIndex(objmoveindex);
	// 皿伊奶乩□动陆反伉正□件
	if( CHAR_getInt( actindex, CHAR_WHICHTYPE ) != CHAR_TYPEPLAYER ) return;

	// 愤坌及奶件犯永弁旦
	meindex = OBJECT_getIndex(objmeindex);

	// 愤坌及INDEX毛忡  仄化中月井＂中卅仃木壬伉正□件
	if( CHAR_getWorkInt( actindex, CHAR_WORKTRADER ) != meindex ){
		return;
	}

	// 升仇引匹失弁扑亦件毛仄化中月井
	seqNo = CHAR_getWorkInt( actindex, CHAR_WORKSHOPRELEVANT );
	//   卅桦宁反  赓井日
	if( seqNo < 1 )seqNo = 1;

	// 娄醒  侬  
    if( NPC_Util_GetArgStr( meindex, szArg, sizeof( szArg ) ) == NULL ){
    	print( "npc_bodylan.c:没有引数(%s)\n",
    		CHAR_getChar(meindex,CHAR_NPCARGUMENT) );
    	return ;
    }


	// 扑□弗件旦毛茧允
	if( NPC_Util_GetStrFromStrWithDelim( szArg, "Act", szP, sizeof( szP ) ) == NULL ){
		print( "npc_bodylan:动作文字列尚未设定(%s)\n",	szArg );
		return;
	}

	//   邰卅赐  
	if(getStringFromIndexWithDelim( szP, ",", seqNo, buf, sizeof( buf)) != FALSE ){
		needSeq = atoi(buf);
	}else{
		// 卅兮井卅井匀凶日  赓井日
		CHAR_setWorkInt( actindex, CHAR_WORKSHOPRELEVANT, 1 );

//		print( "做过头了。回到最初。\n", seqNo, needSeq );
		// 窒井韵月屯五井’’’
		return;
	}

	// 漆荚  邰卅失弁扑亦件互域谯仄凶
	if( needSeq == act ){
		// 域谯仄凶［仇木互    井
//		print( "成功\(%d次数是%d)\n", seqNo, needSeq );
		seqNo ++;
		if( seqNo >= CHAR_getWorkInt( meindex, BODYLAN_E_COMMANDNUM ) ){
//			print( "在此结束。\n" );
		}
		// 仇及匏  毛忡绣
		CHAR_setWorkInt( actindex, CHAR_WORKSHOPRELEVANT, seqNo );
	}else{
		// 撩  仄凶桦宁反  赓井日支曰  仄
		CHAR_setWorkInt( actindex, CHAR_WORKSHOPRELEVANT, 1 );
//		print( "(%d次数是%d)\n", seqNo, needSeq );
	}

}



//********* 岳  凛及仍     *********
static void NPC_BodyLan_Profit( int meindex, int playerindex )
{
	char szArg[4096], szP[256];
	int fl, x, y, pmode, i, subindex, parent;

	// 娄醒  侬  
    if( NPC_Util_GetArgStr( meindex, szArg, sizeof( szArg ) ) == NULL ){
    	print( "npc_bodylan.c:没有引数(%s)\n",
    		CHAR_getChar(meindex,CHAR_NPCARGUMENT) );
    	return ;
    }

	//*********************************************
	//
	//   仍    公及ㄠ［伐□皿涩烂
	//
	//*********************************************
	if( NPC_Util_GetStrFromStrWithDelim( szArg, "Warp", szP, sizeof( szP ) ) != NULL ){
		// 仍    卞伐□皿［甄  潸月
		if( sscanf( szP, "%d,%d,%d", &fl, &x, &y ) == 3 ){
		}else{
			print( "npc_bodylan: 无法读取空间座标(%s)\n", szP );
			return;
		}
		// 由□  奴赚氏匹月桦宁阂间手伐□皿
		pmode = CHAR_getWorkInt( playerindex, CHAR_WORKPARTYMODE );
		switch( pmode ){
		case 1: // 愤坌互褪
			parent = playerindex;
			break;
		case 2: // 愤坌互阂间［褪毛潸  
			parent = CHAR_getWorkInt( playerindex, CHAR_WORKPARTYINDEX1 );
			break;
		default:
			// 由□  奴元扎卅井匀凶［愤坌分仃伐□皿
			CHAR_warpToSpecificPoint( playerindex, fl, x, y );
			return;
		}
		// 蝈够伐□皿
		for( i = 0; i < CHAR_PARTYMAX; i ++ ){
			subindex = CHAR_getWorkInt( parent, CHAR_WORKPARTYINDEX1+i );
			if( CHAR_CHECKINDEX( subindex ) == FALSE )continue;
			// 由□  奴蝈够伐□皿
			CHAR_warpToSpecificPoint( subindex, fl, x, y );
		}
	}



}

#if 1
static void NPC_BodyLan_Window(
	int meindex,
	int talkerindex,
	int mode
)
{
	char token[1024];
	char escapedname[2048];
	char szArg[4096];
	char szP[256];
	int fd;
	int buttontype = 0, windowtype = 0, windowno = 0;

	if( CHAR_CHECKINDEX( talkerindex ) == FALSE )return;
	fd = getfdFromCharaIndex( talkerindex );

	// 娄醒  侬  
    if( NPC_Util_GetArgStr( meindex, szArg, sizeof( szArg ) ) == NULL ){
    	print( "npc_bodylan.c:没有引数(%s)\n",
    		CHAR_getChar(meindex,CHAR_NPCARGUMENT) );
    	return ;
    }

	szP[0] = 0;

	switch( mode ){
	  case BODYLAN_WIN_FIRST:
		//   赓卞  今木凶桦宁及本伉白
		if( NPC_Util_GetStrFromStrWithDelim( szArg, "First", szP, sizeof( szP ) ) == NULL ){
			print( "npc_bodylan:一开始讲话的文字没有输入(%s)\n",	szArg );
			return;
		}
		sprintf( token,"%s", szP );
		buttontype=WINDOW_BUTTONTYPE_YES|WINDOW_BUTTONTYPE_NO;
	  	windowtype=WINDOW_MESSAGETYPE_MESSAGE;
	  	windowno=mode;
	  	break;

	  case BODYLAN_WIN_LAST_GOOD:
		//     匹恳荸分匀凶日
		if( NPC_Util_GetStrFromStrWithDelim( szArg, "Good", szP, sizeof( szP ) ) == NULL ){
			print( "npc_bodylan:答对时说的文字没有输入(%s)\n",	szArg );
			return;
		}
		sprintf( token,"%s", szP );
		buttontype=WINDOW_BUTTONTYPE_YESNO;// YES|NO
	  	windowtype=WINDOW_MESSAGETYPE_MESSAGE;
	  	windowno=mode;
	  	break;

	  case BODYLAN_WIN_LAST_NG:
		//     匹恳荸分匀凶日
		if( NPC_Util_GetStrFromStrWithDelim( szArg, "Ng", szP, sizeof( szP ) ) == NULL ){
			print( "npc_bodylan:答错时说的文字没有输入(%s)\n",	szArg );
			return;
		}
		sprintf( token,"%s", szP );
		buttontype=WINDOW_BUTTONTYPE_OK;// OK
	  	windowtype=WINDOW_MESSAGETYPE_MESSAGE;
	  	windowno=mode;
	  	break;

	  case BODYLAN_WIN_GOOD_NO:
		// 恳荸凛卞    毛平乓件本伙
		if( NPC_Util_GetStrFromStrWithDelim( szArg, "Good_No", szP, sizeof( szP ) ) == NULL ){
			print( "npc_bodylan:答对时取消奖品的文字没有输入(%s)\n",	szArg );
			return;
		}
		sprintf( token,"%s", szP );
		buttontype=WINDOW_BUTTONTYPE_OK;// OK
	  	windowtype=WINDOW_MESSAGETYPE_MESSAGE;
	  	windowno=mode;
	  	break;

	  case BODYLAN_WIN_ALREADY:
		// 允匹卞奶矛件玄毛蔽  仄化中凶桦宁
		if( NPC_Util_GetStrFromStrWithDelim( szArg, "Good_No", szP, sizeof( szP ) ) == NULL ){
			print( "npc_bodylan:事件结束时的文字没有输入(%s)\n",	szArg );
			return;
		}
		sprintf( token,"%s", szP );
		buttontype=WINDOW_BUTTONTYPE_OK;	// OK
	  	windowtype=WINDOW_MESSAGETYPE_MESSAGE;
	  	windowno=mode;
	  	break;

	case BODYLAN_WIN_NOT_PREEVENT:
		// 仪蟆卞  邰卅奶矛件玄毛仇卅仄化中卅中桦宁
		if( NPC_Util_GetStrFromStrWithDelim( szArg, "Pre_Not", szP, sizeof( szP ) ) == NULL ){
			print( "npc_bodylan:?事前事件结束的文字没有输入(%s)\n",	szArg );
			return;
		}
		sprintf( token,"%s", szP );
		buttontype=WINDOW_BUTTONTYPE_OK;	// OK
	  	windowtype=WINDOW_MESSAGETYPE_MESSAGE;
	  	windowno=mode;
	  	break;
	  default:
	  	return;
	}

	makeEscapeString( token, escapedname, sizeof(escapedname));
	/*-仇仇匹霜耨允月--*/
	lssproto_WN_send( fd, windowtype,
					buttontype,
					windowno,
					CHAR_getWorkInt( meindex, CHAR_WORKOBJINDEX),
					escapedname);


}



/*-----------------------------------------
弁仿奶失件玄井日忒匀化五凶凛卞裟太请今木月［
-------------------------------------------*/
void NPC_BodyLanWindowTalked(
	int meindex,
	int talkerindex,
	int seqno,
	int select,
	char *data
)
{


	if( NPC_Util_CharDistance( talkerindex, meindex ) > 2) return;

	switch( seqno){
	case BODYLAN_WIN_LAST_GOOD:	// 恳荸凛卞OK瓷今木凶日
		if(select==WINDOW_BUTTONTYPE_YES ){
			NPC_BodyLan_Profit( meindex, talkerindex );
			// 皿伊奶乩□卞愤坌及奶件犯永弁旦毛侉木今六月
			CHAR_setWorkInt( talkerindex, CHAR_WORKTRADER, -1 );
			// 扑□弗件旦反  赓井日卞赓渝祭
			CHAR_setWorkInt( talkerindex, CHAR_WORKSHOPRELEVANT, 1 );
		}else
		if( select == WINDOW_BUTTONTYPE_NO ){
			// 恳荸凛卞平乓件本伙今木凶日
			NPC_BodyLan_Window( meindex, talkerindex, BODYLAN_WIN_GOOD_NO );
			// 皿伊奶乩□卞愤坌及奶件犯永弁旦毛侉木今六月
			CHAR_setWorkInt( talkerindex, CHAR_WORKTRADER, -1 );
			// 扑□弗件旦反  赓井日卞赓渝祭
			CHAR_setWorkInt( talkerindex, CHAR_WORKSHOPRELEVANT, 1 );
		}
		break;
	default:
		break;
	}

}



#endif

