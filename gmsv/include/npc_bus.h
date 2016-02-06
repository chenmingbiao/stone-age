#ifndef __NPC_BUS_H__
#define __NPC_BUS_H__

BOOL NPC_BusInit( int meindex );
void NPC_BusLoop( int meindex );
void NPC_BusTalked( int meindex , int talkerindex , char *szMes ,
                     int color );
BOOL NPC_BusCheckAllowItem( int meindex, int charaindex, BOOL pickupmode);
BOOL NPC_BusCheckJoinParty( int meindex, int charaindex, BOOL msgflg);

#endif

