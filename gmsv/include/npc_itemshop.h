#ifndef __NPC_ITEMSHOP_H__
#define __NPC_ITEMSHOP_H__

void NPC_ItemShopTalked( int meindex , int talkerindex , char *msg ,
                     int color );
BOOL NPC_ItemShopInit( int meindex );
void NPC_ItemShopWindowTalked( int meindex, int talkerindex, int seqno, int select, char *data);
void NPC_ItemShopLooked( int meindex , int lookedindex);


#endif
 /*__NPC_ITEMSHOP_H__*/
