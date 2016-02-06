#ifndef NPC_PetRace_Master_
#define NPC_PetRace_Master_

BOOL NPC_PetRaceMasterInit( int meindex );
void NPC_PetRaceMasterLoop( int meindex );
void NPC_PetRaceMasterTalked( int meindex , int talkerindex , char *szMes , int color );
void NPC_PetRaceMasterWindowTalked(int meindex, int talkerindex, int seqno, int select, char *data);

#endif

