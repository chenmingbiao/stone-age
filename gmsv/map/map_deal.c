#include "version.h"
#include "common.h"
#include "readmap.h"
#include "char.h"
#include "map_util.h"
#include "lssproto_serv.h"
#include "net.h"
#include "anim_tbl.h"

/*#define MAPEDITORTROUBLE*/
#define SPR_kmydamY CG_HIT_MARK_10 // anim_tbl.h ±åè¶  ½ñÄ¾ÔÂÒıÆ¥  ¼°¾Ş°×ÄáÛÍĞş

/*------------------------------------------------------------
 * ĞÚØêÔÂ¾®Éıµ¤¾®  Ó¬ÔÊÔÂ£ÛÆ½ÅÒ·Â»¥  ÊÏÆ¥ÖĞÔÂ¾®Éıµ¤¾®Â¦ĞÑ»¥Ø¤ÔÂ£Û
 * Â¦ĞÑ
 *  ff          int     floor
 *  fx          int     xÕç  
 *  fy          int     yÕç  
 *  isfly       int       ÊÏÆ¥ÖĞÔÂ¾®Éıµ¤¾®
 * ß¯Ô»°À
 *  ĞÚØêÔÂ      TRUE(1)
 *  ĞÚØêØ¦ÖĞ    FALSE(0)
 ------------------------------------------------------------*/
BOOL MAP_walkAbleFromPoint( int ff, int fx, int fy, BOOL isfly )
{
    int map[2];

    if( !MAP_getTileAndObjData( ff,fx,fy, &map[0], &map[1] ) ){
        return FALSE;
	}

    if( isfly ){
        int i;
        for( i = 0 ; i < 2 ; i ++ )
            if( MAP_getImageInt( map[i], MAP_HAVEHEIGHT ) == TRUE  ){
                return FALSE;
						}
        return TRUE;
    }else{
        switch( MAP_getImageInt( map[1], MAP_WALKABLE ) ){
        case 0:
            return FALSE;
            break;
        case 1:
            if( MAP_getImageInt( map[0], MAP_WALKABLE ) == 1 ){
                return TRUE;
			}else{
                return FALSE;
			}
            break;
        case 2:
            return TRUE;
            break;
        default:
            return FALSE;
            break;
        }
    }
    return FALSE;
}



/*------------------------------------------------------------
 * ĞÚØêÔÂ¾®Éıµ¤¾®  Ó¬ÔÊÔÂ£Û
 * Â¦ĞÑ
 *  index       int     Char    Æ¥¼° Index
 *  ff          int     floor id
 *  fx          int     xÕç  
 *  fy          int     yÕç  
 * ß¯Ô»°À
 *  ĞÚÖĞ»¯·½ÖĞ      TRUE(1)
 *  ĞÚÖĞ»¯·´Ä¸¶ª    FALSE(0)
 ------------------------------------------------------------*/
BOOL MAP_walkAble( int index,int ff, int fx, int fy)
{
    /*      Æ½ÅÒ·ÂØ¦ÈÕOK    */
    if( CHAR_getFlg( index, CHAR_ISTRANSPARENT ) )      return TRUE;

    // Arminius 7.9 Airplane ok
    if( CHAR_getInt( index, CHAR_WHICHTYPE ) == CHAR_TYPEBUS) return TRUE;
    if( CHAR_getWorkInt( index, CHAR_WORKPARTYMODE) == CHAR_PARTY_CLIENT ) {
      int oyaindex = CHAR_getWorkInt( index, CHAR_WORKPARTYINDEX1);
      if( CHAR_CHECKINDEX( oyaindex)) {
        if (CHAR_getInt( oyaindex, CHAR_WHICHTYPE ) == CHAR_TYPEBUS) return TRUE;
      }
    }

    return MAP_walkAbleFromPoint( ff,fx,fy,CHAR_getFlg(index,CHAR_ISFLYING)|CHAR_getWorkInt(index,CHAR_WORKSKYWALKER) );
}

/*------------------------------------------------------------
 * Õç  Ã«Ï¶ÀÃØÆ»¯£ı¹«³ğ¼°×óÆ¤³âÄáÛÍĞş±åæÎ½ñ»¥Ø¤ÔÂ¾®Éıµ¤¾®
 * Â¦ĞÑ
 *  fl              int     °×·òÊ§
 *  x               int     x Õç  
 *  y               int     y Õç  
 * ß¯Ô»°À
 *  æÎ½ñ»¥Ø¤ÔÂ  TRUE
 *  æÎ½ñ»¥Ø¦ÖĞ  FALSE
 ------------------------------------------------------------*/
BOOL MAP_haveHeight( int fl, int x, int y )
{
    int     map[2];

    /*  Ñ¨ÓÀÃó·¸¡õÕıÃ«  »¯ÈÊÔÂ  */
    if( !MAP_getTileAndObjData( fl,x,y, &map[0], &map[1] ) )
        return FALSE;

    return MAP_getImageInt( map[1], MAP_HAVEHEIGHT );
}

/*------------------------------------------------------------
 * Map ÄÌÃ¬¼şĞş¼° pre postÃ«ÖÊ  ÔÊÔÂ
 * Â¦ĞÑ
 *  index       int     ê¼Ç·µ¤ÎçØÆ»¯ÖĞÔÂÆ½ÅÒ·Â¼°ÄÌ¼ş·¸ÓÀÛÍµ©
 *  mode        BOOL    TRUE¼°Áİ·´£ıin  FALSE¼°Áİ·´ out ±å¸²É±
 * ß¯Ô»°ÀØ¦ØÆ
 ------------------------------------------------------------*/
static void MAP_dealprepostevent( int index, BOOL mode )
{
    int     map[2];
    int     i;
    int     damaged=FALSE;

    if( CHAR_getFlg(index,CHAR_ISFLYING) )
        /*    ÊÏÆ¥ÔÂÖ§¹´±å·´ÖÏÊÖØÆØ¦ÖĞ  */
        return;

    if( !MAP_getMapDataFromCharIndex( index  ,  map ) ) return;

    for( i = 0 ; i < 2 ; i ++ ){
        /*  damage */
        int damage;
        damage = MAP_getImageInt(map[i],
                                 mode ? MAP_INTODAMAGE : MAP_OUTOFDAMAGE);
        if( damage != 0 ){
            /*  Ä¸¶ª¡õ³âĞÚÃ«¿ÒÆ¥  òØÔÊÔÂ¼°Æ¥  Ä¯  Ä¾×¸Òü    */
            int opt[2]={SPR_kmydamY,-damage};
            damaged=TRUE;
            CHAR_setInt(index, CHAR_HP,
                        CHAR_getInt(index,CHAR_HP) + damage );
            CHAR_complianceParameter(index);
            /*  Ä¸¶ª¡õ³â°¾ØêĞ×¾Ş°×ÄáÛÍĞşÇëÔÊ    */
            CHAR_sendWatchEvent(CHAR_getWorkInt(index,CHAR_WORKOBJINDEX),
                                CHAR_ACTDAMAGE,opt,2,TRUE);
        }
    }
    if( damaged )
    	CHAR_sendStatusString(index,"M");
}


/*----------------------------------------
 * ¹«¼°ÕıÄÌ»ï±åê¼ÔÂó¡±åôÄÈÉÄ¾ÔÂ£Û³ğ³ğÆ¥¹«³ğ¾®ÈÕ×ªÈÊÎç£ıxxxÑÌ¼°
 * ÒÇÃ«  òØÔÊÔÂ£Û
 * Â¦ĞÑ
 *  index       int     ê¼Ç·µ¤ÎçØÆ»¯ÖĞÔÂÆ½ÅÒ·Â¼°ÄÌ¼ş·¸ÓÀÛÍµ©
 *  flooor      int     °×·òÊ§ID
 *  fx          int     xÕç  
 *  fy          int     yÕç  
 * ß¯Ô»°À
 *  Ø¦ØÆ
 ----------------------------------------*/
void MAP_preovered( int index )
{
    MAP_dealprepostevent( index, FALSE );
}

/*----------------------------------------
 * ¹«¼°ÕıÄÌ»ï±åê¼ÔÈĞ×  ±åôÄÈÉÄ¾ÔÂ£Û
 * Â¦ĞÑ
 *  index       int     ê¼Ç·µ¤ÎçØÆ»¯ÖĞÔÂÆ½ÅÒ·Â¼°ÄÌ¼ş·¸ÓÀÛÍµ©
 * ß¯Ô»°À
 *  Ø¦ØÆ
 ----------------------------------------*/
void MAP_postovered( int index )
{
    MAP_dealprepostevent( index, TRUE );
}


/*----------------------------------------
 * Æ½ÅÒ·Â¼°Á§Ô»¼°Ñ¨ÓÀÃóÃ«ÔÊÍÍ»¯ËªÔÂ
 * Â¦ĞÑ
 *  fd          int
 *  charaindex  int     Æ½ÅÒ·Â¼°ÄÌ¼ş·¸ÓÀÛÍµ©
 * ß¯Ô»°À
 *  ÔÀ      TRUE(1)
 *  ÁÃ      FALSE(0)
 ----------------------------------------*/
BOOL MAP_sendArroundCharNeedFD( int fd,int charaindex )
{
    char*   stringdata;
    int     x=CHAR_getInt(charaindex,CHAR_X);
    int     y=CHAR_getInt(charaindex,CHAR_Y);
    int     fl=CHAR_getInt(charaindex,CHAR_FLOOR);
    int     size=MAP_CHAR_DEFAULTSEESIZ;
    RECT    seekr,retr;
    seekr.x = x - (int)(size/2);
    seekr.y = y - (int)(size/2);
    seekr.width  = size;
    seekr.height = size;
#if 1
{
	int		tilesum, objsum, eventsum;
    stringdata = MAP_getChecksumFromRECT(fl,&seekr,&retr, &tilesum,&objsum,&eventsum);
    if( stringdata == NULL )
        return FALSE;

    lssproto_MC_send(fd,fl,
                    retr.x,              retr.y,
                    retr.x + retr.width, retr.y + retr.height,
                    tilesum,
                    objsum,
                    eventsum,
                    stringdata );
}
#else
    stringdata = MAP_getdataFromRECT(fl,&seekr,&retr);
    if( stringdata == NULL )
        return FALSE;

    lssproto_M_send(fd,fl,
                    retr.x,              retr.y,
                    retr.x + retr.width, retr.y + retr.height,
                    stringdata );
#endif
    return TRUE;
}


/*----------------------------------------
 * Ñ¨ÓÀÃó·¸¡õÕıÃ«  ÔÂ£Û
 * Â¦ĞÑ
 *  charaindex      int     Æ½ÅÒ·Â¼°ÄÌ¼ş·¸ÓÀÛÍµ©
 * ß¯Ô»°À
 *  ÔÀ      TRUE(1)
 *  ÁÃ      FALSE(0)
 ----------------------------------------*/
BOOL MAP_sendArroundChar(int charaindex)
{
    int fd;
    fd = getfdFromCharaIndex( charaindex );
    if( fd == -1 )return FALSE;

    return MAP_sendArroundCharNeedFD(fd, charaindex);
}
