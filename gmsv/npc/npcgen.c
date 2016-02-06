#include "version.h"
#include <string.h>

#include "common.h"
#include "npccreate.h"
#include "npctemplate.h"
#include "char.h"
#include "char_data.h"
#include "buf.h"
#include "object.h"
#include "readmap.h"
#include "item.h"
#include "handletime.h"
#include "map_deal.h"

typedef struct tagNPC_searchPoint
{
    int     floor;
    int     x;
    int     y;
}NPC_searchPoint;


/*  ÖÏ¼ÔNPCÃ«×ÛÔÂ¾® */
#define NPC_CREATECHALLENGETIME    1
int all_nosee = 0;		/* ¨à¼°ÁÝ£ûòå»¯¼°³ÄÃ« no_see ±å */
int all_nobody = 0;		/* ¨à¼°ÁÝ£ûòå»¯¼°³ÄÃ« no_body ±å */
int one_loop_born = 1;	/* ¨à»ï¡õÃóÆ¥  æÎÏ·ÐÄÇëÔÊÐÑ oneloop_born */

/*------------------------------------------------------------
 * ³ÄÃ«µ¤ÐÄ·ÖÔÊèëô÷Ã«è£ÀÃÔÊÔÂ
 * Â¦ÐÑ
 *  nobody  int                 1 ¼°ÁÝ·´²¾ÊÖÖÐØ¦ÖÐÁÝ±å¹´ÈÊÔÂ
 *                                          (²¾¾®ÖÐ»¯ÊÖ×ÛÔÂ)
 *                                          (²¾¾®ÖÐÐ×ÁÝ¼°ÐÄ×ÛÔÂ)
 *                                  Ã«ØÆØ¦ÈÊØ¦ÔÂ
 * ß¯Ô»°À
 *  ×ÛÄ¾ÔÂÁÝ·´  TRUE
 *  ×ÛÄ¾Ø¦ÖÐÁÝ·´  FALSE
 ------------------------------------------------------------*/
static BOOL NPC_searchCreatePoint( NPC_Create* cr,int  nobody,int nosee,
                            NPC_searchPoint* point, BOOL isflying )
{
    int     floor=cr->intdata[NPC_CREATEFLOORID];
    int     x   = cr->intdata[NPC_CREATEBORNLEFTUPX];
    int     y   = cr->intdata[NPC_CREATEBORNLEFTUPY];
    int     width;
    int     height;
    int     area;
    int     loop;
    BOOL    ret = FALSE;
    int     i, j;

    width = cr->intdata[NPC_CREATEBORNRIGHTDOWNX]
        - cr->intdata[NPC_CREATEBORNLEFTUPX] + 1;
    height = cr->intdata[NPC_CREATEBORNRIGHTDOWNY]
        - cr->intdata[NPC_CREATEBORNLEFTUPY] + 1;
    area = width * height;
    if( nobody == 0 && all_nobody == 0 ) {
        BOOL    found = FALSE;
        for( i = x ; i <= x +width && found == FALSE; i ++ ) {
            for( j = y; j <= y + height && found == FALSE; j ++ ) {
                OBJECT  object;
                for( object = MAP_getTopObj( floor, i,j);
                     object;
                     object = NEXT_OBJECT(object) )
                {
                    int objindex = GET_OBJINDEX( object);
                    if( OBJECT_getType(objindex) == OBJTYPE_CHARA ){
                        if( CHAR_getInt( OBJECT_getIndex(objindex),
                                     CHAR_WHICHTYPE ) == CHAR_TYPEPLAYER )
                        {
                            found = TRUE;
                            break;
                        }
                    }
                }
            }
        }
        if( found == FALSE ) {
            return( FALSE);
        }
    }
    for( loop=0 ; loop<NPC_CREATECHALLENGETIME ; loop++ ){
        int startpoint;
        int crx;
        int cry;

        startpoint = RAND(0,area-1);
        if( width == 0 ){
            return FALSE;
        }else{
            crx = startpoint%width + x;
            cry = startpoint/width + y;
        }

        if( cr->intdata[NPC_CREATEIGNOREINVINCIBLE] == 0
            && CHAR_isInvincibleArea( floor,crx,cry ) ){
            continue;
        }
        if( nosee == 0 && all_nosee == 0 ) {
            BOOL    found = FALSE;
            for( i = crx - CHAR_DEFAULTSEESIZ/2 ;
                 i <= crx +CHAR_DEFAULTSEESIZ/2 && found == FALSE ; i ++ )
            {
                for( j = cry - CHAR_DEFAULTSEESIZ/2;
                     j <= cry + CHAR_DEFAULTSEESIZ/2 && found == FALSE;
                     j ++ )
                {
                    OBJECT  object;
                    for( object = MAP_getTopObj( floor, i,j);
                         object;
                         object = NEXT_OBJECT(object) )
                    {
                        int objindex = GET_OBJINDEX( object);
                        if( OBJECT_getType(objindex) == OBJTYPE_CHARA ){
                            if( CHAR_getInt( OBJECT_getIndex(objindex),
                                             CHAR_WHICHTYPE )
                                == CHAR_TYPEPLAYER )
                            {
                                found = TRUE;
                                break;
                            }
                        }
                    }
                }
            }
            if( found == TRUE ){
            	return FALSE;
            }
        }
        if( MAP_walkAbleFromPoint( floor,crx,cry,isflying ) == TRUE ){
            point->floor = cr->intdata[NPC_CREATEFLOORID];
            point->x = crx;
            point->y = cry;
            ret = TRUE;
            break;
        }else{
        }
    }

    return( ret );
}


typedef struct tagNPC_Correspondfunction
{
    int template;
    int ch;
}NPC_Correspondfunction;

static NPC_Correspondfunction   correspondfunction[]={
    {   NPC_TEMPLATEINITFUNC,       CHAR_INITFUNC },
    {   NPC_TEMPLATEWALKPREFUNC,    CHAR_WALKPREFUNC },
    {   NPC_TEMPLATEWALKPOSTFUNC,   CHAR_WALKPOSTFUNC   },
    {   NPC_TEMPLATEPREOVERFUNC,    CHAR_PREOVERFUNC    },
    {   NPC_TEMPLATEPOSTOVERFUNC,   CHAR_POSTOVERFUNC   },
    {   NPC_TEMPLATEWATCHFUNC,      CHAR_WATCHFUNC   },
    {   NPC_TEMPLATELOOPFUNC,       CHAR_LOOPFUNC   },
    {   NPC_TEMPLATETALKEDFUNC,     CHAR_TALKEDFUNC   },
    {   NPC_TEMPLATEDYINGFUNC,      CHAR_DYINGFUNC   },
    {   NPC_TEMPLATEPREATTACKEDFUNC,CHAR_PREATTACKEDFUNC },
    {   NPC_TEMPLATEPOSTATTACKEDFUNC, CHAR_POSTATTACKEDFUNC },
    {   NPC_TEMPLATEOFFFUNC,            CHAR_OFFFUNC },
    {   NPC_TEMPLATELOOKEDFUNC,         CHAR_LOOKEDFUNC },
    {   NPC_TEMPLATEITEMPUTFUNC,         CHAR_ITEMPUTFUNC },
    {   NPC_TEMPLATESPECIALTALKEDFUNC,   CHAR_SPECIALTALKEDFUNC},
    {   NPC_TEMPLATEWINDOWTALKEDFUNC,    CHAR_WINDOWTALKEDFUNC},
#ifdef _USER_CHARLOOPS
	{   NPC_TEMPLATELOOPFUNCTEMP1,    CHAR_LOOPFUNCTEMP1},
	{   NPC_TEMPLATELOOPFUNCTEMP2,    CHAR_LOOPFUNCTEMP2},
	{   NPC_TEMPLATEBATTLEPROPERTY,    CHAR_BATTLEPROPERTY},
#endif
};
/*------------------------------------------------------------
 * èúÐÑÎìñâ¡õÔÊÔÂ
 * Â¦ÐÑ
 *  ch      Char*           Æ½ÅÒ·Â·¸¡õÕý
 * ß¯Ô»°À
 *  Ø¦ØÆ
 ------------------------------------------------------------*/
static void NPC_copyFunction( Char* ch, NPC_Template*   temp )
{
    int i;
    for( i=0 ; i<arraysizeof(correspondfunction) ; i++ )
        if( temp->chardata[correspondfunction[i].template].string[0] != '\0' )
            strcpysafe( ch->charfunctable[correspondfunction[i].ch].string,
                        sizeof(ch->charfunctable[correspondfunction[i].ch].string),
                        temp->chardata[correspondfunction[i].template].string);
}

static BOOL NPC_generateNPC( int createindex, int createtemplateindex )
{
    Char    one;
    NPC_searchPoint sp;
    int     i;
    NPC_Template*   template;
    NPC_Create*     cr;

    if( NPC_CHECKCREATEINDEX(createindex)
        && 0 <= createtemplateindex
        && createtemplateindex < arraysizeof(NPC_create[createindex].templateindex )
        && NPC_CHECKTEMPLATEINDEX( NPC_create[createindex].templateindex[createtemplateindex])){
        cr       = &NPC_create[createindex];
        template = &NPC_template[NPC_create[createindex].
                                templateindex[createtemplateindex]];
    }else{
        return FALSE;
	}
    if( NPC_searchCreatePoint( cr,
                               template->intdata[NPC_TEMPLATEMAKEATNOBODY],
                               template->intdata[NPC_TEMPLATEMAKEATNOSEE], &sp,
                               template->intdata[NPC_TEMPLATEISFLYING])  == FALSE ){
        return FALSE;
	}
    if( CHAR_getDefaultChar( &one, template->intdata[NPC_TEMPLATETYPE] ) == FALSE ){
        return FALSE;
	}
    one.data[CHAR_FLOOR] = sp.floor;
    one.data[CHAR_X] = sp.x;
    one.data[CHAR_Y] = sp.y;
    one.data[CHAR_DIR] = cr->intdata[NPC_CREATEDIR];
#ifdef _ADD_ACTION    
	one.data[CHAR_ACTIONSTYLE] = cr->intdata[NPC_CREATEACTION];
#endif

    one.data[CHAR_BASEBASEIMAGENUMBER] = one.data[CHAR_BASEIMAGENUMBER]
        = template->intdata[NPC_TEMPLATEIMAGENUMBER];
    if( cr->intdata[NPC_CREATEBASEIMAGENUMBER] != -1 )
        one.data[CHAR_BASEBASEIMAGENUMBER]
            = one.data[CHAR_BASEIMAGENUMBER]
            = cr->intdata[NPC_CREATEBASEIMAGENUMBER];

    strcpysafe(one.string[CHAR_NAME].string,
               sizeof(one.string[CHAR_NAME].string),
               template->chardata[NPC_TEMPLATECHARNAME].string);


    if( cr->chardata[NPC_CREATENAME].string[0] != '\0' ){
        strcpysafe(one.string[CHAR_NAME].string,
                   sizeof(one.string[CHAR_NAME].string),
                   cr->chardata[NPC_CREATENAME].string);
    }
    
    // Robin 0731
    one.data[CHAR_FMINDEX] = cr->intdata[NPC_CREATEFAMILY];
    NPC_copyFunctionSetToChar( template->
                               intdata[NPC_TEMPLATEFUNCTIONINDEX],
                               &one );
    NPC_copyFunction( &one, template );
    one.data[CHAR_LOOPINTERVAL] = template->
        intdata[NPC_TEMPLATELOOPFUNCTIME];
    if( cr->arg[createtemplateindex].string[0] != '\0'  ){
        strcpysafe( one.string[CHAR_NPCARGUMENT].string,
                    sizeof(one.string[CHAR_NPCARGUMENT].string),
                    cr->arg[createtemplateindex].string );
    }else{
        one.string[CHAR_NPCARGUMENT].string[0] = '\0';
    }
    one.data[CHAR_WHICHTYPE] = CHAR_TYPEENEMY;
    one.data[CHAR_NPCCREATEINDEX] = createindex;
    {
        static struct NPC_Paramtable
        {
            int     templateindex;
            int     charadataindex;
        }paramtbl[]={
            { NPC_TEMPLATEMINMP, CHAR_MAXMP },
            { NPC_TEMPLATEMINSTR, CHAR_STR },
            { NPC_TEMPLATEMINTOUGH, CHAR_TOUGH },


        };
        for( i=0 ; i<arraysizeof(paramtbl) ; i++ ){
            int randomvalue;
            randomvalue = RAND( 0,
                                template->randomdata[
                                    paramtbl[i].templateindex] );
            one.data[paramtbl[i].charadataindex]
                = one.data[paramtbl[i].charadataindex]
                + template->intdata[paramtbl[i].templateindex]
                + randomvalue;
        }
        one.data[CHAR_MP] = one.data[CHAR_MAXMP];
    }
    {
        int     itemindex=CHAR_STARTITEMARRAY;
        for( i = 0 ; i < template->intdata[NPC_TEMPLATEITEMNUM] ; i ++ ){
            if( RAND(0,32767) <= template->haveitem[i].haverate ){
                if( template->haveitem[i].itemnumber == -1
                    && template->haveitem[i].havenum > 0 ){
                    int     randomvalue = template->haveitem[i].havenum;
                    randomvalue *= (RAND(90,110))/100.0;
                    one.data[CHAR_GOLD] = min( CHAR_MAXGOLDHAVE, randomvalue );
                }else if( itemindex+i < CHAR_MAXITEMHAVE ){
                    one.indexOfExistItems[itemindex+i] =
                        ITEM_makeItemAndRegist(template->haveitem[i]
                                               .itemnumber);
                }
            }
        }
    }

    {
        int     charaindex;
        int     objindex;
        Object  obj;
        charaindex = CHAR_initCharOneArray( &one );
        if( charaindex == -1 ){
            CHAR_endCharData( &one );
            return FALSE;
        }
        if( template->intdata[NPC_TEMPLATEISFLYING] )
            CHAR_setFlg(charaindex, CHAR_ISFLYING, 1 );

        obj.type = OBJTYPE_CHARA;
        obj.index= charaindex;
        obj.x       = CHAR_getInt(charaindex,CHAR_X);
        obj.y       = CHAR_getInt(charaindex,CHAR_Y);
        obj.floor   = CHAR_getInt(charaindex,CHAR_FLOOR);
        objindex = initObjectOne( &obj );
        if( objindex == -1 ){
            CHAR_endCharOneArray( charaindex );
            return FALSE;
        }else{
        }
        CHAR_setWorkInt( charaindex,CHAR_WORKOBJINDEX,objindex );
#ifdef _ADD_ACTION
//CHAR_WORKACTION
		CHAR_setWorkInt( charaindex, CHAR_WORKACTION, 
			CHAR_getInt( charaindex, CHAR_ACTIONSTYLE ) );
		
        CHAR_sendWatchEvent( objindex, CHAR_getWorkInt( charaindex, CHAR_WORKACTION)
			,NULL,0,TRUE);
#else
		CHAR_sendWatchEvent(objindex,CHAR_ACTSTAND,NULL,0,TRUE);
#endif
        CHAR_complianceParameter( charaindex );
    }

    return TRUE;
}

void NPC_generateLoop( BOOL checkall )
{
    int     i,j;
    int    CreateOk=0;
    int enemynum ;
    static int  createcount = 0;        /* create¼°ÖÏ    ¾®ÈÕÝþ¾ôÔÊÔÂ¾® */
    static struct timeval _store_npc_generateloop_time;
	if( checkall == FALSE) {
	    if( time_diff_us( NowTime, _store_npc_generateloop_time ) < (1000*1000)){
	        return;
	    } else {
	        _store_npc_generateloop_time = NowTime;
	    }
	}

    for( i=0 ; i < NPC_createnum ; i++ ){
        if( createcount >= NPC_createnum ) createcount = 0;
        enemynum = NPC_getCreateInt( createcount, NPC_CREATEENEMYNUM );
        for( j = 0 ; j < enemynum ; j ++ ){
            if( NPC_createCheckGenerateFromTime(createcount) == TRUE ){
                CreateOk ++;
                NPC_createInitTime(createcount);
                if( NPC_generateNPC(createcount,j) == TRUE ){
                    NPC_createIncreaseEnemynum(createcount);
                }else{
                }
            }
        }
        createcount ++;
        if( checkall == FALSE && CreateOk >= one_loop_born  ){
            break;
        }
    }
}

