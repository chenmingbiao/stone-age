#ifndef __NPC_GAMBLEBANK_H__
#define __NPC_GAMBLEBANK_H__
BOOL NPC_GambleBankInit( int meindex );
void NPC_GambleBankLoop( int meindex);
void NPC_GambleBankTalked( int meindex , int talkerindex , char *msg , int color );
void NPC_GambleBankWindowTalked( int meindex, int talkerindex, int seqno, int select, char *data);
#endif


