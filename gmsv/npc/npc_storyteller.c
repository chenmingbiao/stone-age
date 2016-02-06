#include "version.h"
#include "char.h"

BOOL NPC_StoryTellerInit( int meindex )
{
    CHAR_setInt( meindex , CHAR_WHICHTYPE , CHAR_TYPESTORYTELLER );
    CHAR_setFlg( meindex , CHAR_ISATTACKED , 0 );
    CHAR_setFlg( meindex , CHAR_ISOVERED , 0 );

    return TRUE;
}


void NPC_StoryTellerTalked( int meindex , int talker , char *msg , int col )
{
    
}
