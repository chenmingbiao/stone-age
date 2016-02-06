#define __BUF_C__
#include "version.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "buf.h"
#include "handletime.h"

static int UNIT;
static int UNITNUMBER;
static int memconfig;
static int readblock;
static int NowMemory;
static struct timeval AllocOldTime;

typedef struct tagMemory
{
    char*   pointer;
    BOOL    used;

    unsigned int     nsize;
}Memory;
static Memory *mem;


void memEnd( void )
{
  if( mem != NULL && mem[0].pointer != NULL ){
    free(mem[0].pointer);
    free(mem);
  }
}

BOOL configmem( int unit , int unitnumber )
{
    if( memconfig == TRUE )
        return FALSE;
    UNIT = unit;
    UNITNUMBER = unitnumber;
    if( UNIT <= 0 || UNITNUMBER <= 0 )
        return memconfig = FALSE;
    return memconfig = TRUE;
}

BOOL memInit( void )
{
    int i;
    if( memconfig == FALSE )
        return FALSE;
    mem = calloc( 1, sizeof( Memory ) * UNITNUMBER );
    if( mem == NULL ){
        print( "memInit: Can't alloc memory: %d\n" ,
               sizeof(Memory)*UNITNUMBER );
        return FALSE;
    }

    memset( mem , 0 , sizeof( Memory )* UNITNUMBER );
    for( i = 0 ; i < UNITNUMBER ; i ++ ){
        mem[i].pointer  =   NULL;
        mem[i].used     =   FALSE;
        mem[i].nsize    =   0;
    }
    mem[0].pointer = calloc( 1, UNIT*UNITNUMBER );
    if( mem[0].pointer == NULL ){
        print( "²»¿É·ÖÅä %d byte\n" , UNIT*UNITNUMBER );
        free( mem );
        return FALSE;
    }
    memset( mem[0].pointer , 0 , sizeof( UNIT*UNITNUMBER ));
		print( "ÄÚ´æÒÑ·ÖÅä %.2f MB..." ,UNIT*UNITNUMBER/1024.0/1024.0);
#ifdef DEBUG
    print( "Allocate %d byte( %.2fK byte %.2fM byte )\n" ,
           UNIT*UNITNUMBER,
           UNIT*UNITNUMBER/1024.0,
           UNIT*UNITNUMBER/1024.0/1024.0
        );
#endif
	readblock = 0;
    for( i = 0 ; i < UNITNUMBER ; i ++ )
        mem[i].pointer = mem[0].pointer + i * UNIT;

	NowMemory = 0;
	AllocOldTime.tv_sec = NowTime.tv_sec;
	AllocOldTime.tv_usec = NowTime.tv_usec;
	
  return TRUE;
}

void* allocateMemory( const unsigned int nbyte )
{
    int i;
    int arrayAllocSize;
	BOOL flg = FALSE;
    void *ret;
	int first = 0;
 
    arrayAllocSize = nbyte/UNIT + ( nbyte%UNIT ? 1 : 0 );
    if( arrayAllocSize == 0 )return NULL;
#ifdef DEBUG
    debug( arrayAllocSize , d );
    printf("(%.2f KB)\n",nbyte/1024.0);
#endif
	i = readblock;
	while( 1 ) {
		if( i > UNITNUMBER - arrayAllocSize) {
			i = 0;
		}
		if( mem[i].used != FALSE ){
			i += mem[i].nsize;
		}else{
            int	j;
            BOOL	found = TRUE;
            for( j = i; j < i + arrayAllocSize; j ++ ) {
			 	if( mem[j].used != FALSE ){
			 		i = j + mem[j].nsize;
					found = FALSE;
					if( first == 0 ) first = 1;
			 		break;
			 	}
            }
            if( found ) {
                mem[i].used=TRUE;
                mem[i].nsize = arrayAllocSize;
			    readblock = i + arrayAllocSize;
		        ret = mem[i].pointer;
				break;
			 }
		}
		if( ( i >= readblock || i > UNITNUMBER - arrayAllocSize )
            && flg == TRUE ) {
			ret = NULL;
			break;
		}
		if( i > UNITNUMBER - arrayAllocSize) {
			i = 0;
			flg = TRUE;
		}
		
	}
	if( ret == NULL ) {
	    print( "Can't Allocate %d byte .remnants:%4.2f\n" , nbyte, (float)(NowMemory/UNITNUMBER));
	}else {
		NowMemory += arrayAllocSize;

		if( NowTime.tv_sec > AllocOldTime.tv_sec +10 ) {
			print( "\n");
			if( NowMemory > (double)UNITNUMBER * 0.9) {
				print( "Warning!! Memory use rate exceeded 90%% .remnants:%d\n", UNITNUMBER-NowMemory);
			}else if( NowMemory > (double)UNITNUMBER * 0.8) {
				print( "Warning!! Memory use rate exceeded 80%% .remnants:%d\n", UNITNUMBER-NowMemory);
			}else if( NowMemory > (double)UNITNUMBER * 0.7) {
				print( "Memory use rate exceeded 70%% .remnants:%d\n", UNITNUMBER-NowMemory);
			}
			memcpy( &AllocOldTime, &NowTime, sizeof( AllocOldTime));
		}
		//print( "NowMemory.remnants:%4.2f\n", (float)(UNITNUMBER-NowMemory)/UNITNUMBER);
	}

	return ret;
}



/*------------------------------------------------------------
 * Â¦ÐÑ
 * ß¯Ô»°À
 *  Ø¦ØÆ
 ------------------------------------------------------------*/
void freeMemory( void* freepointer )
{
    int   arrayindex;
    char* toppointer;
    toppointer = mem[0].pointer;
    if( freepointer == NULL )return;
    arrayindex = ((int)freepointer-(int)toppointer)/UNIT;
	if( arrayindex < readblock) {
		readblock = arrayindex;
	}
    mem[arrayindex].used = FALSE;
	
	NowMemory -= mem[arrayindex].nsize;

}

void showMem( char *buf)
{
	sprintf( buf, "NowMemory.remnants:%d%%", ((UNITNUMBER-NowMemory)*100)/UNITNUMBER);
	printf( "\n" );
	printf( buf );
}

