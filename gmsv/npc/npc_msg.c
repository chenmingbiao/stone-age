#include "version.h"
#define _NPC_MSG_C_

#include "char.h"
#include "char_base.h"
#include "npcutil.h"


/*
  霾滇读卅枣  ［  飓反窒匹手方仁［
  午曰丐尹内升及  轾井日匹手苇尹月方丹卞允月［

  init , looked 及心卞  杀

  */  
BOOL NPC_MsgInit( int meindex )
{
    //CHAR_setInt( meindex , CHAR_HP , 0 );

    //CHAR_setInt( meindex , CHAR_MP , 0 );
    //CHAR_setInt( meindex , CHAR_MAXMP , 0 );

    //CHAR_setInt( meindex , CHAR_STR , 0 );
    //CHAR_setInt( meindex , CHAR_TOUGH, 0 );

    //CHAR_setInt( meindex , CHAR_LV , 0 );

    CHAR_setInt( meindex , CHAR_WHICHTYPE , CHAR_TYPETOWNPEOPLE );
    //CHAR_setFlg( meindex , CHAR_ISOVERED , 0 );
    //CHAR_setFlg( meindex , CHAR_ISATTACKED , 0 );  /*   猾今木卅中方氏 */

    return TRUE;
}


/*
 * 苇日木凶［反卅允分仃［
 */
void NPC_MsgLooked( int meindex , int lookedindex )
{
	char	arg[NPC_UTIL_GETARGSTR_BUFSIZE];
    
	NPC_Util_GetArgStr( meindex, arg, sizeof( arg));
    CHAR_talkToCli( lookedindex , -1 , arg , CHAR_COLORWHITE );

}
