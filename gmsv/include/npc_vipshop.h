#ifndef _VIP_SHOP_H
#define _VIP_SHOP_H

void NPC_VipshopTalked( int meindex, int talkerindex, char *msg, int color );
BOOL NPC_VipshopInit( int meindex );
void NPC_VipshopWindowTalked( int meindex, int talkerindex, int seqno, int select, char *data);
void NPC_VipshopLoop( int meindex);

#endif
