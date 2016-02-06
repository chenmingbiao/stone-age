#ifndef __MAPWARPPOINT_H__
#define __MAPWARPPOINT_H__

#include "common.h"
#include "util.h"

int MAPPOINT_InitMapWarpPoint( void);
void MAPPOINT_resetMapWarpPoint( int flg);
int MAPPOINT_loadMapWarpPoint( void);

BOOL MAPPOINT_CHECKINDEX( int ps);
int MAPPOINT_getMPointEVType( int ps);

int MAPPOINT_creatMapWarpObj( int pointindex, char *buf, int objtype);
int  MAPPOINT_setMapWarpGoal( int ps, char *buf);
int  MAPPOINT_setMapWarpFrom( int ps, char *buf);
int MAPPOINT_getMapWarpGoal( int ps, int ofl, int ox, int oy, int *fl, int *x, int *y);

void MAPPOINT_MapWarpHandle( int charaindex, int ps, int ofl, int ox, int oy );

#endif
