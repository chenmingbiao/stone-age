#ifndef __NPC_NPCENEMY_H__
#define __NPC_NPCENEMY_H__

void NPC_NPCEnemyTalked( int meindex , int talkerindex , char *msg ,
                     int color );

BOOL NPC_NPCEnemyInit( int meindex );
int NPC_NPCEnemy_Dying( int battleindex, int meindex );
int NPC_NPCEnemy_Encount( int meindex, int charaindex, int mode);
void NPC_NPCEnemyWatch( int meobjindex, int objindex, CHAR_ACTION act,
                    int x,int y,int dir, int* opt,int optlen );
void NPC_NPCEnemyLoop( int meindex );
BOOL NPC_NPCEnemy_BattleIn(  int meindex, int charaindex);
void NPC_NPCEnemyWindowTalked( int meindex, int talkerindex, int seqno, int select, char *data);

#endif
 /*__NPC_NPCENEMY_H__*/
