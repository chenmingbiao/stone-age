#ifndef __CHAR_H__
#define __CHAR_H__

#include "common.h"
#include "char_base.h"
#include "net.h"
#include "char_data.h"


/*------------------------------------------------------------
 *白巧件玄及缙及烂聒
 *------------------------------------------------------------*/
typedef enum
{
	CHAR_COLORWHITE,
	CHAR_COLORCYAN,
	CHAR_COLORPURPLE,
	CHAR_COLORBLUE,
	CHAR_COLORYELLOW,
	CHAR_COLORGREEN,
	CHAR_COLORRED,
	CHAR_COLORGRAY,
	CHAR_COLORBLUE2,
	CHAR_COLORGREEN2,
}CHAR_COLOR;
/*====================平乓仿及综岳卞楮允月楮醒====================*/
void CHAR_createNewChar( int clifd, int dataplacenum,char* charname ,
						  int imgno,int faceimgno,
						  int vital,int str,int tgh,int dex,
						  int earth,int water,int fire,int wind,
						  int hometown , char *cdkey );

/*====================平乓仿及夫弘奶件====================*/

void CHAR_login( int clifd, char* data, int saveindex );

#define		CHAR_warpToSpecificPoint( cindex, fl, x, y)	_CHAR_warpToSpecificPoint( __FILE__, __LINE__, cindex, fl, x, y)
BOOL _CHAR_warpToSpecificPoint( char *file, int line, 
							int charaindex, int fl, int x, int y);


/*====================平乓仿及本□皮====================*/
/*====================平乓仿及夫弘失它玄====================*/
BOOL CHAR_charSaveFromConnectAndChar( int fd,Char* ch, int unlock );
BOOL CHAR_charSaveFromConnect( int fd,int unlock );
#define	CHAR_logout( clifd, save)	_CHAR_logout( __FILE__, __LINE__, clifd, save)
BOOL _CHAR_logout( char *file, int line, int clifd, BOOL save);

/*====================watch event    ====================*/
void CHAR_sendWatchEvent( int objindex, int chac, int* opt,int optlen,BOOL myflg );

/*====================旦平伙====================*/
BOOL CHAR_Skillupsend(int charaindex );
void CHAR_SkillUp(  int charaindex, int skillid );
void CHAR_useSkill( int charaindex, int dir ,int skindex );

typedef enum
{
    CHAR_WALKSUCCESSED,     /* 岳      */
    CHAR_WALKSYSTEMERROR,   /* 扑旦  丞巨仿□    及index民尼永弁卞
                             * 夫匀井井匀凶午井 */
    CHAR_WALKEXTEND,        /*    区左□田□仄化汹仇丹午仄凶［NPC卞及心*/
    CHAR_WALKHITOBJECT,     /*  窒井  卞癫匀化｝汹仃卅井匀凶    */
    CHAR_WALKPREWALK,       /*  prewalk奶矛件玄匹汹仃卅井匀凶( 檗祭 ) */
    CHAR_WALKDIE,           /*  韶氏匹中化汹仃卅中  */
    CHAR_WALK1357,          /*  标户  轾卞汹仇丹午仄化汹仃卅井匀凶  */
}CHAR_WALKRET;

/*====================平乓仿及啖  卞楮允月楮醒====================*/
void CHAR_ctodirmode(char moji , int* dir , int* mode);
INLINE void CHAR_getDXDY( int dir , int* dx, int* dy );
INLINE int CHAR_getDX( int dir );
INLINE int CHAR_getDY( int dir );
int CHAR_getSameCoordinateObjects(int* objbuf, int siz,int ff, int fx, int fy);
void CHAR_walkcall( int index );
void CHAR_walk_start(int index, int x, int y, char* dir, BOOL mapsendmode);
void CHAR_walk_init( int fd, int x, int y, char *direction, BOOL mapsendmode);

CHAR_WALKRET CHAR_walk(int index, int dir, int mode);

char* CHAR_makeOptionString( Char* ch );
char* CHAR_makeStatusString( int index, char* category );

#define CHAR_makeObjectCString( objindex, buf, buflen) _CHAR_makeObjectCString( __FILE__, __LINE__, objindex, buf, buflen)
BOOL _CHAR_makeObjectCString( char *file, int line, int objindex, char* buf, int buflen );

//BOOL CHAR_sendStatusString( int charaindex, char* category );
#define CHAR_sendStatusString( A, B) _CHAR_sendStatusString( A, B, __FILE__, __LINE__ )
BOOL _CHAR_sendStatusString( int charaindex, char* category, char* file, int line );
BOOL CHAR_sendItemData( int charaindex, int *itemgroup, int num);
BOOL CHAR_sendItemDataOne( int charaindex, int haveitemindex);
BOOL CHAR_send_P_StatusString( int charaindex, unsigned int indextable );
BOOL CHAR_send_N_StatusString( int charaindex, int num, unsigned int indextable );
BOOL CHAR_send_K_StatusString( int charaindex, int num, unsigned int indextable );

void CHAR_inputOwnTitle( int index ,char* name );

void CHAR_selectTitle( int index, int titleindex );
void CHAR_deleteTitle( int index, int titleindex );

#define CHAR_complianceParameter( index) _CHAR_complianceParameter( index, __FILE__, __LINE__)
int _CHAR_complianceParameter( int index, char *FILE, int LINE);

int CHAR_findSurplusItemBox( int index );
int CHAR_findEmptyItemBox( int index );
int CHAR_findEmptyPoolItemBox( int index );
int CHAR_findEmptyItemBoxNo( int index );

void CHAR_moveEquipItem( int index, int fromindex, int toindex );
void CHAR_ItemUse( int charaindex, int to_charaindex, int haveitemindex );
void CHAR_DropItem( int charaindex,  int itemindex );
int CHAR_DropItemAbsolute( int itemindex, int floor, int x, int y,BOOL net);
BOOL CHAR_DropItemFXY( int charaindex, int itemcharaindex, int fl,
							  int x, int y, int* objindex );
int  CHAR_addItemSpecificItemIndex( int charaindex, int itemindex );

void CHAR_PickUpItem( int charaindex, int dir );
void CHAR_DropMoney( int charaindex,  int amount );
int CHAR_addItem( int charaindex, int itemid );
int CHAR_addItemToChar( Char* ch, int itemid );

int CHAR_pickupFollowPet( int charaindex, int petindex );

#ifdef _GAMBLE_ROULETTE
int NPC_MAPCLEANGOLD( int meindex , int floor);
#endif

#ifdef _DROPSTAKENEW
#define MAXSTAKENUM	5
int CasinoAccumulation(int charindex, int npcindex, int floor, int wincasinotype);
void CHAR_talkToFloor(int floor, int talkindex, char* message, CHAR_COLOR color);
int SetCasinoMap(int npcindex, int casinotype, int mapdropflag);
int CasinoPay(int npcindex, int wincasinotype);
#endif

void CHAR_sendCSpecifiedObjindex( int fd, int index);
void CHAR_sendSpecifiedobjindexCToCharaindex(int charaindex,int objindex);

void CHAR_sendCToArroundCharacter( int charaindex );

void CHAR_sendArroundCharaData( int charaindex );

void CHAR_sendCDArroundChar( int fl, int x, int y, int objindex );
void CHAR_sendCDArroundChar_Main( int fl, int x, int y, int objindex, BOOL mode );


void CHAR_Look( int charaindex, int dir );

void CHAR_initChatMagic(void);

char* CHAR_appendNameAndTitle( int charaindex, char* src, char* buf,
                               int buflen );
void CHAR_Talk( int fd, int index,char* message,int color, int area );

void CHAR_Loop( void );

#define		WINDOW_BUTTONTYPE_NONE		(0)
#define		WINDOW_BUTTONTYPE_OK		(1 << 0)
#define		WINDOW_BUTTONTYPE_CANCEL	(1 << 1)
#define		WINDOW_BUTTONTYPE_YES		(1 << 2)
#define		WINDOW_BUTTONTYPE_NO		(1 << 3)
#define		WINDOW_BUTTONTYPE_PREV		(1 << 4)
#define		WINDOW_BUTTONTYPE_NEXT		(1 << 5)

#define		WINDOW_BUTTONTYPE_OKCANCEL	(WINDOW_BUTTONTYPE_OK | WINDOW_BUTTONTYPE_CANCEL)
#define		WINDOW_BUTTONTYPE_YESNO	(WINDOW_BUTTONTYPE_YES | WINDOW_BUTTONTYPE_NO)


typedef enum
{
	WINDOW_MESSAGETYPE_MESSAGE,
	WINDOW_MESSAGETYPE_MESSAGEANDLINEINPUT,
	WINDOW_MESSAGETYPE_SELECT,
	WINDOW_MESSAGETYPE_PETSELECT,
	WINDOW_MESSAGETYPE_PARTYSELECT,
	WINDOW_MESSAGETYPE_PETANDPARTYSELECT,
	WINDOW_MESSAGETYPE_ITEMSHOPMENU,
	WINDOW_MESSAGETYPE_ITEMSHOPMAIN,
	WINDOW_MESSAGETYPE_LIMITITEMSHOPMAIN,
	WINDOW_MESSAGETYPE_PETSKILLSHOP,
	WINDOW_MESSAGETYPE_WIDEMESSAGE,
	WINDOW_MESSAGETYPE_WIDEMESSAGEANDLINEINPUT,
	WINDOW_MESSAGETYPE_POOLITEMSHOPMENU,
	WINDOW_MESSAGETYPE_POOLITEMSHOPMAIN,

	WINDOW_MESSAGETYPE_FAMILYADD,
	WINDOW_MESSAGETYPE_FAMILYJOIN,
	WINDOW_MESSAGETYPE_FAMILYOUT,
	WINDOW_MESSAGETYPE_FAMILYEND,
//=======================================
        // shan add	
        WINDOW_FMMESSAGETYPE_SELECT,
        WINDOW_FMMESSAGETYPE_DENGON,
        WINDOW_FMMESSAGETYPE_FMSDENGON,
        WINDOW_FMMESSAGETYPE_POINTLIST,
#ifdef _FMVER21
        WINDOW_FMMESSAGETYPE_TOP30DP,
#endif
        WINDOW_FMMESSAGETYPE_DP,
        WINDOW_MESSAGETYPE_BANK,

	// Arminius 7.12 scheduleman
	WINDOW_MESSAGETYPE_PKSCHEDULELIST,
	WINDOW_MESSAGETYPE_PKSCHEDULESELECTFAMILY,
	WINDOW_MESSAGETYPE_PKSCHEDULEDETAIL,
	
	// Robin
	WINDOW_MESSAGETYPE_LOGINMESSAGE,
	WINDOW_MESSAGETYPE_FAMILYTAX,
	WINDOW_MESSAGETYPE_SHOWRIDEPET,
	WINDOW_MESSAGETYPE_FAMILYDETAIL,

	WINDOW_MESSAGETYPE_LEADERSELECT,
	WINDOW_MESSAGETYPE_LEADERSELECTQ,
	WINDOW_MESSAGETYPE_LEADERSELECTA,

  // Arminius 1.3 Auctioneer
  WINDOW_MESSAGETYPE_AUCTIONNEW,
  WINDOW_MESSAGETYPE_AUCTIONLIST_BUY,
  WINDOW_MESSAGETYPE_AUCTIONSURVEY,
  WINDOW_MESSAGETYPE_AUCTIONMODIFY,
  WINDOW_MESSAGETYPE_AUCTIONLIST_MODIFY,

#ifdef _PETSKILL_CANNEDFOOD
	WINDOWS_MESSAGETYPE_PETSKILLSHOW,
#endif

#ifdef _MOUSE_DBL_CLICK
	WINDOW_MESSAGETYPE_MOUSEGETNAME,
#endif

#ifdef _RACEMAN
	WINDOW_MESSAGETYPE_RACEMAN_RANK,
#endif

}WINDOW_MESSAGETYPE;

typedef enum
{
    CHAR_WINDOWTYPE_RETURNTOELDER=-1,   /*  赢  尺  月它奴件玉它    */
    CHAR_WINDOWTYPE_RESURRECTION=-2,   /*  汊唾允月它奴件玉它    */

	CHAR_WINDOWTYPE_SELECTBATTLE = 1,		/*   月爵  毛蓟  允月它奴件玉它 */
	CHAR_WINDOWTYPE_SELECTDUEL = 2,  		/*   月DUEL毛蓟  允月它奴件玉它 */
	CHAR_WINDOWTYPE_SELECTTRADECARD = 3, 	/*   铜跟晶毛蓟  允月它奴件玉它 */
	CHAR_WINDOWTYPE_SELECTPARTY = 4, 		/* 由□  奴毛蓟  允月它奴件玉它 */
	CHAR_WINDOWTYPE_SELECTBATTLEWATCH = 5, 	/* 棋爵毛蓟  允月它奴件玉它 */
	CHAR_WINDOWTYPE_MICMESSAGE = 6,			/* MICNPC毛银匀化请月它奴件玉它 */

	// CoolFish: Trade 2001/4/18
	CHAR_WINDOWTYPE_SELECTTRADE = 7,	/* 交易 Window */

	CHAR_WINDOWTYPE_SELECTRENAMEITEM_PAGE1 = 10,	/*   蟆毛  凳允月失奶  丞毛蓟  允月它奴件玉它 */
	CHAR_WINDOWTYPE_SELECTRENAMEITEM_PAGE2 = 11,	/*   蟆毛  凳允月失奶  丞毛蓟  允月它奴件玉它 */
	CHAR_WINDOWTYPE_SELECTRENAMEITEM_PAGE3 = 12,	/*   蟆毛  凳允月失奶  丞毛蓟  允月它奴件玉它 */
	CHAR_WINDOWTYPE_SELECTRENAMEITEM_PAGE4 = 13,	/*   蟆毛  凳允月失奶  丞毛蓟  允月它奴件玉它 */
	CHAR_WINDOWTYPE_SELECTRENAMEITEM_RENAME = 14,	/*   蟆毛    允月它奴件玉它 */
	CHAR_WINDOWTYPE_SELECTRENAMEITEM_RENAME_ATTENTION  = 15,	/*   蟆毛    允月它奴件玉它 */
	
	CHAR_WINDOWTYPE_DENGON = 50,			/* 鳗蜕   */

	CHAR_WINDOWTYPE_WINDOWMAN_START = 100,
	CHAR_WINDOWTYPE_WINDOWMAN_STARTMSG = CHAR_WINDOWTYPE_WINDOWMAN_START,
	CHAR_WINDOWTYPE_WINDOWMAN_END = 200,

	CHAR_WINDOWTYPE_JANKEN_START = 210,
	CHAR_WINDOWTYPE_JANKEN_MAIN = 211,
	CHAR_WINDOWTYPE_JANKEN_END = 212,
	
	CHAR_WINDOWTYPE_TRANSMIGRATION_START = 213,
	CHAR_WINDOWTYPE_TRANSMIGRATION_MAIN = 214,
	CHAR_WINDOWTYPE_TRANSMIGRATION_END = 215,
	CHAR_WINDOWTYPE_TRANSMIGRATION_NONE = 216,
	

	CHAR_WINDOWTYPE_WINDOWHEALER_START = 220,
	CHAR_WINDOWTYPE_WINDOWHEALER_STARTMSG = CHAR_WINDOWTYPE_WINDOWHEALER_START,
	CHAR_WINDOWTYPE_WINDOWHEALER_HPMSG = 221,
	CHAR_WINDOWTYPE_WINDOWHEALER_OKHPMSG = 222,
	CHAR_WINDOWTYPE_WINDOWHEALER_SPIRITMSG = 223,
	CHAR_WINDOWTYPE_WINDOWHEALER_OKSPIRITMSG = 224,
	CHAR_WINDOWTYPE_WINDOWHEALER_ALLMSG = 225,
	CHAR_WINDOWTYPE_WINDOWHEALER_OKALLMSG = 226,
	CHAR_WINDOWTYPE_WINDOWHEALER_END = 227,

	CHAR_WINDOWTYPE_WINDOWSAVEPOINT_START = 230,

	CHAR_WINDOWTYPE_WINDOWEVENT_STARTMSG = 231,
	CHAR_WINDOWTYPE_WINDOWEVENT_NOWEVENT = 232,
	CHAR_WINDOWTYPE_WINDOWEVENT_ENDEVENT = 233,
	CHAR_WINDOWTYPE_WINDOWEVENT_REQMAINMSG = 234,
	CHAR_WINDOWTYPE_WINDOWEVENT_ACCMAINMSG = 235,
	CHAR_WINDOWTYPE_WINDOWEVENT_NOMALMSG = 236,
	CHAR_WINDOWTYPE_WINDOWEVENT_CLEANMSG = 237,
	CHAR_WINDOWTYPE_WINDOWEVENT_REQTHANK = 238,
	CHAR_WINDOWTYPE_WINDOWEVENT_ACCTHANK = 239,
	

	CHAR_WINDOWTYPE_WINDOWITEMSHOP_START = 240,
	CHAR_WINDOWTYPE_WINDOWITEMSHOP_STARTMSG = CHAR_WINDOWTYPE_WINDOWITEMSHOP_START,
	CHAR_WINDOWTYPE_WINDOWITEMSHOP_MENU = 241,
	CHAR_WINDOWTYPE_WINDOWITEMSHOP_BUY_MSG = 242,
	CHAR_WINDOWTYPE_WINDOWITEMSHOP_SELL_MSG = 243,
	CHAR_WINDOWTYPE_WINDOWITEMSHOP_END = 244,
	CHAR_WINDOWTYPE_WINDOWITEMSHOP_LIMIT = 245,
	CHAR_WINDOWTYPE_WINDOWITEMSHOP_EXPRESS = 246,

	CHAR_WINDOWTYPE_DUELRANKING_START = 250,
	CHAR_WINDOWTYPE_DUELRANKING_TOPRANKING = 251,
	CHAR_WINDOWTYPE_DUELRANKING_MYRANKING = 252,
	CHAR_WINDOWTYPE_DUELRANKING_WAIT = 253,
	CHAR_WINDOWTYPE_DEFEND_BILLDBOARD = 254,

	CHAR_WINDOWTYPE_WINDOWPETSKILLSHOP = 260,

	CHAR_WINDOWTYPE_WINDOWPETSHOP_START = 261,
	CHAR_WINDOWTYPE_WINDOWPETSHOP_PETSELECT = 262,
	CHAR_WINDOWTYPE_WINDOWPETSHOP_MAIN = 263,
	CHAR_WINDOWTYPE_WINDOWPETSHOP_GOLDOVER 	= 264,
	CHAR_WINDOWTYPE_WINDOWPETSHOP_PETSELECT2 = 265,
	CHAR_WINDOWTYPE_WINDOWPETSHOP_MAIN2 = 266,
	CHAR_WINDOWTYPE_WINDOWPETSHOP_DRAWSELECT = 267,
	CHAR_WINDOWTYPE_WINDOWPETSHOP_ASKDRAW = 268,
	CHAR_WINDOWTYPE_WINDOWPETSHOP_END = 269,

	CHAR_WINDOWTYPE_WINDOWWARPMAN_MAIN = 271,
	CHAR_WINDOWTYPE_WINDOWWARPMAN_ERR = 272,
	CHAR_WINDOWTYPE_WINDOWWARPMAN_END = 273,

	CHAR_WINDOWTYPE_NPCENEMY_START = 281,
	
	CHAR_WINDOWTYPE_CHARM_START = 282,
	CHAR_WINDOWTYPE_CHARM_END = 283,

	CHAR_WINDOWTYPE_QUIZ_START = 284,
	CHAR_WINDOWTYPE_QUIZ_MAIN = 285,
	CHAR_WINDOWTYPE_QUIZ_END = 286,

	CHAR_WINDOWTYPE_CHECKMAN_START = 287,
	CHAR_WINDOWTYPE_CHECKMAN_MAIN = 288,
	CHAR_WINDOWTYPE_CHECKMAN_END = 289,

	CHAR_WINDOWTYPE_FAMILYMAN_START = 320,
	CHAR_WINDOWTYPE_FAMILYMAN_ADD = 321,
	CHAR_WINDOWTYPE_FAMILYMAN_JOIN = 322,
	CHAR_WINDOWTYPE_FAMILYMAN_OUT = 323,
	CHAR_WINDOWTYPE_FAMILYMAN_BROKEN = 324,
	
	CHAR_WINDOWTYPE_BANKMAN = 330,
	
	// CoolFish: FMPKMan 2001/7/4
	CHAR_WINDOWTYPE_FMPKMAN_START = 340,
	CHAR_WINDOWTYPE_FMPKMAN_VIEW = 341,
	CHAR_WINDOWTYPE_FMPKMAN_LEAVEPK = 342,
	CHAR_WINDOWTYPE_FMPKCALLMAN_START = 345,
	CHAR_WINDOWTYPE_FMPKCALLMAN_CALL = 346,
	CHAR_WINDOWTYPE_FMPKCALLMAN_COME = 347,
	CHAR_WINDOWTYPE_FMPKCALLMAN_LEAVE = 348,
	
	// Arminius 7.13 scheduleman
	CHAR_WINDOWTYPE_SCHEDULEMAN_START = 350,
	CHAR_WINDOWTYPE_SCHEDULEMAN_SELECT = 351,
	CHAR_WINDOWTYPE_SCHEDULEMAN_DETAIL = 352,
	
	// Arminius 7.27
	CHAR_WINDOWTYPE_MANORPK_START = 360,
	CHAR_WINDOWTYPE_MANORPK_ASK = 361,
	CHAR_WINDOWTYPE_MANORPK_END = 362,

        // shan
        CHAR_WINDOWTYPE_FM_DENGON      = 370,     // 家族留言板
        CHAR_WINDOWTYPE_FM_FMSDENGON   = 371,     // 家族之间留言板
        CHAR_WINDOWTYPE_FM_MESSAGE1    = 372,     // 说明视窗(据点)
        CHAR_WINDOWTYPE_FM_MESSAGE2    = 373,     // 说明视窗(成员)
        CHAR_WINDOWTYPE_FM_SELECT      = 374,     // 选项视窗
        CHAR_WINDOWTYPE_FM_MEMBERLIST  = 375,     // 成员列表
        CHAR_WINDOWTYPE_FM_POINTLIST   = 376,     // 据点列表
        CHAR_WINDOWTYPE_FM_DPTOP       = 377,     // 强者表(前叁十大列榜)
        CHAR_WINDOWTYPE_FM_DPME        = 378,     // 强者表(自己的列榜)
        CHAR_WINDOWTYPE_FM_DPSELECT    = 379,     // 强者表的选项视窗

#ifdef _PET_TRANS
		CHAR_WINDOWTYPE_PETTRANS_START		= 384,
		CHAR_WINDOWTYPE_PETTRANS_SELECT		= 385,
		CHAR_WINDOWTYPE_PETTRANS_SELPET		= 386,
		CHAR_WINDOWTYPE_PETTRANS_MAIN1		= 387,
		CHAR_WINDOWTYPE_PETTRANS_END		= 389,
#endif

#ifdef _GAMBLE_BANK
		NPC_GambleBank_START				= 390,
		NPC_GambleBank_SELECT,
		NPC_GambleBank_BANK,
		NPC_GambleBank_CHANG1,
		NPC_GambleBank_CHANG2,
		NPC_GambleBank_END,
#endif

#ifdef _PETRACE
	CHAR_WINDOWTYPE_PETRACEMASTER_START = 410,
	CHAR_WINDOWTYPE_PETRACEMASTER_RULE,
	CHAR_WINDOWTYPE_PETRACEMASTER_PET,
	CHAR_WINDOWTYPE_PETRACEMASTER_LEAVE,
#endif
	
#ifdef _GAMBLE_ROULETTE
	WINDOWTYPE_GAMBLEROULETTE_START = 415,
	WINDOWTYPE_GAMBLEROULETTE_SELECT,
	WINDOWTYPE_GAMBLEROULETTE_END,
#endif

#ifdef _NEWEVENT
  CHAR_WINDOWTYPE_WINDOWEVENT_ACCTHANKNEW = 430,
#endif

#ifdef _TRANSER_MAN
	NPC_TRANSERMAN_START = 440,
	NPC_TRANSERMAN_SELECT,
	NPC_TRANSERMAN_WARP,
	NPC_TRANSERMAN_END,
#endif
#ifdef _ITEM_NPCCHANGE
	NPC_ITEMCHANGE_START = 445,
	NPC_ITEMCHANGE_SELECT,
	NPC_ITEMCHANGE_MESSAGE,
	NPC_ITEMCHANGE_END,
#endif
#ifdef _ADD_FAMILY_TAX			   // WON ADD 增加庄园税收
	CHAR_WINDOWTYPE_FAMILY_TAX = 450,
#endif

#ifdef _ALLDOMAN // (不可开) Syu ADD 排行榜NPC
	NPC_ALLDOMAN_START = 460,
	NPC_ALLDOMAN_START2,
	NPC_ALLDOMAN_START3,
	NPC_ALLDOMAN_HEAL,
	NPC_ALLDOMAN_GIVEMONEY,
	NPC_ALLDOMAN_MAIN_WND , 
	NPC_ALLDOMAN_SELECT_WND , 
	NPC_ALLDOMAN_LIST_WND , 
#endif

#ifdef _PETSKILL_CANNEDFOOD
	ITEM_WINDOWTYPE_SELECTPETSKILL_SELECT = 470,
	ITEM_WINDOWTYPE_SELECTPETSKILL_END,
#endif

}CHAR_WINDOWTYPE;

#ifdef _GMRELOAD
typedef struct tagGMInfo
{
	char cdkey[24];
	int level;
}GMInfo;
#endif

BOOL CHAR_talkToCli( int talkedcharaindex,int talkcharaindex, char* message, CHAR_COLOR color );
void CHAR_talkToCliAndParty( int talkedcharaindex,int talkcharaindex,char* message, CHAR_COLOR color );

BOOL CHAR_talkToCharacter( int talkedcharaindex,int talkcharaindex, char* message );
void CHAR_getCoordinationDir( int dir , int x, int y ,int c,
                              int *xout , int *yout );
BOOL CHAR_createCharacter( int type, int floor, int x, int y, int dir,
                           int* charaindex, int* objindex, BOOL seemap );
void CHAR_CharaDelete( int charaindex );
void CHAR_ObjectDelete( int objindex );
int CHAR_makeDBKey( int charaindex, char *pszBuffer, int size );
int CHAR_getEmptyPartyArray( int charaindex);
BOOL CHAR_JoinParty( int charaindex );
void CHAR_JoinParty_Main( int charaindex, int targetindex);
BOOL CHAR_DischargeParty( int charaindex, int flg);
BOOL CHAR_DischargePartyNoMsg( int charaindex);
BOOL CHAR_setMyPosition_main( int index, int x, int y, int setdir, BOOL CAFlg);
BOOL CHAR_setMyPosition( int index, int x, int y, BOOL CAFlg);

void CHAR_CharaDeleteHavePet( int charaindex);
int CHAR_sendAction( int charaindex, int action, int mode);
void CHAR_sendLeader( int objindex, int leader);
void CHAR_sendBattleWatch( int objindex, int onoff);
void CHAR_sendBattleEffect( int charaindex, int onoff);

// shan
void CHAR_sendTradeEffect( int charaindex, int onoff);
#ifdef _MIND_ICON
void CHAR_sendMindEffect( int charaindex, int onoff);
#endif
#ifdef _ITEM_CRACKER
void CHAR_sendCrackerEffect( int charaindex, int onoff);
#endif

void CHAR_inputUserPetName( int index , int havepetindex, char* name );
int CHAR_getPartyIndex( int index, int num);
void CHAR_processWindow(int charaindex, int seqno, int select,
						int objindex, char* data );
void CHAR_AddCharm( int charaindex, int iValue );
void CHAR_PetAddVariableAi( int petindex, int iValue );
void CHAR_PartyUpdate( int charaindex, int senddata );
char *CHAR_getUseName( int charaindex );
char *CHAR_getUseID( int charaindex );
EXTERN int EnemyMoveNum;	/*   凛卞  嫖  仃月衬及醒 */

#define DB_DUELPOINT	"db_duel"			// 犯亘巨伙禾奶件玄犯□正矛□旦
#define DB_ADDRESSBOOK	"db_addressbook"	// 失玉伊旦皮永弁犯□正矛□旦

BOOL CHAR_send_DpDBUpdate( int charaindex );
BOOL CHAR_send_DpDBUpdate_AddressBook( int charaindex, int mode );


void CHAR_sendPMEToArroundCharacter( int charaindex, int petindex, int flg, int no );
void CHAR_sendPMEToArroundCharacterFLXY( int petindex, 
								int fl, int x, int y, int dir, int flg, int no );

void CHAR_sendSEoArroundCharacter( int fl, int x, int y, int senumber, int sw );

BOOL CHAR_initEffectSetting( char* filename );
void CHAR_checkEffect( int charaindex);
void CHAR_checkEffectLoop( void);
void CHAR_initDebugChatCdkey( void);
int CHAR_setChatMagicCDKey( int mode, char *cdkey);


float GetRecoveryRate( int charaindex );
int storeCharaData( void );
#ifdef _MAGIC_REHPAI	//补血AI
int Magic_RideGetHP( int toindex, int petindex, int flg);
#endif
// CoolFish: Trade 2001/4/18
int CHAR_findTotalEmptyItem(int index);


#ifdef _FIX_METAMORIDE
int CHAR_CHECKJOINENEMY( int index);
#endif

#ifdef _SEND_EFFECT	   	        // WON ADD AC送下雪、下雨等特效
#define		CHAR_EFFECT_SETTINGBUFFER	256
typedef struct tagCHAR_effectsetting
{
    int     floor;								//	白夫失
	int		effect;								//	梢请  寞
	int		level;								//	梢请及伊矛伙［  蜇及雄今［
	int		sendflg;							//	  憎巨白尼弁玄毛霜匀凶井升丹井［
	char	month[CHAR_EFFECT_SETTINGBUFFER];	//	  垫允月畸
	char	day[CHAR_EFFECT_SETTINGBUFFER];		//	  垫允月  
	char	hour[CHAR_EFFECT_SETTINGBUFFER];	//	  垫允月凛棉
	char	min[CHAR_EFFECT_SETTINGBUFFER];		//	  垫允月坌
	char	expire[CHAR_EFFECT_SETTINGBUFFER];	//	  垫仄化中月赢今［(    
	
}CHAR_effectsetting;

CHAR_effectsetting*    CHAR_effect;
int                    CHAR_effectnum;
#endif

#ifdef _PET_LOSTPET
BOOL CHAR_CharSaveLostPet( int petindex, int type);
#endif
#ifdef _ALLDOMAN
void InitHeroList( void);
#endif
#endif

BOOL checkUnlawWarpFloor( int floor);

#ifdef _HELP_NEWHAND
void CHAR_loginAddItemForNew( int charaindex );
#endif

#ifdef _TIME_TICKET
void check_TimeTicket();
int check_TimeTicketMap(int floor);
#endif
