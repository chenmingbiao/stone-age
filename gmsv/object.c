#include "version.h"
#include <sys/stat.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>

#include "handletime.h"

#include "common.h"
#include "object.h"
#include "char_base.h"
#include "char.h"
#include "item.h"
#include "buf.h"
#include "readmap.h"
#include "pet.h"

static Object *obj;
static int objnum;

BOOL initObjectArray( int num )
{
    int     i;
    objnum = num;
    obj = allocateMemory( sizeof( Object ) * objnum );
    
    if( obj == NULL ) return FALSE;
    for( i = 0 ; i < objnum ; i ++ ){
        memset( &obj[i], 0 , sizeof( Object ));
        obj[i].type = OBJTYPE_NOUSE;
    }
    print( "·ÖÅä %.2f MB MB ¿Õ¼ä...", sizeof( Object ) * objnum / 1024.0 / 1024.0 );
    return TRUE;
}


BOOL endObjectArray( void )
{
    freeMemory( obj );
    return TRUE;
}

INLINE int _initObjectOne( char *file, int line, Object* ob )
{
    static int allocobjnum = 0;
    int i;
    BOOL    first;

    i = allocobjnum;
    first = TRUE;
    while( 1 ) {
        if( !first && i >= allocobjnum ) {
            fprint( "Can't allocate an object\n" );
            return -1;
        }
        if( obj[i].type == OBJTYPE_NOUSE ) {
            if( MAP_addNewObj( ob->floor ,ob->x, ob->y, i ) == 1 ){
                memcpy( &obj[i] , ob , sizeof( Object ) );
                allocobjnum = ( i+1 >= objnum) ? 0:i+1;
                return i;
            }else{
                allocobjnum = ( i+1 >= objnum) ? 0:i+1;
                fprint( "MAP_addNewObj error\n" );
                return -1;
            }
        }else {
            if( ++i >= objnum ) {
                i = 0;
                first = FALSE;
            }
        }
    }

    fprint( "Can't allocate an object\n" );
    return -1;
}

void endObjectOne( int index )
{
    if(objnum <= index || index < 0 )return;

    if( MAP_removeObj(obj[index].floor,obj[index].x, obj[index].y,
                      index) == FALSE){
//        fprint( "REMOVE OBJ ERROR  floor:%d  x:%d  y:%d\n",obj[index].floor,obj[index].x, obj[index].y );
    }
    obj[index].type = OBJTYPE_NOUSE;
}

BOOL CHECKOBJECT( int index )
{
    if( 0<=index && index<objnum )return TRUE;
    return FALSE;
}

BOOL CHECKOBJECTUSE( int index )
{
    if( CHECKOBJECT(index) == FALSE )return FALSE;
    if( obj[index].type == OBJTYPE_NOUSE )return FALSE;
    return TRUE;
}

INLINE int OBJECT_getType( int index )
{
    return obj[index].type;
}

INLINE int OBJECT_getchartype( int index )
{
    return obj[index].chartype;
}
INLINE void OBJECT_setchartype( int index, int flg)
{
    obj[index].chartype = flg;
}

INLINE int OBJECT_setType( int index, int newvalue )
{
    int     old;
    old = obj[index].type;
    obj[index].type = newvalue;
    return old;
}

INLINE int OBJECT_getFloor( int index )
{
    return obj[index].floor;
}

INLINE int OBJECT_setFloor( int index, int newvalue )
{
    int     old;
    old = obj[index].floor;
    obj[index].floor = newvalue;
    return old;
}

INLINE int OBJECT_getX( int index )
{
    return obj[index].x;
}

INLINE int OBJECT_setX( int index, int newvalue )
{
    int     old;
    old = obj[index].x;
    obj[index].x = newvalue;
    return old;
}

INLINE int OBJECT_getY( int index )
{
    return obj[index].y;
}

INLINE int OBJECT_setY( int index, int newvalue )
{
    int     old;
    old = obj[index].y;
    obj[index].y = newvalue;
    return old;
}

#ifdef _DEL_DROP_GOLD
INLINE int OBJECT_getTime( int index )
{
    return obj[index].time;
}

INLINE int OBJECT_setTime( int index, int newvalue )
{
    int     old;
    old = obj[index].time;
    obj[index].time = newvalue;
    return old;
}
#endif


/*------------------------------------------------------------
 * index Ã«  ÔÂ
 * Â¦ÐÑ
 *  index      int     ÄÌ¼þ·¸ÓÀÛÍµ©
 * ß¯Ô»°À
 *  int
 ------------------------------------------------------------*/
INLINE int OBJECT_getIndex( int index )
{
    return obj[index].index;
}
/*------------------------------------------------------------
 * y Ã«É¬ÀÃÔÊÔÂ
 * Â¦ÐÑ
 *  index      int     ÄÌ¼þ·¸ÓÀÛÍµ©
 *  newvalue    int     Þ¥ØÆÖÐ°À
 * ß¯Ô»°À
 *  int éÉ¼°°À
 ------------------------------------------------------------*/
INLINE int OBJECT_setIndex( int index, int newvalue )
{
    int     old;
    old = obj[index].index;
    obj[index].index = newvalue;
    return old;
}


/*------------------------------------------------------------
 * ×óÆ¤³âÄáÛÍÐþ¼°    ÐÑÃ«·Æ»§ÔÂ
 * Â¦ÐÑ
 *  Ø¦ØÆ
 * ß¯Ô»°À
 *  int
 ------------------------------------------------------------*/
INLINE int OBJECT_getNum( void )
{
    return objnum;
}

/*------------------------------------------------------------
 *  ×óÆ¤³âÄáÛÍÐþÑáÕ°  ¼°¶ª¼þÌïÃ«òå  Â¦ÐÑ±å  ÔÈ»¯
 *  ×óÆ¤³âÄáÛÍÐþÑáÕ°  Ã«×ÛÔ»ÇëØÆ»¯£ýè¶  ÔÊÔÂ£Û
 *
 * Â¦ÐÑ
 *  type    OBJTYPE     ×óÆ¤³âÄáÛÍÐþ¼°ÕýÄÌÃó
 *  index   int         ¹«Ä¾ÈßÄ¾¼°    Æ¥¼°index
 *  x       int         xÕç  
 *  y       int         yÕç  
 *  floor   int         floorid
 * ß¯Ô»°À
 *   int  à«Ô»ñ²»¯Ä¾Ð×index, ÁÃ  ¼°ÁÝ·´£ý -1
 ------------------------------------------------------------*/
int initObjectFromObjectMember(
    OBJTYPE type, int index, int x, int y , int floor )
{
    Object  obj;
    obj.type = type;
    obj.index = index;
    obj.x = x;
    obj.y = y;
    obj.floor = floor;

    return initObjectOne( &obj );
}

int searchObjectFromCharaIndex( int index )
{
    int i;
    for( i = 0 ; i < objnum ; i ++ )
        if( obj[i].type == OBJTYPE_CHARA && obj[i].index == index )
            return i;
    return -1;
}

#define ITEMGOLDSTOREFILENAME   "itemgold"

#define STOREITEMID         "ITEM"
#define STOREGOLDID         "GOLD"
#define	STORECHARID			"CHAR"

int detectStoreFile( char*  filename )
{
    FILE*   f;
    char    buffer[10];
    f = fopen ( filename ,"r" );
    if( f == NULL )return 3;

    if( fgets( buffer, sizeof( buffer ),  f ) == NULL ){
        fclose(f);
        return 3;
    }

    if( strncmp(buffer,STOREITEMID,sizeof(STOREITEMID) ) == 0 ||
        strncmp(buffer,STOREGOLDID,sizeof(STOREGOLDID) ) == 0   ){
        fclose(f);
        return 2;
    }

    fclose(f);
    return 3;
}

