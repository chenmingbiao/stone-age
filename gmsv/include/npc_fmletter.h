#ifndef __NPC_FMLETTER_H__
#define __NPC_FMLETTER_H__

void NPC_FmLetterTalked( int meindex , int talkerindex , char *msg ,
                     int color );

BOOL NPC_FmLetterInit( int meindex );
void NPC_FmLetterLooked( int meindex, int lookedindex );
void NPC_FmLetterWindowTalked( int meindex, int talkerindex, int seqno, int select, char *data);

#endif 

/*__NPC_FMLETTER_H__*/
