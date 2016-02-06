#ifndef __NPC_FAMILYMAN_H__
#define __NPC_FAMILYMAN_H__

void NPC_FamilymanTalked( int meindex , int talkerindex , char *msg ,
                     int color );

BOOL NPC_FamilymanInit( int meindex );
void NPC_FamilymanLooked( int meindex, int lookedindex );
void NPC_FamilymanWindowTalked( int meindex, int talkerindex, int seqno, int select, char *data);

#endif 

/*__NPC_FAMILYMAN_H__*/
