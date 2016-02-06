#include "version.h"
#include <string.h>
#include "common.h"
#include "char_base.h"
#include "npc_quiz.h"
#include "npcutil.h"
#include "char.h"
#include "lssproto_serv.h"
#include "buf.h"
#include "function.h"
#include "readmap.h"
#include "object.h"
#include "log.h"

#define MEPLAYER	8
#define OLDNO		100

enum {
	CHAR_WORK_PLAYER1	= CHAR_NPCWORKINT1,
	CHAR_WORK_PLAYER2	= CHAR_NPCWORKINT2,
	CHAR_WORK_PLAYER3	= CHAR_NPCWORKINT3,
	CHAR_WORK_PLAYER4	= CHAR_NPCWORKINT4,
	CHAR_WORK_PLAYER5	= CHAR_NPCWORKINT5,
	CHAR_WORK_PLAYER6	= CHAR_NPCWORKINT6,
	CHAR_WORK_PLAYER7	= CHAR_NPCWORKINT7,
	CHAR_WORK_PLAYER8	= CHAR_NPCWORKINT8,
	CHAR_WORK_QUIZNUM	= CHAR_NPCWORKINT9,
};

NPC_QUIZ* Quiz;

struct pl{
	int talkerindex;
	int quizno;
	int answer;
	int ansno;
	int oldno[OLDNO];
	int *ptr;
};


static int quizcnt = 0;


static void NPC_Quiz_selectWindow( int meindex, int talker, int num);
int *NPC_GetQuestion(int meindex);
BOOL NPC_QuizItemFullCheck(int meindex, int talker);
BOOL NPC_PlayerCheck(int meindex,int talker);
int NPC_RealyCheack(int meindex,int talker);
int NPC_QuizRandAnswer(int p_no,int q_no, int mode,
						char *work1,char *work2,char *work3);
int NPC_QuizBorderLine(int ans, char *buf);
int NPC_QuizWarpBorderLine(int ans, char *buf);

BOOL NPC_QuizAddItem(int talker,char *buf);

BOOL NPC_EntryItemDel(int talker,char *buf);
BOOL NPC_EntryItemCheck(int talker,char *buf);
BOOL NPC_EntryStoneDel(int talker,char *buf);
BOOL NPC_EntryStoneCheck(int talker,char *buf);
BOOL NPC_QUIZPARTY_CHAECK(int meindex,int talker);

BOOL NPC_QuizInit( int meindex )
{

	char argstr[NPC_UTIL_GETARGSTR_BUFSIZE];
	char buf[1024];
	int *tbl;
	int i;

	NPC_Util_GetArgStr( meindex, argstr, sizeof( argstr));

	if(NPC_Util_GetStrFromStrWithDelim( argstr, "StartMsg", buf, sizeof( buf)) == NULL){
		print("Quiz_StartMsgErr:%s", CHAR_getChar( meindex, CHAR_NAME));
		return FALSE;
	}

	if(NPC_Util_GetStrFromStrWithDelim( argstr, "Quiznum", buf, sizeof( buf)) == NULL){
		print("QuizNumErrErr:%s", CHAR_getChar( meindex, CHAR_NAME));
		return FALSE;
	}

	CHAR_setWorkInt( meindex, CHAR_WORK_QUIZNUM, atoi( buf));

	for(i =0  ; i < 8 ; i++){
		CHAR_setWorkInt( meindex, CHAR_WORK_PLAYER1 + i, -1);
	}
	tbl =  NPC_GetQuestion( meindex);

	if( CHAR_getWorkInt( meindex, CHAR_WORK_QUIZNUM) > ( tbl[0] - 1)){
		print(
		 "%s不符合条件,圆圈quiz的问题数(%d? 比出题数(%d题)不足",
			CHAR_getChar( meindex, CHAR_NAME),
			(tbl[0]-1),
			CHAR_getWorkInt( meindex, CHAR_WORK_QUIZNUM)
		);
		return FALSE;
	}

	return TRUE;

}

void NPC_QuizTalked( int meindex , int talkerindex , char *szMes ,
					 int color )
{
	if( CHAR_getInt( talkerindex , CHAR_WHICHTYPE ) != CHAR_TYPEPLAYER ) {
		return;
	}
	if( NPC_Util_CharDistance( talkerindex, meindex ) > 1) return;

	if(NPC_QUIZPARTY_CHAECK(meindex,talkerindex)==FALSE){
		NPC_Quiz_selectWindow( meindex, talkerindex, 4);
	
	}
	if(NPC_QuizItemFullCheck(meindex, talkerindex) == FALSE){
		NPC_Quiz_selectWindow( meindex, talkerindex, 3);
	}else{
		NPC_Quiz_selectWindow( meindex, talkerindex, 0);
	}
}

static void NPC_Quiz_selectWindow( int meindex, int talker, int num)
{

	char argstr[NPC_UTIL_GETARGSTR_BUFSIZE];
	char token[1024];
	char buf[256];
  	char buf2[256];
  	char buf3[256];
  	
	int i = 0,k;
	int questionno = 0;
	int fd = getfdFromCharaIndex( talker);
	int buttontype = 0;
	int messagetype = 0;
	int windowno = 0;
	int len = 4;
	int p_no;
	int p_old;
	int loop=0;
	int ans;
	BOOL entryflg = TRUE;
	switch( num) {
	  case 0:
		if(NPC_Util_GetArgStr( meindex, argstr, sizeof( argstr)) == NULL){
			print("GetArgstrErr:Quiz %s",CHAR_getChar( meindex, CHAR_NAME));
		}
		if(NPC_Util_GetStrFromStrWithDelim( argstr,"StartMsg",token, sizeof( token)) == NULL){
			print("%s的StartMsg不存在", CHAR_getChar(meindex,CHAR_NAME));
			return;
		}
		buttontype = WINDOW_BUTTONTYPE_YESNO;
		messagetype = WINDOW_MESSAGETYPE_MESSAGE;
		windowno = CHAR_WINDOWTYPE_QUIZ_START;
		break;
	
	  case 1:
		if(NPC_Util_GetArgStr( meindex, argstr, sizeof( argstr)) == NULL){
			print("GetArgstrErr:Quiz %s",CHAR_getChar( meindex, CHAR_NAME));
		}
		if(NPC_Util_GetStrFromStrWithDelim( argstr,"EntryItem",	buf2, sizeof( buf2)) != NULL){
			if(NPC_EntryItemCheck( talker, buf2) == FALSE){
				entryflg = FALSE;
			}
		}
		if(NPC_Util_GetStrFromStrWithDelim( argstr,"EntryStone",buf3, sizeof( buf3)) != NULL){
			if(NPC_EntryStoneCheck( talker, buf3) == FALSE){
				entryflg = FALSE;
			}
		}
		if(entryflg == FALSE){
			if(NPC_Util_GetStrFromStrWithDelim( argstr,"NoEntryMsg",token, sizeof( token)) != NULL) {
				buttontype = WINDOW_BUTTONTYPE_OK;
				messagetype = WINDOW_MESSAGETYPE_MESSAGE;
				windowno = CHAR_WINDOWTYPE_QUIZ_START;

			}
		
		}else if(NPC_PlayerCheck( meindex, talker) == FALSE) {
			/*--谛醒左□田□---*/
			sprintf(token,"\n\n\n        请,稍等一下"
						      "\n｛｛ ㄠ蘸卞公氏卅卞锹澎匹五卅中井日"
			);
			buttontype = WINDOW_BUTTONTYPE_OK;
			messagetype = WINDOW_MESSAGETYPE_MESSAGE;
			windowno = CHAR_WINDOWTYPE_QUIZ_START;

		}else{
			if(NPC_Util_GetStrFromStrWithDelim( argstr,"EntryItem",buf2, sizeof( buf2)) != NULL){
				if(NPC_EntryItemDel( talker, buf2)	==FALSE){
					print("道具没有消失(%s)",CHAR_getChar(meindex,CHAR_NAME));
					return;
				}
			}
			if(NPC_Util_GetStrFromStrWithDelim( argstr,"EntryStone",buf3, sizeof( buf3)) != NULL){
				if(NPC_EntryStoneDel( talker, buf3)	==FALSE){
					print("金钱没有消失(%s)",CHAR_getChar(meindex,CHAR_NAME));
					return;
				}
			}
			NPC_Quiz_selectWindow(meindex, talker, 2);
			return;
		}
	  	break;

	  case 3:
		if(NPC_Util_GetArgStr( meindex, argstr, sizeof( argstr)) == NULL){
			print("GetArgstrErr:Quiz %s",CHAR_getChar( meindex, CHAR_NAME));
		}

		/*-- 旦正□玄丢永本□斥 --*/
		if(NPC_Util_GetStrFromStrWithDelim( argstr,"ItemFullMsg",	token, sizeof( token)) == NULL)
		{
			print("%s的ItemFullMsg不存在", CHAR_getChar(meindex,CHAR_NAME));
			return;
		}
		
		buttontype = WINDOW_BUTTONTYPE_OK;
		messagetype = WINDOW_MESSAGETYPE_MESSAGE;
		windowno = CHAR_WINDOWTYPE_QUIZ_START;

	  	break;
	  case 2:
  		/*--丢奶件--*/
  		{
			int *tbl;
			int point;
			int *pl_ptr;
			int warp_flg = -1;
			int floor=0,x=0,y=0;

			struct pl PLAYER;
			
			
			if(NPC_QUIZPARTY_CHAECK(meindex,talker)==FALSE){
				NPC_Quiz_selectWindow( meindex, talker, 4);
			}

			
			
			p_no = CHAR_getWorkInt( talker, CHAR_WORKSHOPRELEVANT);
			point = CHAR_getWorkInt( meindex, CHAR_WORK_PLAYER1 + p_no);
			pl_ptr = (int *)point;
			
			memcpy(&PLAYER, pl_ptr, sizeof(struct pl));
			
			if(PLAYER.quizno
			 == CHAR_getWorkInt( meindex, CHAR_WORK_QUIZNUM))
			{
				double avg;
				int okflg;
				char buf2[512];

				if(NPC_Util_GetArgStr( meindex, argstr, sizeof( argstr)) == NULL)
				{
					print("GetArgstrErr:Quiz %s",CHAR_getChar( meindex, CHAR_NAME));
				}

				if(NPC_Util_GetStrFromStrWithDelim( argstr, "GetItem", buf, sizeof( buf) )!=NULL)
				{
					okflg = NPC_QuizBorderLine(PLAYER.answer,buf);
					if(okflg == -1)
					{
				
					}else{

						getStringFromIndexWithDelim(buf , "," , okflg+1, buf3, sizeof(buf3));
						if(NPC_QuizAddItem( talker, buf3) == FALSE) return ;

					}
				}

				if(NPC_Util_GetStrFromStrWithDelim( argstr, "Border", buf, sizeof( buf) )!=NULL)
				{
					okflg = NPC_QuizBorderLine(PLAYER.answer,buf);
					if(okflg == -1)
					{
						NPC_Util_GetStrFromStrWithDelim( argstr, "FailureMsg",
							buf2,sizeof(buf2));
				
							CHAR_talkToCli( talker, meindex, buf2, CHAR_COLORWHITE);
				
					}else{

						getStringFromIndexWithDelim(buf , "," , okflg+1, buf2, sizeof(buf2));
						CHAR_talkToCli( talker, meindex, buf2, CHAR_COLORWHITE);
					}
				}

				
				/*--恳荸醒卞杀元化伐□皿今六月---*/
				if(NPC_Util_GetStrFromStrWithDelim( argstr, "Warp", buf, sizeof( buf) )!=NULL)
				{
					warp_flg = NPC_QuizWarpBorderLine(PLAYER.answer,buf);
					if(warp_flg != -1) {
						char warpbuf[32];
						
						getStringFromIndexWithDelim(buf , "," , warp_flg+1, buf2, sizeof(buf2));
						getStringFromIndexWithDelim(buf2 , "." , 1, warpbuf, sizeof(warpbuf));
						floor = atoi(warpbuf);
						getStringFromIndexWithDelim(buf2 , "." , 2, warpbuf, sizeof(warpbuf));
						x = atoi(warpbuf);
						getStringFromIndexWithDelim(buf2 , "." , 3, warpbuf, sizeof(warpbuf));
						y = atoi(warpbuf);
					}
				}


				
				avg = (double)PLAYER.answer /
					CHAR_getWorkInt( meindex, CHAR_WORK_QUIZNUM) * 100;

				CHAR_setWorkInt( meindex, CHAR_WORK_PLAYER1 + 
							CHAR_getWorkInt( talker, CHAR_WORKSHOPRELEVANT) ,-1);
	
//				print("\n %s %d开放",CHAR_getChar(talker,CHAR_NAME),(int)PLAYER.ptr);

				free(PLAYER.ptr);
				
				if(warp_flg != -1){
					CHAR_warpToSpecificPoint(talker, floor, x, y);
				}
				return;
				
			}

			tbl =  NPC_GetQuestion( meindex);
			
			
			if( CHAR_getWorkInt( meindex, CHAR_WORK_QUIZNUM) > ( tbl[0] - 1))
			{
				free(PLAYER.ptr);
				return ;
			}

			while(1){
				loop++;
				i = rand()%( tbl[0]);
				if(i == 0) continue;
				questionno = tbl[i];
		
				for( k = 0 ; k < OLDNO ; k++){
					if(PLAYER.oldno[k] == questionno) {
						loop = 0;
						break;
					}
				}
				if(k == OLDNO) break;
				
				if(loop == 5000) {
					print("问题不足i全部重覆了j");
				 	break;
				}
			}

			p_no = CHAR_getWorkInt( talker, CHAR_WORKSHOPRELEVANT);
			p_old = CHAR_getWorkInt( talker, CHAR_WORKSHOPRELEVANTSEC);

			PLAYER.quizno++;
			
			PLAYER.oldno[p_old] = questionno;
			p_old++;
			CHAR_setWorkInt( talker, CHAR_WORKSHOPRELEVANTSEC, p_old);

			strncpy( buf, Quiz[questionno].question, 
							sizeof( Quiz[questionno].question));

			if(strlen( buf) < 40) {
				strcat( buf, "\n");
			}else if(strlen( buf) > 79) {
				len = 5;
			}	

			/*--签贿穴永民--*/
			if( Quiz[questionno].answertype == 4) {
				
				if(strlen( buf) < 40) {
					strcat( buf,"\n\n");
				}else if( strlen( buf) < 80) {
					strcat( buf,"\n\n");
				}else{
					strcat( buf,"\n");
				}

				sprintf( token,"   quiz 第 %d 题"
						  	"\n%s "
				  			"\n↓写在这里\n"
				  			"?(写完请按ＯＫ)",
				  			PLAYER.quizno,
				  			buf);

				buttontype = WINDOW_BUTTONTYPE_OK;
				messagetype = WINDOW_MESSAGETYPE_MESSAGEANDLINEINPUT;
				windowno = CHAR_WINDOWTYPE_QUIZ_MAIN;

				/*--蜇箕及橇须毛丢乒伉卞筏盛今六化云仁--*/
				memcpy(pl_ptr,&PLAYER,sizeof(struct pl));
				
				break;
			
			}else if( Quiz[questionno].answertype == 2) {
				char work1[128];
				char work2[128];
				char work3[128];

				ans = (rand() % 3) + 1;

				ans = NPC_QuizRandAnswer(p_no,questionno,3, work1,work2,work3);
				PLAYER.ansno = ans;

				sprintf(token,"%d\n   quiz 第 %d 题"
						  	"\n%s "
							"\n\n１． %s"
							"\n２． %s"
				  			"\n３． %s",
				  			len,
				  			PLAYER.quizno,
				  			buf,
				  			work1,
				  			work2,
				  			work3
				);
				
			}else if( Quiz[questionno].answertype == 1){
				char work1[128];
				char work2[128];
				char work3[128];
				
				strcpy(work3,"-1");
				ans = NPC_QuizRandAnswer(p_no,questionno,2, work1,work2,work3);
				PLAYER.ansno = ans;

				sprintf(token,"%d\n   quiz 第 %d 题"
							"\n%s "
							"\n\n１． %s"
							"\n２． %s",
							len,
							PLAYER.quizno,
							buf,
							work1,
							work2
				);
			}
			buttontype = WINDOW_BUTTONTYPE_CANCEL;
		  	messagetype = WINDOW_MESSAGETYPE_SELECT;
			windowno = CHAR_WINDOWTYPE_QUIZ_MAIN;

			memcpy(pl_ptr,&PLAYER,sizeof(struct pl));

		}
		break;
		
	  case 4:
		if(NPC_Util_GetArgStr( meindex, argstr, sizeof( argstr)) == NULL)
		{
			print("GetArgstrErr:Quiz %s",CHAR_getChar( meindex, CHAR_NAME));
		}

		if(NPC_Util_GetStrFromStrWithDelim( argstr,"Party",	token, sizeof( token)) != NULL) 
		{
			buttontype = WINDOW_BUTTONTYPE_OK;
			messagetype = WINDOW_MESSAGETYPE_MESSAGE;
			windowno = CHAR_WINDOWTYPE_QUIZ_START;
		}

		break;

	}
	 
	/*-仇仇匹霜耨允月--*/
	lssproto_WN_send( fd, messagetype, 
					buttontype, 
					windowno,
					CHAR_getWorkInt( meindex, CHAR_WORKOBJINDEX),
					token);
}

void NPC_QuizWindowTalked( int meindex, int talkerindex, 
								int seqno, int select, char *data)
{
	int i=0;
	int old_no;

	if( NPC_Util_CharDistance( talkerindex, meindex ) > 1) {
		return;
	}
	switch( seqno){
	  case CHAR_WINDOWTYPE_QUIZ_START:
		if( select == WINDOW_BUTTONTYPE_YES) {
			NPC_Quiz_selectWindow(meindex, talkerindex, 1);
		}
		
		if(select == WINDOW_BUTTONTYPE_OK){
		}
		
		break;

	  case CHAR_WINDOWTYPE_QUIZ_MAIN:
	  {
	  	int point;
		int *pl_ptr;
		int p_no;
		struct pl PLAYER;
		
			p_no = CHAR_getWorkInt( talkerindex, CHAR_WORKSHOPRELEVANT);
			point = CHAR_getWorkInt( meindex, CHAR_WORK_PLAYER1 + p_no);
			pl_ptr = (int *)point;
		

			memcpy(&PLAYER,pl_ptr,sizeof(struct pl));

			p_no = CHAR_getWorkInt( talkerindex, CHAR_WORKSHOPRELEVANT);
			old_no = CHAR_getWorkInt( talkerindex, CHAR_WORKSHOPRELEVANTSEC)-1;
			i=	PLAYER.oldno[old_no];

			if(select == WINDOW_BUTTONTYPE_OK){
				if( i >= quizcnt || i < 0 ) return;
				if( Quiz == NULL || strlen( Quiz[i].select1) >= 128 ) return;

				if( strstr( data, Quiz[i].select1) != NULL) {
					PLAYER.answer++;
				}
				memcpy(pl_ptr,&PLAYER,sizeof(struct pl));
				NPC_Quiz_selectWindow(meindex, talkerindex, 2);
			}else if(select == WINDOW_BUTTONTYPE_CANCEL){
				CHAR_setWorkInt( meindex, CHAR_WORK_PLAYER1 + 
							CHAR_getWorkInt( talkerindex, CHAR_WORKSHOPRELEVANT) ,-1);
				free(PLAYER.ptr);
			}else if( atoi( data) == 0){
			}else{
				if(atoi(data) == PLAYER.ansno){
					PLAYER.answer++;
				}
				memcpy(pl_ptr,&PLAYER,sizeof(struct pl));
				NPC_Quiz_selectWindow(meindex, talkerindex, 2);
			}
	  }
	  break;
	
	}


}

/*
 *示□母□仿奶件及民尼永弁
 */
int NPC_QuizBorderLine(int ans ,char *buf)
{
	char buf2[32];
	int j = 1;
	int border;
	
	
	while(getStringFromIndexWithDelim(buf , "," , j, buf2, sizeof( buf2))
	 !=FALSE )
	{
		border = atoi(buf2);
	
		if(ans >= border) {
			return j;
		}
	
		j += 2;

	}
	return -1;
}
/*
 *示□母□仿奶件及民尼永弁
 */
int NPC_QuizWarpBorderLine(int ans ,char *buf)
{
	char buf2[32];
	int j = 1;
	int border;
	
	
	while(getStringFromIndexWithDelim(buf , "," , j, buf2, sizeof( buf2))
	 != FALSE )
	{
		border = atoi(buf2);
	
		if(ans >= border) {
			return j;
		}
	
		j += 2;

	}
	return -1;
}


/*
 */
BOOL NPC_QuizAddItem(int talker,char *buf)
{
	int i=1;
	int ret;
	char buf2[16];
	int itemindex;
	char token[512];
	
	while(getStringFromIndexWithDelim(buf , "." , i, buf2, sizeof(buf2))
	!= FALSE )
	{
		i++;
	}
	i--;
	if(i == 1){
		getStringFromIndexWithDelim(buf , "." , i, buf2, sizeof(buf2));
	}else{
		/*--仿件母丞--*/
		i = (rand() % i)+1;
		getStringFromIndexWithDelim(buf , "." , i, buf2, sizeof(buf2));
	}

	itemindex = ITEM_makeItemAndRegist(atoi(buf2));

	if(itemindex == -1) return FALSE;

	ret = CHAR_addItemSpecificItemIndex( talker, itemindex);
	if( !CHAR_CHECKITEMINDEX( talker, ret) ){
		print( "npc_quiz.c: additem error itemindex[%d]\n", itemindex);
		ITEM_endExistItemsOne( itemindex);
	
		CHAR_talkToCli( talker, -1, "道具栏已满", CHAR_COLORWHITE);
			
		return FALSE;
	}

	if(itemindex != -1) {
		LogItem(
			CHAR_getChar( talker, CHAR_NAME ), /* 平乓仿   */
			CHAR_getChar( talker, CHAR_CDKEY ),
#ifdef _add_item_log_name  // WON ADD 在item的log中增加item名称
			itemindex,
#else
    		ITEM_getInt( itemindex, ITEM_ID),  /* 失奶  丞  寞 */
#endif
			"QuizAddItem(猜谜->领到的道具)",
			CHAR_getInt( talker, CHAR_FLOOR),
			CHAR_getInt( talker, CHAR_X ),
			CHAR_getInt( talker, CHAR_Y ),
			ITEM_getChar( itemindex, ITEM_UNIQUECODE),
			ITEM_getChar( itemindex, ITEM_NAME),
			ITEM_getInt( itemindex, ITEM_ID)
		);
	}
	
	sprintf(token,"拿到%s。", ITEM_getChar( itemindex, ITEM_NAME));
	CHAR_talkToCli( talker, -1, token, CHAR_COLORWHITE);


	CHAR_sendItemDataOne( talker, ret);
	return TRUE;

}

int  NPC_QuizRandAnswer(int p_no,int q_no, int mode,
						char *work1,char *work2,char *work3)
{
	int ans;
	int ra;

	ans = (rand() % mode) + 1;
	if(ans == 1) {
		if( Quiz[q_no].answerNo == 1) {
			strcpy( work1, Quiz[q_no].select1);
			if(mode == 2) {
				strcpy( work2, Quiz[q_no].select2);
				return ans;
			}
			ra = rand()%2;

			if(ra == 0) {
				strcpy( work2, Quiz[q_no].select2);
				strcpy( work3, Quiz[q_no].select3);
			}else{
				strcpy( work2, Quiz[q_no].select3);
				strcpy( work3, Quiz[q_no].select2);
			}
		}else if( Quiz[q_no].answerNo == 2) {
			strcpy( work1, Quiz[q_no].select2);
			if(mode == 2) {
				strcpy( work2, Quiz[q_no].select1);
				return ans;
			}

			ra = rand()%2;
			if(ra == 0) {
				strcpy( work2, Quiz[q_no].select1);
				strcpy( work3, Quiz[q_no].select3);
			}else{
				strcpy( work2, Quiz[q_no].select3);
				strcpy( work3, Quiz[q_no].select1);
			}
		}else{
			strcpy( work1, Quiz[q_no].select3);
			ra = rand()%2;
			if(ra == 0) {
				strcpy( work2, Quiz[q_no].select1);
				strcpy( work3, Quiz[q_no].select2);
			}else{
				strcpy( work2, Quiz[q_no].select1);
				strcpy( work3, Quiz[q_no].select2);
			}
		}
	}else if(ans == 2) {
		if(Quiz[q_no].answerNo == 1) {
			strcpy( work2, Quiz[q_no].select1);
			if(mode == 2) {
				strcpy( work1, Quiz[q_no].select2);
				return ans;
			}
			ra=rand()%2;
			if(ra == 0) {
				strcpy( work1, Quiz[q_no].select2);
				strcpy( work3, Quiz[q_no].select3);
			}else {
				strcpy( work1, Quiz[q_no].select3);
				strcpy( work3, Quiz[q_no].select2);
			}
		}else if( Quiz[q_no].answerNo == 2) {
			strcpy( work2, Quiz[q_no].select2);
			if(mode == 2){
				strcpy( work1, Quiz[q_no].select1);
				return ans;
			}
			ra = rand()%2;
			if(ra == 0) {
				strcpy( work1, Quiz[q_no].select1);
				strcpy( work3, Quiz[q_no].select3);
			}else {
				strcpy( work1, Quiz[q_no].select3);
				strcpy( work3, Quiz[q_no].select1);
			}
		}else {
			strcpy( work2, Quiz[q_no].select3);
			ra = rand()%2;
			if(ra == 0) {
				strcpy( work1, Quiz[q_no].select1);
				strcpy( work3, Quiz[q_no].select2);
			}else {
				strcpy( work1, Quiz[q_no].select2);
				strcpy( work3, Quiz[q_no].select1);
			}
		}
	}else if(ans == 3) {
		if( Quiz[q_no].answerNo == 1) {
			strcpy( work3, Quiz[q_no].select1);
			ra = rand()%2;
			if(ra == 0) {
				strcpy( work1, Quiz[q_no].select2);
				strcpy( work2, Quiz[q_no].select3);
			}else {
				strcpy( work1, Quiz[q_no].select3);
				strcpy( work2, Quiz[q_no].select2);
			}
		}else if( Quiz[q_no].answerNo == 2) {
			strcpy( work3, Quiz[q_no].select2);
			ra = rand()%2;
			if(ra == 0) {
				strcpy( work1, Quiz[q_no].select1);
				strcpy( work2, Quiz[q_no].select3);
			}else {
				strcpy( work1, Quiz[q_no].select3);
				strcpy( work2, Quiz[q_no].select1);
			}
		}else{
			strcpy( work3, Quiz[q_no].select3);
			ra = rand()%2;
			if(ra == 0) {
				strcpy( work1, Quiz[q_no].select1);
				strcpy( work2, Quiz[q_no].select2);
			}else {
				strcpy( work1, Quiz[q_no].select2);
				strcpy( work2, Quiz[q_no].select1);
			}
		}
	}
	return ans;

}


BOOL NPC_PlayerCheck(int meindex,int talker)
	{
	int i;
	int k=0;
	int *ptr;
	struct pl Player;


	NPC_RealyCheack( meindex, talker);
	
	for(i = 0 ; i < MEPLAYER ; i++){
		if( (CHAR_getWorkInt( meindex, CHAR_WORK_PLAYER1 +i ) == -1))
		{
			break;
		}
	}

	if(i == MEPLAYER) return FALSE;


	/*-- 丢乒伉割忡 --*/
	if((ptr =(int *) calloc( 1,sizeof(struct pl)))  == NULL){
        print("记忆体无法确保%s",CHAR_getChar(meindex,CHAR_NAME));
    	return FALSE;
	}

    /*-- 禾奶件正及桦赭毛创尹化云仁??-*/
    Player.ptr = ptr;
    Player.talkerindex = talker;	//npc及奶件犯永弁旦???
	Player.quizno = 0;		//蜇箕及  锁醒
	Player.answer = 0;		//恳荸醒
	Player.ansno = 0;		//蚕尹及  寞毛创尹化云仁  蚕尹毛田仿田仿卞请仄凶端卞银迕)

    for(k=0 ; k < OLDNO; k++) {
		Player.oldno[k] = -1;
	}

	/*--丢乒伉毛戊疋□--*/
	memcpy(ptr,&Player,sizeof(struct pl));


	CHAR_setWorkInt( talker, CHAR_WORKSHOPRELEVANTSEC,0);
	CHAR_setWorkInt( talker, CHAR_WORKSHOPRELEVANT,0);
	CHAR_setWorkInt( talker, CHAR_WORKSHOPRELEVANTTRD,0);


	CHAR_setWorkInt( talker, CHAR_WORKSHOPRELEVANT, i);

	k= (int)ptr;
	CHAR_setWorkInt( meindex, CHAR_WORK_PLAYER1 + i, k );
	return TRUE;

}

int NPC_RealyCheack(int meindex,int talker)
{
	int fl, x, y;
	int i, j;
	int px[10] = {0, 1, 0,-1, 1,-1, 1, 0,-1};
	int py[10] = {0,-1,-1,-1, 0, 0, 1, 1, 1};
	int objindex;
	
	OBJECT object;
	int talkerindex;
	BOOL okflg = FALSE;
	struct pl PLAYER;
	int *pl_ptr;
	int point;
	
	fl = CHAR_getInt( meindex, CHAR_FLOOR);

	for(j = 0 ; j < MEPLAYER ; j++) {
		point = CHAR_getWorkInt( meindex, CHAR_WORK_PLAYER1 + j);
		okflg = FALSE;
		if(point == -1) continue;

		pl_ptr = (int *)point;
			
		memcpy(&PLAYER, pl_ptr, sizeof(struct pl));
		talkerindex = PLAYER.talkerindex;
		
		for(i=0 ; i < 10 ; i++) {
			x = px[i] + CHAR_getInt( meindex, CHAR_X);
			y = py[i] + CHAR_getInt( meindex, CHAR_Y);
			
			for(object = MAP_getTopObj(fl,x,y) ; object;
						object = NEXT_OBJECT( object))
			{
				objindex = GET_OBJINDEX( object);

				if( OBJECT_getType( objindex)    == OBJTYPE_CHARA ){
					if( OBJECT_getIndex( objindex) == talkerindex) {
						if(talkerindex == talker) {
							okflg = TRUE;
//							print("\n %s %d开放",CHAR_getChar(talker,CHAR_NAME),(int)PLAYER.ptr);
							free(PLAYER.ptr);
							CHAR_setWorkInt( meindex, CHAR_WORK_PLAYER1 + j,-1);
						}else{
							okflg = TRUE;
						}
						break;
					}
				}
			}	
			if(okflg == TRUE) break;
		}

		if(okflg ==FALSE){
//			print("\n %s %d开放",CHAR_getChar(talker,CHAR_NAME),(int)PLAYER.ptr);
			free(PLAYER.ptr);
			CHAR_setWorkInt( meindex, CHAR_WORK_PLAYER1 + j,-1);
		}
		
	}

	return -1;
}

BOOL QUIZ_initQuiz( char *filename)
{

    FILE*	f;
    char	line[512];
	char buf2[512], buf3[128];
	char buf[512];
	int i, linenum=0;
    f = fopen(filename,"r");
    if( f == NULL ){
        print( "file open error\n");
        return FALSE;
    }
    while( fgets( line, sizeof( line ), f ) ){
        if( line[0] == '#' )continue;
        if( line[0] == '\n' )continue;
        chomp( line );
        linenum ++;
    	quizcnt++;
    }

    if( fseek( f, 0, SEEK_SET ) == -1 ){
        fprint( "Seek Error\n" );
        fclose(f);
        return FALSE;
    }

	Quiz = allocateMemory( sizeof(NPC_QUIZ) * (quizcnt+1) );
	if( Quiz == NULL ){
		fprint( "Can't allocate Memory %d\n", sizeof(Quiz) * (quizcnt+1));
		fclose( f );
		return FALSE;
	}
    linenum = 0;
	memset( buf3, 0, sizeof(buf3));
    while( fgets( line, sizeof( line ), f ) ){
        if( line[0] == '#' )continue;
        if( line[0] == '\n' )continue;
        chomp( line );
        replaceString( line, '\t' , ' ' );
        for( i = 0; i < strlen( line); i ++) {
            if( line[i] != ' ' ) {
                break;
            }
            strcpy( buf, &line[i]);
        }
        if( i != 0 ) {
            strcpy( line, buf);
        }

		if( getStringFromIndexWithDelim( line, ",", 1, buf2, sizeof(buf2)) == FALSE ) continue;
		Quiz[linenum].no = atoi(buf2);
		
		if( getStringFromIndexWithDelim( line, ",", 2, buf2, sizeof(buf2)) == FALSE ) continue;
		Quiz[linenum].type = atoi(buf2);

		if( getStringFromIndexWithDelim( line, ",", 3, buf2, sizeof(buf2)) == FALSE ) continue;
		Quiz[linenum].level = atoi(buf2);
		
		if( getStringFromIndexWithDelim( line, ",", 4, buf2, sizeof(buf2)) == FALSE ) continue;
		Quiz[linenum].answertype =	atoi(buf2);
		
		if( getStringFromIndexWithDelim( line, ",", 5, buf2, sizeof(buf2)) == FALSE ) continue;
		Quiz[linenum].answerNo = atoi(buf2);
	
		if( getStringFromIndexWithDelim( line, ",", 6, buf2, sizeof(buf2)) == FALSE ) continue;
		memcpy( Quiz[linenum].question, buf2, sizeof( Quiz[linenum].question));


		if( getStringFromIndexWithDelim( line, ",", 7, buf3, sizeof(buf3)) == FALSE ) continue;
		memcpy( Quiz[linenum].select1, buf3, sizeof( Quiz[linenum].select1));
		if( getStringFromIndexWithDelim( line, ",", 8, buf3, sizeof(buf3)) == FALSE ) continue;
		memcpy( Quiz[linenum].select2, buf3, sizeof( Quiz[linenum].select2));
		if( getStringFromIndexWithDelim( line, ",", 9, buf3, sizeof(buf3)) == FALSE ) continue;
		memcpy( Quiz[linenum].select3, buf3, sizeof( Quiz[linenum].select3));
		
		if(Quiz[linenum].answertype == 1){
			if(Quiz[linenum].answerNo == 3){
				return FALSE;
			}
		}

		if(Quiz[linenum].answertype == 4){
			if(Quiz[linenum].answerNo != 1){
				return FALSE;
			}
		}
	        linenum ++;
//		quizcnt++;
	}
	if( quizcnt < linenum ){
		print("ERR linenum:%d >= quizcnt:%d\n", linenum, quizcnt);
		return FALSE;
	}

	quizcnt = linenum;
	fclose( f);
	return TRUE;

}

int *NPC_GetQuestion(int meindex)
{
	char argstr[NPC_UTIL_GETARGSTR_BUFSIZE];
	int i,j;
	int type=0;
	int answer=0;
	int level=0;
	int *tbl;
	char buf[16];
	NPC_Util_GetArgStr( meindex, argstr, sizeof( argstr));

	if(NPC_Util_GetStrFromStrWithDelim(argstr,"Type",buf, sizeof( buf)) != NULL ){
		type = atoi(buf);
	}
	if( type <= 0){
		type = 0xffff;
	}

	if(NPC_Util_GetStrFromStrWithDelim( argstr, "Answer", buf, sizeof( buf)) != NULL){
		answer = atoi(buf);
	}
	if(answer <= 0){
		answer = 0xffff;
	}

	if(NPC_Util_GetStrFromStrWithDelim( argstr, "Level", buf, sizeof( buf)) != NULL ){
		level = atoi(buf);
	}
	if(level <= 0){
		level = 0xffff;
	}
	for(j=0,i=0; i < quizcnt ;i++){
		if( (type & (1 << (Quiz[i].type-1)))  != (1 << (Quiz[i].type-1))){
			continue;
		}
		if((answer & (Quiz[i].answertype))  != Quiz[i].answertype){
			continue;
		}
		if((level & (Quiz[i].level))  != Quiz[i].level){
			continue;
		}
		j++;
	}

	{
		int tmp_tbl[j+1];
		tmp_tbl[0] = j+1;
		for(j=1,i=0; i < quizcnt ;i++){
			if( (type & (1 << (Quiz[i].type-1)))  != (1 << (Quiz[i].type-1))){
				continue;
			}

			if((answer & (Quiz[i].answertype))  != Quiz[i].answertype){
				continue;
			}

			if((level & (Quiz[i].level))  != Quiz[i].level){
				continue;
			}
			tmp_tbl[j] = i;

			j++;
		}

		tbl = tmp_tbl;
		return tbl;
	}
}

BOOL NPC_QuizItemFullCheck(int meindex,int talker)
{
	int i;
	int itemindex;
	char argstr[NPC_UTIL_GETARGSTR_BUFSIZE];
	char buf2[32];
	
		
	for( i = CHAR_STARTITEMARRAY ; i < CHAR_MAXITEMHAVE ; i++ ) {
		itemindex = CHAR_getItemIndex( talker , i );
		if( !ITEM_CHECKINDEX( itemindex) ) {
		 	return TRUE;
		 }
	}

	if(NPC_Util_GetArgStr( meindex, argstr, sizeof( argstr)) == NULL)
	{
		print("GetArgstrErr:Quiz %s",CHAR_getChar( meindex, CHAR_NAME));
	}

	if(NPC_Util_GetStrFromStrWithDelim( argstr,"EntryItem",	buf2, sizeof( buf2)) != NULL)
	{
		if(NPC_EntryItemCheck( talker, buf2) == FALSE){
			NPC_Quiz_selectWindow( meindex, talker, 3);
			return  FALSE;
			
		}else{
			return TRUE;
		}
	}


	return FALSE;

}


/*
 *云嗯及民尼永弁
 */
BOOL NPC_EntryStoneCheck(int talker,char *buf)
{
	int stone;
	int cost;
	
	cost= atoi( buf);
	stone = CHAR_getInt( talker, CHAR_GOLD);

	if(cost > stone){
		return FALSE;
	}
	
	return TRUE;
}

/*
 *云嗯及绰轮
 */
BOOL NPC_EntryStoneDel(int talker,char *buf)
{
	int stone;
	int cost;
	char token[256];
	
	cost= atoi( buf);
	stone = CHAR_getInt( talker, CHAR_GOLD);
	
	if((stone-cost) < 0){
		CHAR_setInt( talker, CHAR_GOLD, 0);
	}else{
		CHAR_DelGold( talker, cost );
	}

	sprintf( token, "%d 交出stone。", cost);
	CHAR_talkToCli( talker, -1, token, CHAR_COLORWHITE);

	CHAR_send_P_StatusString( talker, CHAR_P_STRING_GOLD);
	return TRUE;
}


/*
 */
BOOL NPC_EntryItemDel(int talker,char *buf)
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
							CHAR_getChar( talker, CHAR_NAME ), /* 平乓仿抩 */
							CHAR_getChar( talker, CHAR_CDKEY ),
#ifdef _add_item_log_name  // WON ADD 在item的log中增加item名称
							itemindex,
#else
							ITEM_getInt( itemindex, ITEM_ID),  /* 失奶泛丞?寞 */
#endif
							"QuizDelItem(猜谜->交出的道具)",
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
			for( j = 0 ;  j < CHAR_MAXITEMHAVE ; j++){
				itemindex = CHAR_getItemIndex( talker ,j);

				if( ITEM_CHECKINDEX(itemindex) ){
					if( atoi( buff3) == ITEM_getInt(itemindex,ITEM_ID)){
						LogItem(
							CHAR_getChar( talker, CHAR_NAME ), /* 平乓仿抩 */
							CHAR_getChar( talker, CHAR_CDKEY ),
#ifdef _add_item_log_name  // WON ADD 在item的log中增加item名称
							itemindex,
#else
							ITEM_getInt( itemindex, ITEM_ID),  /* 失奶泛丞?寞 */
#endif
							"QuizDelItem(猜谜->领到的道具)",
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

/*
 */
BOOL NPC_EntryItemCheck(int talker,char *buf)
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

/*
 */
BOOL NPC_QUIZPARTY_CHAECK(int meindex,int talker)
{

	if(CHAR_getWorkInt(talker,CHAR_WORKPARTYMODE) != CHAR_PARTY_NONE){
		return FALSE;
	}

	return TRUE;

}

