#include "version.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>
#include <unistd.h>

#include <netinet/in.h>
#include <sys/stat.h>

#include "common.h"
#include "object.h"
#include "readmap.h"
#include "buf.h"
#include "util.h"
#include "char.h"
#include "lssproto_serv.h"
#include "configfile.h"

#define MAP_MAGIC  "LS2MAP"

static void MAP_makeDefaultImageData( MAP_ImageData* one )
{
    one->data[MAP_WALKABLE]     = TRUE;
    one->data[MAP_HAVEHEIGHT]   = FALSE;
    one->data[MAP_DEFENCE]     = 0;
    one->data[MAP_INTODAMAGE]   = 0;
    one->data[MAP_OUTOFDAMAGE]  = 0;

    one->data[MAP_SETED_BATTLEMAP]    = 0;
    one->data[MAP_BATTLEMAP]       = 0;
    one->data[MAP_BATTLEMAP2]       = 0;
    one->data[MAP_BATTLEMAP3]       = 0;
    one->data[MAP_INTODARKNESS]     = 0;
    one->data[MAP_INTOCONFUSION]    = 0;

    one->data[MAP_OUTOFPOISON]       = 0;
    one->data[MAP_OUTOFPARALYSIS]    = 0;
    one->data[MAP_OUTOFSILENCE]      = 0;
    one->data[MAP_OUTOFSTONE]        = 0;
    one->data[MAP_OUTOFDARKNESS]     = 0;
    one->data[MAP_OUTOFCONFUSION]    = 0;

}

static int MAP_flgSet( char* string )
{
    int     tmp;
    tmp = atoi(string);
    if( tmp == 0 )
        return 0;
    return 1;
}
typedef enum
{
    MAP_INTENTRY=0,
    MAP_CHARENTRY=1,
    MAP_INTFUNC=2,
    MAP_CHARFUNC=3,
}MAP_DATATYPECATEGORY;
static struct MAP_itemconfentry
{
    int     type;
    int     index;
    void*   func;
}MAP_confentries[MAP_DATAINT_NUM+MAP_DATACHAR_NUM]={
    {MAP_INTENTRY,  MAP_WALKABLE,       MAP_flgSet},
    {MAP_INTENTRY,  MAP_HAVEHEIGHT,     MAP_flgSet},
    {MAP_INTENTRY,  MAP_DEFENCE,       NULL},
    {MAP_INTENTRY,  MAP_INTODAMAGE,     NULL},
    {MAP_INTENTRY,  MAP_OUTOFDAMAGE,    NULL},

    {MAP_INTENTRY,  MAP_SETED_BATTLEMAP,  NULL},
    {MAP_INTENTRY,  MAP_BATTLEMAP,	    NULL},
    {MAP_INTENTRY,  MAP_BATTLEMAP2,	    NULL},
    {MAP_INTENTRY,  MAP_BATTLEMAP3,	    NULL},
    {MAP_INTENTRY,  MAP_INTODARKNESS,   NULL},
    {MAP_INTENTRY,  MAP_INTOCONFUSION,  NULL},

    {MAP_INTENTRY,  MAP_OUTOFPOISON,    NULL},
    {MAP_INTENTRY,  MAP_OUTOFPARALYSIS, NULL},
    {MAP_INTENTRY,  MAP_OUTOFSILENCE,   NULL},
    {MAP_INTENTRY,  MAP_OUTOFSTONE,     NULL},
    {MAP_INTENTRY,  MAP_OUTOFDARKNESS,  NULL},
    {MAP_INTENTRY,  MAP_OUTOFCONFUSION, NULL},

};

static MAP_ImageData*  MAP_imagedata;
static unsigned int    MAP_imagedatanum;
static  int     MAP_imgfilt[65535];

BOOL MAP_readMapConfFile( char* filename )
{
    FILE*   file;
    char    line[512];
    int     maximagenumber=0;
    int     i;
    int     linenum=0;
    int     imgdataindex=0;

    file = fopen( filename , "r" );
    if( file == NULL ){
        fprint( "无法打开文件 %s\n", filename );
        return FALSE;
    }
    while( fgets( line , sizeof( line ) , file ) ){
        char    imgnum[16];
        BOOL    ret;
        int     imgnumber;
        deleteSequentChar( line, " \t" );
        pohcd( line, " \t" );
        dchop(line, " \t");

        if( line[0] == '#' )continue;
        if( line[0] == '\n' )continue;
        chomp( line );
        ret = getStringFromIndexWithDelim( line , " " ,  1 , imgnum , sizeof( imgnum ) );
        if( ret == FALSE )continue;
        imgnumber = atoi(imgnum);
        maximagenumber = max( maximagenumber , imgnumber );
    }

    MAP_imagedatanum = maximagenumber + 1;
    MAP_imagedata=allocateMemory(sizeof(MAP_ImageData)*MAP_imagedatanum);
    if( MAP_imagedata == NULL ){
        fprint("无法分配地图数据大小=%d\n",MAP_imagedatanum);
        return FALSE;
    }
    
    for( i = 0; i <arraysizeof( MAP_imgfilt) ; i ++ )
        MAP_imgfilt[i] = -1;
    if( fseek( file, 0 , SEEK_SET ) != 0 ){
        fprint( "无法查找 SEEK_SET %s\n" , strerror( errno ));
        return FALSE;
    }
    while( fgets( line , sizeof( line ) , file ) ){
        char    token[64];
        int     imagenumber;
        int     ret;
        int     i;
        MAP_ImageData  *one;
        int     offset;
        linenum++;

        deleteSequentChar( line, " \t" );
        pohcd( line, " \t" );
        chomp(line);
        if( line[0] == '#' )continue;
        if( line[0] == '\n' )continue;
        chomp( line );

        ret = getStringFromIndexWithDelim(line, " " ,1, token , sizeof( token ) );
        if( ret == FALSE )continue;
        imagenumber = atoi( token );
        MAP_imgfilt[imagenumber] = imgdataindex;
        one = &MAP_imagedata[imgdataindex];
        imgdataindex ++;

        MAP_makeDefaultImageData(one);
        offset = 4 ;
        for( i = 0 ; i < MAP_DATAINT_NUM + MAP_DATACHAR_NUM ; i ++ ){
            BOOL    ret;
            ret = getStringFromIndexWithDelim(line, " " ,i + offset, token, sizeof(token) );
            if( ret == FALSE )continue;
            switch( MAP_confentries[i].type ){
            case MAP_INTENTRY:
                one->data[MAP_confentries[i].index]
                    = atoi( token );
                break;
            case MAP_CHARENTRY:
                strcpysafe( one->string[MAP_confentries[i].index].string,
                            sizeof( one->string[MAP_confentries[i].index].
                                    string),
                            token);
                break;
            case MAP_INTFUNC:
            {
                int (*intfunction)(char* );
                intfunction = MAP_confentries[i].func;
                one->data[MAP_confentries[i].index]
                    = intfunction(token);
                break;
            }
            case MAP_CHARFUNC:
            {
                char* (*charfunction)(char* );
                charfunction = MAP_confentries[i].func;

                strcpysafe( one->string[MAP_confentries[i].index].string,
                            sizeof(one->string[MAP_confentries[i].index].
                                   string),
                            charfunction(token));
                break;
            }
            default:
                break;
            }
        }
    }

    fclose( file );
    return TRUE;
}

#if 1
BOOL MAP_readBattleMapConfFile( char* filename )
{
    FILE*   file;
    char    line[512];

    int     i, j;
    int     linenum=0;
	int 	BattleMapNo[3], iRet, iWork, iPre[3],iFirst, iLast;

    file = fopen( filename , "r" );
    if( file == NULL ){
        fprint( "无法打开 %s\n", filename );
        return FALSE;
    }

	BattleMapNo[0] = 0;
	BattleMapNo[1] = 0;
	BattleMapNo[2] = 0;
    while( fgets( line , sizeof( line ) , file ) ){
        char    *p;
        linenum++;

        pohcd( line, " \t" );
        dchop(line, " \t");

        if( line[0] == '#' )continue;
        if( line[0] == '\n' )continue;
        chomp( line );

		if( line[0] == '$' ){
			iRet = sscanf( line+1, "%d %d %d", &iPre[0], &iPre[1], &iPre[2] );
			BattleMapNo[0] = BattleMapNo[1] = BattleMapNo[2] = iPre[0];
			if( iRet < 1 ){
				print( "!!!!!错误 无法读取战斗地图(%s)( line %d )\n", filename, linenum );
			}
			for( i = 0; i < iRet; i ++ ){
				BattleMapNo[i] = iPre[i];
				if( BattleMapNo[i] < 0 ){
					print( "!!!!!错误 战斗地图号码为负值 (%s)( line %d )\n", filename, linenum );
				}
			}

			continue;

		}
		if( sscanf( line, "%d", &iWork ) != 1 ){
			continue;
		}
		if( iWork < 0 ){
			print( "!!!!!错误 金钱号码为负值 (%s)( line %d )\n", filename, linenum );
			continue;
		}
		iFirst = iWork;
		p = strstr( line, "to" );
		if( p != NULL ){
			if( sscanf( p+2, "%d", &iWork ) != 1 ){
				iLast = iFirst;
			}else{
				iLast = iWork;
			}
			if( iWork < 0 ){
				print( "!!!!!错误 金钱号码为负值 (%s)( line %d )\n", filename, linenum );
				continue;
			}
		}else{
			iLast = iFirst;
		}

		for( i = iFirst; i <= iLast; i ++ ){
			if( MAP_getImageInt( i, MAP_SETED_BATTLEMAP ) > 0 ){
				print( "!!!!!错误 相同金钱重复设定了两次(%s)( line %d )(%d)(%d & %d)\n",
					filename, linenum, i,
					MAP_getImageInt( i, MAP_BATTLEMAP ),
					BattleMapNo[0] );
			}

			//   飓  寞恳仄中井＂
			if( IsValidImagenumber( i ) == FALSE ){
				continue;
			}
			MAP_setImageInt( i, MAP_BATTLEMAP,  BattleMapNo[0] );
			MAP_setImageInt( i, MAP_BATTLEMAP2, BattleMapNo[1] );
			MAP_setImageInt( i, MAP_BATTLEMAP3, BattleMapNo[2] );
			MAP_setImageInt( i, MAP_SETED_BATTLEMAP, 1 );
		}

    }
    fclose( file );

	for( j = 0; j < arraysizeof( MAP_imgfilt ); j ++ ){
		if( MAP_imgfilt[j] == -1 )continue;
		if( MAP_getImageInt( j, MAP_SETED_BATTLEMAP ) == 0 ){
			print( "!!!!!错误 金钱(%d)尚未设定 (%s)\n", j, filename );
		}
	}
    return TRUE;
}
#endif

BOOL IsValidImagenumber( int imagenumber )
{
    if( imagenumber < 0 || imagenumber >= arraysizeof( MAP_imgfilt ) )
        return FALSE;

    if( MAP_imgfilt[imagenumber] == -1 )
    	return FALSE;
    return TRUE;
}

int MAP_getImageInt( int imagenumber, int element )
{
    if( imagenumber < 0 || imagenumber >= arraysizeof( MAP_imgfilt ))
        return FALSE;

    if( MAP_imgfilt[imagenumber] == -1 )return FALSE;
    return MAP_imagedata[MAP_imgfilt[imagenumber]].data[element];
}

BOOL MAP_setImageInt( int imagenumber, int element, int value )
{
    if( imagenumber < 0 || imagenumber >= arraysizeof( MAP_imgfilt ))
        return FALSE;

    if( MAP_imgfilt[imagenumber] == -1 )return FALSE;
    MAP_imagedata[MAP_imgfilt[imagenumber]].data[element] = value;

    return TRUE;
}

static MAP_Map*         MAP_map;
static unsigned int     MAP_mapnum;
static unsigned int     MAP_mapnum_index;


static int*             MAP_idjumptbl;
static int              MAP_idtblsize;

BOOL MAP_initMapArray( int num )
{
    MAP_mapnum = num ;

    MAP_map = allocateMemory( sizeof( MAP_Map ) * MAP_mapnum );
    if( MAP_map == NULL )return FALSE;

    MAP_mapnum_index = 0;
    
    return TRUE;
}

void MAP_endMapArray( void )
{
    int i;
    for( i = 0 ; i < MAP_mapnum ; i ++ ){
        if( MAP_map[i].tile != NULL )
            freeMemory( MAP_map[i].tile );

        if( MAP_map[i].obj  != NULL)
            freeMemory( MAP_map[i].obj );

    }
    MAP_mapnum = 0;
    MAP_mapnum_index = 0;
    freeMemory( MAP_map );
    MAP_map = NULL;
}

static BOOL MAP_IsMapFile( char*    filename )
{
    FILE*   f;
    char    buf[16];
    int     ret;

    f  = fopen(filename, "r");
    if( f == NULL ){
        errorprint;
        return FALSE;
    }

    ret = fread( buf , sizeof( char ) , 6 , f);
    if( ret != 6 )
        goto FCLOSERETURNFALSE;
    buf[ret] = '\0';

    if( strcmp( buf , MAP_MAGIC ) != 0 )
        goto FCLOSERETURNFALSE;
    else
        goto FCLOSERETURNTRUE;

FCLOSERETURNFALSE:
    fclose(f);
    return FALSE;
FCLOSERETURNTRUE:
    fclose(f);
    return TRUE;
}

static BOOL MAP_readMapOne( char*   filename )
{
    FILE*   f;					/*  白央奶伙    */
    char    buf[16];            /*  穴斥永弁瓜件田□  心迕  */
    short   data[1024];         /*  扑亦□玄  心迕田永白央  */
    int     ret;                /*  忒曰袄熬仃潸曰迕        */
    int     i;                  /*  伙□皿  醒  */
    int     mapindex;

    int     id=0,xsiz=0,ysiz=0;     /*  穴永皿犯□正及域凛伞  桦赭  */

    short   *tile=NULL;
    short   *obj=NULL;
    MAP_Objlink**   olink=NULL;
    char    showstring[32];         /*  穴永皿犯□正及域凛伞  桦赭  */
    struct  stat    filestat;       /*  民尼永弁迕卞银丹    */
    BOOL    invaliddata=FALSE;
    if( MAP_mapnum_index >= MAP_mapnum ){
        fprint( "这里没有足够空间装载地图数组.\n" );
        return FALSE;
    }
    mapindex=MAP_mapnum_index;
    f  = fopen(filename, "r");
    if( f == NULL ){
        errorprint;
        return FALSE;
    }
    if( fstat( fileno(f) , &filestat ) != 0 ){
        errorprint;
        goto FCLOSERETURNFALSE;
    }

    ret = fread( buf , sizeof( char ) , 6 , f);
    if( ret != 6 ){
        errorprint;
        goto FCLOSERETURNFALSE;
    }
    buf[ret] = '\0';

    if( strcmp( buf , MAP_MAGIC ) != 0 )
        goto FCLOSERETURNFALSE;
    ret = fread( data , sizeof( short ) , 1 , f);
    if( ret != 1 ){
        errorprint;
        goto FCLOSERETURNFALSE;
    }
    id = ntohs( data[0] );

    ret = fread( showstring , sizeof(char), arraysizeof(showstring), f);
    if( ret != arraysizeof(showstring) ){
        errorprint;
        goto FCLOSERETURNFALSE;
    }
    showstring[arraysizeof(showstring)-1] = '\0';
    ret = fread( data, sizeof( short ) , 1 , f);
    if( ret != 1 ){
        errorprint;
        goto FCLOSERETURNFALSE;
    }
    xsiz = ntohs( data[0] );
    ret = fread( data, sizeof( short ) , 1 , f);
    if( ret != 1 ){
        errorprint;
        goto FCLOSERETURNFALSE;
    }
    ysiz = ntohs( data[0] );
    tile = allocateMemory( sizeof( short ) * xsiz * ysiz );
    if( tile == NULL ){
        fprint( "无法分配内存给地图名称:%s xsiz:%d ysiz:%d\n", filename, xsiz, ysiz);
        goto FREEOBJHP;
    }

    obj = allocateMemory( sizeof( short ) * xsiz * ysiz );
    if( obj == NULL ){
        fprint( "无法分配内存给对象\n");
        goto FREETILE;
    }

    olink = allocateMemory( sizeof(MAP_Objlink*)  * xsiz * ysiz );
    if( olink == NULL ){
        fprint( "无法分配内存给链接\n");
        goto FREEOBJ;
    }

    ret = fread( tile , sizeof( short )*xsiz*ysiz,1,f );
    if( ret != 1 ){
        errorprint;
        goto FREELINK;
    }
//    for( i  = 0 ; i < xsiz * ysiz ; i ++ )
//        tile[i] = ntohs( tile[i] );
    for( i = 0 ; i < xsiz * ysiz ; i ++ ){
    		tile[i] = ntohs( tile[i] );
        if( !IsValidImagenumber( tile[i] ) ){
            fprint("地图的图片有问题:%d x:%d y:%d 数量:%d\n",
                id, i % xsiz, (int)(i / xsiz) , tile[i]);
            invaliddata = TRUE;
        }
    }
    ret = fread( obj , sizeof( short )*xsiz*ysiz,1,f );
    if( ret != 1 ){
        errorprint;
        goto FREELINK;
    }
//    for( i  = 0 ; i < xsiz * ysiz ; i ++ )
//        obj[i] = ntohs( obj[i] );
    for( i = 0 ; i < xsiz * ysiz ; i ++ ){
    		obj[i] = ntohs( obj[i] );
        if( !IsValidImagenumber( obj[i] ) ){
            fprint(
                "地图的图片有问题:%d x:%d y:%d 数量:%d\n",
                id, i % xsiz, (int)(i / xsiz) , obj[i]);
            invaliddata = TRUE;
        }
    }
    if( invaliddata )goto FREELINK;
/*    if( ftell(f) != filestat.st_size)
        fprintf(stderr,"文件大小不正确(%s). 目标文件大小:%"
#ifdef linux
                "l"
#elif __FreeBSD__
                "ll"
#endif
                "d 实际大小:%ld\n",
                filename, filestat.st_size,ftell(f));
*/

    for( i=0 ; i< xsiz*ysiz ; i ++ )
        olink[i] = NULL;

    MAP_map[mapindex].id = id;
    MAP_map[mapindex].xsiz = xsiz;
    MAP_map[mapindex].ysiz = ysiz;
    strcpysafe( MAP_map[mapindex].string ,
                sizeof(MAP_map[mapindex].string),
                showstring );
    MAP_map[mapindex].tile = tile;
    MAP_map[mapindex].obj = obj;
    MAP_map[mapindex].olink = olink;
#ifdef _MAP_NOEXIT
	{
		FILE *fp;
		char mpexit[128];
		int floorID=0,exfloor=0,ex_X=0,ex_Y=0;
		int map_type=0;
		MAP_map[mapindex].startpoint = 0;
		fp = fopen( "./data/map/map_noexit.txt","r");
		if( fp != NULL ){
			while(	fgets(mpexit, 128, fp) != NULL )	{
				sscanf( mpexit,"%d %d %d %d %d", &floorID, &exfloor, &ex_X, &ex_Y, &map_type);
				if( strstr( mpexit, "#" ) != NULL)
					continue;
				if( floorID == id)	{
					MAP_map[mapindex].startpoint = (exfloor<<16)+(ex_X << 8)+(ex_Y<<0);
					MAP_map[mapindex].MapType = map_type;
				}
			}
			// Nuke 1204: Bug fix
			fclose(fp);
		}else	{
			print("\n **错误** 找不到 map_noexit.txt 文件!!!");
		}
	}
#endif
    goto FCLOSERETURNTRUE;

FCLOSERETURNTRUE:
    MAP_mapnum_index++;
    fclose(f);
    return TRUE;

FREELINK:
    freeMemory( olink );
FREEOBJ:
    freeMemory( obj );
FREETILE:
    freeMemory( tile );
FREEOBJHP:
FCLOSERETURNFALSE:
    fclose(f);
    return FALSE;
}
#ifdef _MAP_NOEXIT
unsigned int MAP_getExFloor_XY( int floor, int *map_type)
{
	int i;
	int exfloor;
	for( i=0; i<MAP_mapnum; i++)	{
		if( MAP_map[i].id == floor )	{
			if(	MAP_map[i].startpoint > 0 )	{
				exfloor = (MAP_map[i].startpoint>>16)&0xffffff;
				if( exfloor >= 0 )	{
					*map_type = MAP_map[i].MapType;
					return MAP_map[i].startpoint ;
				}
			}
		}
	}
	return 0;
}
BOOL CHECKFLOORID( id)
{
	int i;

	if( id < 0 )
		return FALSE;

	for( i=0; i<MAP_mapnum; i++)	{
		if( MAP_map[i].id == id )	{
			return TRUE;
		}
	}
	return FALSE;
}
#endif

#define MAX_MAP_FILES 1300 // 地图目录最大档案数
//#define MAX_MAP_FILES 2000 // 地图目录最大档案数

BOOL MAP_readMapDir( char*  dirname )
{
    int     mapfilenum=0;
    STRING64    filenames[MAX_MAP_FILES];
    int     filenum;
    int     i;


    filenum = rgetFileName( dirname, filenames, arraysizeof(filenames) );
    if( filenum == -1 ){
        fprint( "无法在目录下获取文件 %s \n" , dirname );
        return FALSE;
    }


    for( i = 0 ; i < filenum ; i ++ )
        if( MAP_IsMapFile( filenames[i].string ) )
            mapfilenum++;

    print( "找到 %d 地图\n" , mapfilenum );
    if( mapfilenum == 0 )return FALSE;
    if( !MAP_initMapArray( mapfilenum ) ){
        fprint( "开启地图数组错误\n" );
        return FALSE;
    }

    for( i = 0 ; i < filenum ; i ++ )
        if( MAP_IsMapFile( filenames[i].string ) ){
            MAP_readMapOne( filenames[i].string );
//            print(".");
        }
    print( "正确地图文件 %d...",MAP_mapnum_index );
    if( MAP_mapnum_index == 0 ){
        MAP_endMapArray();
        return FALSE;
    }

    MAP_mapnum = MAP_mapnum_index;
    {
        int maxfloorid=0;
        for( i=0; i<MAP_mapnum ; i++ ){
            if( maxfloorid <= MAP_map[i].id )
                maxfloorid = MAP_map[i].id;
        }
        MAP_idtblsize = maxfloorid+1;
        MAP_idjumptbl = allocateMemory( MAP_idtblsize* sizeof(int) );
        if( MAP_idjumptbl == 0 )
            return FALSE;
        for( i=0 ; i<MAP_idtblsize ; i++ )
            MAP_idjumptbl[i] = -1;
        for( i=0 ; i<MAP_mapnum ; i++ )
            MAP_idjumptbl[MAP_map[i].id]  = i;
    }

    return TRUE;

}

#define CHECKFLOORID(id)	if( MAP_mapnum<=(id) || (id)<0 )return (-1);

INLINE int MAP_getfloorIndex( int floorid )
{
    if( 0 <= floorid && floorid <= MAP_idtblsize  )
        return MAP_idjumptbl[floorid];

	print( "readmap.c:[%d] err floorid[%d]\n", __LINE__, floorid);
    return -1;
}

int MAP_getfloorX( int floorid )
{
    int index;
    index = MAP_getfloorIndex( floorid );
    if( index == -1 )return -1;
    return MAP_map[index].xsiz;
}

char* MAP_getfloorShowstring( int floorid )
{
    int     index;
    index = MAP_getfloorIndex( floorid );
    if( index == -1 )return NULL;
    return MAP_map[index].string;
}

int MAP_getfloorY( int floorid )
{
    int index;
    index = MAP_getfloorIndex( floorid );
    if( index == -1 )return -1;
    return MAP_map[index].ysiz;
}

BOOL MAP_checkCoordinates( int mapid, int x, int y )
{
    if( MAP_getfloorIndex( mapid ) == -1 ||
        x < 0 || x > MAP_getfloorX(mapid) ||
        y < 0 || y > MAP_getfloorY(mapid))
        return FALSE;

    return TRUE;
}

#define MAP_GETMAXSIZE  45
static char MAP_dataString[STRINGBUFSIZ*3];
static int MAP_workdatabuffer[MAP_GETMAXSIZE*MAP_GETMAXSIZE];
#define MAP_DATADELIMITER ","

char* MAP_getdataFromRECT( int floor, RECT* seekr, RECT* realr )
{
    int floorindex;
    int stringlength=0;
    char    tmpbuffer[128];
    int i,j;
    int     floorx;
    int databufferindex=0;
    RECT    scr;
    char    escapebuffer[128];

    floorindex = MAP_getfloorIndex( floor );
    if( floorindex == -1 ) {
    	print( "%s:%d:err\n", __FILE__, __LINE__);
    	return NULL;
	}
    if( seekr->width < 0 || seekr->height < 0 ||
        MAP_GETMAXSIZE < seekr->width || MAP_GETMAXSIZE < seekr->height ){
    	print( "%s:%d:err\n", __FILE__, __LINE__);
        return NULL;
	}
    snprintf( tmpbuffer,sizeof(tmpbuffer),
               "%s|" ,
               makeEscapeString( MAP_map[floorindex].string,
                                 escapebuffer,sizeof(escapebuffer)));

    strcpysafe( MAP_dataString, sizeof( MAP_dataString ), tmpbuffer );
    stringlength = strlen( tmpbuffer );

    floorx = MAP_map[floorindex].xsiz;
    scr.x = 0;
    scr.y = 0;
    scr.width = floorx;
    scr.height= MAP_map[floorindex].ysiz;

    if( clipRect( seekr,&scr,realr ) == FALSE ){
        realr->x=realr->y=realr->width=realr->height=0;
        return MAP_dataString;
    }
    for( i = realr->y ; i < realr->y + realr->height ; i ++ ){
        for( j = realr->x ; j < realr->x + realr->width ; j ++ ){
            MAP_workdatabuffer[databufferindex++]
                = MAP_map[floorindex].tile[i*floorx+j];
        }
    }
    for( i = 0 ; i < databufferindex ; i ++ ){
		char	buf[64];
        snprintf( tmpbuffer,sizeof(tmpbuffer),"%s" MAP_DATADELIMITER ,
                  cnv10to62( MAP_workdatabuffer[i], buf, sizeof( buf)) );
        strcpysafe( &MAP_dataString[stringlength] ,
                    sizeof( MAP_dataString ) - stringlength,
                    tmpbuffer );
        stringlength += strlen( tmpbuffer );
    }
    if( stringlength < sizeof( MAP_dataString ) )
        MAP_dataString[stringlength-1] = NONCHAR_DELIMITER[0];
    databufferindex=0;
    for( i = realr->y ; i < realr->y + realr->height ; i ++ ){
        for( j = realr->x ; j < realr->x + realr->width ; j ++ ){
            MAP_workdatabuffer[databufferindex++]
                = MAP_map[floorindex].obj[i*floorx+j];
        }
    }
    for( i = 0 ; i < databufferindex ; i ++ ){
		char	buf[64];
		if(  MAP_workdatabuffer[i] == 0 ) buf[0] = '\0';
		else cnv10to62( MAP_workdatabuffer[i], buf, sizeof( buf));
        snprintf( tmpbuffer,sizeof(tmpbuffer),"%s" MAP_DATADELIMITER ,
                   buf );
        strcpysafe( &MAP_dataString[stringlength] ,
                    sizeof( MAP_dataString ) - stringlength,
                    tmpbuffer );
        stringlength += strlen( tmpbuffer );
    }

    if( stringlength < sizeof( MAP_dataString ) )
        MAP_dataString[stringlength-1] = NONCHAR_DELIMITER[0];

    databufferindex=0;
    for( i = realr->y ; i < realr->y + realr->height ; i ++ ){
        for( j = realr->x ; j < realr->x + realr->width ; j ++ ){
			OBJECT	object;
			BOOL	found = FALSE;
            for( object = MAP_getTopObj(floor,j,i) ; object ;
                 object = NEXT_OBJECT(object) ){
                int o = GET_OBJINDEX(object);
                if( OBJECT_getType(o) == OBJTYPE_CHARA ){
	                int	etype;
	                int charaindex=OBJECT_getIndex(o);
	                if( !CHAR_CHECKINDEX(charaindex) )continue;
					etype = CHAR_getWorkInt( charaindex, CHAR_WORKEVENTTYPE);
					if( etype != CHAR_EVENT_NONE ) {
			            MAP_workdatabuffer[databufferindex++] = etype;
						found = TRUE;
						break;
					}
				}
#ifdef _MAP_WARPPOINT
				else if( OBJECT_getType(o) == OBJTYPE_WARPPOINT ){
					int	etype = OBJECT_getchartype( o);
					if( etype != CHAR_EVENT_NONE ) {
						MAP_workdatabuffer[databufferindex++] = (unsigned short)etype;
						found = TRUE;
						break;
					}
				}
#endif
			}
			if( !found ) {
	            MAP_workdatabuffer[databufferindex++]
	            	= CHAR_EVENT_NONE;
			}
        }
    }
    for( i = 0 ; i < databufferindex ; i ++ ){
		char	buf[64];
		if(  MAP_workdatabuffer[i] == 0 ) buf[0] = '\0';
		else cnv10to62( MAP_workdatabuffer[i], buf, sizeof( buf));

        snprintf( tmpbuffer,sizeof(tmpbuffer),"%s" MAP_DATADELIMITER, buf);
        strcpysafe( &MAP_dataString[stringlength] ,
                    sizeof( MAP_dataString ) - stringlength, tmpbuffer );
        stringlength += strlen( tmpbuffer );
    }
    dchop( MAP_dataString, MAP_DATADELIMITER );
    return MAP_dataString;
}

char *MAP_getChecksumFromRECT( int floor, RECT* seekr, RECT* realr,
								int *tilesum, int *objsum, int *eventsum )
{
    int floorindex;
    int i,j;
    int     floorx;
    int databufferindex=0;
    RECT    scr;
    char    escapebuffer[128];
	unsigned short	eventbuf[MAP_CHAR_DEFAULTSEESIZ * MAP_CHAR_DEFAULTSEESIZ];
	unsigned short	tilebuf[ MAP_CHAR_DEFAULTSEESIZ * MAP_CHAR_DEFAULTSEESIZ];
	unsigned short	objbuf[  MAP_CHAR_DEFAULTSEESIZ * MAP_CHAR_DEFAULTSEESIZ];

    floorindex = MAP_getfloorIndex( floor );
    if( floorindex == -1 ) return NULL;
    if( seekr->width < 0 || seekr->height < 0 ||
        MAP_GETMAXSIZE < seekr->width || MAP_GETMAXSIZE < seekr->height )
        return NULL;

    strcpysafe( MAP_dataString, sizeof( MAP_dataString ),
               makeEscapeString( MAP_map[floorindex].string,
                                 escapebuffer,sizeof(escapebuffer)));

    floorx = MAP_map[floorindex].xsiz;
    scr.x = 0;
    scr.y = 0;
    scr.width = floorx;
    scr.height= MAP_map[floorindex].ysiz;

    if( clipRect( seekr,&scr,realr ) == FALSE ){
        realr->x=realr->y=realr->width=realr->height=0;
        return MAP_dataString;
    }

    *tilesum = 0;
    *objsum =0;
    *eventsum = 0;
	memset( tilebuf, 0, sizeof( tilebuf));
	memset( objbuf, 0, sizeof( objbuf));
	memset( eventbuf, 0, sizeof( eventbuf));
    for( i = realr->y ; i < realr->y + realr->height ; i ++ ){
        for( j = realr->x ; j < realr->x + realr->width ; j ++ ){
            OBJECT  object;
            //BOOL found = FALSE;
            for( object = MAP_getTopObj(floor,j,i) ; object ;
                 object = NEXT_OBJECT(object) ){
                int o = GET_OBJINDEX(object);
	            if( OBJECT_getType(o) == OBJTYPE_CHARA ){
	                int	etype;
	                int charaindex=OBJECT_getIndex(o);
	                if( !CHAR_CHECKINDEX(charaindex) )continue;
					etype = CHAR_getWorkInt( charaindex, CHAR_WORKEVENTTYPE);
					if( etype != CHAR_EVENT_NONE ) {
						eventbuf[databufferindex] = (unsigned short)etype;
						break;
					}
				}
#ifdef _MAP_WARPPOINT
				else if( OBJECT_getType(o) == OBJTYPE_WARPPOINT ){
					int	etype = OBJECT_getchartype( o);
					if( etype != CHAR_EVENT_NONE ) {
						eventbuf[databufferindex] = (unsigned short)etype;
						break;
					}
				}
#endif
			}
			tilebuf[databufferindex] = (unsigned short)MAP_map[floorindex].tile[i*floorx+j];
			objbuf[databufferindex]  = (unsigned short)MAP_map[floorindex].obj[i*floorx+j];
        	databufferindex++;
        }
    }
	*tilesum = CheckCRC( (unsigned char*)tilebuf, sizeof( tilebuf));
	*objsum  = CheckCRC( (unsigned char*)objbuf, sizeof( objbuf));
	*eventsum = CheckCRC( (unsigned char*)eventbuf, sizeof( eventbuf));

    return MAP_dataString;
}

BOOL MAP_getTileAndObjData( int ff ,int fx, int fy, int* tile, int* obj)
{
    int     floorindex,xsiz;
    floorindex = MAP_getfloorIndex( ff );
    if( floorindex == -1 ) {
    	print( "%s:%d:err\n", __FILE__, __LINE__);
    	return FALSE;
	}
    xsiz = MAP_map[floorindex].xsiz;

    if( fx < 0 || fx >= xsiz ) {
        return FALSE;
	}
    if( fy < 0 || fy >= MAP_map[floorindex].ysiz ) {
        return FALSE;
	}

    if( tile )
        *tile = MAP_map[floorindex].tile[fy*xsiz+fx];
    if( obj )
        *obj  = MAP_map[floorindex].obj[fy*xsiz+fx];

    return TRUE;
}

BOOL MAP_setTileAndObjData( int ff ,int fx, int fy, int tile, int obj)
{
    int     floorindex,xsiz;
    floorindex = MAP_getfloorIndex( ff );
    if( floorindex == -1 ) {
    	print( "%s:%d:err\n", __FILE__, __LINE__);
    	return FALSE;
	}
    xsiz = MAP_map[floorindex].xsiz;
    if( fx < 0 || fx >= xsiz ) {
        return FALSE;
	}
    if( fy < 0 || fy >= MAP_map[floorindex].ysiz ) {
        return FALSE;
	}
    if( tile != -1 )
    	MAP_map[floorindex].tile[fy*xsiz+fx] = tile;
    if( obj != -1)
    	MAP_map[floorindex].obj[fy*xsiz+fx] = obj;
    return TRUE;
}

BOOL MAP_initReadMap( char* maptilefile , char* mapdir )
{
    print("\n");
    if( !MAP_readMapConfFile(maptilefile) )
        return FALSE;
    if( !MAP_readBattleMapConfFile( getBattleMapfile( ) ) )
        return FALSE;
    if( !MAP_readMapDir(mapdir) )
        return FALSE;
    return TRUE;
}

static int MAP_coKindAndInt[MAP_KINDNUM]=
{
    MAP_WALKABLE,
};

BOOL MAP_makeVariousMap(char* atile, char* aobj, int floor, int startx,
                        int starty, int xsiz, int ysiz, MAP_kind   kind )
{
    int     i,j;
    int     findex;
    int     fxsiz;
    int     startpoint;

    if( atile == NULL || aobj == NULL || kind < 0 || kind >= MAP_KINDNUM ) {
    	print( "%s:%d:err\n", __FILE__, __LINE__);
        return FALSE;
	}
    findex = MAP_getfloorIndex( floor );
    if( findex == -1 ) {
    	print( "%s:%d:err\n", __FILE__, __LINE__);
    	return FALSE;
	}
    fxsiz = MAP_map[findex].xsiz;
    startpoint = startx + starty * fxsiz ;

    for( i = 0 ; i < ysiz ; i ++ ){
        for( j = 0 ; j < xsiz ; j ++ ){
            atile[i*xsiz+j] = MAP_getImageInt( MAP_map[findex].tile[
                startpoint + i*xsiz + j],MAP_coKindAndInt[kind] )
                ? 1 : 0;

            aobj[i*xsiz+j] = MAP_getImageInt( MAP_map[findex].obj[
                startpoint + i*xsiz + j],MAP_coKindAndInt[kind] )
                ? 1 : 0;
        }
    }
    return TRUE;
}

BOOL MAP_makeWalkableMap( char* data,  int floor, int startx, int starty,
                          int xsiz, int ysiz )
{
    int     i,j;
    char*   obj;
    obj = allocateMemory(xsiz*ysiz*sizeof(char));
    if( obj == NULL )return FALSE;

    if( MAP_makeVariousMap( data, obj, floor, startx, starty, xsiz, ysiz,
                            MAP_KINDWALKABLE ) == FALSE ){
        freeMemory( obj );
        return FALSE;
    }

    for( i = 0 ; i < ysiz ; i ++ )
        for( j = 0 ; j < xsiz ; j ++ )
            data[j+i*ysiz] = data[j+i*ysiz] & obj[j+i*ysiz];

#ifdef DEBUG
    for( i = 0 ; i < ysiz ; i ++ ){
        for( j = 0 ; j < xsiz ; j ++ )
            if( data[j+i*ysiz] ){
                print(" ");
            }else
                print("O");
        print("\n");
    }
#endif  /*DEBUG*/

    return TRUE;
}

BOOL MAP_IsThereSpecificFloorid( int floorid )
{
    if( MAP_getfloorIndex(floorid)== -1)return FALSE;
    else                                return TRUE;
}

BOOL MAP_IsValidCoordinate( int floor, int x, int y )
{
    if( MAP_IsThereSpecificFloorid(floor)==FALSE )return FALSE;
    if( x < 0 || MAP_getfloorX(floor)<=x )return FALSE;
    if( y < 0 || MAP_getfloorY(floor)<=y )return FALSE;
    return TRUE;
}

int MAP_attackSpecificPoint( int floor, int x, int y, int damage ,
                             int charaindex )
{
#if 0
    int     mapindex;
    int     xsiz;
    int     objimagenumber;

    mapindex = MAP_getfloorIndex(floor);
    if( mapindex == -1 ) {
    	print( "%s:%d:err\n", __FILE__, __LINE__);
    	return 5;
	}
    xsiz = MAP_map[mapindex].xsiz;
    if( 0 > x || x >= xsiz || 0 >y || y >= MAP_map[mapindex].ysiz ) {
    	print( "%s:%d:err\n", __FILE__, __LINE__);
        return 5;
	}
    if( damage <= 0 )return 5;

    objimagenumber = MAP_map[mapindex].obj[x+y*xsiz];

    if( MAP_getImageInt( objimagenumber,MAP_DEFENCE ) > 0 ){
        MAP_map[mapindex].objhp[x+y*xsiz] -= damage;
        if( MAP_map[mapindex].objhp[x+y*xsiz] < 0 ){
            MAP_map[mapindex].obj[x+y*xsiz] = 0;
#if 1
            {
                char *stringdata;
                RECT seekr , retr;
                seekr.x = x;
                seekr.y = y;
                seekr.width = 1;
                seekr.height = 1;
                stringdata = MAP_getdataFromRECT(floor,&seekr,&retr);
                if( stringdata ){
                    print("RINGO: RETR: %d %d %d %d\n",
                          retr.x,retr.y,retr.width,retr.height);
                    if( CHAR_getInt( charaindex , CHAR_WHICHTYPE )
                        == CHAR_TYPEPLAYER ){
                        int fd = getfdFromCharaIndex( charaindex );
                        lssproto_M_send(fd,floor, retr.x, retr.y,
                                        retr.x + retr.width, retr.y + retr.height,
                                        stringdata );
                    }
                } else {
                    print("RINGO: bad stringdata. %d %d %d %d\n",
                          seekr.x,seekr.y,seekr.width,seekr.height);

                }

            }
#endif
            return 1;
        }
        return 2;
    }

    if( MAP_getImageInt(MAP_map[mapindex].obj[y*xsiz+x],MAP_HAVEHEIGHT )
        == 0 )
        return 4;
#endif
    return 3;
}
BOOL MAP_appendTailObj( int floor, int x, int y, int objindex )
{
    int     mapindex;
    int     xsiz;
    int     dataindex;
    MAP_Objlink*    c;
    MAP_Objlink*    top;

    mapindex = MAP_getfloorIndex( floor );
    if( mapindex == -1 ) {
    	//print( "%s:%d:err(fl=%d,x=%d,y=%d)\n", __FILE__, __LINE__, floor, x, y );
    	return FALSE;
	}
    xsiz = MAP_map[mapindex].xsiz;
    if( 0 > x || x >= xsiz || 0 >y || y >= MAP_map[mapindex].ysiz ) {
    	//print( "%s:%d:err(fl=%d,x=%d,y=%d)\n", __FILE__, __LINE__, floor, x, y);
        return FALSE;
	}
    dataindex = y*xsiz+x;
    top = MAP_map[mapindex].olink[dataindex];
    if( top == NULL ){
        top = allocateMemory( sizeof(MAP_Objlink) );
        if( top == NULL ) {
    		//print( "%s:%d:err(fl=%d,x=%d,y=%d)\n", __FILE__, __LINE__, floor, x, y);
        	return FALSE;
        }
        top->next      = NULL;
        top->objindex  = objindex;
        MAP_map[mapindex].olink[dataindex] = top;
        return TRUE;
    }
    for( c=top ; c->next ; c = c->next );
    c->next = allocateMemory(sizeof(MAP_Objlink));
    if( c->next == NULL )return FALSE;
    c->next->next = NULL;
    c->next->objindex = objindex;
    return TRUE;
}

BOOL MAP_removeObj( int floor, int x, int y, int objindex )
{
    int     mapindex;
    int     xsiz;
    int     dataindex;
    MAP_Objlink*    c;
    MAP_Objlink*    last=NULL;

    mapindex = MAP_getfloorIndex( floor );
    if( mapindex == -1 ) {
    	print( "%s:%d:错误\n", __FILE__, __LINE__);
    	return FALSE;
	}
    xsiz = MAP_map[mapindex].xsiz;
    if( 0 > x || x >= xsiz || 0 >y || y >= MAP_map[mapindex].ysiz ) {
    	print( "%s:%d:错误 地图号[%d] x:[%d] y:[%d]\n", __FILE__, __LINE__, floor, x, y);
        return FALSE;
	}
    dataindex = y*xsiz+x;
    c = MAP_map[mapindex].olink[dataindex];
    while( c ){
        if( c->objindex == objindex ){
            if( last )
                last->next = c->next;
            else
                MAP_map[mapindex].olink[dataindex] = c->next;
            freeMemory( c );
            return TRUE;
        }
        last = c;
        c = c->next;
    }
    print( "%s:%d:错误\n", __FILE__, __LINE__);
    return FALSE;
}

BOOL _MAP_objmove( char *file, int line, int objindex, int ofloor, int ox, int oy, int nfloor,
                  int nx, int ny )
{
#if 1

    int     oldmapindex;
    int     oldxsiz;

    int     dataindex;
    MAP_Objlink*    c;
    MAP_Objlink*    last=NULL;
    MAP_Objlink*    pointer=NULL;

    oldmapindex = MAP_getfloorIndex( ofloor );
    if( oldmapindex == -1 ) {
    	print( "%s:%d:错误 地图号[%d] x:[%d] y:[%d]\n", __FILE__, __LINE__,
		    				 ofloor, ox, oy);
    	return FALSE;
	}
    oldxsiz = MAP_map[oldmapindex].xsiz;
    if( 0 > ox || ox >= oldxsiz
        || 0 >oy || oy >= MAP_map[oldmapindex].ysiz )
	{
    	print( "%s:%d:错误 地图号[%d] x:[%d] y:[%d]\n", __FILE__, __LINE__,
		    				 ofloor, ox, oy);
        return FALSE;
	}
    dataindex = oy*oldxsiz+ox;
    c = MAP_map[oldmapindex].olink[dataindex];
    while( c ){
        if( c->objindex == objindex ){
            if( last )
                last->next = c->next;
            else
                MAP_map[oldmapindex].olink[dataindex] = c->next;

            pointer = c;
            pointer->next = NULL;
            break;
        }
        last = c;
        c = c->next;
    }
    if( !pointer ) {
    	print( "\n%s:%d:错误( %d,%d,%d )->(%d,%d,%d)\n", __FILE__, __LINE__,
			ofloor, ox, oy, nfloor, nx, ny );
    	return FALSE;
	}
    {
        int     newmapindex;
        int     newxsiz;
        MAP_Objlink*    top;

        if( ofloor == nfloor ){
            newmapindex = oldmapindex;
            newxsiz     = oldxsiz;
        }else{
            newmapindex = MAP_getfloorIndex( nfloor );
            if( newmapindex == -1 ) {
		    	print( "%s:%d:错误 地图号[%d] x:[%d] y:[%d]\n", __FILE__, __LINE__,
		    				 nfloor, nx, ny);
            	return FALSE;
			}
            newxsiz = MAP_map[newmapindex].xsiz;
        }
        if( 0 > nx || nx >= newxsiz
            || 0 >ny || ny >= MAP_map[newmapindex].ysiz )
		{
	    	print( "%s:%d:错误 地图号[%d] x:[%d] y:[%d]\n", __FILE__, __LINE__,
	    				 nfloor, nx, ny);
            return FALSE;
		}

        dataindex = ny*newxsiz+nx;
        top = MAP_map[newmapindex].olink[dataindex];
        if( top == NULL ){
            top = MAP_map[newmapindex].olink[dataindex] = pointer;
            top->next = NULL;
            return TRUE;
        }

        for( c=top ; c->next ; c = c->next );
        c->next = pointer;
        c->next->next = NULL;
        return TRUE;
    }
#else
    if( MAP_removeObj( ofloor,ox,oy,objindex)
        && MAP_appendTailObj( nfloor, nx ,ny, objindex ) )
        return TRUE;
#endif
    print( "%s:%d:错误\n", __FILE__, __LINE__);
    return FALSE;
}

MAP_Objlink* _MAP_getTopObj( char *file, int line,int floor, int x, int y )
{
    int     mapindex;
    int     xsiz;

    mapindex = MAP_getfloorIndex( floor );
    if( mapindex == -1 ) {
    	//print( "callfrom[%s:%d] %s:%d:err floor(%d,%d,%d)\n", file, line, __FILE__, __LINE__,
    	//	floor, x, y );
    	return NULL;
	}

    xsiz = MAP_map[mapindex].xsiz;

#if 1
	if(  x >= xsiz) return NULL;
	if( y >= MAP_map[mapindex].ysiz ) return NULL;
	if( x < 0 ) return NULL;
	if( y < 0 ) return NULL;
    return MAP_map[mapindex].olink[y*xsiz+x];
#else
    if( 0 <= x && x < xsiz && 0 <= y && y < MAP_map[mapindex].ysiz )
        return MAP_map[mapindex].olink[y*xsiz+x];
    else
        return NULL;
#endif
}

BOOL MAP_addNewObj( int floor, int x, int y, int objindex )
{
    OBJECT  map;
    BOOL    ret;

    for( map=MAP_getTopObj(floor,x,y) ; map ; map = NEXT_OBJECT(map)) {
        if( GET_OBJINDEX(map) == objindex ) {
	    	print( "%s:%d:err\n", __FILE__, __LINE__);
            return 0;
		}
	}
    ret = MAP_appendTailObj( floor, x, y, objindex );
    if( ret )return 1;
    else     return -1;
}

char *MAP_getFloorName( int floor)
{
    int     floorindex;
    floorindex = MAP_getfloorIndex( floor );
    if( floorindex == -1 ) {
    	print( "%s:%d:err\n", __FILE__, __LINE__);
    	return NULL;
    }
    return MAP_map[floorindex].string;
}

#ifdef _MAKE_MAP
int MAP_getFloorXY( int floor, int *x, int *y)
{
    int     floorindex;
    floorindex = MAP_getfloorIndex( floor );
    if( floorindex == -1 ) {
    	print( "%s:%d:err\n", __FILE__, __LINE__);
    	return FALSE;
    }
    *x=MAP_map[floorindex].xsiz;
    *y=MAP_map[floorindex].ysiz;
    return TRUE;
}
#endif

BOOL MAP_setObjData( int ff ,int fx, int fy, int obj, int objhp )
{
    int     floorindex,xsiz;

    floorindex = MAP_getfloorIndex( ff );
    if( floorindex == -1 ) {
    	print( "%s:%d:err\n", __FILE__, __LINE__);
    	return FALSE;
	}
    xsiz = MAP_map[floorindex].xsiz;
    if( fx < 0 || fx >= xsiz ) {
        return FALSE;
	}
    if( fy < 0 || fy >= MAP_map[floorindex].ysiz ) {
        return FALSE;
	}
    MAP_map[floorindex].obj[fy*xsiz+fx] = obj;
    return TRUE;
}

void MAP_sendAroundMapdata( int fl, int fromx, int fromy)
{
    char*   mapdata;
    RECT    seek,ret;
	int		i,j;
	seek.x = fromx;
	seek.y = fromy;
	seek.width = seek.height = 1;
    mapdata = MAP_getdataFromRECT(fl,&seek,&ret);
    if( mapdata != NULL ) {
		int x = fromx;
		int	y = fromy;

		for( i =x-MAP_CHAR_DEFAULTSEESIZ/2 ; i <= x+MAP_CHAR_DEFAULTSEESIZ/2 ; i++ ){
			for( j = y-MAP_CHAR_DEFAULTSEESIZ/2 ; j <= y+MAP_CHAR_DEFAULTSEESIZ/2 ;
				 j ++ ){
				OBJECT  object;
				for( object = MAP_getTopObj(fl,i,j) ;
					 object ;
					 object = NEXT_OBJECT(object ) ){
					int objindex = GET_OBJINDEX(object);
					if( OBJECT_getType(objindex) == OBJTYPE_NOUSE ) continue;
					if( OBJECT_getType(objindex) == OBJTYPE_CHARA ) {
						int index = OBJECT_getIndex( objindex);
						if( CHAR_getInt( index, CHAR_WHICHTYPE)	== CHAR_TYPEPLAYER)	{
							int fd = getfdFromCharaIndex( index );
							if( fd !=  -1 ) {
						        lssproto_M_send( fd, fl, ret.x,ret.y,
					                         ret.x+ret.width, ret.y+ret.height,
					                         mapdata );
							}
						}
					}
				}
			}
		}
	}else {
		print( "地图数据错误 %s:%d \n" , __FILE__, __LINE__);
	}
}


