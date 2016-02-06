
#ifndef __ITEM_H__
#define __ITEM_H__

#include "char.h"

#define NULLITEM    "0"

typedef enum
{
    ITEM_FIST =0,
    ITEM_AXE,
    ITEM_CLUB,
    ITEM_SPEAR,
    ITEM_BOW,
    ITEM_SHIELD,
    ITEM_HELM,
    ITEM_ARMOUR,

	ITEM_BRACELET =8,
	ITEM_MUSIC,
	ITEM_NECKLACE,
	ITEM_RING,
	ITEM_BELT,
	ITEM_EARRING,
	ITEM_NOSERING,
	ITEM_AMULET,
    /* ****** */
    ITEM_OTHER =16,
    ITEM_BOOMERANG,		// »ØÐý±ê
    ITEM_BOUNDTHROW,	// Í¶ÖÀ¸«Í·
    ITEM_BREAKTHROW,	// Í¶ÖÀÊ¯
    ITEM_DISH =20,
#ifdef _ITEM_INSLAY
	ITEM_METAL,
	ITEM_JEWEL,
#endif
#ifdef _ITEM_CHECKWARES
	ITEM_WARES,			//»õÎï
#endif
    ITEM_CATEGORYNUM,
    
}ITEM_CATEGORY;

typedef enum
{
	ITEM_FIELD_ALL,
	ITEM_FIELD_BATTLE,
	ITEM_FIELD_MAP,
}ITEM_FIELDTYPE;

typedef enum
{
	ITEM_TARGET_MYSELF,
	ITEM_TARGET_OTHER,
	ITEM_TARGET_ALLMYSIDE,
	ITEM_TARGET_ALLOTHERSIDE,
	ITEM_TARGET_ALL,
}ITEM_TARGETTYPE;

typedef enum
{
    ITEM_ID,
    ITEM_BASEIMAGENUMBER,
    ITEM_COST,
    ITEM_TYPE,
		ITEM_ABLEUSEFIELD,
		ITEM_TARGET,
    ITEM_LEVEL,                     /*  LEVEL  */
#ifdef _ITEM_MAXUSERNUM
	ITEM_DAMAGEBREAK,				//ÎïÆ·Ê¹ÓÃ´ÎÊý
#endif

#ifdef _ITEMSET4_TXT
	ITEM_USEPILENUMS,				//ÎïÆ·¶Ñµþ´ÎÊý
	ITEM_CANBEPILE,					//ÊÇ·ñ¿É¶Ñµþ

	ITEM_NEEDSTR,
	ITEM_NEEDDEX,
	ITEM_NEEDTRANS,
	ITEM_NEEDPROFESSION,
#endif

#ifdef _TAKE_ITEMDAMAGE
	ITEM_DAMAGECRUSHE,
	ITEM_MAXDAMAGECRUSHE,
#endif

#ifdef _ADD_DEAMGEDEFC
	ITEM_OTHERDAMAGE,
	ITEM_OTHERDEFC,
#endif

#ifdef _SUIT_ITEM
	ITEM_SUITCODE,
#endif

    ITEM_ATTACKNUM_MIN,             /*    ìµ  »«¼ÔÐÑ  */
    ITEM_ATTACKNUM_MAX,             /*    æÎ  »«¼ÔÐÑ  */
    ITEM_MODIFYATTACK,              /*    »«    ¼ÀÐÚ  */
    ITEM_MODIFYDEFENCE,             /*    »¿    ¼ÀÐÚ  */
    ITEM_MODIFYQUICK,               /*  QUICK  ¼ÀÐÚ  */

    ITEM_MODIFYHP,                  /*  HP  ¼ÀÐÚ    */
    ITEM_MODIFYMP,                  /*  MP  ¼ÀÐÚ    */
    ITEM_MODIFYLUCK,                /*  LUCK  ¼ÀÐÚ    */
    ITEM_MODIFYCHARM,               /*  CHARM  ¼ÀÐÚ    */
    ITEM_MODIFYAVOID,               /*  ¼Ô    Ä¤¿Ò    */
	ITEM_MODIFYATTRIB,				/*  óìÁëÄ¤¿Ò */
	ITEM_MODIFYATTRIBVALUE,			/*  óìÁëÄ¤¿Ò°À */
	ITEM_MAGICID,					/*  ÈÈÖî  Ä¯ */
	ITEM_MAGICPROB,					/*  ÈÈÖî       */
	ITEM_MAGICUSEMP,				/*  ÛÕ  MP */

#ifdef _ITEMSET5_TXT
	ITEM_MODIFYARRANGE,
	ITEM_MODIFYSEQUENCE,

	ITEM_ATTACHPILE,
	ITEM_HITRIGHT,	//¶îÍâÃüÖÐ
#endif
#ifdef _ITEMSET6_TXT
	ITEM_NEGLECTGUARD,
//	ITEM_BEMERGE,
#endif
    /*  µ©  ¡õÕýµ©Ä¤¿Ò°À£Û*/
    ITEM_POISON,					/*   ÐÚÈÊÐ×Ì«±åÄ¸¶ª¡õ³â          */
    ITEM_PARALYSIS,          		/* ØÆÌ«Ä¾£ý1  ¼°µæ  »¥Æ¥ÎåØ¦ÖÐ£Û */
    ITEM_SLEEP,              		/* ìæÔ»£Ûµæ  Æ¥ÎåØ¦ÖÐ            */
    ITEM_STONE,              		/* éÞ£Ûµæ  Æ¥ÎåØ¦ÖÐ              */
    ITEM_DRUNK,              		/* °ìµ¤£Û      »¥Æ±»¥ÔÂ     */
    ITEM_CONFUSION,          		/* ÓæØÙ£Û  »«    Ã«êßÔÂ     */

	ITEM_CRITICAL,					/* ÛÍØø  Å«ÊÐ»ï  Ä¤¿Ò */

	ITEM_USEACTION,					/* ÒøÔÈÐ×ÁÝ¼°Ê§ÛÍÆËÒà¼þ */
    ITEM_DROPATLOGOUT,              /* ·òºëÊ§ËüÐþÔÊÔÂÁÝ±å  ÔÊ¾®Éýµ¤¾®  */
    ITEM_VANISHATDROP,              /*   ØÆÐ×ÁÝ±åÛÕÒüÔÂ¾®Éýµ¤¾® */
    ITEM_ISOVERED,                  /*  Ïþ±åê¼ÔÈ¾®ÈÕÈÕÄ¾ÔÂ¾®Éýµ¤¾®£Û*/
	ITEM_CANPETMAIL,				/* Ê¸ÓÀÐþ¶ª¡õ»ïÆ¥ËªÄ¾ÔÂ¾® */
	ITEM_CANMERGEFROM,				/* ÄþÔÀÝç±åØ¦Ä¾ÔÂ¾® */
	ITEM_CANMERGETO,				/* ÄþÔÀÛÆ±åØ¦Ä¾ÔÂ¾® */

    ITEM_INGVALUE0,                 /* ÔÀÛÐ(5òÛÛÐ) */
    ITEM_INGVALUE1,
    ITEM_INGVALUE2,
    ITEM_INGVALUE3,
    ITEM_INGVALUE4,
    
	ITEM_PUTTIME,					/*  Ê§ÄÌ  Ø©»¥  ¾®Ä¾Ð×ÁÝÃÞ */
    ITEM_LEAKLEVEL,                 /*    âô»¥ÉýÄ¾·ÖØêÈÉÄ¾Ð×¾®  */
	ITEM_MERGEFLG,					/*  ÄþÔÀ½ñÄ¾Ð×Ê§ÄÌ  Ø©¾®Éýµ¤¾® */
	ITEM_CRUSHLEVEL,				/*  Ã§Ä¾ÕºÄþÖÐ 0  2 ¨ß·´Ã§Ä¾»¯Ø¦ÖÐ 2·´òåÃ§ */

    ITEM_VAR1,              	/*    åÃ×Û½ö       */
    ITEM_VAR2,              	/*    åÃ×Û½ö       */
    ITEM_VAR3,              	/*    åÃ×Û½ö       */
    ITEM_VAR4,              	/*    åÃ×Û½ö       */

	ITEM_DATAINTNUM,

}ITEM_DATAINT;

typedef enum
{
    ITEM_NAME,                      /*    ó¡     ñ²¼°  ó¡     */
    ITEM_SECRETNAME,                /*    ó¡    µÊ½ñÄ¾ÔÂµÚ  Áë  Ô»    */
    ITEM_EFFECTSTRING,              /*  ¶ã°í  Ù¯    */
    ITEM_ARGUMENT,                  /*  Ê§ÄÌ  Ø©¼°Â¦ÐÑ  */
#ifdef _ITEM_INSLAY
	ITEM_TYPECODE,
	ITEM_INLAYCODE,
#endif
	ITEM_CDKEY,						/*  Ê§ÄÌ  Ø©¼°  ó¡Ã«  âÙ±å  µÊØÆÐ×ÚÐ¼°           */
#ifdef _ITEM_FORUSERNAMES
	ITEM_FORUSERNAME,
	ITEM_FORUSERCDKEY,
#endif
// CoolFish: 2001/10/11
#ifdef _UNIQUE_P_I
    ITEM_UNIQUECODE,		  /* ÎïÆ·±àÂë */
#endif

    ITEM_INGNAME0,                  /*  ÔÀÛÐ¼°  ó¡(5òÛÛÐ) */
    ITEM_INGNAME1,
    ITEM_INGNAME2,
    ITEM_INGNAME3,
    ITEM_INGNAME4,


    ITEM_INITFUNC,                  /* Â¦ÐÑ
                                     * ITEM_Item*
                                     * ß¯Ô»°À BOOL
                                     * ß¯Ô»°À¼°À²  ·´ CHAR_INITFUNC
                                     * Îç  Ôª  */
    ITEM_FIRSTFUNCTION = ITEM_INITFUNC,
    ITEM_PREOVERFUNC,               /* CHAR_PREOVERFUNC Ã«¸¨å¾ */
    ITEM_POSTOVERFUNC,              /* CHAR_POSTOVERFUNC Ã«¸¨å¾*/
    ITEM_WATCHFUNC,                 /* CHAR_WATCHFUNC Ã«¸¨å¾ */
    ITEM_USEFUNC,                   /* Â¦ÐÑ·´£ý
                                     * int charaindex Æ½ÅÒ·ÂÄÌ¼þ·¸ÓÀÛÍµ©
                                     * int charitemindex ·ßÛÐ¼°
                                     *              Ê§ÄÌ  Ø©    ¼°ÖÏ    
                                     *              Ã«ÒøÔÈÐ×¾®
                                     */
    ITEM_ATTACHFUNC,                /* Â¦ÐÑ·´£ý
                                     * int charaindex Æ½ÅÒ·ÂÄÌ¼þ·¸ÓÀÛÍµ©
                                     * int itemindex  Ê§ÄÌ  Ø©ÄÌ¼þ·¸ÓÀÛÍµ©
                                     *      Æ½ÅÒ·ÂÛÍÕý¼°  ÔÈ»¯ÖÐÔÂÊ§ÄÌ  Ø©
                                     *      ¼°Ê§ÄÌ  Ø©  Æ¥¼°ÄÌ¼þ·¸ÓÀÛÍµ©
                                     *      Æ¥·´Ø¦ÖÐÒÇ±åïÛÀ²£Û
                                     */
    ITEM_DETACHFUNC,                /* Â¦ÐÑ·´£ý
                                     * int charaindex Æ½ÅÒ·ÂÄÌ¼þ·¸ÓÀÛÍµ©
                                     * int itemindex  Ê§ÄÌ  Ø©ÄÌ¼þ·¸ÓÀÛÍµ©
                                     *      Æ½ÅÒ·ÂÛÍÕý¼°  ÔÈ»¯ÖÐÔÂÊ§ÄÌ  Ø©
                                     *      ¼°Ê§ÄÌ  Ø©  Æ¥¼°ÄÌ¼þ·¸ÓÀÛÍµ©
                                     *      Æ¥·´Ø¦ÖÐÒÇ±åïÛÀ²£Û
                                     */
    ITEM_DROPFUNC, 		            /*   ÎçØÆÐ×ÎçÎå
                                     * Â¦ÐÑ·´
                                     *  int charaindex   ÎçØÆÐ×Æ½ÅÒ·Â
                                     *  int itemindex Ê§ÄÌ  Ø©ÄÌ¼þ·¸ÓÀÛÍµ©
                                     */
    ITEM_PICKUPFUNC,              /* Ê§ÄÌ  Ø©Ã«½ºÔÈÐ×ÁÝ
                                     * Â¦ÐÑ·´
                                     *  int charaindex  ½ºÔÈÐ×Æ½ÅÒ·Âindex
                                     *  int itemindex Ê§ÄÌ  Ø©ÄÌ¼þ·¸ÓÀÛÍµ©
                                     */
#ifdef _Item_ReLifeAct
	ITEM_DIERELIFEFUNC,					/*ANDY_ADD
										¸´»îµÀ¾ß	
									 */
#endif

    ITEM_LASTFUNCTION,

    ITEM_DATACHARNUM = ITEM_LASTFUNCTION,

}ITEM_DATACHAR;

typedef enum
{
    ITEM_WORKOBJINDEX,
    ITEM_WORKCHARAINDEX,
#ifdef _ITEM_ORNAMENTS
	ITEM_CANPICKUP,
#endif
#ifdef _ITEM_TIME_LIMIT
	ITEM_WORKTIMELIMIT,
#endif
    ITEM_WORKDATAINTNUM,
}ITEM_WORKDATAINT;



typedef struct tagItem
{
    int         data[ITEM_DATAINTNUM];
    STRING64    string[ITEM_DATACHARNUM];
    int         workint[ITEM_WORKDATAINTNUM];

    void*       functable[ITEM_LASTFUNCTION-ITEM_FIRSTFUNCTION];
}ITEM_Item;

typedef struct tagITEM_table
{
    ITEM_Item   itm;
    int         randomdata[ITEM_DATAINTNUM];
}ITEM_table;

typedef struct tagITEM_index
{
    BOOL         use; 
    int         index;
}ITEM_index;

typedef struct tagITEM_exists
{
    BOOL        use;
    ITEM_Item   itm;
}ITEM_exists;

#define		ITEM_CHECKINDEX(index)		\
	_ITEM_CHECKINDEX( __FILE__, __LINE__, index)
INLINE BOOL _ITEM_CHECKINDEX( char *file, int line, int index);


BOOL ITEM_initExistItemsArray( int num );
BOOL ITEM_endExistItemsArray( ITEM_table* ITEM_item );
#define		ITEM_initExistItemsOne( itm) \
	_ITEM_initExistItemsOne( __FILE__, __LINE__, itm)
int _ITEM_initExistItemsOne( char *file, int line, ITEM_Item* itm );

#define		ITEM_endExistItemsOne( index ) \
			_ITEM_endExistItemsOne( index, __FILE__, __LINE__)

void _ITEM_endExistItemsOne( int index , char *file, int line);

#define ITEM_getInt( Index, element) _ITEM_getInt( __FILE__, __LINE__, Index, element )
INLINE int _ITEM_getInt( char *file, int line, int index ,ITEM_DATAINT element);


#define ITEM_setInt( Index, element, data) _ITEM_setInt( __FILE__, __LINE__, Index, element, data)
INLINE int _ITEM_setInt( char *file, int line, int index ,ITEM_DATAINT element, int data);


INLINE char* ITEM_getChar( int index ,ITEM_DATACHAR element );
INLINE BOOL ITEM_setChar( int index ,ITEM_DATACHAR element , char* new);

INLINE int ITEM_getWorkInt( int index ,ITEM_WORKDATAINT element);
INLINE int ITEM_setWorkInt( int index ,ITEM_WORKDATAINT element, int data);
INLINE int ITEM_getITEM_itemnum( void );
INLINE int ITEM_getITEM_UseItemnum( void );
INLINE BOOL ITEM_getITEM_use( int index );
void ITEM_constructFunctable( int itemindex );
void* ITEM_getFunctionPointer( int itemindex, int functype );
INLINE ITEM_Item *ITEM_getItemPointer( int index );
int ITEM_getItemMaxIdNum( void);


char* ITEM_makeStringFromItemData( ITEM_Item* one, int mode );
char* ITEM_makeStringFromItemIndex( int index, int mode );

BOOL ITEM_makeExistItemsFromStringToArg( char* src , ITEM_Item* item, int mode );
void ITEM_getDefaultItemSetting( ITEM_Item* itm);


INLINE BOOL ITEM_CHECKITEMTABLE( int number );
BOOL    ITEM_readItemConfFile( char* filename );


CHAR_EquipPlace ITEM_getEquipPlace( int charaindex, int itmid );


char*  ITEM_makeItemStatusString( int haveitemindex, int itemindex );
char*   ITEM_makeItemFalseString( void );
char*   ITEM_makeItemFalseStringWithNum( int haveitemindex );


BOOL ITEM_makeItem( ITEM_Item* itm, int number );
int ITEM_makeItemAndRegist( int number );


void ITEM_equipEffect( int index );

void Other_DefcharWorkInt( int index);

char* ITEM_getAppropriateName(int itemindex);
char* ITEM_getEffectString( int itemindex );


int ITEM_getcostFromITEMtabl( int itemid );

#define ITEM_getNameFromNumber( id) _ITEM_getNameFromNumber( __FILE__, __LINE__, id)
INLINE char* _ITEM_getNameFromNumber( char *file, int line, int itemid );


int ITEM_getlevelFromITEMtabl( int itemid );
int ITEM_getgraNoFromITEMtabl( int itemid );
char *ITEM_getItemInfoFromNumber( int itemid );

int ITEM_getdropatlogoutFromITEMtabl( int itemid );
int ITEM_getvanishatdropFromITEMtabl( int itemid );
int ITEM_getcanpetmailFromITEMtabl( int itemid );
int ITEM_getmergeItemFromFromITEMtabl( int itemid );

#ifdef _ITEM_CHECKWARES
BOOL CHAR_CheckInItemForWares( int charaindex, int flg);
#endif

BOOL ITEM_canuseMagic( int itemindex);
// Nuke +1 08/23 : For checking the validity of item target
int ITEM_isTargetValid( int charaindex, int itemindex, int toindex);

int ITEMTBL_getInt( int ItemID, ITEM_DATAINT datatype);
char *ITEMTBL_getChar( int ItemID, ITEM_DATACHAR datatype);

int ITEM_getItemDamageCrusheED( int itemindex);
void ITEM_RsetEquit( int charaindex);//×Ô¶¯Ð¶³ý×°±¸Î»ÖÃ´íÎóÖ®ÎïÆ·

#ifdef _SIMPLIFY_ITEMSTRING
void ITEM_getDefaultItemData( int itemID, ITEM_Item* itm);
#endif

#endif
