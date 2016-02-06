#include "version.h"
#include <ctype.h>

#include "common.h"
#include "char.h"
#include "item_event.h"
#include "net.h"
#include "item.h"
#include "object.h"
#include "lssproto_serv.h"
#include "util.h"
#include "char_data.h"
#include "readmap.h"
#include "map_deal.h"
#include "item.h"
#include "saacproto_cli.h"
#include "npccreate.h"
#include "handletime.h"
#include "anim_tbl.h"
#include "family.h"

#define SPR_kmydam	CG_HIT_MARK_00
BOOL CHAR_allprewalk( int index,int* dir,int* mode)
{
    int     poisonvalue;
    poisonvalue = CHAR_getInt(index, CHAR_POISON );
    if( poisonvalue >= 1 ){
        CHAR_setInt(index,CHAR_HP , CHAR_getInt(index,CHAR_HP)
                    - poisonvalue );
        CHAR_setWorkInt(index,CHAR_WORKLASTATTACKCHARAINDEX,-2);
        {
            int opt[2] = { SPR_kmydam,poisonvalue};
            CHAR_sendWatchEvent( CHAR_getWorkInt(index,CHAR_WORKOBJINDEX),
                                 CHAR_ACTDAMAGE,opt,2,TRUE);

            CHAR_sendStatusString( index,"M");
        }
        poisonvalue--;
        if( poisonvalue <= 0)poisonvalue = 1;
        CHAR_setInt(index,CHAR_POISON,poisonvalue);
    }
    if( CHAR_getInt(index,CHAR_CONFUSION) != 0 )
        *dir = RAND(0,7);
    if( CHAR_getInt(index,CHAR_STONE) != 0 )
        return FALSE;
    return TRUE;
}

void CHAR_allpostwalk( int index )
{
    return;
}

BOOL CHAR_makeCADefaultString( int objindex,char* buf,int buflen, int act )
{
	char	objindexbuf[64];
    /*  ÃñÄáÓÀÛÍ    */
    if( CHECKOBJECT(objindex) == FALSE )    return FALSE;
    if( OBJECT_getType(objindex) != OBJTYPE_CHARA ) return FALSE;
    snprintf( buf,buflen,"%s|%d|%d|%d|%d",
    			cnv10to62( objindex,objindexbuf, sizeof(objindexbuf)),
              OBJECT_getX(objindex), OBJECT_getY(objindex),
              act,
              CHAR_getInt(OBJECT_getIndex(objindex),CHAR_DIR));
    return TRUE;
}

static void CHAR_makeCADAMAGEStringFromSpecifiedCoordinate(
    int x , int y, char* buf,int buflen,int ef1,int damagevalue )
{
    snprintf( buf,buflen,"%d|%d|%d|%d|%d|%d|%d",
              -1,x,y, CHAR_ACTDAMAGE,0, ef1,damagevalue);
}

BOOL CHAR_makeCAOPT1String( int objindex,char* buf, int buflen, int act,int opt1 )
{
	char	objindexbuf[64];
    if( CHECKOBJECT(objindex) == FALSE )    return FALSE;
    if( OBJECT_getType(objindex) != OBJTYPE_CHARA ) return FALSE;
    snprintf( buf,buflen,"%s|%d|%d|%d|%d|%d",
    			cnv10to62( objindex,objindexbuf, sizeof(objindexbuf)),
              OBJECT_getX(objindex), OBJECT_getY(objindex),
              act,
              CHAR_getInt(OBJECT_getIndex(objindex),CHAR_DIR), opt1);
    return TRUE;
}

BOOL CHAR_makeCAOPT3String( int objindex,char* buf, int buflen, int act,int opt1,int opt2,int opt3 )
{
	char	objindexbuf[64];

    if( CHECKOBJECT(objindex) == FALSE )    return FALSE;
    if( OBJECT_getType(objindex) != OBJTYPE_CHARA ) return FALSE;
    snprintf( buf,buflen,"%s|%d|%d|%d|%d|%d|%d|%d",
    			cnv10to62( objindex,objindexbuf, sizeof(objindexbuf)),
              OBJECT_getX(objindex), OBJECT_getY(objindex),
              act,
              CHAR_getInt(OBJECT_getIndex(objindex),CHAR_DIR),
              opt1,opt2,opt3);
    return TRUE;
}

void CHAR_playerWatchfunc( int objmeindex, int objmoveindex,
                           CHAR_ACTION act, int x, int y, int dir, int* opt, int optlen )
{
    int     fd;
    int     meindex;
    if( !CHECKOBJECTUSE(objmeindex) )return;
    if( !CHECKOBJECTUSE(objmoveindex))return;
    if( OBJECT_getType(objmeindex) != OBJTYPE_CHARA )return;

    meindex = OBJECT_getIndex(objmeindex);
    if( CHAR_getInt( meindex,CHAR_WHICHTYPE ) != CHAR_TYPEPLAYER )return;

    fd = getfdFromCharaIndex( meindex );
    if( fd == -1 )return;
    switch( OBJECT_getType(objmoveindex) ){
    case OBJTYPE_CHARA:
    {
        char    buf[128];
        if( !CHAR_getFlg( OBJECT_getIndex(objmoveindex),CHAR_ISVISIBLE) )
            return;

        switch( act ){
        case CHAR_ACTSTAND:
        case CHAR_ACTWALK:
        case CHAR_ACTATTACK:
        case CHAR_ACTDEAD:
        case CHAR_ACTMAGIC:
        case CHAR_ACTITEM:
		case CHAR_ACTDOWN:
		case CHAR_ACTSIT:
		case CHAR_ACTHAND:
		case CHAR_ACTPLEASURE:
		case CHAR_ACTANGRY:
		case CHAR_ACTSAD:
        case CHAR_ACTDAMAGE:
		case CHAR_ACTGUARD:
		case CHAR_ACTTURN:
		case CHAR_ACTWARP:
		case CHAR_ACTACTIONWALK:
		case CHAR_ACTNOD:
		case CHAR_ACTTHROW:
		case CHAR_ACTACTIONSTAND:
			if( CHAR_makeCADefaultString(objmoveindex,buf,sizeof(buf),act) ) {
                CONNECT_appendCAbuf( fd,buf,strlen(buf));
            }
            break;
        // shan begin
        case CHAR_ACTTRADE:
            if( optlen == 1 )
                if( CHAR_makeCAOPT1String( objmoveindex, buf,
                                               sizeof( buf), act, opt[0] ) )
                    CONNECT_appendCAbuf( fd, buf, strlen(buf));
            break;

		// shan end
#ifdef _MIND_ICON
		case CHAR_MIND:			
			if( optlen == 1 )
                if( CHAR_makeCAOPT1String( objmoveindex, buf,
                                               sizeof(buf), act, opt[0]))
                    CONNECT_appendCAbuf( fd, buf, strlen(buf));
            break;
#endif
#ifdef _ITEM_CRACKER
		case CHAR_ITEM_CRACKER:			
			if( optlen == 1 )
                if( CHAR_makeCAOPT1String( objmoveindex, buf,
                                               sizeof(buf), act, opt[0]))
                    CONNECT_appendCAbuf( fd, buf, strlen(buf));
            break;
#endif
        case CHAR_ACTEFFECT:
            if( optlen == 1 )
                if( CHAR_makeCAOPT1String(objmoveindex,buf,
                                               sizeof(buf),act,opt[0] ) )
                    CONNECT_appendCAbuf( fd,buf,strlen(buf));
            break;
		case CHAR_ACTPOPUPNAME:
		case CHAR_ACTLEADER:
		case CHAR_ACTBATTLEWATCH:
            if( optlen == 1 )
                if( CHAR_makeCAOPT1String(objmoveindex,buf,
                                               sizeof(buf),act,opt[0] ) )
                    CONNECT_appendCAbuf( fd,buf,strlen(buf));
			break;
		case CHAR_ACTBATTLE:
            if( optlen == 3 )
                if( CHAR_makeCAOPT3String( objmoveindex,buf, sizeof(buf),
                						   act,opt[0],opt[1],opt[2] ) )
                    CONNECT_appendCAbuf( fd,buf,strlen(buf));
			break;
        default:
            break;
        }

        break;
    }
    case OBJTYPE_ITEM:
    case OBJTYPE_GOLD:
        CHAR_sendSpecifiedobjindexCToCharaindex(meindex,objmoveindex);
        break;
    default:
        break;
    }

}

void CHAR_sendWallDamage( int charaindex,int x, int y, int damage )
{
    int     fd;
    char    cabuf[256]="";

    fd = getfdFromCharaIndex(charaindex);
    if( fd == -1 )return;

    CHAR_makeCADAMAGEStringFromSpecifiedCoordinate(
        x,y,cabuf,sizeof(cabuf),SPR_kmydam,damage );
    CONNECT_appendCAbuf( fd,cabuf,strlen(cabuf));
}



static struct ParamShow
{
    int     paramindex;     /*  ÓÉ·Â¶ª¡õÕý¼°ÄÌ¼þ·¸ÓÀÛÍµ©    */
    char*   offmessage;     /*    ÔÈÐ×ÁÝ¼°¶ªÓÀ±¾¡õ³â  Ù¯      */
}pShow[]={
    {CHAR_PARALYSIS,    CHAR_RECOVERPARALYSISSTRING},
    {CHAR_SLEEP,      CHAR_RECOVERSILENCESTRING},
    {CHAR_STONE,        CHAR_RECOVERSTONESTRING},
    {CHAR_DRUNK,     CHAR_RECOVERDARKNESSSTRING},
    {CHAR_CONFUSION,    CHAR_RECOVERCONFUSIONSTRING},
};

void CHAR_recoveryStatus( int charaindex )
{
    int     i;
    int     cure=FALSE;
    if( !CHAR_CHECKINDEX(charaindex) )return;

    for( i=0 ; i < arraysizeof(pShow) ; i ++ ){
        if( CHAR_getInt(charaindex,pShow[i].paramindex) > 0 ){
            int old;
            old = CHAR_setInt(charaindex,pShow[i].paramindex,
                              CHAR_getInt(charaindex,
                                          pShow[i].paramindex) - 1 );
            if( old == 1 ){
                if( CHAR_getInt(charaindex,CHAR_WHICHTYPE) == CHAR_TYPEPLAYER){
                    CHAR_talkToCli( charaindex,-1, pShow[i].offmessage,
                                    CHAR_COLORWHITE );
                }
                cure = TRUE;
            }
        }
    }
    if( cure ){
        CHAR_sendStatusString( charaindex, "P" );
        CHAR_sendCToArroundCharacter( CHAR_getWorkInt(
            charaindex, CHAR_WORKOBJINDEX) );
    }
}

static BOOL CHAR_clairvoyance_gold(int objindex,char* buf )
{
    if( OBJECT_getType( objindex ) == OBJTYPE_GOLD ){
        *buf = 'G';
        return TRUE;
    }
    return FALSE;
}

static BOOL CHAR_clairvoyance_item(int objindex,char* buf )
{
    if( OBJECT_getType( objindex ) == OBJTYPE_ITEM){
        *buf = 'I';
        return TRUE;
    }
    return FALSE;
}

static BOOL CHAR_clairvoyance_player(int objindex,char* buf )
{
    if( OBJECT_getType( objindex ) == OBJTYPE_CHARA
        && CHAR_getInt(OBJECT_getIndex(objindex),CHAR_WHICHTYPE)
        == CHAR_TYPEPLAYER ){
        *buf = 'P';
        return TRUE;
    }
    return FALSE;
}

static BOOL CHAR_clairvoyance_enemy(int objindex,char* buf )
{
    if( OBJECT_getType( objindex ) == OBJTYPE_CHARA
        && CHAR_getInt(OBJECT_getIndex(objindex),CHAR_WHICHTYPE)
        == CHAR_TYPEENEMY ){
        *buf = 'E';
        return TRUE;
    }
    return FALSE;
}

static char CHAR_clairvoyanceString[STRINGBUFSIZ];

static void CHAR_clairvoyance( int charaindex )
{
    char    onebuf[128];
    int     stringlen=0;
    int     clairvoyancelevel;
    int     centerx,centery,fl;
    int     i,j;
    BOOL    sendclairvoyance=FALSE;

#define CHAR_CLAIRVOYANCEWIDTH  81
    BOOL (*clairvoyancefunc[])(int,char*) ={
        CHAR_clairvoyance_gold,
        CHAR_clairvoyance_item,
        CHAR_clairvoyance_player,
        CHAR_clairvoyance_enemy,
    };
    if( CHAR_getWorkInt( charaindex, CHAR_WORKLOOPSTARTSEC)%10 != 0 )
        return;

    clairvoyancelevel = CHAR_getInt(charaindex,CHAR_RADARSTRENGTH );

    if( clairvoyancelevel <= 0 )return;
    if( clairvoyancelevel > arraysizeof( clairvoyancefunc ) )
        clairvoyancelevel = arraysizeof( clairvoyancefunc );

    centerx = CHAR_getInt(charaindex,CHAR_X);
    centery = CHAR_getInt(charaindex,CHAR_Y);
    fl = CHAR_getInt(charaindex,CHAR_FLOOR);


    for( i = centerx - CHAR_CLAIRVOYANCEWIDTH/2 ;
         i < centerx + CHAR_CLAIRVOYANCEWIDTH/2 ; i ++ ){
        for( j = centery - CHAR_CLAIRVOYANCEWIDTH/2 ;
             j < centery + CHAR_CLAIRVOYANCEWIDTH/2 ; j ++ ){
            OBJECT  object;
            for( object = MAP_getTopObj(fl,i,j) ; object ;
                 object = NEXT_OBJECT( object ) ){
                int     objindex = GET_OBJINDEX(object);
                int     loop;

                if( OBJECT_getType(objindex) == OBJTYPE_NOUSE )continue;
                if( OBJECT_getType(objindex) == OBJTYPE_CHARA
                    && OBJECT_getIndex(objindex) == charaindex) continue;
                for( loop = 0 ; loop < clairvoyancelevel ; loop ++ ){
                    BOOL    ret;
                    char    id;
                    ret = clairvoyancefunc[loop](objindex,&id);
                    if( ret == TRUE ){
                        sendclairvoyance=TRUE;
                        snprintf( onebuf,sizeof(onebuf),
                                  "%d|%d|%c|",i,j,id );
                        strcpysafe( CHAR_clairvoyanceString + stringlen,
                                    sizeof(CHAR_clairvoyanceString)
                                    - stringlen,   onebuf);
                        stringlen += strlen(onebuf);
                        if( stringlen > sizeof(CHAR_clairvoyanceString) )
                            goto RETURN;
                    }
                }
            }
        }
    }

RETURN:
    if( stringlen >= 1 ){
        int fd;
        fd = getfdFromCharaIndex( charaindex );
        dchop( CHAR_clairvoyanceString, "|" );
        if( fd != -1 )
            lssproto_R_send( fd, CHAR_clairvoyanceString );
    }else if( !sendclairvoyance ){
        int fd;
        fd = getfdFromCharaIndex( charaindex );
        if( fd != -1 )
            lssproto_R_send( fd, "" );
    }
}

void CHAR_loopFunc( int charaindex )
{
    CHAR_recoveryStatus( charaindex );
    CHAR_clairvoyance( charaindex );
}

void CHAR_playerresurrect( int charaindex, int hp )
{
    CHAR_setInt(charaindex,CHAR_BASEIMAGENUMBER,
                CHAR_getInt(charaindex,CHAR_BASEBASEIMAGENUMBER));
    CHAR_setFlg(charaindex,CHAR_ISDIE,      0);
    CHAR_setFlg(charaindex,CHAR_ISATTACKED, 1);
    CHAR_setFlg(charaindex,CHAR_ISOVERED, 0);
    if( hp >= CHAR_getWorkInt(charaindex,CHAR_WORKMAXHP) )
        hp = CHAR_getWorkInt(charaindex,CHAR_WORKMAXHP);
    else if( hp <= 0 )
        hp = 1;
    CHAR_setInt(charaindex,CHAR_HP, hp);
}

int CHAR_die( int charaindex )
{
	CHAR_DischargeParty( charaindex, 0);
    {
        int bymonster = 0;
        int attackindex = CHAR_getWorkInt(charaindex,
                                          CHAR_WORKLASTATTACKCHARAINDEX);
        if( attackindex == -2  )
            bymonster = 0;
        else if( CHAR_CHECKINDEX(attackindex) == TRUE ){
            if( CHAR_getInt(attackindex,CHAR_WHICHTYPE)
                == CHAR_TYPEENEMY ){
                bymonster = 1;
            }else{
                bymonster = 2;
            }
        }
        if( bymonster == 0 || bymonster == 1 ){
            int i;
            for( i=0 ; i<CHAR_EQUIPPLACENUM ; i++ )
                CHAR_DropItem(charaindex,i);
        }else{
            int eqindex[CHAR_EQUIPPLACENUM];
            int itemhavenum=0;
            int i;

            for( i=0 ; i<CHAR_EQUIPPLACENUM ; i++ )
                if( ITEM_CHECKINDEX(CHAR_getItemIndex(charaindex,i)) )
                    eqindex[itemhavenum++] = i;

            if( itemhavenum >= 1 ){
                int  randomindex = RAND(0,itemhavenum-1);
                CHAR_DropItem(charaindex,eqindex[randomindex]);

            }
        }

        CHAR_DropMoney(charaindex,
                       CHAR_getInt(charaindex,CHAR_GOLD)/2 );
        CHAR_setInt(charaindex,CHAR_GOLD,0);
    }
    CHAR_complianceParameter(charaindex);
    CHAR_sendCToArroundCharacter( CHAR_getWorkInt(charaindex, CHAR_WORKOBJINDEX) );
    CHAR_setInt(charaindex,CHAR_DEADCOUNT,
                CHAR_getInt(charaindex,CHAR_DEADCOUNT)+1);
    {
        int i;
        BOOL old=FALSE;
        for( i=0 ; i<arraysizeof(pShow);i++)
            if( CHAR_setInt( charaindex, pShow[i].paramindex, 0 ) > 0 ){
                old = TRUE;
            }
        if( CHAR_setInt( charaindex, CHAR_POISON, 0 ) > 0 )old=TRUE;
        if( old )
            CHAR_sendCToArroundCharacter( CHAR_getInt(charaindex, CHAR_WORKOBJINDEX));
    }
    CHAR_sendStatusString( charaindex, "P");
    CHAR_sendStatusString( charaindex, "I");
    CHAR_setFlg(charaindex,CHAR_ISDIE,1);
    CHAR_setFlg(charaindex,CHAR_ISATTACKED,0);
    return 0;
}

void CHAR_playerTalkedfunc( int charaindex, int talkindex,char* message, int color, int channel )
{
    int     fd;
    char    lastbuf[4096];
    char    mesgbuf[4096];
    char    escapebuf[4096];
    int fmindex = CHAR_getInt( charaindex, CHAR_FMINDEX );
    fd = getfdFromCharaIndex( charaindex );
    if( fd == -1 )return;
    if( (channel>-1) && (fmindex>0) ){
		if( channel == 0 ){
			snprintf( lastbuf,sizeof(lastbuf),"P|[×å]%s",
				makeEscapeString( CHAR_appendNameAndTitle(talkindex, message, mesgbuf,sizeof(mesgbuf)),
				escapebuf,sizeof(escapebuf) ));
		}
#ifdef _FMVER21	                  
		else if( channel == FAMILY_MAXCHANNEL && CHAR_getInt( talkindex, CHAR_FMLEADERFLAG ) == FMMEMBER_LEADER ){
#else
		else if( channel == FAMILY_MAXCHANNEL && CHAR_getInt( talkindex, CHAR_FMLEADERFLAG ) == 1 ){
#endif
			snprintf( lastbuf,sizeof(lastbuf),"P|[×å³¤¹ã²¥]%s",
				makeEscapeString(CHAR_appendNameAndTitle(talkindex, message,mesgbuf,sizeof(mesgbuf)),
				escapebuf,sizeof(escapebuf) ));
		}else{
			snprintf( lastbuf,sizeof(lastbuf),"P|[%d]%s",channel,
				makeEscapeString(CHAR_appendNameAndTitle(talkindex, message,mesgbuf,sizeof(mesgbuf)),
				escapebuf,sizeof(escapebuf) ));
		}
	}else{
		snprintf( lastbuf,sizeof(lastbuf),"P|%s",
				makeEscapeString(CHAR_appendNameAndTitle(talkindex, message,mesgbuf,sizeof(mesgbuf)),
				escapebuf,sizeof(escapebuf) ));
	}
	lssproto_TK_send( fd, CHAR_getWorkInt( talkindex, CHAR_WORKOBJINDEX ),lastbuf, color);
}
