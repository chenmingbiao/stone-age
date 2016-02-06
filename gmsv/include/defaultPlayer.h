#include "version.h"

static Char    player=
{
    FALSE,      /*  use     升丹匹手中中*/
    {
		0,		/*  CHAR_DATAPLACENUMBER */
        0,      /*  CHAR_BASEIMAGENUMBER */
        0,      /*  CHAR_BASEBASEIMAGENUMBER */
		0,		/*  CHAR_FACEIMAGENUMBER */
        0,      /*  CHAR_FLOOR  */
        0,      /*  CHAR_X */
        0,      /*  CHAR_Y  */
        5,      /*  CHAR_DIR 12凜毛0卞凜煌璃曰卞 */
        1,      /*  CHAR_LV  */
        0,      /*  CHAR_GOLD    */
        1,      /*  CHAR_HP  */
        0,      /*  CHAR_MP  */

        0,      /*  CHAR_MAXMP   */

    	0,		/*  CHAR_VITAL	*/
        0,      /*  CHAR_STR */
        0,      /*  CHAR_TOUGH */
    	0,		/*  CHAR_DEX	*/

    	0,		/*  CHAR_CHARM	*/
    	0,		/*  CHAR_LUCK	*/
		0,		/* 譁簞嶺 */
		0,		/* 踹簞嶺 */
		0,		/* 紹簞嶺 */
		0,		/* 氘簞嶺 */

		0,		/* CHAR_SLOT*/
		0,		/* CHAR_CRITIAL */
		0,		/* CHAR_COUNTER */
		0,		/* CHAR_RARE */
        0,      /*  CHAR_RADARSTRLENGTH */
        0,      /*  CHAR_CHATVOLUME */
        MAKE2VALUE(100,20),  /*  CHAR_MERCHANTLEVEL  */
        0,      /*  CHAR_HEALERLEVEL    */
        0,      /*  CHAR_DETERMINEITEM  */

        -1,     /*  CHAR_INDEXOFEQTITLE  */


        0,      /*  CHAR_POISON  */
        0,      /*  CHAR_PARALYSIS  */
        0,      /*  CHAR_SILENCE    */
        0,      /*  CHAR_STONE  */
        0,      /*  CHAR_DARKNESS   */
        0,      /*  CHAR_CONFUSION  */

        0,      /*  CHAR_LOGINCOUNT */
        0,      /*  CHAR_DEADCOUNT */
        0,      /*  CHAR_WALKCOUNT */
        0,      /*  CHAR_TALKCOUNT */

    	0,		/*  CHAR_DAMAGECOUNT */
    	0,		/*  CHAR_GETPETCOUNT */
    	0,		/*  CHAR_KILLPETCOUNT */
    	0,		/*  CHAR_DEADPETCOUNT */
    	0,		/*  CHAR_SENDMAILCOUNT */
		0,		/*  CHAR_MERGEITEMCOUNT */

		0,		/*  CHAR_DUELBATTLECOUNT */
		0,		/*  CHAR_DUELWINCOUNT */
		0,		/*  CHAR_DUELLOSECOUNT */
		0,		/*  CHAR_DUELSTWINCOUNT */
		0,		/*  CHAR_DUELMAXSTWINCOUNT */

        CHAR_TYPEPLAYER,    /*  CHAR_WHICHTYPE  */
        1000,   /*  CHAR_WALKINTERVAL   */
        1000,   /*  CHAR_LOOPINTERVAL   */
		0,	// CHAR_OLDEXP,
		0,	// CHAR_EXP,
        -1,     /*  CHAR_LASTTALKELDER*/
        0,      /*  CHAR_SKILLUPPOINT    */
    	0,		/*  CHAR_LEVELUPPOINT */

        0,      /*  CHAR_IMAGETYPE    */
		CHAR_COLORWHITE,	/* CHAR_NAMECOLOR */
		CHAR_COLORWHITE,	/* CHAR_POPUPNAMECOLOR */
		0,		/*  CHAR_LASTTIMESETLUCK */
		100,	/*  CHAR_DUELPOINT */
		0,	/*  CHAR_EVENT */
		0,	/*  CHAR_EVENT2 */
		0,	/*  CHAR_EVENT3 */
#ifdef _NEWEVENT
		0,	/*  CHAR_EVENT4 */
		0,	/*  CHAR_EVENT5 */
		0,	/*  CHAR_EVENT6 */
#endif
		0,	/*  CHAR_NOWEVENT */
		0,	/*  CHAR_NOWEVENT2 */
		0,	/*  CHAR_NOWEVENT3 */
#ifdef _NEWEVENT
		0,	/*  CHAR_NOWEVENT4 */
		0,	/*  CHAR_NOWEVENT5 */
		0,	/*  CHAR_NOWEVENT6 */
#endif
		0,  /* CHAR_TRANSMIGRATION */
		0,  /* CHAR_TRANSEQUATION */

        0,      /*  CHAR_INITDATA    */

//#ifdef _PETSKILL_BECOMEPIG
//        -1,
//#endif

		0,	//CHAR_SILENT,				/* char shutup time */    
		0,	//    CHAR_FMINDEX,				// 家族 index
		0,	//    CHAR_FMLEADERFLAG,			
								/* 家族成員種類
    							 * FMMEMBER_NONE	:沒有加入任何家族
    							 * FMMEMBER_APPLY	:申請加入家族中
    							 * FMMEMBER_LEADER	:族長
    							 * FMMEMBER_MEMBER	:一般成員
    							 * FMMEMBER_ELDER	:長老    
    							 * FMMEMBER_INVITE	:祭司			// 暫時不用
    							 * FMMEMBER_BAILEE	:財務長			// 暫時不用
    							 * FMMEMBER_VICELEADER  :副族長		// 暫時不用
    							*/
		0,	//    CHAR_FMSPRITE,		// 家族守護精靈

		0,	//    CHAR_BANKGOLD,
		0,	//    CHAR_RIDEPET,
		0,	//    CHAR_LEARNRIDE,
 		0,	//   CHAR_LIMITLEVEL,
#ifdef _PERSONAL_FAME	// Arminius 8.30: 家族個人聲望
		0,	//    CHAR_FAME,
#endif

#ifdef _NEWSAVE
		0,	//    CHAR_SAVEINDEXNUMBER,	/*  SaveFile .0.char or .1.char */
#endif
 
#ifdef _ATTACK_MAGIC
		0,	//		CHAR_EARTH_EXP,						// 玩家的地魔法熟練度
		0,	//		CHAR_WATER_EXP,						// 玩家的水魔法熟練度
		0,	//		CHAR_FIRE_EXP,						// 玩家的火魔法熟練度
		0,	//		CHAR_WIND_EXP,						// 玩家的風魔法熟練度
		0,	//		CHAR_EARTH_RESIST,					// 玩家的地魔法抗性
		0,	//		CHAR_WATER_RESIST,					// 玩家的水魔法抗性
		0,	//		CHAR_FIRE_RESIST,					// 玩家的火魔法抗性
		0,	//		CHAR_WIND_RESIST,					// 玩家的風魔法抗性
		0,	//		CHAR_EARTH_ATTMAGIC_EXP,			// 玩家的地魔法熟練度經驗值
		0,	//		CHAR_WATER_ATTMAGIC_EXP,			// 玩家的水魔法熟練度經驗值
		0,	//		CHAR_FIRE_ATTMAGIC_EXP,				// 玩家的火魔法熟練度經驗值
		0,	//		CHAR_WIND_ATTMAGIC_EXP,				// 玩家的風魔法熟練度經驗值
		0,	//	 	CHAR_EARTH_DEFMAGIC_EXP,			// 玩家的地魔法抗性經驗值
		0,	//		CHAR_WATER_DEFMAGIC_EXP,			// 玩家的水魔法抗性經驗值
		0,	//		CHAR_FIRE_DEFMAGIC_EXP,				// 玩家的火魔法抗性經驗值
		0,	//		CHAR_WIND_DEFMAGIC_EXP,				// 玩家的風魔法抗性經驗值
#endif


#ifdef _GAMBLE_BANK
		0,	//		CHAR_PERSONAGOLD,	//賭場個人銀行
#endif
#ifdef _DROPSTAKENEW
		0,	//		CHAR_GAMBLENUM,		//賭場積分
#endif
#ifdef _ADD_ACTION          //npc動作
		0,	//		CHAR_ACTIONSTYLE,
#endif


#ifdef _FM_JOINLIMIT
		0,	//		CHAR_FMTIMELIMIT,
#endif

#ifdef _ALLDOMAN // (不可開) Syu ADD 排行榜NPC
		0,	//		CHAR_HEROFLOOR, 
#endif
#ifdef _PETSKILL_BECOMEPIG
		-1,	//		CHAR_BECOMEPIG,
		100250, //CHAR_BECOMEPIG_BBI
#endif
		0,	//		CHAR_LASTLEAVETIME, // Robin add 最後離線時間

#ifdef _ITEM_ADDEXP2
		0,	//		CHAR_ADDEXPPOWER,
		0,	//		CHAR_ADDEXPTIME,
#endif
#ifdef _GM_ITEM
	  0, // 俙模GM韜鍔棒杅
#endif
#ifdef _VIP_SERVER
	  0, // 頗埜萸杅
#endif
#ifdef _VIP_RIDE
	  0, // 頗埜る窪憐
#endif
    },

    {
        {""},     /*  CHAR_NAME   */
        {""},     /*  CHAR_OWNTITLE   */
        {""},           /*  CHAR_ARGUMENT */
        {""},       /*  CHAR_OWNERCDKEY   */
        {""},       /*  CHAR_OWNERCHARANAME   */
    },
    {
        SETFLG(1,1,1,1,1,1,0,0 ),
        SETFLG(0,0,0,0,0,0,0,1 ),
    }
};

LevelUpPattern lvplayer00={ {{100,10},{200,10},{50,8}},9,11,10};
LevelUpPattern lvplayer10={ {{100,10},{200,10},{50,8}},9,11,10};
LevelUpPattern lvplayer20={ {{100,10},{200,10},{50,8}},9,11,10};
LevelUpPattern lvplayer30={ {{100,10},{200,10},{50,8}},9,11,10};

LevelUpPattern lvplayer01={ {{100,10},{200,10},{50,8}},9,11,10};
LevelUpPattern lvplayer11={ {{100,10},{200,10},{50,8}},9,11,10};
LevelUpPattern lvplayer21={ {{100,10},{200,10},{50,8}},9,11,10};
LevelUpPattern lvplayer31={ {{100,10},{200,10},{50,8}},9,11,10};

LevelUpPattern lvplayer02={ {{100,10},{200,10},{50,8}},9,11,10};
LevelUpPattern lvplayer12={ {{100,10},{200,10},{50,8}},9,11,10};
LevelUpPattern lvplayer22={ {{100,10},{200,10},{50,8}},9,11,10};
LevelUpPattern lvplayer32={ {{100,10},{200,10},{50,8}},9,11,10};

LevelUpPattern lvplayer03={ {{100,10},{200,10},{50,8}},9,11,10};
LevelUpPattern lvplayer13={ {{100,10},{200,10},{50,8}},9,11,10};
LevelUpPattern lvplayer23={ {{100,10},{200,10},{50,8}},9,11,10};
LevelUpPattern lvplayer33={ {{100,10},{200,10},{50,8}},9,11,10};

LevelUpPattern lvplayer04={ {{100,10},{200,10},{50,8}},9,11,10};
LevelUpPattern lvplayer14={ {{100,10},{200,10},{50,8}},9,11,10};
LevelUpPattern lvplayer24={ {{100,10},{200,10},{50,8}},9,11,10};
LevelUpPattern lvplayer34={ {{100,10},{200,10},{50,8}},9,11,10};

LevelUpPattern lvplayer05={ {{100,10},{200,10},{50,8}},9,11,10};
LevelUpPattern lvplayer15={ {{100,10},{200,10},{50,8}},9,11,10};
LevelUpPattern lvplayer25={ {{100,10},{200,10},{50,8}},9,11,10};
LevelUpPattern lvplayer35={ {{100,10},{200,10},{50,8}},9,11,10};

LevelUpPattern lvplayer06={ {{100,10},{200,10},{50,8}},9,11,10};
LevelUpPattern lvplayer16={ {{100,10},{200,10},{50,8}},9,11,10};
LevelUpPattern lvplayer26={ {{100,10},{200,10},{50,8}},9,11,10};
LevelUpPattern lvplayer36={ {{100,10},{200,10},{50,8}},9,11,10};

LevelUpPattern lvplayer07={ {{100,10},{200,10},{50,8}},9,11,10};
LevelUpPattern lvplayer17={ {{100,10},{200,10},{50,8}},9,11,10};
LevelUpPattern lvplayer27={ {{100,10},{200,10},{50,8}},9,11,10};
LevelUpPattern lvplayer37={ {{100,10},{200,10},{50,8}},9,11,10};

LevelUpPattern lvplayer08={ {{100,10},{200,10},{50,8}},9,11,10};
LevelUpPattern lvplayer18={ {{100,10},{200,10},{50,8}},9,11,10};
LevelUpPattern lvplayer28={ {{100,10},{200,10},{50,8}},9,11,10};
LevelUpPattern lvplayer38={ {{100,10},{200,10},{50,8}},9,11,10};

LevelUpPattern lvplayer09={ {{100,10},{200,10},{50,8}},9,11,10};
LevelUpPattern lvplayer19={ {{100,10},{200,10},{50,8}},9,11,10};
LevelUpPattern lvplayer29={ {{100,10},{200,10},{50,8}},9,11,10};
LevelUpPattern lvplayer39={ {{100,10},{200,10},{50,8}},9,11,10};


