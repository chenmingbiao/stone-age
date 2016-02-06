#ifndef __NPC_PETFUSION_H__
#define __NPC_PETFUSION_H__


void NPC_PetFusionManTalked( int meindex, int talkerindex, char *msg, int color );
BOOL NPC_PetFusionManInit( int meindex );
void NPC_PetFusionManWindowTalked( int meindex, int talkerindex, int seqno, int select, char *data);
void NPC_PetFusionManLoop( int meindex);

#endif

