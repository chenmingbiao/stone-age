#include "version.h"
#include <string.h> /* strstr */
#include <ctype.h>  /* isdigit, isalnum */
#include "common.h"
#include "char.h"
#include "char_base.h"
#include "char_data.h"
#include "configfile.h"
#include "item.h"
#include "item_event.h"
#include "object.h"
#include "anim_tbl.h"
#include "magic.h"
#include "handletime.h"
#include "log.h"
#include "util.h"
#include "encount.h"
#include "battle.h"
#include "battle_item.h"
#include "lssproto_serv.h"
#include "net.h"
#include "pet.h"
#include "npcutil.h"
#include "enemy.h"
#ifdef _Item_ReLifeAct
#include "battle_magic.h"
#endif
#ifdef _ITEM_WARP_FIX_BI
extern	tagRidePetTable	ridePetTable[296];
#endif
#include "pet_skill.h"

#ifdef _CFREE_petskill
#include "npc_freepetskillshop.h"
#endif

int ITEM_TimeDelCheck( int itemindex )
{
	int icnt, jcnt, playernum;
	playernum = CHAR_getPlayerMaxNum();
	for( icnt = 0; icnt < playernum; icnt ++ ) {
		if( CHAR_CHECKINDEX( icnt ) ) {
			for( jcnt = 0; jcnt < CHAR_MAXITEMHAVE; jcnt ++ ) {
				if( CHAR_getItemIndex( icnt, jcnt ) == itemindex ) {
					print( "道具效力时间已到，进行奇怪道具处理(%s)(%s)\n",
						CHAR_getUseName(icnt),ITEM_getAppropriateName(itemindex) );
					return FALSE;
				}
			}
		}
	}
	return TRUE;
}

int ITEM_eventDrop( int itemindex, int charaindex, int itemcharaindex )
{
	typedef void (*DROPF)(int,int);
	char szBuffer[256]="";
	DROPF dropfunc=NULL;

    if( ITEM_CHECKINDEX(itemindex) == FALSE )return -1;

	dropfunc=(DROPF)ITEM_getFunctionPointer( itemindex, ITEM_DROPFUNC );
	if( dropfunc ){
		dropfunc( charaindex, itemindex );
	}

    if( ITEM_getInt( itemindex, ITEM_VANISHATDROP) != 1 )return 0;
	snprintf( szBuffer, sizeof( szBuffer), "%s 消灭了。",
		ITEM_getAppropriateName(itemindex) );
	CHAR_talkToCli( charaindex, -1, szBuffer, CHAR_COLORWHITE );

	{
		LogItem(
			CHAR_getChar( charaindex, CHAR_NAME ), /* 平乓仿   */
			CHAR_getChar( charaindex, CHAR_CDKEY ),
#ifdef _add_item_log_name  // WON ADD 在item的log中增加item名称
			itemindex,
#else
       		ITEM_getInt( itemindex, ITEM_ID ),  /* 失奶  丞  寞 */
#endif
			"Drop&Delete(丢出後消失)",
			CHAR_getInt( charaindex,CHAR_FLOOR),
			CHAR_getInt( charaindex,CHAR_X ),
 	      	CHAR_getInt( charaindex,CHAR_Y ),
            ITEM_getChar( itemindex, ITEM_UNIQUECODE),
						ITEM_getChar( itemindex, ITEM_NAME),
						ITEM_getInt( itemindex, ITEM_ID)
		);
	}
	CHAR_setItemIndex( charaindex, itemcharaindex, -1);
	ITEM_endExistItemsOne( itemindex );
	return 1;
}

#undef  UNDEF
#define UNDEF (-1)

typedef struct {
    char* cmd;        /* 摹    侬   */
    char*   onmessage;    /*  丢永本□斥  侬      */
    char*   offmessage;    /*  丢永本□斥  侬      */
    int   element;    /* 覆擂卞卅月邰豳 */
    int   maxElement; /* element匹隙烂今木凶邰豳及    袄毛手勾邰豳 */
} ITEM_EFFECTPARAM;
static ITEM_EFFECTPARAM ITEM_restorableParam[] = {
    {"hp", "HP回复了。", "HP降低了。",CHAR_HP, CHAR_WORKMAXHP},
    {"mp", "MP回复了。", "MP降低了。",CHAR_MP, CHAR_WORKMAXMP},
};
static ITEM_EFFECTPARAM ITEM_statusParam[] = {
    {"po", CHAR_POISONSTRING, CHAR_RECOVERPOISONSTRING,
     CHAR_POISON,    UNDEF},

    {"pa", CHAR_PARALYSISSTRING, CHAR_RECOVERPARALYSISSTRING,
     CHAR_PARALYSIS, UNDEF},

    {"si", CHAR_SILENCESTRING, CHAR_RECOVERSILENCESTRING,
     CHAR_SLEEP,UNDEF},

    {"st", CHAR_STONESTRING, CHAR_RECOVERSTONESTRING, CHAR_STONE, UNDEF},

    {"da", CHAR_DARKNESSSTRING, CHAR_RECOVERDARKNESSSTRING,
     CHAR_DRUNK,UNDEF},

    {"co", CHAR_CONFUSIONSTRING,CHAR_RECOVERCONFUSIONSTRING,
     CHAR_CONFUSION, UNDEF},
};

/*#define LOCAL_DEBUG*/
#ifdef LOCAL_DEBUG
#define DOUTFILE  "doutfile"
#include <stdio.h>
#include <stdarg.h>
static int eprintf(char* format, ...){
    va_list arg;
    long len;
    FILE* fp;
    fp = fopen(DOUTFILE, "a");
    va_start(arg, format);
    len = vfprintf(stderr, format, arg);
    if(fp){ vfprintf(fp, format, arg); fclose(fp); }
    va_end(arg);
    return len;
}

#define fprint eprintf
#endif

static BOOL ITEM_isValidEffect(char* cmd, int value){
    int i;

    for(i=0; i<arraysizeof(ITEM_restorableParam); i++){
        if(! strcmp(cmd, ITEM_restorableParam[i].cmd)){
            return value <= 0 ? FALSE : TRUE;
        }
    }
    if(value < 0 ) return FALSE;
    for(i=0; i<arraysizeof(ITEM_statusParam); i++){
        if(! strcmp(cmd, ITEM_statusParam[i].cmd)){
            return TRUE;
        }
    }
    return FALSE;
}

#define ID_BUF_LEN_MAX  20
#define SEPARATORI       '|'
BOOL ITEM_MedicineInit(ITEM_Item* itm)
{
    char cmd[ID_BUF_LEN_MAX], arg[ID_BUF_LEN_MAX];
    int value;
    int effectCount = 0;
    char* p, * q;
    char* effectarg;

    effectarg = itm->string[ITEM_ARGUMENT].string;
    if(* effectarg == '\0') return TRUE;
    for(p=effectarg; *p != '\0'; ){
        int i;
        if(*p == SEPARATORI) p++;
        for(q=cmd, i=0; isalnum(*p) && i<ID_BUF_LEN_MAX; i++){
            *q++ = *p++;
        }
        *q = '\0';
        if(q == cmd || *p != SEPARATORI){
            fprint("ITEM_medicineInit: error(c)? invalid:%s(%d)\n",
                   effectarg, effectCount);
            return FALSE;
        }
        p++;
        for(q=arg, i=0; isdigit(*p) && i<ID_BUF_LEN_MAX; i++){
            *q++ = *p++;
        }
        *q = '\0';
        value = strtol(arg, & q, 10); /* strtol()反OK? */
        if(ITEM_isValidEffect(cmd, value)){
            effectCount++;
        }else{
            fprint("ITEM_medicineInit: error(v)? invalid:%s(%d)\n",
                   effectarg, effectCount);
            return FALSE;
        }
    }
    return (effectCount == 0) ? FALSE : TRUE;
}

static BOOL ITEM_medicineRaiseEffect(int charaindex, char* cmd,int value)
{
    int i;
    char ansmsg[256];
    for( i=0 ; i<arraysizeof(ITEM_restorableParam); i++){
        if( ! strcmp(cmd, ITEM_restorableParam[i].cmd) ){
#if 1
            int maxv,curv,amount;
            char*   onoroff=NULL;
            maxv=CHAR_getWorkInt( charaindex, ITEM_restorableParam[i].maxElement);
            curv=CHAR_getInt(charaindex,ITEM_restorableParam[i].element);
            amount = value;
            if( curv + amount < 0 ) amount = -curv;
            CHAR_setInt(charaindex,ITEM_restorableParam[i].element, min((curv+amount),maxv));
            if( amount > 0 )
                onoroff=ITEM_restorableParam[i].onmessage;
            else if( amount < 0 )
                onoroff=ITEM_restorableParam[i].offmessage;
            else
                return FALSE;
            strcpysafe( ansmsg, sizeof(ansmsg), onoroff );
            CHAR_talkToCli( charaindex, -1, ansmsg, CHAR_COLORWHITE);
            return TRUE;
#else
            if(value == ITEM_EFFECT_RESTORE_NORMAL){
                int maxv, curv, amount;
                maxv =CHAR_getWorkInt(charaindex, ITEM_restorableParam[i].maxElement);
                curv =CHAR_getInt(charaindex, ITEM_restorableParam[i].element);
                amount = 30;
                if(curv + amount < 0) amount = -curv;
                CHAR_setInt(charaindex, ITEM_restorableParam[i].element, min((curv+amount), maxv));
                snprintf( ansmsg, sizeof(ansmsg), "%s回复了。",
                          ITEM_restorableParam[i].onmessage );
                CHAR_talkToCli( charaindex, -1, ansmsg, CHAR_COLORWHITE);
                return TRUE;
            }else if(value == ITEM_EFFECT_RESTORE_COMPLETE){
                int maxv;
                maxv =CHAR_getWorkInt(charaindex, ITEM_restorableParam[i].maxElement);
                CHAR_setInt(charaindex, ITEM_restorableParam[i].element, maxv);
                snprintf( ansmsg, sizeof(ansmsg), "%s已完全回复。", ITEM_restorableParam[i].onmessage );
                CHAR_talkToCli( charaindex, -1, ansmsg, CHAR_COLORWHITE );
                return TRUE;
            }else
                return FALSE;
#endif

        }
    }
    if( value >= 0 ){
        int found = 0;
        for(i=0; i<arraysizeof(ITEM_statusParam); i++){
            if(! strcmp(cmd, ITEM_statusParam[i].cmd)){
                ansmsg[0] = '\0';
                if( value && CHAR_getInt(charaindex,ITEM_statusParam[i].element) < value ){
                    CHAR_setInt(charaindex, ITEM_statusParam[i].element, value);
                    strcpysafe( ansmsg, sizeof(ansmsg), ITEM_statusParam[i].onmessage );
                }else{
                    if( CHAR_getInt(charaindex, ITEM_statusParam[i].element ) ){ 
						strcpysafe( ansmsg, sizeof(ansmsg), ITEM_statusParam[i].offmessage );
                    }
                    CHAR_setInt(charaindex, ITEM_statusParam[i].element, 0 );
                }
                found = 1;
                if( ansmsg[0] != '\0' )
                    CHAR_talkToCli( charaindex,-1,ansmsg,CHAR_COLORWHITE);
            }
        }
        if(found){
            CHAR_sendCToArroundCharacter( CHAR_getWorkInt(charaindex,CHAR_WORKOBJINDEX) );
            return TRUE;
        }
    }
#undef ITEM_STATUSCHANGEVALUE
    strcpysafe( ansmsg, sizeof(ansmsg),"什麽也没发生。");
    CHAR_talkToCli( charaindex, -1, ansmsg, CHAR_COLORWHITE );
    return FALSE;
}

void ITEM_MedicineUsed(int charaindex, int to_charaindex, int itemindex)
{
    int itemid;
    int usedf = 0;
    char cmd[ID_BUF_LEN_MAX], arg[ID_BUF_LEN_MAX];
    int value;
    char* p, * q;
    char* effectarg;
    char ansmsg[256];
    itemid = CHAR_getItemIndex(charaindex, itemindex);
    if(!ITEM_CHECKINDEX(itemid)) return;
    effectarg = ITEM_getChar(itemid, ITEM_ARGUMENT);
    snprintf( ansmsg, sizeof(ansmsg), "抓到了%s 。", ITEM_getChar(itemid, ITEM_NAME) );
    CHAR_talkToCli( charaindex, -1, ansmsg, CHAR_COLORWHITE );
    for( p=effectarg ; *p != '\0'; ){
        int i;
        if(*p == SEPARATORI) p++;
        for(q=cmd, i=0; isalnum(*p) && i<ID_BUF_LEN_MAX; i++){
            *q++ = *p++;
        }
        *q = '\0';
        if(q == cmd || *p != SEPARATORI) return;
        p++;
        for(q=arg, i=0; isdigit(*p) && i<ID_BUF_LEN_MAX; i++){
            *q++ = *p++;
        }
        *q = '\0';
        value = strtol(arg, & q, 10);
        if(ITEM_medicineRaiseEffect(charaindex, cmd, value)){
            usedf = 1;
        }
    }
    if(usedf){
		CHAR_DelItem( charaindex, itemindex);
        CHAR_sendStatusString(charaindex, "P");

    }else
        fprint("ITEM_medicineUsed: error? cannot be used.\n");
#undef ID_BUF_LEN_MAX
#undef SEPARATORI
}

void ITEM_SandClockDetach( int charaindex , int itemid )
{
    int     i;
    if( !ITEM_CHECKINDEX( itemid )) return;
    for( i=0 ; i<CHAR_MAXITEMHAVE ; i++ ){
        if( CHAR_getItemIndex(charaindex,i ) == itemid ){
			CHAR_DelItem( charaindex, i);
            CHAR_talkToCli( charaindex, -1, "一卸下沙漏，竟忽然坏了！", CHAR_COLORWHITE );
            print( "deleted sand clock!\n" );
            break;
        }
    }
}

void ITEM_SandClockLogin( int charaindex )
{
	int i;
	int dTime;

	for( i=0 ; i<CHAR_MAXITEMHAVE; i++ ){
		int itemindex = CHAR_getItemIndex(charaindex,i);
		if( ITEM_getInt( itemindex, ITEM_ID ) != 29 )continue;
		if( ITEM_getInt( itemindex, ITEM_VAR4 ) == 0 )continue;
		dTime = NowTime.tv_sec - ITEM_getInt( itemindex, ITEM_VAR4 );
		ITEM_setInt( itemindex, ITEM_VAR3, ITEM_getInt( itemindex, ITEM_VAR3 ) + dTime );
	}

}

void ITEM_SandClockLogout( int charaindex )
{
	int i;
	for( i=0 ; i<CHAR_MAXITEMHAVE; i++ ){
		int itemindex = CHAR_getItemIndex(charaindex,i);
		if( ITEM_getInt( itemindex, ITEM_ID ) != 29 )continue;
		ITEM_setInt( itemindex, ITEM_VAR4, NowTime.tv_sec );
	}
}

// Arminius 7.2: Ra's amulet , remove "static"
/*static*/
BOOL ITEM_getArgument( char* argument , char* entryname, char* buf , int buflen )
{
    int     i;
    char    dividedbypipeline[512];
    for( i=1;  ; i++ ){
        BOOL   ret;
		ret = getStringFromIndexWithDelim( argument, "|", i, dividedbypipeline,
                                           sizeof(dividedbypipeline) );
        if( ret == TRUE ){
            int     tworet=1;
            char    first[512];
            tworet &= getStringFromIndexWithDelim( dividedbypipeline, ":", 1, first,sizeof(first) );
            tworet &= getStringFromIndexWithDelim( dividedbypipeline, ":", 2, buf,buflen );
            if( tworet != 0 )
                if( strcasecmp( first, entryname ) == 0 )
                    return TRUE;
        }else
            break;
    }

    return FALSE;
}

void ITEM_addTitleAttach( int charaindex, int itemindex )
{
    char    titlenumstring[256];
    int     titleindex;

    if( ITEM_CHECKINDEX(itemindex) == FALSE )return;

    if( ITEM_getArgument( ITEM_getChar(itemindex,ITEM_ARGUMENT),"addt",
                          titlenumstring, sizeof( titlenumstring) ) == FALSE ){
        print( "Can't find \"addt\" entry: %s\n",
               ITEM_getChar(itemindex,ITEM_ARGUMENT));
        return;
    }
    titleindex = atoi( titlenumstring );
    TITLE_addtitle( charaindex, titleindex );
    CHAR_sendStatusString( charaindex, "T" );
}

void ITEM_delTitleDetach( int charaindex, int itemindex )
{
    char    titlenumstring[256];
    int     titleindex;

    if( ITEM_CHECKINDEX(itemindex) == FALSE )return;

    if( ITEM_getArgument( ITEM_getChar(itemindex,ITEM_ARGUMENT),"delt",
                          titlenumstring, sizeof( titlenumstring) ) == FALSE ){
        print( "Can't find \"delt\" entry: %s\n",
               ITEM_getChar(itemindex,ITEM_ARGUMENT));
        return;
    }
    titleindex = atoi( titlenumstring );
    TITLE_deltitle( charaindex, titleindex );
    CHAR_sendStatusString( charaindex, "T" );
}

void ITEM_DeleteByWatched(
	int myobjindex, int moveobjindex, CHAR_ACTION act,
    int x, int y, int dir, int* opt, int optlen )
{

	int		itemindex, moveindex;
	char szBuffer[256]="";
	itemindex = OBJECT_getIndex(myobjindex);
    if( !ITEM_CHECKINDEX( itemindex )) return;
	if( OBJECT_getType( moveobjindex ) == OBJTYPE_CHARA ){
		moveindex = OBJECT_getIndex(moveobjindex);
    	if( CHAR_getInt( moveindex , CHAR_WHICHTYPE ) == CHAR_TYPEPLAYER ){
    		snprintf( szBuffer, sizeof( szBuffer ), "%s",
    							ITEM_getAppropriateName(itemindex) );
            CHAR_talkToCli( moveindex	, -1, "%s 消灭了。", CHAR_COLORWHITE );
    	}
    }

	ITEM_endExistItemsOne(itemindex);
	CHAR_ObjectDelete(myobjindex);

}

void ITEM_DeleteTimeWatched(
	int objindex, int moveobjindex, CHAR_ACTION act,
    int x, int y, int dir, int* opt, int optlen)
{
	int	itemindex;
	int itemputtime;

	if( !CHECKOBJECTUSE(objindex) ){
		return;
	}
	itemindex = OBJECT_getIndex(objindex);
	if(!ITEM_CHECKINDEX(itemindex)){
		return;
	}
	itemputtime=ITEM_getInt(itemindex,ITEM_PUTTIME);
	if( !ITEM_CHECKINDEX( itemindex )) return;
	if( (int)NowTime.tv_sec > (int)( itemputtime + getItemdeletetime() ) ) {
		if( ITEM_TimeDelCheck( itemindex ) == FALSE ){
			return ;
		}
		{
			LogItem(
					"NULL",
					"NULL",
#ifdef _add_item_log_name  // WON ADD 在item的log中增加item名称
					itemindex,
#else
		       		ITEM_getInt( itemindex, ITEM_ID ),
#endif
					"TiemDelete",
					OBJECT_getFloor( objindex ),
					OBJECT_getX( objindex ),
					OBJECT_getY( objindex ),
					ITEM_getChar( itemindex, ITEM_UNIQUECODE),
						ITEM_getChar( itemindex, ITEM_NAME),
						ITEM_getInt( itemindex, ITEM_ID)
				);
		}
		ITEM_endExistItemsOne(itemindex);
		CHAR_ObjectDelete(objindex);
	}
}

void ITEM_useEffectTohelos( int charaindex, int to_charaindex, int haveitemindex)
{
	char	buf[64];
	char	msgbuf[64];
	int		ret;
	int		itemindex;
	int		cutrate, limitcount;
    int		per;
    int		sendcharaindex = charaindex;

    itemindex = CHAR_getItemIndex( charaindex, haveitemindex);
    if(!ITEM_CHECKINDEX(itemindex)) return;

    CHAR_setItemIndex(charaindex, haveitemindex ,-1);
    CHAR_sendItemDataOne( charaindex, haveitemindex);
	ret = getStringFromIndexWithDelim( ITEM_getChar(itemindex, ITEM_ARGUMENT) ,
										"|", 1, buf, sizeof( buf));
	if( ret != TRUE ) {
		{
			LogItem(
				CHAR_getChar( charaindex, CHAR_NAME ),
				CHAR_getChar( charaindex, CHAR_CDKEY ),
#ifdef _add_item_log_name  // WON ADD 在item的log中增加item名称
				itemindex,
#else
       			ITEM_getInt( itemindex, ITEM_ID ),
#endif
				"FieldErrorUse",
		       	CHAR_getInt( charaindex,CHAR_FLOOR),
				CHAR_getInt( charaindex,CHAR_X ),
        		CHAR_getInt( charaindex,CHAR_Y ),
				ITEM_getChar( itemindex, ITEM_UNIQUECODE),
						ITEM_getChar( itemindex, ITEM_NAME),
						ITEM_getInt( itemindex, ITEM_ID)
			);
		}
	    ITEM_endExistItemsOne( itemindex);
		return;
	}
	cutrate = atoi( buf);
	if( cutrate < 0 ) cutrate = 0;
	ret = getStringFromIndexWithDelim( ITEM_getChar(itemindex, ITEM_ARGUMENT) ,
										"|", 2, buf, sizeof( buf));
	if( ret != TRUE ) {
		{
			LogItem(
				CHAR_getChar( charaindex, CHAR_NAME ),
				CHAR_getChar( charaindex, CHAR_CDKEY ),
#ifdef _add_item_log_name  // WON ADD 在item的log中增加item名称
				itemindex,
#else
       			ITEM_getInt( itemindex, ITEM_ID ),
#endif
				"FieldUse",
		       	CHAR_getInt( charaindex,CHAR_FLOOR),
				CHAR_getInt( charaindex,CHAR_X ),
        		CHAR_getInt( charaindex,CHAR_Y ),
				ITEM_getChar( itemindex, ITEM_UNIQUECODE),
						ITEM_getChar( itemindex, ITEM_NAME),
						ITEM_getInt( itemindex, ITEM_ID)
			);
		}
	    ITEM_endExistItemsOne( itemindex);
		return;
	}
	limitcount = atoi( buf);
	if( limitcount < 0) limitcount = 0;
	if( CHAR_getWorkInt( charaindex, CHAR_WORKPARTYMODE) == CHAR_PARTY_CLIENT) {
		sendcharaindex = CHAR_getWorkInt( charaindex, CHAR_WORKPARTYINDEX1);
	}
	CHAR_setWorkInt( sendcharaindex, CHAR_WORK_TOHELOS_CUTRATE, cutrate);
	CHAR_setWorkInt( sendcharaindex, CHAR_WORK_TOHELOS_COUNT, limitcount);

    snprintf( msgbuf, sizeof(msgbuf),
              "抓到了%s 。", ITEM_getChar(itemindex, ITEM_NAME) );
    CHAR_talkToCli( charaindex, -1, msgbuf, CHAR_COLORWHITE );

    if( sendcharaindex != charaindex ) {
	    snprintf( msgbuf, sizeof(msgbuf),
	              "%s 抓到了 %s。 ",
	              CHAR_getChar( charaindex, CHAR_NAME),
	              ITEM_getChar( itemindex, ITEM_NAME) );
	    CHAR_talkToCli( sendcharaindex, -1, msgbuf, CHAR_COLORWHITE );
	}

	{
		LogItem(
			CHAR_getChar( charaindex, CHAR_NAME ), 
			CHAR_getChar( charaindex, CHAR_CDKEY ),
#ifdef _add_item_log_name  // WON ADD 在item的log中增加item名称
			itemindex,
#else
       		ITEM_getInt( itemindex, ITEM_ID ),
#endif
			"FieldUse",
	       	CHAR_getInt( charaindex,CHAR_FLOOR),
			CHAR_getInt( charaindex,CHAR_X ),
        	CHAR_getInt( charaindex,CHAR_Y ),
			ITEM_getChar( itemindex, ITEM_UNIQUECODE),
						ITEM_getChar( itemindex, ITEM_NAME),
						ITEM_getInt( itemindex, ITEM_ID)
		);
	}
	ITEM_endExistItemsOne( itemindex);
	per = ENCOUNT_getEncountPercentMin( sendcharaindex,
									CHAR_getInt( sendcharaindex, CHAR_FLOOR),
									CHAR_getInt( sendcharaindex, CHAR_X),
									CHAR_getInt( sendcharaindex, CHAR_Y));
	if( per != -1) {
		CHAR_setWorkInt( sendcharaindex, CHAR_WORKENCOUNTPROBABILITY_MIN, per);
	}
	per = ENCOUNT_getEncountPercentMax( sendcharaindex,
									CHAR_getInt( sendcharaindex, CHAR_FLOOR),
									CHAR_getInt( sendcharaindex, CHAR_X),
									CHAR_getInt( sendcharaindex, CHAR_Y));
	if( per != -1) {
		CHAR_setWorkInt( sendcharaindex, CHAR_WORKENCOUNTPROBABILITY_MAX, per);
	}
	CHAR_sendStatusString( sendcharaindex, "E" );
}

void ITEM_dropMic( int charaindex , int itemindex )
{
    if( !ITEM_CHECKINDEX( itemindex )) return;

	CHAR_setWorkInt( charaindex, CHAR_WORKFLG,
		CHAR_getWorkInt( charaindex, CHAR_WORKFLG ) & ~WORKFLG_MICMODE );
}

void ITEM_useMic_Field( int charaindex, int to_charaindex, int haveitemindex )
{
	if( CHAR_getWorkInt( charaindex, CHAR_WORKFLG ) & WORKFLG_MICMODE ){
		CHAR_setWorkInt( charaindex, CHAR_WORKFLG,
			CHAR_getWorkInt( charaindex, CHAR_WORKFLG ) & ~WORKFLG_MICMODE );
		CHAR_talkToCli( charaindex, -1, "将麦克风设定为OFF。", CHAR_COLORWHITE);
	}else{
		CHAR_setWorkInt( charaindex, CHAR_WORKFLG,
			CHAR_getWorkInt( charaindex, CHAR_WORKFLG ) | WORKFLG_MICMODE );
		CHAR_talkToCli( charaindex, -1, "将麦克风设定为ON。", CHAR_COLORWHITE);
	}
}
#if 1
char *aszHealStringByOwn[] = {
	"%s的耐久力回复%d",
	"%s的气力回复%d",
	"%s的魅力上升%d",
	"%s的忠诚度上升%d",
	""
};

char *aszDownStringByOwn[] = {
	"%s的耐久力减低%d",
	"%s的气力减低%d",
	"%s的魅力下降%d",
	"%s的忠诚度下降%d",
	""
};

char *aszHealStringByOther[] = {
	"藉由%s%s的耐久力回复%d",
	"藉由%s%s的气力回复%d",
	"藉由%s%s的魅力上升%d",
	"藉由%s%s的忠诚度上升%d",
	""
};

char *aszDownStringByOther[] = {
	"藉由%s%s的耐久力减低%d",
	"藉由%s%s的气力减低%d",
	"藉由%s%s的魅力减低%d",
	"藉由%s%s的忠诚度减低%d",
	""
};

char *aszKeyString[] = {"体", "气", "魅", "忠", ""};
int  aHealInt[] = { CHAR_HP, CHAR_MP, CHAR_CHARM, CHAR_VARIABLEAI, -1 };
int  aHealMaxWork[] = { CHAR_WORKMAXHP, CHAR_WORKMAXMP, -1, -1,  -1 };

void ITEM_useRecovery_Field(
	int charaindex,
	int toindex,
	int haveitemindex
){
	int work, workmax, workmin;
	int power[BD_KIND_END] = {0,0,0},
		prevhp = 0,
		workhp = 0,
		recovery[BD_KIND_END] = {0,0,0};
	int itemindex, kind = BD_KIND_HP, HealFlg = 0, j;
	char *p = NULL, *arg, msgbuf[256];
    itemindex = CHAR_getItemIndex( charaindex, haveitemindex);
    if( !ITEM_CHECKINDEX(itemindex) ) return;
	if( CHAR_CHECKINDEX( toindex ) == FALSE )return ;
	arg = ITEM_getChar(itemindex, ITEM_ARGUMENT );
	if( arg == "\0" ) return;

#ifdef _ITEM_UNBECOMEPIG
    if( (p = strstr( arg, "解猪" )) != NULL ){
        if( CHAR_getInt( toindex, CHAR_BECOMEPIG ) > -1 ){
		    CHAR_setInt( toindex, CHAR_BECOMEPIG, -1 );
			CHAR_complianceParameter( toindex );
		    CHAR_sendCToArroundCharacter( CHAR_getWorkInt( toindex , CHAR_WORKOBJINDEX ));
		    CHAR_send_P_StatusString( toindex , CHAR_P_STRING_BASEBASEIMAGENUMBER);
		    CHAR_talkToCli( toindex,-1,"乌力化失效了。",CHAR_COLORWHITE);
       		CHAR_DelItemMess( charaindex, haveitemindex, 0);
		}
		return;
	}
#endif
#ifdef _ITEM_LVUPUP
	if( (p = strstr( arg, "LVUPUP" ) ) != NULL ){
		if (CHAR_getInt(toindex, CHAR_WHICHTYPE)==CHAR_TYPEPET){
		    int pidx=0,lvup=0,lv=0;
			sprintf( msgbuf, "%s", p+7 );
            pidx = atoi( strtok( msgbuf, " " ) );
			if( pidx != CHAR_getInt( toindex, CHAR_PETID) || CHAR_getInt(toindex, CHAR_LIMITLEVEL) == 0 ){
			    CHAR_talkToCli( charaindex,-1,"无法使用",CHAR_COLORWHITE);
				return;
			}
			lv = CHAR_getInt( toindex, CHAR_LV );
			if( lv < 10 || lv >= 140 ){
			    CHAR_talkToCli( charaindex,-1,"目前等级无法使用",CHAR_COLORWHITE);
				return;
			}
			if( (p = strstr( arg, "地" )) != NULL )
					lvup=2;
		    if( (p = strstr( arg, "水" )) != NULL )
					lvup=3;
            if( (p = strstr( arg, "火" )) != NULL )
					lvup=0;
			if( (p = strstr( arg, "风" )) != NULL )
					lvup=1;		    
			if( lv%4 != lvup || CHAR_getInt(toindex, CHAR_LIMITLEVEL)-lv >= 1 ){//
			    CHAR_talkToCli( charaindex,-1,"破除封印失败",CHAR_COLORWHITE);
			    CHAR_DelItemMess( charaindex, haveitemindex, 0);
				return;
			}
			CHAR_setInt(toindex, CHAR_LIMITLEVEL, CHAR_getInt(toindex, CHAR_LIMITLEVEL)+1);	
			CHAR_talkToCli( charaindex,-1,"封印魔咒减弱，最高等级提升",CHAR_COLORWHITE);    
			if( CHAR_getInt(toindex, CHAR_LIMITLEVEL) == 140 ){
				CHAR_setInt(toindex, CHAR_LIMITLEVEL, 0);
			    CHAR_talkToCli( charaindex,-1,"封印魔咒解除了",CHAR_COLORWHITE);    	
			}
			CHAR_DelItemMess( charaindex, haveitemindex, 0);
			CHAR_complianceParameter( toindex );
		    CHAR_sendCToArroundCharacter( CHAR_getWorkInt( toindex , CHAR_WORKOBJINDEX ));
		}
		return;
	}
	if( (p = strstr( arg, "UPUPLV" ) ) != NULL ){
		if (CHAR_getInt(toindex, CHAR_WHICHTYPE)==CHAR_TYPEPET){
		    int pidx=0,lv=0;
			sprintf( msgbuf, "%s", p+7 );
			pidx = atoi( strtok( msgbuf, " " ) );
			if( pidx != CHAR_getInt( toindex, CHAR_PETID) || CHAR_getInt(toindex, CHAR_LIMITLEVEL) == 0 ){
			    CHAR_talkToCli( charaindex,-1,"无法使用",CHAR_COLORWHITE);
				return;
			}
			lv = CHAR_getInt( toindex, CHAR_LV );
			if( lv < 125 || lv > 140 ){
			    CHAR_talkToCli( charaindex,-1,"目前等级无法使用",CHAR_COLORWHITE);
				return;
			}
			CHAR_setInt(toindex, CHAR_LIMITLEVEL, 0);
			CHAR_talkToCli( charaindex,-1,"封印魔咒解除了",CHAR_COLORWHITE);    	
			CHAR_DelItemMess( charaindex, haveitemindex, 0);
			CHAR_complianceParameter( toindex );
		    CHAR_sendCToArroundCharacter( CHAR_getWorkInt( toindex , CHAR_WORKOBJINDEX ));
		}
		return;
	}
#endif
#ifdef _ITEM_PROPERTY
    if( (p = strstr( arg, "PROPERTY" ) ) != NULL ){
		//print("地水火风:%s", arg );
		if (CHAR_getInt(toindex, CHAR_WHICHTYPE)!=CHAR_TYPEPLAYER)
            return;
		if( (p = strstr( arg, "+" ) ) != NULL ){//旋转属性 正转
	        if( CHAR_getInt( toindex, CHAR_EARTHAT ) == 100 )
                CHAR_setInt( toindex, CHAR_EARTHAT, CHAR_getInt( toindex, CHAR_EARTHAT )-10 ),
			    CHAR_setInt( toindex, CHAR_WATERAT, CHAR_getInt( toindex, CHAR_WATERAT )+10 );
		    else if( CHAR_getInt( toindex, CHAR_WATERAT ) == 100 )
                CHAR_setInt( toindex, CHAR_WATERAT, CHAR_getInt( toindex, CHAR_WATERAT )-10 ),
			    CHAR_setInt( toindex, CHAR_FIREAT, CHAR_getInt( toindex, CHAR_FIREAT )+10 );
            else if( CHAR_getInt( toindex, CHAR_FIREAT ) == 100 )
                CHAR_setInt( toindex, CHAR_FIREAT, CHAR_getInt( toindex, CHAR_FIREAT )-10 ),
			    CHAR_setInt( toindex, CHAR_WINDAT, CHAR_getInt( toindex, CHAR_WINDAT )+10 );
            else if( CHAR_getInt( toindex, CHAR_WINDAT ) == 100 )
                CHAR_setInt( toindex, CHAR_WINDAT, CHAR_getInt( toindex, CHAR_WINDAT )-10 ),
			    CHAR_setInt( toindex, CHAR_EARTHAT, CHAR_getInt( toindex, CHAR_EARTHAT )+10 );
			else if( CHAR_getInt( toindex, CHAR_EARTHAT ) > 0 && CHAR_getInt( toindex, CHAR_WATERAT ) > 0 )
                CHAR_setInt( toindex, CHAR_EARTHAT, CHAR_getInt( toindex, CHAR_EARTHAT )-10 ),
			    CHAR_setInt( toindex, CHAR_WATERAT, CHAR_getInt( toindex, CHAR_WATERAT )+10 );
            else if( CHAR_getInt( toindex, CHAR_WATERAT ) > 0 && CHAR_getInt( toindex, CHAR_FIREAT ) > 0 )
                CHAR_setInt( toindex, CHAR_WATERAT, CHAR_getInt( toindex, CHAR_WATERAT )-10 ),
			    CHAR_setInt( toindex, CHAR_FIREAT, CHAR_getInt( toindex, CHAR_FIREAT )+10 );
		    else if( CHAR_getInt( toindex, CHAR_FIREAT ) > 0 && CHAR_getInt( toindex, CHAR_WINDAT ) > 0 )
                CHAR_setInt( toindex, CHAR_FIREAT, CHAR_getInt( toindex, CHAR_FIREAT )-10 ),
			    CHAR_setInt( toindex, CHAR_WINDAT, CHAR_getInt( toindex, CHAR_WINDAT )+10 );
            else if( CHAR_getInt( toindex, CHAR_WINDAT ) > 0 && CHAR_getInt( toindex, CHAR_EARTHAT ) > 0 )
                CHAR_setInt( toindex, CHAR_WINDAT, CHAR_getInt( toindex, CHAR_WINDAT )-10 ),
			    CHAR_setInt( toindex, CHAR_EARTHAT, CHAR_getInt( toindex, CHAR_EARTHAT )+10 );
		}
        if( (p = strstr( arg, "-" ) ) != NULL ){//旋转属性 反转
			if( CHAR_getInt( toindex, CHAR_EARTHAT ) == 100 )
                CHAR_setInt( toindex, CHAR_EARTHAT, CHAR_getInt( toindex, CHAR_EARTHAT )-10 ),
			    CHAR_setInt( toindex, CHAR_WINDAT, CHAR_getInt( toindex, CHAR_WINDAT )+10 );
		    else if( CHAR_getInt( toindex, CHAR_WATERAT ) == 100 )
                CHAR_setInt( toindex, CHAR_WATERAT, CHAR_getInt( toindex, CHAR_WATERAT )-10 ),
			    CHAR_setInt( toindex, CHAR_EARTHAT, CHAR_getInt( toindex, CHAR_EARTHAT )+10 );
            else if( CHAR_getInt( toindex, CHAR_FIREAT ) == 100 )
                CHAR_setInt( toindex, CHAR_FIREAT, CHAR_getInt( toindex, CHAR_FIREAT )-10 ),
			    CHAR_setInt( toindex, CHAR_WATERAT, CHAR_getInt( toindex, CHAR_WATERAT )+10 );
            else if( CHAR_getInt( toindex, CHAR_WINDAT ) == 100 )
                CHAR_setInt( toindex, CHAR_WINDAT, CHAR_getInt( toindex, CHAR_WINDAT )-10 ),
			    CHAR_setInt( toindex, CHAR_FIREAT, CHAR_getInt( toindex, CHAR_FIREAT )+10 );
		    else if( CHAR_getInt( toindex, CHAR_EARTHAT ) > 0 && CHAR_getInt( toindex, CHAR_WATERAT ) > 0 )
                CHAR_setInt( toindex, CHAR_EARTHAT, CHAR_getInt( toindex, CHAR_EARTHAT )+10 ),
			    CHAR_setInt( toindex, CHAR_WATERAT, CHAR_getInt( toindex, CHAR_WATERAT )-10 );
            else if( CHAR_getInt( toindex, CHAR_WATERAT ) > 0 && CHAR_getInt( toindex, CHAR_FIREAT ) > 0 )
                CHAR_setInt( toindex, CHAR_WATERAT, CHAR_getInt( toindex, CHAR_WATERAT )+10 ),
			    CHAR_setInt( toindex, CHAR_FIREAT, CHAR_getInt( toindex, CHAR_FIREAT )-10 );
		    else if( CHAR_getInt( toindex, CHAR_FIREAT ) > 0 && CHAR_getInt( toindex, CHAR_WINDAT ) > 0 )
                CHAR_setInt( toindex, CHAR_FIREAT, CHAR_getInt( toindex, CHAR_FIREAT )+10 ),
			    CHAR_setInt( toindex, CHAR_WINDAT, CHAR_getInt( toindex, CHAR_WINDAT )-10 );
            else if( CHAR_getInt( toindex, CHAR_WINDAT ) > 0 && CHAR_getInt( toindex, CHAR_EARTHAT ) > 0 )
                CHAR_setInt( toindex, CHAR_WINDAT, CHAR_getInt( toindex, CHAR_WINDAT )+10 ),
			    CHAR_setInt( toindex, CHAR_EARTHAT, CHAR_getInt( toindex, CHAR_EARTHAT )-10 );
		}
		CHAR_DelItemMess( charaindex, haveitemindex, 0);
		CHAR_complianceParameter( toindex );
		CHAR_sendCToArroundCharacter( CHAR_getWorkInt( toindex , CHAR_WORKOBJINDEX ));
		CHAR_send_P_StatusString( toindex ,
					CHAR_P_STRING_EARTH	|
					CHAR_P_STRING_WATER	|
					CHAR_P_STRING_FIRE	|
					CHAR_P_STRING_WIND	
				);
		return;
	}
#endif
#ifdef _ITEM_ADDPETEXP
	if( (p = strstr( arg, "GETEXP" )) != NULL ){
		if (CHAR_getInt(toindex, CHAR_WHICHTYPE)==CHAR_TYPEPET){
			getStringFromIndexWithDelim( arg, "|", 2, msgbuf,sizeof( msgbuf));//宠物编号
			if( atoi(msgbuf) == CHAR_getInt( toindex, CHAR_PETID) ){
				getStringFromIndexWithDelim( arg, "|", 3, msgbuf,sizeof( msgbuf));//等级上限(等级超过才可用)
				if( CHAR_getInt( toindex, CHAR_LV ) >= atoi(msgbuf) ){
					getStringFromIndexWithDelim( arg, "|", 4, msgbuf,sizeof( msgbuf));//经验值增加
					if( CHAR_getInt( toindex, CHAR_LV) < CHAR_MAXUPLEVEL ){
						int UpLevel = 0;
						CHAR_setWorkInt( toindex, CHAR_WORKGETEXP, atoi(msgbuf) );//回存CHAR_WORKGETEXP
						CHAR_AddMaxExp( toindex, CHAR_getWorkInt( toindex, CHAR_WORKGETEXP ) );
						sprintf( msgbuf,"这石头里面不知道有什麽样的矿，让我看看。(增加经验%d)", CHAR_getWorkInt( toindex, CHAR_WORKGETEXP ) );
						CHAR_talkToCli( charaindex,-1,msgbuf,CHAR_COLORWHITE);  
						UpLevel = CHAR_LevelUpCheck( toindex , charaindex);
						if( UpLevel > 0 ){
							if( getBattleDebugMsg( ) != 0 ){
								snprintf( msgbuf, sizeof(msgbuf),
											"(%s) 升级至 %d",
											CHAR_getUseName( toindex ),
											CHAR_getInt( toindex, CHAR_LV ) );
							BATTLE_talkToCli( charaindex, msgbuf, CHAR_COLORYELLOW );
							}
						}
						for( j = 0; j < UpLevel; j ++ ){
							CHAR_PetLevelUp( toindex );
							CHAR_PetAddVariableAi( toindex, AI_FIX_PETLEVELUP );
						}
						CHAR_complianceParameter( toindex );
					}
					else{
						CHAR_talkToCli( charaindex,-1,"不能使用",CHAR_COLORWHITE);  
					}
					/*CHAR_send_P_StatusString(  charindex, CHAR_P_STRING_DUELPOINT|
					CHAR_P_STRING_TRANSMIGRATION| CHAR_P_STRING_RIDEPET|
					CHAR_P_STRING_BASEBASEIMAGENUMBER| CHAR_P_STRING_GOLD|
					CHAR_P_STRING_EXP| CHAR_P_STRING_LV| CHAR_P_STRING_HP|CHAR_P_STRING_LEARNRIDE);
				*/
				}
				else
					CHAR_talkToCli( charaindex,-1,"这石头....(无法判别，无法增加经验)",CHAR_COLORWHITE);  
			}
			CHAR_DelItemMess( charaindex, haveitemindex, 0);
			CHAR_complianceParameter( toindex );
			CHAR_sendCToArroundCharacter( CHAR_getWorkInt( toindex , CHAR_WORKOBJINDEX ));
			return;
		}
		else{
			CHAR_talkToCli( charaindex,-1,"不能使用",CHAR_COLORWHITE);  
		}
	}
#endif
	for( j = 0; j < BD_KIND_END; j ++ ){
		power[j] = 0;
		recovery[j] = 0;
	}
	if( (p = strstr( arg, "全" )) != NULL ){
		HealFlg = ( 1 << BD_KIND_HP );
		if( CHAR_getInt( toindex, CHAR_WHICHTYPE) == CHAR_TYPEPET ){
		}else{
			HealFlg |= ( 1 << BD_KIND_MP );
		}
		power[BD_KIND_HP] = 10000000;
		power[BD_KIND_MP] = 100;
	}
	kind = BD_KIND_HP;
	if( (p = strstr( arg, aszKeyString[kind] )) != NULL ){
		HealFlg |= ( 1 << kind );
		if( sscanf( p+2, "%d", &work ) != 1 ){
			power[kind] = 1;
		}else{
			power[kind] = RAND( (work*0.9), (work*1.1) );
		}
#ifndef _MAGIC_REHPAI
		power[kind] *= GetRecoveryRate( toindex );
#endif
	}
	kind = BD_KIND_MP;
	if( (p = strstr( arg, aszKeyString[kind] )) != NULL
	&&  CHAR_getInt( toindex, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER){
		HealFlg |= ( 1 << kind );
		if( sscanf( p+2, "%d", &work ) != 1 ){
			power[kind] = 1;
		}else{
			power[kind] = RAND( (work*0.9), (work*1.1) );
		}
	}
	kind = BD_KIND_AI;
	if( (p = strstr( arg, aszKeyString[kind] )) != NULL
		&& CHAR_getInt( toindex, CHAR_WHICHTYPE) == CHAR_TYPEPET){
		HealFlg |= ( 1 << kind );
		if( sscanf( p+2, "%d", &work ) != 1 ){
			power[kind] = 1;
		}else{
			power[kind] = RAND( (work*0.9), (work*1.1) );
		}
		power[kind] *= 100;
	}
	kind = BD_KIND_CHARM;
	if( (p = strstr( arg, aszKeyString[kind] )) != NULL
	&& CHAR_getInt( toindex, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER	){
		HealFlg |= ( 1 << kind );
		if( sscanf( p+2, "%d", &work ) != 1 ){
			power[kind] = 1;
		}else{
			power[kind] = RAND( (work*0.9), (work*1.1) );
		}
	}
	if( HealFlg == 0 )return;
#ifdef _TYPE_TOXICATION
	if( CHAR_CanCureFlg( toindex, "HP") == FALSE )return;
#endif

	for( j = 0; j < BD_KIND_END; j ++ ){
		if( ( HealFlg & ( 1 << j ) ) == 0 )continue;
		prevhp = CHAR_getInt( toindex, aHealInt[j] );
		workhp = prevhp + (int)power[j];

		if( j == BD_KIND_CHARM ){
			workmax = 100;
			workmin = 0;
		}else
		if( j == BD_KIND_AI ){
			workmax = 10000;
			workmin = -10000;
		}else{
			workmax = CHAR_getWorkInt( toindex, aHealMaxWork[j] );
			workmin = 1;
		}
		workhp = min( workhp, workmax );
		workhp = max( workhp, workmin );
		CHAR_setInt( toindex, aHealInt[j], workhp );
		recovery[j] = workhp - prevhp;
		if( j == BD_KIND_AI ){
			recovery[j] *= 0.01;
		}
	}
	CHAR_complianceParameter( toindex );

	if( CHAR_getInt( toindex, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER) {
		if( charaindex != toindex ) {
			CHAR_send_P_StatusString( toindex, CHAR_P_STRING_HP|CHAR_P_STRING_MP|CHAR_P_STRING_CHARM);
		}
	}
	CHAR_send_P_StatusString( charaindex, CHAR_P_STRING_HP|CHAR_P_STRING_MP|CHAR_P_STRING_CHARM);
	if( CHAR_getInt( toindex, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER &&
		CHAR_getWorkInt( charaindex, CHAR_WORKPARTYMODE) != CHAR_PARTY_NONE ){
		CHAR_PartyUpdate( toindex, CHAR_N_STRING_HP|CHAR_N_STRING_MP );
	}
	if( CHAR_getInt( toindex, CHAR_WHICHTYPE) == CHAR_TYPEPET ){
		int		i;
		for( i = 0; i < CHAR_MAXPETHAVE; i ++ ) {
			int workindex = CHAR_getCharPet( charaindex, i );
			if( workindex == toindex ){
				CHAR_send_K_StatusString( charaindex, i, CHAR_K_STRING_HP|CHAR_K_STRING_AI);
			}
		}
	}


	for( j = 0; j < BD_KIND_END; j ++ ){
		if( ( HealFlg & ( 1 << j ) ) == 0 )continue;
		if( charaindex != toindex) {
			if( power[j] >= 0 ){
				snprintf( msgbuf, sizeof( msgbuf),
					aszHealStringByOwn[j],
					CHAR_getUseName( toindex ), recovery[j] );
			}else{
				snprintf( msgbuf, sizeof( msgbuf),
					aszDownStringByOwn[j],
					CHAR_getUseName( toindex ), -recovery[j] );
			}
			CHAR_talkToCli( charaindex, -1, msgbuf, CHAR_COLORWHITE);
			if( CHAR_getInt( toindex, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER) {
				if( power[j] >= 0 ){
					snprintf( msgbuf, sizeof( msgbuf),
						aszHealStringByOther[j],
						CHAR_getUseName( charaindex ),
						CHAR_getUseName( toindex ), recovery[j] );
				}else{
					snprintf( msgbuf, sizeof( msgbuf),
						aszDownStringByOther[j],
						CHAR_getUseName( charaindex ),
						CHAR_getUseName( toindex ), -recovery[j] );
				}
				CHAR_talkToCli( toindex, -1, msgbuf, CHAR_COLORWHITE);
			}
		}else {
			if( power[j] >= 0 ){
				snprintf( msgbuf, sizeof( msgbuf),
					aszHealStringByOwn[j],
					CHAR_getUseName( charaindex ), recovery[j] );
			}else{
				snprintf( msgbuf, sizeof( msgbuf),
					aszDownStringByOwn[j],
					CHAR_getUseName( charaindex ), -recovery[j] );
			}
			CHAR_talkToCli( charaindex, -1, msgbuf, CHAR_COLORWHITE);
		}
	}
	{
		LogItem(
			CHAR_getChar( charaindex, CHAR_NAME ),
			CHAR_getChar( charaindex, CHAR_CDKEY ),
#ifdef _add_item_log_name  // WON ADD 在item的log中增加item名称
			itemindex,
#else
       		ITEM_getInt( itemindex, ITEM_ID ),
#endif
			"FieldUse",
	       	CHAR_getInt( charaindex,CHAR_FLOOR),
			CHAR_getInt( charaindex,CHAR_X ),
        	CHAR_getInt( charaindex,CHAR_Y ),
			ITEM_getChar( itemindex, ITEM_UNIQUECODE),
						ITEM_getChar( itemindex, ITEM_NAME),
						ITEM_getInt( itemindex, ITEM_ID)
		);
	}
	CHAR_DelItemMess( charaindex, haveitemindex, 0);

}

#endif

void ITEM_useRecovery( int charaindex, int toindex, int haveitemindex)
{
	int battlemode;
	if( CHAR_CHECKINDEX( charaindex ) == FALSE )return ;
	battlemode = CHAR_getWorkInt( charaindex, CHAR_WORKBATTLEMODE );
	if( battlemode == BATTLE_CHARMODE_INIT ){
	}else
		if( battlemode ){
			ITEM_useRecovery_Battle( charaindex, toindex, haveitemindex );
		}else{
			ITEM_useRecovery_Field(	charaindex, toindex, haveitemindex );
		}

}

#ifdef _ITEM_MAGICRECOVERY
void ITEM_useMRecovery( int charaindex, int toindex, int haveitemindex)
{
	int battlemode;
	if( CHAR_CHECKINDEX( charaindex ) == FALSE )return ;
	battlemode = CHAR_getWorkInt( charaindex, CHAR_WORKBATTLEMODE );
	if( battlemode == BATTLE_CHARMODE_INIT ){
	}else
		if( battlemode ){
			ITEM_useMRecovery_Battle( charaindex, toindex, haveitemindex );
		}else{
//			ITEM_useRecovery_Field(	charaindex, toindex, haveitemindex );
		}

}

#endif

#ifdef _ITEM_USEMAGIC
void ITEM_useMagic( int charaindex, int toindex, int haveitemindex)
{
	int battlemode;
	if( CHAR_CHECKINDEX( charaindex ) == FALSE )return ;
	battlemode = CHAR_getWorkInt( charaindex, CHAR_WORKBATTLEMODE );
	if( battlemode == BATTLE_CHARMODE_INIT ){
	}else
		if( battlemode ){
			ITEM_useMagic_Battle( charaindex, toindex, haveitemindex );
		}
}
#endif

#ifdef _PET_LIMITLEVEL
void ITEM_useOtherEditBase( int charaindex, int toindex, int haveitemindex)
{
	int itemindex,i;
	int work[4];
	int num=-1,type;
	int LevelUpPoint,petrank;
	char buf1[256];
	char buf2[][32]={"腕力成长率","耐久力成长率","速度成长率","体力成长率","能力"};
	char buf3[][32]={"大幅提高","略为提高","略为减少"};
	if( !CHAR_CHECKINDEX( charaindex ) )  return;
	if( !CHAR_CHECKINDEX( toindex ) )  return; 
#define RAND(x,y)   ((x-1)+1+ (int)( (double)(y-(x-1))*rand()/(RAND_MAX+1.0)) )
		itemindex = CHAR_getItemIndex( charaindex, haveitemindex);
		if( !ITEM_CHECKINDEX(itemindex) )  return;
		if( CHAR_getInt( toindex, CHAR_PETID) == 718 
#ifdef _PET_2LIMITLEVEL
			|| CHAR_getInt( toindex, CHAR_PETID) == 401 
#endif
			)	{

			int maxnums=50;

			if( CHAR_getInt( toindex,CHAR_LV ) < 74 )	{
				sprintf(buf1,"给我的吗？好美丽的项链喔！〈能力起了变化〉");
				CHAR_talkToCli( charaindex, toindex, buf1, CHAR_COLORWHITE);
				num = ITEM_MODIFYATTACK;
				LevelUpPoint = CHAR_getInt( toindex, CHAR_ALLOCPOINT );
				petrank = CHAR_getInt( toindex, CHAR_PETRANK );
				work[3] =(( LevelUpPoint >> 24 ) & 0xFF);
				work[0] = (( LevelUpPoint >> 16 ) & 0xFF);
				work[1] = (( LevelUpPoint >> 8 ) & 0xFF);
				work[2] = (( LevelUpPoint >> 0 ) & 0xFF);
				for( i=0; i<4; i++)	{
					type = ITEM_getInt( itemindex, (num + i));
					work[i] += type;
					strcpy( buf1,"\0");
					if( work[i] > maxnums )	{
						sprintf(buf1,"%s 已经达到最高了。", buf2[i]);
						work[i] = maxnums;
					}else if( work[i] < 0 )	{
						sprintf(buf1,"%s 已经为零了。", buf2[i]);
						work[i] = 0;
					}else	{
						if( type > 0 )	{
							if( type > 2 )
								sprintf(buf1,"%s %s %s", buf2[i], buf3[0], "。");
							else
								sprintf(buf1,"%s %s %s", buf2[i], buf3[1], "。");
						}else if( type < 0 ){
							sprintf(buf1,"%s %s %s", buf2[i], buf3[2], "。");	
						}
					}
					if( strcmp( buf1, "\0"))	{
						CHAR_talkToCli( charaindex, toindex, buf1, CHAR_COLORWHITE);
					}
				}
				
				LevelUpPoint = ( work[3]<< 24) + ( work[0]<< 16) + ( work[1]<< 8) + ( work[2]<< 0);
				CHAR_setInt( toindex, CHAR_ALLOCPOINT, LevelUpPoint);
				CHAR_setInt( toindex, CHAR_PETRANK, petrank);
				LogPetPointChange(
					CHAR_getChar( charaindex, CHAR_NAME ),
					CHAR_getChar( charaindex, CHAR_CDKEY ),
					CHAR_getChar( charaindex, CHAR_NAME),
					toindex, 4,
					CHAR_getInt( toindex, CHAR_LV),
					"item_use",
					CHAR_getInt( charaindex, CHAR_FLOOR),
					CHAR_getInt( charaindex, CHAR_X ),
					CHAR_getInt( charaindex, CHAR_Y )
					);
			}
				
		}else	{
			sprintf(buf1,"？？？");
		}
		CHAR_DelItem( charaindex, haveitemindex);
		return;
}
#endif

void ITEM_useStatusChange( int charaindex, int toindex, int haveitemindex)
{
	int battlemode;
	if( CHAR_CHECKINDEX( charaindex ) == FALSE )return ; //｛撩  
	battlemode = CHAR_getWorkInt( charaindex, CHAR_WORKBATTLEMODE );
	if( battlemode == BATTLE_CHARMODE_INIT ){
	}else
	if( battlemode ){
		ITEM_useStatusChange_Battle( charaindex, toindex, haveitemindex );
	}else{
	}

}

void ITEM_useStatusRecovery( int charaindex, int toindex, int haveitemindex)
{
	int battlemode;
	if( CHAR_CHECKINDEX( charaindex ) == FALSE )return ; //｛撩  
	battlemode = CHAR_getWorkInt( charaindex, CHAR_WORKBATTLEMODE );
	if( battlemode == BATTLE_CHARMODE_INIT ){
	}else
	if( battlemode ){
		ITEM_useStatusRecovery_Battle( charaindex, toindex, haveitemindex );
	}else{
	}
}

void ITEM_useMagicDef( int charaindex, int toindex, int haveitemindex)
{
	int battlemode;
	if( CHAR_CHECKINDEX( charaindex ) == FALSE )return;
	battlemode = CHAR_getWorkInt( charaindex, CHAR_WORKBATTLEMODE );
	if( battlemode == BATTLE_CHARMODE_INIT ){
	}else
	if( battlemode ){
		ITEM_useMagicDef_Battle( charaindex, toindex, haveitemindex );
	}else{
	}

}

void ITEM_useParamChange( int charaindex, int toindex, int haveitemindex)
{
	int battlemode;
	if( CHAR_CHECKINDEX( charaindex ) == FALSE )return ;
	battlemode = CHAR_getWorkInt( charaindex, CHAR_WORKBATTLEMODE );
	if( battlemode == BATTLE_CHARMODE_INIT ){
	}else
		if( battlemode ){
			ITEM_useParamChange_Battle( charaindex, toindex, haveitemindex );
		}
}

void ITEM_useFieldChange( int charaindex, int toindex, int haveitemindex)
{
	int battlemode;
	if( CHAR_CHECKINDEX( charaindex ) == FALSE )return ;
	battlemode = CHAR_getWorkInt( charaindex, CHAR_WORKBATTLEMODE );
	if( battlemode == BATTLE_CHARMODE_INIT ){
	}else
		if( battlemode ){
			ITEM_useFieldChange_Battle( charaindex, toindex, haveitemindex );
		}
}

void ITEM_useAttReverse( int charaindex, int toindex, int haveitemindex)
{
	int battlemode;
	if( CHAR_CHECKINDEX( charaindex ) == FALSE )return;
	battlemode = CHAR_getWorkInt( charaindex, CHAR_WORKBATTLEMODE );
	if( battlemode == BATTLE_CHARMODE_INIT ){
	}else
	if( battlemode ){
		ITEM_useAttReverse_Battle( charaindex, toindex, haveitemindex );
	}else{
	}
}

void ITEM_useMic( int charaindex, int toindex, int haveitemindex)
{
	int battlemode;
	if( CHAR_CHECKINDEX( charaindex ) == FALSE )return;
	battlemode = CHAR_getWorkInt( charaindex, CHAR_WORKBATTLEMODE );
	if( battlemode == BATTLE_CHARMODE_INIT ){
	}else
		if( battlemode ){
		}else{
			ITEM_useMic_Field( charaindex, toindex, haveitemindex );
		}

}

void ITEM_useCaptureUp( int charaindex, int toindex, int haveitemindex)
{
	int battlemode;
	if( CHAR_CHECKINDEX( charaindex ) == FALSE )return ; //｛撩  

	battlemode = CHAR_getWorkInt( charaindex, CHAR_WORKBATTLEMODE );
	if( battlemode == BATTLE_CHARMODE_INIT ){
	}else
	if( battlemode ){
		ITEM_useCaptureUp_Battle( charaindex, toindex, haveitemindex );
	}else{
	}

}

#ifdef _PETSKILL_CANNEDFOOD
static void ITEM_usePetSkillCanned_PrintWindow( int charaindex, int flg)
{
	int fd;
	char	message[256], buf[2048];
	if( !CHAR_CHECKINDEX( charaindex )) return;
	fd = getfdFromCharaIndex( charaindex);
	if( fd == - 1 ) return;

	sprintf( message, "%d", flg);
	lssproto_WN_send( fd, WINDOWS_MESSAGETYPE_PETSKILLSHOW, 
					WINDOW_BUTTONTYPE_NONE,
					ITEM_WINDOWTYPE_SELECTPETSKILL_SELECT,
					-1,
					makeEscapeString( message, buf, sizeof( buf)));
}

void ITEM_usePetSkillCanned_WindowResult( int charaindex, int seqno, int select, char * data)
{
	int itemindex=-1, itemNo, petindex=-1, petNo;
	int SkillNo, SkillID;
	char buf1[256];
	char *skillarg=NULL;

	petNo = CHAR_getWorkInt( charaindex, CHAR_WORKRENAMEITEMNUM);
	itemNo = CHAR_getWorkInt( charaindex, CHAR_WORKRENAMEITEMINDEX);


	itemindex = CHAR_getItemIndex( charaindex, itemNo);
	if( !ITEM_CHECKINDEX( itemindex) )	return;
	petindex = CHAR_getCharPet( charaindex, petNo);
	if( !CHAR_CHECKINDEX(petindex) ) return;
	SkillNo = atoi( data);
	if( SkillNo < 0 || SkillNo >= CHAR_MAXPETSKILLHAVE ) return;

	skillarg = ITEM_getChar( itemindex, ITEM_ARGUMENT);
	SkillID = atoi( skillarg);

#ifdef _CFREE_petskill
	if( NPC_CHECKFREEPETSKILL( charaindex, petindex, SkillID) == FALSE ){
		CHAR_talkToCli( charaindex, -1, "该宠物无法学习此项技能！", CHAR_COLORYELLOW);
		return;
	}
#endif

	CHAR_setPetSkill( petindex, SkillNo, SkillID);
	snprintf( buf1, sizeof( buf1 ), "W%d", petNo);
	CHAR_sendStatusString( charaindex, buf1 );
	CHAR_sendStatusString( charaindex, "P");
	{
		int skillarray = PETSKILL_getPetskillArray( SkillID);
		sprintf( buf1, "宠物%s学习%s，道具 %s消失了。",
			CHAR_getUseName( petindex), PETSKILL_getChar( skillarray, PETSKILL_NAME),
			ITEM_getChar( itemindex, ITEM_NAME) );
	}

	CHAR_talkToCli( charaindex, -1, buf1, CHAR_COLORYELLOW);
	CHAR_setItemIndex( charaindex, itemNo ,-1);
	CHAR_sendItemDataOne( charaindex, itemNo);
	ITEM_endExistItemsOne( itemindex);
}

#endif

static void ITEM_useRenameItem_PrintWindow( int charaindex, int page)
{
	int fd;
	int	pos = 0;
	int i;
	int btntype = WINDOW_BUTTONTYPE_CANCEL;
	char	message[1024];
	char	msgwk[1024];
	char	buf[2048];

	if( !CHAR_CHECKINDEX( charaindex )) return;
	if( page < 0 || page > 3 ) {
		print( "%s:%d err\n", __FILE__, __LINE__);
		return;
	}
	fd = getfdFromCharaIndex( charaindex);
	if( fd == - 1 ) return;
	
	snprintf( message, sizeof( message),
				"2\n   要变更那个项目的名称呢\n"
				"                               Page:%d\n", page +1);
	
	for( i = page *5; i < page *5 +5; i ++ ) {
		int itemindex = CHAR_getItemIndex( charaindex, i);
		BOOL	flg = FALSE;
		while( 1 ) {
			char *cdkey;
			if( !ITEM_CHECKINDEX( itemindex)) break;
			cdkey = ITEM_getChar( itemindex, ITEM_CDKEY);
			if( !cdkey) {
				print( "%s:%d err\n", __FILE__, __LINE__);
				break;
			}
			if( ITEM_getInt( itemindex, ITEM_MERGEFLG) != 1) break;
			if( ITEM_getInt( itemindex, ITEM_TYPE) == ITEM_DISH) break;
			if( ITEM_getInt( itemindex, ITEM_CRUSHLEVEL) != 0 ) break;
			if( strlen( cdkey) != 0 ) {
				if( strcmp( cdkey, CHAR_getChar( charaindex, CHAR_CDKEY)) != 0 ) {
					break;
				}
			}
			flg = TRUE;
			break;
		}
		if( flg ) {
			char *nm = ITEM_getChar( itemindex, ITEM_SECRETNAME);
			char wk[256];
			if( pos +strlen( nm) +1 > sizeof( msgwk)) {
				print( "buffer over error %s:%d\n", __FILE__, __LINE__);
				break;
			}
			snprintf( wk, sizeof( wk), "%s\n", nm);
			strncpy( &msgwk[pos],  wk, sizeof( msgwk) -pos -1);
			pos += strlen( wk);
		}
		else {
			if( pos +2 > sizeof( msgwk)) {
				print( "buffer over error %s:%d\n", __FILE__, __LINE__);
				break;
			}
			strncpy( &msgwk[pos], "\n", sizeof( msgwk) -pos -1 );
			pos += 1;
		}
	}
	strcat( message, msgwk);
	switch( page){
	  case 0:
	  	btntype |= WINDOW_BUTTONTYPE_NEXT;
	  	break;
	  case 3:
	  	btntype |= WINDOW_BUTTONTYPE_PREV;
	  	break;
	  case 1:
	  case 2:
	  	btntype |= WINDOW_BUTTONTYPE_PREV|WINDOW_BUTTONTYPE_NEXT;
	  	break;
	}
	lssproto_WN_send( fd, WINDOW_MESSAGETYPE_SELECT, 
					btntype,
					CHAR_WINDOWTYPE_SELECTRENAMEITEM_PAGE1+page,
					-1,
					makeEscapeString( message, buf, sizeof(buf)));
}

void ITEM_useRenameItem( int charaindex, int toindex, int haveitemindex)
{

	ITEM_useRenameItem_PrintWindow( charaindex, 0);

	CHAR_setWorkInt( charaindex, CHAR_WORKRENAMEITEMNUM, -1);
	CHAR_setWorkInt( charaindex, CHAR_WORKRENAMEITEMINDEX, haveitemindex);
/*
	char buf[256];
	int itemindex = CHAR_getItemIndex( charaindex, haveitemindex);
	if( !ITEM_CHECKINDEX( itemindex) ) return;
	sprintf( buf, "%s功能已被取消。", ITEM_getChar( itemindex, ITEM_NAME));
	CHAR_talkToCli( charaindex, -1, "功能已被取消。", CHAR_COLORRED );

	{
		LogItem(
			CHAR_getChar( charaindex, CHAR_NAME ),
			CHAR_getChar( charaindex, CHAR_CDKEY ),
			itemindex,
			"魔术笔DEL",
			CHAR_getInt( charaindex, CHAR_FLOOR),
			CHAR_getInt( charaindex, CHAR_X ),
 	      	CHAR_getInt( charaindex, CHAR_Y ),
	        ITEM_getChar( itemindex, ITEM_UNIQUECODE),
			ITEM_getChar( itemindex, ITEM_NAME),
			ITEM_getInt( itemindex, ITEM_ID)
		);
	}

	CHAR_setItemIndex( charaindex, haveitemindex, -1);
	CHAR_sendItemDataOne( charaindex, haveitemindex);
	ITEM_endExistItemsOne( itemindex );
*/
}

void ITEM_useRenameItem_WindowResult( int charaindex, int seqno, int select, char * data)
{
	int page = 0;
	int fd;
	if( select == WINDOW_BUTTONTYPE_CANCEL) return;

	fd = getfdFromCharaIndex( charaindex);
	if( fd == - 1 ) return;
	if( seqno != CHAR_WINDOWTYPE_SELECTRENAMEITEM_RENAME ) {
		if( select == WINDOW_BUTTONTYPE_NEXT ) page = 1;
		else if( select == WINDOW_BUTTONTYPE_PREV ) page = -1;
		if( select == WINDOW_BUTTONTYPE_NEXT || select == WINDOW_BUTTONTYPE_PREV ) {
			int winno = seqno - CHAR_WINDOWTYPE_SELECTRENAMEITEM_PAGE1;
			winno += page;
			if( winno < 0 ) winno = 0;
			if( winno > 3 ) winno = 3;
			ITEM_useRenameItem_PrintWindow( charaindex, winno);
		}else {
			char	message[1024];
			char	buf[2048];
			char	haveitemindex = CHAR_getWorkInt( charaindex, CHAR_WORKRENAMEITEMNUM);
			int		itemindex;
			if( haveitemindex == -1 ) {
				haveitemindex = (seqno - CHAR_WINDOWTYPE_SELECTRENAMEITEM_PAGE1) * 5 + 
								( atoi(data)-1);
				CHAR_setWorkInt( charaindex, CHAR_WORKRENAMEITEMNUM, haveitemindex);
			}
			itemindex = CHAR_getItemIndex( charaindex, haveitemindex);

			snprintf( message, sizeof( message),
							"%s 名字要更改\n"
							"请输入姓名\n"
							"全形13个字, 半形26个字",
							ITEM_getChar( itemindex, ITEM_NAME));
			
			
			lssproto_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGEANDLINEINPUT, 
							WINDOW_BUTTONTYPE_OKCANCEL,
							CHAR_WINDOWTYPE_SELECTRENAMEITEM_RENAME,
							-1,
							makeEscapeString( message, buf, sizeof(buf)));
			
		}
	}
	else {
		BOOL	flg = FALSE;
		char	message[1024];
		char	buf[2048];
		while( 1 ) {
			char	*p;
			if( strlen( data) > 26 || strlen( data) < 1) {
				if( strlen( data) > 26 ) {
					strcpy( message, "超过文字限制");
				}else {
					strcpy( message, "请输入一个文字以上");
				}
				break;
			}

            // WON ADD 修正魔术笔改名问题
			flg = TRUE;

			for( p = data; *p ; p ++) {
				if( *p == ' '){
					strcpy( message, "不可空白");
					flg = FALSE;
					break;
				}
				if( strncmp( p, "　",2) == 0 ) {
					strcpy( message, "不可空白");
					flg = FALSE;
					break;
				}
				if( *p == '|'){
					strcpy( message, "不可有特殊符号");
					flg = FALSE;
					break;
				}
			}
	
			break;
		}


		if( !flg ) {
			lssproto_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE, 
							WINDOW_BUTTONTYPE_OK,
							CHAR_WINDOWTYPE_SELECTRENAMEITEM_RENAME_ATTENTION,
							-1,
							makeEscapeString( message, buf, sizeof(buf)));
		}
		else {
			char	haveitemindex = CHAR_getWorkInt( charaindex, CHAR_WORKRENAMEITEMNUM);
			int		itemindex;
			int		renameitemindex;
			int		renameitemhaveindex;
			int		remain;
			char msgbuf[128];

			itemindex = CHAR_getItemIndex( charaindex, haveitemindex);
			if( !ITEM_CHECKINDEX( itemindex)) {
				print( "%s %d err\n", __FILE__, __LINE__);
				return;
			}
			ITEM_setChar( itemindex, ITEM_SECRETNAME, data);
			ITEM_setChar( itemindex, ITEM_CDKEY, 
						CHAR_getChar( charaindex, CHAR_CDKEY));
			CHAR_sendItemDataOne( charaindex, haveitemindex);
			snprintf( msgbuf, sizeof(msgbuf),"把 %s 命名成 %s ", 
					ITEM_getChar( itemindex, ITEM_NAME), data);
		    CHAR_talkToCli( charaindex, -1, msgbuf, CHAR_COLORYELLOW);
			renameitemhaveindex = CHAR_getWorkInt( charaindex, CHAR_WORKRENAMEITEMINDEX);
			renameitemindex = CHAR_getItemIndex( charaindex, renameitemhaveindex);
			if( !ITEM_CHECKINDEX( renameitemindex)) {
				print( "%s %d err\n", __FILE__, __LINE__);
				return;
			}
			remain = atoi( ITEM_getChar( renameitemindex, ITEM_ARGUMENT));
			if( remain != 0 ) {
				remain --;
				if( remain <= 0 ) {
					snprintf( msgbuf, sizeof(msgbuf),"%s 消失了", 
							ITEM_getChar( renameitemindex, ITEM_NAME));
				    CHAR_talkToCli( charaindex, -1, msgbuf, CHAR_COLORYELLOW);
				    CHAR_setItemIndex( charaindex, renameitemhaveindex, -1);
				    CHAR_sendItemDataOne( charaindex, renameitemhaveindex);
					ITEM_endExistItemsOne( renameitemindex );
				}else {
					char buf[32];
					snprintf( buf, sizeof( buf),"%d", remain);
					ITEM_setChar( renameitemindex, ITEM_ARGUMENT, buf);
				}
			}
		}
	}
}

//-------------------------------------------------------------------------
//	今中仇欠毛  蜇允月楮醒［
//	  午仄凶凛卞］仿件母丞卞ㄠ  ㄥ毛蓟太］  蟆午  飓  寞毛  凳允月［
//-------------------------------------------------------------------------
void ITEM_dropDice( int charaindex, int itemindex)
{
	char *dicename[] = { "一", "二",  "三", "四", "五", "六"};
	int  diceimagenumber[] = { 24298,24299,24300,24301,24302,24303};
	int r = RAND( 0,5);
	
	//   飓  寞毛谨  
	ITEM_setInt( itemindex, ITEM_VAR1, ITEM_getInt( itemindex, ITEM_BASEIMAGENUMBER));
	//   飓  寞凳蕙
	ITEM_setInt( itemindex, ITEM_BASEIMAGENUMBER, diceimagenumber[r]);
	//   蟆凳蕙
	ITEM_setChar( itemindex, ITEM_SECRETNAME, dicename[r]);
	
	// 犯□正及弁仿奶失件玄尺及霜耨反晓匏楮醒匹垫丹及匹］仇仇匹反支日卅中［
}
//-------------------------------------------------------------------------
//	今中仇欠毛  蜇允月楮醒［
//  胶匀凶凛卞］  蟆午  飓  寞毛葭卞  允［
//-------------------------------------------------------------------------
void ITEM_pickupDice( int charaindex, int itemindex)
{
	//   飓  寞毛葭卞  允［
	ITEM_setInt( itemindex,  ITEM_BASEIMAGENUMBER, ITEM_getInt( itemindex, ITEM_VAR1));
	//   蟆手葭卞  允
	ITEM_setChar( itemindex, ITEM_SECRETNAME, ITEM_getChar( itemindex, ITEM_NAME));
}
enum {
	ITEM_LOTTERY_1ST,		// 1羁
	ITEM_LOTTERY_2ND,
	ITEM_LOTTERY_3RD,
	ITEM_LOTTERY_4TH,
	ITEM_LOTTERY_5TH,		// 5羁
	ITEM_LOTTERY_6TH,		// 6羁
	ITEM_LOTTERY_NONE,		// 陆木
	ITEM_LOTTERY_NUM,
};
//-------------------------------------------------------------------------
//	旦疋□玉仁元互综日木月凛及楮醒［
//  仇仇匹窒羁操卞卅月井毛瑁户月［
//  割  反动票及骚曰［

#define PRE_6		(10000)
#define PRE_5		(1300 + PRE_6)
#define PRE_4		(600 + PRE_5)
#define PRE_3		(300 + PRE_4)
#define PRE_2		(8 + PRE_3)
#define PRE_1		(1 + PRE_2)


//#define PRE_5		(13000)
//#define PRE_4		(2300 + PRE_5)
//#define PRE_3		(540 + PRE_4)
//#define PRE_2		(8 + PRE_3)
//#define PRE_1		(1 + PRE_2)


//#define PRE_5		(16000)
//#define PRE_4		(2500 + PRE_5)
//#define PRE_3		(400 + PRE_4)
//#define PRE_2		(10 + PRE_3)
//#define PRE_1		(1 + PRE_2)
//#define PRE_5		(27000)
//#define PRE_4		(4000 + PRE_5)
//#define PRE_3		(400 + PRE_4)
//#define PRE_2		(10 + PRE_3)
//#define PRE_1		(1 + PRE_2)
//
//-------------------------------------------------------------------------
BOOL ITEM_initLottery(ITEM_Item* itm)
{
	int r = RAND( 0, 49999);
	int hit = ITEM_LOTTERY_NONE;		// 
	char result[7];		// 请  
	int countnum[6];
	int count;
	int i;
	int len;
	// 域荚综匀化月及匹［疯粟  仄凶凛午井卞综曰卅云今木卅中方丹卞［
	if( itm->data[ITEM_VAR3] == 1 ) return TRUE;

	// 铲蓟允月［
	if( r < PRE_6 ) hit = ITEM_LOTTERY_6TH;
	else if( r < PRE_5 ) hit = ITEM_LOTTERY_5TH;
	else if( r < PRE_4 ) hit = ITEM_LOTTERY_4TH;
	else if( r < PRE_3 ) hit = ITEM_LOTTERY_3RD;
	else if( r < PRE_2 ) hit = ITEM_LOTTERY_2ND;
	else if( r < PRE_1 ) hit = ITEM_LOTTERY_1ST;
	else hit = ITEM_LOTTERY_NONE;
	
	// 请  毛瑁烂允月［
	count = 0;
	if( hit != ITEM_LOTTERY_NONE ) {
		// 癫曰毛本永玄
		result[0] = result[1] = result[2] = hit+1;
		count = 3;
		countnum[hit] = 3;
	}
	while( count < 6 ) {
		int r = RAND( ITEM_LOTTERY_1ST, ITEM_LOTTERY_6TH);
		if( countnum[r] >= 2 ) continue;
		// 2/3仁日中及割  匹］职及陆木钓  毛伉□民今六化支月［
		// 升五升五［
		if(      ( hit != ITEM_LOTTERY_NONE && count == 3 )
		     ||  ( hit == ITEM_LOTTERY_NONE && count == 0 ) )
		{
			if( RAND( 0,2)) {
				result[count] = result[count+1] = r+1;
				countnum[r] += 2;
				count += 2;
				continue;
			}
		}
		countnum[r] ++;
		result[count] = r+1;
		count++;
	}
	// 请  毛扑乓永白伙允月［
	// 癫曰及凛反2/3及割  匹］癫曰醒侬毛域    欠卞裔烂允月［
	// 玉平玉平躲绊毛谎丹啃［
	len = sizeof( result)-2;
	if( hit != ITEM_LOTTERY_NONE ) {
		if( RAND( 0,2) ) {
			// 癫曰醒侬毛域    欠卞［
			char s = result[0];
			result[0] = result[5];
			result[5] = s;
			len = sizeof( result)-3;
		}
	}
	for( i = 0; i < 10; i ++) {
		int x = RAND( 0, len);
		int y = RAND( 0, len);
		int s;
		s = result[x];
		result[x] = result[y];
		result[y] = s;
	}
	//   及凶户］  侬  健中卞仄化云仁［
	result[sizeof(result)-1] = '\0';
	itm->data[ITEM_VAR1] = hit;
	// 户仁匀凶市它件玄
	itm->data[ITEM_VAR2] = 0;
	itm->data[ITEM_VAR3] = 1;
	memcpy( itm->string[ITEM_ARGUMENT].string, result, sizeof( result));
	
	return TRUE;
}
//-------------------------------------------------------------------------
//	旦疋□玉仁元毛银匀凶凛及楮醒［
//  ㄥ荚银丹午］窒羁操井及失奶  丞卞  祭允月［
//-------------------------------------------------------------------------
void ITEM_useLottery( int charaindex, int toindex, int haveitemindex)
{
	int i,j;
	int itemindex = CHAR_getItemIndex( charaindex, haveitemindex);
	int count = ITEM_getInt( itemindex, ITEM_VAR2);
	int hit = ITEM_getInt( itemindex, ITEM_VAR1);
	char buff[1024];
	char num[6][3] = { {"迭"},{"迫"},{"迤"},{"迨"},{"郊"}, {"郎"}};
	char numbuff[128];
	char *n;
	int result;
	BOOL flg;
	if( !ITEM_CHECKINDEX( itemindex) ) return;
	if( count == 0 ) {
		ITEM_setChar( itemindex, ITEM_EFFECTSTRING, "");
	}
	// ㄥ荚  及银迕反］陆木仄井丐曰  卅中及匹］仇仇匹失奶  丞毛壅允
	else if( count == 6 ) {
        CHAR_setItemIndex( charaindex , haveitemindex, -1 );
		CHAR_sendItemDataOne( charaindex, haveitemindex);
        ITEM_endExistItemsOne( itemindex );
		return;
	}
	n = ITEM_getChar( itemindex, ITEM_ARGUMENT);
	// for debug
	result = (int)n[count]-1;
	// 伉□民井譬屯月
	flg = FALSE;
	for( i = 0; i < count+1 && flg == FALSE; i ++ ) {
		for( j = i+1; j < count+1 && flg == FALSE; j ++ ) {
			if( i != j ) {
				if( n[i] == n[j] ) {
					flg = TRUE;
				}
			}
		}
	}
	memcpy( numbuff, ITEM_getChar( itemindex, ITEM_EFFECTSTRING), (count)*2);
	// 躲绊  侬  卞］请  毛请允［
	snprintf( buff, sizeof( buff), "%s%s", numbuff, num[result]);
	count ++;
	ITEM_setInt( itemindex, ITEM_VAR2, count);
	// ㄥ荚  匹癫曰卅日癫曰失奶  丞卞  凳］陆木卅日陆木午  憎］
	// 戚荚母皮伙弁伉永弁匹壅允［
	if( count >= 6 ) {
		// 癫曰
		if( hit != ITEM_LOTTERY_NONE ) {
      		int newitemindex;
      		char strbuff[1024];
            char msgbuff[1024];
            // 壅仄化
            CHAR_setItemIndex( charaindex , haveitemindex, -1 );
            ITEM_endExistItemsOne( itemindex );
			// 2729 - 2734 反  仁元及1  6羁
			newitemindex = ITEM_makeItemAndRegist( 2729 + hit);
            CHAR_setItemIndex( charaindex , haveitemindex, newitemindex );
			// 请  手戊疋□
			snprintf( strbuff, sizeof( strbuff), "%s                %s", buff, 
						ITEM_getChar( newitemindex, ITEM_EFFECTSTRING));
			ITEM_setChar( newitemindex, ITEM_EFFECTSTRING, strbuff);
			CHAR_sendItemDataOne( charaindex, haveitemindex);
            snprintf( msgbuff, sizeof( msgbuff), "中了第%d奖", hit+1 );
            CHAR_talkToCli( charaindex, -1,
                            msgbuff,
                            CHAR_COLORYELLOW );
		}
		// 陆木
		else {
//            CHAR_setItemIndex( charaindex , haveitemindex, -1 );
			// 请  手戊疋□
      		char strbuff[1024];
			snprintf( strbuff, sizeof( strbuff), "%s                       没中,下次再来", buff);
			ITEM_setChar( itemindex, ITEM_EFFECTSTRING, strbuff);
			CHAR_sendItemDataOne( charaindex, haveitemindex);
//            ITEM_endExistItemsOne( itemindex );
//            CHAR_talkToCli( charaindex, -1,
//                            "反内木″",
//                            CHAR_COLORWHITE );
		}
	}
	// 公丹元扎  中凛反］犯□正毛霜曰卅云允分仃［
	else {
		char strbuff[1024];
		if( flg) {
			// 旦矢□旦煌遥
			// 失奶  丞及      及戊丢件玄反］ㄠ垫ㄡㄧ  侬匹］
			// 旦矢□旦毛银匀化荼垫仄化中月［
			int spc = 16 + ( 6-count)*2;
			char space[17];
			space[spc] = '\0';
			snprintf( strbuff, sizeof( strbuff), "%s%s快中了", buff, space);
		}
		else {
			strcpy( strbuff, buff);
		}
		ITEM_setChar( itemindex, ITEM_EFFECTSTRING, strbuff);
		CHAR_sendItemDataOne( charaindex, haveitemindex);
	}
}

void ITEM_WarpDelErrorItem( int charaindex )
{
	int j;
	for(j=0; j<CHAR_MAXITEMHAVE; j++){
		int itemindex=CHAR_getItemIndex(charaindex, j);
		if(ITEM_CHECKINDEX(itemindex)){
			int id=ITEM_getInt(itemindex, ITEM_ID);
			if(id==2609||id==2704){
				CHAR_setItemIndex(charaindex, j, -1);
				ITEM_endExistItemsOne(itemindex);
				CHAR_sendItemDataOne(charaindex, j);
			}
		}
	}
#ifdef _ITEM_WARP_FIX_BI
	if(!CHAR_getWorkInt( charaindex, CHAR_WORKITEMMETAMO))
		recoverbi(charaindex);
#endif
}

BOOL ITEM_WarpForAny(int charaindex, int haveitemindex, int ff, int fx, int fy,int flg)
{
	if( CHAR_getWorkInt( charaindex, CHAR_WORKBATTLEMODE ) != BATTLE_CHARMODE_NONE )
		return FALSE;
	if( CHAR_getInt( charaindex, CHAR_FLOOR) == 117 
		){
		CHAR_talkToCli(charaindex, -1, "此处无法使用。", CHAR_COLORYELLOW);
		return FALSE;
	}
	if( CHAR_getWorkInt( charaindex, CHAR_WORKPARTYMODE ) == CHAR_PARTY_LEADER ){
		int i;
		if( flg == 0 )	{//单体
			CHAR_talkToCli(charaindex, -1, "只能单人使用。", CHAR_COLORYELLOW);
			return FALSE;
		}
		for( i = 0; i < CHAR_PARTYMAX; i ++ ){
			int subindex = CHAR_getWorkInt( charaindex, CHAR_WORKPARTYINDEX1+i );
			if( CHAR_CHECKINDEX( subindex ) == FALSE ) continue;
			CHAR_talkToCli( subindex, -1, "全体瞬间飞行．．．", CHAR_COLORWHITE);
			ITEM_WarpDelErrorItem( subindex );
			CHAR_warpToSpecificPoint( subindex, ff, fx, fy );
		}
	}else if( CHAR_getWorkInt( charaindex, CHAR_WORKPARTYMODE ) == CHAR_PARTY_CLIENT ){
		CHAR_talkToCli(charaindex, -1, "队员无法使用。", CHAR_COLORYELLOW);
		return FALSE;
	}else if( CHAR_getWorkInt( charaindex, CHAR_WORKPARTYMODE ) == CHAR_PARTY_NONE ){
		ITEM_WarpDelErrorItem( charaindex );
		CHAR_warpToSpecificPoint( charaindex, ff, fx, fy);
	}
	return TRUE;
}
//andy_end

// Robin 0523
void ITEM_useWarp( int charaindex, int toindex, int haveitemindex )
{
	char *arg;
	int itemindex, warp_t, warp_fl, warp_x, warp_y;
	itemindex = CHAR_getItemIndex( charaindex, haveitemindex);
	if(!ITEM_CHECKINDEX(itemindex)) return;

#ifdef _ITEM_CHECKWARES
	if( CHAR_CheckInItemForWares( charaindex, 0) == FALSE ){
		CHAR_talkToCli(charaindex, -1, "携带货物无法使用。", CHAR_COLORYELLOW);
		return;
	}
#endif
	arg = ITEM_getChar(itemindex, ITEM_ARGUMENT );
	if( arg == "\0" )	return;
	if( sscanf( arg, "%d %d %d %d", &warp_t, &warp_fl, &warp_x, &warp_y) != 4 )
		return;
	if( ITEM_WarpForAny(charaindex, haveitemindex, warp_fl, warp_x, warp_y, warp_t) == FALSE )
		return;

	CHAR_DelItem( charaindex, haveitemindex);
	CHAR_sendStatusString(charaindex, "P");
}

#ifdef _USEWARP_FORNUM
void ITEM_useWarpForNum( int charaindex, int toindex, int haveitemindex )
{
	char *arg;
	int flg, ff, fx, fy, itemindex, usenum=0, i;
	int Mf, Mx, My;
	int MapPoint[12]={
		100, 200, 300, 400, 700,
		701, 702, 703, 704, 705,
		707, 708 };
		char MapString[12][256]={
			"萨伊那斯","加鲁卡","吉鲁岛","沙姆岛","伊甸园区",
			"伊甸大陆西方","尼克斯城区","伊甸大陆南方","亚伊欧城区","伊甸大陆东方",
			"伊甸大陆北方","塔耳塔城区"};
	
	itemindex = CHAR_getItemIndex( charaindex, haveitemindex);
	if( !ITEM_CHECKINDEX( itemindex) ) return;
	arg = ITEM_getChar(itemindex, ITEM_ARGUMENT );
	if( arg == "\0" )	return;

	if( sscanf( arg, "%d %d %d %d", &flg, &ff, &fx, &fy) != 4 )
		return;
	Mf = CHAR_getInt( charaindex, CHAR_FLOOR );
	Mx = CHAR_getInt( charaindex, CHAR_X );
	My = CHAR_getInt( charaindex, CHAR_Y );

	usenum = ITEM_getInt( itemindex, ITEM_DAMAGEBREAK);
	for( i=0; i<12; i++)	{
		if( Mf == MapPoint[i] ){
			break;
		}
	}
#ifdef _ITEM_CHECKWARES
	if( CHAR_CheckInItemForWares( charaindex, 0) == FALSE ){
		CHAR_talkToCli(charaindex, -1, "携带货物无法使用。", CHAR_COLORYELLOW);
		return;
	}
#endif
	if( --usenum <= 0 )	{
		CHAR_DelItem( charaindex, haveitemindex);
		CHAR_sendStatusString(charaindex, "P");
	}else	{
		char buf[256];
		// WON ADD
		if( i >= 12 ){
			CHAR_talkToCli(charaindex, -1, "此处无法使用。", CHAR_COLORYELLOW);
			return;
		}
		sprintf( buf, "%d %d %d %d", flg, Mf, Mx, My);
		ITEM_setChar(itemindex, ITEM_ARGUMENT, buf);
		ITEM_setInt( itemindex, ITEM_DAMAGEBREAK, usenum);
		sprintf( buf, "飞行纪录点(%s,%d,%d)", MapString[i], Mx, My);
		ITEM_setChar( itemindex, ITEM_EFFECTSTRING, buf);
		CHAR_talkToCli(charaindex, -1, buf, CHAR_COLORYELLOW);
		CHAR_sendItemDataOne( charaindex, haveitemindex);
	}

	// WON ADD
	if( ITEM_WarpForAny(charaindex, haveitemindex, ff, fx, fy, flg) == FALSE )
		return;
}
#endif

// Robin 0707 petFollow
void ITEM_petFollow( int charaindex, int toindex, int haveitemindex )
{
	char *arg;
	int itemindex, followLv, haveindex, i;
	//print(" PetFollow_toindex:%d ", toindex);
	if( CHAR_getWorkInt( charaindex, CHAR_WORKPETFOLLOW ) != -1 ){
		if( CHAR_CHECKINDEX( CHAR_getWorkInt( charaindex, CHAR_WORKPETFOLLOW ) ) ){
			CHAR_talkToCli( charaindex, -1, "请先收回放出的宠物！", CHAR_COLORWHITE );
			return;
		}
		CHAR_setWorkInt( charaindex, CHAR_WORKPETFOLLOW, -1);
	}
#ifdef _FIX_METAMORIDE
	if( CHAR_CHECKJOINENEMY( charaindex) == TRUE ){
		CHAR_talkToCli( charaindex, -1, "搭乘中无法使用！", CHAR_COLORWHITE );
		return;
	}
#endif

	if( CHAR_CHECKINDEX( toindex ) == FALSE )	return;
	itemindex = CHAR_getItemIndex( charaindex, haveitemindex);
	if(!ITEM_CHECKINDEX(itemindex)) return;
	arg = ITEM_getChar(itemindex, ITEM_ARGUMENT );
	if( arg == "\0" )return;

	if( sscanf( arg, "%d", &followLv) != 1 )
		return;
	if( CHAR_getInt( toindex, CHAR_LV ) > followLv ){
		CHAR_talkToCli( charaindex, -1, "道具的等级不足！", CHAR_COLORWHITE );
		return;
	}
	if( CHAR_getWorkInt( toindex, CHAR_WORKFIXAI ) < 80 )
	{
		//CHAR_talkToCli( charaindex, -1, "宠物的忠诚度不足！", CHAR_COLORWHITE );
		//return;
	}	
	
	
	haveindex = -1;	
	for( i = 0; i < 5; i++ )
	{
		if( CHAR_getCharPet( charaindex, i) == toindex ) {
			haveindex = i;
			break;
		}
	}
	if( haveindex == -1) return;
	
	
	if( !PET_dropPetFollow( charaindex, haveindex, -1, -1, -1 ) ) {
		CHAR_talkToCli( charaindex, -1, "宠物跟随失败！", CHAR_COLORWHITE );
		return;
	}

}

// Nuke start 0624: Hero's bless
void ITEM_useSkup( int charaindex, int toindex, int haveitemindex)
{
	int itemindex;
	itemindex = CHAR_getItemIndex( charaindex, haveitemindex);
	if(!ITEM_CHECKINDEX(itemindex)) return;	
	CHAR_setInt(charaindex,CHAR_SKILLUPPOINT,
	CHAR_getInt(charaindex,CHAR_SKILLUPPOINT)+1);
	CHAR_Skillupsend(charaindex);
	CHAR_talkToCli(charaindex, -1, "你感受到自己的能力被提升了。", CHAR_COLORWHITE);

	CHAR_DelItem( charaindex, haveitemindex);
}
// Nuke end
extern void setNoenemy();
// Nuke start 0626: Dragon's bless
void ITEM_useNoenemy( int charaindex, int toindex, int haveitemindex)
{
	int itemindex,fd;
	itemindex = CHAR_getItemIndex( charaindex, haveitemindex);
	if(!ITEM_CHECKINDEX(itemindex)) return;	
	fd=CHAR_getWorkInt( charaindex, CHAR_WORKFD);
	setNoenemy(fd);
	CHAR_talkToCli(charaindex, -1, "你感受到周边的杀气消失了。", CHAR_COLORWHITE);
	CHAR_DelItem( charaindex, haveitemindex);

}
// Nuke end

// Arminius 7.2: Ra's amulet
void ITEM_equipNoenemy( int charaindex, int itemindex )
{
  char buf[4096];
  int evadelevel;
  int fl,fd;
  
  if( ITEM_CHECKINDEX(itemindex) == FALSE )return;

  if( ITEM_getArgument( ITEM_getChar(itemindex,ITEM_ARGUMENT),"noen", buf, sizeof(buf) )
      == FALSE ){
        return;
  }

  evadelevel=atoi(buf);
  fl=CHAR_getInt(charaindex, CHAR_FLOOR);
  fd=CHAR_getWorkInt( charaindex, CHAR_WORKFD);
  if (evadelevel>=200) {
    setEqNoenemy(fd, 200);
    CHAR_talkToCli(charaindex, -1, "一道奇异的光芒隐藏了你的行踪。", CHAR_COLORWHITE);
    return;
  } else if (evadelevel>=120) {
    setEqNoenemy(fd, 120);

	if ( (fl==100)||(fl==200)||(fl==300)||(fl==400)||(fl==500) ){
	  CHAR_talkToCli(charaindex, -1, "一道奇异的光芒隐藏了你的行踪。", CHAR_COLORWHITE);
      return;
    }
  } else if (evadelevel>=80) {
    setEqNoenemy(fd, 80);
	if ( (fl==100)||(fl==200)||(fl==300)||(fl==400) ){
	
      CHAR_talkToCli(charaindex, -1, "一道奇异的光芒隐藏了你的行踪。", CHAR_COLORWHITE);
      return;
    }
  } else if (evadelevel>=40) {
    setEqNoenemy(fd, 40);
    if ( (fl==100)||(fl==200) ) {
      CHAR_talkToCli(charaindex, -1, "一道奇异的光芒隐藏了你的行踪。", CHAR_COLORWHITE);
      return;
    }
  }
  CHAR_talkToCli(charaindex, -1, "什麽事也没有发生。", CHAR_COLORWHITE);
}

#ifdef _Item_MoonAct
void ITEM_randEnemyEquipOne( int charaindex, int toindex, int haveitemindex)
{
	int itemindex, RandNum=0;
	char buf[256];

	itemindex = CHAR_getItemIndex( charaindex, haveitemindex);
	if( !ITEM_CHECKINDEX( itemindex)) return;

	if( ITEM_getArgument( ITEM_getChar( itemindex,ITEM_ARGUMENT), "rand", buf, sizeof(buf) ) == FALSE ){
		return;
	}

	if( (RandNum=atoi( buf)) > 0 ){
		int fd = CHAR_getWorkInt( charaindex, CHAR_WORKFD);
		setEqRandenemy(fd, RandNum);
		CHAR_talkToCli(charaindex, -1, "遇敌率降低了。", CHAR_COLORWHITE);
		sprintf( buf, "道具 %s消失了。", ITEM_getChar( itemindex, ITEM_NAME) );
		CHAR_talkToCli( charaindex, -1, buf, CHAR_COLORYELLOW);
		CHAR_DelItem( charaindex, haveitemindex);
		return;
	}


}

void ITEM_randEnemyEquip( int charaindex, int itemindex)
{
  char buf[4096];
  int RandNum=0;
  int fd;
  
  if( ITEM_CHECKINDEX(itemindex) == FALSE )return;
  if( ITEM_getArgument( ITEM_getChar(itemindex,ITEM_ARGUMENT),"rand", buf, sizeof(buf) )
      == FALSE ){
        return;
  }

  RandNum=atoi( buf);
  fd=CHAR_getWorkInt( charaindex, CHAR_WORKFD);
  if (RandNum > 0 ) {
    setEqRandenemy(fd, RandNum);
    CHAR_talkToCli(charaindex, -1, "遇敌率降低了。", CHAR_COLORWHITE);
    return;
  }

}
void ITEM_RerandEnemyEquip( int charaindex, int itemindex)
{
  int RandNum=0;
  int fd;
  
  if( ITEM_CHECKINDEX(itemindex) == FALSE )return;

  fd=CHAR_getWorkInt( charaindex, CHAR_WORKFD);
  RandNum = getEqRandenemy( fd);

  if (RandNum > 0 ) {
    clearEqRandenemy( fd);
    CHAR_talkToCli(charaindex, -1, "遇敌率回复。", CHAR_COLORWHITE);
    return;
  }
}
#endif

#ifdef _CHIKULA_STONE
void ITEM_ChikulaStone( int charaindex, int toindex, int haveitemindex)
{
	int itemindex,fd;
	char itemarg[256];

	if( !CHAR_CHECKINDEX( charaindex) ) return;
	itemindex = CHAR_getItemIndex( charaindex, haveitemindex);
	if( !ITEM_CHECKINDEX( itemindex) ) return;

	fd = CHAR_getWorkInt( charaindex, CHAR_WORKFD);
	CHAR_setWorkInt( charaindex, CHAR_WORKCHIKULAHP, 0 );
	CHAR_setWorkInt( charaindex, CHAR_WORKCHIKULAMP, 0 );
	setChiStone( fd, 0);	//1hp 2mp

	if( ITEM_getArgument( ITEM_getChar( itemindex, ITEM_ARGUMENT),"hp", itemarg, sizeof(itemarg) ) != FALSE ){
		setChiStone( fd, 1);
		CHAR_setWorkInt( charaindex, CHAR_WORKCHIKULAHP, atoi( itemarg) );
	}else if( ITEM_getArgument( ITEM_getChar( itemindex, ITEM_ARGUMENT),"mp", itemarg, sizeof(itemarg) ) != FALSE ){
		setChiStone( fd, 2);
		CHAR_setWorkInt( charaindex, CHAR_WORKCHIKULAMP, atoi( itemarg) );
	}else{
	}

	CHAR_talkToCli(charaindex, -1, "接受奇克拉的祝福。", CHAR_COLORWHITE);
	CHAR_setItemIndex(charaindex, haveitemindex ,-1);
	CHAR_sendItemDataOne( charaindex, haveitemindex);
	ITEM_endExistItemsOne(itemindex);
}
#endif

#ifdef _ITEM_ORNAMENTS
void ITEM_PutOrnaments( int charaindex, int itemindex)
{
	char *arg=NULL;
	char itemname[256];
	int bbnums=0;
	if( !CHAR_CHECKINDEX( charaindex) ) return;
	if( !ITEM_CHECKINDEX( itemindex) ) return;
	arg = ITEM_getChar( itemindex, ITEM_ARGUMENT);
	if( arg == "\0" ) return;//ITEM_BASEIMAGENUMBER
	bbnums = atoi( arg);
	ITEM_setInt( itemindex, ITEM_BASEIMAGENUMBER, bbnums);
	ITEM_setWorkInt( itemindex, ITEM_CANPICKUP, 1);
	
	sprintf( itemname,"%s%s%s",CHAR_getChar( charaindex, CHAR_NAME), "的", ITEM_getChar( itemindex, ITEM_SECRETNAME));
	ITEM_setChar( itemindex, ITEM_SECRETNAME, itemname);
}
#endif

#ifdef _SUIT_ITEM

void ITEM_CheckSuitEquip( int charaindex)
{
	int i, j, itemindex, defCode=-1, same=0;
	int nItem[CHAR_STARTITEMARRAY];
	int maxitem;
	struct tagIntSuit{
		char fun[256];
		int intfun;	//CHAR_getInt
	};
	struct tagIntSuit ListSuit[]={
		{"VIT",CHAR_WORKSUITVIT},
		{"FSTR",CHAR_WORKSUITMODSTR},
		{"MSTR",CHAR_WORKSUITSTR},
		{"MTGH",CHAR_WORKSUITTGH},
		{"MDEX",CHAR_WORKSUITDEX},	
		{"HP", CHAR_WORKROUNDHP},
		{"MP", CHAR_WORKROUNDMP}
#ifdef _SUIT_ADDENDUM
	   ,{"RESIST",CHAR_WORKRESIST},   //异常抗性率
		{"COUNTER",CHAR_WORKCOUNTER}, //反击率
		{"M_POW",CHAR_WORKMPOWER}      //加强法师的魔法
#endif
#ifdef _SUIT_TWFWENDUM
	   ,{"EARTH",CHAR_WORK_EA},   //地
	   {"WRITER",CHAR_WORK_WR},   //水
	   {"FIRE",CHAR_WORK_FI},   //火
	   {"WIND",CHAR_WORK_WI}     //风
#endif
#ifdef _SUIT_ADDPART3
	   ,{"WDUCKPOWER",CHAR_WORKDUCKPOWER},//套装回避
	   {"RENOCASE",CHAR_WORKRENOCAST}, //沉默抗性率
	   {"SUITSTRP",CHAR_WORKSUITSTR_P},//攻提升 单位为%
	   {"SUITTGH_P",CHAR_WORKSUITTGH_P},//防提升 单位为%
	   {"SUITDEXP",CHAR_WORKSUITDEX_P}//敏提升 单位为%
#endif
	};
	maxitem = sizeof(ListSuit)/sizeof(ListSuit[0]);
	CHAR_setWorkInt( charaindex, CHAR_WORKSUITITEM, 0);
	for( i=0; i<maxitem/*MAX_SUITTYPE*/; i++)	{
		CHAR_setWorkInt( charaindex, ListSuit[i].intfun, 0 );
	}
	j=0;
	for( i=0; i<CHAR_STARTITEMARRAY; i++){
		nItem[i] = -1;
		itemindex = CHAR_getItemIndex( charaindex ,i);
		if( !ITEM_CHECKINDEX( itemindex) ) continue;
		nItem[j++] = ITEM_getInt( itemindex, ITEM_SUITCODE);
	}
	for( i=0; i<j && defCode==-1; i++){
		int k;
		same = 0;
		if( nItem[i] <= 0 ) continue;
		for( k=(j-1); k>=0; k-- ){
			if( nItem[i] == nItem[k] ) same++;
		}
		if( same >= 3 && nItem[i] != 0 )defCode = nItem[i];
	}
	if( defCode == -1 ) return;
	CHAR_setWorkInt( charaindex, CHAR_WORKSUITITEM, defCode);
	for( i=0; i<CHAR_STARTITEMARRAY; i++){
		char *buf, buf1[256];
		itemindex = CHAR_getItemIndex( charaindex ,i);
		if( !ITEM_CHECKINDEX( itemindex) ) continue;
		if( ITEM_getInt( itemindex, ITEM_SUITCODE) == defCode ){
			for( j=0; j<maxitem/*MAX_SUITTYPE*/; j++){
				buf = ITEM_getChar( itemindex, ITEM_ARGUMENT);
				if( strstr( buf, ListSuit[j].fun) == NULL ) continue;
				if( NPC_Util_GetStrFromStrWithDelim( buf, ListSuit[j].fun, buf1, sizeof( buf1)) == NULL )continue;
				CHAR_setWorkInt( charaindex, ListSuit[j].intfun, atoi( buf1));
			}
		}
	}
	CHAR_complianceParameter( charaindex );
}

void ITEM_suitEquip( int charaindex, int itemindex)
{
	ITEM_CheckSuitEquip( charaindex);
}

void ITEM_ResuitEquip( int charaindex, int itemindex)
{
	ITEM_CheckSuitEquip( charaindex);
}
#endif//_SUIT_ITEM

void ITEM_remNoenemy( int charaindex, int itemindex )
{
  int fd=CHAR_getWorkInt( charaindex, CHAR_WORKFD);
  int el=getEqNoenemy(fd);
  int fl=CHAR_getInt(charaindex, CHAR_FLOOR);

  if( ITEM_CHECKINDEX(itemindex) == FALSE )return;

  clearEqNoenemy(CHAR_getWorkInt(charaindex, CHAR_WORKFD));
  if (el>=200) {
    CHAR_talkToCli(charaindex, -1, "环绕着你的光芒消失了。", CHAR_COLORWHITE);
    return;
  } else if (el>=120) {
    if ( (fl==100)||(fl==200)||(fl==300)||(fl==400)||(fl==500) ) {
      CHAR_talkToCli(charaindex, -1, "环绕着你的光芒消失了。", CHAR_COLORWHITE);
      return;
    }
  } else if (el>=80) {
    if ( (fl==100)||(fl==200)||(fl==300)||(fl==400) ) {
      CHAR_talkToCli(charaindex, -1, "环绕着你的光芒消失了。", CHAR_COLORWHITE);
      return;
    }
  } else if (el>=40) {
    if ( (fl==100)||(fl==200) ) {
      CHAR_talkToCli(charaindex, -1, "环绕着你的光芒消失了。", CHAR_COLORWHITE);
      return;
    }
  }
  CHAR_talkToCli(charaindex, -1, "什麽事也没有发生。", CHAR_COLORWHITE);
}

extern void setStayEncount(int fd);
void ITEM_useEncounter( int charaindex, int toindex, int haveitemindex)
{
	int itemindex,fd;
	itemindex = CHAR_getItemIndex( charaindex, haveitemindex);
	if(!ITEM_CHECKINDEX(itemindex)) return;	

	fd=CHAR_getWorkInt( charaindex, CHAR_WORKFD);
	setStayEncount(fd);
#ifdef _USER_CHARLOOPS
	{
		Char 	*ch;
		ch  = CHAR_getCharPointer( charaindex);
		if( ch == NULL ) return;
		strcpysafe( ch->charfunctable[CHAR_LOOPFUNCTEMP1].string,
			sizeof( ch->charfunctable[CHAR_LOOPFUNCTEMP1]), "CHAR_BattleStayLoop");//战斗
		CHAR_setInt( charaindex, CHAR_LOOPINTERVAL, 2500);
		CHAR_constructFunctable( charaindex);
	}
#endif
	CHAR_talkToCli(charaindex, -1, "你感受到周边突然充满了杀气！", CHAR_COLORYELLOW);
	CHAR_DelItem( charaindex, haveitemindex);

}

#ifdef _Item_DeathAct
void ITEM_UseDeathCounter( int charaindex, int toindex, int haveitemindex)
{
	int itemindex,fd;
	int itemmaxuse=-1;
	char buf1[256];
	char *itemarg;
	char itemnumstr[32];
	int  okfloor = 0;
	BOOL Useflag=FALSE;
	int i = 1;

	itemindex = CHAR_getItemIndex( charaindex, haveitemindex);
	if(!ITEM_CHECKINDEX(itemindex)) return;

#ifdef _ITEM_STONE
	itemarg = ITEM_getChar(itemindex,ITEM_ARGUMENT);
	while(1){
		if( getStringFromIndexWithDelim( itemarg, "|", i , itemnumstr, sizeof( itemnumstr)) == FALSE )
			break;
		okfloor = atoi(itemnumstr);
		if( CHAR_getInt( charaindex, CHAR_FLOOR ) == okfloor ){
			Useflag = TRUE;
			break;
		}
		i++;
	}
	if(okfloor != 0){   // 奇怪的石头
		itemmaxuse = ITEM_getInt( itemindex, ITEM_DAMAGEBREAK);	
		if( itemmaxuse != -1 )	{
			itemmaxuse--;
			ITEM_setInt( itemindex, ITEM_DAMAGEBREAK, itemmaxuse);
			if( itemmaxuse < 1 )	{
				sprintf( buf1, "%s消失了。", ITEM_getChar( itemindex, ITEM_NAME) );
				CHAR_talkToCli(charaindex, -1, buf1, CHAR_COLORYELLOW);
				CHAR_DelItem( charaindex, haveitemindex);
				if(Useflag==FALSE) {
					CHAR_talkToCli(charaindex, -1, "没有发生任何事情！", CHAR_COLORYELLOW);
					return;
				}
			}else{
				sprintf( buf1, "原地遇敌，可使用次数剩余%d次。", itemmaxuse);
				ITEM_setChar( itemindex, ITEM_EFFECTSTRING, buf1);
				CHAR_sendItemDataOne( charaindex, haveitemindex);
				if(Useflag==FALSE) {
					CHAR_talkToCli(charaindex, -1, "没有发生任何事情！", CHAR_COLORYELLOW);
					return;
				}
			}
			fd = CHAR_getWorkInt( charaindex, CHAR_WORKFD);
			setStayEncount(fd);
			CHAR_talkToCli(charaindex, -1, "你身边笼罩阴暗气息！", CHAR_COLORYELLOW);
		}else{
			sprintf( buf1, "%s消失了。", ITEM_getChar( itemindex, ITEM_NAME) );
			CHAR_talkToCli(charaindex, -1, buf1, CHAR_COLORYELLOW);
			CHAR_DelItem( charaindex, haveitemindex);
			if(Useflag==FALSE) {
				CHAR_talkToCli(charaindex, -1, "没有发生任何事情！", CHAR_COLORYELLOW);
				return;
			}
			fd = CHAR_getWorkInt( charaindex, CHAR_WORKFD);
			setStayEncount(fd);
			CHAR_talkToCli(charaindex, -1, "你身边笼罩阴暗气息！", CHAR_COLORYELLOW);		
		}
	}else{		//恶魔宝石
#endif
#ifdef _ITEM_MAXUSERNUM
	itemmaxuse = ITEM_getInt( itemindex, ITEM_DAMAGEBREAK);
	if( itemmaxuse != -1 )	{
		itemmaxuse--;
		ITEM_setInt( itemindex, ITEM_DAMAGEBREAK, itemmaxuse);
		if( itemmaxuse < 1 )	{
			sprintf( buf1, "道具 %s消失了。", ITEM_getChar( itemindex, ITEM_NAME) );
			CHAR_talkToCli(charaindex, -1, buf1, CHAR_COLORYELLOW);
			CHAR_DelItem( charaindex, haveitemindex);
		}else{
			sprintf( buf1, "原地遇敌，可使用次数剩余%d次。", itemmaxuse);
			ITEM_setChar( itemindex, ITEM_EFFECTSTRING, buf1);
			CHAR_sendItemDataOne( charaindex, haveitemindex);
		}
		fd = CHAR_getWorkInt( charaindex, CHAR_WORKFD);
		setStayEncount(fd);
		CHAR_talkToCli(charaindex, -1, "你感受到周边突然充满了杀气！", CHAR_COLORYELLOW);
	}else{
		CHAR_DelItem( charaindex, haveitemindex);
		return;
	}
#else
	fd = CHAR_getWorkInt( charaindex, CHAR_WORKFD);
	setStayEncount(fd);
	sprintf( buf1, "道具 %s消失了。", ITEM_getChar( itemindex, ITEM_NAME) );
	CHAR_talkToCli(charaindex, -1, buf1, CHAR_COLORYELLOW);
	CHAR_DelItem( charaindex, haveitemindex);
#endif
#ifdef _ITEM_STONE
	}
#endif
#ifdef _USER_CHARLOOPS
	{
		Char 	*ch;
		ch  = CHAR_getCharPointer( charaindex);
		if( ch == NULL ) return;
		strcpysafe( ch->charfunctable[CHAR_LOOPFUNCTEMP1].string,
			sizeof( ch->charfunctable[CHAR_LOOPFUNCTEMP1]), "CHAR_BattleStayLoop");//战斗
		CHAR_setInt( charaindex, CHAR_LOOPINTERVAL, 2500);
		CHAR_constructFunctable( charaindex);
	}
#endif
}
#endif

#ifdef _CHRISTMAS_REDSOCKS
void ITEM_useMaxRedSocks( int charaindex, int toindex, int haveitemindex)
{
	int itemtimes = -1, itemindex;
	char *itemarg=NULL;
	int present[13]={ 13061, 13062, 13063, 13064, 13088, 13089, 13090, 13091, //1.
						14756, 17256,
					    13092,19692,20594};
	int nowtimes = time( NULL);

	itemindex = CHAR_getItemIndex( charaindex, haveitemindex);
	if(!ITEM_CHECKINDEX(itemindex)) return;

	itemarg = ITEM_getChar( itemindex, ITEM_ARGUMENT);
	if( itemarg == "\0" ) {
		CHAR_talkToCli(charaindex, -1, "道具无效!", CHAR_COLORYELLOW);
		return;
	}
	itemtimes = atoi( itemarg);
	if( nowtimes >= itemtimes && nowtimes <= itemtimes+(60*60*24) ){ //限制时间内可换礼物
		int si=0, ret;
		char token[256];
		//删除
		CHAR_setItemIndex(charaindex, haveitemindex ,-1);
		CHAR_sendItemDataOne( charaindex, haveitemindex);
		ITEM_endExistItemsOne(itemindex);
		itemindex = -1;
		//新增
		si = rand()%100;
		if( si > 70 ){
			si = rand()%3+10;
		}else if( si > 60 ){
			si = rand()%2+8;
		}else {
			si = rand()%8;
		}
		itemindex = ITEM_makeItemAndRegist( present[ si]);
		if( !ITEM_CHECKINDEX( itemindex)){
			CHAR_talkToCli(charaindex, -1, "礼物无效!", CHAR_COLORYELLOW);
			return;
		}
		ret = CHAR_addItemSpecificItemIndex( charaindex, itemindex);
		if( ret < 0 || ret >= CHAR_MAXITEMHAVE ) {
			ITEM_endExistItemsOne( itemindex);
			CHAR_talkToCli(charaindex, -1, "礼物置放栏位错误!", CHAR_COLORYELLOW);
			return;
		}
		sprintf( token,"拿到%s",ITEM_getChar( itemindex, ITEM_NAME));
		CHAR_talkToCli( charaindex, -1, token, CHAR_COLORYELLOW );
		CHAR_sendItemDataOne( charaindex, ret);
	}else{
		char token[256];
		if( nowtimes < itemtimes ){
			int days, hours, minute, second;
			int defTimes = itemtimes - nowtimes;

			days = defTimes/(24*60*60);
			defTimes = defTimes-( days*(24*60*60));
			hours = defTimes/(60*60);
			defTimes = defTimes-( hours*(60*60));
			minute = defTimes/60;
			defTimes = defTimes-( minute*60);
			second = defTimes;
			sprintf( token,"%s还差%d天%d小时%d分%d秒才可使用!",
				ITEM_getChar( itemindex, ITEM_NAME), days, hours, minute, second);
			CHAR_talkToCli( charaindex, -1, token, CHAR_COLORYELLOW );
		}else if( nowtimes > itemtimes+(60*60*24) ){
			sprintf( token,"%s使用期限已过!", ITEM_getChar( itemindex, ITEM_NAME));
			CHAR_talkToCli( charaindex, -1, token, CHAR_COLORYELLOW );
		}
	}
}
#endif

#ifdef _CHRISTMAS_REDSOCKS_NEW
void ITEM_useMaxRedSocksNew( int charaindex, int toindex, int haveitemindex)
{
	int itemindex;
	char *itemarg=NULL;
	char itemnumstr[32];
	int itemnum=0;
	int present[20];
    int si=0, ret,i;
	char token[256];

	itemindex = CHAR_getItemIndex( charaindex, haveitemindex);
	if(!ITEM_CHECKINDEX(itemindex)) return;

	itemarg = ITEM_getChar( itemindex, ITEM_ARGUMENT);
	if( itemarg == "\0" ){
		CHAR_talkToCli(charaindex, -1, "这是个芭乐圣诞袜!", CHAR_COLORYELLOW);
		return;
	}

	//道具数量
	if( getStringFromIndexWithDelim( itemarg, "|", 1, itemnumstr, sizeof( itemnumstr)) == FALSE )
		return;
    itemnum = atoi(itemnumstr);
	if( itemnum > 20 )
		itemnum = 20;
	for(i=0;i<itemnum;i++){
		if( getStringFromIndexWithDelim( itemarg, "|", 2+i, itemnumstr, sizeof( itemnumstr)) )
		    present[i] = atoi(itemnumstr);
	}
	
	//删除
	CHAR_setItemIndex(charaindex, haveitemindex ,-1);
	CHAR_sendItemDataOne( charaindex, haveitemindex);
	ITEM_endExistItemsOne(itemindex);
	itemindex = -1;
	//新增
	si = rand()%itemnum;

	itemindex = ITEM_makeItemAndRegist( present[ si]);
	if( !ITEM_CHECKINDEX( itemindex)){
		CHAR_talkToCli(charaindex, -1, "礼物无效!", CHAR_COLORYELLOW);
		return;
	}
	ret = CHAR_addItemSpecificItemIndex( charaindex, itemindex);
	if( ret < 0 || ret >= CHAR_MAXITEMHAVE ) {
		ITEM_endExistItemsOne( itemindex);
		CHAR_talkToCli(charaindex, -1, "礼物置放栏位错误!", CHAR_COLORYELLOW);
		return;
	}
	sprintf( token,"拿到%s",ITEM_getChar( itemindex, ITEM_NAME));
	CHAR_talkToCli( charaindex, -1, token, CHAR_COLORYELLOW );
	CHAR_sendItemDataOne( charaindex, ret);

}
#endif

#ifdef _PETSKILL_CANNEDFOOD
void ITEM_useSkillCanned( int charaindex, int toindex, int itemNo)
{
	int itemindex;
	char buf1[256];
	itemindex = CHAR_getItemIndex( charaindex, itemNo);
	if(!ITEM_CHECKINDEX( itemindex)) return;
	if( !CHAR_CHECKINDEX( toindex) ) return;
	if( CHAR_getInt( toindex, CHAR_WHICHTYPE) == CHAR_TYPEPET) {
		int i, petNo=-1;
		for( i=0; i<CHAR_MAXPETHAVE; i++)	{
			if( toindex == CHAR_getCharPet( charaindex, i) ){
				petNo = i;
				break;
			}
		}
		if( petNo == -1 ){
			sprintf( buf1, "%s不在人物身上。", CHAR_getChar( toindex, CHAR_NAME) );
			CHAR_talkToCli( charaindex, -1, buf1, CHAR_COLORYELLOW);
			return;
		}
		ITEM_usePetSkillCanned_PrintWindow( charaindex, petNo);
		CHAR_setWorkInt( charaindex, CHAR_WORKRENAMEITEMNUM, petNo);
		CHAR_setWorkInt( charaindex, CHAR_WORKRENAMEITEMINDEX, itemNo);
	}else{
		sprintf( buf1, "道具 %s仅限宠物使用。", ITEM_getChar( itemindex, ITEM_NAME) );
		CHAR_talkToCli( charaindex, -1, buf1, CHAR_COLORYELLOW);
		return;
	}

}
#endif

#ifdef _ITEM_METAMO
void ITEM_metamo( int charaindex, int toindex, int haveitemindex )
{
	
	char *arg, msg[128];
	int itemindex, metamoTime, haveindex, battlemode, i;

	if( CHAR_CHECKINDEX( charaindex ) == FALSE )	return;
	//print(" PetMetamo_toindex:%d ", toindex);

	if( CHAR_getInt( charaindex, CHAR_RIDEPET ) != -1 ){
		CHAR_talkToCli( charaindex, -1, "无法变身，骑乘中不能变身！", CHAR_COLORYELLOW );
		return;
	}
#ifdef _FIX_METAMORIDE
	if( CHAR_CHECKJOINENEMY( charaindex) == TRUE ){
		CHAR_talkToCli( charaindex, -1, "无法变身，搭乘中不能变身！", CHAR_COLORYELLOW );
		return;
	}
#else
	if( CHAR_getInt( charaindex, CHAR_BASEIMAGENUMBER) == 100259 ){
		CHAR_talkToCli( charaindex, -1, "无法变身，搭乘中不能变身！", CHAR_COLORYELLOW );
		return;
	}
#endif
#ifdef _PETSKILL_BECOMEPIG
    if( CHAR_getInt( charaindex, CHAR_BECOMEPIG) > -1 ){//变成乌力了
		CHAR_talkToCli( charaindex, -1, "无法变身，乌力化中不能变身！", CHAR_COLORYELLOW );
	    return;
	}
#endif


#ifdef _FIXBUG_ATTACKBOW
	{
		int armindex = CHAR_getItemIndex( charaindex, CHAR_ARM);
		if( ITEM_CHECKINDEX( armindex) == TRUE )	{
			int armtype = BATTLE_GetWepon( charaindex);
			if( armtype == ITEM_BOW || armtype == ITEM_BOUNDTHROW ||
				armtype == ITEM_BREAKTHROW || armtype == ITEM_BOOMERANG)	{
				CHAR_talkToCli( charaindex, -1, "使用远距离武器无法变身！", CHAR_COLORYELLOW );
				return;
			}
		}
	}
#endif

	battlemode = CHAR_getWorkInt( charaindex, CHAR_WORKBATTLEMODE );
	// 爵  钒铵凛反  骰允月
	if( battlemode == BATTLE_CHARMODE_INIT ){
		return;
	}
	// 爵    分匀凶日
	if( IsBATTLING( charaindex ) == TRUE ){
		toindex = BATTLE_No2Index(CHAR_getWorkInt( charaindex, CHAR_WORKBATTLEINDEX ), toindex );

	}
	
	if( CHAR_CHECKINDEX( toindex ) == FALSE )	return;

	haveindex = -1;	
	for( i = 0; i < 5; i++ ){
		if( CHAR_getCharPet( charaindex, i) == toindex ) {
			haveindex = i;
			break;
		}
	}
	if( haveindex == -1 && charaindex != toindex ){
		CHAR_talkToCli( charaindex, -1, "无法变身，只能变成自己的宠物！", CHAR_COLORYELLOW );
		return;
	}

	itemindex = CHAR_getItemIndex( charaindex, haveitemindex);
	if(!ITEM_CHECKINDEX(itemindex)) return;
	
	arg = ITEM_getChar(itemindex, ITEM_ARGUMENT );
	if( arg == "\0" )return;
	if( sscanf( arg, "%d", &metamoTime) != 1 )
		return;


	if( toindex != charaindex ){
		CHAR_setWorkInt( charaindex, CHAR_WORKITEMMETAMO, NowTime.tv_sec +metamoTime);
		sprintf( msg, "变身成%s！", CHAR_getChar( toindex, CHAR_NAME) );
	}
	else {
		CHAR_setWorkInt( charaindex, CHAR_WORKITEMMETAMO, 0);
		sprintf( msg, "变回自己！");
	}
	CHAR_talkToCli( charaindex, -1, msg, CHAR_COLORYELLOW );

	CHAR_setInt( charaindex, CHAR_BASEIMAGENUMBER, CHAR_getInt( toindex, CHAR_BASEBASEIMAGENUMBER) );
	CHAR_complianceParameter( charaindex);
	CHAR_sendCToArroundCharacter( CHAR_getWorkInt( charaindex , CHAR_WORKOBJINDEX ));
	CHAR_send_P_StatusString( charaindex , CHAR_P_STRING_BASEBASEIMAGENUMBER);

	CHAR_DelItem( charaindex, haveitemindex);

}
#endif
#ifdef _ITEM_CRACKER
void ITEM_Cracker(int charaindex,int toindex,int haveitemindex)
{
	int battlemode;
	// 检查玩家是否有效
	if(CHAR_CHECKINDEX(charaindex) == FALSE) return; //失败
	battlemode = CHAR_getWorkInt(charaindex,CHAR_WORKBATTLEMODE);
	// 战斗中使用无效
	if(!battlemode) ITEM_useCracker_Effect(charaindex,toindex,haveitemindex);
	else CHAR_talkToCli(charaindex,-1,"什麽也没发生。",CHAR_COLORWHITE);
}
#endif

#ifdef _ITEM_ADDEXP	//vincent 经验提升
void ITEM_Addexp(int charaindex,int toindex,int haveitemindex)
{
	// 检查玩家是否有效
	if(CHAR_CHECKINDEX(charaindex) == FALSE) return; //失败
#if 1
	ITEM_useAddexp_Effect(charaindex,toindex,haveitemindex);
#else
	if( !CHAR_getWorkInt(charaindex,CHAR_WORKITEM_ADDEXP)){
		ITEM_useAddexp_Effect(charaindex,toindex,haveitemindex);
	}else{
		CHAR_talkToCli(charaindex,-1,"先前使用之药效依然存在",CHAR_COLORYELLOW);
	}
#endif
}
#endif

#ifdef _ITEM_REFRESH //vincent 解除异常状态道具
void ITEM_Refresh(int charaindex,int toindex,int haveitemindex)
{
	int battlemode,itemindex;
print("\nvincent--ITEM_Refresh");
		// 检查玩家是否有效
	if(CHAR_CHECKINDEX(charaindex) == FALSE) 
	{
print("\nvincent--(charaindex) == FALSE");
		return; //失败
	}
	itemindex = CHAR_getItemIndex(charaindex,haveitemindex);

	battlemode = CHAR_getWorkInt(charaindex,CHAR_WORKBATTLEMODE);
	if(battlemode)
	{
print("\nvincent--enter ITEM_useAddexp_Effect");
print("\nvincent-->charaindex:%d,toindex:%d",charaindex,toindex);
		ITEM_useRefresh_Effect(charaindex,toindex,haveitemindex);
	}
	else CHAR_talkToCli(charaindex,-1,"什麽也没发生。",CHAR_COLORWHITE);

    /* 平乓仿弁正□及赭    伉旦玄井日壅蛔 */
    CHAR_setItemIndex(charaindex, haveitemindex ,-1);
	CHAR_sendItemDataOne( charaindex, haveitemindex);/* 失奶  丞凳蕙 */
	/* 壅允 */
	ITEM_endExistItemsOne( itemindex );
}
#endif
//Terry 2001/12/21
#ifdef _ITEM_FIRECRACKER
void ITEM_firecracker(int charaindex,int toindex,int haveitemindex)
{
	int battlemode;

	// 检查玩家是否有效
	if(CHAR_CHECKINDEX(charaindex) == FALSE) return; //失败

	battlemode = CHAR_getWorkInt(charaindex,CHAR_WORKBATTLEMODE);
	
	if( battlemode // 检查是否在战斗中
#ifdef _PETSKILL_BECOMEPIG
	    && CHAR_getInt( charaindex, CHAR_BECOMEPIG) == -1 
#endif
		) 
		ITEM_useFirecracker_Battle(charaindex,toindex,haveitemindex);
	else 
		CHAR_talkToCli(charaindex,-1,"什麽也没发生。",CHAR_COLORWHITE);
}
#endif
//Terry end


void ITEM_WearEquip( int charaindex, int itemindex)
{
	// WON ADD
//	if( ITEM_getInt(itemindex,ITEM_ID) == 20130 ){
		CHAR_setWorkInt( charaindex, CHAR_PickAllPet, TRUE);
///	}
	return;
}
void ITEM_ReWearEquip( int charaindex, int itemindex)
{
	CHAR_setWorkInt( charaindex, CHAR_PickAllPet, FALSE);
	return;
}


#ifdef _Item_ReLifeAct
void ITEM_DIErelife( int charaindex, int itemindex, int eqw)
{
	int ReceveEffect=-1;
	int toNo;
	int battleindex=-1;
	int attackNo=-1;
	int WORK_HP=1;
	char buf[256];
	battleindex = CHAR_getWorkInt( charaindex, CHAR_WORKBATTLEINDEX );
	if( BATTLE_CHECKINDEX( battleindex ) == FALSE )	{
		print("\n battleindex =%d return", battleindex);
		return;
	}
	if( CHAR_CHECKINDEX( charaindex) == FALSE )
		return;
	if( ITEM_CHECKINDEX( itemindex) == FALSE )
		return;
#ifdef _DUMMYDIE
	if( CHAR_getFlg( charaindex, CHAR_ISDUMMYDIE) == FALSE ) {
#else
	if( CHAR_getFlg( charaindex, CHAR_ISDIE) == FALSE )	{
#endif
		print("\n [ %d, CHAR_ISDIE DUMMY FALSE] return !", charaindex);
		return;
	}

	if( ITEM_getArgument( ITEM_getChar(itemindex,ITEM_ARGUMENT),"HP", buf, sizeof(buf) )
		== FALSE ){
		WORK_HP = 1;
	}else	{
		if( !strcmp( buf, "FULL") )	{
			WORK_HP = CHAR_getWorkInt( charaindex, CHAR_WORKMAXHP );
		}else	{
			WORK_HP=atoi( buf);
		}
	}

	ReceveEffect = SPR_fukkatu3;
	toNo = BATTLE_Index2No( battleindex, charaindex );
	attackNo = -1;

	BATTLE_MultiReLife( battleindex, attackNo, toNo, WORK_HP, ReceveEffect );
	CHAR_setItemIndex( charaindex, eqw ,-1);
	ITEM_endExistItemsOne( itemindex);
	CHAR_sendItemDataOne( charaindex, eqw);
	return;
}
#endif

#ifdef _EQUIT_DEFMAGIC
void ITEM_MagicEquitWear( int charaindex, int itemindex)
{
	char buf[256];
	char *itemarg;
	char Free[][128]={"EA","WA","FI","WI","QU"};
	int index=0;
	int dMagic=0;
	itemarg = ITEM_getChar(itemindex,ITEM_ARGUMENT);
	if( itemarg == "\0" ) return;
	while( index < arraysizeof( Free) )	{
		if( ITEM_getArgument( itemarg, Free[ index], buf, sizeof(buf)) == TRUE )	{
			dMagic = atoi( buf);
			if( dMagic <= 100 && dMagic >= -100 )	{
				int def_magic = CHAR_getWorkInt( charaindex, CHAR_EQUITDEFMAGIC_E+index);
				CHAR_setWorkInt( charaindex, CHAR_EQUITDEFMAGIC_E+index, def_magic + dMagic);
			}
			dMagic = 0;
		}
		index++;
		if( (CHAR_EQUITDEFMAGIC_E+index) > CHAR_EQUITQUIMAGIC )
			break;
	}

	return;
}

void ITEM_MagicEquitReWear( int charaindex, int itemindex){
	char buf[256];
	char *itemarg;
	char Free[][128]={"EA","WA","FI","WI","QU"};
	int index=0;
	int dMagic=0;

	itemarg = ITEM_getChar(itemindex,ITEM_ARGUMENT);
	if( itemarg == "\0" ) return;
	while( index < arraysizeof( Free) )	{
		if( ITEM_getArgument( itemarg, Free[ index], buf, sizeof(buf)) == TRUE )	{
			dMagic = atoi( buf);
			if( dMagic <= 100 && dMagic >= -100 )	{
				int def_magic = CHAR_getWorkInt( charaindex, CHAR_EQUITDEFMAGIC_E+index);
				CHAR_setWorkInt( charaindex, CHAR_EQUITDEFMAGIC_E+index, def_magic - dMagic);
			}
			dMagic = 0;
		}
		index++;
		if( (CHAR_EQUITDEFMAGIC_E+index) > CHAR_EQUITQUIMAGIC )
			break;
	}

	return;	
}
#endif

#ifdef _ITEM_WARP_FIX_BI
void recoverbi(int index)
{
	int eBbi=-1, eArm=-1, eNum=-1, eBi=-1, bi=-1;	

	bi   = CHAR_getInt( index, CHAR_BASEIMAGENUMBER);	
	eBbi = CHAR_getInt( index, CHAR_BASEBASEIMAGENUMBER);
	eArm = CHAR_getItemIndex( index, CHAR_ARM);
	eNum = ITEM_FIST;
	if(ITEM_CHECKINDEX(eArm))
		eNum = ITEM_getInt( eArm, ITEM_TYPE);
	
	eBi = CHAR_getNewImagenumberFromEquip( eBbi, eNum);	

	if(CHAR_getInt( index, CHAR_RIDEPET)!=-1)	     //骑宠			
		CHAR_complianceParameter(index);
	else                                             //非骑宠		
		if((eBi!=-1)&&(eBi!=bi))
			CHAR_setInt( index, CHAR_BASEIMAGENUMBER, eBi);
}
#endif

#ifdef _ITEM_TIME_LIMIT
void ITEM_TimeLimit( int charaindex)
{
    int  i      = 0;
	int  iid    = 0;
	long lTime  = 0;
	char buff[256];

	if( !CHAR_CHECKINDEX( charaindex) ) return;
	for( i=0; i < CHAR_MAXITEMHAVE ; i++ ){
		int itemindex = CHAR_getItemIndex( charaindex , i );
		if( !ITEM_CHECKINDEX( itemindex ) ) continue;
		lTime = ITEM_getWorkInt( itemindex, ITEM_WORKTIMELIMIT);
		if( ITEM_getInt( itemindex, ITEM_ID) == 20173 //燃烧火把
			|| ITEM_getInt( itemindex, ITEM_ID) == 20704 ){
			if( lTime > 0 && NowTime.tv_sec > lTime ){
				iid = ITEM_getInt( itemindex, ITEM_ID) + 1;
				snprintf( buff, sizeof( buff), "%s的效果已消失..", ITEM_getChar( itemindex, ITEM_NAME));
				CHAR_talkToCli( charaindex, -1, buff, CHAR_COLORGREEN);
				CHAR_DelItemMess( charaindex, i, 0);
				itemindex = ITEM_makeItemAndRegist( iid);
				if(itemindex!=-1){
					CHAR_setItemIndex( charaindex, i, itemindex);
					ITEM_setWorkInt( itemindex, ITEM_WORKOBJINDEX, -1);
					ITEM_setWorkInt( itemindex, ITEM_WORKCHARAINDEX, charaindex);
					CHAR_sendItemDataOne( charaindex, i);
				}
			}
		}
	}	

}
#endif

#ifdef _ITEM_CONSTITUTION
void ITEM_Constitution( int charaindex, int toindex, int haveitemindex)
{
	char buf[256];
	char *itemarg;
	char Free[][128]={"VI","ST","TG","DE"};
	int index=0, FixPoint=0, itemindex;
	BOOL FIXs=FALSE;
	int AllPoint=0;

	if( CHAR_getInt( charaindex, CHAR_WHICHTYPE) != CHAR_TYPEPLAYER )
		return;
	itemindex = CHAR_getItemIndex( charaindex, haveitemindex);
	if(!ITEM_CHECKINDEX(itemindex)) return;

	itemarg = ITEM_getChar( itemindex, ITEM_ARGUMENT);
	if( itemarg == "\0" ) return;

	AllPoint = CHAR_getInt( charaindex, CHAR_VITAL)
		+CHAR_getInt( charaindex, CHAR_VITAL+1)
		+CHAR_getInt( charaindex, CHAR_VITAL+2)
		+CHAR_getInt( charaindex, CHAR_VITAL+3);

	while( index < arraysizeof( Free) )	{
		if( ITEM_getArgument( itemarg, Free[ index], buf, sizeof(buf)) == TRUE )	{
			FixPoint = atoi( buf);
			if( FixPoint <= 100 && FixPoint > 0 )	{
				int points = CHAR_getInt( charaindex, CHAR_VITAL+index);
				//Change Fix
				//if( points < (FixPoint*100) ) break;
				if( points < (FixPoint*100) || AllPoint-(FixPoint*100) <= 0 ) 
				{
					CHAR_talkToCli( charaindex, -1, "物品无效。", CHAR_COLORYELLOW);
					return;
				}
				CHAR_setInt( charaindex, CHAR_VITAL+index, (points-(FixPoint*100)));
				CHAR_setInt( charaindex, CHAR_SKILLUPPOINT, CHAR_getInt( charaindex, CHAR_SKILLUPPOINT) + FixPoint);
				FIXs = TRUE;
			}
			FixPoint = 0;
		}
		index++;
		if( (CHAR_VITAL+index) > CHAR_DEX )
			break;
	}
	CHAR_complianceParameter( charaindex);
	CHAR_send_P_StatusString(  charaindex,
		CHAR_P_STRING_MAXHP|CHAR_P_STRING_HP|CHAR_P_STRING_LV|CHAR_P_STRING_EXP|
		CHAR_P_STRING_ATK|CHAR_P_STRING_DEF|CHAR_P_STRING_QUICK|
		CHAR_P_STRING_VITAL|CHAR_P_STRING_STR|CHAR_P_STRING_TOUGH|CHAR_P_STRING_DEX);

	CHAR_Skillupsend( charaindex);
	if( FIXs == TRUE )	{
		sprintf( buf, "%s", "你的身体感到变化。");
	}else	{
		sprintf( buf, "%s", "物品无效。");
	}
	CHAR_talkToCli( charaindex, -1, buf, CHAR_COLORYELLOW);
	CHAR_DelItem( charaindex, haveitemindex);
}
#endif

#ifdef _NEW_RIDEPETS
void ITEM_useLearnRideCode( int charaindex, int toindex, int haveitemindex)
{//CHAR_LOWRIDEPETS
	
	if(getRideMode()==2){
		CHAR_talkToCli( charaindex, -1, "服务端为2.0非骑证模式，所以你无法使用该物品", CHAR_COLORYELLOW);
		return;
	}
	int itemindex, i;
	char buf1[256];
	char *itemarg=NULL;
	int ridetrans;
	typedef struct{
		char arg[32];
		int Code;
	}tagNewRideCode;
	tagNewRideCode NewRides[]={
		{ "RIDE_PET0",  RIDE_PET0},  { "RIDE_PET1",  RIDE_PET1},  { "RIDE_PET2",  RIDE_PET2},
		{ "RIDE_PET3",  RIDE_PET3},  { "RIDE_PET4",  RIDE_PET4},  { "RIDE_PET5",  RIDE_PET5},
		{ "RIDE_PET6",  RIDE_PET6},  { "RIDE_PET7",  RIDE_PET7},  { "RIDE_PET8",  RIDE_PET8},
		{ "RIDE_PET9",  RIDE_PET9},  { "RIDE_PET10", RIDE_PET10}, { "RIDE_PET11", RIDE_PET11},
		{ "RIDE_PET12", RIDE_PET12}, { "RIDE_PET13", RIDE_PET13}, { "RIDE_PET14", RIDE_PET14},
#ifdef _RIDE_CF
		{ "RIDE_PET15", RIDE_PET15}, { "RIDE_PET16", RIDE_PET16}, { "RIDE_PET17", RIDE_PET17},
	  { "RIDE_PET18", RIDE_PET18}, { "RIDE_PET19", RIDE_PET19}, { "RIDE_PET20", RIDE_PET20},
	  { "RIDE_PET21", RIDE_PET21}, { "RIDE_PET22", RIDE_PET22}, { "RIDE_PET23", RIDE_PET23},
	  { "RIDE_PET24", RIDE_PET24}, { "RIDE_PET25", RIDE_PET25}, { "RIDE_PET26", RIDE_PET26},
	  { "RIDE_PET27", RIDE_PET27}, { "RIDE_PET28", RIDE_PET28}, { "RIDE_PET29", RIDE_PET29},
	  { "RIDE_PET30", RIDE_PET30}, { "RIDE_PET31", RIDE_PET31},
	  { "RIDE_PET32", RIDE_PET0 }, { "RIDE_PET33", RIDE_PET1 }, { "RIDE_PET34", RIDE_PET2 }, 
	  { "RIDE_PET35", RIDE_PET3 }, { "RIDE_PET36", RIDE_PET4 }, { "RIDE_PET37", RIDE_PET5 }, 
	  { "RIDE_PET38", RIDE_PET6 }, { "RIDE_PET39", RIDE_PET7 }, { "RIDE_PET40", RIDE_PET8 }, 
	  { "RIDE_PET41", RIDE_PET9 }, { "RIDE_PET42", RIDE_PET10}, { "RIDE_PET43", RIDE_PET11}, 
	  { "RIDE_PET44", RIDE_PET12}, { "RIDE_PET45", RIDE_PET13}, { "RIDE_PET46", RIDE_PET14}, 
	  { "RIDE_PET47", RIDE_PET15}, { "RIDE_PET48", RIDE_PET16}, { "RIDE_PET49", RIDE_PET17}, 
	  { "RIDE_PET50", RIDE_PET18}, { "RIDE_PET51", RIDE_PET19}, { "RIDE_PET52", RIDE_PET20}, 
	  { "RIDE_PET53", RIDE_PET21}, { "RIDE_PET54", RIDE_PET22}, { "RIDE_PET55", RIDE_PET23}, 
	  { "RIDE_PET56", RIDE_PET24}, { "RIDE_PET57", RIDE_PET25}, { "RIDE_PET58", RIDE_PET26}, 
	  { "RIDE_PET59", RIDE_PET27}, { "RIDE_PET60", RIDE_PET28}, { "RIDE_PET61", RIDE_PET29}, 
	  { "RIDE_PET62", RIDE_PET30}, { "RIDE_PET63", RIDE_PET31}
#endif
	};

	itemindex = CHAR_getItemIndex( charaindex, haveitemindex);
	if(!ITEM_CHECKINDEX(itemindex)) return;
	itemarg = ITEM_getChar( itemindex, ITEM_ARGUMENT);
	if( itemarg == "\0" ) return;

	if( getStringFromIndexWithDelim(itemarg,"|", 3, buf1, sizeof(buf1)) == FALSE )
		ridetrans=0;
	else
		ridetrans=atoi(buf1);
	if( getStringFromIndexWithDelim(itemarg,"|", 1, buf1, sizeof(buf1)) == FALSE )
		return;
	for( i=0; i<MAXNOINDEX; i++){
		if( !strcmp( NewRides[i].arg, buf1))	{
			int LRCode;
			if(CHAR_getInt( charaindex, CHAR_TRANSMIGRATION) < ridetrans){
				char token[256];
				if( getStringFromIndexWithDelim(itemarg,"|", 2, buf1, sizeof(buf1)) != FALSE ){
					sprintf( token, "必须%d转人以上才能学习骑%s。",ridetrans,buf1);
					CHAR_talkToCli( charaindex, -1, token, CHAR_COLORYELLOW);
					return;
				}
			}
#ifdef _RIDE_CF
			if(i<MAXNOINDEX/2){
				LRCode = CHAR_getInt( charaindex, CHAR_LOWRIDEPETS);
				LRCode = LRCode|NewRides[i].Code;
				CHAR_setInt( charaindex, CHAR_LOWRIDEPETS, LRCode);
			}else{
				LRCode = CHAR_getInt( charaindex, CHAR_LOWRIDEPETS1);
				LRCode = LRCode|NewRides[i].Code;
				CHAR_setInt( charaindex, CHAR_LOWRIDEPETS1, LRCode);
			}
#else
			LRCode = CHAR_getInt( charaindex, CHAR_LOWRIDEPETS);
			LRCode = LRCode|NewRides[i].Code;
			CHAR_setInt( charaindex, CHAR_LOWRIDEPETS, LRCode);
#endif
			if( getStringFromIndexWithDelim(itemarg,"|", 2, buf1, sizeof(buf1)) != FALSE ){
				char token[256];

				sprintf( token, "学习了新的骑宠 (%s)。", buf1);
				CHAR_talkToCli( charaindex, -1, token, CHAR_COLORYELLOW);
				CHAR_DelItem( charaindex, haveitemindex);
				CHAR_sendStatusString( charaindex, "x");
			}
			break;
		}
	}
}

#endif
#ifdef _THROWITEM_ITEMS
static int Niceitem = 0;
	int golds[3]={ 10000, 20000, 50000};
	int items1[18] = { 13092, 13091, 20439, 20417, 1284, 20172, 18210, 19014, 18360, 18362, 18364,
						18359, 18356, 18357, 18510, 20418, 20419, 1452};
	int items2[11] = { 15842, 16136, 14334, 14034, 14634, 14934, 15534, 14934, 16432, 17057, 19695};

	int items3[10] = { 16014, 16314, 14515, 14215, 14815, 15115, 15715, 15295, 16552, 17157};

	int items4[18] = { 14516, 14513, 14216, 14213, 14816, 14813, 15116, 15716, 15415, 17360, 20279,
						20282, 20276, 20270, 20288, 20290, 20291, 20289};
	int items5[5] = { 20280, 20283, 20277, 20271, 20274};
	int items6[5] = { 20284, 20272, 20275, 20281, 20278};
void ITEM_ThrowItemBox( int charaindex, int toindex, int haveitemindex)
{
	int i, ret, Iindex, ItemID=-1, itemindex;
	char token[256];

	if( !CHAR_CHECKINDEX(charaindex) ) return;
	itemindex = CHAR_getItemIndex( charaindex, haveitemindex);
	if( !ITEM_CHECKINDEX(itemindex) )  return;

	if( Niceitem > 10 ) ret = rand()%920;
	else ret = rand()%1000;

	for( i = CHAR_STARTITEMARRAY ; i < CHAR_MAXITEMHAVE ; i++ ){
		Iindex = CHAR_getItemIndex( charaindex , i );
		if( !ITEM_CHECKINDEX(itemindex) ) continue;
		if( itemindex == Iindex ){
			CHAR_DelItem( charaindex, i);

			LogItem(
				CHAR_getChar( charaindex, CHAR_NAME ), CHAR_getChar( charaindex, CHAR_CDKEY ),
#ifdef _add_item_log_name  // WON ADD 在item的log中增加item名称
				itemindex,
#else
				ITEM_getInt( itemindex, ITEM_ID),
#endif
				"WarpManDelItem",
				CHAR_getInt( charaindex, CHAR_FLOOR), CHAR_getInt( charaindex, CHAR_X ),
				CHAR_getInt( charaindex, CHAR_Y ), ITEM_getChar( itemindex, ITEM_UNIQUECODE),
				ITEM_getChar( itemindex, ITEM_NAME),
				ITEM_getInt( itemindex, ITEM_ID)
			);
			break;
		}
	}
	if( i >= CHAR_MAXITEMHAVE ){
		return;
	}

	if( ret <= 774 ){
		int Golds=0;
		Golds = golds[ RAND(0,2)];
		CHAR_AddGold( charaindex, Golds);
	}else {
		if( ret < 924 ){
			ItemID = items1[RAND(0,17)];
		}else if( ret < 964 ){
			ItemID = items2[RAND(0,10)];
		}else if( ret < 984 ){
			ItemID = items3[RAND(0,9)];
			Niceitem++;
		}else if( ret < 994 ){
			ItemID = items4[RAND(0,17)];
			Niceitem++;
		}else if( ret < 999 ){
			ItemID = items5[RAND(0,4)];
			Niceitem++;
		}else {
			ItemID = items6[RAND(0,4)];
			Niceitem++;
		}

		Iindex = ITEM_makeItemAndRegist( ItemID);
		if( !ITEM_CHECKINDEX( Iindex) ){
			return;
		}
		ret = CHAR_addItemSpecificItemIndex( charaindex, Iindex);
		if( ret < 0 || ret >= CHAR_MAXITEMHAVE ) {
			ITEM_endExistItemsOne( Iindex);
			return;
		}
		sprintf( token,"拿到%s", ITEM_getChar( Iindex, ITEM_NAME));
		CHAR_talkToCli( charaindex, -1, token, CHAR_COLORYELLOW );
		CHAR_sendItemDataOne( charaindex, ret);
	}
}
#endif

#ifdef _ITEM_LOVERPARTY
void ITEM_LoverSelectUser( int charaindex, int toindex, int haveitemindex)
{
	int itemindex, i;
	int playernum = CHAR_getPlayerMaxNum();

	if( !CHAR_CHECKINDEX( charaindex ) )  return;
	if( !CHAR_CHECKINDEX( toindex ) )  return;
	if( CHAR_getInt( toindex, CHAR_WHICHTYPE) != CHAR_TYPEPLAYER){
		CHAR_talkToCli( charaindex, -1, "物品仅限人物使用。", CHAR_COLORYELLOW);
		return;
	}
	itemindex = CHAR_getItemIndex( charaindex, haveitemindex);
	if( !ITEM_CHECKINDEX(itemindex) )  return;
	if( ITEM_getInt( itemindex, ITEM_TYPE) != 16 ){
		ITEM_setInt( itemindex, ITEM_TYPE, 16);
		return;
	}
	if( !strcmp( ITEM_getChar( itemindex, ITEM_FORUSERNAME), "") ||
		!strcmp( ITEM_getChar( itemindex, ITEM_FORUSERCDKEY), "") ){//寻找设定对象
		if( charaindex == toindex ){
			CHAR_talkToCli( charaindex, -1, "不能选择自己使用。", CHAR_COLORYELLOW);
			return;
		}
		ITEM_setChar( itemindex, ITEM_FORUSERNAME, CHAR_getChar( toindex, CHAR_NAME) );
		ITEM_setChar( itemindex, ITEM_FORUSERCDKEY, CHAR_getChar( toindex, CHAR_CDKEY) );
		ITEM_setInt( itemindex, ITEM_TARGET, 0);
		{
			char token[256];
			sprintf( token, "%s(%s)",
				ITEM_getChar( itemindex, ITEM_SECRETNAME), CHAR_getChar( toindex, CHAR_NAME));
			ITEM_setChar( itemindex, ITEM_SECRETNAME, token);
			sprintf( token, "传送对象设定为%s。", CHAR_getChar( toindex, CHAR_NAME));
			CHAR_talkToCli( charaindex, -1, token, CHAR_COLORYELLOW);
		}
		CHAR_sendItemDataOne( charaindex, haveitemindex);
	}else{
		char buf1[256];
		//if( CHAR_getInt( charaindex, CHAR_FLOOR) == 117 || CHAR_getInt( charaindex, CHAR_FLOOR) == 887 ||
		//CHAR_getInt( charaindex, CHAR_FLOOR) == 1042 || CHAR_getInt( charaindex, CHAR_FLOOR) == 2032 ||
		//CHAR_getInt( charaindex, CHAR_FLOOR) == 3032 || CHAR_getInt( charaindex, CHAR_FLOOR) == 4032 ||
		//CHAR_getInt( charaindex, CHAR_FLOOR) == 5032 || CHAR_getInt( charaindex, CHAR_FLOOR) == 6032 ||
		//CHAR_getInt( charaindex, CHAR_FLOOR) == 7032 || CHAR_getInt( charaindex, CHAR_FLOOR) == 8032 ||
		//CHAR_getInt( charaindex, CHAR_FLOOR) == 9032 ){ 
		if( checkUnlawWarpFloor( CHAR_getInt( charaindex, CHAR_FLOOR) ) ) {
			CHAR_talkToCli( charaindex, -1, "你的所在位置无法传送。", CHAR_COLORYELLOW );
			return;
		}

		for( i=0; i<playernum; i++)	{
			int itemmaxuse=0;
			if( !CHAR_CHECKINDEX( i) ) continue;
			if( !strcmp( ITEM_getChar( itemindex, ITEM_FORUSERNAME), CHAR_getChar( i, CHAR_NAME)) &&
				!strcmp( ITEM_getChar( itemindex, ITEM_FORUSERCDKEY), CHAR_getChar( i, CHAR_CDKEY)) ){
				int floor, x, y;
				char token[256];
				floor = CHAR_getInt( i, CHAR_FLOOR);
				x = CHAR_getInt( i, CHAR_X);
				y = CHAR_getInt( i, CHAR_Y);

			//if( floor == 887 || floor == 117 ||
			//	floor == 1042 || floor == 2032 || floor == 3032 || floor == 4032 ||
			//	floor == 5032 || floor == 6032 ||floor == 7032 || floor == 8032 || floor == 9032 ){ 
				if( checkUnlawWarpFloor( floor) ) {
					CHAR_talkToCli( charaindex, -1, "对象所在地方无法传送。", CHAR_COLORYELLOW );
					return;
				}

				CHAR_warpToSpecificPoint( charaindex, floor, x, y );
				sprintf( token, "%s藉由戒指传送到你身边来。", CHAR_getChar( charaindex, CHAR_NAME));
				CHAR_talkToCli( i, -1, token, CHAR_COLORYELLOW );
				sprintf( token, "藉由戒指传送到%s身边。", CHAR_getChar( i, CHAR_NAME));
				CHAR_talkToCli( charaindex, -1, token, CHAR_COLORYELLOW );
				CHAR_DischargePartyNoMsg( charaindex);//解散团队
#ifdef _ITEM_MAXUSERNUM
				itemmaxuse = ITEM_getInt( itemindex, ITEM_DAMAGEBREAK);
#endif
				if( itemmaxuse != -1 )	{
					itemmaxuse--;
#ifdef _ITEM_MAXUSERNUM
					ITEM_setInt( itemindex, ITEM_DAMAGEBREAK, itemmaxuse);
#endif
					if( itemmaxuse < 1 )	{
						sprintf( buf1, "道具 %s消失了。", ITEM_getChar( itemindex, ITEM_NAME) );
						CHAR_talkToCli(charaindex, -1, buf1, CHAR_COLORYELLOW);
						CHAR_DelItem( charaindex, haveitemindex);
						return;
					}else{
						sprintf( buf1, "传送至目标对象所在位置，可使用次数剩馀%d次。", itemmaxuse);
						ITEM_setChar( itemindex, ITEM_EFFECTSTRING, buf1);
					}
				}else{
					itemmaxuse = 10;
					ITEM_setInt( itemindex, ITEM_DAMAGEBREAK, itemmaxuse);
					sprintf( buf1, "传送至目标对象所在位置，可使用次数剩余%d次。", itemmaxuse);
					ITEM_setChar( itemindex, ITEM_EFFECTSTRING, buf1);
				}

				CHAR_DischargePartyNoMsg( charaindex);
				CHAR_complianceParameter( charaindex );
				CHAR_sendItemDataOne( charaindex, haveitemindex);
				return;
			}
		}
		CHAR_talkToCli( charaindex, -1, "对象不在此伺服器或不在线上。", CHAR_COLORYELLOW );
	}
}
#endif

#ifdef _HALLOWEEN_EFFECT
void ITEM_MapEffect(int charaindex,int toindex,int haveitemindex)
{
	int itemindex,floor;
	char *pActionNumber,szMsg[128];

	if(CHAR_CHECKINDEX(charaindex) == FALSE )return ;

	itemindex = CHAR_getItemIndex(charaindex,haveitemindex);
	if(!ITEM_CHECKINDEX(itemindex)) return;
	
	// 找出玩家所在的地图编号
	floor = CHAR_getInt(charaindex,CHAR_FLOOR);
	// 找出道具要放的特效的编号
	pActionNumber = ITEM_getChar(itemindex,ITEM_ARGUMENT);
	sprintf(szMsg,"%d 8 %s",floor,pActionNumber);
	// 执行
	CHAR_CHAT_DEBUG_effect(charaindex,szMsg);
	CHAR_DelItemMess(charaindex,haveitemindex,0);
}
#endif

void ITEM_changePetOwner( int charaindex, int toindex, int haveitemindex)
{
	int itemindex, i;
	if( CHAR_CHECKINDEX( charaindex ) == FALSE )return ;
	if( CHAR_CHECKINDEX( toindex ) == FALSE )return ;

	//ITEM_useRecovery_Field(	charaindex, toindex, haveitemindex );

	itemindex = CHAR_getItemIndex( charaindex, haveitemindex);
	if( !ITEM_CHECKINDEX(itemindex) ) return;
	
	if( CHAR_getInt( toindex, CHAR_WHICHTYPE) != CHAR_TYPEPET ) {
		CHAR_talkToCli( charaindex, -1, "这只能用在宠物身上喔。", CHAR_COLORYELLOW );
		return;
	}

	if( !strcmp( CHAR_getChar( toindex, CHAR_NPCARGUMENT), "") || 
		!strcmp( CHAR_getChar( toindex, CHAR_NPCARGUMENT), CHAR_getChar( charaindex, CHAR_CDKEY))) {
		CHAR_talkToCli( charaindex, -1, "这只宠物本来就是你的，并不需要使用这个呀。", CHAR_COLORYELLOW);
		return;
	}

	CHAR_setChar( toindex, CHAR_NPCARGUMENT, "");
	
	for( i =0; i < CHAR_MAXPETHAVE; i++)
		if( CHAR_getCharPet( charaindex, i) == toindex )
			CHAR_send_K_StatusString( charaindex, i,CHAR_K_STRING_NAME|CHAR_K_STRING_CHANGENAMEFLG);

	CHAR_talkToCli( charaindex, -1, "现在你可以帮这只宠物改名字了。", CHAR_COLORYELLOW );

	CHAR_DelItemMess( charaindex, haveitemindex, 0);
}


#ifdef _DEL_DROP_GOLD
void GOLD_DeleteTimeCheckLoop( void)
{
	int amount;
	int objindex;
	int objmaxnum;
	
	objmaxnum = OBJECT_getNum();
	for( objindex=0; objindex<objmaxnum; objindex++)	{
		if( CHECKOBJECT( objindex ) == FALSE ) continue;
		if( OBJECT_getType( objindex) != OBJTYPE_GOLD) continue;
		
		amount = OBJECT_getIndex( objindex);
		if( amount >= 10000) continue;
		
		if( (int)NowTime.tv_sec > (int)( OBJECT_getTime( objindex) + getItemdeletetime() ) ) {
			
			LogStone(
				-1,
				"NULL",
				"NULL",
				amount,
				0,
				"Del(删除过时金钱)",
				OBJECT_getFloor( objindex ),
				OBJECT_getX( objindex ),
				OBJECT_getY( objindex )
				);
			
			CHAR_ObjectDelete(objindex);
		}
	}
}

void GOLD_DeleteTimeCheckOne( int objindex)
{
	int amount;
	
	if( CHECKOBJECT( objindex ) == FALSE ) return;
	if( OBJECT_getType( objindex) != OBJTYPE_GOLD) return;
	
	amount = OBJECT_getIndex( objindex);
	if( amount >= 10000) return;
	
	if( (int)NowTime.tv_sec > (int)( OBJECT_getTime( objindex) + getItemdeletetime() ) ) {
		
		LogStone(
			-1,
			"NULL",
			"NULL",
			amount,
			0,
			"Del(删除过时金钱)",
			OBJECT_getFloor( objindex ),
			OBJECT_getX( objindex ),
			OBJECT_getY( objindex )
			);
		
		CHAR_ObjectDelete(objindex);
	}

}

#endif

#ifdef _TIME_TICKET
void ITEM_timeticketEx( int charaindex, int toindex, int haveitemindex, int flag);
void ITEM_timeticket( int charaindex, int toindex, int haveitemindex)
{
	ITEM_timeticketEx( charaindex, toindex, haveitemindex, 0);
}

void ITEM_timeticketEx( int charaindex, int toindex, int haveitemindex, int flag)
{
	int itemindex;
	int addtime;
	int nowtime = time(NULL);
	int tickettime;
	int lefttime;
	char msg[1024];
	
	if( !CHAR_CHECKINDEX( charaindex ) )  return;

	if( check_TimeTicketMap( CHAR_getInt( charaindex, CHAR_FLOOR)) == FALSE 
		&& flag == 0 ) {
		CHAR_talkToCli( charaindex, -1, "这个地点不可使用。", CHAR_COLORYELLOW);
		return;
	}
	
	itemindex = CHAR_getItemIndex( charaindex, haveitemindex);
	if( !ITEM_CHECKINDEX(itemindex) )  return;

	tickettime = CHAR_getWorkInt( charaindex, CHAR_WORKTICKETTIME);
	if( tickettime > nowtime+20 ) {
		CHAR_talkToCli( charaindex, -1, "时间必须剩下２０秒以内才可使用。", CHAR_COLORYELLOW);
		return;
	}
	// 第一次使用的话
	if( tickettime == 0 ) {
		tickettime = nowtime;
		CHAR_setWorkInt( charaindex, CHAR_WORKTICKETTIMESTART, nowtime);
	}
	// 战斗中且超过时限时使用
	//if( CHAR_getWorkInt( charaindex, CHAR_WORKBATTLEMODE) != BATTLE_CHARMODE_NONE 
	//		&& tickettime < nowtime ) {
		//tickettime = nowtime;
	//	sprintf(msg, "时间不足%d秒。", nowtime - tickettime );
	//	CHAR_talkToCli( charaindex, -1, msg, CHAR_COLORYELLOW);
	//}

	addtime = atoi( ITEM_getChar( itemindex, ITEM_ARGUMENT));
	tickettime += addtime;
	CHAR_setWorkInt( charaindex, CHAR_WORKTICKETTIME, tickettime );
	lefttime = tickettime - nowtime;
	if( lefttime > 0 )
		sprintf(msg, "时间增加%d秒，还剩下%d分%d秒。", addtime, lefttime/60, lefttime%60 );
	else
		sprintf(msg, "时间增加%d秒，还不足%d分%d秒。", addtime, (-lefttime)/60, (-lefttime)%60 );
	CHAR_talkToCli( charaindex, -1, msg, CHAR_COLORYELLOW);
	CHAR_DelItem( charaindex, haveitemindex);
}
#endif
#ifdef _ITEM_SETLOVER
void ITEM_SetLoverUser( int charaindex, int toindex, int haveitemindex)
{
   int itemindex = CHAR_getItemIndex( charaindex, haveitemindex);
   int playernum = CHAR_getPlayerMaxNum();
   char token[256], szMsg[128];
   int floor = CHAR_getInt(charaindex,CHAR_FLOOR);

   if( !strcmp( CHAR_getChar( charaindex, CHAR_LOVE), "YES"))
        {
            CHAR_talkToCli( charaindex, -1, "重婚是违法的！您已经结婚了哦~", CHAR_COLORYELLOW);
            return;
        }
   if( !CHAR_CHECKINDEX( charaindex ) ) return;
   if( !CHAR_CHECKINDEX( toindex ) ) return;
   if( CHAR_getInt( toindex, CHAR_WHICHTYPE) != CHAR_TYPEPLAYER)
   {
   		 CHAR_talkToCli( charaindex, -1, "只能和玩家结婚哦", CHAR_COLORYELLOW);
       return;
   }
   if( CHAR_getInt(charaindex,CHAR_TRANSMIGRATION)<3 ||CHAR_getInt(toindex,CHAR_TRANSMIGRATION)<3)
   {
   	   CHAR_talkToCli( charaindex, -1, "如果想结婚的话，双方必须3转以上哦~", CHAR_COLORYELLOW);
       return;
   }
   if( charaindex == toindex )
   {
       CHAR_talkToCli( charaindex, -1, "难道您想和自己结婚吗？", CHAR_COLORYELLOW);
       return;
   }
   if ( IsMale(charaindex)==IsMale(toindex))
   {
       CHAR_talkToCli( charaindex, -1, "同性怎么结婚呢？", CHAR_COLORYELLOW);
       return;    
   }
   if( !ITEM_CHECKINDEX(itemindex) ) return;
			int id = ITEM_getInt( itemindex, ITEM_ID);
			itemindex = ITEM_makeItemAndRegist( id );

   if( strcmp( CHAR_getChar( charaindex, CHAR_LOVE), "YES") &&
   			strcmp( CHAR_getChar( toindex, CHAR_LOVE), "YES"))//第二次判断双方是否已经结婚
   {
	    if( itemindex != -1 ){
					 CHAR_setChar( charaindex, CHAR_LOVERID, CHAR_getChar( toindex, CHAR_CDKEY) );
					 CHAR_setChar( charaindex, CHAR_LOVERNAME, CHAR_getChar( toindex, CHAR_NAME) );
	    	 if (strcmp(CHAR_getChar( toindex, CHAR_LOVERID), CHAR_getChar( charaindex, CHAR_CDKEY))==0 &&
         			strcmp(CHAR_getChar( toindex, CHAR_LOVERNAME), CHAR_getChar( charaindex, CHAR_NAME))==0){
         		int i;
            sprintf( token, "已经答应了%s的求婚", CHAR_getChar( toindex, CHAR_NAME));
		        CHAR_talkToCli( charaindex, -1, token, CHAR_COLORYELLOW);
		        sprintf( token, "%s已经响应了你的求婚", CHAR_getChar( charaindex, CHAR_NAME));
		        CHAR_talkToCli( toindex, -1, token, CHAR_COLORYELLOW);
            CHAR_setChar( toindex, CHAR_LOVE,"YES");
		        CHAR_setChar( charaindex, CHAR_LOVE,"YES");
						CHAR_setChar( charaindex, CHAR_LOVERID, CHAR_getChar( toindex, CHAR_CDKEY) );
						CHAR_setChar( charaindex, CHAR_LOVERNAME, CHAR_getChar( toindex, CHAR_NAME) );
						CHAR_setChar( toindex, CHAR_LOVERID, CHAR_getChar( charaindex, CHAR_CDKEY) );
						CHAR_setChar( toindex, CHAR_LOVERNAME, CHAR_getChar( charaindex, CHAR_NAME) );
						sprintf(szMsg,"%d 8 101883",floor);
		        CHAR_CHAT_DEBUG_effect(charaindex,szMsg);
		        CHAR_DelItem( charaindex, haveitemindex);
         		for(i=0;i<playernum;i++)
            {    
		           if(CHAR_CHECKINDEX(i) == FALSE) continue;
		           sprintf( token, "恭喜%s和%s新婚之喜，恭祝他们百年好合.白头到老.", 
														           CHAR_getChar( charaindex, CHAR_NAME),
														           CHAR_getChar( toindex, CHAR_NAME));
														           CHAR_talkToCli( i, -1, token, CHAR_COLORRED);
		        } 
		        return;
		     }else{
					 	int emptyitemindexinchara = CHAR_findEmptyItemBox( toindex );
								
						if( emptyitemindexinchara < 0 ){
								sprintf( token, "%s物品栏已满。求婚失败" , CHAR_getChar( toindex, CHAR_NAME));
								CHAR_talkToCli( charaindex, -1, token, CHAR_COLORYELLOW);
								sprintf( token, "%s向您发起求婚，但您的物品栏已满。" , CHAR_getChar( charaindex, CHAR_NAME));
								CHAR_talkToCli( toindex, -1, token, CHAR_COLORYELLOW);}
						else{
						 		CHAR_setItemIndex( toindex, emptyitemindexinchara, itemindex);
						   	sprintf( token, "%s向%s求婚的戒指!同意求婚请使用!", CHAR_getChar( charaindex, CHAR_NAME) 
																																							,CHAR_getChar( toindex, CHAR_NAME));
						    ITEM_setChar( itemindex, ITEM_EFFECTSTRING, token);
						   	CHAR_sendItemDataOne( toindex, emptyitemindexinchara);
						   	ITEM_endExistItemsOne(itemindex);
					  }
				
						sprintf( token, "%s向您求婚，获得对方的%s", CHAR_getChar( charaindex, CHAR_NAME),ITEM_getChar( itemindex, ITEM_NAME));
						CHAR_talkToCli( toindex, -1, token, CHAR_COLORYELLOW);
						sprintf( token, "已经向%s发起求婚", CHAR_getChar( toindex, CHAR_NAME));
						CHAR_talkToCli( charaindex, -1, token, CHAR_COLORYELLOW);
						CHAR_DelItem( charaindex, haveitemindex);
				}
				CHAR_talkToCli( charaindex, -1, "请耐心等待对方回应！", CHAR_COLORYELLOW );    
			}else{
					sprintf( token, "求婚失败，戒指无法创建!" );
					CHAR_talkToCli( charaindex, -1, token, CHAR_COLORYELLOW);
					return;
			}
   }
}

void ITEM_LoverWarp( int charaindex, int toindex, int haveitemindex)
{
  int itemindex, i;
  int playernum = CHAR_getPlayerMaxNum();
  //检查是否结婚
  if( !strcmp( CHAR_getChar( charaindex, CHAR_LOVE), "YES"))
  {   
    for(i=0;i<playernum;i++)
    {
        if(CHAR_CHECKINDEX(i) == FALSE) continue;
        if(strcmp( CHAR_getChar( i, CHAR_LOVE), "YES") == 0 &&
        		strcmp(CHAR_getChar(charaindex,CHAR_LOVERID),CHAR_getChar(i,CHAR_CDKEY)) == 0 &&
            strcmp(CHAR_getChar(charaindex,CHAR_LOVERNAME),CHAR_getChar(i,CHAR_NAME)) == 0)
        {
          break;
        }
    }//从index中对比LOVERNAME
        if (i>=playernum)
          {
            CHAR_talkToCli( charaindex, -1, "很抱歉，您的爱人不在线哦~", CHAR_COLORYELLOW );   
            return;
          }

  if( !CHAR_CHECKINDEX( charaindex ) ) return;
  if( !CHAR_CHECKINDEX( i ) ) return;
  
  itemindex = CHAR_getItemIndex( charaindex, haveitemindex);
  if( !ITEM_CHECKINDEX(itemindex) ) return;
  
    if( checkUnlawWarpFloor( CHAR_getInt( charaindex, CHAR_FLOOR) ) ) {
        CHAR_talkToCli( charaindex, -1, "很抱歉，您的爱人在神秘遥远地方，无法传送。", CHAR_COLORYELLOW );
        return;
    }//UNWARP地图禁止使用
    if( checkUnlawWarpFloor( CHAR_getInt( i, CHAR_FLOOR) ) ) {
        CHAR_talkToCli( charaindex, -1, "很抱歉，您的爱人在神秘遥远地方，无法传送。", CHAR_COLORYELLOW );
        return;
    }//UNWARP地图禁止使用
        if(strcmp(CHAR_getChar(i,CHAR_LOVERID),CHAR_getChar(charaindex,CHAR_CDKEY)) == 0 &&
            strcmp(CHAR_getChar(i,CHAR_LOVERNAME),CHAR_getChar(charaindex,CHAR_NAME)) == 0)
          {
          CHAR_DischargePartyNoMsg( charaindex);//解散团队
          int floor, x, y;
          char token[256];
          floor = CHAR_getInt( i, CHAR_FLOOR);
          x = CHAR_getInt( i, CHAR_X);
          y = CHAR_getInt( i, CHAR_Y);

          CHAR_warpToSpecificPoint( charaindex, floor, x, y );
          sprintf( token, "%s:亲爱的，我来了！", CHAR_getChar( charaindex, CHAR_NAME));
          CHAR_talkToCli( i, -1, token, CHAR_COLORYELLOW );
          sprintf( token, "已传送到爱人%s身边！", CHAR_getChar( i, CHAR_NAME));
          CHAR_talkToCli( charaindex, -1, token, CHAR_COLORYELLOW );
          print("\n玩家%s 传送到玩家%s身边", CHAR_getChar( charaindex, CHAR_NAME),CHAR_getChar(i, CHAR_NAME));
          return;
        }
    }else
    CHAR_talkToCli( charaindex, -1, "您没有结婚哦~", CHAR_COLORYELLOW );
}

void ITEM_LoverUnmarry( int charaindex, int toindex, int haveitemindex)
{
  int itemindex = CHAR_getItemIndex( charaindex, haveitemindex);
  int  i;
  char token[256];
  int playernum = CHAR_getPlayerMaxNum();
  //检查是否结婚
  if( !strcmp( CHAR_getChar( charaindex, CHAR_LOVE), "YES"))
  {   
    for(i=0;i<playernum;i++)
    {
        if(CHAR_CHECKINDEX(i) == FALSE) continue;
        if(strcmp( CHAR_getChar( i, CHAR_LOVE), "YES") == 0 &&
        		strcmp(CHAR_getChar(charaindex,CHAR_LOVERID),CHAR_getChar(i,CHAR_CDKEY)) == 0 &&
            strcmp(CHAR_getChar(charaindex,CHAR_LOVERNAME),CHAR_getChar(i,CHAR_NAME)) == 0)
        {
          break;
        }
    }//从index中对比LOVERNAME
        if (i>=playernum)
          {
            CHAR_talkToCli( charaindex, -1, "很抱歉，您的爱人不在线哦~", CHAR_COLORYELLOW );   
            return;
          }

  if( !CHAR_CHECKINDEX( charaindex ) ) return;
  if( !CHAR_CHECKINDEX( i ) ) return;
  if( !ITEM_CHECKINDEX(itemindex) ) return;
  	
  	 if(strcmp(ITEM_getChar( itemindex, ITEM_SECRETNAME), "同意离婚戒指")){
				int id = ITEM_getInt( itemindex, ITEM_ID);
				itemindex = ITEM_makeItemAndRegist( id );
	
		    if( itemindex != -1 ){
		    	
		   			int emptyitemindexinchara = CHAR_findEmptyItemBox( toindex );
					
						if( emptyitemindexinchara < 0 ){
								sprintf( token, "%s物品栏已满。离婚失败" , CHAR_getChar( i, CHAR_NAME));
								CHAR_talkToCli( charaindex, -1, token, CHAR_COLORYELLOW);
								sprintf( token, "%s向您发起离婚，但您的物品栏已满。" , CHAR_getChar( charaindex, CHAR_NAME));
								CHAR_talkToCli( i, -1, token, CHAR_COLORYELLOW);}
					 	else{
					 		CHAR_setItemIndex( i, emptyitemindexinchara, itemindex);
				      ITEM_setChar( itemindex, ITEM_NAME, "同意离婚戒指");
				      ITEM_setChar( itemindex, ITEM_SECRETNAME, "同意离婚戒指");
			      	sprintf( token, "%s向你申请离婚，如果同意请使用该戒指!", CHAR_getChar( charaindex, CHAR_NAME));
				      ITEM_setChar( itemindex, ITEM_EFFECTSTRING, token);
			      	CHAR_sendItemDataOne( i, emptyitemindexinchara);
			      	ITEM_endExistItemsOne(itemindex);
		       		sprintf( token, " %s向您提出离婚并把 %s 给你！", CHAR_getChar( charaindex, CHAR_NAME),ITEM_getChar( itemindex, ITEM_NAME));
		       		CHAR_talkToCli( i, -1, token, CHAR_COLORYELLOW);
		       		sprintf( token, "已经向%s发起离婚", CHAR_getChar( i, CHAR_NAME));
		       		CHAR_talkToCli( charaindex, -1, token, CHAR_COLORYELLOW);
			      	CHAR_DelItem( charaindex, haveitemindex);
	          	return;}}
      }else{
     			CHAR_setChar( i, CHAR_LOVE,"");
		      CHAR_setChar( i, CHAR_LOVERID,"");
					CHAR_setChar( i, CHAR_LOVERNAME,"");
		      CHAR_setChar( charaindex, CHAR_LOVE,"");
					CHAR_setChar( charaindex, CHAR_LOVERID,"");
		      CHAR_setChar( charaindex, CHAR_LOVERNAME,"");
	       	CHAR_talkToCli( i, -1, "双方离婚成功！", CHAR_COLORYELLOW);
	      	CHAR_talkToCli( charaindex, -1, "双方离婚成功！", CHAR_COLORYELLOW);}
	      	CHAR_DelItem( charaindex, haveitemindex);
    }else
    CHAR_talkToCli( charaindex, -1, "您没有结婚哦~", CHAR_COLORYELLOW );
}


#endif

#ifdef _ITEM_METAMO
void ITEM_ColorMetamo( int charaindex, int toindex, int haveitemindex)
{
		int MetamoList[13][7]={
		/*{  造型 ,   红  ,   绿  ,   黄  ,   灰  ,   白  ,   黑  },  //此行为说明行*/
			{ 100000, 100000, 100005, 100010, 100015, 100700, 100705},	//小矮子
			{ 100020, 100025, 100030, 100035, 100020, 100710, 100715},	//赛亚人
			{ 100040, 100055, 100050, 100045, 100040, 100720, 100725},	//辫子男孩
			{ 100060, 100060, 100065, 100070, 100075, 100730, 100735},	//酷哥
			{ 100080, 100095, 100085, 100090, 100080, 100740, 100745},	//熊皮男
			{ 100100, 100100, 100115, 100110, 100105, 100750, 100755},	//大个
			{ 100120, 100135, 100120, 100125, 100130, 100760, 100765},	//小矮妹
			{ 100140, 100145, 100140, 100150, 100155, 100770, 100775},	//熊皮妹
			{ 100160, 100165, 100170, 100160, 100175, 100780, 100785},	//帽子妹
			{ 100180, 100190, 100195, 100185, 100180, 100790, 100795},	//短发夹妹
			{ 100200, 100200, 100210, 100210, 100205, 100800, 100805},	//手套女
			{ 100220, 100230, 100225, 100220, 100235, 100810, 100815},	//辣妹
			{ 100240,   0   ,   0   ,   0   ,   0   , 100820,   0   },	//
		};
		int itemindex = CHAR_getItemIndex( charaindex, haveitemindex);
		char *itemarg="\0";
		int OldMetamoId, NewMetamoId, i;
		itemarg=ITEM_getChar(itemindex,ITEM_ARGUMENT);
		if( itemarg == "\0" ) return;
		OldMetamoId=CHAR_getInt( charaindex, CHAR_BASEBASEIMAGENUMBER);
		for(i=0;i<12;i++)
			if((OldMetamoId>=MetamoList[i][0] && OldMetamoId<MetamoList[i+1][0])
				  || (OldMetamoId>=MetamoList[i][5] && OldMetamoId<MetamoList[i+1][5]))
					break;
		if(i==12){
				CHAR_talkToCli( charaindex, -1, "变色失败，你造型并不是人物造型", CHAR_COLORYELLOW);
				return;
		}
		if( strstr( itemarg, "红" ) ){
				CHAR_talkToCli( charaindex, -1, "变成红色", CHAR_COLORYELLOW);
				NewMetamoId=MetamoList[i][1];
		}else if( strstr( itemarg, "绿" ) ){
				CHAR_talkToCli( charaindex, -1, "变成绿色", CHAR_COLORYELLOW);
				NewMetamoId=MetamoList[i][2];
		}else if( strstr( itemarg, "黄" ) ){
				CHAR_talkToCli( charaindex, -1, "变成黄色", CHAR_COLORYELLOW);
				NewMetamoId=MetamoList[i][3];
		}else if( strstr( itemarg, "灰" ) ){
				CHAR_talkToCli( charaindex, -1, "变成灰色", CHAR_COLORYELLOW);
				NewMetamoId=MetamoList[i][4];
		}else if( strstr( itemarg, "白" ) ){
#ifdef _FM_METAMO
				switch( CHAR_getWorkInt( charaindex, CHAR_WORKFMFLOOR) ){
					case 1041:
					case 2031:
					case 3031:
					case 4031:
					case 5031:
					case 6031:
					case 7031:
					case 8031:
					case 9031:
					case 10031:
						if(CHAR_getInt( charaindex, CHAR_FMLEADERFLAG ) != FMMEMBER_NONE
								&& CHAR_getInt( charaindex, CHAR_FMLEADERFLAG ) != FMMEMBER_APPLY
								&&  CHAR_getInt( charaindex, CHAR_FMSPRITE ) == 0)
						{
#endif
							CHAR_talkToCli( charaindex, -1, "变成白色", CHAR_COLORYELLOW);
							NewMetamoId=MetamoList[i][5];
#ifdef _FM_METAMO
							break;
						}
					default:
						CHAR_talkToCli( charaindex, -1, "该戒指只能拥有庄园的光明家族成员使用！", CHAR_COLORYELLOW);
						return;
				}
#endif
		}else if( strstr( itemarg, "黑" ) ){
#ifdef _FM_METAMO
				switch( CHAR_getWorkInt( charaindex, CHAR_WORKFMFLOOR) ){
					case 1041:
					case 2031:
					case 3031:
					case 4031:
					case 5031:
					case 6031:
					case 7031:
					case 8031:
					case 9031:
					case 10031:
						if(CHAR_getInt( charaindex, CHAR_FMLEADERFLAG ) != FMMEMBER_NONE
								&& CHAR_getInt( charaindex, CHAR_FMLEADERFLAG ) != FMMEMBER_APPLY
								&&  CHAR_getInt( charaindex, CHAR_FMSPRITE ) == 1)
						{
#endif
							CHAR_talkToCli( charaindex, -1, "变成黑色", CHAR_COLORYELLOW);
							NewMetamoId=MetamoList[i][6];
#ifdef _FM_METAMO
							break;
						}
					default:
						CHAR_talkToCli( charaindex, -1, "该戒指只能拥有庄园的黑暗家族成员使用！", CHAR_COLORYELLOW);
						return;
				}
#endif
		}else if( strstr( itemarg, "随" ) ){
				srand(time(0));
				do
						NewMetamoId=MetamoList[i][rand()%4+1];
				while(NewMetamoId==OldMetamoId);
				CHAR_talkToCli( charaindex, -1, "随机变色成功", CHAR_COLORYELLOW);
		}else{
				CHAR_talkToCli( charaindex, -1, "变色失败", CHAR_COLORYELLOW);
			return;
		}
		if(NewMetamoId==0)return;
		CHAR_setInt( charaindex , CHAR_BASEIMAGENUMBER , NewMetamoId );
		CHAR_setInt( charaindex , CHAR_BASEBASEIMAGENUMBER , NewMetamoId );
	
		CHAR_complianceParameter( charaindex );
		CHAR_sendCToArroundCharacter( CHAR_getWorkInt( charaindex , CHAR_WORKOBJINDEX ));
		CHAR_send_P_StatusString( charaindex , CHAR_P_STRING_BASEBASEIMAGENUMBER);
		CHAR_DelItem( charaindex, haveitemindex);
}

void ITEM_CharaMetamo( int charaindex, int toindex, int haveitemindex)
{
		int MetamoList[12][7]={
		/*{  造型 ,   红  ,   绿  ,   黄  ,   灰  ,   白  ,   黑  },  //此行为说明行*/
			{ 100000, 100000, 100005, 100010, 100015, 100700, 100705},	//小矮子
			{ 100020, 100025, 100030, 100035, 100020, 100710, 100715},	//赛亚人
			{ 100040, 100055, 100050, 100045, 100040, 100720, 100725},	//辫子男孩
			{ 100060, 100060, 100065, 100070, 100075, 100730, 100735},	//酷哥
			{ 100080, 100095, 100085, 100090, 100080, 100740, 100745},	//熊皮男
			{ 100100, 100100, 100115, 100110, 100105, 100750, 100755},	//大个
			{ 100120, 100135, 100120, 100125, 100130, 100760, 100765},	//小矮妹
			{ 100140, 100145, 100140, 100150, 100155, 100770, 100775},	//熊皮妹
			{ 100160, 100165, 100170, 100160, 100175, 100780, 100785},	//帽子妹
			{ 100180, 100190, 100195, 100185, 100180, 100790, 100795},	//短发夹妹
			{ 100200, 100200, 100210, 100215, 100205, 100800, 100805},	//手套女
			{ 100220, 100230, 100225, 100220, 100235, 100810, 100815},	//辣妹
		};
		int OldMetamoId=0, NewMetamoId=0;
		int itemindex = CHAR_getItemIndex( charaindex, haveitemindex);
		char *itemarg="\0";
		srand(time(0));
		itemarg=ITEM_getChar(itemindex,ITEM_ARGUMENT);
		if( itemarg == "\0" ) return;
		OldMetamoId=CHAR_getInt( charaindex, CHAR_BASEBASEIMAGENUMBER);
		if((OldMetamoId >= 100000 && OldMetamoId < 100240 )
				|| (OldMetamoId >= 100700 && OldMetamoId < 100820)){
			if( strstr( itemarg, "豆丁囝" ) && OldMetamoId >= 100000 && OldMetamoId < 100120){
					CHAR_talkToCli( charaindex, -1, "更改造型为豆囝", CHAR_COLORYELLOW);
					NewMetamoId=MetamoList[0][rand()%4+1];
			}else if( strstr( itemarg, "赛亚人" ) && OldMetamoId >= 100000 && OldMetamoId < 100120){
					CHAR_talkToCli( charaindex, -1, "更改造型为赛亚人", CHAR_COLORYELLOW);
					NewMetamoId=MetamoList[1][rand()%4+1];
			}else if( strstr( itemarg, "辫子男孩" ) && OldMetamoId >= 100000 && OldMetamoId < 100120){
					CHAR_talkToCli( charaindex, -1, "更改造型为辫子男孩", CHAR_COLORYELLOW);
					NewMetamoId=MetamoList[2][rand()%4+1];
			}else if( strstr( itemarg, "酷哥" ) && OldMetamoId >= 100000 && OldMetamoId < 100120){
					CHAR_talkToCli( charaindex, -1, "更改造型为酷哥", CHAR_COLORYELLOW);
					NewMetamoId=MetamoList[3][rand()%4+1];
			}else if( strstr( itemarg, "熊皮男" ) && OldMetamoId >= 100000 && OldMetamoId < 100120){
					CHAR_talkToCli( charaindex, -1, "更改造型为熊皮男", CHAR_COLORYELLOW);
					NewMetamoId=MetamoList[4][rand()%4+1];
			}else if( strstr( itemarg, "大只佬" ) && OldMetamoId >= 100000 && OldMetamoId < 100120){
					CHAR_talkToCli( charaindex, -1, "更改造型为大只佬", CHAR_COLORYELLOW);
					NewMetamoId=MetamoList[5][rand()%4+1];
			}else if( strstr( itemarg, "豆丁囡" ) && OldMetamoId >= 100120 && OldMetamoId < 100240){
					CHAR_talkToCli( charaindex, -1, "更改造型为豆丁囡", CHAR_COLORYELLOW);
					NewMetamoId=MetamoList[6][rand()%4+1];
			}else if( strstr( itemarg, "熊皮妹" ) && OldMetamoId >= 100120 && OldMetamoId < 100240){
					CHAR_talkToCli( charaindex, -1, "更改造型为熊皮妹", CHAR_COLORYELLOW);
  				NewMetamoId=MetamoList[7][rand()%4+1];
			}else if( strstr( itemarg, "帽子妹" ) && OldMetamoId >= 100120 && OldMetamoId < 100240){
					CHAR_talkToCli( charaindex, -1, "更改造型为帽子妹", CHAR_COLORYELLOW);
					NewMetamoId=MetamoList[8][rand()%4+1];
			}else if( strstr( itemarg, "短发夹妹" ) && OldMetamoId >= 100120 && OldMetamoId < 100240){
					CHAR_talkToCli( charaindex, -1, "更改造型为短发夹妹", CHAR_COLORYELLOW);
					NewMetamoId=MetamoList[9][rand()%4+1];
			}else if( strstr( itemarg, "手套女" ) && OldMetamoId >= 100120 && OldMetamoId < 100240){
					CHAR_talkToCli( charaindex, -1, "更改造型为手套女", CHAR_COLORYELLOW);
					NewMetamoId=MetamoList[10][rand()%4+1];
			}else if( strstr( itemarg, "辣妹" ) && OldMetamoId >= 100120 && OldMetamoId < 100240){
					CHAR_talkToCli( charaindex, -1, "更改造型为辣妹", CHAR_COLORYELLOW);
					NewMetamoId=MetamoList[11][rand()%4+1];
			}else if( strstr( itemarg, "豆丁囝" ) && OldMetamoId >= 100700 && OldMetamoId < 100760){
					CHAR_talkToCli( charaindex, -1, "更改造型为豆囝", CHAR_COLORYELLOW);
					if( ( OldMetamoId - 100700 ) % 10 == 0 )
						  NewMetamoId=MetamoList[0][5];
					else if( ( OldMetamoId - 100700 ) % 10 == 5 )
						  NewMetamoId=MetamoList[0][6];
			}else if( strstr( itemarg, "赛亚人" ) && OldMetamoId >= 100700 && OldMetamoId < 100760){
					CHAR_talkToCli( charaindex, -1, "更改造型为赛亚人", CHAR_COLORYELLOW);
					if( ( OldMetamoId - 100700 ) % 10 == 0 )
						  NewMetamoId=MetamoList[1][5];
					else if( ( OldMetamoId - 100700 ) % 10 == 5 )
						  NewMetamoId=MetamoList[1][6];
			}else if( strstr( itemarg, "辫子男孩" ) && OldMetamoId >= 100700 && OldMetamoId < 100760){
					CHAR_talkToCli( charaindex, -1, "更改造型为辫子男孩", CHAR_COLORYELLOW);
					if( ( OldMetamoId - 100700 ) % 10 == 0 )
						  NewMetamoId=MetamoList[2][5];
					else if( ( OldMetamoId - 100700 ) % 10 == 5 )
						  NewMetamoId=MetamoList[2][6];
			}else if( strstr( itemarg, "酷哥" ) && OldMetamoId >= 100700 && OldMetamoId < 100760){
					CHAR_talkToCli( charaindex, -1, "更改造型为酷哥", CHAR_COLORYELLOW);
					if( ( OldMetamoId - 100700 ) % 10 == 0 )
						  NewMetamoId=MetamoList[3][5];
					else if( ( OldMetamoId - 100700 ) % 10 == 5 )
						  NewMetamoId=MetamoList[3][6];
			}else if( strstr( itemarg, "熊皮男" ) && OldMetamoId >= 100700 && OldMetamoId < 100760){
					CHAR_talkToCli( charaindex, -1, "更改造型为熊皮男", CHAR_COLORYELLOW);
					if( ( OldMetamoId - 100700 ) % 10 == 0 )
						  NewMetamoId=MetamoList[4][5];
					else if( ( OldMetamoId - 100700 ) % 10 == 5 )
						  NewMetamoId=MetamoList[4][6];
			}else if( strstr( itemarg, "大只佬" ) && OldMetamoId >= 100700 && OldMetamoId < 100760){
					CHAR_talkToCli( charaindex, -1, "更改造型为大只佬", CHAR_COLORYELLOW);
					if( ( OldMetamoId - 100700 ) % 10 == 0 )
						  NewMetamoId=MetamoList[5][5];
					else if( ( OldMetamoId - 100700 ) % 10 == 5 )
						  NewMetamoId=MetamoList[5][6];	
			}else if( strstr( itemarg, "豆丁囡" ) && OldMetamoId >= 100760 && OldMetamoId < 100820){
					CHAR_talkToCli( charaindex, -1, "更改造型为豆丁囡", CHAR_COLORYELLOW);
					if( ( OldMetamoId - 100700 ) % 10 == 0 )
						  NewMetamoId=MetamoList[6][6];
					else if( ( OldMetamoId - 100700 ) % 10 == 5 )
						  NewMetamoId=MetamoList[6][6];
			}else if( strstr( itemarg, "熊皮妹" ) && OldMetamoId >= 100760 && OldMetamoId < 100820){
					CHAR_talkToCli( charaindex, -1, "更改造型为熊皮妹", CHAR_COLORYELLOW);
								if( ( OldMetamoId - 100700 ) % 10 == 0 )
						  NewMetamoId=MetamoList[7][5];
					else if( ( OldMetamoId - 100700 ) % 10 == 5 )
						  NewMetamoId=MetamoList[7][6];	
			}else if( strstr( itemarg, "帽子妹" ) && OldMetamoId >= 100760 && OldMetamoId < 100820){
					CHAR_talkToCli( charaindex, -1, "更改造型为帽子妹", CHAR_COLORYELLOW);
								if( ( OldMetamoId - 100700 ) % 10 == 0 )
						  NewMetamoId=MetamoList[8][5];
					else if( ( OldMetamoId - 100700 ) % 10 == 5 )
						  NewMetamoId=MetamoList[8][6];	
			}else if( strstr( itemarg, "短发夹妹" ) && OldMetamoId >= 100760 && OldMetamoId < 100820){
					CHAR_talkToCli( charaindex, -1, "更改造型为短发夹妹", CHAR_COLORYELLOW);
								if( ( OldMetamoId - 100700 ) % 10 == 0 )
						  NewMetamoId=MetamoList[9][5];
					else if( ( OldMetamoId - 100700 ) % 10 == 5 )
						  NewMetamoId=MetamoList[9][6];	
		  }else if( strstr( itemarg, "手套女" ) && OldMetamoId >= 100760 && OldMetamoId < 100820){
					CHAR_talkToCli( charaindex, -1, "更改造型为手套女", CHAR_COLORYELLOW);
								if( ( OldMetamoId - 100700 ) % 10 == 0 )
						  NewMetamoId=MetamoList[10][5];
					else if( ( OldMetamoId - 100700 ) % 10 == 5 )
						  NewMetamoId=MetamoList[10][6];	
		  }else if( strstr( itemarg, "辣妹" ) && OldMetamoId >= 100760 && OldMetamoId < 100820){
					CHAR_talkToCli( charaindex, -1, "更改造型为辣妹", CHAR_COLORYELLOW);
								if( ( OldMetamoId - 100700 ) % 10 == 0 )
						  NewMetamoId=MetamoList[11][5];
					else if( ( OldMetamoId - 100700 ) % 10 == 5 )
						  NewMetamoId=MetamoList[11][6];	
			}else if( strstr( itemarg, "随机" ) && OldMetamoId >= 100120 && OldMetamoId < 100240){
					do
							if( OldMetamoId >= 100000 && OldMetamoId < 100120 ){
									NewMetamoId=MetamoList[rand()%6][rand()%4+1];
							}else if( OldMetamoId >= 100120 && OldMetamoId < 100240 ){
								  NewMetamoId=MetamoList[rand()%6+6][rand()%4+1];
							}else if( OldMetamoId >= 100700 && OldMetamoId < 100760 ){
									if( ( OldMetamoId - 100700 ) % 10 == 0 )
										  NewMetamoId=MetamoList[rand()%6][5];
									else if( ( OldMetamoId - 100700 ) % 10 == 5 )
										  NewMetamoId=MetamoList[rand()%6][6];
							}else if( OldMetamoId >= 100760 && OldMetamoId < 100820 ){
									if( ( OldMetamoId - 100700 ) % 10 == 0 )
										  NewMetamoId=MetamoList[rand()%6+6][5];
									else if( ( OldMetamoId - 100700 ) % 10 == 5 )
										  NewMetamoId=MetamoList[rand()%6+6][6];
							}
					while(NewMetamoId==OldMetamoId);
					CHAR_talkToCli( charaindex, -1, "随机更改造型成功", CHAR_COLORYELLOW);
			}
		}else{
			CHAR_talkToCli( charaindex, -1, "你并不是人物造型!", CHAR_COLORYELLOW);
			return;
		}
		if(NewMetamoId==0){
			CHAR_talkToCli( charaindex, -1, "你的性别不同，无没更改此造型!", CHAR_COLORYELLOW);
			return;}
		CHAR_setInt( charaindex , CHAR_BASEIMAGENUMBER , NewMetamoId );
		CHAR_setInt( charaindex , CHAR_BASEBASEIMAGENUMBER , NewMetamoId );
	
		CHAR_complianceParameter( charaindex );
		CHAR_sendCToArroundCharacter( CHAR_getWorkInt( charaindex , CHAR_WORKOBJINDEX ));
		CHAR_send_P_StatusString( charaindex , CHAR_P_STRING_BASEBASEIMAGENUMBER);
		CHAR_DelItem( charaindex, haveitemindex);
}

void ITEM_SexMetamo( int charaindex, int toindex, int haveitemindex)
{
		int OldMetamoId, NewMetamoId;
		OldMetamoId=CHAR_getInt( charaindex, CHAR_BASEBASEIMAGENUMBER);
		if(OldMetamoId >=100000 && OldMetamoId <100020){                 //豆丁囝
				NewMetamoId=OldMetamoId+120;
				CHAR_talkToCli( charaindex, -1, "恭喜你变性成豆丁囡!", CHAR_COLORYELLOW);
		}else if(OldMetamoId>=100020 && OldMetamoId <100040){           //赛亚人
				NewMetamoId=OldMetamoId+160;
				CHAR_talkToCli( charaindex, -1, "恭喜你变性成短发夹妹!", CHAR_COLORYELLOW);
		}else if(OldMetamoId>=100040 && OldMetamoId <100060){           //辫子男孩
				NewMetamoId=OldMetamoId+120;
				CHAR_talkToCli( charaindex, -1, "恭喜你变性成帽子妹!", CHAR_COLORYELLOW);
		}else if(OldMetamoId>=100060 && OldMetamoId <100080){           //酷哥
				NewMetamoId=OldMetamoId+160;
				CHAR_talkToCli( charaindex, -1, "恭喜你变性成辣妹!", CHAR_COLORYELLOW);
		}else if(OldMetamoId>=100080 && OldMetamoId <100100){           //熊皮男
				NewMetamoId=OldMetamoId+60;
				CHAR_talkToCli( charaindex, -1, "恭喜你变性成熊皮妹!", CHAR_COLORYELLOW);
		}else if(OldMetamoId>=100100 && OldMetamoId <100120){           //大只佬
				NewMetamoId=OldMetamoId+100;
				CHAR_talkToCli( charaindex, -1, "恭喜你变性成手套女!", CHAR_COLORYELLOW);
		}else if(OldMetamoId>=100120 && OldMetamoId <100140){           //豆丁囡
				NewMetamoId=OldMetamoId-120;
				CHAR_talkToCli( charaindex, -1, "恭喜你变性成豆丁囝!", CHAR_COLORYELLOW);
		}else if(OldMetamoId>=100140 && OldMetamoId <100160){           //熊皮妹
				NewMetamoId=OldMetamoId-60;
				CHAR_talkToCli( charaindex, -1, "恭喜你变性成熊皮男!", CHAR_COLORYELLOW);
		}else if(OldMetamoId>=100160 && OldMetamoId <100180){           //帽子妹
				NewMetamoId=OldMetamoId-120;
				CHAR_talkToCli( charaindex, -1, "恭喜你变性成辫子男孩!", CHAR_COLORYELLOW);
		}else if(OldMetamoId>=100180 && OldMetamoId <100200){           //短发夹妹
				NewMetamoId=OldMetamoId-160;
				CHAR_talkToCli( charaindex, -1, "恭喜你变性成赛亚人!", CHAR_COLORYELLOW);
		}else if(OldMetamoId>=100200 && OldMetamoId <100220){           //手套女
				NewMetamoId=OldMetamoId-100;
				CHAR_talkToCli( charaindex, -1, "恭喜你变性成大只佬!", CHAR_COLORYELLOW);
		}else if(OldMetamoId>=100220 && OldMetamoId <100240){           //辣妹
				NewMetamoId=OldMetamoId-160;
				CHAR_talkToCli( charaindex, -1, "恭喜你变性成酷哥!", CHAR_COLORYELLOW);
		}else if(OldMetamoId >=100700 && OldMetamoId <100710){          //豆丁囝
				NewMetamoId=OldMetamoId+60;
				CHAR_talkToCli( charaindex, -1, "恭喜你变性成豆丁囡!", CHAR_COLORYELLOW);
		}else if(OldMetamoId>=100710 && OldMetamoId <100720){           //赛亚人
				NewMetamoId=OldMetamoId+80;
				CHAR_talkToCli( charaindex, -1, "恭喜你变性成短发夹妹!", CHAR_COLORYELLOW);
		}else if(OldMetamoId>=100720 && OldMetamoId <100730){           //辫子男孩
				NewMetamoId=OldMetamoId+60;
				CHAR_talkToCli( charaindex, -1, "恭喜你变性成帽子妹!", CHAR_COLORYELLOW);
		}else if(OldMetamoId>=100730 && OldMetamoId <100740){           //酷哥
				NewMetamoId=OldMetamoId+80;
				CHAR_talkToCli( charaindex, -1, "恭喜你变性成辣妹!", CHAR_COLORYELLOW);
		}else if(OldMetamoId>=100740 && OldMetamoId <100750){           //熊皮男
				NewMetamoId=OldMetamoId+30;
				CHAR_talkToCli( charaindex, -1, "恭喜你变性成熊皮妹!", CHAR_COLORYELLOW);
		}else if(OldMetamoId>=100750 && OldMetamoId <100760){           //大只佬
				NewMetamoId=OldMetamoId+50;
				CHAR_talkToCli( charaindex, -1, "恭喜你变性成手套女!", CHAR_COLORYELLOW);
		}else if(OldMetamoId>=100760 && OldMetamoId <100770){           //豆丁囡
				NewMetamoId=OldMetamoId-60;
				CHAR_talkToCli( charaindex, -1, "恭喜你变性成豆丁囝!", CHAR_COLORYELLOW);
		}else if(OldMetamoId>=100770 && OldMetamoId <100780){           //熊皮妹
				NewMetamoId=OldMetamoId-30;
				CHAR_talkToCli( charaindex, -1, "恭喜你变性成熊皮男!", CHAR_COLORYELLOW);
		}else if(OldMetamoId>=100780 && OldMetamoId <100790){           //帽子妹
				NewMetamoId=OldMetamoId-60;
				CHAR_talkToCli( charaindex, -1, "恭喜你变性成辫子男孩!", CHAR_COLORYELLOW);
		}else if(OldMetamoId>=100790 && OldMetamoId <100800){           //短发夹妹
				NewMetamoId=OldMetamoId-80;
				CHAR_talkToCli( charaindex, -1, "恭喜你变性成赛亚人!", CHAR_COLORYELLOW);
		}else if(OldMetamoId>=100810 && OldMetamoId <100820){           //手套女
				NewMetamoId=OldMetamoId-50;
				CHAR_talkToCli( charaindex, -1, "恭喜你变性成大只佬!", CHAR_COLORYELLOW);
		}else if(OldMetamoId>=100820 && OldMetamoId <100830){           //辣妹
				NewMetamoId=OldMetamoId-80;
				CHAR_talkToCli( charaindex, -1, "恭喜你变性成酷哥!", CHAR_COLORYELLOW);
		}else{
				CHAR_talkToCli( charaindex, -1, "你并不是人物造型，无法变性!", CHAR_COLORYELLOW);
				return;}
		if(NewMetamoId==0)return;
		CHAR_setInt( charaindex , CHAR_BASEIMAGENUMBER , NewMetamoId );
		CHAR_setInt( charaindex , CHAR_BASEBASEIMAGENUMBER , NewMetamoId );
		
		CHAR_complianceParameter( charaindex );
		CHAR_sendCToArroundCharacter( CHAR_getWorkInt( charaindex , CHAR_WORKOBJINDEX ));
		CHAR_send_P_StatusString( charaindex , CHAR_P_STRING_BASEBASEIMAGENUMBER);
		CHAR_DelItem( charaindex, haveitemindex);
}
#endif

#ifdef _GM_ITEM
void ITEM_GMFUNCTION( int charaindex, int toindex, int haveitemindex)
{
		int itemindex = CHAR_getItemIndex( charaindex, haveitemindex);
		char *itemarg="\0";
		char gmtime[16];
		char gmfunction[16];
		char token[64];
		itemarg=ITEM_getChar(itemindex,ITEM_ARGUMENT);
		if( itemarg == "\0" ) return;

		getStringFromIndexWithDelim(itemarg,"|", 1, gmfunction, sizeof(gmfunction));
		getStringFromIndexWithDelim(itemarg,"|", 2, gmtime, sizeof(gmtime));
		CHAR_setChar( charaindex , CHAR_GMFUNCTION, gmfunction );
		CHAR_setInt( charaindex , CHAR_GMTIME, atoi(gmtime) );
		sprintf( token, "获得使用%s权限%d!", gmfunction, atoi(gmtime));
		CHAR_talkToCli( charaindex, -1, token, CHAR_COLORYELLOW);
		sprintf( token, "相关使用请查看[help %s]无需GM密码!",CHAR_getChar( charaindex, CHAR_GMFUNCTION));
		CHAR_talkToCli( charaindex, -1, token, CHAR_COLORYELLOW );		
		CHAR_DelItem( charaindex, haveitemindex);
}
#endif

#ifdef _VIP_SERVER
void ITEM_AddMemberPoint( int charaindex, int toindex, int haveitemindex)
{
		int itemindex = CHAR_getItemIndex( charaindex, haveitemindex);
		char *itemarg="\0";
		char token[64];
		char fd;
		int point=CHAR_getInt( charaindex , CHAR_AMPOINT);
		itemarg=ITEM_getChar(itemindex,ITEM_ARGUMENT);
		if( itemarg == "\0" ) return;
		CHAR_setInt( charaindex , CHAR_AMPOINT, point+atoi(itemarg));
		sprintf( token, "获得会员点数%d,目前你拥有会员点数为%d!", atoi(itemarg),point+atoi(itemarg));
		CHAR_talkToCli( charaindex, -1, token, CHAR_COLORYELLOW );		
		CHAR_DelItem( charaindex, haveitemindex);
		CHAR_charSaveFromConnect(fd, FALSE);
		CHAR_talkToCli( charaindex, -1, "您的数据以保存", CHAR_COLORRED );
}
#endif

#ifdef _VIP_RIDE
void ITEM_VipRide( int charaindex, int toindex, int haveitemindex)
{
		if(CHAR_getInt( charaindex , CHAR_VIPRIDE)!=1 
					|| CHAR_getInt( charaindex , CHAR_LOWRIDEPETS)&RIDE_PET14!=RIDE_PET14){
			int LRCode = CHAR_getInt( charaindex, CHAR_LOWRIDEPETS);
			LRCode = LRCode|RIDE_PET14;
			CHAR_setInt( charaindex, CHAR_LOWRIDEPETS, LRCode);
			CHAR_sendStatusString( charaindex, "x");
			CHAR_setInt( charaindex , CHAR_VIPRIDE, 1);
			CHAR_talkToCli( charaindex, -1, "恭喜你，你已成为正式会员了！", CHAR_COLORYELLOW );		
			CHAR_DelItem( charaindex, haveitemindex);
		}else
			CHAR_talkToCli( charaindex, -1, "你已是会员了，没必要再使用！", CHAR_COLORRED );
}
#endif

#ifdef _FM_ITEM
void ITEM_AddFame( int charaindex, int toindex, int haveitemindex)
{
		int itemindex = CHAR_getItemIndex( charaindex, haveitemindex);
		char *itemarg="\0";
		char token[64];
		int fame=CHAR_getInt( charaindex , CHAR_FAME);
		itemarg=ITEM_getChar(itemindex,ITEM_ARGUMENT);
		if( itemarg == "\0" ) return;
		CHAR_setInt( charaindex , CHAR_FAME, fame+atoi(itemarg)*100);
		sprintf( token, "声望点数提升%d,目前你的声望是%d!", atoi(itemarg),fame/100+atoi(itemarg));
		CHAR_talkToCli( charaindex, -1, token, CHAR_COLORYELLOW );		
		CHAR_DelItem( charaindex, haveitemindex);
}
#endif

#ifdef _LUCK_ITEM
void ITEM_Luck( int charaindex, int toindex, int haveitemindex)
{
		int itemindex = CHAR_getItemIndex( charaindex, haveitemindex);
		int i;
		char *itemarg="\0";
		char token[64];
		char luck[][5]={"凶","一般","小吉","中吉","大吉"};
		itemarg=ITEM_getChar(itemindex,ITEM_ARGUMENT);
		for(i=0;i<5;i++)
			if(strstr( itemarg, luck[i]))
				break;
		if(i==6)i=0;
		CHAR_setInt( charaindex , CHAR_LUCK, i+1);
		sprintf( token, "你现在的运气为%s", luck[i]);
		CHAR_talkToCli( charaindex, -1, token, CHAR_COLORYELLOW );		
		CHAR_DelItem( charaindex, haveitemindex);
}
#endif

#ifdef _ITEM_METAMO_TIME
void ITEM_MetamoTime( int charaindex, int toindex, int haveitemindex )
{
	char *itemarg="\0", msg[128], buff[32];
	int itemindex, metamoTime, battlemode, metamoNo;

	if( CHAR_CHECKINDEX( charaindex ) == FALSE )	return;
	//print(" PetMetamo_toindex:%d ", toindex);

	if( CHAR_getInt( charaindex, CHAR_RIDEPET ) != -1 ){
		CHAR_talkToCli( charaindex, -1, "无法变身，骑乘中不能变身！", CHAR_COLORYELLOW );
		return;
	}
#ifdef _FIX_METAMORIDE
	if( CHAR_CHECKJOINENEMY( charaindex) == TRUE ){
		CHAR_talkToCli( charaindex, -1, "无法变身，搭乘中不能变身！", CHAR_COLORYELLOW );
		return;
	}
#else
	if( CHAR_getInt( charaindex, CHAR_BASEIMAGENUMBER) == 100259 ){
		CHAR_talkToCli( charaindex, -1, "无法变身，搭乘中不能变身！", CHAR_COLORYELLOW );
		return;
	}
#endif
#ifdef _PETSKILL_BECOMEPIG
    if( CHAR_getInt( charaindex, CHAR_BECOMEPIG) > -1 ){//变成乌力了
		CHAR_talkToCli( charaindex, -1, "无法变身，乌力化中不能变身！", CHAR_COLORYELLOW );
	    return;
	}
#endif


#ifdef _FIXBUG_ATTACKBOW
	{
		int armindex = CHAR_getItemIndex( charaindex, CHAR_ARM);
		if( ITEM_CHECKINDEX( armindex) == TRUE )	{
			int armtype = BATTLE_GetWepon( charaindex);
			if( armtype == ITEM_BOW || armtype == ITEM_BOUNDTHROW ||
				armtype == ITEM_BREAKTHROW || armtype == ITEM_BOOMERANG)	{
				CHAR_talkToCli( charaindex, -1, "使用远距离武器无法变身！", CHAR_COLORYELLOW );
				return;
			}
		}
	}
#endif

	battlemode = CHAR_getWorkInt( charaindex, CHAR_WORKBATTLEMODE );
	// 爵  钒铵凛反  骰允月
	if( battlemode == BATTLE_CHARMODE_INIT ){
		return;
	}
	itemindex = CHAR_getItemIndex( charaindex, haveitemindex);
	if(!ITEM_CHECKINDEX(itemindex)) return;
	itemarg = ITEM_getChar(itemindex, ITEM_ARGUMENT );
	if( itemarg == "\0" )return;
	if( getStringFromIndexWithDelim(itemarg,"|", 1, buff, sizeof(buff)) )
		metamoNo=atoi(buff);
	if( getStringFromIndexWithDelim(itemarg,"|", 2, buff, sizeof(buff)) )
		metamoTime=atoi(buff);
	getStringFromIndexWithDelim(itemarg,"|", 3, buff, sizeof(buff));

	CHAR_setWorkInt( charaindex, CHAR_WORKITEMMETAMO, NowTime.tv_sec + metamoTime);
	if(metamoTime>60)
		sprintf( msg, "变身成%s，可持续%d小时！", buff, metamoTime / 60 );
	else
		sprintf( msg, "变身成%s，可持续%d分钟！", buff, metamoTime );

	CHAR_talkToCli( charaindex, -1, msg, CHAR_COLORYELLOW );

	CHAR_setInt( charaindex, CHAR_BASEIMAGENUMBER, metamoNo );
	CHAR_complianceParameter( charaindex);
	CHAR_sendCToArroundCharacter( CHAR_getWorkInt( charaindex , CHAR_WORKOBJINDEX ));
	CHAR_send_P_StatusString( charaindex , CHAR_P_STRING_BASEBASEIMAGENUMBER);

	CHAR_DelItem( charaindex, haveitemindex);

}
#endif

#ifdef _ITEM_GOLD
void ITEM_Gold( int charaindex, int toindex, int haveitemindex)
{
		int itemindex = CHAR_getItemIndex( charaindex, haveitemindex);
		char itemarg[10];
		char token[64];
		int gold=CHAR_getInt( charaindex , CHAR_GOLD);
		strcpy(itemarg,ITEM_getChar(itemindex,ITEM_ARGUMENT));
		gold+=atoi(itemarg);
		if(gold>CHAR_MAXGOLDHAVE)
			gold=CHAR_MAXGOLDHAVE;
		CHAR_setInt( charaindex , CHAR_GOLD, gold);
		CHAR_complianceParameter( charaindex);	
	  CHAR_send_P_StatusString( charaindex, CHAR_P_STRING_GOLD);
		sprintf( token, "你的石币增加%dS", atoi(itemarg));
		CHAR_talkToCli( charaindex, -1, token, CHAR_COLORYELLOW );		
		CHAR_DelItem( charaindex, haveitemindex);
}
#endif


#ifdef _MYSTERIOUS_GIFT
void ITEM_MysteriousGift( int charaindex, int toindex, int haveitemindex)
{
	int itemindex;
	char *itemarg=NULL;
	char petnumstr[32];
	int petnum=0;
	int present[20]={0};
    int si=0, ret,i;
	char token[256];

	itemindex = CHAR_getItemIndex( charaindex, haveitemindex);
	if(!ITEM_CHECKINDEX(itemindex)) return;

	itemarg = ITEM_getChar( itemindex, ITEM_ARGUMENT);
	if( itemarg == "" ){
		CHAR_talkToCli(charaindex, -1, "这是个神秘礼物!", CHAR_COLORYELLOW);
		return;
	}

	if( getStringFromIndexWithDelim( itemarg, "|", 1, petnumstr, sizeof( petnumstr)) == FALSE )
		return;
    petnum = atoi(petnumstr);
	if( petnum > 20 )
		petnum = 20;
	for(i=0;i<petnum;i++){
		if( getStringFromIndexWithDelim( itemarg, "|", 2+i, petnumstr, sizeof( petnumstr)) )
		    present[i] = atoi(petnumstr);
	}
	
	srand(time(0));
	si = rand()%petnum;

	for( i = 0 ;i < CHAR_MAXPETHAVE ; i++) {
		if( CHAR_getCharPet( charaindex, i) == -1  )
			break;
	}

  if( i == CHAR_MAXPETHAVE ){
		snprintf( token,sizeof( token), "宠物已满！！");
		CHAR_talkToCli( charaindex, -1, token,  CHAR_COLORYELLOW);
		return;
	}	
	
	int enemynum = ENEMY_getEnemyNum();
	for( i = 0; i < enemynum; i ++ ) {
		if( ENEMY_getInt( i, ENEMY_ID) == present[si]) {
			break;
		}
	}
	
	if( i == enemynum )return;
	
	ret = ENEMY_createPetFromEnemyIndex( charaindex, i);
	if(!CHAR_CHECKINDEX( ret))
		return;
	
	sprintf( token,"拿到%s",ENEMY_getChar( i, ENEMY_NAME));
	CHAR_talkToCli( charaindex, -1, token, CHAR_COLORYELLOW );
	
	for( i = 0; i < CHAR_MAXPETHAVE; i ++ ){
		if( CHAR_getCharPet( charaindex, i ) == ret )break;
	}
	if( i == CHAR_MAXPETHAVE )i = 0;
			
	snprintf( token, sizeof( token ), "K%d", i );
	CHAR_sendStatusString( charaindex, token );
	snprintf( token, sizeof( token ), "W%d", i );
	CHAR_sendStatusString( charaindex, token );
	CHAR_DelItem( charaindex, haveitemindex);
}
#endif

#ifdef _BATTLE_PK
void ITEM_BattlePK( int charaindex, int toindex, int haveitemindex)
{
	int itemindex = CHAR_getItemIndex( charaindex, haveitemindex);
	int playernum = CHAR_getPlayerMaxNum();
	char token[256];
	if( !CHAR_CHECKINDEX( charaindex ) ) return;
	if( !CHAR_CHECKINDEX( toindex ) ) return;
	if( CHAR_getInt( toindex, CHAR_WHICHTYPE) != CHAR_TYPEPLAYER)
	{
	   CHAR_talkToCli( charaindex, -1, "只能挑战玩家哦！", CHAR_COLORYELLOW);
	   return;
	}
	if( charaindex == toindex )
	{
	   CHAR_talkToCli( charaindex, -1, "难道有自己挑战自己的吗？", CHAR_COLORYELLOW);
	   return;
	}
	CHAR_DischargePartyNoMsg( charaindex );//解散团队
	CHAR_DischargePartyNoMsg( toindex );//解散团队
	BATTLE_CreateVsPlayer(charaindex,toindex);
	CHAR_setInt( charaindex, CHAR_BATTLEPK,TRUE);
	CHAR_setInt( toindex, CHAR_BATTLEPK,TRUE);
	sprintf( token,"成功向对方%s挑战!",CHAR_getChar( toindex, CHAR_NAME));
	CHAR_talkToCli( charaindex, -1, token, CHAR_COLORYELLOW);
	sprintf( token,"对方%s正式向你挑战!",CHAR_getChar( charaindex, CHAR_NAME));
	CHAR_talkToCli( toindex, -1, token, CHAR_COLORYELLOW);
	CHAR_DelItem( charaindex, haveitemindex);
}
#endif

#ifdef _SILENTZERO
void ITEM_SetSilentZero( int charaindex, int toindex, int haveitemindex)
{
		CHAR_setInt(charaindex,CHAR_SILENT, 0);
		CHAR_setWorkInt( charaindex, CHAR_WORKLOGINTIME, (int)NowTime.tv_sec);
		CHAR_DelItem( charaindex, haveitemindex);
}
#endif

#ifdef _PET_LEVEL_ITEM
void ITEM_PetLevelItem( int charaindex, int toindex, int haveitemindex)
{
  if( !CHAR_CHECKINDEX( charaindex ) ) return;
  if( !CHAR_CHECKINDEX( toindex ) ) return;
  if( CHAR_getInt( toindex, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER)
  {
  	CHAR_talkToCli( charaindex, -1, "该物品只能给宠物使用！", CHAR_COLORYELLOW);
    return;
  }
	if( CHAR_getInt( toindex, CHAR_PETID) == 718 || CHAR_getInt( toindex, CHAR_PETID) == 401 )
	{
		CHAR_talkToCli( charaindex, -1, "该物品不能给MM使用！", CHAR_COLORYELLOW);
		return;
	}
	int itemindex = CHAR_getItemIndex( charaindex, haveitemindex);
	if(!ITEM_CHECKINDEX(itemindex)) return;
	char *itemarg = ITEM_getChar( itemindex, ITEM_ARGUMENT);
	char low[12],hight[12];
	getStringFromIndexWithDelim( itemarg, "|", 1, low, sizeof(low));
	getStringFromIndexWithDelim( itemarg, "|", 2, hight, sizeof(hight));
	if(CHAR_getInt( toindex, CHAR_LV)<atoi(low) && (CHAR_getInt(toindex, CHAR_LIMITLEVEL)>0 && CHAR_getInt(toindex, CHAR_LIMITLEVEL)<atoi(low))){
		CHAR_talkToCli( charaindex, -1, "目前你的宠物无法使用该物品！", CHAR_COLORYELLOW);
		return;
	}else	if(CHAR_getInt( toindex, CHAR_LV)>=atoi(hight) && CHAR_getInt(toindex, CHAR_LIMITLEVEL)>=atoi(hight)){
		CHAR_talkToCli( charaindex, -1, "目前你的宠物不必再使用该物品！", CHAR_COLORYELLOW);
		return;
	}
	CHAR_setInt(toindex, CHAR_LIMITLEVEL, atoi(hight));
	if(CHAR_getInt( toindex, CHAR_EXP)<0){
		int nextexp = CHAR_GetLevelExp( toindex, CHAR_getInt(toindex, CHAR_LV));
		CHAR_setInt( charaindex, CHAR_EXP , nextexp );
	}
		
	char token[256];
	sprintf(token, "宠物%s等级已突破%d级！",CHAR_getChar( toindex, CHAR_NAME), atoi(hight));
	CHAR_talkToCli( charaindex, -1, token, CHAR_COLORYELLOW);
	CHAR_DelItem( charaindex, haveitemindex);
}
#endif

#ifdef _ITEM_EFMETAMO
void ITEM_efMetamo( int charaindex, int toindex, int haveitemindex)
{
	int itemindex = CHAR_getItemIndex( charaindex, haveitemindex);
	if(!ITEM_CHECKINDEX(itemindex)) return;

	char *itemarg = ITEM_getChar( itemindex, ITEM_ARGUMENT);
	if( itemarg == "" ){
		CHAR_talkToCli(charaindex, -1, "这是个永久变身戒指!", CHAR_COLORYELLOW);
		return;
	}
	CHAR_setInt( charaindex , CHAR_BASEIMAGENUMBER , atoi(itemarg) );
	CHAR_setInt( charaindex , CHAR_BASEBASEIMAGENUMBER , atoi(itemarg));

	CHAR_complianceParameter( charaindex );
	CHAR_sendCToArroundCharacter( CHAR_getWorkInt( charaindex , CHAR_WORKOBJINDEX ));
	CHAR_send_P_StatusString( charaindex , CHAR_P_STRING_BASEBASEIMAGENUMBER);
	CHAR_DelItem( charaindex, haveitemindex);
}
#endif

#ifdef _PET_BEATITUDE
void PET_BEATITUDE( int charaindex, int toindex, int haveitemindex)
{
  if( !CHAR_CHECKINDEX( charaindex ) ) return;
  if( !CHAR_CHECKINDEX( toindex ) ) return;
  if( CHAR_getInt( toindex, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER)
  {
  	CHAR_talkToCli( charaindex, -1, "该物品只能给宠物使用！", CHAR_COLORYELLOW);
    return;
  }
	if( CHAR_getInt( toindex, CHAR_PETID) == 718 || CHAR_getInt( toindex, CHAR_PETID) == 401 )
	{
		CHAR_talkToCli( charaindex, -1, "该物品不能给MM使用！", CHAR_COLORYELLOW);
		return;
	}
	int itemindex = CHAR_getItemIndex( charaindex, haveitemindex);
	if(!ITEM_CHECKINDEX(itemindex)) return;
	char *itemarg = ITEM_getChar( itemindex, ITEM_ARGUMENT);
	char beatitude[12],mun[12];
	getStringFromIndexWithDelim( itemarg, "|", 1, beatitude, sizeof(beatitude));
	getStringFromIndexWithDelim( itemarg, "|", 2, mun, sizeof(mun));
	char token[256];
	int beat=CHAR_getInt( toindex, CHAR_BEATITUDE);
	if(strcmp( beatitude, "体")==0){
		if(CHAR_getInt( toindex, CHAR_BEATITUDE)&BEATITUDE_VITAL){
			CHAR_talkToCli( charaindex, -1, "您的宠物该项属性已提升过了！", CHAR_COLORYELLOW);
			return;
		}else{
			sprintf(token, "%s体力上升%d", CHAR_getChar( toindex, CHAR_NAME),atoi(mun));
			CHAR_setInt( toindex, CHAR_VITAL,(CHAR_getInt( toindex, CHAR_VITAL)+atoi(mun)));
			beat|=BEATITUDE_VITAL;
		}
	}else if(strcmp( beatitude, "攻")==0){
		if(CHAR_getInt( toindex, CHAR_BEATITUDE)&BEATITUDE_STR){
			CHAR_talkToCli( charaindex, -1, "您的宠物该项属性已提升过了！", CHAR_COLORYELLOW);
			return;
		}else{
			sprintf(token, "%s攻击力上升%d", CHAR_getChar( toindex, CHAR_NAME),atoi(mun));
			CHAR_setInt( toindex, CHAR_STR, (CHAR_getInt( toindex, CHAR_STR)+atoi(mun)));
			beat|=BEATITUDE_STR;
		}
	}else if(strcmp( beatitude, "防")==0){
		if(CHAR_getInt( toindex, CHAR_BEATITUDE)&BEATITUDE_TOUGH){
			CHAR_talkToCli( charaindex, -1, "您的宠物该项属性已提升过了！", CHAR_COLORYELLOW);
			return;
		}else{
			sprintf(token, "%s防御力上升%d", CHAR_getChar( toindex, CHAR_NAME),atoi(mun));
			CHAR_setInt( toindex, CHAR_TOUGH, (CHAR_getInt( toindex, CHAR_TOUGH)+atoi(mun)));
			beat|=BEATITUDE_TOUGH;
		}
	}else if(strcmp( beatitude, "敏")==0){
		if(CHAR_getInt( toindex, CHAR_BEATITUDE)&BEATITUDE_DEX){
			CHAR_talkToCli( charaindex, -1, "您的宠物该项属性已提升过了！", CHAR_COLORYELLOW);
			return;
		}else{
			sprintf(token, "%s敏捷力上升%d", CHAR_getChar( toindex, CHAR_NAME),atoi(mun));
			CHAR_setInt( toindex, CHAR_DEX, (CHAR_getInt( toindex, CHAR_DEX)+atoi(mun)));
			beat|=BEATITUDE_DEX;
		}
	}else{
		sprintf(token, "%s当前属性为体:%d,攻:%d,防:%d,敏:%d", CHAR_getChar( toindex, CHAR_NAME),
																													CHAR_getInt( toindex, CHAR_VITAL),
																													CHAR_getInt( toindex, CHAR_STR),
																													CHAR_getInt( toindex, CHAR_TOUGH),
																													CHAR_getInt( toindex, CHAR_DEX));
		CHAR_talkToCli( charaindex, -1, token, CHAR_COLORYELLOW);
		return;
	}
	CHAR_talkToCli( charaindex, -1, "主人，这东西太棒了，我已充满力量了！", CHAR_COLORYELLOW);
	CHAR_setInt( toindex, CHAR_BEATITUDE,beat);
	CHAR_talkToCli( charaindex, -1, token, CHAR_COLORYELLOW);
	int i;
	for( i = 0; i < CHAR_MAXPETHAVE; i ++ ){
		if( CHAR_getCharPet( charaindex, i ) == toindex )break;
	}
	if( i == CHAR_MAXPETHAVE )return;
	CHAR_complianceParameter( toindex );
	snprintf( token, sizeof( token ), "K%d", i );
	CHAR_sendStatusString( charaindex, token );
	snprintf( token, sizeof( token ), "W%d", i );
	CHAR_sendStatusString( charaindex, token );

	CHAR_talkToCli( charaindex, -1, token, CHAR_COLORYELLOW);
	CHAR_DelItem( charaindex, haveitemindex);
}
#endif




#ifdef _GET_MULTI_ITEM
void ITEM_GetMultiItem( int charaindex, int toindex, int haveitemindex)
{
	int itemindex;
	char *itemarg=NULL;
	char buf[32];
	int itemnum=0;
	int itemid={-1};
	char itemname[512]="";
  int si=0, ret,i;
	char token[256];

	itemindex = CHAR_getItemIndex( charaindex, haveitemindex);
	if(!ITEM_CHECKINDEX(itemindex)) return;

	itemarg = ITEM_getChar( itemindex, ITEM_ARGUMENT);
	if( itemarg == "" ){
		CHAR_talkToCli(charaindex, -1, "这是个空盒子!", CHAR_COLORYELLOW);
		return;
	}

	if( getStringFromIndexWithDelim( itemarg, "|", 1, buf, sizeof( buf)) == FALSE ){
		CHAR_talkToCli(charaindex, -1, "不明东西!", CHAR_COLORYELLOW);
		return;
	}
  itemnum = atoi(buf);
	if( itemnum > 10 ){
		itemnum = 10;
	}else if( itemnum < 0 ){
		CHAR_talkToCli(charaindex, -1, "这是个空盒子!", CHAR_COLORYELLOW);
		return;
	}
	if( (itemnum - 1) <= CHAR_findSurplusItemBox( charaindex )){
		CHAR_DelItem( charaindex, haveitemindex);
	}else{
		sprintf( token, "盒子里有%d件物品，你的物品栏位不足。", itemnum);
		CHAR_talkToCli( charaindex, -1, token, CHAR_COLORYELLOW);
		return;
	}

	for(i=0;i<itemnum;i++){
		if( getStringFromIndexWithDelim( itemarg, "|", 2+i, buf, sizeof( buf)) == FALSE ){
			CHAR_talkToCli(charaindex, -1, "不明东西!", CHAR_COLORYELLOW);
			return;
		}
		itemid = atoi(buf);
		itemindex = ITEM_makeItemAndRegist( itemid );
		if( itemindex != -1 ){
			int emptyteimbox=CHAR_findEmptyItemBox( charaindex );
		  CHAR_setItemIndex( charaindex, emptyteimbox, itemindex );
		  ITEM_setWorkInt(itemindex, ITEM_WORKOBJINDEX,-1);
		  ITEM_setWorkInt(itemindex, ITEM_WORKCHARAINDEX,charaindex);
		  CHAR_sendItemDataOne( charaindex, emptyteimbox);
		  strcat(itemname,ITEM_getChar( itemindex, ITEM_NAME));
		  strcat(itemname,"|");
		}
	}
	sprintf( token,"获得物品:|%s",itemname);
	CHAR_talkToCli( charaindex, -1, token, CHAR_COLORYELLOW );
}
#endif

void ITEM_NewName( int charaindex, int toindex, int haveitemindex)
{
		int itemindex = CHAR_getItemIndex( charaindex, haveitemindex);
		char *itemarg="\0";
		char token[64];
		itemarg=ITEM_getChar(itemindex,ITEM_ARGUMENT);
		if( itemarg == "\0" ) return;
		CHAR_setChar( charaindex , CHAR_NEWNAME, itemarg);
		sprintf( token, "恭喜你,你已获得〖%s〗新称号!", itemarg);
		CHAR_talkToCli( charaindex, -1, token, CHAR_COLORYELLOW );		
		int i;
		int playernum = CHAR_getPlayerMaxNum();
		char NameMsg[256];
		char *MyName = CHAR_getChar( charaindex,CHAR_NAME );
		for( i = 0 ; i < playernum ; i++) 
				{
        	sprintf( NameMsg, "恭喜玩家「%s」获得≮%s≯称号!", MyName, itemarg );
					CHAR_talkToCli( i, -1, NameMsg, CHAR_COLORBLUE2);
  			}
		CHAR_DelItem( charaindex, haveitemindex); 
}
