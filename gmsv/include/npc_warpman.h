#ifndef __NPC_WARPMAN_H__
#define __NPC_WARPMAN_H__

void NPC_WarpManTalked( int meindex , int talkerindex , char *msg ,
                     int color );
BOOL NPC_WarpManInit( int meindex );
void NPC_WarpManWindowTalked( int meindex, int talkerindex, int seqno, int select, char *data);
void NPC_GetDuelRank(int rank,int msgid,int msgid2);
void NPC_WarpManLoop( int meindex);
void NPC_WarpManWatch( int meobjindex, int objindex, CHAR_ACTION act,
                    int x,int y,int dir, int* opt,int optlen );
#endif 
/*__NPC_WARPMAN_H__*/
