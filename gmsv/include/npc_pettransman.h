#ifndef __NPC_PETTRANSMAN_H__
#define __NPC_PETTRANSMAN_H__

BOOL NPC_PetTransManInit(int meindex);
void NPC_PetTransMan_selectWindow(int meindex,int toindex,int num,int select);
void NPC_PetTransManWindowTalked(int meindex,int talkerindex,int seqno,int select,char *data);
void NPC_PetTransManTalked(int meindex,int talkerindex,char *szMes,int color);
int NPC_PetTransManCheck( int meindex, int toindex, int select);
BOOL NPC_PetTransManStatus( int meindex, int toindex, int petNo);
#endif
 /*__NPC_PETTRANSMAN_H__*/
