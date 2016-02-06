#define __CONFIGFILE_C__
#include "version.h"
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>

#include "util.h"
//#include "configfile.h"
//ttom
#include "lssproto_util.h"
#include "configfile.h"
#include "net.h"
//ttom end
#include "npcutil.h"
// Arminius 7.12 login announce
#include "char.h"
#include "char_data.h"
// CoolFish: add 
#include "lssproto_serv.h"

#ifdef _ANGEL_SUMMON
extern struct MissionInfo missionlist[MAXMISSION];
extern struct MissionTable missiontable[MAXMISSIONTABLE];
#endif

/* É¬ÀÃÃ«âç  ÔÊÔÂÑáÕ°   */
typedef struct tagConfig
{
    /*Ãó·òºë·ÂØ©  (·ß  ¶Á±å·Æ»§Ð×ÖÐØêÉýÒý·ÖòÍ  */
  char    progname[8];
  char    configfilename[32]; /* config°×ÑëÄÌ»ï   */
    unsigned int debuglevel;   /* ·¸ÌïÓÀºëÒÁÃ¬»ï */
  unsigned int  usememoryunit;    /*¶ªÆ¹Øø¼°½»ÍßÓÀÐþÈÓÄÌÊõ */
  unsigned int  usememoryunitnum; /*¶ªÆ¹Øø¼°½»ÍßÓÀÐþÐÑ */
  char    asname[32];         /*Ê§ÊÐËü¼þÐþÈÓ¡õÌï¼°  ó¡*/
  unsigned short  acservport; /*Ê§ÊÐËü¼þÐþÈÓ¡õÌï¼°ºÌ¡õÐþ  */
  char    acpasswd[32];       /*Ê§ÊÐËü¼þÐþÈÓ¡õÌï³ß¼°ÓÉµ©·¥¡õÓñ*/
  char    gsnamefromas[32];   /*
                                 * Ê§ÊÐËü¼þÐþÈÓ¡õÌï¾®ÈÕÎ­ÒüÔÂ
                                 * ±Ø¡õØ©ÈÓ¡õÌïÎçØÆ»¯¼°  ó¡
                                 */
                                 
    // Arminius 7.24 manor pk
  char gsid[32];	// game server chinese id
  unsigned short allowmanorpk;	// is this server allow manor pk
  unsigned short port;        /* ÈÓ¡õÌï¼°½÷ÇÐ°¾ØêºÌ¡õÐþ */
	int				servernumber;	/* ±Ø¡õØ©ÈÓ¡õÌï¼°  Ä¯ */
	int				reuseaddr;	/* Address already used... »¥÷±ÒýÈÕØ¦ÖÐÁÝ¼°Ð×»§±å */
  int             do_nodelay;     /* TCP_NODELAY ±åÔÊÔÂ¾®Éýµ¤¾® */
  int             log_write_time; /* Ì¤Îå³ðÐÄ·òºëÃ«ÔÊÔÂ¾®Éýµ¤¾®£Û */
  int             log_io_time;    /* I/Oòå  ¼°ÁÝÃÞ·´¾®ÔÂ¾®Éýµ¤¾®£Û */
  int             log_game_time;  /* ±Ø¡õØ©¼°ÖÊ  òå  ¼°ÁÝÃÞÃ«·´¾®ÔÂ */
  int             log_netloop_faster; /* netloop_faster ¼°·òºë */
	int				saacwritenum;	/* Ê§ÊÐËü¼þÐþÈÓ¡õÌï³ß¼°ÖÏÚÐ  ±åwriteÔÊÔÂ¾® */
	int				saacreadnum;	/* Ê§ÊÐËü¼þÐþÈÓ¡õÌï¾®ÈÕ¼°dispatch Ã«ÖÏ¼ÔÔÊÔÂ¾® */
  unsigned short fdnum;           /*ÉýÄ¾·ÖØêÎìÉúÛÍÆËÒà¼þÃ«âç  ÔÊÔÂ¾® */
  unsigned int   othercharnum;    /*  ¹«¼°Ö°¼°Æ½ÅÒ·Â¼°ÐÑ  */
  unsigned int objnum;            /* ×óÆ¤³âÄáÛÍÐþ¼°    ÐÑ*/
  unsigned int   petcharnum;   	/* Ê¸ÓÀÐþ¼°ÐÑ    */
  unsigned int itemnum;           /* Ê§ÄÌ  Ø©¼°    ÐÑ*/
  unsigned int battlenum;         /* ÌïÐþ»ï¼°    */
  unsigned int battleexp;         /* ÌïÐþ»ï¼°    */
  char    topdir[32];         	/* ÐþÓÀÃó·¸Å«ÒÁÛÍÐþØø  */
  char    mapdir[32];         	/* Ñ¨ÓÀÃó·¸Å«ÒÁÛÍÐþØø  */
  char    maptilefile[32];    	/* Ñ¨ÓÀÃóÉ¬ÀÃ°×ÑëÄÌ»ï  */
  char    battlemapfile[32];    	/* ÌïÐþ»ïÑ¨ÓÀÃóÉ¬ÀÃ°×ÑëÄÌ»ï  */
  char    itemfile[32];       	/* Ê§ÄÌ  Ø©É¬ÀÃ°×ÑëÄÌ»ï  */
  char    invfile[32];        	/*   ³ÄÉ¬ÀÃ°×ÑëÄÌ»ï  */
  char    appearfile[32];     	/* ÇëòØÞË  É¬ÀÃ°×ÑëÄÌ»ï  */
	char	titlenamefile[32];		/* ±¹Ä¯°×ÑëÄÌ»ï   */
	char	titleconfigfile[32];	/* ±¹Ä¯É¬ÀÃ°×ÑëÄÌ»ï   */
	char	encountfile[32];		/* ¾Þ¼þÊÐËü¼þÐþÉ¬ÀÃ°×ÑëÄÌ»ï   */
	char	enemybasefile[32];		/* ³ÄÏæ  É¬ÀÃ°×ÑëÄÌ»ï   */
	char	enemyfile[32];			/* ³ÄÉ¬ÀÃ°×ÑëÄÌ»ï   */
	char	groupfile[32];			/* ºë»ï¡õÃóÉ¬ÀÃ°×ÑëÄÌ»ï   */
	char	magicfile[32];			/* ÈÈÖîÉ¬ÀÃ°×ÑëÄÌ»ï   */
#ifdef _ATTACK_MAGIC
  char    attmagicfile[32];       // ¹¥»÷ÐÔÖäÊõ
#endif

	char	petskillfile[32];		/* Ê¸ÓÀÐþ  ÈÈÖîÉ¬ÀÃ°×ÑëÄÌ»ï   */
  char    itematomfile[32];       /* Ê§ÄÌ  Ø©¼°¼ã    °×ÑëÄÌ»ï */
  char    effectfile[32];     	/* ÉÒÇëÉ¬ÀÃ°×ÑëÄÌ»ï  */
  char    quizfile[32];     		/* ÛÍÄÌÊõÉ¬ÀÃ°×ÑëÄÌ»ï  */
  char    lsgenlog[32];       /*ÈÓ¡õÌï¼°lsgen Ê§ËüÐþÃóÓÀÐþ°×ÑëÄÌ»ï  */
  char    storedir[128];       /*µ©ÐþÊ§·¸Å«ÒÁÛÍÐþØø    */
  char    npcdir[32];         /*NPC¼°É¬ÀÃ°×ÑëÄÌ»ïÃ«  ÈÊ·¸Å«ÒÁÛÍÐþØø   */
  char    logdir[32];         /*
                               * ·òºë·¸Å«ÒÁÛÍÐþØø
                               */
  char    logconfname[32];    /*
                               * ·òºëÉ¬ÀÃ°×ÑëÄÌ»ï  
                               */
  char	chatmagicpasswd[32];	/* ÃñÅÒÓÀÐþ  Ü·ÓÉµ©·¥¡õÓñ */
#ifdef _STORECHAR
  char	storechar[32];
#endif
	unsigned int 	chatmagiccdkeycheck;	/* ÃñÅÒÓÀÐþ  Ü·Æ¥CDKEYÃ«ÃñÄáÓÀÛÍÔÊÔÂ¾® */
  unsigned int    filesearchnum;     /*°×ÑëÄÌ»ïÃ«¸¹³ñÆ¥ÎåÔÂ°×ÑëÄÌ»ï¼°ÐÑ*/
  unsigned int    npctemplatenum;     /*NPC¼°  ¼þÃóÒÁ¡õÐþ°×ÑëÄÌ»ï¼°ÐÑ*/
  unsigned int    npccreatenum;       /*NPC¼°Ï·Ç²°×ÑëÄÌ»ï¼°ÐÑ*/
  unsigned int    walksendinterval;   /* ÐÚÈÊ¼°Ã«ËªÔÂÃÞØÊ */
  unsigned int    CAsendinterval_ms;     /* CAÃ«ËªÔÂÃÞØÊ (ms)*/
  unsigned int    CDsendinterval_ms;     /* CDÃ«ËªÔÂÃÞØÊ (ms)*/
  unsigned int    Onelooptime_ms;     	/* 1»ï¡õÃó±å¾®ØêÔÂÁÝÃÞ */
	unsigned int	Petdeletetime;		/* Ê¸ÓÀÐþ»¥ÛÕ  ÔÊÔÂÁÝÃÞ */
	unsigned int	Itemdeletetime;		/* Ê§ÄÌ  Ø©»¥ÛÕ  ÔÊÔÂÁÝÃÞ */
    /* ·òºëÄÌ¼þ  ¼°Æ½ÅÒ·Â¼°±¾¡õÆ¤Ã«ÔÊÔÂÃÞØÊ */
  unsigned int    CharSavesendinterval;
  unsigned int    addressbookoffmsgnum;  /*
                                          * Ê§ÓñÒÁµ©Æ¤ÓÀÛÍ±å×ó°×·ÂÄÌ¼þ
                                          * ¶ªÓÀ±¾¡õ³âÃ«
                                          * ÖÏ¶ªÓÀ±¾¡õ³âËáÁùÔÂ¾®
                                          */
  unsigned int    protocolreadfrequency;  /*
                                           * Ãó·òÐþÎì»ïÃ«ÖÏÁ¢Øø    
                                           * ±å  ¸ê¾®
                                           */
  unsigned int    allowerrornum;          /*
                                           * ¾Þ·Â¡õÃ«ÖÏòÛÒýÆ¥¸¤ÔÊ¾®
                                           */
  unsigned int    loghour;          		/*
                                           * ·òºëÃ«âçÐåÔÊÔÂÁÝ¶Ô  £¢ÁÝ  
                                           */
  unsigned int    battledebugmsg;    		/*
                                           * ÌïÐþ»ï  ¼°·¸ÌïÓÀºë¶ªÓÀ±¾¡õ³âÃ«ÇëÔÊ¾®£Û¨ßØ¦ÈÕÇë½ñØ¦ÖÐ
                                           */
  //ttom add this because the second had this                                         
  unsigned int    encodekey;              
  unsigned int    acwbsize;             
  unsigned int    acwritesize;
  unsigned int    ErrUserDownFlg;
  //ttom end
#ifdef _GMRELOAD
	char	gmsetfile[32];				/* GMÕÊºÅ¡¢È¨ÏÞÉè¶¨µµ */
#endif
#ifdef _ITEM_QUITPARTY
    char itemquitparty[32];
#endif

#ifdef _DEL_DROP_GOLD	
	unsigned int	Golddeletetime;
#endif
#ifdef _NEW_PLAYER_CF
	int	newplayertrans;
	int	newplayerlv;
	int	newplayergivepet[5];
	int newplayergiveitem[15];
	int	newplayerpetlv;
	int newplayergivegold;
	int ridepetlevel;
#ifdef _VIP_SERVER
	int	newplayerpetvip;
#endif
#ifdef _JZ_CF_DELPETITEM
	int DelPet[5];
	int DelItem[5];
#endif
#endif
#ifdef _USER_EXP_CF
	char	expfile[64];
#endif
#ifdef _UNLAW_WARP_FLOOR
	int	unlawwarpfloor[10];
#endif
#ifdef _WATCH_FLOOR
	int	watchfloor[6];
#endif
#ifdef _BATTLE_FLOOR
	int	battlefloor[6];
#endif
#ifdef _UNREG_NEMA
	char	unregname[5][16];
#endif
#ifdef _TRANS_LEVEL_CF
	int chartrans;
	int pettrans;
	int yblevel;
	int maxlevel;
#endif
#ifdef _POINT
int point;
	int transpoint[8];
#endif
#ifdef _VIP_SERVER
	int vippoint;
#endif
#ifdef _PET_UP
	int petup;
#endif
#ifdef _LOOP_ANNOUNCE
	char loopannouncepath[32];
	int loopannouncetime;
	char loopannounce[10][1024];
	int loopannouncemax;
#endif
#ifdef _SKILLUPPOINT_CF
	int skup;
#endif
#ifdef _RIDELEVEL
	int ridelevel;
#endif
#ifdef _REVLEVEL
	int revlevel;
#endif
#ifdef _NEW_PLAYER_RIDE
	int npride;
#endif
#ifdef _FIX_CHARLOOPS
	int charloops;
#endif
#ifdef _PLAYER_ANNOUNCE
	int pannounce;
#endif
#ifdef _PLAYER_MOVE
	int pmove;
#endif
	int recvbuffer;
	int sendbuffer;
	int recvlowatbuffer;
	int runlevel;
#ifdef _SHOW_VIP_CF
	int showvip;
#endif
#ifdef _PLAYER_NUM
	int playernum;
#endif
#ifdef _BATTLE_GOLD
	int battlegold;
#endif
#ifdef _ANGEL_TIME
	int angelplayertime;
	int angelplayermun;
#endif
#ifdef _RIDEMODE_20
	int ridemode;
#endif
#ifdef _FM_POINT_PK
	int fmpointpk;
#endif
#ifdef _ENEMY_ACTION
	int	enemyact;
#endif

#ifdef _CHECK_PEPEAT
	int	CheckRepeat;
#endif
	int	cpuuse;
#ifdef _VIP_ALL
	int	checkvip;
#endif
#ifdef _FM_JOINLIMIT
	int	joinfamilytime;
#endif
}Config;

Config config;

/*
 *  Îì¼þ°×Å«ºë°×ÑëÄÌ»ïÃ«  ¸êÁÝ±åÒøµ¤ÑáÕ°  
 *  xxxx=yyyy ÐÄÐ×ÖÐØ¦¼°Ã«  ¸ê
 */

typedef struct tagReadConf
{
    char    name[32];       /*xxxx±åØ¤Ð×ÔÂ°À*/

    /*ÆÝ¼°2¹´·´NULLÃ«  Ä¾ÔÂÎçÖÏÊÖÖÊ  ØÆØ¦ÖÐ*/
    char  *charvalue;      /*yyyyÃ«¹«¼°ÒýÒýÒ½  ÔÊÔÂÁÝ¼°Ò½  ÛÆ*/
    size_t  charsize;       /*charvalue¼°ÈÓÄÌÊõ*/

    /*
     *     ¿á  ÎçØÆ»¯=¼°  »¥ "ON"·ÖÔÈÐ×ÈÕ intvalue ±å·´1Ã«Ò½  ÔÊÔÂ
     * ¹«Ä¾¶¯Â½·´ atoi ¼°çø°í
     */
    void*    value;       /*yyyyÃ«  ¾§ÔÊÔÂÒ½  ÔÊÔÂÁÝ¼°Ò½  ÛÆ*/
    CTYPE    valuetype;
}ReadConf;

ReadConf readconf[]=
{
    { "debuglevel"      , NULL ,0 , (void*)&config.debuglevel      ,CHAR},

    { "usememoryunit"   , NULL ,0 , (void*)&config.usememoryunit   ,INT},
    { "usememoryunitnum", NULL ,0 , (void*)&config.usememoryunitnum,INT},

    { "acserv",			config.asname,sizeof(config.asname) ,NULL , 0},
    { "acservport",		NULL ,0 , (void*)&config.acservport     ,SHORT},
    { "acpasswd",		config.acpasswd,sizeof( config.acpasswd),NULL,0},
    { "gameservname",	config.gsnamefromas,sizeof(config.gsnamefromas),
     NULL,0},

    // Arminius 7.24 manor pk
    { "gameservid", config.gsid, sizeof(config.gsid), NULL, 0}, 
    { "allowmanorpk", NULL, 0, (void*)&config.allowmanorpk, SHORT},

    { "port",			NULL ,0 , (void*)&config.port           ,SHORT},
    { "servernumber",	NULL ,0 , (void*)&config.servernumber           ,INT},

    { "reuseaddr",			NULL ,0 , (void*)&config.reuseaddr  ,		INT},
    { "nodelay",			NULL , 0 , (void*)&config.do_nodelay ,		INT},
    { "log_write_time", 	NULL, 0 , (void*)&config.log_write_time,	INT},
    { "log_io_time", 		NULL, 0 , (void*)&config.log_io_time, 		INT},
    { "log_game_time",		NULL, 0 , (void*)&config.log_game_time,		INT},
    { "log_netloop_faster", NULL,0,(void*)&config.log_netloop_faster,	INT},
    { "saacwritenum",		NULL,0,(void*)&config.saacwritenum, 		INT},
    { "saacreadnum",		NULL,0,(void*)&config.saacreadnum, 			INT},
    { "fdnum",				NULL ,0 , (void*)&config.fdnum,				SHORT},
    { "petnum",				NULL ,0 , (void*)&config.petcharnum,		INT},
    { "othercharnum",		NULL ,0 , (void*)&config.othercharnum,		INT},

    { "objnum",			NULL ,0 , (void*)&config.objnum,				INT},
    { "itemnum",		NULL ,0 , (void*)&config.itemnum,				INT},
    { "battlenum",		NULL ,0 , (void*)&config.battlenum,				INT},
    { "battleexp",		NULL ,0 , (void*)&config.battleexp,				INT},
    { "topdir"          , config.topdir,sizeof(config.topdir),NULL,0},
    { "mapdir"          , config.mapdir,sizeof(config.mapdir),NULL,0},
    { "maptilefile"     , config.maptilefile,sizeof(config.maptilefile),NULL,0},
    { "battlemapfile"   , config.battlemapfile,sizeof(config.battlemapfile),NULL,0},

#ifdef _ITEMSET6_TXT
	{ "itemset6file",	config.itemfile,	sizeof(config.invfile),	NULL,	0},
#else
#ifdef _ITEMSET5_TXT
	{ "itemset5file",	config.itemfile,	sizeof(config.invfile),	NULL,	0},
#else
#ifdef _ITEMSET4_TXT
	{ "itemset4file"  , config.itemfile,sizeof(config.invfile),NULL,0},
#else
#ifdef _ITEMSET3_ITEM
	{ "itemset3file"  , config.itemfile,sizeof(config.invfile),NULL,0},
#endif
#endif
#endif
#endif
  { "invinciblefile"  , config.invfile,sizeof(config.invfile),NULL,0},
  { "appearpositionfile"  , config.appearfile,sizeof(config.appearfile),NULL,0},
	{ "titlenamefile", config.titlenamefile, sizeof( config.titlenamefile),NULL,0},
	{ "titleconfigfile", config.titleconfigfile, sizeof( config.titleconfigfile),NULL,0},
	{ "encountfile", config.encountfile, sizeof( config.encountfile),NULL,0},
	{ "enemyfile", config.enemyfile, sizeof( config.enemyfile),NULL,0},
	{ "enemybasefile", config.enemybasefile, sizeof( config.enemybasefile),NULL,0},
	{ "groupfile", config.groupfile, sizeof( config.groupfile),NULL,0},
	{ "magicfile", config.magicfile, sizeof( config.magicfile),NULL,0},
#ifdef _ATTACK_MAGIC
	{ "attmagicfile" , config.attmagicfile , sizeof( config.attmagicfile )  , NULL , 0 },
#endif

#ifdef _PETSKILL2_TXT
	{ "petskillfile2", config.petskillfile, sizeof( config.petskillfile),NULL,0},
#else
	{ "petskillfile1", config.petskillfile, sizeof( config.petskillfile),NULL,0},
#endif

    { "itematomfile" , config.itematomfile, sizeof( config.itematomfile),NULL,0},
    { "effectfile"  , config.effectfile,sizeof(config.effectfile),NULL,0},
    { "quizfile"  , config.quizfile,sizeof(config.quizfile),NULL,0},

    { "lsgenlogfilename", config.lsgenlog,sizeof(config.lsgenlog),NULL,0},
#ifdef _GMRELOAD
	{ "gmsetfile", config.gmsetfile, sizeof( config.gmsetfile),NULL,0},
#endif

    { "storedir"        ,config.storedir,sizeof(config.storedir),NULL,0},
    { "npcdir"          ,config.npcdir,sizeof(config.npcdir),NULL,0},
    { "logdir"          ,config.logdir,sizeof(config.logdir),NULL,0},
    { "logconfname"     ,config.logconfname,sizeof(config.logconfname),NULL,0},
    { "chatmagicpasswd", config.chatmagicpasswd, sizeof( config.chatmagicpasswd),NULL,0},
#ifdef _STORECHAR
    { "storechar", config.storechar, sizeof( config.storechar),NULL,0},
#endif
    { "chatmagiccdkeycheck",  NULL,0, &config.chatmagiccdkeycheck,INT},
    { "filesearchnum",  NULL,0, &config.filesearchnum,INT},
    { "npctemplatenum",  NULL,0, &config.npctemplatenum,INT},
    { "npccreatenum",    NULL,0, &config.npccreatenum,INT},
    { "walkinterval" ,NULL,0,(void*)&config.walksendinterval,INT},
    { "CAinterval" ,NULL,0,(void*)&config.CAsendinterval_ms,INT},
    { "CDinterval" ,NULL,0,(void*)&config.CDsendinterval_ms,INT},
    { "CharSaveinterval" ,NULL,0,(void*)&config.CharSavesendinterval,INT},
    { "Onelooptime" ,NULL,0,(void*)&config.Onelooptime_ms,INT},
    { "Petdeletetime" ,NULL,0,(void*)&config.Petdeletetime,INT},
    { "Itemdeletetime" ,NULL,0,(void*)&config.Itemdeletetime,INT},
	 { "addressbookoffmesgnum" ,NULL,0,
      (void*)&config.addressbookoffmsgnum,INT},

    { "protocolreadfrequency" ,NULL,0,
      (void*)&config.protocolreadfrequency,INT},

    { "allowerrornum" ,NULL,0,(void*)&config.allowerrornum,INT},
    { "loghour" ,NULL,0,(void*)&config.loghour,INT},
    { "battledebugmsg" ,NULL,0,(void*)&config.battledebugmsg,INT},
    //ttom add because the second had
    { "encodekey" ,NULL,0,(void*)&config.encodekey,INT},
    { "acwritesize" ,NULL,0,(void*)&config.acwritesize,INT},
    { "acwbsize" ,NULL,0,(void*)&config.acwbsize,INT},
    { "erruser_down" ,NULL,0,(void*)&config.ErrUserDownFlg,INT},    

#ifdef _ITEM_QUITPARTY
    { "itemquitparty",	config.itemquitparty, sizeof(config.itemquitparty) ,NULL , 0},
#endif

#ifdef _DEL_DROP_GOLD
	{ "Golddeletetime" ,NULL,0,(void*)&config.Golddeletetime,	INT},
#endif

#ifdef _NEW_PLAYER_CF
	{ "TRANS" ,NULL,0,(void*)&config.newplayertrans,	INT},
	{ "LV" ,NULL,0,(void*)&config.newplayerlv,	INT},	
	{ "PET1" ,NULL,0,(void*)&config.newplayergivepet[1],	INT},
	{ "PET2" ,NULL,0,(void*)&config.newplayergivepet[2],	INT},	
	{ "PET3" ,NULL,0,(void*)&config.newplayergivepet[3],	INT},	
	{ "PET4" ,NULL,0,(void*)&config.newplayergivepet[4],	INT},	
	{ "ITEM1" ,NULL,0,(void*)&config.newplayergiveitem[0],	INT},
	{ "ITEM2" ,NULL,0,(void*)&config.newplayergiveitem[1],	INT},	
	{ "ITEM3" ,NULL,0,(void*)&config.newplayergiveitem[2],	INT},	
	{ "ITEM4" ,NULL,0,(void*)&config.newplayergiveitem[3],	INT},	
	{ "ITEM5" ,NULL,0,(void*)&config.newplayergiveitem[4],	INT},
	{ "ITEM6" ,NULL,0,(void*)&config.newplayergiveitem[5],	INT},	
	{ "ITEM7" ,NULL,0,(void*)&config.newplayergiveitem[6],	INT},	
	{ "ITEM8" ,NULL,0,(void*)&config.newplayergiveitem[7],	INT},	
	{ "ITEM9" ,NULL,0,(void*)&config.newplayergiveitem[8],	INT},
	{ "ITEM10" ,NULL,0,(void*)&config.newplayergiveitem[9],	INT},	
	{ "ITEM11" ,NULL,0,(void*)&config.newplayergiveitem[10],	INT},	
	{ "ITEM12" ,NULL,0,(void*)&config.newplayergiveitem[11],	INT},	
	{ "ITEM13" ,NULL,0,(void*)&config.newplayergiveitem[12],	INT},	
	{ "ITEM14" ,NULL,0,(void*)&config.newplayergiveitem[13],	INT},	
	{ "ITEM15" ,NULL,0,(void*)&config.newplayergiveitem[14],	INT},
	{ "PETLV" ,NULL,0,(void*)&config.newplayerpetlv,	INT},	
	{ "GOLD" ,NULL,0,(void*)&config.newplayergivegold,	INT},
	{ "RIDEPETLEVEL" ,NULL,0,(void*)&config.ridepetlevel,	INT},
#ifdef _VIP_SERVER
	{ "GIVEVIPPOINT" ,NULL,0,(void*)&config.newplayerpetvip,	INT},
#endif
#endif

#ifdef _USER_EXP_CF
	{ "USEREXP", config.expfile, sizeof( config.expfile),NULL,0},
#endif

#ifdef _UNLAW_WARP_FLOOR
	{ "FLOOR1" ,NULL,0,(void*)&config.unlawwarpfloor[0],	INT},
	{ "FLOOR2" ,NULL,0,(void*)&config.unlawwarpfloor[1],	INT},	
	{ "FLOOR3" ,NULL,0,(void*)&config.unlawwarpfloor[2],	INT},	
	{ "FLOOR4" ,NULL,0,(void*)&config.unlawwarpfloor[3],	INT},	
	{ "FLOOR5" ,NULL,0,(void*)&config.unlawwarpfloor[4],	INT},
	{ "FLOOR6" ,NULL,0,(void*)&config.unlawwarpfloor[5],	INT},
	{ "FLOOR7" ,NULL,0,(void*)&config.unlawwarpfloor[6],	INT},	
	{ "FLOOR8" ,NULL,0,(void*)&config.unlawwarpfloor[7],	INT},	
	{ "FLOOR6" ,NULL,0,(void*)&config.unlawwarpfloor[8],	INT},	
	{ "FLOOR10" ,NULL,0,(void*)&config.unlawwarpfloor[9],	INT},
#endif

#ifdef _WATCH_FLOOR
	{ "WATCHFLOOR" ,NULL,0,(void*)&config.watchfloor[0],	INT},
	{ "WATCHFLOOR1" ,NULL,0,(void*)&config.watchfloor[1],	INT},
	{ "WATCHFLOOR2" ,NULL,0,(void*)&config.watchfloor[2],	INT},	
	{ "WATCHFLOOR3" ,NULL,0,(void*)&config.watchfloor[3],	INT},	
	{ "WATCHFLOOR4" ,NULL,0,(void*)&config.watchfloor[4],	INT},	
	{ "WATCHFLOOR5" ,NULL,0,(void*)&config.watchfloor[5],	INT},
#endif

#ifdef _BATTLE_FLOOR
	{ "BATTLEFLOOR" ,NULL,0,(void*)&config.battlefloor[0],	INT},
	{ "BATTLEFLOOR1" ,NULL,0,(void*)&config.battlefloor[1],	INT},
	{ "BATTLEFLOOR2" ,NULL,0,(void*)&config.battlefloor[2],	INT},	
	{ "BATTLEFLOOR3" ,NULL,0,(void*)&config.battlefloor[3],	INT},	
	{ "BATTLEFLOOR4" ,NULL,0,(void*)&config.battlefloor[4],	INT},	
	{ "BATTLEFLOOR5" ,NULL,0,(void*)&config.battlefloor[5],	INT},
#endif

#ifdef _UNREG_NEMA
	{ "NAME1" ,config.unregname[0], sizeof( config.unregname[0]),NULL,0},
	{ "NAME2" ,config.unregname[1], sizeof( config.unregname[1]),NULL,0},
	{ "NAME3" ,config.unregname[2], sizeof( config.unregname[2]),NULL,0},
	{ "NAME4" ,config.unregname[3], sizeof( config.unregname[3]),NULL,0},
	{ "NAME5" ,config.unregname[4], sizeof( config.unregname[4]),NULL,0},
#endif
#ifdef _TRANS_LEVEL_CF
	{ "CHARTRANS" ,NULL,0,(void*)&config.chartrans,	INT},
	{ "PETTRANS" ,NULL,0,(void*)&config.pettrans,	INT},	
	{ "LEVEL" ,NULL,0,(void*)&config.yblevel,	INT},	
	{ "MAXLEVEL" ,NULL,0,(void*)&config.maxlevel,	INT},	
#endif
#ifdef _POINT
	{ "POINT" ,NULL,0,(void*)&config.point,	INT},
	{ "TRANS0" ,NULL,0,(void*)&config.transpoint[0],	INT},	
	{ "TRANS1" ,NULL,0,(void*)&config.transpoint[1],	INT},	
	{ "TRANS2" ,NULL,0,(void*)&config.transpoint[2],	INT},	
	{ "TRANS3" ,NULL,0,(void*)&config.transpoint[3],	INT},	
	{ "TRANS4" ,NULL,0,(void*)&config.transpoint[4],	INT},	
	{ "TRANS5" ,NULL,0,(void*)&config.transpoint[5],	INT},	
	{ "TRANS6" ,NULL,0,(void*)&config.transpoint[6],	INT},	
	{ "TRANS7" ,NULL,0,(void*)&config.transpoint[7],	INT},	
#endif

#ifdef _PET_UP
	{ "PETUP" ,NULL,0,(void*)&config.petup,	INT},
#endif
#ifdef _LOOP_ANNOUNCE
	{ "ANNOUNCEPATH" ,config.loopannouncepath, sizeof( config.loopannouncepath),NULL,0},
	{ "ANNOUNCETIME" ,NULL,0,(void*)&config.loopannouncetime,	INT},
#endif
#ifdef _SKILLUPPOINT_CF
	{ "SKILLUPPOINT" ,NULL,0,(void*)&config.skup,	INT},	
#endif
#ifdef _RIDELEVEL
	{ "RIDELEVEL" ,NULL,0,(void*)&config.ridelevel,	INT},	
#endif

#ifdef _REVLEVEL
	{ "REVLEVEL" ,NULL,0,(void*)&config.revlevel,	INT},	
#endif
#ifdef _NEW_PLAYER_RIDE
	{ "NPRIDE" ,NULL,0,(void*)&config.npride,	INT},	
#endif
#ifdef _FIX_CHARLOOPS
	{ "CHARLOOPS" ,NULL,0,(void*)&config.charloops,	INT},	
#endif
#ifdef _PLAYER_ANNOUNCE
	{ "PANNOUNCE" ,NULL,0,(void*)&config.pannounce,	INT},
#endif
#ifdef _PLAYER_MOVE
	{ "PMOVE" ,NULL,0,(void*)&config.pmove,	INT},
#endif

	{ "recvbuffer" ,NULL,0,(void*)&config.recvbuffer,	INT},
	{ "sendbuffer" ,NULL,0,(void*)&config.sendbuffer,	INT},
	{ "recvlowatbuffer" ,NULL,0,(void*)&config.recvlowatbuffer,	INT},
	{ "runlevel" ,NULL,0,(void*)&config.runlevel,	INT},
	
#ifdef _SHOW_VIP_CF
	{ "SHOWVIP" ,NULL,0,(void*)&config.showvip,	INT},
#endif

#ifdef _PLAYER_NUM
	{ "PLAYERNUM" ,NULL,0,(void*)&config.playernum,	INT},
#endif
#ifdef _JZ_CF_DELPETITEM
		{ "delitem1"      , NULL ,0 , (void*)&config.DelItem[0]      ,INT},
		{ "delitem2"      , NULL ,0 , (void*)&config.DelItem[1]      ,INT},
		{ "delitem3"      , NULL ,0 , (void*)&config.DelItem[2]      ,INT},
		{ "delitem4"      , NULL ,0 , (void*)&config.DelItem[3]      ,INT},
		{ "delitem5"      , NULL ,0 , (void*)&config.DelItem[4]      ,INT},
		{ "delpet1"      , NULL ,0 , (void*)&config.DelPet[0]      ,INT},
		{ "delpet2"      , NULL ,0 , (void*)&config.DelPet[1]      ,INT},
		{ "delpet3"      , NULL ,0 , (void*)&config.DelPet[2]      ,INT},
		{ "delpet4"      , NULL ,0 , (void*)&config.DelPet[3]      ,INT},
		{ "delpet5"      , NULL ,0 , (void*)&config.DelPet[4]      ,INT},
#endif
#ifdef _BATTLE_GOLD
	{ "BATTLEGOLD" ,NULL,0,(void*)&config.battlegold,	INT},
#endif
#ifdef _ANGEL_TIME
	{ "ANGELPLAYERTIME" ,NULL,0,(void*)&config.angelplayertime,	INT},
	{ "ANGELPLAYERMUN" ,NULL,0,(void*)&config.angelplayermun,	INT},
#endif
#ifdef _RIDEMODE_20
	{ "RIDEMODE" ,NULL,0,(void*)&config.ridemode,	INT},
#endif
#ifdef _FM_POINT_PK
	{ "FMPOINTPK" ,NULL,0,(void*)&config.fmpointpk,	INT},
#endif
#ifdef _ENEMY_ACTION
	{ "ENEMYACTION" ,NULL,0,(void*)&config.enemyact,	INT},
#endif

#ifdef _CHECK_PEPEAT
	{ "CHECKPEPEAT" ,NULL,0,(void*)&config.CheckRepeat,	INT},
#endif
	{ "CPUUSE" ,NULL,0,(void*)&config.cpuuse,	INT},
#ifdef _VIP_ALL
	{ "QQ" ,NULL,0,(void*)&config.checkvip,	INT},
#endif
#ifdef _FM_JOINLIMIT
	{ "JOINFAMILYTIME" ,NULL,0,(void*)&config.joinfamilytime,	INT},
#endif
};

// Arminius 7.12 login announce
char announcetext[8192];
void AnnounceToPlayer(int charaindex)
{
  char *ptr,*qtr;
  
  ptr=announcetext;
  while ((qtr=strstr(ptr,"\n"))!=NULL) {
    qtr[0]='\0';
//    printf("ptr=%s\n",ptr);
    CHAR_talkToCli(charaindex, -1, ptr, CHAR_COLORYELLOW);
    qtr[0]='\n';
    ptr=qtr+1;
  }
  CHAR_talkToCli(charaindex, -1, ptr, CHAR_COLORYELLOW);
  
}

// Robin 0720
void AnnounceToPlayerWN(int fd)
{
	char buf[8192];
	lssproto_WN_send( fd , WINDOW_MESSAGETYPE_LOGINMESSAGE,
		WINDOW_BUTTONTYPE_OK,
		-1, -1,
		makeEscapeString( announcetext, buf, sizeof(buf))
		);
}


void LoadAnnounce(void)
{
    FILE *f;

    memset(announcetext, 0, sizeof(announcetext));
    if ((f=fopen("./announce.txt","r"))!=NULL) {
      fread(announcetext, sizeof(announcetext), 1, f);
      announcetext[sizeof(announcetext)-1]='\0';
      fclose(f);
    }
}
#ifdef _PET_TALKPRO
PTALK pettalktext[PETTALK_MAXID];

void LoadPetTalk(void)
{
	FILE *fp;
	char fn[256];
	char line[ 4096];
	char talkmem[4096];
	int maxid=0;
	char buf1[256], buf2[256], buf3[256];
	int talkNO=-1, mark=-1, i;
	int len = sizeof( talkmem);
	
	memset(talkmem, 0, sizeof(talkmem));
	sprintf(fn, "%s/pettalk/pettalk.menu", getNpcdir());

	for( i=0;i<PETTALK_MAXID;i++)	{
		pettalktext[i].ID = -1;
		strcpy( pettalktext[i].DATA, "\0");
	}

	print("\n×°ÔØ³èÎï¶Ô»°ÎÄ¼þ:%s...", fn);
	fp = fopen( fn, "r");
    if( fp != NULL ) {
		while( fgets( line, sizeof( line), fp)) {
			if( strlen( talkmem) != 0 ) {
				if( talkmem[strlen( talkmem) -1] != '|' ) {
					strcatsafe( talkmem, len, "|");		
				}
			}
			chompex( line);
			strcatsafe( talkmem,len,  line);
		}
		fclose( fp);
    }else	{
		print("´íÎó:ÕÒ²»µ½ÎÄ¼þ!");
	}

	talkNO=1;
	while( getStringFromIndexWithDelim( talkmem,"END",talkNO, buf1, sizeof( buf1)) != FALSE	){
		talkNO++;
		if( NPC_Util_GetStrFromStrWithDelim( buf1, "PETTEMPNO", buf2, sizeof( buf2)) == NULL  )
			continue;
		mark=1;
		strcpy( fn,"\0");

		if( getStringFromIndexWithDelim( buf2,",", mark+1,buf3,sizeof( buf3)) != FALSE )	{
			pettalktext[maxid].ID = atoi( buf3);
			if( getStringFromIndexWithDelim( buf2,",", mark,buf3,sizeof( buf3)) != FALSE )	{
				sprintf(fn, "%s/pettalk/%s", getNpcdir(), buf3);
				//print("\n ...file:%s", fn);
				fp = fopen( fn, "r");
				if( fp != NULL )	{
					char line[4096];
					while( fgets( line, sizeof( line), fp ) ) {
						if( strlen( pettalktext[maxid].DATA) != 0 ) {
							if( pettalktext[maxid].DATA[strlen( pettalktext[maxid].DATA) -1] != '|' ) {
								strcatsafe( pettalktext[maxid].DATA, sizeof( pettalktext[maxid].DATA), "|");
							}
						}
						chompex( line);
						strcatsafe( pettalktext[maxid].DATA, sizeof( pettalktext[maxid].DATA), line);
					}
					maxid++;
					fclose( fp);
				}else	{
					print("´íÎó:[%s] ÕÒ²»µ½!", fn);
					pettalktext[maxid].ID=-1;
				}
			}else	{
				pettalktext[maxid].ID=-1;
			}
		}
		print(".");
		if( maxid >= PETTALK_MAXID )
			break;
	}
	print("×î´óID=%d...", maxid);
	{
		int haveid=0;
		for( i=0;i<PETTALK_MAXID;i++)	{
			if( pettalktext[i].ID >= 0 )	{
				haveid++;
			}
		}
		print("ÔØÈë×ÜÊý=%d", haveid);
	}

}

#else
char pettalktext[4096];
void LoadPetTalk(void)
{
  FILE *fp;
  char fn[256];
  char	line[ 4096];
  int len = sizeof( pettalktext);
  
  memset(pettalktext, 0, sizeof(pettalktext));
  sprintf(fn, "%s/pettalk/pettalk.mem", getNpcdir());
  
  fp = fopen( fn, "r");
    if( fp != NULL ) {
		print("\n\n ¶ÁÈ¡ pettalk.mem");
		while( fgets( line, sizeof( line), fp)) {
			if( strlen( pettalktext) != 0 ) {
				if( pettalktext[strlen( pettalktext) -1] != '|' ) {
					strcatsafe( pettalktext, len, "|");		
				}
			}
			chompex( line);
			strcatsafe( pettalktext,len,  line);
		}
		fclose( fp);
		print("\n %s", pettalktext);
    }else	{
		print("\n ²»ÄÜÕÒµ½ pettalk.mem");
	}
}
#endif

#ifdef _GAMBLE_BANK
GAMBLEBANK_ITEMS GB_ITEMS[GAMBLEBANK_ITEMSMAX];

void Load_GambleBankItems( void)
{

	FILE *fp;
	char filename[256];
	char buf1[256];
	char name[128];
	int num,ID,type;
	int i=0;
	sprintf(filename, "./data/gambleitems.txt" );
	print("\n¼ÓÔØ¶Ä²©ÎïÆ·ÎÄ¼þ %s ...", filename);
	fp = fopen( filename, "r");
    if( fp != NULL ) {
		while( fgets( buf1, sizeof( buf1), fp) != NULL )	{
			if( strstr( buf1, "#") != 0 ) continue;
			sscanf( buf1,"%s %d %d %d", name, &ID, &num , &type);
			strcpy( GB_ITEMS[i].name, name);
			GB_ITEMS[i].Gnum = num;
			GB_ITEMS[i].ItemId = ID;
			GB_ITEMS[i].type = type;
			i++;
		}
		print("×î´óID: %d ", i);
		fclose( fp);
    }else	{
		print("´íÎó ÕÒ²»µ½ÎÄ¼þ %s", filename);
	}

}
#endif


#ifdef _CFREE_petskill
PETSKILL_CODES Code_skill[PETSKILL_CODE];
void Load_PetSkillCodes( void)
{
	FILE *fp;
	char filename[256];
	char buf1[256];
	char name[128];
	char type[256];
	int num,ID;
	int i=0;
	sprintf(filename, "./data/skillcode.txt" );
	print("\n¼ÓÔØ³èÎï¼¼ÄÜ±àÂëÎÄ¼þ:%s...", filename);
	fp = fopen( filename, "r");
    if( fp != NULL ) {
		while( fgets( buf1, sizeof( buf1), fp) != NULL )	{
			sscanf( buf1,"%s %d %d %s", name, &num, &ID, type);
			strcpy( Code_skill[i].name, name);
			Code_skill[i].TempNo = num;
			Code_skill[i].PetId = ID;
			strcpy( Code_skill[i].Code, type);
			//print("\n %s|%d|%d|%s|", Code_skill[i].name, Code_skill[i].TempNo, 
			//	Code_skill[i].PetId, Code_skill[i].Code);
			i++;
			if( i >= PETSKILL_CODE ) break;
		}
		fclose( fp);
    }else	{
		print("´ò²»µ½ÎÄ¼þ %s", filename);
	}
	print("Íê³É\n");
}
#endif

#ifdef _GMRELOAD
BOOL LoadGMSet( char* filename )
{
	FILE* fp;
	int i = 0, gm_num = 0;
	fp = fopen(filename, "r");
	if (fp == NULL)
	{
		print("ÎÞ·¨´ò¿ªÎÄ¼þ\n");
		return FALSE;
	}
	for (i = 0; i < GMMAXNUM; i++)
	{
		strcpy(gminfo[i].cdkey, "");
		gminfo[i].level = 0;
	}
	while(1){
		char	line[64], cdkey[64], level[64];
		if (fgets(line, sizeof(line), fp) == NULL)	break;
		chop(line);
		//change Ê¹gmset.txt¿ÉÒÔÔö¼Ó×¢½â*******
		if( line[0] == '#' )
			continue;
		for( i=0; i<strlen(line); i++ ){
            if( line[i] == '#' ){
			    line[i] = '\0';
		        break;
			}
		}
		//*************************************
		gm_num = gm_num + 1;
		if (gm_num > GMMAXNUM)	break;
		easyGetTokenFromString(line, 1, cdkey, sizeof(cdkey));
		if (strcmp(cdkey, "") == 0)	break;
		strncpy(gminfo[gm_num].cdkey, cdkey, sizeof(gminfo[gm_num].cdkey));
		easyGetTokenFromString(line, 2, level, sizeof(level));
		if (strcmp(level, "") == 0)	break;
		gminfo[gm_num].level = atoi(level);
//		print("\ncdkey:%s, level:%d", gminfo[gm_num].cdkey, gminfo[gm_num].level);
	}
	fclose(fp);
	return TRUE;
}
#endif

/*------------------------------------------------------------
 * Ãó·òºë·ÂØ©  Ã«  ÔÂ
 * Â¦ÐÑ
 *  Ø¦ØÆ
 * ß¯Ô»°À
 *  cahr*
 ------------------------------------------------------------*/
char* getProgname( void )
{
    return config.progname;
}
/*------------------------------------------------------------
 * configfilename Ã«  ÔÂ£Û
 * Â¦ÐÑ
 *  Ø¦ØÆ
 * ß¯Ô»°À
 *  char*
 ------------------------------------------------------------*/
char* getConfigfilename( void )
{
    return config.configfilename;
}
/*------------------------------------------------------------
 * configfilename Ã«É¬ÀÃÔÊÔÂ£Û
 * Â¦ÐÑ
 *  newv    char*   Þ¥ØÆÖÐ°À
 * ß¯Ô»°À
 *  Ø¦ØÆ
 ------------------------------------------------------------*/
void setConfigfilename( char* newv )
{
    strcpysafe( config.configfilename, sizeof( config.configfilename ),
                newv );
}

/*------------------------------------------------------------
 * ·¸ÌïÓÀºëÒÁÃ¬»ïÃ«  ÔÂ
 * Â¦ÐÑ
 *  Ø¦ØÆ
 * ß¯Ô»°À
 *  unsigned int
 ------------------------------------------------------------*/
unsigned int getDebuglevel( void )
{
    return config.debuglevel;
}
/*------------------------------------------------------------
 * ·¸ÌïÓÀºëÒÁÃ¬»ïÃ«É¬ÀÃÔÊÔÂ
 * Â¦ÐÑ
 *  newv    int     Þ¥ØÆÖÐ°À
 * ß¯Ô»°À
 *  unsigned int    éÉ¼°°À
 ------------------------------------------------------------*/
unsigned int setDebuglevel( unsigned int newv )
{
    int old;
    old = config.debuglevel;
    config.debuglevel = newv;
    return old;
}
/*------------------------------------------------------------
 * memoryunit Ã«  ÔÂ
 * Â¦ÐÑ
 *  Ø¦ØÆ
 * ß¯Ô»°À
 *  unsigned int
 ------------------------------------------------------------*/
unsigned int getMemoryunit( void )
{
    return config.usememoryunit;
}
/*------------------------------------------------------------
 * memoryunitnum Ã«  ÔÂ
 * Â¦ÐÑ
 *  Ø¦ØÆ
 * ß¯Ô»°À
 *  unsigned int
 ------------------------------------------------------------*/
unsigned int getMemoryunitnum( void )
{
    return config.usememoryunitnum;
}

/*------------------------------------------------------------
 * Ê§ÊÐËü¼þÐþÈÓ¡õÌï¼°Ê§ÓñÒÁµ©Ã«  ÔÂ
 * Â¦ÐÑ
 *  Ø¦ØÆ
 * ß¯Ô»°À
 *  char*
 ------------------------------------------------------------*/
char*   getAccountservername( void )
{
    return config.asname;
}
/*------------------------------------------------------------
 * Ê§ÊÐËü¼þÐþÈÓ¡õÌï¼°ºÌ¡õÐþÃ«  ÔÂ
 * Â¦ÐÑ
 *  Ø¦ØÆ
 * ß¯Ô»°À
 *  unsigned short
 ------------------------------------------------------------*/
unsigned short   getAccountserverport( void )
{
    return config.acservport;
}
/*------------------------------------------------------------
 * Ê§ÊÐËü¼þÐþÈÓ¡õÌï³ß¼°ÓÉµ©·¥¡õÓñÃ«  ÔÂ£Û
 * Â¦ÐÑ
 *  Ø¦ØÆ
 * ß¯Ô»°À
 *  unsigned short
 ------------------------------------------------------------*/
char*   getAccountserverpasswd( void )
{
    return config.acpasswd;
}
/*------------------------------------------------------------
 * Ê§ÊÐËü¼þÐþÈÓ¡õÌï¾®ÈÕÎ­ÒüÔÂ±Ø¡õØ©ÈÓ¡õÌïÎçØÆ»¯¼°  ó¡Ã«  ÔÂ£Û
 * Â¦ÐÑ
 *  Ø¦ØÆ
 * ß¯Ô»°À
 *  unsigned short
 ------------------------------------------------------------*/
char*   getGameservername( void )
{
    return config.gsnamefromas;
}

// Arminius 7.24 manor pk
char* getGameserverID( void )
{
    if (config.gsid[strlen(config.gsid)-1]=='\n')
      config.gsid[strlen(config.gsid)-1]='\0';
      
    return config.gsid;
}

unsigned short getAllowManorPK( void )
{
    return config.allowmanorpk;
}

unsigned short getPortnumber( void )
{
    return config.port;
}
/*------------------------------------------------------------
 * ±Ø¡õØ©ÈÓ¡õÌï¼°  Ä¯   Ä¯Ã«  ÔÂ
 * Â¦ÐÑ
 *  Ø¦ØÆ
 * ß¯Ô»°À
 *  unsigned short
 ------------------------------------------------------------*/
int getServernumber( void )
{
    return config.servernumber;
}
/*------------------------------------------------------------
 * reuseaddr ¼°°ÀÃ«  ÔÂ
 * Â¦ÐÑ
 *  Ø¦ØÆ
 * ß¯Ô»°À
 *  unsigned short
 ------------------------------------------------------------*/
int getReuseaddr( void )
{
    return config.reuseaddr;
}

int getNodelay( void )
{
    return config.do_nodelay;
}
int getLogWriteTime(void)
{
    return config.log_write_time;
}
int getLogIOTime( void)
{
    return config.log_io_time;
}
int getLogGameTime(void)
{
    return config.log_game_time;
}
int getLogNetloopFaster(void)
{
    return config.log_netloop_faster;
}

/*------------------------------------------------------------
 * saacwritenum Ã«  ÔÂ£Û
 * Â¦ÐÑ
 *  Ø¦ØÆ
 * ß¯Ô»°À
 *   int
 ------------------------------------------------------------*/
int getSaacwritenum( void )
{
    return config.saacwritenum;
}
/*------------------------------------------------------------
 * saacwritenum Ã«É¬ÀÃÔÊÔÂ£Û
 * Â¦ÐÑ
 *  Ø¦ØÆ
 * ß¯Ô»°À
 *   int
 ------------------------------------------------------------*/
void setSaacwritenum( int num )
{
    config.saacwritenum = num;
}
/*------------------------------------------------------------
 * saacreadnum Ã«  ÔÂ£Û
 * Â¦ÐÑ
 *  Ø¦ØÆ
 * ß¯Ô»°À
 *   int
 ------------------------------------------------------------*/
int getSaacreadnum( void )
{
    return config.saacreadnum;
}
/*------------------------------------------------------------
 * saacreadnum Ã«É¬ÀÃÔÊÔÂ£Û
 * Â¦ÐÑ
 *  Ø¦ØÆ
 * ß¯Ô»°À
 *   int
 ------------------------------------------------------------*/
void setSaacreadnum( int num )
{
    config.saacreadnum = num;
}
/*------------------------------------------------------------
 * fdnum Ã«  ÔÂ£Û
 * Â¦ÐÑ
 *  Ø¦ØÆ
 * ß¯Ô»°À
 *  unsigned int
 ------------------------------------------------------------*/
unsigned int getFdnum( void )
{
    return config.fdnum;
}
/*------------------------------------------------------------
 * petcharanum Ã«  ÔÂ
 * Â¦ÐÑ
 *  Ø¦ØÆ
 * ß¯Ô»°À
 *  unsigned int
 ------------------------------------------------------------*/
unsigned int getPetcharnum( void )
{
    return config.petcharnum;
}


/*------------------------------------------------------------
 * othercharnum Ã«  ÔÂ
 * Â¦ÐÑ
 *  Ø¦ØÆ
 * ß¯Ô»°À
 *  unsigned int
 ------------------------------------------------------------*/
unsigned int getOtherscharnum( void )
{
    return config.othercharnum;
}

/*------------------------------------------------------------
 * objnum Ã«  ÔÂ
 * Â¦ÐÑ
 *  Ø¦ØÆ
 * ß¯Ô»°À
 *  unsigned int
 ------------------------------------------------------------*/
unsigned int getObjnum( void )
{
    return config.objnum;
}

/*------------------------------------------------------------
 * itemnum Ã«  ÔÂ
 * Â¦ÐÑ
 *  Ø¦ØÆ
 * ß¯Ô»°À
 *  unsigned int
 ------------------------------------------------------------*/
unsigned int getItemnum( void )
{
    return config.itemnum;
}


/*------------------------------------------------------------
 * battlenum Ã«  ÔÂ
 * Â¦ÐÑ
 *  Ø¦ØÆ
 * ß¯Ô»°À
 *  unsigned int
 ------------------------------------------------------------*/
unsigned int getBattlenum( void )
{
    return config.battlenum;
}

#ifdef _GET_BATTLE_EXP
unsigned int getBattleexp( void )
{
    return config.battleexp;
}
#endif
/*------------------------------------------------------------
 * topdir Ã«  ÔÂ£Û
 * Â¦ÐÑ
 *  Ø¦ØÆ
 * ß¯Ô»°À
 *  char*
 ------------------------------------------------------------*/
char* getTopdir( void )
{
    return config.topdir;
}
/*------------------------------------------------------------
 * mapdir Ã«  ÔÂ£Û
 * Â¦ÐÑ
 *  Ø¦ØÆ
 * ß¯Ô»°À
 *  char*
 ------------------------------------------------------------*/
char* getMapdir( void )
{
    return config.mapdir;
}
/*------------------------------------------------------------
 * maptilefile Ã«  ÔÂ£Û
 * Â¦ÐÑ
 *  Ø¦ØÆ
 * ß¯Ô»°À
 *  char*
 ------------------------------------------------------------*/
char* getMaptilefile( void )
{
    return config.maptilefile;
}
/*------------------------------------------------------------
 * battlemapfile Ã«  ÔÂ£Û
 * Â¦ÐÑ
 *  Ø¦ØÆ
 * ß¯Ô»°À
 *  char*
 ------------------------------------------------------------*/
char* getBattleMapfile( void )
{
    return config.battlemapfile;
}
/*------------------------------------------------------------
 * itemfile Ã«  ÔÂ£Û
 * Â¦ÐÑ
 *  Ø¦ØÆ
 * ß¯Ô»°À
 *  char*
 ------------------------------------------------------------*/
char* getItemfile( void )
{
    return config.itemfile;
}
/*------------------------------------------------------------
 * invfile Ã«  ÔÂ£Û
 * Â¦ÐÑ
 *  Ø¦ØÆ
 * ß¯Ô»°À
 *  char*
 ------------------------------------------------------------*/
char* getInvfile( void )
{
    return config.invfile;
}
/*------------------------------------------------------------
 * appearfile Ã«  ÔÂ£Û
 * Â¦ÐÑ
 *  Ø¦ØÆ
 * ß¯Ô»°À
 *  char*
 ------------------------------------------------------------*/
char* getAppearfile( void )
{
    return config.appearfile;
}
/*------------------------------------------------------------
 * effectfile Ã«  ÔÂ£Û
 * Â¦ÐÑ
 *  Ø¦ØÆ
 * ß¯Ô»°À
 *  char*
 ------------------------------------------------------------*/
char* getEffectfile( void )
{
    return config.effectfile;
}
/*------------------------------------------------------------
 * titlenamefile Ã«  ÔÂ£Û
 * Â¦ÐÑ
 *  Ø¦ØÆ
 * ß¯Ô»°À
 *  char*
 ------------------------------------------------------------*/
char* getTitleNamefile( void )
{
    return config.titlenamefile;
}
/*------------------------------------------------------------
 * titleconfigfile Ã«  ÔÂ£Û
 * Â¦ÐÑ
 *  Ø¦ØÆ
 * ß¯Ô»°À
 *  char*
 ------------------------------------------------------------*/
char* getTitleConfigfile( void )
{
    return config.titleconfigfile;
}
/*------------------------------------------------------------
 * encountfile Ã«  ÔÂ£Û
 * Â¦ÐÑ
 *  Ø¦ØÆ
 * ß¯Ô»°À
 *  char*
 ------------------------------------------------------------*/
char* getEncountfile( void )
{
    return config.encountfile;
}
/*------------------------------------------------------------
 * enemyfile Ã«  ÔÂ£Û
 * Â¦ÐÑ
 *  Ø¦ØÆ
 * ß¯Ô»°À
 *  char*
 ------------------------------------------------------------*/
char* getEnemyfile( void )
{
    return config.enemyfile;
}
/*------------------------------------------------------------
 * enemybasefile Ã«  ÔÂ£Û
 * Â¦ÐÑ
 *  Ø¦ØÆ
 * ß¯Ô»°À
 *  char*
 ------------------------------------------------------------*/
char* getEnemyBasefile( void )
{
    return config.enemybasefile;
}
/*------------------------------------------------------------
 * groupfile Ã«  ÔÂ£Û
 * Â¦ÐÑ
 *  Ø¦ØÆ
 * ß¯Ô»°À
 *  char*
 ------------------------------------------------------------*/
char* getGroupfile( void )
{
    return config.groupfile;
}
/*------------------------------------------------------------
 * magicfile Ã«  ÔÂ£Û
 * Â¦ÐÑ
 *  Ø¦ØÆ
 * ß¯Ô»°À
 *  char*
 ------------------------------------------------------------*/
char* getMagicfile( void )
{
    return config.magicfile;
}

#ifdef _ATTACK_MAGIC

/*------------------------------------------------------------
 * È¡µÃ¹¥»÷ÐÔµÄÖäÊõ
 * ²ÎÊý
 * None
 * ·µ»ØÖµ
 * char*
 ------------------------------------------------------------*/
char* getAttMagicfileName( void )
{
    return config.attmagicfile;
}

#endif


char* getPetskillfile( void )
{
    return config.petskillfile;
}


#ifdef _ITEM_QUITPARTY
char* getitemquitparty( void )
{
    return config.itemquitparty;
}
#endif

char *getItematomfile( void )
{
    return config.itematomfile;
}


char* getQuizfile( void )
{
    return config.quizfile;
}

/*------------------------------------------------------------
 * lsgenlogfile Ã«  ÔÂ£Û
 * Â¦ÐÑ
 *  Ø¦ØÆ
 * ß¯Ô»°À
 *  char*
 ------------------------------------------------------------*/
char* getLsgenlogfilename( void )
{
    return config.lsgenlog;
}

#ifdef _GMRELOAD
char* getGMSetfile( void )
{
    return config.gmsetfile;
}
#endif

/*------------------------------------------------------------
 * storedir Ã«  ÔÂ£Û
 * Â¦ÐÑ
 *  Ø¦ØÆ
 * ß¯Ô»°À
 *  char*
 ------------------------------------------------------------*/
char* getStoredir( void )
{
    return config.storedir;
}
#ifdef _STORECHAR
/*------------------------------------------------------------
 ------------------------------------------------------------*/
char* getStorechar( void )
{
    return config.storechar;
}
#endif

/*------------------------------------------------------------
 * NPC åÃ¼°·¸Å«ÒÁÛÍÐþØøÃ«  »¯ÈÊÔÂèúÐÑ
 * Â¦ÐÑ
 *  Ø¦ØÆ
 * ß¯Ô»°À
 *  char*
 ------------------------------------------------------------*/
char* getNpcdir( void )
{
    return config.npcdir;
}
/*------------------------------------------------------------
 * ·òºë·¸Å«ÒÁÛÍÐþØøÃ«  ÔÂ£Û
 * Â¦ÐÑ
 *  Ø¦ØÆ
 * ß¯Ô»°À
 *  char*
 ------------------------------------------------------------*/
char* getLogdir( void )
{
    return config.logdir;
}

/*------------------------------------------------------------
 * ·òºëÉ¬ÀÃ°×ÑëÄÌ»ï  Ã«  ÔÂ£Û
 * Â¦ÐÑ
 *  Ø¦ØÆ
 * ß¯Ô»°À
 *  char*
 ------------------------------------------------------------*/
char* getLogconffile( void )
{
    return config.logconfname;
}
/*------------------------------------------------------------
 * ÃñÅÒÓÀÐþ  Ü·ÓÉµ©·¥¡õÓñ Ã«  ÔÂ£Û
 * Â¦ÐÑ
 *  Ø¦ØÆ
 * ß¯Ô»°À
 *  char*
 ------------------------------------------------------------*/
char* getChatMagicPasswd( void )
{
    return config.chatmagicpasswd;
}
/*------------------------------------------------------------
 * ·¸ÌïÓÀºëÃñÅÒÓÀÐþ  Ü·Æ¥¼°CDKEYÃñÄáÓÀÛÍÃ«ÔÊÔÂ¾®Éýµ¤¾®Ã«  ÔÂ£Û
 * Â¦ÐÑ
 *  Ø¦ØÆ
 * ß¯Ô»°À
 *  char*
 ------------------------------------------------------------*/
unsigned getChatMagicCDKeyCheck( void )
{
    return config.chatmagiccdkeycheck;
}

/*------------------------------------------------------------
 * filesearchnumÃ«  ÔÂ£Û
 * Â¦ÐÑ
 *  Ø¦ØÆ
 * ß¯Ô»°À
 *  unsigned int
 ------------------------------------------------------------*/
unsigned int getFilesearchnum( void )
{
    return config.filesearchnum;
}
/*------------------------------------------------------------
 * npctemplatenumÃ«  ÔÂ£Û
 * Â¦ÐÑ
 *  Ø¦ØÆ
 * ß¯Ô»°À
 *  unsigned int
 ------------------------------------------------------------*/
unsigned int getNpctemplatenum( void )
{
    return config.npctemplatenum;
}
/*------------------------------------------------------------
 * npccreatenumÃ«  ÔÂ£Û
 * Â¦ÐÑ
 *  Ø¦ØÆ
 * ß¯Ô»°À
 *  unsigned int
 ------------------------------------------------------------*/
unsigned int getNpccreatenum( void )
{
    return config.npccreatenum;
}

/*------------------------------------------------------------
 * walksendintervalÃ«  ÔÂ£Û
 * Â¦ÐÑ
 *  Ø¦ØÆ
 * ß¯Ô»°À
 *  unsigned int
 ------------------------------------------------------------*/
unsigned int getWalksendinterval( void )
{
    return config.walksendinterval;
}
/*------------------------------------------------------------
 * walksendintervalÃ«±¾ÓÀÐþÔÊÔÂ£Û
 * Â¦ÐÑ
 *  unsigned int 	interval	ÁÝÃÞ  Á¢Øø    
 * ß¯Ô»°À
 *  void
 ------------------------------------------------------------*/
void setWalksendinterval( unsigned int interval )
{
    config.walksendinterval = interval;
}
/*------------------------------------------------------------
 * CAsendintervalÃ«  ÔÂ£Û
 * Â¦ÐÑ
 *  Ø¦ØÆ
 * ß¯Ô»°À
 *  unsigned int
 ------------------------------------------------------------*/
unsigned int getCAsendinterval_ms( void )
{
    return config.CAsendinterval_ms;
}
/*------------------------------------------------------------
 * CAsendintervalÃ«±¾ÓÀÐþÔÊÔÂ£Û
 * Â¦ÐÑ
 *  unsigned int interval
 * ß¯Ô»°À
 *  unsigned int
 ------------------------------------------------------------*/
void setCAsendinterval_ms( unsigned int interval_ms )
{
    config.CAsendinterval_ms = interval_ms;
}
/*------------------------------------------------------------
 * CDsendintervalÃ«  ÔÂ£Û
 * Â¦ÐÑ
 *  Ø¦ØÆ
 * ß¯Ô»°À
 *  unsigned int
 ------------------------------------------------------------*/
unsigned int getCDsendinterval_ms( void )
{
    return config.CDsendinterval_ms;
}
/*------------------------------------------------------------
 * CDsendintervalÃ«±¾ÓÀÐþÔÊÔÂ£Û
 * Â¦ÐÑ
 *  interval		unsigned int
 * ß¯Ô»°À
 * void
 ------------------------------------------------------------*/
void setCDsendinterval_ms( unsigned int interval_ms )
{
	config.CDsendinterval_ms = interval_ms;
}
/*------------------------------------------------------------
 * OnelooptimeÃ«  ÔÂ£Û
 * Â¦ÐÑ
 *  Ø¦ØÆ
 * ß¯Ô»°À
 *  unsigned int
 ------------------------------------------------------------*/
unsigned int getOnelooptime_ms( void )
{
    return config.Onelooptime_ms;
}
/*------------------------------------------------------------
 * OnelooptimeÃ«±¾ÓÀÐþÔÊÔÂ£Û
 * Â¦ÐÑ
 *  interval		unsigned int
 * ß¯Ô»°À
 * void
 ------------------------------------------------------------*/
void setOnelooptime_ms( unsigned int interval_ms )
{
	config.Onelooptime_ms = interval_ms;
}
/*------------------------------------------------------------
 * PetdeletetimeÃ«  ÔÂ£Û
 * Â¦ÐÑ
 *  Ø¦ØÆ
 * ß¯Ô»°À
 *  unsigned int
 ------------------------------------------------------------*/
unsigned int getPetdeletetime( void )
{
    return config.Petdeletetime;
}
/*------------------------------------------------------------
 * PetdeletetimeÃ«±¾ÓÀÐþÔÊÔÂ£Û
 * Â¦ÐÑ
 *  interval		unsigned int
 * ß¯Ô»°À
 * void
 ------------------------------------------------------------*/
void setPetdeletetime( unsigned int interval )
{
	config.Petdeletetime = interval;
}
/*------------------------------------------------------------
 * ItemdeletetimeÃ«  ÔÂ£Û
 * Â¦ÐÑ
 *  Ø¦ØÆ
 * ß¯Ô»°À
 *  unsigned int
 ------------------------------------------------------------*/
unsigned int getItemdeletetime( void )
{
    return config.Itemdeletetime;
}
/*------------------------------------------------------------
 * ItemdeletetimeÃ«±¾ÓÀÐþÔÊÔÂ£Û
 * Â¦ÐÑ
 *  interval		unsigned int
 * ß¯Ô»°À
 * void
 ------------------------------------------------------------*/
void setItemdeletetime( unsigned int interval )
{
	config.Itemdeletetime = interval;
}

/*------------------------------------------------------------
 * CharSavesendintervalÃ«  ÔÂ£Û
 * Â¦ÐÑ
 *  Ø¦ØÆ
 * ß¯Ô»°À
 *  unsigned int
 ------------------------------------------------------------*/
unsigned int getCharSavesendinterval( void )
{
    return config.CharSavesendinterval;
}
/*------------------------------------------------------------
 * CharSavesendintervalÃ«±¾ÓÀÐþÔÊÔÂ£Û
 * Â¦ÐÑ
 *  Ø¦ØÆ
 * ß¯Ô»°À
 *  unsigned int
 ------------------------------------------------------------*/
void setCharSavesendinterval( unsigned int interval)
{
	config.CharSavesendinterval = interval;
}

/*------------------------------------------------------------
 * Addressbookoffmsgnum Ã«  ÔÂ£Û
 * Â¦ÐÑ
 *  Ø¦ØÆ
 * ß¯Ô»°À
 *  unsigned int
 ------------------------------------------------------------*/
unsigned int getAddressbookoffmsgnum( void )
{
    return config.addressbookoffmsgnum;
}
/*------------------------------------------------------------
 * Protocolreadfrequency Ã«  ÔÂ£Û
 * Â¦ÐÑ
 *  Ø¦ØÆ
 * ß¯Ô»°À
 *  unsigned int
 ------------------------------------------------------------*/
unsigned int getProtocolreadfrequency( void )
{
    return config.protocolreadfrequency;
}

/*------------------------------------------------------------
 * Allowerrornum Ã«  ÔÂ£Û
 * Â¦ÐÑ
 *  Ø¦ØÆ
 * ß¯Ô»°À
 *  unsigned int
 ------------------------------------------------------------*/
unsigned int getAllowerrornum( void )
{
    return config.allowerrornum;
}

/*------------------------------------------------------------
 * ·òºëÃ«äú  ÔÊÔÂÁÝ¶ÔÃ«  ÔÂ£Û
 * Â¦ÐÑ
 *  Ø¦ØÆ
 * ß¯Ô»°À
 *  unsigned int
 ------------------------------------------------------------*/
unsigned int getLogHour( void )
{
    return config.loghour;
}

/*------------------------------------------------------------
 * ÌïÐþ»ï  ¼°·¸ÌïÓÀºë¶ªÓÀ±¾¡õ³âÃ«ÇëÔÊ¾®£Û
 * Â¦ÐÑ
 *  Ø¦ØÆ
 * ß¯Ô»°À
 *  unsigned int 1Ø¦ÈÕÇëÔÊ
 ------------------------------------------------------------*/
unsigned int getBattleDebugMsg( void )
{
    return config.battledebugmsg;
}
/*------------------------------------------------------------
 * ÌïÐþ»ï  ¼°·¸ÌïÓÀºë¶ªÓÀ±¾¡õ³âÃ«ÇëÔÊ¾®£Û
 * Â¦ÐÑ
 *  Ø¦ØÆ
 * ß¯Ô»°À
 *  unsigned int 1Ø¦ÈÕÇëÔÊ
 ------------------------------------------------------------*/
void setBattleDebugMsg( unsigned int num )
{
    config.battledebugmsg = num;
}



/*
 * Config¼°·¸°×ÇÉ»ïÐþ°ÀÃ«è£»§ÔÂèúÐÑ
 * Â¦ÐÑ
 *  argv0   char*   ÎìÑ¨¼þÓñ·ÂÄÌ¼þÂ¦ÐÑ¼°  âÙ
 */
void  defaultConfig( char* argv0 )
{
    char* program;                  /* program  Ã«·Æ»§ÔÂ¼°±åÒøµ¤ */

    /* ·¸°×ÇÉ»ïÐþ°ÀÃ«  Ä¾ÔÂ */

    /*Ãó·òºë·ÂØ©  */
    program = rindex(argv0, '/');
    if (program == NULL)
        program = argv0;
    else
        program++;   /* "/"¼°ÆÝ¾®ÈÕ±åØÆÐ×ÖÐ¼°Æ¥++ÔÊÔÂ*/
    strcpysafe( config.progname , sizeof( config.progname ) ,program );

    /*É¬ÀÃ°×ÑëÄÌ»ï  */
    strcpysafe( config.configfilename,
                sizeof( config.configfilename ),"setup.cf" );

}

/*
 * É¬ÀÃ°×ÑëÄÌ»ï  Ã«  ÊÏ·Ö  Æ¥¼°ÖÊ  Ã«µæµ¤£Û
 * Â¦ÐÑ
 *  Ø¦ØÆ
 * ß¯Ô»°À
 *  Ø¦ØÆ
 */
void lastConfig( void )
{
    char    entry[256];
    /*  Ñ¨ÓÀÃó·¸Å«ÒÁÛÍÐþØø¼°É¬ÀÃ    */
    snprintf(entry, sizeof(entry), "%s/%s", config.topdir, config.mapdir);
    strcpysafe(config.mapdir, sizeof(config.mapdir), entry);

    /*  Ñ¨ÓÀÃóÉ¬ÀÃ°×ÑëÄÌ»ï  ¼°É¬ÀÃ    */
    snprintf(entry,sizeof(entry),"%s/%s",
             config.topdir,config.maptilefile);
    strcpysafe(config.maptilefile, sizeof(config.maptilefile), entry);

    /*  ÌïÐþ»ïÑ¨ÓÀÃóÉ¬ÀÃ°×ÑëÄÌ»ï  ¼°É¬ÀÃ    */
    snprintf(entry,sizeof(entry),"%s/%s",
             config.topdir,config.battlemapfile);
    strcpysafe(config.battlemapfile, sizeof(config.battlemapfile), entry);

    /*  Ê§ÄÌ  Ø©É¬ÀÃ°×ÑëÄÌ»ï  ¼°É¬ÀÃ    */
    snprintf(entry,sizeof(entry), "%s/%s", config.topdir,config.itemfile);
    strcpysafe(config.itemfile, sizeof(config.itemfile), entry);

    /*    ³ÄÉ¬ÀÃ°×ÑëÄÌ»ï  ¼°É¬ÀÃ    */
    snprintf(entry,sizeof(entry), "%s/%s", config.topdir,config.invfile);
    strcpysafe(config.invfile, sizeof(config.invfile), entry);

    /*  ÇëòØÞË  É¬ÀÃ°×ÑëÄÌ»ï  ¼°É¬ÀÃ    */
    snprintf(entry,sizeof(entry), "%s/%s", config.topdir,config.appearfile);
    strcpysafe(config.appearfile, sizeof(config.appearfile), entry);

    /*  ÉÒÇëÉ¬ÀÃ°×ÑëÄÌ»ï  ¼°É¬ÀÃ    */
    snprintf(entry,sizeof(entry), "%s/%s", config.topdir,config.effectfile);
    strcpysafe(config.effectfile, sizeof(config.effectfile), entry);

    /*  ÛÍÄÌÊõÉ¬ÀÃ°×ÑëÄÌ»ï  ¼°É¬ÀÃ    */
    snprintf(entry,sizeof(entry), "%s/%s", config.topdir,config.quizfile);
    strcpysafe(config.quizfile, sizeof(config.quizfile), entry);

    /*  ±¹Ä¯  °×ÑëÄÌ»ï  ¼°É¬ÀÃ    */
    snprintf(entry,sizeof(entry), "%s/%s", config.topdir,config.titlenamefile);
    strcpysafe(config.titlenamefile, sizeof(config.titlenamefile), entry);

    /*  lsgen Ê§ËüÐþÃóÓÀÐþ°×ÑëÄÌ»ï      */
    snprintf(entry,sizeof(entry),"%s/%s", config.topdir,config.lsgenlog);
    strcpysafe(config.lsgenlog, sizeof(config.lsgenlog), entry);

    /*  µ©ÐþÊ§·¸Å«ÒÁÛÍÐþØø¼°É¬ÀÃ    */
/*
    snprintf(entry,sizeof(entry), "%s/%s",config.topdir,config.storedir);
    strcpysafe(config.storedir, sizeof(config.storedir), entry);
*/
    /*  NPCÉ¬ÀÃÐþÓÀÃó·¸Å«ÒÁÛÍÐþØø¼°É¬ÀÃ    */
    snprintf(entry,sizeof(entry), "%s/%s",config.topdir,config.npcdir);
    strcpysafe(config.npcdir, sizeof(config.npcdir), entry);

#ifdef _STORECHAR
    /*   */
    snprintf(entry,sizeof(entry), "%s/%s",config.topdir,config.storechar);
    strcpysafe(config.storechar, sizeof(config.storechar), entry);
#endif

}


/*
 * ºÌÄÌ¼þÕý¡õÎçØÍå©Ø¦µáÊ÷  Ã«  ÔÈ»¯Ò½  Ã«ÔÊÔÂèúÐÑ
 * Â¦ÐÑ
 *  to      void*   °ÀÃ«Ò½  ÔÊÔÂºÌÄÌ¼þÕý
 *  type    CTYPE   to¼°µáÃ«è£»§ÔÂ
 *  value   double  to±åÒ½  ÔÊÔÂ°À
 * ß¯Ô»°À
 *  Ø¦ØÆ
 */
void substitutePointerFromType( void* to , CTYPE type ,double value)
{
    switch( type  ){
    case CHAR:
        *(char*)to = (char)value;
        break;
    case SHORT:
        *(short*)to = (short)value;
        break;
    case INT:
        *(int*)to = (int)value;
        break;
    case DOUBLE:
        *(double*)to = (double)value;
        break;
    }
}


/*------------------------------------------------------------
 * É¬ÀÃ°×ÑëÄÌ»ïÃ«  ¸ê
 * Â¦ÐÑ
 *      filename            °×ÑëÄÌ»ï  
 * ß¯Ô»°À
 *      TRUE(1)     ÔÀ  
 *      FALSE(0)    ÁÃ      -> °×ÑëÄÌ»ï¼°×ó¡õÃó¼þ±åÁÃ  ØÆÐ×
 ------------------------------------------------------------*/
BOOL readconfigfile( char* filename )
{
    FILE* f=NULL;
    char linebuf[256];                  /* Óòµæ  ÐÄ  ¸êÌïÓÀ°×Ñë */
    int linenum=0;                      /* µæÐÑÃ«ÐÑÒüÔÂ */
    char    realopenfilename[256];      /*    ¶Ë±åopen ÔÊÔÂ°×ÑëÄÌ»ï  */

    char    hostname[128];

    /*  Ê¯µ©Ðþ  Ã«·Æ»§ÔÂ    */
    if( gethostname( hostname, sizeof(hostname) ) != -1 ){
        char*   initdot;
        initdot = index( hostname, '.' );
        if( initdot != NULL )
            *initdot = '\0';
        snprintf( realopenfilename, sizeof(realopenfilename),
                  "%s.%s" , filename, hostname);

        /* °×ÑëÄÌ»ï¼°×ó¡õÃó¼þ */
        f=fopen( realopenfilename, "r" );
        if( f == NULL )
            print( "Can't open %s.  use %s instead\n", realopenfilename,
                   filename );
    }
    if( f == NULL ){
        f=fopen( filename , "r" );          /* °×ÑëÄÌ»ï¼°×ó¡õÃó¼þ */
        if( f == NULL ){
            print( "Can't open %s\n", filename );
            return FALSE;
        }
    }

    /* ÓòµæÎð¹´  ÐÄ  ¸ê */
    while( fgets( linebuf , sizeof( linebuf ), f ) ){
        char firstToken[256];       /*1    ¼°  Ù¯  */
        int i;                      /*»ï¡õÃó  ÐÑ*/
        int ret;                    /*ØøÕý¡õ¼þÎì¡õÓñ*/

        linenum ++;

        deleteWhiteSpace(linebuf);          /* remove whitespace    */

        if( linebuf[0] == '#' )continue;        /* comment */
        if( linebuf[0] == '\n' )continue;       /* none    */

        chomp( linebuf );                    /* remove tail newline  */

        /* delim "=" Æ¥  âÙ(1)¼°Ðþ¡õÛÍ¼þÃ«  ÔÂ*/
        ret = getStringFromIndexWithDelim( linebuf , "=",  1, firstToken,
                                           sizeof(firstToken) );
        if( ret == FALSE ){
            print( "Find error at %s in line %d. Ignore\n",
                     filename , linenum);
            continue;
        }

        /* readconf ¼°ÈÓÄÌÊõÆ¥»ï¡õÃó */
        for( i = 0 ; i < arraysizeof( readconf ) ; i ++ ){
            if( strcmp( readconf[i].name ,firstToken ) == 0 ){
                /* match */
                char secondToken[256];      /*2    ¼°  Ù¯  */
                /* delim "=" Æ¥2    ¼°Ðþ¡õÛÍ¼þÃ«  ÔÂ*/
                ret = getStringFromIndexWithDelim( linebuf , "=" , 2
                                                   , secondToken ,
                                                   sizeof(secondToken) );

                /* NULL  Ù¯¾®Éýµ¤¾®Ã«Æ©ÍÍÔÂ */
                if( ret == FALSE ){
                    print( "Find error at %s in line %d. Ignore",
                           filename , linenum);
                    break;
                }


                /*NULL·ÖÔÈÐ×ÈÕÒ½  ØÆØ¦ÖÐ*/
                if( readconf[i].charvalue != NULL )
                    strcpysafe( readconf[i].charvalue
                                ,readconf[i].charsize, secondToken);

                /*NULL·ÖÔÈÐ×ÈÕÒ½  ØÆØ¦ÖÐ*/
                if( readconf[i].value != NULL ) {
                    if( strcmp( "ON" ,secondToken ) == 0 ) {
                        /*ON·ÖÔÈÐ×ÈÕ1Ã«  Ä¾ÔÂ*/
                        substitutePointerFromType( readconf[i].value,
                                                   readconf[i].valuetype,
                                                   1.0);

                    }else if( strcmp( "OFF" ,secondToken ) == 0 ) {
                        /*OFF·ÖÔÈÐ×ÈÕ1Ã«  Ä¾ÔÂ*/
                        substitutePointerFromType( readconf[i].value,
                                                   readconf[i].valuetype,
                                                   1.0);
                    }else {
                        strtolchecknum(secondToken,
                                       (int*)readconf[i].value,
                                       10, readconf[i].valuetype);
					}
				}
                break;
            }
        }
    }
    fclose( f );
    lastConfig();
    return TRUE;
}
//ttom add this becaus the second had this function 
/*------------------------------------------------------------
 * ¾Þ¼þÎì¡õÓñÆ½¡õÃ«É¬ÀÃÔÊÔÂ
 * Â¦ÐÑ
 *  Ø¦ØÆ
 * ß¯Ô»°À
 *  unsigned int Æ½¡õÃ«ß¯ÔÊ
------------------------------------------------------------*/
unsigned int setEncodeKey( void )
{
   JENCODE_KEY = config.encodekey;
   return JENCODE_KEY;
}
/*------------------------------------------------------------
* Ê§ÊÐËü¼þÐþÈÓ¡õÌï¡õ±åÌ¤Îå  ¸êÌïÓÀ°×Ñë¼°ÈÓÄÌÊõÃ«É¬ÀÃÔÊÔÂ
* Â¦ÐÑ
*  Ø¦ØÆ
* ß¯Ô»°À
*  unsigned int Æ½¡õÃ«ß¯ÔÊ
------------------------------------------------------------*/
unsigned int setAcWBSize( void )
{
    AC_WBSIZE = config.acwbsize;
    return AC_WBSIZE;
}
unsigned int getAcwriteSize( void )
{
    return config.acwritesize;
}
unsigned int getErrUserDownFlg( void )
{
    return config.ErrUserDownFlg;
}
    

#ifdef _DEL_DROP_GOLD
unsigned int getGolddeletetime( void )
{
    if( config.Golddeletetime > 0 )
		return config.Golddeletetime;
	else
		return config.Itemdeletetime;
}
void setIGolddeletetime( unsigned int interval )
{
	config.Golddeletetime = interval;
}
#endif

#ifdef _NEW_PLAYER_CF
int getNewplayertrans( void )
{
  if(config.newplayertrans > 7)
		return 7;
	else if(config.newplayertrans >= 0)
		return config.newplayertrans;
	else
		return 0;
}
int getNewplayerlv( void )
{
  if(config.newplayerlv > 160)
		return 160;
	else if(config.newplayerlv >0)
		return config.newplayerlv;
	else
		return 0;
}
int getNewplayerpetlv( void )
{
  if(config.newplayerpetlv > 160)
		return 160;
	else if(config.newplayerpetlv > 0)
		return config.newplayerpetlv;
	else
		return 0;
}

int getNewplayergivepet( unsigned int index )
{
  if(config.newplayergivepet[index] > 0 )
		return config.newplayergivepet[index];
	else
		return -1;
}

int getNewplayergiveitem( unsigned int index )
{
  if(config.newplayergiveitem[index] > 0 )
		return config.newplayergiveitem[index];
	else
		return -1;
}

void setNewplayergivepet( unsigned int index ,unsigned int interval)
{
	config.newplayergivepet[index] = interval;
}

int getNewplayergivegold( void )
{
  if(config.newplayergivegold > 1000000)
		return 1000000;
	else if(config.newplayergivegold < 0)
		return 0;
	else
		return config.newplayergivegold;
}
int getRidePetLevel( void )
{
  if(config.ridepetlevel > 0 )
		return config.ridepetlevel;
	else
		return -1;
}
#ifdef _VIP_SERVER
int getNewplayergivevip( void )
{
	return config.newplayerpetvip < 0?0:config.newplayerpetvip;
}
#endif
#endif

#ifdef _UNLAW_WARP_FLOOR
int getUnlawwarpfloor( unsigned int index )
{
  if(config.unlawwarpfloor[index] > 0 )
		return config.unlawwarpfloor[index];
	else
		return -1;
}
#endif

#ifdef _WATCH_FLOOR
int getWatchFloor( unsigned int index )
{
  if(config.watchfloor[index] > 0 )
		return config.watchfloor[index];
	else
		return -1;
}
char* getWatchFloorCF( void )
{
	return (config.watchfloor[0]>0)? "ÊÇ":"·ñ";
}
#endif

#ifdef _BATTLE_FLOOR
int getBattleFloor( unsigned int index )
{
  if(config.battlefloor[index] > 0 )
		return config.battlefloor[index];
	else
		return -1;
}
char* getBattleFloorCF( void )
{
	return (config.battlefloor[0]>0)? "ÊÇ":"·ñ";
}
#endif

#ifdef _USER_EXP_CF

char* getEXPfile( void )
{
    return config.expfile;
}
#endif

#ifdef _UNREG_NEMA
char* getUnregname( int index )
{
    return config.unregname[index];
}
#endif

#ifdef _TRANS_LEVEL_CF
int getChartrans( void )
{
		if(config.chartrans>6)
			config.chartrans=6;
    return config.chartrans;
}
int getPettrans( void )
{
		if(config.pettrans>2)
			return 2;
		else if(config.pettrans<-1)
			return -1;
    return config.pettrans;
}
int getYBLevel( void )
{
		if(config.yblevel>config.maxlevel)
			config.yblevel=config.maxlevel;
    return config.yblevel;
}
int getMaxLevel( void )
{
    return config.maxlevel;
}
#endif

#ifdef _POINT
char* getPoint( void )
{
		return (config.point>0)? "ÊÇ":"·ñ";
}
int getTransPoint( int index )
{
		return config.transpoint[index];
}
#endif

#ifdef _PET_UP
char* getPetup( void )
{
		return (config.petup>0)? "ÊÇ":"·ñ";
}
#endif
#ifdef _LOOP_ANNOUNCE
char* getLoopAnnouncePath( void )
{
		return config.loopannouncepath;
}
int loadLoopAnnounce( void )
{
	FILE* fp;
	int i = 0;
	config.loopannouncemax=0;
	fp = fopen(config.loopannouncepath, "r");
	if (fp == NULL)
	{
		print("ÎÞ·¨´ò¿ªÎÄ¼þ\n");
		return FALSE;
	}
	while(1){
		char	line[1024];
		if (fgets(line, sizeof(line), fp) == NULL)	break;
		chop(line);

		if( line[0] == '#' )
			continue;
		for( i=0; i<10; i++ ){
            if( line[i] == '#' ){
			    line[i] = '\0';
		        break;
			}
		}
		//*************************************
		strcpy(config.loopannounce[config.loopannouncemax],line);
		config.loopannouncemax++;
	}
	fclose(fp);
	return TRUE;
}
int getLoopAnnounceTime( void )
{
    return (config.loopannouncetime<0)?-1:config.loopannouncetime;
}
int getLoopAnnounceMax( void )
{
    return (config.loopannouncemax>0)?config.loopannouncemax:0;
}
char* getLoopAnnounce( int index )
{
    return config.loopannounce[index];
}
#endif

#ifdef _SKILLUPPOINT_CF
int getSkup( void )
{
    return (config.skup>0)?config.skup:0;
}
#endif
#ifdef _RIDELEVEL
int getRideLevel( void )
{
    return config.ridelevel;
}
#endif
#ifdef _REVLEVEL
char* getRevLevel( void )
{
		return (config.revlevel>0)?"ÊÇ":"·ñ";
}
#endif
#ifdef _NEW_PLAYER_RIDE
char* getPlayerRide( void )
{
		if(config.npride>2)
			return "ÅäÌ×ËÍ»¢¼ÓÀ×";
		else if(config.npride==2)
			return "ÅäÌ×ËÍÀ×";
		else if(config.npride==1)
			return "ÅäÌ×ËÍ»¢";
		else
			return "²»ËÍÅäÌ×Æï³è";
}
#endif

#ifdef _FIX_CHARLOOPS
int getCharloops( void )
{
    return config.charloops-1;
}
#endif

#ifdef _PLAYER_ANNOUNCE
int getPAnnounce( void )
{
    return (config.pannounce>-1)?config.pannounce:-1;
}
#endif
#ifdef _PLAYER_MOVE
int getPMove( void )
{
    return (config.pmove>-1)?config.pmove:-1;
}
#endif

int getrecvbuffer( void )
{
		if(config.recvbuffer<0)
	    return 0;
	  else if(config.recvbuffer>128)
	    return 128;
	  else
	  	return config.recvbuffer;
}

int getsendbuffer( void )
{
		if(config.sendbuffer<0)
	    return 0;
	  else if(config.sendbuffer>128)
	    return 128;
	  else
	  	return config.sendbuffer;
}

int getrecvlowatbuffer( void )
{
		if(config.recvlowatbuffer<0)
	    return 0;
	  else if(config.recvlowatbuffer>1024)
	    return 1024;
	  else
	  	return config.recvlowatbuffer;
}

int getrunlevel( void )
{
		if(config.runlevel<-20)
	    return -20;
	  else if(config.runlevel>19)
	    return 19;
	  else
	  	return config.runlevel;

}

#ifdef _SHOW_VIP_CF
int getShowVip( void )
{
		if(config.showvip>2)
			return 2;
		else if(config.showvip<0)
			return 0;
		else
			return config.showvip;
}
#endif

#ifdef _PLAYER_NUM
int getPlayerNum( void )
{
		return config.playernum;
}
void setPlayerNum( int num )
{
		config.playernum=num;
}
#endif
#ifdef _JZ_CF_DELPETITEM
int getDelPet(int Num)
{
	return config.DelPet[Num];
}

int getDelItem(int Num)
{
	return config.DelItem[Num];
}
#endif
#ifdef _BATTLE_GOLD
int getBattleGold( void )
{
		if(config.battlegold<0)
	    return 0;
	  else if(config.battlegold>100)
	    return 100;
	  else
	  	return config.battlegold;

}
#endif

#ifdef _ANGEL_TIME
int getAngelPlayerTime( void )
{
		return (config.angelplayertime>1)?config.angelplayertime:1;
}
int getAngelPlayerMun( void )
{
		return (config.angelplayermun>2)?config.angelplayermun:2;
}
#endif

#ifdef _RIDEMODE_20
int getRideMode( void )
{
		if(config.ridemode<0)
			config.ridemode=0;
		return config.ridemode;
}
#endif
#ifdef _FM_POINT_PK
char *getFmPointPK( void )
{
		return (config.fmpointpk>0)?"ÊÇ":"·ñ";
}
#endif
#ifdef _ENEMY_ACTION
int getEnemyAction( void )
{
		if(config.enemyact>100)
			return 100;
		else if(config.enemyact<1)
			return 1;
		else
			return config.enemyact;
}
#endif

int getCpuUse( void )
{
	return config.cpuuse;
}
#ifdef _CHECK_PEPEAT
int getCheckRepeat( void )
{
		return (config.CheckRepeat>0)?1:0;
}
#endif

#ifdef _VIP_ALL
int getCheckVip( void )
{
		return config.checkvip;
}
#endif

#ifdef _FM_JOINLIMIT
int getJoinFamilyTime( void )
{
		return config.joinfamilytime;
}
#endif
