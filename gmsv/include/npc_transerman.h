#ifndef _TRANSER_MAN_H
#define _TRANSER_MAN_H

void NPC_TranserManTalked( int meindex, int talkerindex, char *msg, int color );
BOOL NPC_TranserManInit( int meindex );
void NPC_TranserManWindowTalked( int meindex, int talkerindex, int seqno, int select, char *data);
void NPC_TranserManLoop( int meindex);

#endif
