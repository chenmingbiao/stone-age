#ifndef __NPC_CHECKMAN_H__
#define __NPC_CHECKMAN_H__

void NPC_CheckManTalked( int meindex , int talkerindex , char *msg ,
                     int color );
BOOL NPC_CheckManInit( int meindex );
void NPC_CheckManWindowTalked( int meindex, int talkerindex, int seqno, int select, char *data);
void NPC_CheckManLooked( int meindex , int lookedindex);

#endif

