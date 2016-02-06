#ifndef __NPC_FMPKMAN_H__
#define __NPC_FMPKMAN_H__

void NPC_FMPKManTalked(int meindex, int talkerindex, char *msg, int color);
BOOL NPC_FMPKManInit(int meindex);
void NPC_FMPKManWindowTalked(int meindex, int talkerindex, int seqno,
	int select, char *data);

#endif 
/*__NPC_FMPKMAN_H__*/
