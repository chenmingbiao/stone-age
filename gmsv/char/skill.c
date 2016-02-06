#include "version.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "skill.h"
#include "util.h"
#include "char.h"


static SKILL_intDataSetting SKILL_setint[SKILL_DATAINTNUM]={
    {"lv"},           /*  SKILL_LEVEL	*/
    {"id"},           /*  SKILL_ID		*/
};

static SKILL_charDataSetting SKILL_setchar[SKILL_DATACHARNUM]={
};




static char    SKILL_dataString[STRINGBUFSIZ];

char* SKILL_makeStringFromSkillData( Skill* sk )
{
    int     i;
    int     strlength=0;

    for( i = 0 ; i < SKILL_DATAINTNUM ; i ++ ){
        char    linedata[128];
        snprintf( linedata , sizeof(linedata),
                  "%s=%d" NONCHAR_DELIMITER,
                  SKILL_setint[i].dumpskill, sk->data[i] );

        strcpysafe( &SKILL_dataString[strlength],
                    sizeof( SKILL_dataString ) - strlength,
                    linedata );
        strlength += strlen( linedata );
        if( strlength > sizeof( SKILL_dataString ) )goto RETURN;
    }

    for( i = 0 ; i < SKILL_DATACHARNUM ; i ++ ){
        char    linedata[128];
        char    escapebuffer[128];
        snprintf( linedata, sizeof(linedata),
                  "%s=%s" NONCHAR_DELIMITER,
                  SKILL_setchar[i].dumpskill,
                  makeEscapeString(sk->string[i].string,escapebuffer,
                                   sizeof(escapebuffer)));

        strcpysafe( &SKILL_dataString[strlength],
                    sizeof( SKILL_dataString ) - strlength,
                    linedata );
        strlength += strlen( linedata );
        if( strlength > sizeof( SKILL_dataString ) )goto RETURN;
    }

RETURN:
    dchop( SKILL_dataString , NONCHAR_DELIMITER );

    return SKILL_dataString;
}


BOOL SKILL_makeSkillFromStringToArg( char* src, Skill* sk )
{
    int     readindex=1;
    while( 1 ){
        BOOL    ret;
        char    linebuf[512];
        char    first[256];
        char    second[256];
        int     i;


        ret = getStringFromIndexWithDelim( src ,NONCHAR_DELIMITER ,
                                           readindex,
                                           linebuf,  sizeof( linebuf ) );
        if( ret == FALSE )
            break;

        ret = getStringFromIndexWithDelim( linebuf ,"=", 1,
                                           first,  sizeof( first ) );
        if( ret == FALSE )  return FALSE;
        strcpysafe( second , sizeof( second ),
                    linebuf + strlen(first) + strlen("=") );

        for( i = 0 ; i < SKILL_DATAINTNUM ; i ++ ){
            if( strcmp(first ,SKILL_setint[i].dumpskill) == 0 ){
                sk->data[i] = atoi( second );
                goto NEXT;
            }
        }

        for( i = 0 ; i < SKILL_DATACHARNUM ; i ++ ){
            if( strcmp(first ,SKILL_setchar[i].dumpskill) == 0 ){
                strcpysafe( sk->string[i].string,
                            sizeof(sk->string[i].string),
                            makeStringFromEscaped(second) );
                goto NEXT;
            }
        }

        fprint( "??? : %s[%s]\n" , linebuf, first );
        
    NEXT:
        readindex++;
    }

    return TRUE;
}

INLINE int SKILL_getInt( Skill* skill, int element)
{
	int value = skill->data[element];

	if( element == SKILL_LEVEL )	value /= 100;

    return value;
}


INLINE int SKILL_setInt( Skill* skill, int element, int new)
{
    int     buf = SKILL_getInt( skill, element );
    skill->data[element] = new;
    return buf;
}

static void SKILL_setitemlimit( int charaindex, Skill* sk );
static void SKILL_setmerchant( int charaindex, Skill* sk );
static void SKILL_setlevel( int charaindex, Skill* sk );

static SKILL_table  SKILL_tbl[]={
    {8,SKILL_setlevel},			/*  SKILL_FIRE				*/
    {4,NULL},					/*  SKILL_MAGICIAN			*/
    {4,NULL},					/*  SKILL_PREIST			*/
    {8,SKILL_setitemlimit},		/*  SKILL_ALOTOFTHINGS		*/
    {8,NULL},					/*  SKILL_AVOIDRATEUP		*/
    {4,SKILL_setlevel},			/*  SKILL_DETERMINEITEM		*/
    {5,SKILL_setlevel},			/*  SKILL_DETERMINEOTHERS	*/
    {8,SKILL_setmerchant},		/*  SKILL_MERCHANT			*/
    {8,SKILL_setlevel},			/*  SKILL_HEALER			*/
    {8,SKILL_setlevel},			/*  SKILL_LARGEVOICE		*/
};

static void SKILL_setitemlimit( int charaindex, Skill* sk )
{
    static int itemlimit[9]={
        7 + 8 + 4*0,
        7 + 8 + 4*1,
        7 + 8 + 4*2,
        7 + 8 + 4*3,
        7 + 8 + 4*4,
        7 + 8 + 4*5,
        7 + 8 + 4*6,
        7 + 8 + 4*7,
        7 + 8 + 4*8,
    };
    int     level;
    if( !CHAR_CHECKINDEX(charaindex) )return;
    if( sk->data[SKILL_IDENTITY] != SKILL_ALOTOFTHINGS )return;
    level = sk->data[SKILL_LEVEL];
    if( level < 0 )level = 0;
    if( level>=arraysizeof(itemlimit) ) level=arraysizeof(itemlimit) - 1;
}


static void SKILL_setmerchant( int charaindex, Skill* sk )
{
    int     level;
    int     merchantlevel=0;

    if( !CHAR_CHECKINDEX(charaindex) )return;
    if( sk->data[SKILL_IDENTITY] != SKILL_MERCHANT )return;

    level = sk->data[SKILL_LEVEL];
    if( level < 0 )level = 0;
    if( level >= SKILL_tbl[sk->data[SKILL_IDENTITY]].maxlevel )
        level  = SKILL_tbl[sk->data[SKILL_IDENTITY]].maxlevel;

    SETHIGHVALUE(merchantlevel,(100 - level*5));
    SETLOWVALUE(merchantlevel,(20 + level*5));
    
    CHAR_setInt( charaindex,CHAR_MERCHANTLEVEL,merchantlevel );
}


static void SKILL_setlevel( int charaindex, Skill* sk )
{
    static struct skillvalset
    {
        SKILL_ID        id;
        CHAR_DATAINT    charadataindex;
    } skvalset[] = {
        { SKILL_DETERMINEITEM,      CHAR_DETERMINEITEM },
        { SKILL_DETERMINEOTHERS,    CHAR_RADARSTRENGTH },
        { SKILL_HEALER,             CHAR_HEALERLEVEL },
        { SKILL_LARGEVOICE,         CHAR_CHATVOLUME },
    };
    int id;
    int i;
    int index=-1;
    int level;

    if( !CHAR_CHECKINDEX(charaindex) )return;
    id = sk->data[SKILL_IDENTITY];
    for( i=0 ; i<arraysizeof(skvalset) ; i ++ )
        if( skvalset[i].id == id )
            index = i;

    if( index == -1 )return;

    level = sk->data[SKILL_LEVEL];
    if( level < 0 )level = 0;
    if( level >= SKILL_tbl[id].maxlevel )
        level  = SKILL_tbl[id].maxlevel;

    CHAR_setInt(charaindex,skvalset[index].charadataindex,level );
}

#define SKILLSTRINGBUFSIZ   256

static char    ITEM_statusStringBuffer[SKILLSTRINGBUFSIZ];

char* SKILL_makeSkillStatusString( Skill* skill )
{
    snprintf( ITEM_statusStringBuffer,
              sizeof( ITEM_statusStringBuffer ),
              "%d|%d",
              SKILL_getInt(skill,SKILL_IDENTITY),
              SKILL_getInt(skill,SKILL_LEVEL) );


    return ITEM_statusStringBuffer;
}

char* SKILL_makeSkillFalseString( void )
{
    snprintf( ITEM_statusStringBuffer,
              sizeof( ITEM_statusStringBuffer ),
              "|" );
    return ITEM_statusStringBuffer;
}


BOOL  SKILL_CHECKID( int skillid )
{
    if( SKILL_NUM <= skillid && skillid > 0 )return FALSE;
    return TRUE;
}

BOOL SKILL_makeSkillData( Skill* sk ,int skid, int lev )
{
    sk->data[SKILL_LEVEL] = lev;
    sk->data[SKILL_IDENTITY] = skid;

    return TRUE;
}

int SKILL_levelup( Skill* sk )
{
    int     id = sk->data[SKILL_IDENTITY];
    if( !SKILL_CHECKID(id) )return -1;
    sk->data[SKILL_LEVEL] ++;
    sk->data[SKILL_LEVEL] = min( sk->data[SKILL_LEVEL],
                                 SKILL_tbl[id].maxlevel );
    return TRUE;
}

int SKILL_getLevelFromSkillID( int charaindex, SKILL_ID id )
{
    int     i;
    if( !CHAR_CHECKINDEX(charaindex) )return -1;
    for( i=0 ; i<CHAR_SKILLMAXHAVE ; i++ ){
        CHAR_HaveSkill* sk;
        sk = CHAR_getCharHaveSkill(charaindex,i);
        if( sk && sk->use && sk->skill.data[SKILL_IDENTITY] == id )
            return sk->skill.data[SKILL_LEVEL];
        
    }
    return -1;
}

BOOL SKILL_getUpableSkillID( int charaindex,char* buf, int buflen )
{
    int     i;
    if( !CHAR_CHECKINDEX(charaindex)) return FALSE;
    if( buflen <= 0 )return FALSE;
    buf[0] = '\0';
    for( i=0 ; i<CHAR_SKILLMAXHAVE ; i++ ){
        CHAR_HaveSkill* chsk;
        chsk = CHAR_getCharHaveSkill(charaindex,i);
        if( chsk && chsk->use
            && SKILL_CHECKID(chsk->skill.data[SKILL_IDENTITY])
            && chsk->skill.data[SKILL_LEVEL]
            < SKILL_tbl[chsk->skill.data[SKILL_IDENTITY]].maxlevel ){
            char    tmpbuf[512];
            snprintf( tmpbuf,sizeof(tmpbuf),"%d|",
                      chsk->skill.data[SKILL_IDENTITY] );
            strcatsafe( buf, buflen,tmpbuf );
        }
    }
    dchop(buf,"|");
    return TRUE;
}

void SKILL_skillEffect( int charaindex )
{
    int i;
    if( !CHAR_CHECKINDEX(charaindex) )return;

    if( CHAR_getInt(charaindex,CHAR_WHICHTYPE) != CHAR_TYPEPLAYER)
        return;
        
    for( i=0 ; i<CHAR_SKILLMAXHAVE ; i ++ ){
        typedef void (*SKILLEFFECTFUNC)(int,Skill*);
        SKILLEFFECTFUNC skfunc;
        CHAR_HaveSkill* cskill;
        Skill*          skill;
        int             id;

        cskill = CHAR_getCharHaveSkill(charaindex,i);
        if( cskill == NULL  || cskill->use == FALSE )continue;

        skill = &cskill->skill;
        id = skill->data[SKILL_IDENTITY];
        if( !SKILL_CHECKID(id) )continue;
        
        skfunc = (SKILLEFFECTFUNC)SKILL_tbl[id].effectfunc;
        if( skfunc )skfunc(charaindex,skill);
    }    
}

