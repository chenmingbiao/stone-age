#ifndef __NPC_CHARM_H__
#define __NPC_CHARM_H__

void NPC_CharmTalked( int meindex , int talkerindex , char *msg ,
                     int color );
BOOL NPC_CharmInit( int meindex );
void NPC_CharmWindowTalked( int meindex, int talkerindex, int seqno, int select, char *data);
void NPC_CharmLooked( int meindex , int lookedindex);

#endif 

