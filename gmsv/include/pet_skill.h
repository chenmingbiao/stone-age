#ifndef __PET_SKILL_H__
#define __PET_SKILL_H__

#include "util.h"

typedef enum
{
	PETSKILL_FIELD_ALL,			/* ÔÊÍÍ»¯¼°èëô÷Æ¥ÒøÒüÔÂ */
	PETSKILL_FIELD_BATTLE,				/* ¾ô    ¼°ÐÄ */
	PETSKILL_FIELD_MAP,				/* É§éÙÑ¨ÓÀÃóÏþ¼°ÐÄ */

}PETSKILL_FIELDTYPE;

typedef enum
{
	PETSKILL_TARGET_MYSELF,		/* ·ßÛÐ¼°ÐÄ */
	PETSKILL_TARGET_OTHER,			/* Ö°¼°ÚÐ  ·ßÛÐÖ³¸ê) */
	PETSKILL_TARGET_ALLMYSIDE,		/*     òå   */
	PETSKILL_TARGET_ALLOTHERSIDE,	/* ÇÂÅì´¡òå   */
	PETSKILL_TARGET_ALL,			/* òå»¯ */
	PETSKILL_TARGET_NONE,			/* ²¾ÊÖ¼»  Çë  Ø¦ÖÐ£Û  »¿Ö§Ð×»§¼°ÁÝ */
	PETSKILL_TARGET_OTHERWITHOUTMYSELF,/* Ö°¼°ÚÐ  ·ßÛÐÖ³ÒýØ¦ÖÐ) */
	PETSKILL_TARGET_WITHOUTMYSELFANDPET,  /* ·ßÛÐÎçÊ¸ÓÀÐþ¶¯Â½ */
}PETSKILL_TARGETTYPE;

typedef enum
{
	PETSKILL_ID,
	PETSKILL_FIELD,
	PETSKILL_TARGET,
#ifdef _PETSKILL2_TXT
	PETSKILL_USETYPE,
#endif
	PETSKILL_COST,
	PETSKILL_ILLEGAL,
	PETSKILL_DATAINTNUM,
}PETSKILL_DATAINT;

typedef enum
{
	PETSKILL_NAME,					/*      */
	PETSKILL_COMMENT,				/* Îì¶ª¼þÐþ*/
	PETSKILL_FUNCNAME,				/* èúÐÑ   */
	PETSKILL_OPTION,				/* ×óÃóÆËÒà¼þ */
#ifdef _CFREE_petskill
	PETSKILL_FREE,					/*Ìõ¼þ*/
	PETSKILL_KINDCODE,				/*ÖÖÀàÂë*/
#endif

	PETSKILL_DATACHARNUM,
}PETSKILL_DATACHAR;

typedef struct tagPetskill
{
	int			data[PETSKILL_DATAINTNUM];
	STRING64	string[PETSKILL_DATACHARNUM];

}Petskill;

typedef int (*PETSKILL_CALLFUNC)( int, int, int, char * );

INLINE BOOL PETSKILL_CHECKINDEX( int index );
INLINE int PETSKILL_getInt( int index, PETSKILL_DATAINT element);
INLINE int PETSKILL_setInt( int index, PETSKILL_DATAINT element, int data);
INLINE char* PETSKILL_getChar( int index, PETSKILL_DATACHAR element);
INLINE BOOL PETSKILL_setChar( int index ,PETSKILL_DATACHAR element, char* new );
int PETSKILL_getPetskillNum( void);

#define		PETSKILL_GetArray( charaindex, havepetskill)	_PETSKILL_GetArray( __FILE__, __LINE__, charaindex, havepetskill)
int _PETSKILL_GetArray( char *file, int line, int charaindex, int havepetskill );

BOOL PETSKILL_initPetskill( char *filename);
BOOL PETSKILL_reinitPetskill( void );
int PETSKILL_getPetskillArray( int petskillid);
PETSKILL_CALLFUNC PETSKILL_getPetskillFuncPointer(char* name);

int PETSKILL_Use(
	int charaindex,
	int toindex,
	int array,
	char *data
	//BOOL isCLI	// Robin 2001/02/26 if owner is player
);

int PETSKILL_ContinuationAttack(
	int charaindex,
	int toindex,
	int array,
	char *data
);
int PETSKILL_ChargeAttack(
	int charaindex,
	int toindex,
	int array,
	char *data
);


#define PETSKILL_ID_GBREAK		0	// ²¼¡õÓñÆ¤ÒÁÄÌÛÍ
#define PETSKILL_ID_RENZOKU		1	//   Á¸  »«
#define PETSKILL_ID_GUARDIAN	2	// ïá¾ËË¦ÃñõÓ
#define PETSKILL_ID_CHARGE		3	// ÃñÅÒ¡õ³â  »«
#define PETSKILL_ID_ICHIGEKI	100	// Óò»«  Ú½
#define PETSKILL_ID_POWERBALANCE 110	//     ¼°·í

//**********************************************************************
//
//  éÞ  -- Ö°ÚÐÃ«³±Ô»Ø¦»¥ÈÕ  »«
//
int PETSKILL_Guardian(
	int charaindex,
	int toindex,
	int array,
	char *data
);
//
//**********************************************************************

int PETSKILL_PowerBalance(
	int charaindex,
	int toindex,
	int array,
	char *data
);

//**********************************************************************
//
//  éÞ  -- ÐÛ  ·Ö»¥¼Ô  ½ñÄ¾ÔÂ¸î  »¥æÎÖÐ
//
int PETSKILL_Mighty(
	int charaindex,
	int toindex,
	int array,
	char *data
);
//
//**********************************************************************


//**********************************************************************
//
//  éÞ  -- µ©  ¡õÕýµ©³ªéÙ  »«
//
int PETSKILL_StatusChange(
	int charaindex,
	int toindex,
	int array,
	char *data

);
//
//**********************************************************************


//*******************************************************
//
// éÞ  -- É§éÙ  »«
//
int PETSKILL_NormalAttack(
	int charaindex,
	int toindex,
	int array,
	char *data

);
//
//*******************************************************

//*******************************************************
//
// éÞ  -- É§éÙ  »¿
//
int PETSKILL_NormalGuard(
	int charaindex,
	int toindex,
	int array,
	char *data

);
//
//*******************************************************

//*******************************************************
// éÞ  -- ½÷´°(ÖÏÊÖØÆØ¦ÖÐ)
//
int PETSKILL_None(
	int charaindex,
	int toindex,
	int array,
	char *data

);
//
//*******************************************************

//*******************************************************
// éÞ  -- »©ÌÑÓò´Ð(âêÄ¾»¯¨àÕý¡õ¼þ  ±åÇÂÅì¼°    ¾®ÈÕ  »«)
//
int PETSKILL_EarthRound(
	int charaindex,
	int toNo,
	int array,
	char *data

);
//
//*******************************************************

//*******************************************************
// éÞ  -- ²¼¡õÓñÆ¤ÒÁÄÌÛÍ
//
int PETSKILL_GuardBreak(
	int charaindex,
	int toNo,
	int array,
	char *data

);
///////////////////////////////////////////
#ifdef _SKILL_GUARDBREAK2//ÆÆ³ý·ÀÓù2 vincent add 2002/05/20
int PETSKILL_GuardBreak2(
	int charaindex,
	int toNo,
	int array,
	char *data

);
#endif
//
//*******************************************************
//*******************************************************
// éÞ  --     Ä¾
//
int PETSKILL_Abduct(
	int charaindex,
	int toNo,
	int array,
	char *data

);
//
//*******************************************************
//*******************************************************
// éÞ  --   ¸ê
int PETSKILL_Steal(
	int charaindex,
	int toNo,
	int array,
	char *data

);
//*******************************************************
#ifdef _BATTLESTEAL_FIX
int PETSKILL_StealMoney(
	int charaindex,
	int toNo,
	int array,
	char *data

);
#endif

#ifdef _ITEM_INSLAY
int PETSKILL_Inslay(
	int index,
	int toNo,
	int array,
	char *data

);
#endif

#ifdef _PETSKILL_FIXITEM
int PETSKILL_Fixitem(
	int index,
	int toNo,
	int array,
	char *data

);
#endif

// éÞ  -- Ê§ÄÌ  Ø©ÄþÔÀ
//
int PETSKILL_Merge(
	int charaindex,
	int toNo,
	int array,
	char *data

);
//
//*******************************************************

//*******************************************************
// éÞ  -- ÓÃ¡õ²¼¡õÓñ
//
int PETSKILL_NoGuard(
	int charaindex,
	int toNo,
	int array,
	char *data

);
//
//*******************************************************



// Terry add 2001/11/05
#ifdef _ATTACK_MAGIC
int PETSKILL_AttackMagic(
	int charaindex,
	int toindex,
	int array,
	char *data
);
#endif

#ifdef _VARY_WOLF
int PETSKILL_Vary( int cindex, int tindex, int id, char* data);
#endif

#ifdef _SKILL_WILDVIOLENT_ATT
//vincent add 2002/05/16
int PETSKILL_WildViolentAttack(
	int charaindex,
	int toindex,
	int array,
	char *data
);
#endif

#ifdef _SKILL_SPEEDY_ATT
//vincent add 2002/05/16
int PETSKILL_SpeedyAttack(
	int charaindex,
	int toindex,
	int array,
	char *data
);
#endif

#ifdef _SKILL_SACRIFICE
//vincent add 2002/05/30
int PETSKILL_Sacrifice(
	int charaindex,
	int toindex,
	int array,
	char *data
);
#endif

#ifdef _SKILL_REFRESH
//vincent add 2002/08/08
int PETSKILL_Refresh(
	int charaindex,
	int toindex,
	int array,
	char *data
);
#endif

#ifdef _SKILL_WEAKEN  //vincent³è¼¼:ÐéÈõ
int PETSKILL_Weaken(
	int charaindex,
	int toindex,
	int array,
	char *data
);
#endif

#ifdef _SKILL_DEEPPOISON  //vincent³è¼¼:¾ç¶¾ 
int PETSKILL_Deeppoison(
	int charaindex,
	int toindex,
	int array,
	char *data
);
#endif

#ifdef _SKILL_BARRIER  //vincent³è¼¼:Ä§ÕÏ
int PETSKILL_Barrier(
	int charaindex,
	int toindex,
	int array,
	char *data
);
#endif

#ifdef _SKILL_ROAR //vincent³è¼¼:´óºð
int PETSKILL_Roar(
	int charaindex,
	int toindex,
	int array,
	char *data
);
#endif

#ifdef _PSKILL_FALLGROUND
int PETSKILL_FallGround( int charaindex, int toNo, int array, char *data );
#endif
#ifdef _PRO_BATTLEENEMYSKILL
int ENEMYSKILL_ReLife( int enemyindex, int toNo, int array, char *data );
int ENEMYSKILL_ReHP( int enemyindex, int toNo, int array, char *data );

int ENEMYSKILL_EnemyHelp( int enemyindex, int toNo, int array, char *data );
#endif

#ifdef _SKILL_DAMAGETOHP
int PETSKILL_DamageToHp( int charaindex, int toNo, int array, char *data );
#endif

#ifdef _PETSKILL_TIMID
int PETSKILL_BattleTimid( int charaindex, int toNo, int array, char *data );
#endif
#ifdef _PETSKILL_2TIMID
int PETSKILL_2BattleTimid( int charaindex, int toNo, int array, char *data );
#endif
#ifdef _PETSKILL_ANTINTER
int PETSKILL_AntInter(int charaindex, int toindex, int array, char* data);		// ³èÎï¼¼ÄÜÕ½¶·Ä£×é
#endif

#ifdef _PETSKILL_PROPERTY
int PETSKILL_BattleProperty( int charaindex, int toNo, int array, char *data );
#endif

#ifdef _PETSKILL_TEAR
int PETSKILL_BattleTearDamage( int charaindex, int toNo, int array, char *data );
#endif

#ifdef _BATTLE_LIGHTTAKE
int PETSKILL_Lighttakeed( int charaindex, int toNo, int array, char *data );
#endif

#ifdef _BATTLE_ATTCRAZED
int PETSKILL_AttackCrazed( int charaindex, int toNo, int array, char *data );
#endif

#ifdef _Skill_MPDAMAGE
int PETSKILL_MpDamage( int charaindex, int toNo, int array, char *data );
#endif

#ifdef _PETSKILL_SETDUCK
int PETSKILL_SetDuck( int charaindex, int toNo, int array, char *data );
#endif

#ifdef _MAGICPET_SKILL
int PETSKILL_SetMagicPet( int charaindex, int toNo, int array, char *data );
#endif

#ifdef _SKILL_TOOTH
int PETSKILL_ToothCrushe( int charaindex, int toNo, int array, char *data );
#endif

#ifdef _PSKILL_MODIFY
int PETSKILL_Modifyattack( int charaindex, int toNo, int array, char *data );
#endif

#ifdef _PSKILL_MDFYATTACK
int PETSKILL_Mdfyattack( int charaindex, int toNo, int array, char *data );
#endif


#ifdef _MAGIC_SUPERWALL
int	PETSKILL_MagicStatusChange( int charaindex, int toindex, int array, char *data );
#endif

#ifdef _PET_SKILL_SARS				// WON ADD ¶¾É·ÂûÑÓ
int PETSKILL_Sars( int charaindex, int toNo, int array, char *data );
#endif

#ifdef _SONIC_ATTACK				// WON ADD Òô²¨¹¥»÷
int PETSKILL_Sonic( int charaindex, int toNo, int array, char *data );
#endif
#ifdef _PETSKILL_REGRET
int PETSKILL_Regret( int charaindex, int toNo, int array, char *data );
#endif

#ifdef _PETSKILL_GYRATE
int PETSKILL_Gyrate( int charaindex, int toNo, int array, char *data );
#endif

#ifdef _PETSKILL_RETRACE
int PETSKILL_Retrace( int charaindex, int toNo, int array, char *data );
#endif

#ifdef _PETSKILL_HECTOR
int PETSKILL_Hector( int charaindex, int toNo, int array, char *data );
#endif

#ifdef _PETSKILL_FIREKILL
int PETSKILL_Firekill( int charaindex, int toNo, int array, char *data );
#endif

#ifdef _PETSKILL_DAMAGETOHP   //°µÔÂ¿ñÀÇ(ÊÈÑª¼¼µÄ±äÌå) 
int PETSKILL_DamageToHp2( int charaindex, int toNo, int array, char *data );
#endif

#ifdef _PETSKILL_BECOMEFOX
int PETSKILL_BecomeFox( int charaindex, int toNo, int array, char* data);
#endif

#ifdef _PETSKILL_BECOMEPIG
int PETSKILL_BecomePig( int charaindex, int toNo, int array, char* data);
#endif

#ifdef _PETSKILL_SHOWMERCY
int PETSKILL_ShowMercy(int charaindex, int toNo, int array, char* data);
#endif

#ifdef _PETSKILL_COMBINED
int PETSKILL_Combined(int charaindex, int toNo, int array, char* data);
#endif

#ifdef _PETSKILL_BATTLE_MODEL
int PETSKILL_BattleModel(int charaindex, int toindex, int array, char* data);		// ³èÎï¼¼ÄÜÕ½¶·Ä£×é
#endif

#endif
