#include "version.h"
#include <string.h>
#include "char.h"
#include "object.h"
#include "char_base.h"
#include "npcutil.h"
#include "lssproto_serv.h"
#include "pet_skill.h"
#include "readmap.h"
#include "battle.h"
#include "log.h"
/* ¶ªÓÀ±¾¡õ³â¼°enum */
enum {
	NPC_PETSHOP_MSG_OVER,
	NPC_PETSHOP_MSG_MAIN,
	NPC_PETSHOP_MSG_REALY,
	NPC_PETSHOP_MSG_THANKS,
	NPC_PETSHOP_MSG_COST,
	NPC_PETSHOP_MSG_POOLTHANKS,
	NPC_PETSHOP_MSG_POOLFULL,
	NPC_PETSHOP_MSG_GETFULL,
};
typedef struct {
	char	option[32];
	char	defaultmsg[128];
}NPC_PETSHOP_MSG;

NPC_PETSHOP_MSG		shopmsg[] = {
	{ "over_msg",		"Ç®ÓÃÍêÂÞ!Ã»ÎÊÌâÂð?"},
	{ "main_msg",		"»¶Ó­"},
	{ "realy_msg",		"ÕæµÄ¿ÉÒÔÂð?"},
	{ "thanks_msg",		"Ð»Ð»!"},
	{ "cost_msg",		"Ç®²»¹»à¸!"},
	{ "pooltanks_msg",	"Ð»Ð»!"},
	{ "poolfull_msg",	"³¬¹ýµÄÎÞ·¨´¦Àíà¸"},
	{ "getfull_msg",	"Äã²»ÊÇÓÐºÜ¶à³èÎïÂð"}
	
};

#define PETCOST	10

void NPC_PetDate(int meindex,int toindex,int select,char *token);
void NPC_PetDate2(int meindex,int talker,int select,char *token);
void NPC_PetDate3(int meindex,int talker,int select,char *token);
void NPC_PetDel2( int meindex, int talker, int select, char *token);
void NPC_PetDel3( int meindex, int talker, int select, char *token);
void NPC_getDrawPet( int meindex, int toindex, char *token, int *buttontype, int select);

static void NPC_PetShop_selectWindow( int meindex, int toindex, int num,int select);
int NPC_GetCost(int meindex,int talker,int petindex);
void NPC_PetDel(int meindex, int toindex,int select,char* token);
void NPC_MaxGoldOver(int meindex,int toindex,int select,char *token);

void NPC_DepotPet_CheckRepeat_del( int charaindex, int petindex);

//   Øê  Ä¾àÅº°»ÍÒ£Ö¿
#define		NPC_GETPOOLCOST( talker)		( 50 + CHAR_getInt( talker, CHAR_LV)*4)

//   Øê  Ä¾àÅ·¸°×ÇÉ»ïÐþ
#define		NPC_PETSHOP_POOLCOST_DEFAULT	200

/*--·¥¡õÛÍ    ¼°¸îâç--*/
enum {
	CHAR_WORK_NOMALRATE	= CHAR_NPCWORKINT1,
	CHAR_WORK_SPECIALRATE	= CHAR_NPCWORKINT2,
	CHAR_WORK_POOLCOST	= CHAR_NPCWORKINT3,
	CHAR_WORK_POOLFLG	= CHAR_NPCWORKINT4,
};


/*********************************
* âÙÓåÖÊ  
*********************************/
BOOL NPC_PetShopInit( int meindex )
{

	char npcarg[NPC_UTIL_GETARGSTR_BUFSIZE - 1024 * 20];
	char buf[256];
	double rate;
	int intrate;
//	int cost;
	int tmp;

	/*--ÕýÄÌÃóÉ¬ÀÃ--*/
   	CHAR_setInt( meindex , CHAR_WHICHTYPE , CHAR_TYPESTONESHOP );


	if( NPC_Util_GetArgStr( meindex, npcarg, sizeof( npcarg)) == NULL){
		print("GetArgStrErr");
		return FALSE;
	}

	/*--ÒÁ¡õÐþÃ«ï§»§±å·¤Ê¢½ñÁù»¯ÔÆÈÊ*/
	if( NPC_Util_GetStrFromStrWithDelim( npcarg, "nomal_rate", buf, sizeof( buf)) != NULL){
		rate = atof( buf);		
		intrate = ( int)( rate *1000);		
   		CHAR_setWorkInt( meindex , CHAR_WORK_NOMALRATE ,intrate );
	}else{
		CHAR_setWorkInt( meindex , CHAR_WORK_NOMALRATE ,1000 );
	}

		
	/*--µ©Ê¸ÆËÅÒ»ïÒÁ¡õÐþÊÖ·¤Ê¢½ñÁù»¯ÔÆÈÊ--*/
	if(NPC_Util_GetStrFromStrWithDelim( npcarg, "special_rate", buf, sizeof( buf))!=NULL)
	{
		rate = atof( buf);		
		intrate = ( int)( rate *1000);		
   		CHAR_setWorkInt( meindex , CHAR_WORK_SPECIALRATE ,intrate );
	}else{
		CHAR_setWorkInt( meindex , CHAR_WORK_SPECIALRATE ,1200 );
	}
	
	//   Øê  Ä¾àÅ·¤Ê¢
	//cost = NPC_Util_GetNumFromStrWithDelim( npcarg, "pool_cost");
	//if( cost == -1 ) cost = NPC_PETSHOP_POOLCOST_DEFAULT;
	//CHAR_setWorkInt( meindex, CHAR_WORK_POOLCOST, cost);
	
	
	//   Øê  Ä¾»¥Çë  ÔÂ¾®Éýµ¤¾®Ã«·¤Ê¢
	tmp = NPC_Util_GetNumFromStrWithDelim( npcarg, "pool_flg");
	if( tmp != 1 ) tmp = 0;
	CHAR_setWorkInt( meindex, CHAR_WORK_POOLFLG, tmp);
    
    return TRUE;

}




/*********************************
*   ØÆ¾®ØêÈÕÄ¾Ð×ÁÝ¼°ÖÊ  
*********************************/
void NPC_PetShopTalked( int meindex , int talkerindex , char *szMes ,int color )
{
    /* ÃóÒÁÄÌØÀ¡õ±å¸²ØÆ»¯·ÖØê  É±ÔÊÔÂ */
    if( CHAR_getInt( talkerindex , CHAR_WHICHTYPE ) != CHAR_TYPEPLAYER ) {
    	return;
    }
	
	/*--  ¼°ó¡±åÖÐÔÂ¾®Éýµ¤¾®£¢--*/
	if(NPC_Util_isFaceToFace(talkerindex,meindex,2 )==FALSE){
		/* ¨àºëØøÓÀÓñ¶¯  ¼°ÐÄ */
		if( NPC_Util_CharDistance( talkerindex, meindex ) > 1) return;
	}
	/*--·¥¡õÛÍ¼°âÙÓå¼À--*/
	CHAR_setWorkInt(talkerindex,CHAR_WORKSHOPRELEVANT,0);
	/*-·´Ôª»§¼°¼»      --*/
	NPC_PetShop_selectWindow( meindex, talkerindex,0,-1);
}

/*********************************
* Î­ÈÕÄ¾Ð×ÁÝ¼°ÖÊ  
*********************************/
void NPC_PetShopLooked( int meindex , int lookedindex)
{
    /* ÃóÒÁÄÌØÀ¡õ±å¸²ØÆ»¯·ÖØê  É±ÔÊÔÂ */
    if( CHAR_getInt( lookedindex , CHAR_WHICHTYPE ) != CHAR_TYPEPLAYER ) {
    	return;
    }

	/* ¨àºëØøÓÀÓñ¶¯  ¼°ÐÄ */
	if( NPC_Util_CharDistance( lookedindex, meindex ) > 2) return;

	/*--·¥¡õÛÍ¼°âÙÓå¼À--*/
//	CHAR_setWorkInt(lookedindex,CHAR_WORKSHOPRELEVANT,0);


//	NPC_PetShop_selectWindow( meindex, lookedindex,0,-1);

}

static void NPC_PetShop_selectWindow( int meindex, int toindex, int num,int select)
{
	char token[1024];
	int buttontype=0,windowtype=0,windowno=0;
	char buf[256];
	int fd = getfdFromCharaIndex( toindex);
	
	//print("\n NPC_PetShop_selectWindow num:%d select:%d  ", num, select);
	if( fd == -1 ) {
		fprint( "getfd err\n");
		return;
	}
	token[0] = '\0';
	switch(num){
      //   âÙ¼°ËüÅ«¼þÓñËü
	  case 0:
		NPC_MaxGoldOver( meindex, toindex, NPC_PETSHOP_MSG_MAIN, buf);
		
		if( !CHAR_getWorkInt( meindex, CHAR_WORK_POOLFLG)) {
			sprintf(token, "4\n¡¡¡¡¡¡¡¡¡¡¡¡¡î³èÎïÉÌµê¡î"
							"%s"
							"\n"
							"\n¡¡¡¡¡¡¡¡¡¡¡¡ ¡¶ Âô³èÎï ¡·"
							"\n"
							"\n             ¡¶ Àë¿ª ¡·",
							buf);
		}else {
			sprintf(token, "4\n¡¡¡¡¡¡¡¡¡¡¡¡¡î³èÎïÉÌµê¡î"
							"%s"
							"\n            ¡¶ ¼Ä·Å³èÎï ¡·"
							"\n            ¡¶ ÁìÈ¡³èÎï ¡·"
							"\n¡¡¡¡¡¡¡¡¡¡¡¡ ¡¶ Âô³èÎï ¡·"
							"\n              ¡¶ Àë¿ª ¡·",
							buf);
		}
		buttontype=WINDOW_BUTTONTYPE_NONE;
		windowtype=WINDOW_MESSAGETYPE_SELECT;
		windowno=CHAR_WINDOWTYPE_WINDOWPETSHOP_START; 
		break;
	  case 1:
	  buttontype=WINDOW_BUTTONTYPE_CANCEL;
	  windowtype=WINDOW_MESSAGETYPE_PETSELECT;
		windowno=CHAR_WINDOWTYPE_WINDOWPETSHOP_PETSELECT; 
		break;
	  case 2:
		NPC_PetDate(meindex,toindex,select,token);
		buttontype=WINDOW_BUTTONTYPE_YESNO;
	  	windowtype=WINDOW_MESSAGETYPE_MESSAGE;
		windowno=CHAR_WINDOWTYPE_WINDOWPETSHOP_MAIN; 
		break;
	  case 3:
		if( (CHAR_getInt(toindex,CHAR_GOLD)+CHAR_getWorkInt(toindex,CHAR_WORKSHOPRELEVANTSEC))
			> CHAR_getMaxHaveGold( toindex) ){

			NPC_MaxGoldOver( meindex, toindex, NPC_PETSHOP_MSG_OVER, token);
			windowno=CHAR_WINDOWTYPE_WINDOWPETSHOP_GOLDOVER; 
			buttontype=WINDOW_BUTTONTYPE_YESNO;
		}else{
			NPC_PetDel(meindex,toindex,select,token);
			buttontype=WINDOW_BUTTONTYPE_OK;
			windowno=CHAR_WINDOWTYPE_WINDOWPETSHOP_END; 
		}

	  	windowtype=WINDOW_MESSAGETYPE_MESSAGE;
		break;
	  case 4:
	 	NPC_PetDel(meindex,toindex,select,token);
		buttontype=WINDOW_BUTTONTYPE_OK;
		windowno=CHAR_WINDOWTYPE_WINDOWPETSHOP_END; 
		windowtype=WINDOW_MESSAGETYPE_MESSAGE;
		break;
	  case 5: // ÁìÈ¡¼Ä·Å³èÁÐ±í
	  	if( CHAR_getWorkInt( meindex, CHAR_WORK_POOLFLG ) != 1 )
	  		return;
	  	buttontype=WINDOW_BUTTONTYPE_CANCEL;
	  	windowtype=WINDOW_MESSAGETYPE_PETSELECT;
		windowno=CHAR_WINDOWTYPE_WINDOWPETSHOP_PETSELECT2; 
		break;
	  case 6:
		if( CHAR_getInt( toindex, CHAR_GOLD) 
			< NPC_GETPOOLCOST( toindex)) 
		{
		  	NPC_MaxGoldOver(meindex, toindex, NPC_PETSHOP_MSG_COST, token);
			buttontype = WINDOW_BUTTONTYPE_OK;
		}
		else {
			NPC_PetDate2( meindex, toindex, select, token);
			buttontype = WINDOW_BUTTONTYPE_YESNO;
		}
	  	windowtype = WINDOW_MESSAGETYPE_MESSAGE;
		windowno = CHAR_WINDOWTYPE_WINDOWPETSHOP_MAIN2; 
	    break;
	  
	  case 7:
	  	if( CHAR_getWorkInt( meindex, CHAR_WORK_POOLFLG ) != 1 )
	  		return;	    
		if( CHAR_getInt( toindex, CHAR_GOLD) < NPC_GETPOOLCOST( toindex)){
		  	NPC_MaxGoldOver(meindex, toindex, NPC_PETSHOP_MSG_COST, token);
		}else {
			NPC_PetDel2( meindex, toindex, select, token);
		}
		buttontype = WINDOW_BUTTONTYPE_OK;
	  	windowtype = WINDOW_MESSAGETYPE_MESSAGE;
		windowno = CHAR_WINDOWTYPE_WINDOWPETSHOP_END; 
	    break;
	  //   Øê  Ä¾  »¥Óò  
	  case 8:
	  	NPC_MaxGoldOver( meindex, toindex, NPC_PETSHOP_MSG_POOLFULL, token);
		buttontype = WINDOW_BUTTONTYPE_OK;
	  	windowtype = WINDOW_MESSAGETYPE_MESSAGE;
		windowno = CHAR_WINDOWTYPE_WINDOWPETSHOP_END; 
	    break;
	  // Â¦ÎåäúÔ»Ê¸ÓÀÐþÓò  ËüÅ«¼þÓñËü
	  case 9: // È¡³ö¼Ä·Å³èÁÐ±í
	  	if( CHAR_getWorkInt( meindex, CHAR_WORK_POOLFLG) != 1 )
	  		return;
	  	NPC_getDrawPet( meindex, toindex, token, &buttontype, select);
	  	windowtype = WINDOW_MESSAGETYPE_SELECT;
		windowno = CHAR_WINDOWTYPE_WINDOWPETSHOP_DRAWSELECT; 
		break;
	  // Â¦ÎåäúÔ»    £¢
	  case 10:
		NPC_PetDate3( meindex, toindex, select, token);
		buttontype = WINDOW_BUTTONTYPE_YESNO;
	  	windowtype = WINDOW_MESSAGETYPE_MESSAGE;
		windowno = CHAR_WINDOWTYPE_WINDOWPETSHOP_ASKDRAW;
	    break;
	  // ß¯ÔÊ
	  case 11:
	  	if( CHAR_getWorkInt( meindex, CHAR_WORK_POOLFLG ) != 1 )
	  		return;	  
		NPC_PetDel3( meindex, toindex, select, token);
		buttontype = WINDOW_BUTTONTYPE_OK;
	  	windowtype = WINDOW_MESSAGETYPE_MESSAGE;
		windowno = CHAR_WINDOWTYPE_WINDOWPETSHOP_END; 
	    break;
	  // Ê¸ÓÀÐþ  »¥Óò  
	  case 12:
	  	NPC_MaxGoldOver( meindex, toindex, NPC_PETSHOP_MSG_GETFULL, token);
		buttontype = WINDOW_BUTTONTYPE_OK;
	  	windowtype = WINDOW_MESSAGETYPE_MESSAGE;
		windowno = CHAR_WINDOWTYPE_WINDOWPETSHOP_END; 
	    break;
	  // CoolFish: Add 2001/7/4
	  default:
	    break;
	
	}
	
	/*--¾Þµ©¸¥¡õÃó--*/
	//makeEscapeString( token, escapedname, sizeof(escapedname));
		
	/*--Ëªññ--*/
	lssproto_WN_send( fd, windowtype, 
				buttontype, 
				windowno,
				CHAR_getWorkInt( meindex, CHAR_WORKOBJINDEX),
				token);
}

void NPC_PetShopWindowTalked( int meindex, int talkerindex, 
								int seqno, int select, char *data)
{
	int datanum = -1;
	if( NPC_Util_CharDistance( talkerindex, meindex ) > 2) {
		return;
	}
	//print("\n NPC_PetShopWindowTalked: data:%s seq:%d sel:%d ", data, seqno, select);

	makeStringFromEscaped( data);
	
	datanum = atoi( data);
	switch( seqno){
	case CHAR_WINDOWTYPE_WINDOWPETSHOP_START:
		{
			int poolflg = CHAR_getWorkInt( meindex, CHAR_WORK_POOLFLG);
			if( ( datanum == 3 && poolflg) || // Âô³èÎï
				( datanum == 2 && !poolflg ) ){	// ÁìÈ¡³èÎï
				NPC_PetShop_selectWindow( meindex, talkerindex, 1, -1 );
			}else if( datanum == 1 && poolflg) { // ¼Ä·Å³èÎï 
				if( CHAR_getCharPoolPetElement( talkerindex) != -1 ) {
					NPC_PetShop_selectWindow( meindex, talkerindex, 5, -1 );
				}
				else {
					NPC_PetShop_selectWindow( meindex, talkerindex, 8, -1 );
				}
			}else if( datanum == 2 && poolflg ) { // ÁìÈ¡³èÎï
				if( CHAR_getCharPetElement( talkerindex) != -1 ) {
					CHAR_setWorkInt( talkerindex, CHAR_WORKSHOPRELEVANT, 0);
					NPC_PetShop_selectWindow( meindex, talkerindex, 9, 0);
				}else {
					NPC_PetShop_selectWindow( meindex, talkerindex, 12, -1 );
				}
			}
		}
		break;
	  case CHAR_WINDOWTYPE_WINDOWPETSHOP_PETSELECT:
		if(select==WINDOW_BUTTONTYPE_CANCEL){
			NPC_PetShop_selectWindow( meindex, talkerindex, 0,-1 );
			
		}else{
			NPC_PetShop_selectWindow( meindex, talkerindex, 2, datanum );
		}
		break;
	  case CHAR_WINDOWTYPE_WINDOWPETSHOP_PETSELECT2:
		if(select==WINDOW_BUTTONTYPE_CANCEL){
			NPC_PetShop_selectWindow( meindex, talkerindex, 0,-1 );
		}else{
			NPC_PetShop_selectWindow( meindex, talkerindex, 6, datanum );
		}
		break;
	  case CHAR_WINDOWTYPE_WINDOWPETSHOP_MAIN:
		if(select==WINDOW_BUTTONTYPE_YES){
			NPC_PetShop_selectWindow( meindex, talkerindex, 3,-1 );
		}else{
			NPC_PetShop_selectWindow( meindex, talkerindex,1,-1);
	  	}
	  	break;
	  case CHAR_WINDOWTYPE_WINDOWPETSHOP_MAIN2:
		if(select==WINDOW_BUTTONTYPE_YES){
			NPC_PetShop_selectWindow( meindex, talkerindex, 7,-1 );
		}else {
			NPC_PetShop_selectWindow( meindex, talkerindex,5,-1);
	  	}
	  	break;
	  case CHAR_WINDOWTYPE_WINDOWPETSHOP_ASKDRAW:
		if( select == WINDOW_BUTTONTYPE_YES){
			if( CHAR_getCharPetElement( talkerindex) != -1 ) {
				NPC_PetShop_selectWindow( meindex, talkerindex, 11,-1 );
			}else {
				NPC_PetShop_selectWindow( meindex, talkerindex, 12, -1 );
			}
		}else {
			CHAR_setWorkInt( talkerindex, CHAR_WORKSHOPRELEVANT, 0);
			NPC_PetShop_selectWindow( meindex, talkerindex, 9, 0);
	  	}
	  	break;
	  case CHAR_WINDOWTYPE_WINDOWPETSHOP_GOLDOVER:
		if( select == WINDOW_BUTTONTYPE_YES){
			NPC_PetShop_selectWindow( meindex, talkerindex, 4,-1 );
		}else{
			NPC_PetShop_selectWindow( meindex, talkerindex, 0,-1 );
		}
	  	break;
	  case CHAR_WINDOWTYPE_WINDOWPETSHOP_DRAWSELECT:
		if( select == WINDOW_BUTTONTYPE_PREV){
			NPC_PetShop_selectWindow( meindex, talkerindex, 9, -1);
		}else if( select == WINDOW_BUTTONTYPE_NEXT) {
			NPC_PetShop_selectWindow( meindex, talkerindex, 9, 1);
		}else if( select != WINDOW_BUTTONTYPE_CANCEL) {
			if( CHAR_getCharPetElement( talkerindex) != -1 ) {
				NPC_PetShop_selectWindow( meindex, talkerindex, 10, datanum);
			}else {
				NPC_PetShop_selectWindow( meindex, talkerindex, 12, -1 );
			}
		}else {
			NPC_PetShop_selectWindow( meindex, talkerindex, 0,-1 );
		}
		break;

	  case CHAR_WINDOWTYPE_WINDOWPETSHOP_END:
		if( select == WINDOW_BUTTONTYPE_OK){
			NPC_PetShop_selectWindow( meindex, talkerindex, 0,-1 );
		}
	  	break;

	  // CoolFish: Add 2001/7/4
	  default:
	  	break;

	}
}

void NPC_PetDate(int meindex,int talker,int select,char *token)
{
	
	int petindex;
	int cost;
	char *petname;
	char buf[1024];

	/*--·¥¡õÛÍ¼°âÙÓå¼À--*/
	CHAR_setWorkInt(talker,CHAR_WORKSHOPRELEVANT,select);

	petindex = CHAR_getCharPet( talker, select-1);
        

   if( !CHAR_CHECKINDEX(petindex) ) return;

  	NPC_MaxGoldOver( meindex, talker, NPC_PETSHOP_MSG_REALY, buf);

	cost=NPC_GetCost(meindex,talker,petindex);

	petname = CHAR_getUseName( petindex);

	sprintf(token,"\nÊÇ[%s]Ã»´í°É£¿""%s"
					"\n\n    ÂòÂô¼Û¸ñ         %d STONE\n"
					,petname,buf,cost);	
		
}

void NPC_PetDate2(int meindex,int talker,int select,char *token)
{
	
	int petindex;
	char *petname;
	CHAR_setWorkInt(talker,CHAR_WORKSHOPRELEVANT,select);

	petindex = CHAR_getCharPet( talker, select-1);
	if( !CHAR_CHECKINDEX(petindex) ) return;
	petname = CHAR_getUseName( petindex);
	sprintf(token,"\n¼Ä·Å[%s]"
					"\n\n    ¼Ä·Å¼Û¸ñ            %d stone\n"
					,petname, NPC_GETPOOLCOST( talker)
					);	
}

void NPC_PetDate3(int meindex,int talker,int select,char *token)
{
	int poolindex;
	int petindex;
	char *petname;
	poolindex = CHAR_getWorkInt( talker, CHAR_WORKSHOPRELEVANT) * 5 + select - 1;
	CHAR_setWorkInt(talker,CHAR_WORKSHOPRELEVANT,poolindex );
	petindex = CHAR_getCharPoolPet( talker, poolindex);
	if( !CHAR_CHECKINDEX(petindex) ) return;
	petname = CHAR_getUseName( petindex);
	sprintf(token,"\nÁì»Ø[%s]", petname);	
}

int NPC_GetCost(int meindex,int talker,int petindex)
{
	int cost;
	int levelcost;
	int level;
	int getlevel;
	int graNo;
	int rare;
	int charm=50;
	int petai;
	int i=1;
	double rate=1.0;
	int intrate;
	char buf[1024];
	char buff2[128];
	char npcarg[NPC_UTIL_GETARGSTR_BUFSIZE - 1024 * 20];
		
	if(NPC_Util_GetArgStr( meindex, npcarg, sizeof(npcarg))==NULL){
		print("GetArgStrErr");
		return FALSE;
	}

		
	getlevel=CHAR_getInt(petindex,CHAR_PETGETLV);
	level=CHAR_getInt(petindex,CHAR_LV);
	graNo=CHAR_getInt(petindex,CHAR_BASEBASEIMAGENUMBER);
	rare=CHAR_getInt(petindex,CHAR_RARE);

	if(getlevel==0)	getlevel=1	;
	
	/*--°Àó¸Ã«·Æ»§ÔÂ--*/
	levelcost=(level*level)*10;
	getlevel=(getlevel*getlevel)*10;
// Syu ADD ÐÞ¸ÄÂô×ªÉú³è½ðÇ®»á±ä¸ºµÄbug
#ifdef _PET_2TRANS
	if( CHAR_getInt(petindex , CHAR_TRANSMIGRATION)>0)
#else
	if( CHAR_getInt(petindex , CHAR_TRANSMIGRATION)==1)
#endif
		getlevel=10;


	if(rare==0){ rare=1;
	}else if(rare==1){ rare=5;
	}else if(rare==2){ rare=8;}

	cost = ((levelcost-getlevel)+(level*PETCOST))*rare;
	intrate = CHAR_getWorkInt( meindex , CHAR_WORK_NOMALRATE);
	rate=(double) intrate / 1000;
	

	NPC_Util_GetStrFromStrWithDelim( npcarg, "special_pet", buf, sizeof( buf));

	i=1;
	/*--µ©Ê¸ÆËÅÒ»ïÒÁ¡õÐþÆ¥»ÍÒ£½ñÄ¾ÔÂÊ¸ÓÀÐþÃ«à«Ô»ÇëÔÊ  Ê¸ÓÀÐþ¼°Ï¶ÀÃ·´  ì«  Ä¯)--*/
	while( getStringFromIndexWithDelim(buf,",",i,buff2,sizeof(buff2)) !=FALSE )
	{
		if(strstr( buff2, "-")==NULL){
			if(graNo == atoi(buff2)){
				intrate=CHAR_getWorkInt( meindex , CHAR_WORK_SPECIALRATE);
				rate=(double) intrate / 1000;
				break; 
			}
		}else{
			/*--Ê§ÄÌ  Ø©»¥  15-25  ¼°äßÆ¥à¼å©ÈÕÄ¾»¯ÖÐÔÂèëÄþ--*/
			int start;
			int end;
			int tmp;
			int graflg=0;
			char	token2[128];
	
			/*-"-"Æ¥à¼å©ÈÕÄ¾Ð×ï§»§¼°ÐÑ°ÀÎç  ¼°ÐÑ°ÀÃ«äú  --*/
			getStringFromIndexWithDelim( buff2, "-", 1, token2, sizeof(token2));
			start = atoi( token2);
			getStringFromIndexWithDelim( buff2, "-", 2 ,token2, sizeof(token2));
			end = atoi( token2);

			/*--  Ä¯»¥ÝÑ±åØ¦ÔÈ»¯ÖÐÐ×ÈÕ£ý  Ä¾×¸ÒüÔÂ**/
			if(start > end){
				tmp=start;
				start=end;
				end=tmp;
			}

			end++;
			/*--"-"Æ¥à¼å©ÈÕÄ¾Ð×ÛÐ¼°Ê§ÄÌ  Ø©Ê÷  Ã«  ÔÂ--*/
			if(start <= graNo && graNo < end){
				intrate=CHAR_getWorkInt( meindex , CHAR_WORK_SPECIALRATE);
				rate=(double) intrate / 1000;
				graflg=1;
				break; 
			}
			
			if(graflg ==1) break;
		}
		i++;
	}

	/*--Ê¸ÓÀÐþ¼°ïá²ÁÕºÃ«      ---*/
	petai=CHAR_getWorkInt(petindex,CHAR_WORKFIXAI);

	/*--    Ã«±ØÓÀÐþ--*/
	charm = CHAR_getWorkInt(talker,CHAR_WORKFIXCHARM);

	/*--      ¡Öïá²ÁÕº  / ¨á  */
	charm = charm + petai;

	if(charm < 20){
		charm = 20;
	}

	charm = charm / 2;

	cost = (int)cost*rate;

	rate=(double)charm/100;

	/*--    ïá²Á´­Øê--*/
	cost = cost * rate;

	/*--ÔÆàÅÃ«·¤Ê¢--*/
   	CHAR_setWorkInt(talker,CHAR_WORKSHOPRELEVANTSEC,cost);

	return cost;
}


#if 1

void NPC_PetDel(int meindex, int talker,int select,char* token)
{
	int petindex;
	int petsel;
	char szPet[128];
	int cost;
	int defpet;
	int fd = getfdFromCharaIndex( talker );

	NPC_MaxGoldOver( meindex, talker, NPC_PETSHOP_MSG_THANKS, token);
	petsel=(CHAR_getWorkInt(talker,CHAR_WORKSHOPRELEVANT)-1);
	petindex = CHAR_getCharPet( talker, petsel);
	
    if( !CHAR_CHECKINDEX(petindex) ) return;

        // CoolFish: Family 2001/6/14
	if (CHAR_getInt(petindex, CHAR_PETFAMILY) == 1){	
/*		char buf[1024];
		lssproto_WN_send(fd, WINDOW_MESSAGETYPE_MESSAGE,
			WINDOW_BUTTONTYPE_OK,
			-1, -1,
		makeEscapeString("\n\nºÜ±§Ç¸¡«ÊØ»¤ÊÞÎÞ·¨··Âôà¸£¡\nÇë×Ô¼ººÃºÃÕÕ¹Ë£¡", buf, sizeof(buf)));
*/
		sprintf(token,"\n\nºÜ±§Ç¸¡«ÊØ»¤ÊÞÎÞ·¨··Âôà¸£¡\nÇë×Ô¼ººÃºÃÕÕ¹Ë£¡");
		return;
	}

        // Robin 0725
        if (CHAR_getInt( talker, CHAR_RIDEPET ) == petsel ){	
/*        	char buf[1024];
        	lssproto_WN_send(fd, WINDOW_MESSAGETYPE_MESSAGE,
        		WINDOW_BUTTONTYPE_OK,
        		-1, -1,
        		makeEscapeString("\n\nÆï³ËÖÐµÄ³èÎïÎÞ·¨··Âôà¸£¡", buf, sizeof(buf)));
*/
					sprintf(token,"\n\nÆï³ËÖÐµÄ³èÎïÎÞ·¨··Âôà¸£¡");
        	return;
        }

	if( CHAR_getWorkInt( CONNECT_getCharaindex(fd),
                         CHAR_WORKBATTLEMODE) != BATTLE_CHARMODE_NONE) return ;
	defpet=CHAR_getInt(talker,CHAR_DEFAULTPET);
	if(defpet==petsel){
		CHAR_setInt(talker,CHAR_DEFAULTPET,-1);
		lssproto_KS_send( fd, -1, TRUE);
	}
   	CHAR_setCharPet( talker, petsel, -1);
	LogPet(
		CHAR_getChar( talker, CHAR_NAME ), /* Æ½ÅÒ·Â   */
		CHAR_getChar( talker, CHAR_CDKEY ),
		CHAR_getChar( petindex, CHAR_NAME),
		CHAR_getInt( petindex, CHAR_LV),
		"sell(Âô³è)",
		CHAR_getInt( talker,CHAR_FLOOR),
		CHAR_getInt( talker,CHAR_X ),
		CHAR_getInt( talker,CHAR_Y ),
		CHAR_getChar( petindex, CHAR_UNIQUECODE)   // shan 2001/12/14
	);
	CHAR_endCharOneArray( petindex );
		
	snprintf( szPet, sizeof( szPet ), "K%d", petsel);
	CHAR_sendStatusString( talker, szPet );
	cost=NPC_GetCost(meindex,talker,petindex);
	CHAR_AddGold( talker, cost);

}
#endif
void NPC_PetDel2( int meindex, int talker, int select, char *token)
{
	int petindex;
	int selectpet;
	int emptyindex;
	char szPet[128];
	int fd = getfdFromCharaIndex( talker );

	if( fd == -1 ) {
		fprint( "err\n");
		return;
	}
	// Ê¸ÓÀÐþ·¸¡õÕý  µÊ£ÛÊ¸ÓÀÐþ      Øê  Ä¾  
	selectpet = CHAR_getWorkInt( talker, CHAR_WORKSHOPRELEVANT) -1;
	petindex = CHAR_getCharPet( talker, selectpet);

	if( !CHAR_CHECKINDEX( petindex) ) return;
        // Robin 0725
        if (CHAR_getInt( talker, CHAR_RIDEPET ) == selectpet )
        {
/*        	char buf[1024];
        	lssproto_WN_send(fd, WINDOW_MESSAGETYPE_MESSAGE,
        		WINDOW_BUTTONTYPE_OK,
        		-1, -1,
        		makeEscapeString("\n\nÆï³ËÖÐµÄ³èÎïÎÞ·¨¼Ä·Åà¸£¡", buf, sizeof(buf)));
*/
					sprintf(token,"\n\nÆï³ËÖÐµÄ³èÎïÎÞ·¨¼Ä·Åà¸£¡");
        	return;
        }
        
	// ÎëÎåÃ«¼ëÔÊ£Ûó¡±å¼ëØÆ»¯ÃñÄáÓÀÛÍØÆ»¯ÖÐÔÂ¼°Æ¥£Ý³ð³ðÆ¥ÎëÖÐ»¯ÖÐØ¦ÖÐ¼°·´ÔÆ¾®ØÆÖÐ
	emptyindex = CHAR_getCharPoolPetElement( talker);
	if( emptyindex == -1 ) {
		fprint( "err\n");
		return;
	}

	// ¾ô  ÉÍ  åÃÊ¸ÓÀÐþØ¦ÈÕ£Ý¹«Ä¾¼°è¶  Ã«ÛÕÔÊ£Û
	if( CHAR_getInt( talker, CHAR_DEFAULTPET) == selectpet) {
		CHAR_setInt( talker, CHAR_DEFAULTPET, -1);
		lssproto_KS_send( fd, -1, TRUE);
	}
	//   Øê  Ä¾  ±å£ÝÊ¸ÓÀÐþ¼°ÄÌ¼þ·¸ÓÀÛÍµ©Ã«Îìñâ¡õ
	CHAR_setCharPoolPet( talker, emptyindex, petindex);
	// Ê¸ÓÀÐþ  ¾®ÈÕÛÕÔÊ
	CHAR_setCharPet( talker, selectpet, -1);

	// ¶ªÓÀ±¾¡õ³âäú  
	NPC_MaxGoldOver( meindex, talker, NPC_PETSHOP_MSG_POOLTHANKS, token);
	// ÔÆàÅÓ¼ÈÕÔÊ

	CHAR_DelGold( talker, NPC_GETPOOLCOST( talker) );

	// ÔÆàÅ·¸¡õÕýËªÔÂ
	CHAR_send_P_StatusString(talker,CHAR_P_STRING_GOLD);

	snprintf( szPet, sizeof( szPet ), "K%d", selectpet);
	// Þ¥ØÆÖÐÊ¸ÓÀÐþµ©  ¡õÕýµ©ËªÔ»¹´ØêÔÂ
	CHAR_sendStatusString( talker, szPet );
	// Ê¸ÓÀÐþÃ«  ØêÐ×·òºë
	LogPet(
		CHAR_getChar( talker, CHAR_NAME ), /* Æ½ÅÒ·Â   */
		CHAR_getChar( talker, CHAR_CDKEY ),
		CHAR_getChar( petindex, CHAR_NAME),
		CHAR_getInt( petindex, CHAR_LV),
		"pool(¼Ä·Å³èÎïµê)",
		CHAR_getInt( talker,CHAR_FLOOR),
		CHAR_getInt( talker,CHAR_X ),
		CHAR_getInt( talker,CHAR_Y ),
        CHAR_getChar( petindex, CHAR_UNIQUECODE)   // shan 2001/12/14
	);

}	

void NPC_PetDel3( int meindex, int talker, int select, char *token)
{
	int petindex;
	int selectpet;
	int emptyindex;
	char szPet[128];
	int workindex[CHAR_MAXPOOLPETHAVE];
	int i;
	int cnt;

	selectpet = CHAR_getWorkInt( talker, CHAR_WORKSHOPRELEVANT) ;
	petindex = CHAR_getCharPoolPet( talker, selectpet);
	
	if( !CHAR_CHECKINDEX( petindex) ) return;
	emptyindex = CHAR_getCharPetElement( talker);
	if( emptyindex == -1 ) {
		fprint( "err\n");
		return;
	}

	// Èç¹ûÊÇÊØ»¤ÊÞ¸ÄÎªÆÕÍ¨ÊÞ
	if( CHAR_getInt( petindex, CHAR_PETFAMILY) == 1 
		&& CHAR_getInt( talker, CHAR_FMLEADERFLAG) != 3 ) {
		CHAR_setInt( petindex, CHAR_PETFAMILY, 0);
		CHAR_talkToCli( talker, -1, "ÊØ»¤ÊÞ¸ÄÎªÆÕÍ¨ÊÞ", CHAR_COLORYELLOW);
	}
	CHAR_setCharPet( talker, emptyindex, petindex);
	CHAR_setCharPoolPet( talker, selectpet, -1);
	NPC_MaxGoldOver( meindex, talker, NPC_PETSHOP_MSG_POOLTHANKS, token);
	snprintf( szPet, sizeof( szPet ), "K%d", emptyindex);
	CHAR_sendStatusString( talker, szPet );
	snprintf( szPet, sizeof( szPet), "W%d",emptyindex);
	CHAR_sendStatusString( talker, szPet );
	for( i = 0; i < CHAR_MAXPOOLPETHAVE; i ++ ) {
		workindex[i] = -1;
	}
	cnt = 0;
	for( i = 0; i < CHAR_MAXPOOLPETHAVE; i ++ ) {
		int petindex = CHAR_getCharPoolPet( talker, i);
		if( CHAR_CHECKINDEX( petindex) ) {
			workindex[cnt++] = petindex;
		}
	}
	for( i = 0; i < CHAR_MAXPOOLPETHAVE; i ++ ) {
		CHAR_setCharPoolPet( talker, i, workindex[i]);
	}
	LogPet(
		CHAR_getChar( talker, CHAR_NAME ),
		CHAR_getChar( talker, CHAR_CDKEY ),
		CHAR_getChar( petindex, CHAR_NAME),
		CHAR_getInt( petindex, CHAR_LV),
		"draw(ÁìÈ¡³èÎïµê)",
		CHAR_getInt( talker,CHAR_FLOOR),
		CHAR_getInt( talker,CHAR_X ),
		CHAR_getInt( talker,CHAR_Y ),
		CHAR_getChar( petindex, CHAR_UNIQUECODE)   // shan 2001/12/14
	);

}	

void NPC_getDrawPet( int meindex, int toindex, char *token, int *buttontype, int select)
{
	// ¨àÊ¸¡õ³âØ¤Ð×Ô»¼°µæÐÑ
	#define		LINENUM		5
	char buf[1024];
	int i;
	int startnum, endnum;
	int petindex = -1;
	// òØ»þ¼°Ê¸¡õ³â
	int page = CHAR_getWorkInt( toindex, CHAR_WORKSHOPRELEVANT);
	int limit;
	//andy_reEdit 2003/09/18
	limit = (CHAR_getInt(toindex,CHAR_TRANSMIGRATION)*2)+5;

	endnum = (page + select+1)*LINENUM ;
	startnum = endnum - LINENUM;
	*buttontype = WINDOW_BUTTONTYPE_CANCEL;
	// ¸þÊ¸¡õ³âÆ©°ï£Û
	if( endnum >= limit ) {
		endnum = limit;
		if( page +select!= 0 ) *buttontype |= WINDOW_BUTTONTYPE_PREV;
	}
	else if( endnum <= LINENUM ) {
		endnum = LINENUM;
		*buttontype |= WINDOW_BUTTONTYPE_NEXT;
	}
	else {
		*buttontype |= WINDOW_BUTTONTYPE_PREV | WINDOW_BUTTONTYPE_NEXT;
	}
	CHAR_setWorkInt( toindex, CHAR_WORKSHOPRELEVANT, page +select);
	
	strcpy( token, "2\n¡¡¡¡¡¡¡¡ÇëÑ¡Ôñ³èÎï¡¡¡¡¡¡¡¡\n\n");
	for( i =  startnum; i < endnum; i ++ ) {
		petindex = CHAR_getCharPoolPet( toindex, i);
		if( CHAR_CHECKINDEX( petindex) ) {
			snprintf( buf, sizeof( buf), "LV.%3d MaxHP %3d %s\n", 
					CHAR_getInt( petindex, CHAR_LV),
					CHAR_getWorkInt( petindex, CHAR_WORKMAXHP),
					CHAR_getUseName( petindex)
					);
		}
		else {
			strcpy( buf, "\n");
		}
		strcat( token, buf);
	}
	// ÆÝ¼°ÓòòÛÃ«Æ©ÍÍ»¯£Ý  ¾®ÔÈÐ×ÈÕÆÝ³ßÊ¾Õý¼þÃ«äúÔÂ
	if( i != limit ) {
		petindex = CHAR_getCharPoolPet( toindex, i);
		if( !CHAR_CHECKINDEX( petindex)) {
			*buttontype &= ~WINDOW_BUTTONTYPE_NEXT;
		}
	}
}

/*
 *ÔÆàÅ»¥×ó¡õÌï¡õ¹«¼°Ö°ÖÐÇ·ÖÐÇ·
 */
void	NPC_MaxGoldOver(int meindex,int toindex,int select,char *token)
{

	char buf[1024];
	char npcarg[NPC_UTIL_GETARGSTR_BUFSIZE - 1024 * 20];

	NPC_Util_GetArgStr( meindex, npcarg, sizeof(npcarg));
	
	if( select < 0 || select >= arraysizeof( shopmsg)) return;
	
	if( NPC_Util_GetStrFromStrWithDelim( npcarg, shopmsg[select].option, buf, sizeof( buf))	== NULL ) 
	{
		strcpy( buf, shopmsg[select].defaultmsg);
	}

	snprintf(token,sizeof(buf),"\n\n%s",buf);

}
