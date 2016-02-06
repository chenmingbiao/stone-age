#ifndef __NPC_POOLITEMSHOP_H__
#define __NPC_POOLITEMSHOP_H__


void NPC_PoolItemShopLoop( int meindex);
BOOL NPC_PoolItemShopInit( int meindex);
void NPC_PoolItemShopTalked( int meindex , int talkerindex , 
							char *szMes ,int color );
void NPC_PoolItemShopWindowTalked( int meindex, int talkerindex, 
								int seqno, int select, char *data);


#endif
 /*__NPC_POOLITEMSHOP_H__*/
