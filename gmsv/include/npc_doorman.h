#ifndef __NPC_DOORMAN_H__
#define __NPC_DOORMAN_H__

BOOL NPC_DoormanInit( int meindex );
void NPC_DoormanTalked( int meindex , int talkerindex , char *msg ,
                     int color );

#endif 

