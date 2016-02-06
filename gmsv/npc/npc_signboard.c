#include "version.h"
#include <string.h>
#include "char.h"
#include "object.h"
#include "char_base.h"
#include "npcutil.h"
#include "lssproto_serv.h"
#include "readmap.h"
// Arminius 8.4 manor
#include "family.h"

static void NPC_SignBoard_Window( int meindex, int toindex, int num,int select);


/*********************************
*********************************/
BOOL NPC_SignBoardInit( int meindex )
{

	char npcarg[NPC_UTIL_GETARGSTR_BUFSIZE];
	/*--正奶皿涩烂--*/
   	CHAR_setInt( meindex , CHAR_WHICHTYPE , CHAR_TYPEMSG );

	if(NPC_Util_GetArgStr( meindex, npcarg, sizeof(npcarg))==NULL){
		print("GetArgStrErr");
		return FALSE;
	}

	return TRUE;

}




/*********************************
*********************************/
void NPC_SignBoardLooked( int meindex , int lookedindex)
{

    if( CHAR_getInt( lookedindex , CHAR_WHICHTYPE ) != CHAR_TYPEPLAYER ) {
    	return;
    }

	if( NPC_Util_CharDistance( lookedindex, meindex ) > 1) return;


	NPC_SignBoard_Window( meindex, lookedindex,0,-1);

}


static void NPC_SignBoard_Window( int meindex, int toindex, int num,int select)
{

	char npcarg[NPC_UTIL_GETARGSTR_BUFSIZE];
	char token[1024];
	int fd = getfdFromCharaIndex( toindex);


	if(NPC_Util_GetArgStr( meindex, npcarg, sizeof(npcarg))==NULL){
		print("GetArgStrErr");
		return;
	}
	
	// Arminius: manor
	{
	  char *p,*q;
	  extern struct FM_POINTLIST  fmpointlist;
	  p=strstr(npcarg, "%manorid:");
	  if (p!=NULL) {
	    char tmp[1024];
	    int a;
	    
	    q=strstr(p+1,"%");
	    if (q!=NULL) {
  	      q[0]='\0';
 	      p[0]='\0';
	      a=atoi(p+9);
#ifdef _FAMILY_MANORNUM_CHANGE	// CoolFish 用来修改装园数量
	      if ((a>=1) && (a<=MANORNUM)) {		  
#else
	      if ((a>=1) && (a<=4)) {
#endif
                getStringFromIndexWithDelim(fmpointlist.pointlistarray[a-1], "|", 5, token, sizeof(token));
                if (atoi(token)>=0) {
                  getStringFromIndexWithDelim(fmpointlist.pointlistarray[a-1], "|", 6, token, sizeof(token));
  	          sprintf(tmp,"%s%s%s",npcarg,token,q+1);
	          strcpy(npcarg, tmp);
	        } else {
	          sprintf(tmp,"%s没有任何家族%s",npcarg,q+1);
	          strcpy(npcarg, tmp);
	        }
	      }
	    }
	  }
	}

	sprintf(token, "　　　　　　＜　看板　＞\n%s",npcarg);
	
	/*--霜耨--*/
	lssproto_WN_send( fd,WINDOW_MESSAGETYPE_MESSAGE,
					WINDOW_BUTTONTYPE_OK,
					CHAR_WINDOWTYPE_WINDOWPETSHOP_START,
					CHAR_getWorkInt( meindex, CHAR_WORKOBJINDEX),
				token);
}



/*-----------------------------------------
 *
-------------------------------------------*/
void NPC_SignBoardWindowTalked( int meindex, int talkerindex, 
								int seqno, int select, char *data)
{
	
	if( NPC_Util_CharDistance( talkerindex, meindex ) > 1) {
		return;
	}
}

