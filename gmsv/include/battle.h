#ifndef __BATTLE_H__
#define __BATTLE_H__
#include "net.h"

#define	BATTLE_ENTRY_MAX 10
#define	BATTLE_PLAYER_MAX 5
#define SIDE_OFFSET 	10

#define BATTLE_STRING_MAX	4096
#define BATTLE_TIME_LIMIT	(60*60)


#define DUELPOINT_RATE	(0.1)

enum{
	BATTLE_MODE_NONE = 0,	// ÎÞÕ½¶·×´Ì¬
	BATTLE_MODE_INIT,		// Õ½¶·³õÊ¼»¯
	BATTLE_MODE_BATTLE,		// Õ½¶·ÖÐ
	BATTLE_MODE_FINISH,		// Õ½¶·½áÊø
	BATTLE_MODE_STOP,		// Õ½¶·½áÊø(Î´Ê¹ÓÃ)
	BATTLE_MODE_WATCHBC,	// ¹ÛÕ½³õÊ¼»¯
	BATTLE_MODE_WATCHPRE,	// ¹ÛÕ½(Ã»×÷ÓÃ)
	BATTLE_MODE_WATCHWAIT,	// ¹ÛÕ½(Ã»×÷ÓÃ)
	BATTLE_MODE_WATCHMOVIE, // ¹ÛÕ½(Ã»×÷ÓÃ)
	BATTLE_MODE_WATCHAFTER, // ¹ÛÕ½(Ã»×÷ÓÃ)
	BATTLE_MODE_END
};


enum{
	BATTLE_TYPE_NONE = 0,
	BATTLE_TYPE_P_vs_E = 1,
	BATTLE_TYPE_P_vs_P = 2,
	BATTLE_TYPE_E_vs_E = 3,
	BATTLE_TYPE_WATCH = 4,
	BATTLE_TYPE_DP_BATTLE = 5,
	BATTLE_TYPE_BOSS_BATTLE = 6,
	BATTLE_TYPE_END
};


enum{
	BATTLE_CHARMODE_NONE = 0,		// ÎÞÕ½¶·×´Ì¬
	BATTLE_CHARMODE_INIT,			// Õ½¶·³õÊ¼»¯
	BATTLE_CHARMODE_C_WAIT,			// µÈ´ýÕ½¶·Ö¸Áî
	BATTLE_CHARMODE_C_OK,			// ÒÑÊäÈëÕ½¶·Ö¸Áî
	BATTLE_CHARMODE_BATTLE,			// Î´Ê¹ÓÃ
	BATTLE_CHARMODE_RESCUE,			// ÓÉhelp×´Ì¬½øÈëµÄ
	BATTLE_CHARMODE_FINAL,			// Õ½¶·½áÊø
	BATTLE_CHARMODE_WATCHINIT,		// ¹ÛÕ½³õÊ¼»¯
	BATTLE_CHARMODE_COMMAND,		// Î´Ê¹ÓÃ
	BATTLE_CHARMODE_END
};




typedef enum{
	BATTLE_ERR_NONE = 0,
	BATTLE_ERR_NOTASK,
	BATTLE_ERR_NOUSE,
	BATTLE_ERR_PARAM,
	BATTLE_ERR_ENTRYMAX,
	BATTLE_ERR_TYPE,
	BATTLE_ERR_CHARAINDEX,
	BATTLE_ERR_BATTLEINDEX,
	BATTLE_ERR_NOENEMY,
	BATTLE_ERR_ALREADYBATTLE,
	BATTLE_ERR_SAMEPARTY,
	BATTLE_ERR_END
}BATTLE_ERR;


enum{
	BATTLE_S_TYPE_PLAYER = 0,
	BATTLE_S_TYPE_ENEMY,
	BATTLE_S_TYPE_END
};


typedef enum{
	BATTLE_COM_NONE,
	BATTLE_COM_ATTACK,
	BATTLE_COM_GUARD,
	BATTLE_COM_CAPTURE,
	BATTLE_COM_ESCAPE,
	BATTLE_COM_PETIN,
	BATTLE_COM_PETOUT,
	BATTLE_COM_ITEM,
	BATTLE_COM_BOOMERANG,
	BATTLE_COM_COMBO,
	BATTLE_COM_COMBOEND,
	BATTLE_COM_WAIT,

	BATTLE_COM_SEKIBAN = 1000,
	BATTLE_COM_S_RENZOKU,
	BATTLE_COM_S_GBREAK,
	BATTLE_COM_S_GUARDIAN_ATTACK,
	BATTLE_COM_S_GUARDIAN_GUARD,
	BATTLE_COM_S_CHARGE,
	BATTLE_COM_S_MIGHTY,
	BATTLE_COM_S_POWERBALANCE,
	BATTLE_COM_S_STATUSCHANGE,
	BATTLE_COM_S_EARTHROUND0,
	BATTLE_COM_S_EARTHROUND1,
	BATTLE_COM_S_LOSTESCAPE,
	BATTLE_COM_S_ABDUCT,
	BATTLE_COM_S_STEAL,
	BATTLE_COM_S_NOGUARD,
	BATTLE_COM_S_CHARGE_OK,
	BATTLE_COM_JYUJYUTU = 2000,

	BATTLE_COM_COMPELESCAPE, //Ç¿ÖÆÀë¿ª

#ifdef _ATTACK_MAGIC
  BATTLE_COM_S_ATTACK_MAGIC,		// ³èÎïÄ§·¨
#endif

#ifdef _PSKILL_FALLGROUND
	BATTLE_COM_S_FALLRIDE,			//ÂäÂíÊõ
#endif
#ifdef _PETSKILL_TIMID
	BATTLE_COM_S_TIMID,
#endif
#ifdef _PETSKILL_2TIMID
	BATTLE_COM_S_2TIMID,
#endif
#ifdef _PETSKILL_ANTINTER
	BATTLE_COM_S_ANTINTER,
#endif
#ifdef _PETSKILL_PROPERTY
	BATTLE_COM_S_PROPERTYSKILL,
#endif
#ifdef _PETSKILL_TEAR
	BATTLE_COM_S_PETSKILLTEAR,
#endif
#ifdef _BATTLE_LIGHTTAKE
	BATTLE_COM_S_LIGHTTAKE,
#endif
#ifdef _BATTLE_ATTCRAZED		// ANDY ·è¿ñ±©×ß
	BATTLE_COM_S_ATTCRAZED,
#endif
#ifdef _BATTLESTEAL_FIX
	BATTLE_COM_S_STEALMONEY,
#endif
#ifdef _PRO_BATTLEENEMYSKILL
	BATTLE_COM_S_ENEMYRELIFE,		//NPC ENEMY ¸´»î¼¼ÄÜ
	BATTLE_COM_S_ENEMYREHP,			//NPC ENEMY ²¹Ñª¼¼ÄÜ
	BATTLE_COM_S_ENEMYHELP,			//NPC ENEMY ÕÐÈË
#endif
#ifdef _SKILL_DAMAGETOHP
	BATTLE_COM_S_DAMAGETOHP,		//ÊÈÑª¼¼
#endif
#ifdef _Skill_MPDAMAGE
	BATTLE_COM_S_MPDAMAGE,			//MPÉËº¦
#endif
#ifdef _SKILL_WILDVIOLENT_ATT
    BATTLE_COM_S_WILDVIOLENTATTACK, //¿ñ±©¹¥»÷  vincent add 2002/05/16
#endif

#ifdef _SKILL_SPEEDY_ATT
    BATTLE_COM_S_SPEEDYATTACK,		//¼²ËÙ¹¥»÷  vincent add 2002/05/20
#endif
#ifdef _SKILL_GUARDBREAK2
    BATTLE_COM_S_GBREAK2,			//ÆÆ³ý·ÀÓù2 vincent add 2002/05/20
#endif
#ifdef _SKILL_SACRIFICE
    BATTLE_COM_S_SACRIFICE,			//¾ÈÔ®		vincent add 2002/05/30
#endif
#ifdef _SKILL_WEAKEN  
	    BATTLE_COM_S_WEAKEN,		//ÐéÈõ		vincent add 2002/07/11
#endif
#ifdef _SKILL_DEEPPOISON  
	    BATTLE_COM_S_DEEPPOISON,	//¾ç¶¾		vincent add 2002/07/16
#endif
#ifdef _SKILL_BARRIER  
	    BATTLE_COM_S_BARRIER,		//Ä§ÕÏ		vincent add 2002/07/16
#endif

#ifdef _SKILL_ROAR 
    BATTLE_COM_S_ROAR,			    //´óºð		vincent add 2002/07/11
#endif
#ifdef _BATTLENPC_WARP_PLAYER
	BATTLE_COM_WARP,                // npc warp player
#endif
#ifdef _SKILL_TOOTH
	BATTLE_COM_S_TOOTHCRUSHE,
#endif
#ifdef _PSKILL_MODIFY
	BATTLE_COM_S_MODIFYATT,
#endif
#ifdef _PSKILL_MDFYATTACK
	BATTLE_COM_S_MDFYATTACK,
#endif
#ifdef _MAGIC_SUPERWALL
	BATTLE_COM_S_SUPERWALL,
#endif
#ifdef _SKILL_REFRESH
    BATTLE_COM_S_REFRESH,
#endif
#ifdef _VARY_WOLF
	BATTLE_COM_S_VARY,
#endif
#ifdef _PETSKILL_SETDUCK
	BATTLE_COM_S_SETDUCK,
#endif
#ifdef _MAGICPET_SKILL
	BATTLE_COM_S_SETMAGICPET,
#endif
#ifdef _PET_SKILL_SARS				// WON ADD ¶¾É·ÂûÑÓ
	BATTLE_COM_S_SARS,
#endif
#ifdef _SONIC_ATTACK				// WON ADD Òô²¨¹¥»÷
	BATTLE_COM_S_SONIC,
	BATTLE_COM_S_SONIC2,
#endif
#ifdef _PETSKILL_REGRET
	BATTLE_COM_S_REGRET,
	BATTLE_COM_S_REGRET2,
#endif
#ifdef _PETSKILL_GYRATE
    BATTLE_COM_S_GYRATE,
#endif
#ifdef _PETSKILL_RETRACE
	BATTLE_COM_S_RETRACE,
#endif
#ifdef _PETSKILL_HECTOR
	BATTLE_COM_S_HECTOR,
#endif
#ifdef _PETSKILL_FIREKILL
    BATTLE_COM_S_FIREKILL,
#endif
#ifdef _PETSKILL_DAMAGETOHP
	BATTLE_COM_S_DAMAGETOHP2, //°µÔÂ¿ñÀÇ(ÊÈÑª¼¼µÄ±äÌå) 
#endif
#ifdef _PETSKILL_BECOMEFOX
    BATTLE_COM_S_BECOMEFOX,
#endif
#ifdef _PETSKILL_BECOMEPIG
    BATTLE_COM_S_BECOMEPIG,
#endif
#ifdef _PETSKILL_SHOWMERCY
    BATTLE_COM_S_SHOWMERCY,
#endif
#ifdef _PETSKILL_BATTLE_MODEL
		BATTLE_COM_S_BATTLE_MODEL,		// ³èÎï¼¼ÄÜÕ½¶·Ä£×é
#endif

	BATTLE_COM_END
}BATTLE_COM;


enum{
	BATTLE_RET_NORMAL,
	BATTLE_RET_CRITICAL,
	BATTLE_RET_MISS,
	BATTLE_RET_DODGE,
	BATTLE_RET_ALLGUARD,
	BATTLE_RET_END
}BATTLE_RET;


#define BC_FLG_NEW			(1<<0)
#define BC_FLG_DEAD			(1<<1)
#define BC_FLG_PLAYER		(1<<2)
#define BC_FLG_POISON		(1<<3)
#define BC_FLG_PARALYSIS	(1<<4)
#define BC_FLG_SLEEP		(1<<5)
#define BC_FLG_STONE		(1<<6)
#define BC_FLG_DRUNK		(1<<7)
#define BC_FLG_CONFUSION	(1<<8)
#define BC_FLG_HIDE			(1<<9)
#define BC_FLG_REVERSE		(1<<10)
#ifdef _MAGIC_WEAKEN
#define BC_FLG_WEAKEN		(1<<11) // ÐéÈõ
#endif
#ifdef _MAGIC_DEEPPOISON
#define BC_FLG_DEEPPOISON	(1<<12) // ¾ç¶¾
#endif
#ifdef _MAGIC_BARRIER
#define BC_FLG_BARRIER		(1<<13) // Ä§ÕÏ
#endif
#ifdef _MAGIC_NOCAST
#define BC_FLG_NOCAST		(1<<14) // ³ÁÄ¬
#endif

#ifdef _PET_SKILL_SARS			// WON ADD ¶¾É·ÂûÑÓ
#define BC_FLG_SARS			(1<<15) // ¶¾É·
#endif

#define BP_FLG_JOIN					(1<<0)
#define BP_FLG_PLAYER_MENU_OFF		(1<<1)
#define BP_FLG_BOOMERANG			(1<<2)
#define BP_FLG_PET_MENU_OFF			(1<<3)
#define BP_FLG_ENEMY_SURPRISAL		(1<<4)
#define BP_FLG_PLAYER_SURPRISAL		(1<<5)


#define CHAR_BATTLEFLG_ULTIMATE		(1<<0)
#define CHAR_BATTLEFLG_AIBAD		(1<<1)
#define CHAR_BATTLEFLG_REVERSE		(1<<2)
#define CHAR_BATTLEFLG_GUARDIAN		(1<<3)
#define CHAR_BATTLEFLG_NORETURN		(1<<4)
#define CHAR_BATTLEFLG_RECOVERY		(1<<5)
#define CHAR_BATTLEFLG_ABIO			(1<<6)
#define CHAR_BATTLEFLG_NODUCK		(1<<7)

#define GETITEM_MAX	3
typedef struct
{
	int attacker;
}INVADER;

typedef struct
{
	int		charaindex;
	int		bid;
	int		escape;
	int		flg;
	int		guardian;
	int		duelpoint;
	int		getitem[GETITEM_MAX];
}BATTLE_ENTRY;
#define BENT_FLG_ULTIMATE	(1<<0)

typedef struct
{
	int		type;
	int		flg;
	int		common_dp;
	BATTLE_ENTRY	Entry[BATTLE_ENTRY_MAX];
}BATTLE_SIDE;

#define BSIDE_FLG_SURPRISE ( 1 << 0 )
#define BSIDE_FLG_HELP_OK ( 1 << 1 )



typedef struct _Battle
{
	BOOL	use;	/* ÒøÔÈ»¯ÖÐÔÂ¾®Éýµ¤¾® */
	int		battleindex; /* ÌïÐþ»ï  Ä¯ */
	int		mode;	/* òØ»þ¼°¾ô  Æ¹¡õÓñ */
	int		type;	/* ¾ô  ÕýÄÌÃó (0:É§éÙ)(1:DUEL)(2:Ê¾µ©¾ô) */
	int		dpbattle;	/* DPÌïÐþ»ï¾®£¢ */
	int		norisk;	/* ÉØÊÏÆ¥ÊÖØøµ©ÛÍ¼°  ÖÐÌïÐþ»ï¾®£¢ */
	int		turn;	/* Õý¡õ¼þÐÑ */
	int		timer;	/* Ëü¾ÞÄÌÐþî¿±åÒøµ¤ÕýÄÌÑ¨ */
	int		leaderindex; /* ¾Þ¼þÊÐËü¼þÐþÃ«ËÚ³ðØÆÐ×Æ½ÅÒ·Â¼°ÄÌ¼þ·¸ÓÀÛÍµ© */
#ifdef _AUTO_PK
	char  leadercdkey[CDKEYLEN];
	char  leadername[CHARNAMELEN];
	int		rivalindex;
	char  rivalcdkey[CDKEYLEN];
	char  rivalname[CHARNAMELEN];
#endif
	int		winside; /*   ØÇØÆÐ×ÈÓÄÌÓñ */
	int		field_att;	/* °×Å«¡õ»ïÓñ¼°óìÁë */
	int		att_count;	/* °×Å«¡õ»ïÓñ¼°óìÁë  ¼À  ¼°ÕýÄÌÑ¨ */
	int		att_pow;	/* °×Å«¡õ»ïÓñ¼°óìÁë  ¼À  ¼°ÓÉ·¥¡õ */
	int		field_no;
	int		flg;		/* ±¸ÅË°×·Âºë */
	BATTLE_SIDE		Side[2];
#ifdef _BATTLE_TIMESPEED
	unsigned int CreateTime;
	unsigned int EndTime;
	unsigned int PartTime;
	int flgTime;
	
#endif
	int		iEntryBack[BATTLE_ENTRY_MAX*2];		// ó¡Õý¡õ¼þè¶  ½ñÄ¾»¯ÖÐÐ×¶ª¼þÌï¡õ
	int		iEntryBack2[BATTLE_ENTRY_MAX*2];		// ó¡Õý¡õ¼þè¶  ½ñÄ¾»¯ÖÐÐ×¶ª¼þÌï¡õ
	int		createindex;	/* ³ð¼°¾ô  Ã«×ÛÔ»ÇëØÆÐ×Æ½ÅÒ·ÂÄÌ¼þ·¸ÓÀÛÍµ©(NPCØ¦Éý) */
	int    (*WinFunc)( int battleindex, int charaindex );
	struct _Battle *pNext;		// ÆÝ¼°Æå¾ôºë»ï¡õÃó
	struct _Battle *pBefore;	// ó¡¼°Æå¾ôºë»ï¡õÃó
}BATTLE;

#define BATTLE_FLG_FREEDP ( 1 << 0 )	//   ÆÍ»¯ÊÖ    Ó¼ÈÕØ¦ÖÐ
#define BATTLE_FLG_CHARALOST   ( 1 << 1 )	// Æ½ÅÒ·Â»¥ÉØÊÏ·ÖÔ»  ØêÐ×Ô»ØÆ»¯ÖÐÔÂ



enum{	// Ïþ¼°field_att ±å  Ä¾ÔÂ°À
	BATTLE_ATTR_NONE = 0,	//   óìÁë
	BATTLE_ATTR_EARTH,		// »©
	BATTLE_ATTR_WATER,		//   
	BATTLE_ATTR_FIRE,		// ÉÜ
	BATTLE_ATTR_WIND,		// ë®
	BATTLE_ATTR_END
};


//krynn 2001/12/28
enum
{
	BATTLE_SIDE_RIGHT,		// Õ½¶·ÖÐÓÒ²à	right side of battle
	BATTLE_SIDE_LEFT,		// Õ½¶·ÖÐ×ó²à	left side of battle
	BATTLE_SIDE_WATCH,		// ¹ÛÕ½			watch battle player
};
//krynn end

#define TARGET_SIDE_0	20	// ã¯´¡  ÈÓÄÌÓñ¨ß  Ï¶ÀÃ
#define TARGET_SIDE_1	21	// ¶û´¡  ÈÓÄÌÓñ¨à  Ï¶ÀÃ
#define TARGET_ALL	22	// òå  Ï¶ÀÃ

#ifdef _ATTACK_MAGIC

#define TARGET_SIDE_0_B_ROW     26      // ÓÒÏÂááÒ»ÁÐ
#define TARGET_SIDE_0_F_ROW     25      // ÓÒÏÂÇ°Ò»ÁÐ
#define TARGET_SIDE_1_F_ROW     24      // ×óÉÏÇ°Ò»ÁÐ
#define TARGET_SIDE_1_B_ROW     23      // ×óÉÏááÒ»ÁÐ

// won add
#define	TARGER_THROUGH			27

#endif

extern int	gItemCrushRate;		//   Ã§  ·¸°×ÇÉ»ïÐþ
extern BATTLE *BattleArray; 	/* ¾ô  Õýµ©ÛÍ     */
extern int BATTLE_battlenum;	/*     ¼°ÐÑ */
extern char szAllBattleString[BATTLE_STRING_MAX];	/* ¾ô  ±åÒøµ¤ÎìÑ¨¼þÓñ  Ù¯   */
extern char *pszBattleTop, *pszBattleLast;	/* ¾ô  ±åÒøµ¤  Ù¯  ¼°ÞË   */
extern char szBadStatusString[];	// µ©  ¡õÕýµ©³ªéÙåÃ  Ù¯  
extern int gWeponType;	// òØ»þ¼°  Õ¿¼°ÅË  
extern float gDamageDiv;	// Ä¸¶ª¡õ³âÛÐà«

#define BATTLE_CHECKINDEX( a ) ( ((a)>=BATTLE_battlenum || (a)<0 )?(FALSE):(TRUE) )
#define BATTLE_CHECKSIDE( a ) ( ((a)>=2 || (a)<0)?(FALSE):( TRUE) )
#define BATTLE_CHECKNO( a ) ( ((a)>=20 || (a)<0 )?(FALSE):(TRUE) )
#define BATTLE_CHECKADDRESS( a )  ((&BattleArray[0])<=(a) && (a)<=(&BattleArray[BATTLE_battlenum-1] )?(TRUE):(FALSE) ) 

#define IsBATTLING( a ) (CHAR_getWorkInt((a),CHAR_WORKBATTLEMODE)?(TRUE):(FALSE))

#define STRCPY_TAIL( _pszTop, _pszLast, _szBuffer)	{ int _len = strlen( _szBuffer ); ( _pszTop + _len < (_pszLast)-1 )?( memcpy( _pszTop, _szBuffer, _len ),	_pszTop += _len, _pszTop[0] = 0):(0); }

#define BATTLESTR_ADD( _szBuffer ){ int _len = strlen( _szBuffer ); ( pszBattleTop + _len < ( pszBattleLast)-1 )?( memcpy( pszBattleTop, _szBuffer, _len ),	pszBattleTop += _len, pszBattleTop[0] = 0):(0); }

#define BATTLE_MAP_MAX 219

#define CH_FIX_PLAYERLEVELUP	(+2)	// ÃóÒÁÄÌØÀ¡õ¼°ÒÁÃ¬»ï»¥Ê§ÓÀÃó
#define CH_FIX_PLAYERDEAD		(-2)	// ÃóÒÁÄÌØÀ¡õ»¥É§éÙÉØ
#define CH_FIX_PLAYEULTIMATE	(-4)	// ÃóÒÁÄÌØÀ¡õ»¥Ê§»ï  Å«¶ªÓÀÐþÉØ
#define CH_FIX_PETESCAPE		(-1)	// Ê¸ÓÀÐþ»¥  ÆÍÐ×

#define AI_FIX_PETLEVELUP		(+5*100)	// Ê¸ÓÀÐþ»¥ÒÁÃ¬»ïÊ§ÓÀÃó
#define AI_FIX_PETWIN			(+1)		// Ê¸ÓÀÐþ»¥³ÄÃ«åÎØÆÐ×
#define AI_FIX_PETGOLDWIN		(+2*10)	// Ê¸ÓÀÐþ»¥ÒÁÃ¬»ï¼°æÎÖÐ³ÄÃ«åÎØÆÐ×
#define AI_FIX_PETRECOVERY		(+10)		// ¾ô    ±å¼ÔãâØÆ»¯ÊÖÈÕÔÈÐ×
#define AI_FIX_PETRESSURECT		(+3*100)	// ¾ô    ±åãâÍÙØÆ»¯ÊÖÈÕÔÈÐ×
//#define AI_FIX_PETRECOVERY	(+50)		// ¾ô    ±å¼ÔãâØÆ»¯ÊÖÈÕÔÈÐ×

#define AI_FIX_SEKKAN 			(-2*100)	// ·ßÛÐ¼°Ê¸ÓÀÐþÃ«  »«
#define AI_FIX_PLAYERULTIMATE	(-10*100)	// ·ßÛÐ¼°Ç±ÚÐ»¥Ê§»ï  Å«¶ªÓÀÐþÉØ
#define AI_FIX_PETULTIMATE		(-10*100)	// Ê¸ÓÀÐþ»¥Ê§»ï  Å«¶ªÓÀÐþÉØ
#define AI_FIX_PLAYERDEAD		(-1*100)	// ·ßÛÐ¼°Ç±ÚÐ»¥¿¢å§
#define AI_FIX_PETDEAD			(-5*100)	// Ê¸ÓÀÐþ»¥¿¢å§


#ifdef _Item_ReLifeAct
int BATTLE_getBattleDieIndex( int battleindex, int bid );
#endif

int BATTLE_No2Index( int battleindex, int No);

int BATTLE_Index2No( int battleindex, int charaindex);

BOOL BATTLE_initBattleArray( int battlenum);

int BATTLE_CreateBattle( void );
int BATTLE_DeleteBattle( int battleindex);

int BATTLE_NewEntry( int charaindex, int battleindex, int side);

#define BATTLE_Exit( charaindex, battleindex) _BATTLE_Exit( __FILE__, __LINE__, charaindex, battleindex)
INLINE int _BATTLE_Exit( char *file, int line, int charaindex ,int battleindex);

#define BATTLE_ExitAll( battleindex) _BATTLE_ExitAll( __FILE__, __LINE__, battleindex)
INLINE void _BATTLE_ExitAll( char *file, int line, int battleindex);

int BATTLE_CreateVsPlayer( int charaindex0,	int charaindex1);

int BATTLE_CreateVsEnemy( int charaindex, int mode,	int	npcindex);

int BATTLE_CountEntry( int battleindex,	int side);

int BATTLE_Loop( void );

int BATTLE_FinishSet( int battleindex );
int BATTLE_StopSet( int battleindex );
int BATTLE_RescueEntry( int charaindex, int toindex);

int BATTLE_PetDefaultExit( int charaindex, int battleindex);

int	BATTLE_PetDefaultEntry(
	int charaindex,	// Ê¸ÓÀÐþÃ«  ÔÈ»¯ÖÐÔÂÃóÒÁÄÌØÀ¡õ¼°          
	int battleindex,// ÌïÐþ»ïÄÌ¼þ·¸ÓÀÛÍµ©
	int side
);

BOOL BATTLE_RescueTry( int charaindex);

BOOL BATTLE_RescueParentTry(
		int charaindex,
		int pindex
	);

int BATTLE_DefaultAttacker(	int battleindex, int side);

BOOL BATTLE_IsThrowWepon( int itemindex);

void BATTLE_BadStatusString( int defNo, int status );
int BATTLE_MultiList( int battleindex, int toNo, int ToList[] );
BOOL BATTLE_IsCharge( int com );
BOOL BATTLE_CanMoveCheck( int charaindex );
int BATTLE_TargetCheck( int battleindex, int defNo);
char *BATTLE_CharTitle( int charaindex );
void BATTLE_EscapeDpSend( int battleindex, int charaindex );
int BATTLE_GetDuelPoint( int battleindex, int side, int num);
int BATTLE_TargetCheckDead(	int battleindex, int defNo);

void BATTLE_MultiListDead( int battleindex, int toNo, int ToList[] );
BOOL BATTLE_WatchTry( int charaindex);
int BATTLE_WatchEntry( int charaindex, int toindex);
void BATTLE_WatchStop( int charaindex );
int BATTLE_WatchUnLink( int battleindex );
void BATTLE_BpSendToWatch( BATTLE *pBattle,	char *pszBcString);

int BATTLE_GetWepon( int charaindex );

int BATTLE_GetAttackCount( int charaindex );
int DoujyouRandomWeponSet( int charaindex );
void BATTLE_AttReverse( int charaindex );
void BATTLE_BadStatusAllClr( int charaindex );
#define CHAR_GETWORKINT_HIGH( index, pos )	( CHAR_getWorkInt( (index), (pos) ) >> 16 )
#define CHAR_SETWORKINT_HIGH( index, pos, set )	{ int iTmp = CHAR_getWorkInt( (index), (pos) ) & 0xFFFF, work = (set);	CHAR_setWorkInt( (index), (pos), (work << 16)|iTmp ); }
#define CHAR_GETWORKINT_LOW( index, pos )	( CHAR_getWorkInt( (index), (pos) ) & 0xFFFF )
#define CHAR_SETWORKINT_LOW( index, pos, set )	{ int iTmp = CHAR_getWorkInt( index, pos ) & 0xFFFF0000, work = (set);	CHAR_setWorkInt( (index), (pos), (work & 0x0000FFFF) | iTmp ); }

int Battle_getTotalBattleNum();

#ifdef _TYPE_TOXICATION
void CHAR_ComToxicationHp( int charaindex);
#endif
#endif
