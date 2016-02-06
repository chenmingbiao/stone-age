#ifndef NPC_Gamble_Roulette_
#define NPC_Gamble_Roulette_

BOOL NPC_Gamble_RouletteInit( int meindex );
void NPC_Gamble_RouletteLoop( int meindex );
void NPC_Gamble_RouletteWindowTalked( int meindex, int talkerindex, int seqno, int select, char *data);
void NPC_Gamble_RouletteTalked( int meindex , int talkerindex , char *szMes , int color );
#endif

