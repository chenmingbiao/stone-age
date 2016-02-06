#include "version.h"
#include "object.h"
#include "char_base.h"
#include "char.h"
#include "util.h"
#include "handletime.h"
#include "anim_tbl.h"
#include "npc_door.h"
#include "lssproto_serv.h"
#include "npcutil.h"


/*
 *  ÒýÇÐ¼°·òÎçÌ«Îç  by nakamura
 *
 *
 */

/*
 */
void NPC_TownPeopleTalked( int index, int talker, char *msg, int color )
{
	char arg[NPC_UTIL_GETARGSTR_BUFSIZE], token[NPC_UTIL_GETARGSTR_LINEMAX];
    int i, tokennum;


	if( CHAR_getInt(talker,CHAR_WHICHTYPE) == CHAR_TYPEPLAYER 
        && NPC_Util_charIsInFrontOfChar( talker, index, 3 ) ){

        NPC_Util_GetArgStr( index, arg, sizeof( arg));

        tokennum = 1;
        /* Îì¼þÑ¨Æ¥à¼å©ÈÕÄ¾Ð×Ðþ¡õÛÍ¼þ»¥ÖÏ³ðØ¤ÔÂ¾®ÐÑÒüÔÂ */
        for( i=0;arg[i]!='\0';i++ ){
            if( arg[i] == ',' ) tokennum++;
        }

        getStringFromIndexWithDelim( arg,",",
                                     rand()%tokennum+1,token, sizeof(token));

        CHAR_talkToCli( talker, index, token, CHAR_COLORWHITE );
    }
}

/*
 */
BOOL NPC_TownPeopleInit( int meindex )
{

    //CHAR_setInt( meindex , CHAR_HP , 0 );
    //CHAR_setInt( meindex , CHAR_MP , 0 );
    //CHAR_setInt( meindex , CHAR_MAXMP , 0 );
    //CHAR_setInt( meindex , CHAR_STR , 0 );
    //CHAR_setInt( meindex , CHAR_TOUGH, 0 );
    //CHAR_setInt( meindex , CHAR_LV , 0 );

    CHAR_setInt( meindex , CHAR_WHICHTYPE , CHAR_TYPETOWNPEOPLE );
    //CHAR_setFlg( meindex , CHAR_ISOVERED , 1 );
    
    return TRUE;
}
