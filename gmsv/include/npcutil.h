#ifndef _NPCUTIL_H_
#define _NPCUTIL_H_

BOOL NPC_Util_AddOneTitle( int charindex, int titleindex );
BOOL NPC_Util_HaveTitle( int charindex , int titleindex );
BOOL NPC_Util_Nearby( int x1 , int y1, int x2 , int y2 );
BOOL NPC_Util_CharNearby(int ind1,int ind2);
int NPC_Util_YN(char *input );
int NPC_Util_getDirFromTwoPoint( POINT* pstart, POINT* pend );
int NPC_Util_countHaveItem( int meindex , int itemid );
BOOL NPC_Util_isBackContact( int frontindex , int backindex );

void NPC_Util_AnnounceFloor( int floorid , char *msg );


BOOL NPC_Util_moveItemToChar( int charindex, int itemindex,BOOL net );
BOOL NPC_Util_createItemToChar( int charindex, int itemid , BOOL net);
int NPC_Util_CharDistance( int index1, int index2 );
int NPC_Util_SearchNear( int meindex, int maxlen, int type );
int NPC_Util_SearchNearPlayer( int meindex, int maxlen );
int NPC_Util_SearchNearEnemy( int meindex, int maxlen );
int NPC_Util_SuberiWalk( int index,	int	dir );
int NPC_Util_GetNumFromArg( int meindex, char* in);

int NPC_Util_GetDirCharToChar( int fromindex, int toindex, int mode);
int NPC_Util_WalkCharToChar( int fromindex, int toindex, int mode, int suberi);
BOOL NPC_Util_isFaceToFace( int index1, int index2, int distance );
BOOL NPC_Util_isFaceToChara( int index1, int index2, int distance );
BOOL NPC_Util_charIsInFrontOfChar( int index1, int index2, int distance );

int NPC_Util_SearchItemInChar( int charindex , int itemindex);
int NPC_Util_GiveAllItemToChar( int give , int take );
/*
int NPC_Util_ControlOtherNPC( CHAR_TYPE chartype ,
                              char *npcname,
                              char *command );
*/
void NPC_Util_NPCDelete( int srcindex );
BOOL NPC_Util_moveItemToMap( int itemindex , int fl , int x , int y,
                             BOOL net );
char *NPC_Util_GetArgStr( int index, char *argstr, int len);
int NPC_Util_GetNumFromStrWithDelim( char *srcstr, char* in);
char *NPC_Util_GetStrFromStrWithDelim( char *srcstr, char *srhstr,
										char *buf, int len);
inline double NPC_Util_sellRate( int seller );
inline double NPC_Util_buyRate( int buyer );
BOOL NPC_Util_IsVisiblePlayer( int meindex);
BOOL NPC_Util_WordInclude( char *text , char *word );
void NPC_Util_RandomToken(char *in, char *out, int outbufsize );

void cutDotsTail( char *s );
int NPC_Util_FrontItem( int meindex );
void NPC_Util_Boss2KingStart( int bossindex );
int NPC_Util_FrontChar( int meindex );
int *NPC_Util_getEnemy( int meindex, int charaindex);

void  NPC_NowEndEventSetFlgCls(int talker,int shiftbit);
void  NPC_EventSetFlg(int talker,int shiftbit);
BOOL NPC_EventCheckFlg(int point,int shiftbit);
void  NPC_NowEventSetFlg(int talker,int shiftbit);
void  NPC_NowEventSetFlgCls(int talker,int shiftbit);
BOOL NPC_NowEventCheckFlg(int point,int shiftbit);
char *NPC_Util_CheckAssignArgFile( int index, char *filename);

// CoolFish: Family Adv 2001/8/4
void AddFMAdv(int talker, int shiftbit);

// Robin 0817 family income
int addNpcFamilyTax( int meindex, int talkerindex, int income );

#define		NPC_ENEMY_ENEMYNUMBER		10		/* ÇëòØ³Ä    ÐÑ */

/* Â¦ÐÑ°×ÑëÄÌ»ï  ÐÄ  ÐÄ¼°¨àµæ¼°  Ù¯ÐÑÏþò× */
#define NPC_UTIL_GETARGSTR_LINEMAX	4096
/* Â¦ÐÑ°×ÑëÄÌ»ï¼°  ÐÄ  ÐÄÌïÓÀ°×ÑëÈÓÄÌÊõ*/
#ifdef _NEWEVENT
//#define NPC_UTIL_GETARGSTR_BUFSIZE	1024*1200
#define NPC_UTIL_GETARGSTR_BUFSIZE	1024*32
#else
#define NPC_UTIL_GETARGSTR_BUFSIZE	1024*12
#endif
#endif



