#ifndef __NPC_RIDERMAN_H__
#define __NPC_RIDERMAN_H__

void NPC_RidermanTalked( int meindex , int talkerindex , char *msg ,
                     int color );

BOOL NPC_RidermanInit( int meindex );
void NPC_RidermanLooked( int meindex, int lookedindex );
void NPC_RidermanWindowTalked( int meindex, int talkerindex, int seqno, int select, char *data);

#endif
 /*__NPC_RIDERMAN_H__*/
