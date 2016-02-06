#ifndef __NPC_TIMEMAN_H__
#define __NPC_TIMEMAN_H__


void NPC_TimeManTalked( int meindex , int talkerindex , char *msg ,
                     int color );

BOOL NPC_TimeManInit( int meindex );

void NPC_TimeManWatch( int meobjindex, int objindex, CHAR_ACTION act,
                    int x,int y,int dir, int* opt,int optlen );

#endif /*__NPC_TIMEMAN_H__*/
