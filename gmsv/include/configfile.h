#ifndef __CONFIGFILE_H__
#define __CONFIGFILE_H__

#include "common.h"

#undef EXTERN
#ifdef __CONFIGFILE_C__
#define EXTERN
#else  /*  __CONFIGFILE_C__  */
#define EXTERN extern
#endif /*  __CONFIGFILE_C__  */

// Arminius 7.12 login announce
extern char announcetext[8192];
void AnnounceToPlayer(int charaindex);
// Robin 0720
void AnnounceToPlayerWN(int fd);
void LoadAnnounce(void);

#ifdef _PET_TALKPRO
	#define PETTALK_MAXID 8
	typedef struct {
		int ID;
		char DATA[1024*64];	//1M
	}PTALK;
	extern PTALK pettalktext[PETTALK_MAXID];
	void LoadPetTalk(void);
#else
	
extern char pettalktext[4096];
void LoadPetTalk(void);
#endif

#ifdef _GAMBLE_BANK
#define GAMBLEBANK_ITEMSMAX 100
#define DEFEND_ITEMSMAX 40
typedef struct	REGAMBLEBANKITEMS	{
	char name[128];
	int Gnum;
	int ItemId;
	int type;
}GAMBLEBANK_ITEMS;

extern GAMBLEBANK_ITEMS GB_ITEMS[GAMBLEBANK_ITEMSMAX];

void Load_GambleBankItems( void);
#endif
#ifdef _JZ_CF_DELPETITEM
int getDelPet(int Num);
int getDelItem(int Num);
#endif
#ifdef _CFREE_petskill
#define PETSKILL_CODE 300
typedef struct	REPETSKILL_CODES	{
	char name[128];
	int TempNo;
	int PetId;
	char Code[256];
}PETSKILL_CODES;

extern PETSKILL_CODES Code_skill[PETSKILL_CODE];
void Load_PetSkillCodes( void);
#endif

#ifdef _GMRELOAD
#define GMMAXNUM	100
struct GMINFO
{
	char cdkey[24];
	int  level;
};
extern struct GMINFO gminfo[GMMAXNUM];
#endif

BOOL readconfigfile( char* filename );

void  defaultConfig( char* argv0 );
char* getProgname( void );
char* getConfigfilename( void );
void setConfigfilename( char* newv );

unsigned int getDebuglevel( void );
unsigned int setDebuglevel( unsigned int newv );
unsigned int getMemoryunit( void );
unsigned int getMemoryunitnum( void );
char*   getAccountservername( void );
unsigned short   getAccountserverport( void );
char*   getAccountserverpasswd( void );
char*   getGameservername( void );

unsigned short getPortnumber( void );

int getServernumber( void );
int getReuseaddr( void );
int getNodelay( void );
int getLogWriteTime(void);
int getLogIOTime( void);
int getLogGameTime(void);
int getLogNetloopFaster(void);
int getSaacwritenum( void );
void setSaacwritenum( int num );
int getSaacreadnum( void );
void setSaacreadnum( int num );

unsigned int getFdnum( void );
unsigned int setFdnum(int temp_fdnum);
unsigned int getPetcharnum( void );
unsigned int getOtherscharnum( void );
unsigned int getObjnum( void );
unsigned int getItemnum( void );
unsigned int getBattlenum( void );

char* getTopdir( void );
char* getMapdir( void );
char* getMaptilefile( void );
char* getBattleMapfile( void );
char* getItemfile( void );
char* getInvfile( void );
char* getAppearfile( void );
char* getEffectfile( void );
char* getTitleNamefile( void );
char* getTitleConfigfile( void );
char* getLsgenlogfilename( void );
char* getStoredir( void );
#ifdef _STORECHAR
char* getStorechar( void );
#endif
char* getNpcdir( void );
char* getLogdir( void );
char* getLogconffile( void );
char* getChatMagicPasswd( void );
unsigned int getChatMagicCDKeyCheck( void );


unsigned int getFilesearchnum( void );
unsigned int getNpctemplatenum( void );
unsigned int getNpccreatenum( void );
unsigned int getWalksendinterval( void );
void		 setWalksendinterval( unsigned int );
unsigned int getCAsendinterval_ms( void );
void 		 setCAsendinterval_ms( unsigned int );
unsigned int getCDsendinterval_ms( void );
void		 setCDsendinterval_ms( unsigned int );
unsigned int getOnelooptime_ms( void );
void		 setOnelooptime_ms( unsigned int );
unsigned int getCharSavesendinterval( void );
void setCharSavesendinterval( unsigned int interval);
unsigned int getAddressbookoffmsgnum( void );
unsigned int getProtocolreadfrequency( void );
unsigned int getAllowerrornum( void );
unsigned int getLogHour( void );
unsigned int getBattleDebugMsg( void );
void setBattleDebugMsg( unsigned int );

char* getEncountfile( void );
char* getEnemyfile( void );
char* getGroupfile( void );
char* getEnemyBasefile( void );
char* getMagicfile( void );
#ifdef _ATTACK_MAGIC

char* getAttMagicfileName( void );

#endif

char* getPetskillfile( void );
char *getItematomfile( void );
char *getQuizfile( void );

unsigned int getPetdeletetime( void );
void setPetdeletetime( unsigned int interval );

unsigned int getItemdeletetime( void );
void setItemdeletetime( unsigned int interval );
//ttom add this because the second have this function
//unsigned int getAcwriteSize( void );

#ifdef _DEL_DROP_GOLD
unsigned int getGolddeletetime( void );
void setGolddeletetime( unsigned int interval );
#endif

unsigned int setEncodeKey( void );
unsigned int setAcWBSize( void );
//ttom end

// CoolFish: +2 2001/4/18
unsigned int getAcwriteSize( void );
unsigned int getErrUserDownFlg( void );

// Arminius 7.24 manor pk
char* getGameserverID( void );
unsigned short getAllowManorPK( void );

// Terry 2001/10/03 service ap
char* getApID(void);
unsigned short getApPort(void);
int getLoopTime(void);
int getEnableService(void);

#ifdef _GMRELOAD
char* getGMSetfile(void);
BOOL LoadGMSet(char *filename);
#endif

#ifdef _ITEM_QUITPARTY
char* getitemquitparty( void );
#endif

#ifdef _NEW_PLAYER_CF	
int getNewplayertrans( void );
int getNewplayerlv( void );
int getNewplayergivepet( unsigned int index );
void setNewplayergivepet( unsigned int index ,unsigned int interval);
int getNewplayerpetlv( void );
int getNewplayergivegold( void );
int getRidePetLevel( void );
#ifdef _VIP_SERVER
int getNewplayergivevip( void );
#endif
#endif

#ifdef _USER_EXP_CF
char* getEXPfile( void );
#endif

#ifdef _UNREG_NEMA
char* getUnregname( int index );
#endif

#ifdef _TRANS_LEVEL_CF
int getChartrans( void );
int getPettrans( void );
int getYBLevel( void );
int getMaxLevel( void );
#endif

#ifdef _POINT
char* getPoint( void );
int getTransPoint( int index );
#endif

#ifdef _PET_UP
char* getPetup( void );
#endif
#ifdef _WATCH_FLOOR
int getWatchFloor( unsigned int index );
char* getWatchFloorCF( void );
#endif

#ifdef _BATTLE_FLOOR
int getBattleFloor( unsigned int index );
char* getBattleFloorCF( void );
#endif

#ifdef _LOOP_ANNOUNCE
char* getLoopAnnouncePath( void );
int loadLoopAnnounce( void );
int getLoopAnnounceTime( void );
char* getLoopAnnounce( int index );
int getLoopAnnounceMax( void );
#endif

#ifdef _SKILLUPPOINT_CF
int getSkup( void );
#endif
#ifdef _RIDELEVEL
int getRideLevel( void );
#endif
#ifdef _REVLEVEL
char* getRevLevel( void );
#endif
#ifdef _NEW_PLAYER_RIDE
char* getPlayerRide( void );
#endif
#ifdef _FIX_CHARLOOPS
int getCharloops( void );
#endif
#ifdef _PLAYER_ANNOUNCE
int getPAnnounce( void );
#endif
#ifdef _PLAYER_MOVE
int getPMove( void );
#endif

int getrecvbuffer( void );
int getsendbuffer( void );
int getrecvlowatbuffer( void );
int getrunlevel( void );

#ifdef _SHOW_VIP_CF
int getShowVip( void );
#endif

#ifdef _PLAYER_NUM
int getPlayerNum( void );
void setPlayerNum( int num );
#endif
#ifdef _BATTLE_GOLD
int getBattleGold( void );
#endif

#ifdef _ANGEL_TIME
int getAngelPlayerTime( void );
int getAngelPlayerMun( void );
#endif

#ifdef _RIDEMODE_20
int getRideMode( void );
#endif

#ifdef _FM_POINT_PK
char *getFmPointPK( void );
#endif

#ifdef _ENEMY_ACTION
int getEnemyAction( void );
#endif

char *getMacCheck( void );
int getCpuUse( void );

#ifdef _CHECK_PEPEAT
int getCheckRepeat( void );
#endif

#ifdef _VIP_ALL
int getCheckVip( void );
#endif
#ifdef _FM_JOINLIMIT
int getJoinFamilyTime( void );
#endif

#endif
