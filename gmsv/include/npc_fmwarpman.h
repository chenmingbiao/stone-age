#ifndef __NPC_FMWARPMAN_H__
#define __NPC_FMWARPMAN_H__

void NPC_FMWarpManTalked(int meindex, int talkerindex, char *msg, int color);
BOOL NPC_FMWarpManInit(int meindex);
void NPC_FMWarpManWindowTalked(int meindex, int talkerindex, int seqno,
	int select, char *data);
void NPC_FMWarpManLoop(int meindex);
void NPC_GetPKFMNum(int floor, int fmindex1, int fmindex2, int *num1, int *num2);

#endif 
/*__NPC_FMWARPMAN_H__*/
