#include "version.h"
#include <string.h>
#include <math.h>

#include "readmap.h"
#include "common.h"
#include "util.h"
#include "anim_tbl.h"
#include "battle.h"
#include "char.h"
#include "char_data.h"
#include "buf.h"
#include "item.h"
#include "log.h"
#include "pet.h"
#include "enemy.h"
#ifdef _PERSONAL_FAME	// Arminius: ¼Ò×å¸öÈËÉùÍû
#include "char_base.h"
#include "configfile.h"
#endif


#include "defaultPlayer.h"

// CoolFish: Family 2001/7/28
#include "family.h"
#include "saacproto_cli.h"

#ifdef	_PET_LIMITLEVEL
void CHAR_CheckPetDoLimitlevel( int petindex , int toindex, int level);	
#endif


typedef struct tagdefaultCharcterGet
{
    int             imagenumber;
    Char*           initchardata;
    LevelUpPattern* lvpattern;
    CHAR_ImageType  imgtype;
}defaultCharacterGet;

static defaultCharacterGet CHAR_defaultCharacterGet[]=
{
  /*  CHAR_DEFAULTPLAYER  */
//ºÒ¼äñÉ¨à
    {SPR_001em,&player,&lvplayer00,CHAR_IMAGETYPE_CHILDBOY},
    {SPR_002em,&player,&lvplayer00,CHAR_IMAGETYPE_CHILDBOY},
    {SPR_003em,&player,&lvplayer00,CHAR_IMAGETYPE_CHILDBOY},
    {SPR_004em,&player,&lvplayer00,CHAR_IMAGETYPE_CHILDBOY},

//¼Á  ¨à
    {SPR_011em,&player,&lvplayer00,CHAR_IMAGETYPE_BOY},
    {SPR_012em,&player,&lvplayer00,CHAR_IMAGETYPE_BOY},
    {SPR_013em,&player,&lvplayer00,CHAR_IMAGETYPE_BOY},
    {SPR_014em,&player,&lvplayer00,CHAR_IMAGETYPE_BOY},

//¼Á  ¨á
    {SPR_021em,&player,&lvplayer00,CHAR_IMAGETYPE_BOY},
    {SPR_022em,&player,&lvplayer00,CHAR_IMAGETYPE_BOY},
    {SPR_023em,&player,&lvplayer00,CHAR_IMAGETYPE_BOY},
    {SPR_024em,&player,&lvplayer00,CHAR_IMAGETYPE_BOY},

//¼Á  ¨â
    {SPR_031em,&player,&lvplayer00,CHAR_IMAGETYPE_BOY},
    {SPR_032em,&player,&lvplayer00,CHAR_IMAGETYPE_BOY},
    {SPR_033em,&player,&lvplayer00,CHAR_IMAGETYPE_BOY},
    {SPR_034em,&player,&lvplayer00,CHAR_IMAGETYPE_BOY},

//Á²  ¨à
    {SPR_041em,&player,&lvplayer00,CHAR_IMAGETYPE_MAN},
    {SPR_042em,&player,&lvplayer00,CHAR_IMAGETYPE_MAN},
    {SPR_043em,&player,&lvplayer00,CHAR_IMAGETYPE_MAN},
    {SPR_044em,&player,&lvplayer00,CHAR_IMAGETYPE_MAN},

//Á²  ¨á
    {SPR_051em,&player,&lvplayer00,CHAR_IMAGETYPE_MAN},
    {SPR_052em,&player,&lvplayer00,CHAR_IMAGETYPE_MAN},
    {SPR_053em,&player,&lvplayer00,CHAR_IMAGETYPE_MAN},
    {SPR_054em,&player,&lvplayer00,CHAR_IMAGETYPE_MAN},

//ºÒ¼ä»Ô¨à
    {SPR_061em,&player,&lvplayer00,CHAR_IMAGETYPE_CHILDGIRL},
    {SPR_062em,&player,&lvplayer00,CHAR_IMAGETYPE_CHILDGIRL},
    {SPR_063em,&player,&lvplayer00,CHAR_IMAGETYPE_CHILDGIRL},
    {SPR_064em,&player,&lvplayer00,CHAR_IMAGETYPE_CHILDGIRL},

//¼Á»Ô¨à
    {SPR_071em,&player,&lvplayer00,CHAR_IMAGETYPE_GIRL},
    {SPR_072em,&player,&lvplayer00,CHAR_IMAGETYPE_GIRL},
    {SPR_073em,&player,&lvplayer00,CHAR_IMAGETYPE_GIRL},
    {SPR_074em,&player,&lvplayer00,CHAR_IMAGETYPE_GIRL},

//¼Á»Ô¨á
    {SPR_081em,&player,&lvplayer00,CHAR_IMAGETYPE_GIRL},
    {SPR_082em,&player,&lvplayer00,CHAR_IMAGETYPE_GIRL},
    {SPR_083em,&player,&lvplayer00,CHAR_IMAGETYPE_GIRL},
    {SPR_084em,&player,&lvplayer00,CHAR_IMAGETYPE_GIRL},

//¼Á»Ô¨â
    {SPR_091em,&player,&lvplayer00,CHAR_IMAGETYPE_GIRL},
    {SPR_092em,&player,&lvplayer00,CHAR_IMAGETYPE_GIRL},
    {SPR_093em,&player,&lvplayer00,CHAR_IMAGETYPE_GIRL},
    {SPR_094em,&player,&lvplayer00,CHAR_IMAGETYPE_GIRL},

//»ÔÁë¨à
    {SPR_101em,&player,&lvplayer00,CHAR_IMAGETYPE_WOMAN},
    {SPR_102em,&player,&lvplayer00,CHAR_IMAGETYPE_WOMAN},
    {SPR_103em,&player,&lvplayer00,CHAR_IMAGETYPE_WOMAN},
    {SPR_104em,&player,&lvplayer00,CHAR_IMAGETYPE_WOMAN},

//»ÔÁë¨á
    {SPR_111em,&player,&lvplayer00,CHAR_IMAGETYPE_WOMAN},
    {SPR_112em,&player,&lvplayer00,CHAR_IMAGETYPE_WOMAN},
    {SPR_113em,&player,&lvplayer00,CHAR_IMAGETYPE_WOMAN},
    {SPR_114em,&player,&lvplayer00,CHAR_IMAGETYPE_WOMAN},

    {SPR_pet001,&player,&lvplayer00,CHAR_IMAGETYPE_DOG},
    {SPR_pet002,&player,&lvplayer00,CHAR_IMAGETYPE_DOG},
    {SPR_pet003,&player,&lvplayer00,CHAR_IMAGETYPE_DOG},
    {SPR_pet004,&player,&lvplayer00,CHAR_IMAGETYPE_DOG},
    {SPR_pet011,&player,&lvplayer00,CHAR_IMAGETYPE_DOG},
};

BOOL CHAR_getDefaultChar( Char*  nc  , int imagenumber )
{
	int     i, j;
	int     defcharaindex;

	Char*   defaultchar;
	defaultchar = CHAR_defaultCharacterGet[arraysizeof(CHAR_defaultCharacterGet) - 1].initchardata;

	memset( nc,0,sizeof(Char) );
	defcharaindex = 0;
    for( i = 0 ; i < arraysizeof( CHAR_defaultCharacterGet ) ; i  ++ ){
		 if( CHAR_defaultCharacterGet[i].imagenumber == imagenumber ){
            defaultchar = CHAR_defaultCharacterGet[i].initchardata;
            defcharaindex = i;
            break;
		 }
	}
	nc->data[CHAR_IMAGETYPE] = CHAR_defaultCharacterGet[defcharaindex].imgtype;
    nc->use = TRUE;

    for( j = 0 ; j < CHAR_DATAINTNUM ; j ++ )
        nc->data[j] = defaultchar->data[j];
    for( j = 0 ; j < arraysizeof( nc->flg ) ; j ++ )
        nc->flg[j] = defaultchar->flg[j];

    for( j = 0 ; j < CHAR_DATACHARNUM ; j ++ )
		nc->string[j].string[0] = '\0';
    for( j = 0 ; j < CHAR_MAXITEMHAVE ; j ++ )
        nc->indexOfExistItems[j] = -1;
    for( j = 0 ; j < CHAR_MAXPOOLITEMHAVE ; j ++ )
        nc->indexOfExistPoolItems[j] = -1;
    for( j = 0 ; j < CHAR_MAXPETSKILLHAVE ; j ++ )
        nc->unionTable.indexOfPetskill[j] = -1;
    for( j = 0 ; j < CHAR_MAXPOOLPETHAVE ; j ++ )
        nc->indexOfPoolPet[j] = -1;
    for( j = 0 ; j < CHAR_SKILLMAXHAVE ; j ++ )
        nc->haveSkill[j].use = FALSE;
    for( j = 0 ; j < CHAR_TITLEMAXHAVE ; j ++ )
        nc->indexOfHaveTitle[j] = -1;
    for( j=0 ; j<ADDRESSBOOK_MAX ;j++ )
        memset( &nc->addressBook[j],0,sizeof(nc->addressBook[j]) );
    for( j = 0 ; j < CHAR_WORKDATAINTNUM ; j ++ )
        nc->workint[j] = 0;
    for( j = 0 ; j < CHAR_WORKDATACHARNUM ; j ++ )
        nc->workchar[j].string[0] = '\0';
	nc->workint[CHAR_WORKFD] = -1;

    return TRUE;
}

static int CHAR_playerImageNumber[]=
{
  /*  CHAR_DEFAULTPLAYER  */
    SPR_001em,SPR_002em,SPR_003em,SPR_004em,SPR_011em,SPR_012em,
    SPR_013em,SPR_014em,SPR_021em,SPR_022em,SPR_023em,SPR_024em,
    SPR_031em,SPR_032em,SPR_033em,SPR_034em,SPR_041em,SPR_042em,
    SPR_043em,SPR_044em,SPR_051em,SPR_052em,SPR_053em,SPR_054em,
    SPR_061em,SPR_062em,SPR_063em,SPR_064em,SPR_071em,SPR_072em,
    SPR_073em,SPR_074em,SPR_081em,SPR_082em,SPR_083em,SPR_084em,
    SPR_091em,SPR_092em,SPR_093em,SPR_094em,SPR_101em,SPR_102em,
    SPR_103em,SPR_104em,SPR_111em,SPR_112em,SPR_113em,SPR_114em
};

BOOL CHAR_checkPlayerImageNumber( int imagenumber)
{
	int i;
	for( i = 0; i < arraysizeof( CHAR_playerImageNumber); i ++  ) {
		if( imagenumber == CHAR_playerImageNumber[i] ) break;
	}
	if( i == arraysizeof( CHAR_playerImageNumber)) return FALSE;
	else return TRUE;
}
/*------------------------------------------------------------
 * Ó¿  ì«  Ä¯¼°ÃñÄáÓÀÛÍ
 *
 * Ó¿  ì«  Ä¯·´¶¯Æ±¼°°×ÇÉ¡õÑ¨ÓÀÐþÆ¥è£ÒýÔÈ»¯ÖÐÔÂ
 * number = ÖÏÚÐ  ¾®
 * CG_CHR_MAKE_FACE + (number*100) + (  çÆ  number * 25) + (     Ä¯ * 5 ) + éø  Ä¯
 *
 ------------------------------------------------------------*/
BOOL CHAR_checkFaceImageNumber( int imagenumber, int faceimagenumber)
{
	int image = imagenumber - SPR_001em;
	int number;
	int color;
	number = (faceimagenumber - CG_CHR_MAKE_FACE)/ 100*20;
	color = ((faceimagenumber - CG_CHR_MAKE_FACE)% 100) / 25*5;
	if( image != number + color ) return FALSE;
	return TRUE;
}
// Æ½ÅÒ·Â  ì«  ¡õÆ¤»ï£Û¨à  Ø¤Ð×Ô»Á¥  ¨äÅË  
int CHAR_eqimagetbl[][5]={
    // ÔÊÆ¥       £û£û£û£û é÷áß       ¼ü         ÝÔ
    { SPR_001em,SPR_001ax,SPR_001cl,SPR_001sp,SPR_001bw },
    { SPR_002em,SPR_002ax,SPR_002cl,SPR_002sp,SPR_002bw },
    { SPR_003em,SPR_003ax,SPR_003cl,SPR_003sp,SPR_003bw },
    { SPR_004em,SPR_004ax,SPR_004cl,SPR_004sp,SPR_004bw },
    { SPR_011em,SPR_011ax,SPR_011cl,SPR_011sp,SPR_011bw },
    { SPR_012em,SPR_012ax,SPR_012cl,SPR_012sp,SPR_012bw },
    { SPR_013em,SPR_013ax,SPR_013cl,SPR_013sp,SPR_013bw },
    { SPR_014em,SPR_014ax,SPR_014cl,SPR_014sp,SPR_014bw },
    { SPR_021em,SPR_021ax,SPR_021cl,SPR_021sp,SPR_021bw },
    { SPR_022em,SPR_022ax,SPR_022cl,SPR_022sp,SPR_022bw },
    { SPR_023em,SPR_023ax,SPR_023cl,SPR_023sp,SPR_023bw },
    { SPR_024em,SPR_024ax,SPR_024cl,SPR_024sp,SPR_024bw },
    { SPR_021em,SPR_021ax,SPR_021cl,SPR_021sp,SPR_021bw },
    { SPR_031em,SPR_031ax,SPR_031cl,SPR_031sp,SPR_031bw },
    { SPR_032em,SPR_032ax,SPR_032cl,SPR_032sp,SPR_032bw },
    { SPR_033em,SPR_033ax,SPR_033cl,SPR_033sp,SPR_033bw },
    { SPR_034em,SPR_034ax,SPR_034cl,SPR_034sp,SPR_034bw },
    { SPR_041em,SPR_041ax,SPR_041cl,SPR_041sp,SPR_041bw },
    { SPR_042em,SPR_042ax,SPR_042cl,SPR_042sp,SPR_042bw },
    { SPR_043em,SPR_043ax,SPR_043cl,SPR_043sp,SPR_043bw },
    { SPR_044em,SPR_044ax,SPR_044cl,SPR_044sp,SPR_044bw },

    { SPR_051em,SPR_051ax,SPR_051cl,SPR_051sp,SPR_051bw },
    { SPR_052em,SPR_052ax,SPR_052cl,SPR_052sp,SPR_052bw },
    { SPR_053em,SPR_053ax,SPR_053cl,SPR_053sp,SPR_053bw },
    { SPR_054em,SPR_054ax,SPR_054cl,SPR_054sp,SPR_054bw },

    { SPR_061em,SPR_061ax,SPR_061cl,SPR_061sp,SPR_061bw },
    { SPR_062em,SPR_062ax,SPR_062cl,SPR_062sp,SPR_062bw },
    { SPR_063em,SPR_063ax,SPR_063cl,SPR_063sp,SPR_063bw },
    { SPR_064em,SPR_064ax,SPR_064cl,SPR_064sp,SPR_064bw },

    { SPR_071em,SPR_071ax,SPR_071cl,SPR_071sp,SPR_071bw },
    { SPR_072em,SPR_072ax,SPR_072cl,SPR_072sp,SPR_072bw },
    { SPR_073em,SPR_073ax,SPR_073cl,SPR_073sp,SPR_073bw },
    { SPR_074em,SPR_074ax,SPR_074cl,SPR_074sp,SPR_074bw },

    { SPR_081em,SPR_081ax,SPR_081cl,SPR_081sp,SPR_081bw },
    { SPR_082em,SPR_082ax,SPR_082cl,SPR_082sp,SPR_082bw },
    { SPR_083em,SPR_083ax,SPR_083cl,SPR_083sp,SPR_083bw },
    { SPR_084em,SPR_084ax,SPR_084cl,SPR_084sp,SPR_084bw },

    { SPR_091em,SPR_091ax,SPR_091cl,SPR_091sp,SPR_091bw },
    { SPR_092em,SPR_092ax,SPR_092cl,SPR_092sp,SPR_092bw },
    { SPR_093em,SPR_093ax,SPR_093cl,SPR_093sp,SPR_093bw },
    { SPR_094em,SPR_094ax,SPR_094cl,SPR_094sp,SPR_094bw },

    { SPR_101em,SPR_101ax,SPR_101cl,SPR_101sp,SPR_101bw },
    { SPR_102em,SPR_102ax,SPR_102cl,SPR_102sp,SPR_102bw },
    { SPR_103em,SPR_103ax,SPR_103cl,SPR_103sp,SPR_103bw },
    { SPR_104em,SPR_104ax,SPR_104cl,SPR_104sp,SPR_104bw },

    { SPR_111em,SPR_111ax,SPR_111cl,SPR_111sp,SPR_111bw },
    { SPR_112em,SPR_112ax,SPR_112cl,SPR_112sp,SPR_112bw },
    { SPR_113em,SPR_113ax,SPR_113cl,SPR_113sp,SPR_113bw },
    { SPR_114em,SPR_114ax,SPR_114cl,SPR_114sp,SPR_114bw },

    { SPR_091em,SPR_091ax,SPR_091cl,SPR_091sp,SPR_091bw },
    { SPR_092em,SPR_092ax,SPR_092cl,SPR_092sp,SPR_092bw },

	{ SPR_pet001,SPR_pet001,SPR_pet001,SPR_pet001,SPR_pet001},
	{ SPR_pet002,SPR_pet002,SPR_pet002,SPR_pet002,SPR_pet002},
	{ SPR_pet003,SPR_pet003,SPR_pet003,SPR_pet003,SPR_pet003},
	{ SPR_pet004,SPR_pet004,SPR_pet004,SPR_pet004,SPR_pet004},

	{ SPR_pet011,SPR_pet011,SPR_pet011,SPR_pet011,SPR_pet011},

    { SPR_121em,SPR_121ax,SPR_121cl,SPR_121sp,SPR_121bw },
    { SPR_122em,SPR_122ax,SPR_122cl,SPR_122sp,SPR_122bw },

    { SPR_131em,SPR_131ax,SPR_131cl,SPR_131sp,SPR_131bw },
    { SPR_132em,SPR_132ax,SPR_132cl,SPR_132sp,SPR_132bw },
    { SPR_133em,SPR_133ax,SPR_133cl,SPR_133sp,SPR_133bw },

    { SPR_141em,SPR_141ax,SPR_141cl,SPR_141sp,SPR_141bw },
    { SPR_142em,SPR_142ax,SPR_142cl,SPR_142sp,SPR_142bw },

    { SPR_151em,SPR_151ax,SPR_151cl,SPR_151sp,SPR_151bw },
    { SPR_152em,SPR_152ax,SPR_152cl,SPR_152sp,SPR_152bw },

    { SPR_161em,SPR_161ax,SPR_161cl,SPR_161sp,SPR_161bw },
    { SPR_162em,SPR_162ax,SPR_162cl,SPR_162sp,SPR_162bw },

};

/*------------------------------------------------------------
 * Á¥    Îç  ì«  Ä¯¾®ÈÕÞ¥ØÆÖÐ  ì«  Ä¯Ã«  ÔÂ£Û
 * Â¦ÐÑ
 *  basebaseimagenumber     int             Ýç±åØ¦ÔÂ  ì«  Ä¯
 *  category                ITEM_CATEGORY   Ê§ÄÌ  Ø©¼°ÊÐ  ´òØø
 * ß¯Ô»°À
 *  Þ¥ØÆÖÐ  ì«  Ä¯£Û¾Þ·Â¡õ¼°ÁÝ·´£ý-1
 ------------------------------------------------------------*/
int CHAR_getNewImagenumberFromEquip( int basebaseimagenumber,
                                     ITEM_CATEGORY category )
{
    int i;
    /*  ITEM_CATEGORY ÃñÄáÓÀÛÍ  */
    if( category < 0 || category > arraysizeof(CHAR_eqimagetbl[0]) )
        return -1;
    for( i=0 ; i<arraysizeof(CHAR_eqimagetbl) ; i++ ){
        if( CHAR_eqimagetbl[i][0] == basebaseimagenumber )
            return CHAR_eqimagetbl[i][category];
    }
    return -1;
}

/*------------------------------------------------------------
 *   Ù¯  ¾®ÈÕ  ì«  Ä¯Ã«  ÔÂ£Û
 * Â¦ÐÑ
 *  string      char*         Ù¯  
 * ß¯Ô»°À
 *  ¾Þ·Â¡õ -1
 *  ¹«Ä¾¶¯Â½·ÖÎçºë·Â°×Å«ÓÀÛÍ  Ä¯
 ------------------------------------------------------------*/
int CHAR_seekGraphicNumberFromString( char* string )
{
    int     hashvalue;
    int     i;
    int     ret;

    /* ¹«¼°ÒýÒý¼°ÐÑÙ¯·ÖÔÈÐ×ÈÕ¹«¼°ÒýÒýß¯ÔÊ£Û */
    if( strtolchecknum( string, &ret,10,INT ) )return ret;
    return -1;
}




/*====================  ³Äèëô÷====================*/
/*  ³Äèëô÷Ã«âç  ÔÊÔÂ    */
typedef struct tagCHAR_invinsibleArea
{
    CHAR_AREAKIND   kind;
    int     floor;
    RECT    area;
    int     work[10];
}CHAR_invincibleArea;
CHAR_invincibleArea*    CHAR_invarea;
int                     CHAR_invareanum;

/*------------------------------------------------------------
 *   ³Äèëô÷¼°âÙÓå¼ÀÃ«ÔÊÔÂ£Û
 * Â¦ÐÑ
 *  filename        char*       É¬ÀÃ°×ÑëÄÌ»ï  
 * ß¯Ô»°À
 *  ÔÀ      TRUE(1)
 *  ÁÃ      FALSE(0)
 *------------------------------------------------------------*/
BOOL CHAR_initInvinciblePlace( char* filename )
{
    FILE*   f;
    char    line[256];
    int     linenum=0;
    int     invreadlen=0;

    f = fopen(filename,"r");
    if( f == NULL ){
        errorprint;
        return FALSE;
    }

    CHAR_invareanum=0;

    /*  ÒýÄÚ  ¶ãØ¦µæ»¥ÖÏµæØ¤ÔÂ¾®Éýµ¤¾®Æ©ÍÍÔÂ    */
    while( fgets( line, sizeof( line ), f ) ){
        linenum ++;
        if( line[0] == '#' )continue;        /* comment */
        if( line[0] == '\n' )continue;       /* none    */
        chomp( line );

        CHAR_invareanum++;
    }

    if( fseek( f, 0, SEEK_SET ) == -1 ){
        fprint( "Seek Error\n" );
        fclose(f);
        return FALSE;
    }

    CHAR_invarea = allocateMemory( sizeof(CHAR_invincibleArea)
                                   * CHAR_invareanum );
    if( CHAR_invarea == NULL ){
        fprint( "Can't allocate Memory %d\n" ,
                sizeof(CHAR_invincibleArea)*CHAR_invareanum);
        fclose( f );
        return FALSE;
    }
    /* âÙÓå¼À */
{
    int     i;
    for( i = 0; i < CHAR_invareanum; i ++ ) {
        int j;
        CHAR_invarea[i].kind = CHAR_AREAKIND_NONE;
        CHAR_invarea[i].floor = 0;
        CHAR_invarea[i].area.x = 0;
        CHAR_invarea[i].area.y = 0;
        CHAR_invarea[i].area.width = 0;
        CHAR_invarea[i].area.height = 0;
        for( j = 0; j < sizeof(CHAR_invarea[i].work); j ++ ) {
            CHAR_invarea[i].work[j] = -1;
        }
    }
}
    linenum = 0;
    while( fgets( line, sizeof( line ), f ) ){
        linenum ++;
        if( line[0] == '#' )continue;        /* comment */
        if( line[0] == '\n' )continue;       /* none    */
        chomp( line );
        replaceString( line, '\t' , ' ' );
        deleteSequentChar( line, " " );
        if( line[0] == ' ' )continue;
        {
            char    token[256];
            int     ret;
            int     lx,ly,rx,ry;
            CHAR_AREAKIND   kind=-1;

            ret = getStringFromIndexWithDelim( line, " ", 1, token, sizeof(token));
            if( ret==FALSE ){
                fprint("Syntax Error file:%s line:%d\n",filename,linenum);
                continue;
            }else{
                static struct CHAR_charareakindtbl
                {
                    char*   idstring;
                    CHAR_AREAKIND   kind;
                }CHAR_areakindtbl[]={
                    {"INV",  CHAR_INVAREA,},
                    {"ITM",  CHAR_CANNOTDROPAREA,},
                    {"MAG",  CHAR_CANNOTMAGICAREA,},
                };
                int i;
                for( i=0 ; i<arraysizeof(CHAR_areakindtbl) ; i++ )
                    if( strcmp( token,CHAR_areakindtbl[i].idstring )==0 ){
                        kind = CHAR_areakindtbl[i].kind;
                        break;
                    }
                if( kind == -1 ){
                    print( "×ÊÁÏ·¶Î§ÎÊÌâ: %d...",linenum );
                    continue;
                }
            }

            CHAR_invarea[invreadlen].kind = kind;
            ret = getStringFromIndexWithDelim( line, " ", 2, token, sizeof(token));
            if( ret==FALSE ){
                fprint("ÎÄ¼þÓï·¨´íÎó:%s µÚ%dÐÐ\n",filename,linenum);
                continue;
            }else
                CHAR_invarea[invreadlen].floor = atoi( token );

            ret = getStringFromIndexWithDelim( line, " ", 3, token, sizeof(token));
            if( ret ==FALSE){
                fprint("ÎÄ¼þÓï·¨´íÎó:%s µÚ%dÐÐ\n",filename,linenum);
                continue;
            }else
                getFourIntsFromString( token,&lx,&ly,&rx,&ry );

            if( lx > rx )swap(lx,rx);
            if( ly > ry )swap(ly,ry);

            CHAR_invarea[invreadlen].area.x = lx;
            CHAR_invarea[invreadlen].area.y = ly;
            CHAR_invarea[invreadlen].area.width = rx-lx;
            CHAR_invarea[invreadlen].area.height= ry-ly;
            invreadlen ++;
        }
    }
    fclose(f);
    CHAR_invareanum = invreadlen;
    print( "ÓÐÐ§²»¿ÉÕ½¶·ÓÐÐ§ÊýÊÇ %d...", CHAR_invareanum );

#ifdef DEBUG
    {
        int i;
        for( i=0; i <CHAR_invareanum ; i++ )
            print( "kind(%d) %d(%d,%d)-(%d,%d)\n",
                   CHAR_invarea[i].kind,
                   CHAR_invarea[i].floor,
                   CHAR_invarea[i].area.x,CHAR_invarea[i].area.y,
                   CHAR_invarea[i].area.width,
                   CHAR_invarea[i].area.height );
    }
#endif /*DEBUG*/

    return TRUE;
}

BOOL CHAR_isInvincibleArea( int floor , int x, int y )
{
    int     i;
    for( i=0 ; i<CHAR_invareanum ; i++ )
        if( CHAR_invarea[i].kind == CHAR_INVAREA
            && CHAR_invarea[i].floor == floor
            && CoordinateInRect( &CHAR_invarea[i].area,x,y ) )
            return TRUE;
    return FALSE;
}

BOOL CHAR_isCannotMagicArea( int floor , int x, int y, int magicnum )
{
    int     i;
    for( i=0 ; i<CHAR_invareanum ; i++ ) {
        if( CHAR_invarea[i].kind == CHAR_CANNOTMAGICAREA
            && CHAR_invarea[i].floor == floor
            && CoordinateInRect( &CHAR_invarea[i].area,x,y ) )
        {
            int     j;
            for( j = 0;
                j < sizeof(CHAR_invarea[i].work) &&
                CHAR_invarea[i].work[j] != -1;
                j ++ )
            {
                if( CHAR_invarea[i].work[j] == magicnum ) {
                    return TRUE;
                }
            }
        }
    }
    return FALSE;
}

typedef struct tagCHAR_appearposition
{
    int     floor;
    int     x,y;
}CHAR_appearposition;
CHAR_appearposition*    CHAR_appear;
int                     CHAR_appearnum;

BOOL CHAR_initAppearPosition( char* filename )
{
    FILE*   f;
    char    line[256];
    int     linenum=0;
    int     appearreadlen=0;

    f = fopen(filename,"r");
    if( f == NULL ){
        errorprint;
        return FALSE;
    }

    CHAR_appearnum=0;
    while( fgets( line, sizeof( line ), f ) ){
        linenum ++;
        if( line[0] == '#' )continue;        /* comment */
        if( line[0] == '\n' )continue;       /* none    */
        chomp( line );

        CHAR_appearnum++;
    }

    if( fseek( f, 0, SEEK_SET ) == -1 ){
        fprint( "Seek Error\n" );
        fclose(f);
        return FALSE;
    }

    CHAR_appear = allocateMemory( sizeof(CHAR_appearposition)
                                   * CHAR_appearnum );
    if( CHAR_appear == NULL ){
        fprint( "Can't allocate Memory %d\n" ,
                sizeof(CHAR_appearposition)*CHAR_appearnum);
        fclose( f );
        return FALSE;
    }
{
    int     i;
    for( i = 0; i < CHAR_appearnum; i ++ ) {
        CHAR_appear[i].floor = 0;
        CHAR_appear[i].x = 0;
        CHAR_appear[i].y = 0;

    }
}
    linenum = 0;
    while( fgets( line, sizeof( line ), f ) ){
        linenum ++;
        if( line[0] == '#' )continue;        /* comment */
        if( line[0] == '\n' )continue;       /* none    */
        chomp( line );
        replaceString( line, '\t' , ' ' );
        deleteSequentChar( line, " " );
        if( line[0] == ' ' )continue;

        {
            char    token[256];
            int     ret;

            /*  ·òÎç¹´»§¼°Ðþ¡õÛÍ¼þÃ«Î­ÔÂ    */
            ret = getStringFromIndexWithDelim( line," ",1,token,
                                               sizeof(token));
            if( ret==FALSE ){
                fprint("Syntax Error file:%s line:%d\n",filename,linenum);
                continue;
            }
            CHAR_appear[appearreadlen].floor = atoi(token);
            ret = getStringFromIndexWithDelim( line," ",2,token,
                                               sizeof(token));
            if( ret==FALSE ){
                fprint("Syntax Error file:%s line:%d\n",filename,linenum);
                continue;
            }

            CHAR_appear[appearreadlen].x = atoi( token );

            ret = getStringFromIndexWithDelim( line," ",3,token,
                                               sizeof(token));
            if( ret ==FALSE){
                fprint("Syntax Error file:%s line:%d\n",filename,linenum);
                continue;
            }

            CHAR_appear[appearreadlen].y = atoi( token );

            appearreadlen ++;
        }
    }
    fclose(f);

    CHAR_appearnum = appearreadlen;

    print( "ÓÐÐ§ÏÔÊ¾Î»ÖÃ·¶Î§ÊýÊÇ %d..", CHAR_appearnum );
#ifdef DEBUG

    {
        int i;
        for( i=0; i <CHAR_appearnum ; i++ )
            print( "Appear fl[%d] x[%d] y[%d]\n",
                   CHAR_appear[i].floor,
                   CHAR_appear[i].x,
                   CHAR_appear[i].y );
    }
#endif
    return TRUE;
}

BOOL CHAR_isAppearPosition( int floor, int *x, int *y)
{
    int i;
    for( i = 0; i < CHAR_appearnum; i ++ ) {
        if( CHAR_appear[i].floor == floor ) {
            *x = CHAR_appear[i].x;
            *y = CHAR_appear[i].y;
            return( TRUE);
        }
    }
    return( FALSE);
}

BOOL CHAR_isCannotDropArea( int floor , int x, int y )
{
    int     i;
    for( i=0 ; i<CHAR_invareanum ; i++ )
        if( CHAR_invarea[i].kind == CHAR_CANNOTDROPAREA
            && CHAR_invarea[i].floor == floor
            && CoordinateInRect( &CHAR_invarea[i].area,x,y ) )
            return TRUE;
    return FALSE;
}

typedef struct tagEldersPosition
{
    int     floor;
    int     x;
    int     y;
}EldersPosition;

#define MAXELDERS 128
static EldersPosition elders[MAXELDERS]=
{
  { 1006,15,22 },
  { 2006,20,16 },
	{ 3006,21,16 },
  { 4006,14,20 },
	{ 7770, 9,10 },

};

BOOL CHAR_getInitElderPosition( Char* ch,int hometown)
{
    int     index = 0;
	  int		point;
    if( hometown < 0 || hometown > 3 ) {
    	return FALSE;
    }
	index=hometown;

    if( index >= arraysizeof( elders ) ){
 		return FALSE;
    }

    ch->data[CHAR_FLOOR] = elders[index].floor;
    ch->data[CHAR_X] = elders[index].x;
    ch->data[CHAR_Y] = elders[index].y;
	ch->data[CHAR_LASTTALKELDER] = index;
	point = ch->data[CHAR_SAVEPOINT];

	point = point |  (1<<hometown) ;

	ch->data[CHAR_SAVEPOINT] = point;

	if( MAP_checkCoordinates( ch->data[CHAR_FLOOR], ch->data[CHAR_X],
									ch->data[CHAR_Y] ) == FALSE ){
		return FALSE;
	}

    return TRUE;
}

/*------------------------------------------------------------
 * ¾Þ»ïÄ¸¡õ¼°èëô÷Ã«  ÔÂ
 * Â¦ÐÑ
 *  elderindex      int     ¾Þ»ïÄ¸¡õÄÌ¼þ·¸ÓÀÛÍµ©
 *  fl              int*    °×·òÊ§Ã«°¾ØêäúÔÂèëô÷
 *  x               int*    xÃ«°¾ØêäúÔÂèëô÷
 *  y               int*    yÃ«°¾ØêäúÔÂèëô÷
 * Â¦ÐÑ
 *  ÔÀ    TRUE(1)
 *  ÁÃ    FALSE(0)
 ------------------------------------------------------------*/
BOOL CHAR_getElderPosition( int elderindex, int* fl, int* x, int* y )
{
	    if( elderindex<0 || arraysizeof(elders)<=elderindex )return FALSE;

    *fl = elders[elderindex].floor;
    *x  = elders[elderindex].x;
    *y  = elders[elderindex].y;
    return TRUE;
}
/*------------------------------------------------------------
 * Ó®  NPC»¥·ßÛÐ¼°âÙÓå¼À¼°ÎçÎå±å£ýÓ®    ±åÌ¤Îå³ð¸ê¼°·Ö£Û
 * ¹«¼°Ð×»§¼°ÄÌ¼þÕý¡õ°×ÄáÄÌµ©
 * Â¦ÐÑ
 *  index   int ×ó¡õÌï¡õÆ¥ ÁÃ  ØÆÐ×ÈÕFALSE Ã«¾®ÒüÔÊ
 *  fl      int ÞË  ¼°Ê÷  
 *  x       int
 *  y       int
 * NPC¼°»ï¡õÃñ¼þ¾®ÈÕÒø´õÄ¾ÔÂ¼°·Ö£Û
 ------------------------------------------------------------*/
BOOL CHAR_ElderSetPosition( int elderindex ,int fl,int x ,int y)
{
    if( elderindex < ELDERINDEXSTART ||
        elderindex >= MAXELDERS ){
        return FALSE;
    }

    elders[elderindex].floor = fl;
    elders[elderindex].x = x;
    elders[elderindex].y = y;
    return TRUE;
}


void CHAR_setInitValues( Char* ch )
{
    static int CHAR_titleindextbl[] ={ 0,1,2,3,4,5,6 };
    int     elderindex;
    int     i;
    BOOL    magician=FALSE;
    BOOL    priest=FALSE;

    elderindex = ch->data[CHAR_LASTTALKELDER];
    if( 0 <= elderindex
        && elderindex < arraysizeof( CHAR_titleindextbl ) )
        ch->indexOfHaveTitle[0] = CHAR_titleindextbl[elderindex];


    for( i=0 ; i<CHAR_SKILLMAXHAVE ; i++ ){
        static struct addIteTbl
        {
            int itemid[2];
        }additemtbl[SKILL_NUM] = {
            {   {  17, -1}   },
            {   {  -1, -1}   },
            {   {  -1, -1}   },
            {   {  17, -1}   },
            {   {  17, -1}   },
            {   {  17, -1}   },
            {   {  17, -1}   },
            {   {  17, -1}   },
            {   {  17, -1}   },
            {   {  17, -1}   },
            {   {  -1, -1}   }, 
            {   {  17, -1}   },
            {   { 601, -1}   },
            {   { 101, -1}   },
            {   { 201, -1}   },
            {   { 300, -1}   },
            {   { 400, -1}   },
            {   { 501, -1}   },
            {   {  17, -1}   },
        };

        if( ch->haveSkill[i].use ){
            int     j;
            int     skindex=ch->haveSkill[i].skill.data[SKILL_IDENTITY];

            for( j=0 ; j<arraysizeof( additemtbl[0].itemid ); j++ )
                CHAR_addItemToChar( ch, additemtbl[skindex].itemid[j] );

            if( skindex == SKILL_MERCHANT )
                ch->data[CHAR_GOLD] = 20;

            if( skindex == SKILL_MAGICIAN )
                magician = TRUE;
            else if( skindex == SKILL_PRIEST )
                priest = TRUE;

        }
    }

    if( magician || priest ) CHAR_addItemToChar( ch, 802 );
}


#if 1
static int EnemyExpTbl[]={
0,			//  0
2,			//  1
4,			//  2
6,			//  3
8,			//  4
10,			//  5
12,			//  6
14,			//  7
16,			//  8
18,			//  9
20,			// 10
26,			// 11
32,			// 12
38,			// 13
44,			// 14
50,			// 15
60,			// 16
70,			// 17
80,			// 18
90,			// 19
100,		// 20
115,		// 21
130,		// 22
145,		// 23
160,		// 24
175,		// 25
190,		// 26
205,		// 27
220,		// 28
235,		// 29
250,		// 30
270,		// 31
290,		// 32
310,		// 33
330,		// 34
350,		// 35
375,		// 36
400,		// 37
425,		// 38
450,		// 39
475,		// 40
500,		// 41
525,		// 42
550,		// 43
575,		// 44
600,		// 45
625,		// 46
650,		// 47
675,		// 48
700,		// 49
725,		// 50
750,		// 51
775,		// 52
800,		// 53
825,		// 54
850,		// 55
875,		// 56
900,		// 57
925,		// 58
950,		// 59
980,		// 60
1010,		// 61
1040,		// 62
1070,		// 63
1100,		// 64
1130,		// 65
1160,		// 66
1190,		// 67
1220,		// 68
1250,		// 69
1280,		// 70
1310,		// 71
1345,		// 72
1380,		// 73
1415,		// 74
1450,		// 75
1485,		// 76
1520,		// 77
1555,		// 78
1590,		// 79
1630,		// 80
1670,		// 81
1710,		// 82
1750,		// 83
1790,		// 84
1830,		// 85
1870,		// 86
1910,		// 87
1950,		// 88
1990		// 89
};           // 90
















// ½ÍÀÃ¶ÁØ¦·³¶­°À»ÍÒ£
int GetEnemyExp( int level ){
/*
	int exp = 0;
	if( level < 5 ){
		exp = 2 * level;
	}else
	if( level < 10 ){
		exp = 10 * level;
	}else
	if( level < 20 ){
		exp = 15 * level;
	}else
	if( level < 30 ){
		exp = 30 * level;
	}else
	if( level < 40 ){
		exp = 50 * level;
	}else
	if( level < 50 ){
		exp =60 * level;
	}else
	if( level < 60 ){
		exp =60 * level;
	}else{
		exp =70 * level;
	}
*/
	if( level >= arraysizeof( EnemyExpTbl ) )level = arraysizeof( EnemyExpTbl )-1;
	return EnemyExpTbl[level];
}
#endif

static int NeedLevelUpTbls[200] = {
			0, 0,
	        2,        6,       18,       37,       67,      110,      170,
	      246,      344,      464,      610,      782,      986,     1221,
	     1491,     1798,     2146,     2534,     2968,     3448,     3978,
		 4558,     5194,     5885,     6635,     7446,     8322,     9262,
	    10272,    11352,    12506,    13734,    15042,    16429,    17899,
		19454,    21098,    22830,    24656,    26576,    28594,    30710,
		32930,    35253,    37683,    40222,    42874,    45638,    48520,
		51520,    54642,    57886,    61258,    64757,    68387,    72150,
		76050,    80086,    84264,   106110,   113412,   121149,   129352,
	   138044,   147256,   157019,   167366,   178334,   189958,   202282,
	   215348,   229205,   243901,   259495,   276041,   293606,   312258,
	   332071,   353126,   375511,   399318,   424655,   451631,   480370,
	   511007,   543686,   578571,   615838,   655680,   698312,   743971,
	   792917,   845443,   901868,   962554,  1027899,  1098353,  1174420,
	  1256663,  1345723,  1442322,  1547281,  1661531,  1786143,  1922340,
	  2071533,  2235351,  2415689,  2614754,  2835137,  3079892,  3352633,
	  3657676,  4000195,  4386445,  4824041,  5322323,  5892866,  6550125,
	 12326614, 15496114, 20025638, 26821885, 37698249, 56734876, 68097265,
	 68290815, 68487425, 68687119, 68889921, 69095855, 69304945, 69517215,
	 69732689, 69951391, 70173345, 70398575, 70627105, 70858959, 71244161,
	 71342735, 71584705, 71830095, 72078929, 72331231, 72587025, 72846335,
	 73109185, 73615599, 73655601, 73929215, 74206465, 74487375, 74771969,
	 75060271, 75352305, 75648095, 75947665, 76421039, 76563241, 76874295,
	 77189225, 77508055, 77830809, 78157511, 78488185, 78822855, 79161545,
	 79724279, 79856081, 80206975, 80561985, 80921135, 81284449, 81651951,
	 82023665, 82399615, 82779825, 83434319, 83558121, 83951255, 84348745,
	 84750615, 85156889, 85567591, 85982745, 86402375, 86826505, 87575159,
	 87693361, 88131135, 88573505, 89020495, 89472129, 89928431, 90389425,
	 90855135, 91325585, 91800799
};

int CHAR_GetLevelExp( int charaindex, int level)
{
#ifdef _USER_EXP_CF
	if(level>getMaxLevel()){
		if(level>CHAR_getInt(charaindex, CHAR_LIMITLEVEL))
			return -1;
	}else if(level>getYBLevel()){
		if (CHAR_getInt(charaindex, CHAR_WHICHTYPE)==CHAR_TYPEPET ) {
			if(getPettrans() != -1 ){
				if(CHAR_getInt(charaindex,CHAR_TRANSMIGRATION)>=getPettrans())
					return NeedLevelUpTbls[level];
			}else{
				if(level<=CHAR_getInt(charaindex, CHAR_LIMITLEVEL))
					return NeedLevelUpTbls[level];
			}
			return -1;
		}else{
			if(CHAR_getInt(charaindex,CHAR_TRANSMIGRATION)<getChartrans())
				return -1;
		}
	}
	return NeedLevelUpTbls[level];
#else
	if( level >= arraysizeof( NeedLevelUpTbls ) ){
		return -1;
	}
	return NeedLevelUpTbls[level];
#endif
}

int CHAR_LevelUpCheck( int charaindex , int toindex)
{
	int exp, level, nextexp, iRet = FALSE;
	if( CHAR_CHECKINDEX( charaindex  ) == FALSE )return 0;
	exp = CHAR_getInt( charaindex, CHAR_EXP );
	while( 1 ){
		level = CHAR_getInt( charaindex, CHAR_LV );
		nextexp = CHAR_GetLevelExp( charaindex, level+1);
		if( nextexp < 0 )
			break;
		// Arminius 7.30 pet limit lv
		if (CHAR_getInt(charaindex, CHAR_WHICHTYPE)==CHAR_TYPEPET) {

			if (level >= CHAR_getInt(charaindex, CHAR_LIMITLEVEL) && 
				CHAR_getInt(charaindex, CHAR_LIMITLEVEL) > 0 )	{
				CHAR_setInt( charaindex, CHAR_EXP , 0);
				break;
			}
		}
		if( exp >= nextexp ){
			iRet ++;
			if( CHAR_getInt( charaindex, CHAR_WHICHTYPE ) == CHAR_TYPEPLAYER ){
				CHAR_setInt( charaindex, CHAR_DUELPOINT,
				CHAR_getInt( charaindex, CHAR_DUELPOINT) + (level+1)*10 );
			}
#ifdef	_PET_LIMITLEVEL
			else {
				CHAR_CheckPetDoLimitlevel( charaindex, toindex, level );
			}
#endif
			
			if( level >= CHAR_MAXUPLEVEL ){
			}else{
				if( CHAR_HandleExp( charaindex) != -1 ){
					exp = CHAR_getInt( charaindex, CHAR_EXP );
					CHAR_setInt( charaindex, CHAR_LV, level+1 );
				}
			}
		}else{
			break;
		}
	}
	return iRet;
}

#ifdef	_PET_LIMITLEVEL
void CHAR_CheckPetDoLimitlevel( int petindex , int toindex, int level)	
{
	int LevelUpPoint,vital,str,tgh,dex;
	int j,k;
	char token[56];
#define RAND(x,y)   ((x-1)+1+ (int)( (double)(y-(x-1))*rand()/(RAND_MAX+1.0)) )

	if( CHAR_getInt( petindex, CHAR_PETID) == 718 
#ifdef	_PET_2LIMITLEVEL
		|| CHAR_getInt( petindex, CHAR_PETID) == 401
#endif
		)	{
		LevelUpPoint = CHAR_getInt( petindex, CHAR_ALLOCPOINT );
		vital = (int )(( LevelUpPoint >> 24 ) & 0xFF);
		str = (int)(( LevelUpPoint >> 16 ) & 0xFF);
		tgh = (int)(( LevelUpPoint >> 8 ) & 0xFF);
		dex = (int)(( LevelUpPoint >> 0 ) & 0xFF);

		if( strcmp( CHAR_getChar( petindex, CHAR_OWNERCDKEY), CHAR_getChar( toindex, CHAR_CDKEY) ) ||
			strcmp( CHAR_getChar( petindex, CHAR_OWNERCHARANAME), CHAR_getChar( toindex, CHAR_NAME) )){
			sprintf(token,"ÎÒ²»ÈÏÊ¶ÄãÑ½£¡¿ì°ÑÎÒ»¹¸øÎÒÖ÷ÈË£¡");
			//andy 1/11 reEdit
			vital -= RAND(2,10);  if (vital<0) vital=0;
			str -= RAND(2,10);    if (str<0) str=0;
			tgh -= RAND(2,10);    if (tgh<0) tgh=0;
			dex -= RAND(2,10);    if (dex<0) dex=0;
			LevelUpPoint = (vital<<24)+(str<<16)+(tgh<<8)+(dex<<0);

			CHAR_setInt( petindex, CHAR_ALLOCPOINT , LevelUpPoint);
			CHAR_talkToCli( toindex, petindex, token, CHAR_COLORWHITE);

			LogPetPointChange(
					CHAR_getChar( toindex, CHAR_NAME ),
					CHAR_getChar( toindex, CHAR_CDKEY ),
					CHAR_getChar( petindex, CHAR_NAME),
					petindex, 2,
					CHAR_getInt( petindex, CHAR_LV),
					"No_master(ÂêÀÙ²»¶ÔÖ÷ÈË)",
					CHAR_getInt( toindex,CHAR_FLOOR),
					CHAR_getInt( toindex,CHAR_X ),
					CHAR_getInt( toindex,CHAR_Y )
					);
		}

		if( ( level % 20 ) == 0 )	{
			for( j=0; j < 3; j++)	{
				k = RAND( 0, 3);
				if( k == 0 )
					vital--;
				if( k == 1 )
					str--;
				if( k == 2 )
					tgh--;
				if( k == 3 )
					dex--;
			}
			if( vital < 0 ) vital =0;
			if( str < 0 ) str =0;
			if( tgh < 0 ) tgh =0;
			if( dex < 0 ) dex =0;
			LevelUpPoint = (vital<<24)+(str<<16)+(tgh<<8)+(dex<<0);
			CHAR_setInt( petindex, CHAR_ALLOCPOINT , LevelUpPoint);
			//print("\n << ( level % 20 ) == 0 >> ");
			LogPetPointChange(
					CHAR_getChar( toindex, CHAR_NAME ),
					CHAR_getChar( toindex, CHAR_CDKEY ),
					CHAR_getChar( petindex, CHAR_NAME),
					petindex,3,
					CHAR_getInt( petindex, CHAR_LV),
					"level%20",
					CHAR_getInt( toindex,CHAR_FLOOR),
					CHAR_getInt( toindex,CHAR_X ),
					CHAR_getInt( toindex,CHAR_Y )
					);
		}
	}
}
#endif

int CHAR_PetTakeDrop( int petindex, int floor, int ox, int oy)	//Ç¿ÖÆ¶ªÆú³èÎïÉý¼¶
{
	int x, y, objindex;

	x = rand()%10;
	y = rand()%10;

	//¶ªÏÂ³èÎï	CHAR_endCharOneArray
	objindex = PET_dropPetAbsolute( petindex,floor,x,y, FALSE );
	if( objindex == -1 )	{
		return -1;
	}
	CHAR_setWorkInt( petindex,CHAR_WORKOBJINDEX,objindex );
	CHAR_setInt( petindex, CHAR_FLOOR, floor);
	CHAR_setInt( petindex, CHAR_X, x);
	CHAR_setInt( petindex, CHAR_Y, y);
	CHAR_sendCToArroundCharacter( objindex);

	return floor;
}

int CHAR_PetTakeLevelUp( int petindex, int lv)//Ç¿ÖÆ³èÎïÉý¼¶
{
	int k;
	for( k = 1; k < lv; k ++ ){
		CHAR_PetLevelUp( petindex );
		CHAR_PetAddVariableAi( petindex, AI_FIX_PETLEVELUP );
		CHAR_setInt( petindex, CHAR_LV, CHAR_getInt( petindex, CHAR_LV) +1 );
	}

	CHAR_complianceParameter( petindex );
	return k;
}

int CHAR_PetLevelUp( int petindex )
{
	struct _RankRandTbl{
		int min;
		int max;
	}RankRandTbl[] = {
		{ 450, 500 },
		{ 470, 520 },
		{ 490, 540 },
		{ 510, 560 },
		{ 530, 580 },
		{ 550, 600 },
	};
	
	float Param[4] = { 0.0, 0.0, 0.0, 0.0 };
	int LevelUpPoint, petrank;
	float str, vital, dex, tgh, fRand, i;
	// Â¦ÐÑÃñÄáÓÀÛÍ
	if( CHAR_CHECKINDEX( petindex ) == FALSE )	return -1;
	if( CHAR_getInt( petindex, CHAR_WHICHTYPE ) != CHAR_TYPEPET )return -1;
	
	LevelUpPoint = CHAR_getInt( petindex, CHAR_ALLOCPOINT );
	
	// Ê¸ÓÀÐþ¼°·Â¼þÛÍ
	petrank = CHAR_getInt( petindex, CHAR_PETRANK );
	if( petrank < 0 || petrank > 5 ) petrank = 0;
	
	// äú  
	vital = (float)(( LevelUpPoint >> 24 ) & 0xFF);
	str = (float)(( LevelUpPoint >> 16 ) & 0xFF);
	tgh = (float)(( LevelUpPoint >> 8 ) & 0xFF);
	dex = (float)(( LevelUpPoint >> 0 ) & 0xFF);
	
	// ¨à¨ß¼Ô¨ã    ÈÓÄÌÎì·òÃ«ÈïÔÈ»¯±¸ÄÛ  ±å¨àóïÔÊ
	for( i = 0; i < 10; i ++ ){
		Param[RAND( 0, 3 )] += 1.0;
	}
	
	// ·Â¼þÛÍ±å·½ÔÂ  Çø¼°·Â¼þÄ¸Ø©
	fRand = (float)RAND( RankRandTbl[petrank].min, RankRandTbl[petrank].max )
		* 0.01;
	
	// à«Ô»ÈïÔ»ºÌÄÌ¼þÐþ·½Ô»»ÍÒ£
	vital = (float)vital * fRand + Param[0] * fRand;
	str = (float)str   * fRand + Param[1] * fRand;
	tgh = (float)tgh   * fRand + Param[2] * fRand;
	dex = (float)dex   * fRand + Param[3] * fRand;
	
	// »ÍÒ£
	CHAR_setInt( petindex, CHAR_VITAL,
		CHAR_getInt( petindex, CHAR_VITAL ) + (int)vital );
	CHAR_setInt( petindex, CHAR_STR,
		CHAR_getInt( petindex, CHAR_STR ) + (int)str );
	CHAR_setInt( petindex, CHAR_TOUGH,
		CHAR_getInt( petindex, CHAR_TOUGH ) + (int)tgh );
	CHAR_setInt( petindex, CHAR_DEX,
		CHAR_getInt( petindex, CHAR_DEX ) + (int)dex );
		
	// CoolFish: ËÇÓý¼Ò×åÉùÍû¸üÐÂ
	{
		int level = CHAR_getInt(petindex, CHAR_LV);
		
			int exp1 = 0, exp2 = 0;
		if (level > 30){
			char	tmpbuf[128];
			int ownerindex = CHAR_getWorkInt(petindex, CHAR_WORKPLAYERINDEX);
#ifdef _FMVER21
#else
			if (CHAR_getInt(ownerindex, CHAR_FMINDEX) < 0)	return	 0;
			if (CHAR_getInt(ownerindex, CHAR_FMLEADERFLAG) <= 0 )    return  0;
			if (strcmp(CHAR_getChar(ownerindex, CHAR_FMNAME), "") == 0)	return	0;
#endif	   	    
			if (CHAR_CHECKINDEX(ownerindex)){
				exp1 = CHAR_GetLevelExp( petindex, level);
				exp2 = CHAR_GetLevelExp( petindex, level - 1);
				 
//#ifdef _PERSONAL_FAME	// Arminius 8.30: ¼Ò×å¸öÈËÉùÍû
//				feedpoint = (exp1 - exp2) / 20000;	// ¼õ°ë
//#else
//				feedpoint = (exp1 - exp2) / 10000;
//#endif
				if ( exp1 - exp2 > 0){
					int feedpoint;
					// CoolFish: 2001/10/03
					int fd = getfdFromCharaIndex(ownerindex);
#ifdef _PERSONAL_FAME	// Arminius 8.30: ¼Ò×å¸öÈËÉùÍû
					CHAR_earnFame(ownerindex, exp2 / 20000);
					sprintf(tmpbuf, "%d", exp2 / 20000);
#endif
					feedpoint=exp2 / 20000;
#ifdef _FMVER21
					if (CHAR_getInt(ownerindex, CHAR_FMLEADERFLAG) > 0
						 && CHAR_getInt(ownerindex, CHAR_FMLEADERFLAG) != FMMEMBER_APPLY){
						saacproto_ACFixFMData_send(acfd,
							 CHAR_getChar(ownerindex, CHAR_FMNAME),
							 CHAR_getInt(ownerindex, CHAR_FMINDEX),
							 CHAR_getWorkInt(ownerindex, CHAR_WORKFMINDEXI),
							 FM_FIX_FMFEED, tmpbuf,
							 "",
							 CHAR_getWorkInt(ownerindex, CHAR_WORKFMCHARINDEX),
							 CONNECT_getFdid(fd));
					}
#else
					saacproto_ACFixFMData_send(acfd,
						CHAR_getChar(ownerindex, CHAR_FMNAME),
						CHAR_getInt(ownerindex, CHAR_FMINDEX),
						CHAR_getWorkInt(ownerindex, CHAR_WORKFMINDEXI),
						FM_FIX_FMFEED, tmpbuf, "",
						CHAR_getWorkInt(ownerindex, CHAR_WORKFMCHARINDEX), 0);
#endif
				}
			}
		}
	}
	
	return 0;
}

#ifdef _PET_TRANS
int PETTRANS_getPetBase( int petindex, int *work, int *petrank)
{
	int i, total=-1;
	if( !CHAR_CHECKINDEX( petindex) ){
		total = 0;
		*petrank = 1;
		for( i=0; i<4; i++)	{
			work[i] = 50;
			total += work[i];
		}
	}else{
		int LevelUpPoint = CHAR_getInt( petindex, CHAR_ALLOCPOINT );
		*petrank = CHAR_getInt( petindex, CHAR_PETRANK );
		work[0] = (float)(( LevelUpPoint >> 24 ) & 0xFF);
		work[1] = (float)(( LevelUpPoint >> 16 ) & 0xFF);
		work[2] = (float)(( LevelUpPoint >> 8 ) & 0xFF);
		work[3] = (float)(( LevelUpPoint >> 0 ) & 0xFF);
		total = ( work[0] + work[1] + work[2] + work[3] );
		if( total > 150 )	total = 150;
		if( total < 0 )		total = 0;
	}
	return total;
}

#ifdef _PET_2TRANS
int NPC_PetTransManGetAns( int total1, int total2, int LV, int rank, int tran )
#else
int NPC_PetTransManGetAns( int total1, int total2, int LV, int rank )
#endif
{
	int ans=0 , TransLV = 100;
	float total = 0.00;
	int Fx=1;

	if( LV > 130 ) LV = 130;
	total = ( (float)total1/100 ); // ÂêÀÙ×î¸ß total1=200
	total = total*total*total*total*total; 
	if( total < 1 ) total = 0;
	else total= total*1.3; // ËùÒÔtotal×î´ó=41.6 ×îÐ¡=0

	Fx = (int)((5-rank)*1.2)+5; // rank=0~6 ËùÒÔ Fx×î´ó=11(rank=0)×îÐ¡=4(rank=6)
	ans = (int)total + total2 + ((LV-TransLV)/Fx); //42+150+30/11=194

#ifdef _PET_2TRANS
	if( tran == 0 ){
		if( ans > 150 )	
			ans = 150;
	}
	else{
		if( ans > 200 )	
			ans = 200;
	}
#else
	if( ans > 150 )	{
		ans = 150;
	}
#endif
	print("\n ans = %d", ans);
	return ans;
}

int PETTRANS_PetTransManStatus( int toindex, int petindex1, int petindex2)
{
	int petrank=0,i;
	int total1,total2,total,petLV,ans;
	int petID,enemynum,ret=-1;
	int work[4]={0,0,0,0};
	int base[4]={0,0,0,0};

#define RAND(x,y)   ((x-1)+1+ (int)( (double)(y-(x-1))*rand()/(RAND_MAX+1.0)) )
	total1 = PETTRANS_getPetBase( petindex1, work, &petrank);
	total2 = PETTRANS_getPetBase( petindex2, base, &petrank);
	petLV	= CHAR_getInt( petindex2, CHAR_LV);
#ifdef _PET_2TRANS
	ans = NPC_PetTransManGetAns( total1, total2, petLV, petrank, CHAR_getInt( petindex2, CHAR_TRANSMIGRATION ) );
#else
	ans = NPC_PetTransManGetAns( total1, total2, petLV, petrank );
#endif
	total = total1 + (total2*4);
	total2 = 0;
	for( i=0; i<4; i++)	{
		work[i] = work[i]*4;
		total2 += work[i];
	}
	total = total1 + total2;
	for( i=0; i<4; i++)	{
		base[i] = (ans * (base[i]+work[i])) /total;
	}
	enemynum = ENEMY_getEnemyNum();
	petID = CHAR_getInt( petindex2, CHAR_PETID);
	for( i=0; i<enemynum; i++)	{
		if( ENEMY_getInt(i, ENEMY_TEMPNO) == petID )
			break;
	}
	if( i == enemynum )
		return -1;

	ret = GetNewPet( toindex, petindex2, i, base);
	if( ret < 0 )
		return -1;
	return ret;
}
#endif

#ifdef _CHIKULA_STONE
void CHAR_AutoChikulaStone( int charaindex, int Dflg)
{
	int Myhp, i, dnums;
	if( !CHAR_CHECKINDEX( charaindex) ) return;
	if( CHAR_getWorkInt( charaindex, CHAR_WORKBATTLEMODE) != BATTLE_CHARMODE_NONE ) return;
	switch( Dflg ){
	case 1:	//HP
		Myhp = CHAR_getInt( charaindex, CHAR_HP);
		dnums = CHAR_getWorkInt( charaindex, CHAR_WORKCHIKULAHP);
		Myhp += dnums;
		if( Myhp > CHAR_getWorkInt( charaindex, CHAR_WORKMAXHP) ){
			Myhp = CHAR_getWorkInt( charaindex, CHAR_WORKMAXHP);
		}
		CHAR_setInt( charaindex, CHAR_HP, Myhp);
		CHAR_complianceParameter( charaindex );
		CHAR_send_P_StatusString( charaindex, CHAR_P_STRING_HP);
		for( i=0; i<CHAR_MAXPETHAVE; i++)	{
			int petindex = CHAR_getCharPet( charaindex, i);
			if( !CHAR_CHECKINDEX( petindex) ) continue;
			Myhp = CHAR_getInt( petindex, CHAR_HP);
			Myhp += dnums;
			if( Myhp > CHAR_getWorkInt( petindex, CHAR_WORKMAXHP) ){
				Myhp = CHAR_getWorkInt( petindex, CHAR_WORKMAXHP);
			}
			CHAR_setInt( petindex, CHAR_HP, Myhp);
			CHAR_send_K_StatusString( charaindex, i, CHAR_K_STRING_HP|CHAR_K_STRING_AI);
		}
		break;
	case 2://MP
		Myhp = CHAR_getInt( charaindex, CHAR_MP);
		dnums = CHAR_getWorkInt( charaindex, CHAR_WORKCHIKULAMP);
		Myhp += dnums;
		if( Myhp > CHAR_getWorkInt( charaindex, CHAR_WORKMAXMP) ){
			Myhp = CHAR_getWorkInt( charaindex, CHAR_WORKMAXMP);
		}
		CHAR_setInt( charaindex, CHAR_MP, Myhp);
		CHAR_complianceParameter( charaindex );
		CHAR_send_P_StatusString( charaindex, CHAR_P_STRING_MP);
		break;
	}
}
#endif

int CHAR_findSurplusPetBox( int charaindex )
{
	int i, remnants=0;
	for( i=0; i<CHAR_MAXPETHAVE; i++){
    	int petindex = CHAR_getCharPet( charaindex, i);
		if( !CHAR_CHECKINDEX( petindex) ) remnants++;
	}

	return remnants;

}

#ifdef _FM_METAMO
void CHAR_ReMetamo( int charaindex )
{
	int oldMetamo=CHAR_getInt( charaindex , CHAR_BASEIMAGENUMBER);
	if( oldMetamo>=100700 && oldMetamo<100819){
		int newMetamo;
		int hbMetamo[]={
			 100000,100010,100025,100035,100055,100045,
			 100060,100070,100095,100090,100100,100110,
			 100135,100125,100145,100150,100165,100160,
			 100190,100185,100200,100210,100230,100220,
		};
		newMetamo=hbMetamo[(oldMetamo-100700)/5];
		CHAR_setInt( charaindex , CHAR_BASEIMAGENUMBER , newMetamo );
		CHAR_setInt( charaindex , CHAR_BASEBASEIMAGENUMBER , newMetamo );
		CHAR_sendCToArroundCharacter( CHAR_getWorkInt( charaindex , CHAR_WORKOBJINDEX ));
		CHAR_send_P_StatusString( charaindex , CHAR_P_STRING_BASEBASEIMAGENUMBER);
		CHAR_complianceParameter( charaindex );
	}
}
#endif

#ifdef _NEW_PLAYER_RIDE
void CHAR_PlayerRide( int charaindex )
{
		int i,j;
		int MetamoList[5][13]={
		/*{ Ð¡°«×Ó   ÈüÑÇÈË  ±è×ÓÄÐº¢  ¿á¸ç   ÐÜÆ¤ÄÐ   ´ó¸ö    Ð¡°«ÃÃ  ÐÜÆ¤ÃÃ  Ã±×ÓÃÃ  ¶Ì·¢¼ÐÃÃ  ÊÖÌ×Å®   À±ÃÃ    »¢}, ´ËÐÐÎªËµÃ÷ÐÐ */
			{ 100000, 100025, 100055, 100060, 100095, 100100, 100135, 100145, 100165, 100190, 100200, 100230, 309},	//ºì
			{ 100005, 100030, 100050, 100065, 100085, 100115, 100120, 100140, 100170, 100195, 100210, 100225, 307},	//ÂÌ
			{ 100010, 100035, 100045, 100070, 100090, 100110, 100125, 100150, 100160, 100185, 100215, 100220, 310},	//»Æ
			{ 100015, 100020, 100040, 100075, 100080, 100105, 100130, 100155, 100175, 100180, 100205, 100235, 308},	//»Ò
			{   331 ,   483 ,   330 ,   332 ,   483 ,   332 ,   333 ,   333 ,   331 ,   330 ,   332 ,   483 ,  0 },	//À×
		};
		for(i=0;i<4;i++)
				for(j=0;j<12;j++)
				 if(CHAR_getInt( charaindex, CHAR_BASEIMAGENUMBER) == MetamoList[i][j]){
				 	  if(strstr( getPlayerRide(), "ÅäÌ×ËÍ»¢¼ÓÀ×")){
				 			setNewplayergivepet(3,MetamoList[i][12]);
				 			setNewplayergivepet(4,MetamoList[4][j]);
				 		}else if(strstr( getPlayerRide(), "ÅäÌ×ËÍÀ×")){
				 			setNewplayergivepet(4,MetamoList[4][j]);
				 		}else if(strstr( getPlayerRide(), "ÅäÌ×ËÍ»¢")){
				 			setNewplayergivepet(4,MetamoList[i][12]);
				 		}
				 		break;
				 	}
}
#endif

#ifdef _USER_EXP_CF
BOOL LoadEXP( char* filename )
{
	FILE* fp;
	int i = 0;
	int MaxLevel=0;
	char buf[64];
	fp = fopen(filename, "r");
	if (fp == NULL)
	{
		print("ÎÞ·¨´ò¿ªÎÄ¼þ\n");
		return FALSE;
	}

	while(1){
		char	line[64];
		if (fgets(line, sizeof(line), fp) == NULL)	break;
		chop(line);

		if( line[0] == '#' )
			continue;
		for( i=0; i<strlen(line); i++ ){
            if( line[i] == '#' ){
			    line[i] = '\0';
		        break;
			}
		}
		//*************************************
		MaxLevel = MaxLevel + 1;
		
		easyGetTokenFromString(line, 1, buf, sizeof(buf));
		MaxLevel=atoi(exp);
		if (MaxLevel >= 199) {
			MaxLevel=199;
		else if (MaxLevel < 0) {
			MaxLevel=0;
		}
		easyGetTokenFromString(line, 2, buf, sizeof(buf));
		NeedLevelUpTbls[MaxLevel]=atoi(buf);
	}
	fclose(fp);
	return TRUE;
}
#endif

