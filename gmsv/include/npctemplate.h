#ifndef __READNPCTEMPLATE_H__
#define __READNPCTEMPLATE_H__

#include "util.h"
#include "char_base.h"

#define NPC_TEMPLATEFILEMAGIC   "NPCTEMPLATE\n"

#undef EXTERN
#ifdef __NPCTEMPLATE__
#define EXTERN
#else
#define EXTERN extern
#endif /*__NPCTEMPLATE__*/



typedef struct tagNPC_haveItem
{
    int     itemnumber;
    int     haverate;
    int     havenum;
}NPC_haveItem;

typedef enum
{
    NPC_TEMPLATENAME,           /*    ¼þÃóÒÁ¡õÐþ¼°  ó¡  */
    NPC_TEMPLATECHARNAME,       /*    ó¡    */

    NPC_TEMPLATEINITFUNC,       /*  CHAR_INITFUNC±åµæÈÊ */
    NPC_TEMPLATEWALKPREFUNC,    /*  CHAR_WALKPREFUNC    */
    NPC_TEMPLATEWALKPOSTFUNC,   /*  CHAR_WALKPOSTFUNC   */
    NPC_TEMPLATEPREOVERFUNC,    /*  CHAR_PREOVERFUNC    */
    NPC_TEMPLATEPOSTOVERFUNC,   /*  CHAR_POSTOVERFUNC   */
    NPC_TEMPLATEWATCHFUNC,      /*  CHAR_WATCHFUNC      */
    NPC_TEMPLATELOOPFUNC,       /*  CHAR_LOOPFUNC   */
    NPC_TEMPLATEDYINGFUNC,      /*  CHAR_DYINGFUNC  */
    NPC_TEMPLATETALKEDFUNC,     /*  CHAR_TALKEDFUNC */

    NPC_TEMPLATEPREATTACKEDFUNC,        /*  CHAR_PREATTACKEDFUNC    */
    NPC_TEMPLATEPOSTATTACKEDFUNC,       /*  CHAR_POSTATTACKEDFUNC    */

    NPC_TEMPLATEOFFFUNC,                /*  CHAR_OFFFUNC    */
    NPC_TEMPLATELOOKEDFUNC,            /*  CHAR_LOOKEDFUNC  */
    NPC_TEMPLATEITEMPUTFUNC,            /*  CHAR_ITEMPUTFUNC    */

    NPC_TEMPLATESPECIALTALKEDFUNC,    /*  CHAR_SPECIALTALKEDFUNC   */
    NPC_TEMPLATEWINDOWTALKEDFUNC,    /*  CHAR_WINDOWTALKEDFUNC   */
#ifdef _USER_CHARLOOPS
	NPC_TEMPLATELOOPFUNCTEMP1,		//CHAR_LOOPFUNCTEMP1,
	NPC_TEMPLATELOOPFUNCTEMP2,		//CHAR_LOOPFUNCTEMP2,
	NPC_TEMPLATEBATTLEPROPERTY,		//CHAR_BATTLEPROPERTY,
#endif
    NPC_TEMPLATECHARNUM,
}NPC_TEMPLATECHAR;

typedef enum
{
    NPC_TEMPLATEMAKEATNOBODY,           /* ²¾ÊÖÖÐØ¦ÖÐÁÝ±åÊÖ×ÛÔÂ¾®Éýµ¤¾® */
    NPC_TEMPLATEMAKEATNOSEE,            /* Î­ÒüØ¦ÖÐô÷Æ¥×ÛÔÂ¾®Éýµ¤¾®    */
    NPC_TEMPLATEIMAGENUMBER,            /*   ì«  Ä¯    */
    NPC_TEMPLATETYPE,                   /*     è£»§ÔÂÁÝ±åè£»§ÔÂ        */

    NPC_TEMPLATEMINHP,                  /* HP   */

    NPC_TEMPLATEMINMP,                  /* MP   */

    NPC_TEMPLATEMINSTR,                 /* STR  */

    NPC_TEMPLATEMINTOUGH,               /* TOUGH    */

    NPC_TEMPLATEISFLYING,               /*  ÊÏÆ¥ÔÂ¾®Éýµ¤¾®  */

    NPC_TEMPLATEITEMNUM,                /*   ÇÐµ¤ÔÂÊ§ÄÌ  Ø©¼°    ÐÑ    */

    NPC_TEMPLATELOOPFUNCTIME,           /*
                                         * ÖÏÁ¢Øø  ÈÔÎç±å»ï¡õÃóèúÐÑ
                                         * Ã«ôÄÉÙ¾®
                                         */
    NPC_TEMPLATEFUNCTIONINDEX,         /*
                                        * fucntionSet ¼°ÖÏ    ¼°
                                        * ÄÌ¼þ·¸ÓÀÛÍµ©¾®
                                        */

    NPC_TEMPLATEINTNUM,
}NPC_TEMPLATEINT;

typedef struct tagNPC_Template
{
    STRING64    chardata[NPC_TEMPLATECHARNUM];
    int         intdata[NPC_TEMPLATEINTNUM];
    int         randomdata[NPC_TEMPLATEINTNUM]; /*  ·Â¼þÄ¸Ø©¼°  Îå½ñ
                                                    »¥  ÔÈ»¯ÖÐÔÂ  */
    int         hash;
    NPC_haveItem*   haveitem;
}NPC_Template;


EXTERN NPC_Template*   NPC_template;
EXTERN int             NPC_templatenum;
EXTERN int             NPC_template_readindex;

INLINE int NPC_CHECKTEMPLATEINDEX(int index);

BOOL NPC_copyFunctionSetToChar( int id, Char* ch );

BOOL NPC_readNPCTemplateFiles( char* topdirectory ,int templatesize);
int NPC_templateGetTemplateIndex( char* templatename );

#endif
 /*__READNPCTEMPLATE_H__*/
