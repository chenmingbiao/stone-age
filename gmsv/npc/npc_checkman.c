#include "version.h"
#include <string.h>
#include "char.h"
#include "object.h"
#include "char_base.h"
#include "npcutil.h"
#include "lssproto_serv.h"


//蜇箕及银迕匹五月白仿弘及醒
#define MAXEVENTFLG 96

/*
 * 奶矛件玄及白仿弘毛民尼永弁允月      
 *
 */
 
static void NPC_CheckMan_selectWindow( int meindex, int toindex, int num);
int NPC_NowFlgCheck(int meindex,int talker,int now[MAXEVENTFLG]);
int NPC_EndFlgCheck(int meindex,int talker ,int nowflg[MAXEVENTFLG]);
BOOL NPC_FlgCheckMain( int meindex,int talker,int nowindex,int now[MAXEVENTFLG],char *work2);

/*********************************
* 赓渝质  
*********************************/
BOOL NPC_CheckManInit( int meindex )
{
	/*--平乓仿及正奶皿毛涩烂--*/
    CHAR_setInt( meindex , CHAR_WHICHTYPE , CHAR_TYPECHECKMAN);

	return TRUE;
}


/*********************************
*   仄井仃日木凶凛及质  
*********************************/
void NPC_CheckManTalked( int meindex , int talkerindex , char *szMes ,int color )
{

    /* 皿伊奶乩□卞覆仄化分仃  杀允月 */
    if( CHAR_getInt( talkerindex , CHAR_WHICHTYPE ) != CHAR_TYPEPLAYER )
    {
    	return;
    }

	/*--  及蟆卞中月井升丹井＂--*/
	if(NPC_Util_isFaceToFace( meindex ,talkerindex , 2) == FALSE) {
		/* ㄠ弘伉永玉动  及心 */
		if(NPC_Util_isFaceToChara( talkerindex, meindex, 1) == FALSE) return;
	}

	//  赓及它奶件玉它卞
	NPC_CheckMan_selectWindow( meindex, talkerindex, 0);

}


/*
 * 备质  卞坌仃月
 */
static void NPC_CheckMan_selectWindow( int meindex, int talker, int num)
{

	char token[1024];
	char work[256];
	char work2[512];

	char escapedname[1024];
	int fd = getfdFromCharaIndex( talker);
	int buttontype = 0;
	int windowtype = 0;
	int windowno = 0;
	int now[MAXEVENTFLG];
	int nowindex;
	int i;	
	int page;
	
	work[0] = 0;
	work2[0] = 0;
	token[0] = 0;

	now[0] =0;
	
	/*--它奶件玉它正奶皿丢永本□斥互云云中及匹燮卞涩烂--*/
  	windowtype = WINDOW_MESSAGETYPE_MESSAGE;

	switch( num) {
	
	  case 0:
		CHAR_setWorkInt(talker,CHAR_WORKSHOPRELEVANT,0);
		CHAR_setWorkInt(talker,CHAR_WORKSHOPRELEVANTSEC,0);
		
  		/*--蓟      --*/
		sprintf(token,"3\n ｛｛｛｛＞＞｛民尼永弁穴件｛＞＞ "
				  "\n｛｛｛蜇箕及白仿弘民尼永弁毛仄引□允"
				  "\n"
				  "\n｛｛｛｛ Ｕ｛NOW白仿弘民尼永弁｛Ｖ "
				  "\n｛｛｛｛ Ｕ｛END白仿弘民尼永弁｛Ｖ "
				  "\n\n｛｛ Ｕ｛NOW白仿弘民尼永弁  择称  Ｖ "
				  "\n｛｛ Ｕ｛END白仿弘民尼永弁  择称  Ｖ "
		);

	  	buttontype = WINDOW_BUTTONTYPE_CANCEL;
	  	windowtype = WINDOW_MESSAGETYPE_SELECT;
	  	windowno = CHAR_WINDOWTYPE_CHECKMAN_START; 
	  	break;

	//NOW白仿弘及棵签  憎
	  case 1:
	  	
	  	//NOW白仿弘及民尼永弁
		nowindex = NPC_NowFlgCheck( meindex, talker, now);

		//窒矢□斥  井
		page = CHAR_getWorkInt(talker,CHAR_WORKSHOPRELEVANTSEC) ;
		
		if(page == 0 || page == 1){
			i = 0;
		}else{
			i = 83;
		}
		
		//矢□斥
		for(; i < nowindex ; i++)
		{
			sprintf(work,"%d,",now[ i]);
			strcat(work2,work);
		}
		
		sprintf(token,"｛｛｛｛｛＞＞｛民尼永弁穴件｛＞＞ "
					"\n｛｛蜇箕及筑  及  匀化中月 NOW奶矛件玄"
					"\n%s"
			 	,work2);	
	  	
	  	if(page != 2 && nowindex > 83)
	  	{
	  		//乒□玉
	  		CHAR_setWorkInt(talker,CHAR_WORKSHOPRELEVANT,1);
	  		//矢□斥
	  		CHAR_setWorkInt(talker,CHAR_WORKSHOPRELEVANTSEC,2);

			buttontype = WINDOW_BUTTONTYPE_NEXT;
			windowtype = WINDOW_MESSAGETYPE_MESSAGE;
		  	windowno = CHAR_WINDOWTYPE_CHECKMAN_MAIN; 

		}else{
			buttontype = WINDOW_BUTTONTYPE_OK;
			windowtype = WINDOW_MESSAGETYPE_MESSAGE;
		}
		
		break;


	//END白仿弘及棵签  憎
	  case 2:
		//蔽  白仿弘及民尼永弁
		nowindex = NPC_EndFlgCheck( meindex, talker, now);
		
		//窒矢□斥  井
		page = CHAR_getWorkInt(talker,CHAR_WORKSHOPRELEVANTSEC) ;
		
		if(page == 0 || page == 1){
			i = 0;
		}else{
			i = 83;
		}
		work2[0]=0;
		
		//矢□斥
		for(; i < nowindex ; i++)
		{
			sprintf(work,"%d,",now[ i]);
			strcat(work2,work);
		}
		sprintf(token,"｛｛｛｛｛＞＞｛民尼永弁穴件｛＞＞ "
					"\n｛｛蜇箕及筑  及  匀化中月 END奶矛件玄"
					"\n%s"
			 	,work2);	
	  	
	  	if(page != 2 && nowindex > 83)
	  	{
	  		//乒□玉
	  		CHAR_setWorkInt(talker,CHAR_WORKSHOPRELEVANT,2);
	  		//矢□斥
	  		CHAR_setWorkInt(talker,CHAR_WORKSHOPRELEVANTSEC,2);

			buttontype = WINDOW_BUTTONTYPE_NEXT;
			windowtype = WINDOW_MESSAGETYPE_MESSAGE;
		  	windowno = CHAR_WINDOWTYPE_CHECKMAN_MAIN; 

		}else{
			buttontype = WINDOW_BUTTONTYPE_OK;
			windowtype = WINDOW_MESSAGETYPE_MESSAGE;
		}
		
		break;
		
	
	// NOW白仿弘及择称  憎
	  case 4:
		{
			
			//NOW白仿弘民尼永弁
			nowindex = NPC_NowFlgCheck( meindex, talker, now);

			if(NPC_FlgCheckMain( meindex, talker, nowindex,now,work2)
			 == FALSE)
			 {
			 	return;
			 }
			

			sprintf(token,"｛｛｛｛｛＞＞｛民尼永弁穴件｛＞＞ "
						"\n｛｛蜇箕及筑  及  匀化中月 NOW奶矛件玄"
						"\n%s"
				 	,work2);	

			page = CHAR_getWorkInt(talker,CHAR_WORKSHOPRELEVANTSEC) ;
		  	nowindex = (nowindex / ((6*page)+1));

		  	if(page != 16 &&  nowindex != 0)
		  	{
		  		//乒□玉
		  		CHAR_setWorkInt(talker,CHAR_WORKSHOPRELEVANT,4);
		  		//矢□斥
		  		page = CHAR_getWorkInt(talker,CHAR_WORKSHOPRELEVANTSEC);
		  		page++;
		  		CHAR_setWorkInt(talker,CHAR_WORKSHOPRELEVANTSEC,page);
			
				buttontype = WINDOW_BUTTONTYPE_NEXT;
				windowtype = WINDOW_MESSAGETYPE_MESSAGE;
			  	windowno = CHAR_WINDOWTYPE_CHECKMAN_MAIN; 
			}else{
				buttontype = WINDOW_BUTTONTYPE_OK;
				windowtype = WINDOW_MESSAGETYPE_MESSAGE;
			}

		}
	break;

	// END白仿弘及择称  憎
	  case 5:
		{
			//END白仿弘及民尼永弁
			nowindex = NPC_EndFlgCheck( meindex, talker, now);
			
			
			if(NPC_FlgCheckMain( meindex, talker, nowindex, now, work2)
			 == FALSE)
			 {
			 	return;
			 }

			sprintf(token,"｛｛｛｛｛＞＞｛民尼永弁穴件｛＞＞ "
						"\n｛｛蜇箕及筑  及  匀化中月 END奶矛件玄"
						"\n%s"
				 	,work2);	

			page = CHAR_getWorkInt(talker,CHAR_WORKSHOPRELEVANTSEC) ;

		  	nowindex = (nowindex / ((6*page)+1));

		  	if(page != 16 &&  nowindex != 0)
		  	{
		  		//乒□玉
		  		CHAR_setWorkInt(talker, CHAR_WORKSHOPRELEVANT, 5);
		  		//矢□斥
		  		page = CHAR_getWorkInt( talker, CHAR_WORKSHOPRELEVANTSEC);
		  		page++;
		  		CHAR_setWorkInt( talker, CHAR_WORKSHOPRELEVANTSEC, page);
			
				buttontype = WINDOW_BUTTONTYPE_NEXT;
				windowtype = WINDOW_MESSAGETYPE_MESSAGE;
			  	windowno = CHAR_WINDOWTYPE_CHECKMAN_MAIN; 
			}else{
				buttontype = WINDOW_BUTTONTYPE_OK;
				windowtype = WINDOW_MESSAGETYPE_MESSAGE;
			}

		}
	}
	
	makeEscapeString( token, escapedname, sizeof( escapedname));
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
void NPC_CheckManWindowTalked( int meindex, int talkerindex, 
								int seqno, int select, char *data)
{
	int datano;
	
	if( NPC_Util_CharDistance( talkerindex, meindex ) > 2) return;

	datano = atoi(data);
	
	if(select == WINDOW_BUTTONTYPE_OK) 
	{
		NPC_CheckMan_selectWindow( meindex, talkerindex, 0 );
	}else if(select == WINDOW_BUTTONTYPE_CANCEL) {
		return;
	}



	
	switch(CHAR_getWorkInt(talkerindex,CHAR_WORKSHOPRELEVANT)){
	  case 1:
	  	if(CHAR_getWorkInt(talkerindex,CHAR_WORKSHOPRELEVANTSEC) == 2){
			NPC_CheckMan_selectWindow( meindex, talkerindex, 1 );
	  	}
	  	break;

	  case 2:
	  	if(CHAR_getWorkInt(talkerindex,CHAR_WORKSHOPRELEVANTSEC) == 2){
			NPC_CheckMan_selectWindow( meindex, talkerindex, 2 );
	  	}
	  	break;

	  case 4:
	  	if(CHAR_getWorkInt(talkerindex,CHAR_WORKSHOPRELEVANTSEC) >= 2){
			NPC_CheckMan_selectWindow( meindex, talkerindex, 4 );
	  	}

	  case 5:
		if(CHAR_getWorkInt(talkerindex,CHAR_WORKSHOPRELEVANTSEC) >= 2){
			NPC_CheckMan_selectWindow( meindex, talkerindex, 5 );
		}

		break;
	}

	switch( datano ){
	  case 1:
			NPC_CheckMan_selectWindow( meindex, talkerindex, 1 );
		break;

	  case 2:
			NPC_CheckMan_selectWindow( meindex, talkerindex, 2 );
		break;

	  case 4:
			NPC_CheckMan_selectWindow( meindex, talkerindex, 4 );
		break;
		
	  case 5:
			NPC_CheckMan_selectWindow( meindex, talkerindex, 5 );
		break;
	}

}

/*
 *｛奶矛件玄  白仿弘毛民尼永弁允月
 */
int NPC_NowFlgCheck(int meindex,int talker ,int nowflg[MAXEVENTFLG])
{
	int i = 0;
	int j = 0;
	
	for(i= 0; i < MAXEVENTFLG ; i++){
		if(NPC_NowEventCheckFlg( talker, i) == TRUE)
		{
			nowflg[j] = i;
			j++;
		}
 	}
 	
	return j;
}

/*
 *｛奶矛件玄蔽  白仿弘毛民尼永弁允月
 */
int NPC_EndFlgCheck(int meindex,int talker ,int nowflg[MAXEVENTFLG])
{
	int i = 0;
	int j = 0;
	
	for(i= 0; i < MAXEVENTFLG ; i++){

		if(NPC_EventCheckFlg( talker, i) == TRUE)
		{
			nowflg[j] = i;
			j++;
		}
 	}
 	
	return j;
}

/*
 * 择称
 *
 */
BOOL NPC_FlgCheckMain( int meindex,int talker,int nowindex,int now[MAXEVENTFLG],char *work2)
{
	int page;
	int max;
	int i;
	int shou;
	int j=1;
	char argstr[NPC_UTIL_GETARGSTR_BUFSIZE];
	char work[512];
	char buf[40];
	char buf2[42];
	
	//窒矢□斥  井
	page = CHAR_getWorkInt(talker,CHAR_WORKSHOPRELEVANTSEC) ;

	if(page == 0) {
		page =1;
		CHAR_setWorkInt(talker,CHAR_WORKSHOPRELEVANTSEC,1) ;
	}

	if(page == 1){
		i = 0;
		if( nowindex >6 ){
			max =7;
		}else{
			max =nowindex;
		}
	}else{
		max = (page * 6) +1;
		i = max - 7;
		shou = nowindex / max;

		if(shou == 0){
			max = nowindex;
		}else{
			i = max - 7;
		}
	}
			
	//奶矛件玄及择称互踏井木化中月白央奶伙毛  心仇戈
	if(NPC_Util_GetArgStr( meindex, argstr, sizeof(argstr)) == NULL) {
		print("NPC_CheckMan:GetArgStrErr");
		return FALSE;
	}
			
	work[0] = 0;
	work2[0] = 0;

	//矢□斥
	for(; i < max ; i++)
	{
		sprintf(work,"#%d:",now[ i]);
		j = 1;
		while( getStringFromIndexWithDelim(argstr, "|", j, buf,sizeof( buf))
		 !=FALSE )
		 {	
			j++;
			if(strstr(buf,work) != NULL){
				sprintf(buf2,"%s\n",buf);
				strcat(work2,buf2);
				break;
			}
		}
	}
	
	return TRUE;
}
