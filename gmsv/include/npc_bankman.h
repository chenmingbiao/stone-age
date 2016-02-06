#ifndef __NPC_BANKMAN_H__
#define __NPC_BANKMAN_H__

void NPC_BankmanTalked( int meindex , int talkerindex , char *msg ,
                     int color );

BOOL NPC_BankmanInit( int meindex );
void NPC_BankmanLooked( int meindex, int lookedindex );
void NPC_BankmanWindowTalked( int meindex, int talkerindex, int seqno, int select, char *data);

#endif 

