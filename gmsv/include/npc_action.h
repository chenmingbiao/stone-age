#ifndef __NPC_ACTION_H__
#define __NPC_ACTION_H__

void NPC_ActionTalked( int meindex , int talkerindex , char *msg ,
                     int color );

BOOL NPC_ActionInit( int meindex );
void NPC_ActionWatch( int meobjindex, int objindex, CHAR_ACTION act,
                    int x,int y,int dir, int* opt,int optlen );

#endif 

