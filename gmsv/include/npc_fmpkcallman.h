#ifndef __NPC_FMPKCALLMAN_H__
#define __NPC_FMPKCALLMAN_H__

void NPC_FMPKCallManTalked(int meindex, int talkerindex, char *msg, int color);
BOOL NPC_FMPKCallManInit(int meindex);
void NPC_FMPKCallManWindowTalked(int meindex, int talkerindex, int seqno,
	int select, char *data);

#endif 
/*__NPC_FMPKCALLMAN_H__*/
