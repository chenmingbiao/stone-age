#include "version.h"
#include "common.h"
#include "char.h"
#include "readmap.h"


#define MAP_CHARFLOOR(x)   CHAR_getInt(x,CHAR_FLOOR)
#define MAP_CHARX(x)   CHAR_getInt(x,CHAR_X)
#define MAP_CHARY(x)   CHAR_getInt(x,CHAR_Y)

BOOL MAP_getMapDataFromCharIndex( int index  , int* map )
{
    int     mapdata[2];
    int     ret;

    ret = MAP_getTileAndObjData(MAP_CHARFLOOR(index),
                                MAP_CHARX(index),
                                MAP_CHARY(index),
                                &mapdata[0],&mapdata[1] );
    map[0] = mapdata[0];
    map[1] = mapdata[1];
    return ret;
}

BOOL MAP_getMapDataFromFXY( int f , int x , int y, int* map )
{
    int     mapdata[2];
    int     ret;

    ret = MAP_getTileAndObjData(f,x,y,&mapdata[0],&mapdata[1] );
    map[0] = mapdata[0];
    map[1] = mapdata[1];
    return ret;
}

