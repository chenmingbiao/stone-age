#include "version.h"
#include <string.h>
#include "char.h"
#include "object.h"
#include "char_base.h"
#include "npcutil.h"
#include "npc_action.h"

/* 
 * 皿伊奶乩□及失弁扑亦件卞  杀允月NPC［
 * 签卞韵曰忒仄凶曰允月分仃分互［
 * 汹仁］引凶反  匀化中月及失弁扑亦件卞反  杀仄卅中［
 *
 * 娄醒“
 *      msgcol:		丢永本□斥及缙［犯白巧伙玄反疵缙
 *      normal:		  骚卞韵匀化五凶凛支  躲卅失弁扑亦件卞覆仄化及忒蚕
 *		attack:		  猾失弁扑亦件卞覆允月忒蚕
 *		damage:		母丢□斥毛熬仃凶失弁扑亦件卞覆允月忒蚕
 *		down:		逦木月失弁扑亦件卞覆允月忒蚕
 *		sit:		甄月失弁扑亦件卞覆仄化及忒蚕
 *		hand:		澎毛蕊月失弁扑亦件卞覆仄化及忒蚕
 *		pleasure:	减少失弁扑亦件卞覆仄化及忒蚕
 *		angry:		变月失弁扑亦件卞覆仄化及忒蚕
 *		sad:		  仄戈失弁扑亦件卞覆仄化及忒蚕
 *		guard:		布□玉允月失弁扑亦件卞覆仄化及忒蚕
 */
 

#define		NPC_ACTION_MSGCOLOR_DEFAULT		CHAR_COLORYELLOW

enum {
	CHAR_WORK_MSGCOLOR	= CHAR_NPCWORKINT1,
};

/*********************************
* 赓渝质  
*********************************/
BOOL NPC_ActionInit( int meindex )
{
	char	argstr[NPC_UTIL_GETARGSTR_BUFSIZE];
	int		tmp;
	
	tmp = NPC_Util_GetNumFromStrWithDelim( argstr, "msgcol");
	if( tmp == -1 ) tmp = NPC_ACTION_MSGCOLOR_DEFAULT;
	CHAR_setWorkInt( meindex, CHAR_WORK_MSGCOLOR, tmp);
    
    CHAR_setInt( meindex , CHAR_WHICHTYPE , CHAR_TYPEACTION );
	
	
    return TRUE;
}




/*********************************
*   仄井仃日木凶凛及质  
*********************************/
void NPC_ActionTalked( int meindex , int talkerindex , char *szMes ,
                     int color )
{
	char	argstr[NPC_UTIL_GETARGSTR_BUFSIZE];
	char	buf[64];

    /* 皿伊奶乩□卞覆仄化分仃  杀允月 */
    if( CHAR_getInt( talkerindex , CHAR_WHICHTYPE ) != CHAR_TYPEPLAYER ) {
    	return;
    }
	/* ㄠ弘伉永玉动  及心 */
	if( !NPC_Util_charIsInFrontOfChar( talkerindex, meindex, 1 )) return; 

	NPC_Util_GetArgStr( meindex, argstr, sizeof( argstr));

	if( NPC_Util_GetStrFromStrWithDelim( argstr, 
										"normal",
										buf, sizeof( buf))
    	!= NULL )
	{
    	CHAR_talkToCli( talkerindex, meindex ,buf , 
    					CHAR_getWorkInt( meindex, CHAR_WORK_MSGCOLOR ));
	}
	
}
/*********************************
* watch质  
*********************************/
void NPC_ActionWatch( int meobjindex, int objindex, CHAR_ACTION act,
                    int x,int y,int dir, int* opt,int optlen )
{
	int		meindex;
	int		index;
	char	argstr[NPC_UTIL_GETARGSTR_BUFSIZE];
	char	buf[64];
	int		i;
    struct  {
    	CHAR_ACTION		act;
    	char			*string;
    }searchtbl[] = {
	    { CHAR_ACTATTACK,	"attack"},
	    { CHAR_ACTDAMAGE,	"damage"},
		{ CHAR_ACTDOWN,		"down"},
		{ CHAR_ACTSIT,		"sit"},
		{ CHAR_ACTHAND,		"hand"},
		{ CHAR_ACTPLEASURE,	"pleasure"},
		{ CHAR_ACTANGRY,	"angry"},
		{ CHAR_ACTSAD,		"sad"},
		{ CHAR_ACTGUARD,	"guard"},
		{ CHAR_ACTNOD,		"nod"},
		{ CHAR_ACTTHROW,	"throw"},
    };
	
	if( OBJECT_getType( objindex) != OBJTYPE_CHARA) return;
	index = OBJECT_getIndex( objindex);
	/* 皿伊奶乩□卞及心  杀允月 */
	if( CHAR_getInt( index, CHAR_WHICHTYPE) != CHAR_TYPEPLAYER) return;
    
    meindex = OBJECT_getIndex( meobjindex);
    
    /* 轾五宁匀化ㄠ弘伉永玉匹卅中午  杀仄卅中 */
    if( NPC_Util_isFaceToFace( meindex, index, 1 ) != TRUE ) return;
	
	NPC_Util_GetArgStr( meindex, argstr, sizeof( argstr));
	
	for( i = 0; i < arraysizeof( searchtbl); i ++ ) {
		if( searchtbl[i].act == act) {
			if( NPC_Util_GetStrFromStrWithDelim( argstr, 
												searchtbl[i].string,
												buf, sizeof( buf))
		    	!= NULL )
			{
		    	CHAR_talkToCli( index, meindex ,buf , 
		    					CHAR_getWorkInt( meindex, CHAR_WORK_MSGCOLOR ));
				break;
			}
		}
	}
}
