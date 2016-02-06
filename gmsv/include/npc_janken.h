#ifndef __NPC_JANKEN_H__
#define __NPC_JANKEN_H__

void NPC_JankenTalked( int meindex , int talkerindex , char *msg ,
                     int color );
BOOL NPC_JankenInit( int meindex );
void NPC_JankenWindowTalked( int meindex, int talkerindex, int seqno, int select, char *data);

#endif
 /*__NPC_JANKEN_H__*/
