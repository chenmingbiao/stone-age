#include "version.h"
#include <stdio.h>
#include <string.h>

#include "common.h"
#include "char_base.h"
#include "char.h"
#include "npcutil.h"
#include "npc_mic.h"
#include "saacproto_cli.h"
#include "lssproto_serv.h"
#include "battle.h"

enum {
	CHAR_WORK_FLOOR = CHAR_NPCWORKINT1,
	CHAR_WORK_LX = CHAR_NPCWORKINT2,
	CHAR_WORK_LY = CHAR_NPCWORKINT3,
	CHAR_WORK_RX = CHAR_NPCWORKINT4,
	CHAR_WORK_RY = CHAR_NPCWORKINT5,
	CHAR_WORK_MODE = CHAR_NPCWORKINT6,
	CHAR_WORK_TYPE = CHAR_NPCWORKINT7,
	CHAR_WORK_WIND = CHAR_NPCWORKINT8,
	CHAR_WORK_FMFL = CHAR_NPCWORKINT9,
};


BOOL NPC_MicInit( int meindex )
{
	int		i;
	int	 	arg_param[8];
	char 	argstr[256];
	char	buf[64];
	
	
	NPC_Util_GetArgStr( meindex, argstr, sizeof(argstr));


    if( strstr(argstr, "FREE")!=NULL){
   		CHAR_setWorkInt(meindex,CHAR_WORK_TYPE,1);
   	}
    if( strstr(argstr, "WIND")!=NULL){
   		CHAR_setWorkInt(meindex,CHAR_WORK_WIND,1);
   	}
   	

	if( strstr( argstr , "|" ) != NULL){
		for( i = 1; i < 9; i ++ ) {
		    if( getStringFromIndexWithDelim(argstr, "|" , i , buf,sizeof(buf) )
	    		== FALSE ) 
	    	    {
	    		printf( "NPC_MIC:省略参数\n");
	    		arg_param[i-1] = 0;
	    		continue;
	    	    }
	   	    arg_param[i-1] = atoi(buf);
		}
		CHAR_setWorkInt( meindex, CHAR_WORK_FLOOR, arg_param[0]);
		CHAR_setWorkInt( meindex, CHAR_WORK_LX, min( arg_param[1],arg_param[3]));
		CHAR_setWorkInt( meindex, CHAR_WORK_LY, min( arg_param[2],arg_param[4]));
		CHAR_setWorkInt( meindex, CHAR_WORK_RX, max( arg_param[1],arg_param[3]));
		CHAR_setWorkInt( meindex, CHAR_WORK_RY, max( arg_param[2],arg_param[4]));
		CHAR_setInt( meindex, CHAR_WHICHTYPE, CHAR_TYPEMIC);
		
		CHAR_setWorkInt( meindex, CHAR_WORK_FMFL, arg_param[7]);
		return TRUE;

	}else{
		CHAR_setInt( meindex, CHAR_WHICHTYPE, CHAR_TYPEMIC);
		CHAR_setWorkInt( meindex, CHAR_WORK_MODE, 1);
		return TRUE;
	}

	return FALSE;
}

void NPC_MicTalked( int meindex , int talkerindex , char *msg ,int color )
{

	int	i;
    	int     playernum;
	int	x,y;
	char	message[4096];
	char	buf[2048];	

   	if( CHAR_getInt(talkerindex,CHAR_WHICHTYPE) != CHAR_TYPEPLAYER) return;
	if(CHAR_getWorkInt(meindex,CHAR_WORK_TYPE) == 0){
		if(NPC_Util_isFaceToChara(talkerindex,meindex,1 ) ==FALSE) return;
	}
		
	snprintf( buf, sizeof( buf), "%s\n%s",
				CHAR_getChar( talkerindex, CHAR_NAME),
				msg);
	
	// Robin 0621 族长广播
	print(" MIC_FL:%d ", CHAR_getWorkInt( meindex, CHAR_WORK_FMFL ));
	if( CHAR_getWorkInt( meindex, CHAR_WORK_FMFL ) != 0 )
	{
#ifdef _FMVER21	
		if( CHAR_getInt( talkerindex, CHAR_FMLEADERFLAG ) == FMMEMBER_LEADER) 
#else
		if( CHAR_getInt( talkerindex, CHAR_FMLEADERFLAG ) == 1) 
#endif		
		    // && ( CHAR_getWorkInt( talkerindex, CHAR_WORKFMFLOOR) == CHAR_getWorkInt( meindex, CHAR_WORK_FLOOR ) ) )
		{
			print(" FMLeaderTalk_send:%s,%d,%d,%s ",
				CHAR_getChar( talkerindex, CHAR_FMNAME), CHAR_getInt( talkerindex, CHAR_FMINDEX),
				CHAR_getWorkInt( talkerindex, CHAR_WORKFMINDEXI), buf);
			
			sprintf(message, "[族长广播]%s", buf);
			saacproto_ACFMAnnounce_send( acfd, 
						CHAR_getChar( talkerindex, CHAR_FMNAME), 
						CHAR_getInt( talkerindex, CHAR_FMINDEX),
						CHAR_getWorkInt( talkerindex, CHAR_WORKFMINDEXI),
						message,
						color
					);
			return;
		}
	    	
	}
	    	
	makeEscapeString( buf, message, sizeof(buf));

	playernum = CHAR_getPlayerMaxNum();

	for( i = 0; i < playernum; i ++ ) {
		BOOL flg = FALSE;
	        if( CHAR_getCharUse(i) == FALSE ) continue;
            	if( CHAR_getInt( i, CHAR_WHICHTYPE) != CHAR_TYPEPLAYER ) continue;
		if( CHAR_getInt( i, CHAR_FLOOR) != CHAR_getWorkInt( meindex, CHAR_WORK_FLOOR) ){
			continue;
		}
		if( CHAR_getWorkInt( meindex, CHAR_WORK_MODE) == 0 ) {
			x = CHAR_getInt( i, CHAR_X);
			y = CHAR_getInt( i, CHAR_Y);
			if( x >= CHAR_getWorkInt( meindex, CHAR_WORK_LX) && 
				x <= CHAR_getWorkInt( meindex, CHAR_WORK_RX) &&
				y >= CHAR_getWorkInt( meindex, CHAR_WORK_LY) &&
				y <= CHAR_getWorkInt( meindex, CHAR_WORK_RY)){
				flg = TRUE;
			}
		}else {
			flg = TRUE;
		}
		if( flg) {
			CHAR_talkToCli( i, talkerindex, msg, CHAR_COLORYELLOW);
			if( CHAR_getWorkInt( meindex, CHAR_WORK_WIND) == 1 ) {
				if( CHAR_getWorkInt( i, CHAR_WORKBATTLEMODE) 
					== BATTLE_CHARMODE_NONE) 
				{
					int fd = getfdFromCharaIndex( i);
					if( fd != -1 ) {
						lssproto_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,
										WINDOW_BUTTONTYPE_OK, 
										CHAR_WINDOWTYPE_MICMESSAGE,
										CHAR_getWorkInt( meindex, CHAR_WORKOBJINDEX),
										message);
					}
				}
			}
		}
	}
}
