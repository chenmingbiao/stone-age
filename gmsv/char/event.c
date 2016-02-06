#include "version.h"
#include <stdio.h>

#include "object.h"
#include "char_base.h"
#include "char.h"
#include "readmap.h"
#include "map_warppoint.h"
#include "event.h"
#include "npc_warp.h"
#include "npc_npcenemy.h"


static int EVENT_onWarpNPC( int charaindex,int echaraindex,int fl,int x, int y );
static int EVENY_npcEncount( int charaindex,int echaraindex,int fl,int x, int y );


typedef int     (*FUNC)( int charaindex,int echaraindex,int fl,int x, int y );
static FUNC		functbl[] = {
	NULL,
	NULL,				/* “·¿√NPC */
	EVENY_npcEncount,	/* “·¿√≥ƒæﬁº˛ –À¸º˛–˛ */
	EVENT_onWarpNPC,	/* warp*/
	NULL,				/* “·¿√NPC */
	NULL,				/* “·¿√NPC */
	EVENT_onWarpNPC,	/* warp*/
	EVENT_onWarpNPC,	/* warp*/
	EVENT_onWarpNPC,	/* warp*/
};

INLINE	BOOL EVENT_CHECKEVENTINDEX( int event)
{
	if( event < 0 || event >= CHAR_EVENTNUM) return FALSE;
	return TRUE;
}

int EVENT_main( int charaindex,int event, int x, int y)
{
	OBJECT	object;
	BOOL	found = FALSE;
	int		rc = FALSE;
	int fl = CHAR_getInt( charaindex, CHAR_FLOOR);
	if( !EVENT_CHECKEVENTINDEX( event)) return FALSE;

	for( object = MAP_getTopObj(fl,x,y) ; object ; object = NEXT_OBJECT(object) ){
		int o = GET_OBJINDEX(object);
		if( OBJECT_getType(o) == OBJTYPE_CHARA ){
			int	etype;
			int echaraindex=OBJECT_getIndex(o);
			if( !CHAR_CHECKINDEX(echaraindex) )continue;
			if( CHAR_getInt( echaraindex, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER) continue;
			etype = CHAR_getWorkInt( echaraindex, CHAR_WORKEVENTTYPE);
			if( etype != CHAR_EVENT_NONE ) {
				if( etype == event) {
					if( functbl[event] != NULL ) {
						rc = functbl[event]( charaindex, echaraindex, fl,x,y);
					}
					found = TRUE;
					break;
				}
			}
		}
#ifdef _MAP_WARPPOINT
		else if( OBJECT_getType(o) == OBJTYPE_WARPPOINT){
			if( !MAPPOINT_CHECKINDEX( OBJECT_getIndex( o)) ) continue;
			if( OBJECT_getchartype( o) != event ) continue;
			MAPPOINT_MapWarpHandle( charaindex, OBJECT_getIndex( o), fl, x, y );
			found = TRUE;
			rc = TRUE;
			break;
		}
#endif
	}
/*
	if( !found ) {
		print( "not found eventobject : cind [%d] event [%d] fl[%d] x[%d] y[%d]\n", charaindex, event, fl,x,y);
	}
*/
	return rc;
}

static int EVENT_onWarpNPC( int charaindex,int echaraindex,int fl,int x, int y )
{
	NPC_WarpWarpCharacter( echaraindex, charaindex);
	if( CHAR_getWorkInt( charaindex, CHAR_WORKPARTYMODE ) == CHAR_PARTY_LEADER ) {
		int		i;
		for( i = 1; i < CHAR_PARTYMAX; i ++ ) {
			int index = CHAR_getWorkInt( charaindex, i + CHAR_WORKPARTYINDEX1);
			if( CHAR_CHECKINDEX(index) ) {
				NPC_WarpWarpCharacter( echaraindex, index);
			}
		}
	}
	
	return TRUE;
}
static int EVENY_npcEncount( int charaindex,int echaraindex,int fl,int x, int y )
{
	return NPC_NPCEnemy_Encount( echaraindex, charaindex, 0);
}

