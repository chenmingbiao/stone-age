#ifndef __MAP_DEAL_H__
#define __MAP_DEAL_H__
BOOL MAP_walkAbleFromPoint( int ff, int fx, int fy, BOOL isfly );
BOOL MAP_walkAble( int index,int ff, int fx, int fy);
void MAP_preovered( int index );
void MAP_postovered( int index );
BOOL MAP_sendArroundCharNeedFD( int fd,int charaindex );
BOOL MAP_sendArroundChar(int charaindex);

#endif 

