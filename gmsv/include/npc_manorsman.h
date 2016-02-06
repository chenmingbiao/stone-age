#ifndef __NPC_MANORSMAN_H__
#define __NPC_MANORSMAN_H__
#include <time.h>
BOOL NPC_ManorSmanInit(int meindex);
void NPC_ManorSmanTalked(int meindex, int talkerindex, char *msg, int color);
void NPC_ManorSmanWindowTalked(int meindex, int talkerindex,
				 int seqno, int select, char *data);
void NPC_ManorSmanLoop(int meindex);
//andy_123
int NPC_getManorsmanListIndex( int ID);
void NPC_ManorSavePKSchedule(int meindex, int toindex, int flg);
#endif 
/*__NPC_MANORSMAN_H__*/
