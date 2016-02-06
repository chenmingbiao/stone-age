#ifndef __NPC_WARP_H__
#define __NPC_WARP_H__
BOOL NPC_WarpInit( int charaindex );
void NPC_WarpPostOver( int meindex,int charaindex );
void NPC_WarpWatch( int meobjindex, int objindex, CHAR_ACTION act,
                    int x,int y,int dir, int* opt,int optlen );
int NPC_WarpSearchByPosition( int fl , int x, int y);
void NPC_WarpWarpCharacter( int warpnpcindex, int charaindex );
#endif
 /*__NPC_WARP_H__*/
