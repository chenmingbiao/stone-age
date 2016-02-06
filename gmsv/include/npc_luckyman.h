#ifndef __NPC_LUCKYMAN_H__
#define __NPC_LUCKYMAN_H__

void NPC_LuckyManTalked( int meindex , int talkerindex , char *msg ,
                     int color );
BOOL NPC_LuckyManInit( int meindex );
void NPC_LuckyManWindowTalked( int meindex, int talkerindex, int seqno, int select, char *data);

#endif 
/*__NPC_LUCKYMAN_H__*/
