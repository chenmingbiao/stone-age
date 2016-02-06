#ifndef __NPC_AIRPLANE_H__
#define __NPC_AIRPLANE_H__

BOOL NPC_AirInit( int meindex );
void NPC_AirLoop( int meindex );
void NPC_AirTalked( int meindex , int talkerindex , char *szMes ,
                     int color );
BOOL NPC_AirCheckAllowItem( int meindex, int charaindex, BOOL pickupmode);
BOOL NPC_AirCheckJoinParty( int meindex, int charaindex, BOOL msgflg);

#endif 
/*__NPC_AIRPLANE_H__*/
