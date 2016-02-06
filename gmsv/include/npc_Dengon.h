#ifndef __NPC_DENGON_H__
#define __NPC_DENGON_H__
BOOL NPC_DengonInit( int meindex );
//void NPC_DengonSpecialTalked( int index, int talker, char *msg, int color );
void NPC_DengonLooked( int meindex, int lookedindex );
void NPC_DengonWindowTalked( int index, int talker, 
								int seqno, int select, char *data);

#endif

