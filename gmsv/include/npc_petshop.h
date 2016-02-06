#ifndef __NPC_PETSHOP_H__
#define __NPC_PETSHOP_H__

void NPC_PetShopTalked( int meindex , int talkerindex , char *msg ,
                     int color );
BOOL NPC_PetShopInit( int meindex );
void NPC_PetShopWindowTalked( int meindex, int talkerindex, int seqno, int select, char *data);
void NPC_PetShopLooked( int meindex , int lookedindex);


#endif 
/*__NPC_PETSHOP_H__*/
