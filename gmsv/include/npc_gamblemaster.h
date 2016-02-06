#ifndef NPC_Gamble_Master_
#define NPC_Gamble_Master_

BOOL NPC_Gamble_MasterInit( int meindex );
void NPC_Gamble_MasterLoop( int meindex );
void NPC_Gamble_MasterWindowTalked( int meindex, int talkerindex, int seqno, int select, char *data);
void NPC_Gamble_MasterTalked( int meindex , int talkerindex , char *szMes , int color );
#endif

