#ifndef __BATTLE_EVENT_H__
#define __BATTLE_EVENT_H__

#define BCC_HIT		'H'
#define BCC_FIRE	'F'

#define BCF_DEATH		(1 << 0)
#define BCF_NORMAL		(1 << 1)
#define BCF_KAISHIN		(1 << 2)
#define BCF_GUARD		(1 << 3)
#define BCF_COUNTER		(1 << 4)
#define BCF_DODGE		(1 << 5)
#define BCF_ULTIMATE_1	(1 << 6)
#define BCF_ULTIMATE_2	(1 << 7)
#define BCF_GBREAK		(1 << 8)
#define BCF_GUARDIAN	(1 << 9)
#define BCF_REFRECT		(1 << 10)
#define BCF_ABSORB		(1 << 11)
#define BCF_VANISH		(1 << 12)
#define BCF_CRUSH		(1 << 13)
#define BCF_FALL		(1 << 14)	//ÂäÂíÊõ
#ifdef _SKILL_TOOTH
#define BCF_TOOTH		(1 << 15)	//‡Ü³Ý
#endif
#ifdef _PSKILL_MODIFY
#define BCF_ATTDOUBLE	(1 << 16)	//ÊôÐÔÇ¿»¯
#endif
#ifdef _SKILL_ROAR  
#define BCF_ROAR		(1 << 17)	//³è¼¼:´óºð(¿ËÄêÊÞ)
#endif 

#ifdef _MAGIC_DEFMAGICATT
#define BCF_DEFMAGICATT	(1 << 19)
#endif
#ifdef  _MAGIC_SUPERWALL
#define BCF_SUPERWALL	(1 << 20)
#endif
#ifdef _PSKILL_MDFYATTACK
#define BCF_MODIFY		(1 << 21)
#endif

#define BCF_B_SKILLACT	(1 << 23)	//»÷ÖÐááÐãÍ¼

#ifdef _PETSKILL_ANTINTER
#define BCF_ANTINTER    (1 << 28)
#endif

#ifdef _OTHER_MAGICSTAUTS
#ifdef _MAGICSTAUTS_RESIST
#define MAXSTATUSTYPE 6
#else
#define MAXSTATUSTYPE 3
#endif //_MAGICSTAUTS_RESIST
extern char MagicStatus[MAXSTATUSTYPE][36];
extern int MagicTbl[];
extern int MagicTypeTbl[];
#endif

extern char *aszStatus[];
extern char *aszStatusFull[];
extern int StatusTbl[];
extern int RegTbl[];
extern int MagicDefTbl[];
extern char *aszMagicDefFull[];
extern char *aszMagicDef[];

extern char *aszParamChange[];
extern char *aszParamChangeFull[];
extern int aParamChangeTbl[];

extern float gBattleDamageModyfy;
extern int   gBattleDuckModyfy;
extern int	 gBattleStausChange;
extern int	 gBattleStausTurn;

enum{
	BATTLE_ST_NONE,		 //  0 "Õý³£",
	BATTLE_ST_POISON,	 //  1 "¶¾",
	BATTLE_ST_PARALYSIS, //  2 "Âé",
	BATTLE_ST_SLEEP,	 //  3 "Ãß",
	BATTLE_ST_STONE,	 //  4 "Ê¯",
	BATTLE_ST_DRUNK,	 //  5 "×í",
	BATTLE_ST_CONFUSION, //  6 "ÂÒ",
    BATTLE_ST_WEAKEN,    //  7 "Ðé"
	BATTLE_ST_DEEPPOISON,//  8 "¾ç¶¾"
	BATTLE_ST_BARRIER,   //  9 "Ä§ÕÏ"
	BATTLE_ST_NOCAST,    // 10 "³ÁÄ¬"
#ifdef _PET_SKILL_SARS				// WON ADD ¶¾É·ÂûÑÓ
	BATTLE_ST_SARS,		 // 11 "É·"
#endif

	BATTLE_ST_END
};

#ifdef _OTHER_MAGICSTAUTS
enum{
	BATTLE_MST_NONE,
	BATTLE_MST_DEFMAGIC,
	BATTLE_MST_SUPERWALL,
#ifdef _MAGICSTAUTS_RESIST
	BATTLE_MST_MAGICFIRE, 
	BATTLE_MST_MAGICTHUNDER, 
	BATTLE_MST_MAGICICE,
#endif
	BATTLE_MST_END
};
#endif

enum{
	BATTLE_MD_NONE,
	BATTLE_MD_ABSROB,
	BATTLE_MD_REFLEC,
	BATTLE_MD_VANISH,
	BATTLE_MD_TRAP,
	BATTLE_MD_END 
};

#ifdef _PETSKILL_RETRACE
    //´æ·ÅBATTLE_Attackº¯Ê½Ö´ÐÐááµÄ¹¥»÷Ä£Ê½   
typedef struct tagBattle_Attack_ReturnData_x1
{
    int Battle_Attack_ReturnData;
}Battle_Attack_ReturnData_x1;
Battle_Attack_ReturnData_x1 Battle_Attack_ReturnData_x;

#endif

int BATTLE_DamageCalc( int attackindex, int defindex );

BOOL BATTLE_Attack( int battleindex, int attackNo, int defNo );

BOOL BATTLE_Counter( int battleindex, int attackNo, int defNo );

BOOL BATTLE_Capture( int battleindex, int attackNo, int defNo );

void BATTLE_Guard( int battleindex, int attackNo );

// Nuke 20040112 fix for performance
#define BATTLE_BroadCast(A,B,C) 0
#define BATTLE_BroadCastOn(A,B,C) 0
//int  BATTLE_BroadCast( int battleindex, char *pszBuffer, int color );
//int  BATTLE_BroadCastOn( int battleindex, char *pszBuffer, int color );

BOOL BATTLE_Escape( int battleindex, int attackNo, int flag);

int BATTLE_NoAction( int battleindex, int attackNo );

int BATTLE_PetIn( int battleindex, int attackNo );

int BATTLE_PetOut( int battleindex, int attackNo );

int BATTLE_SurpriseCheck( int battleindex );

int BATTLE_Magic( int battleindex, int attackNo );

int BATTLE_S_GBreak( int battleindex, int attackNo, int defNo );

#ifdef _SKILL_GUARDBREAK2//ÆÆ³ý·ÀÓù2 vincent add 2002/05/20
int BATTLE_S_GBreak2( int battleindex, int attackNo, int defNo );
#endif

#ifdef _SKILL_SACRIFICE
int BATTLE_S_Sacrifice( int battleindex, int attackNo, int defNo );
#endif

#ifdef _SKILL_REFRESH
int BATTLE_S_Refresh( int battleindex, int attackNo, int defNo, int marray );
#endif

#ifdef _SKILL_WEAKEN  //vincent³è¼¼:ÐéÈõ
int BATTLE_S_Weaken( int battleindex, int attackNo, int defNo, int marray );
#endif

#ifdef _SKILL_DEEPPOISON  //vincent³è¼¼:¾ç¶¾ 
int BATTLE_S_Deeppoison( int battleindex, int attackNo, int defNo, int marray );
#endif

#ifdef _SKILL_BARRIER  //vincent³è¼¼:Ä§ÕÏ
int BATTLE_S_Barrier( int battleindex, int attackNo, int defNo, int marray );
#endif

#ifdef _SKILL_ROAR //vincent³è¼¼:´óºð
int BATTLE_S_Roar( int battleindex, int attackNo, int defNo, int marray );
#endif

int BATTLE_Charge( int battleindex, int attackNo );


int BATTLE_StatusAttackCheck(

	int attackindex,
	int defindex,
	int status,
	//BATTLE_ST_POISON,	    1 "¶¾",
	//BATTLE_ST_PARALYSIS,  2 "Âé",
	//BATTLE_ST_SLEEP,	    3 "Ãß",
	//BATTLE_ST_STONE,	    4 "Ê¯",
	//BATTLE_ST_DRUNK,	    5 "×í",
	//BATTLE_ST_CONFUSION,  6 "ÂÒ"
	int PerOffset,
	int Range,
	float Bai,
	int *pPer
);

int BATTLE_Combo( int battleindex, int *pAttackList, int defNo );

int BATTLE_EarthRoundHide( int battleindex, int attackNo );

int BATTLE_GetDamageReact( int charaindex );

void  BATTLE_talkToCli( int charaindex, char *pszBuffer, int color );

#ifdef _PETSKILL_SETDUCK
BOOL BATTLE_CheckMySkillDuck( int charaindex );
#endif

BOOL BATTLE_LostEscape( int battleindex, int attackNo );

BOOL BATTLE_Abduct( int battleindex, int attackNo, int defNo, int array );

void BATTLE_Steal( int battleindex, int attackNo, int defNo );

int BATTLE_getReactFlg( int index, int react);

#ifdef _BATTLESTEAL_FIX
void BATTLE_StealMoney( int battleindex, int attackNo, int defNo) ;
#endif

#ifdef _PETSKILL_BATTLE_MODEL
void BATTLE_BattleModel(int battleindex,int attackNo,int myside);
#endif

// Robin 0727 Ride Pet
#define	ATTACKSIDE	1
#define DEFFENCESIDE	2

float BATTLE_adjustRidePet3A( int charaindex, int petindex, int workindex, int action );

int BATTLE_getRidePet( int charaindex );

#ifdef _PSKILL_FALLGROUND	//ÂäÂíÊõ
int BATTLE_S_FallGround( int battleindex, int attackNo, int defNo, int skill_type );
#endif

#ifdef _PETSKILL_PROPERTY
int BATTLE_S_PetSkillProperty( int battleindex, int attackNo, int skill_type, int skill);
#endif

int BATTLE_S_AttackDamage( int battleindex, int attackNo, int defNo, int skill_type, int skill);

#ifdef _MAGIC_SUPERWALL
int	PETSKILL_MagicStatusChange_Battle( int battleindex, int attackNo, int toNo, int marray);
#endif

#ifdef _PETSKILL_SETDUCK
int	PETSKILL_SetDuckChange_Battle( int battleindex, int attackNo, int toNo, int marray);
#endif

#ifdef _MAGICPET_SKILL
int PETSKILL_SetMagicPet_Battle( int battleindex, int attackNo, int toNo, int marray);
#endif

#ifdef _TAKE_ITEMDAMAGE
int BATTLE_ItemCrushCheck( int charaindex , int flg);
int BATTLE_ItemCrush( int charaindex, int ItemEquip, int Damages, int flg);
#endif

#ifdef _PRO_BATTLEENEMYSKILL
int BATTLE_E_ENEMYREFILE( int battleindex, int attackNo, int defNo, int skill_type );

int BATTLE_E_ENEMYREHP( int battleindex, int attackNo, int defNo, int skill_type );

int BATTLE_E_ENEMYHELP( int battleindex, int attackNo, int defNo, int skill_type );
#endif

#ifdef _USER_CHARLOOPS
int CHAR_BattleStayLoop( int charaindex);//Ô­µØÓöµÐ
#endif

#ifdef _PETSKILL_PROPERTY
int PET_PetskillPropertyEvent( int Myindex, int defindex, int *damage, int *T_Pow, int size);
#endif

#ifdef _PETSKILL_FIREKILL //»ðÏßÁÔÉ±×¨ÓÃ
int BATTLE_Attack_FIREKILL( int battleindex, int attackNo, int defNo );
#endif
#ifdef _NEED_ITEM_ENEMY
int need_item_eneny_init();
#endif
#endif
