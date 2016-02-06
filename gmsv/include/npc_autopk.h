#ifndef _AUTO_PK_H
#define _AUTO_PK_H

void NPC_AutoPkTalked( int meindex, int talkerindex, char *msg, int color );
BOOL NPC_AutoPkInit( int meindex );
void NPC_AutoPkWindowTalked( int meindex, int talkerindex, int seqno, int select, char *data);
void NPC_AutoPkLoop( void );
void AutoPk_PKSystemTalk( char *token1, char *token2 );
void AutoPk_PKSystemInfo();
void AutoPk_PKTimeSet( int pktime );
int AutoPk_PKTimeGet( void );
void AutoPk_AwardSet( int petid, char *pet, int itemid, char *item );
void AutoPk_ChampionShipSet( char *cdkey, char *name, int winnum, int win );
void NPC_AUTOPKHealerTalked( int meindex );
void AutoPk_SetHistoryStr( void );
void AutoPk_SetAwardStr( char *token );
BOOL AutoPk_GetAwardStr( void );
void AutoPk_GetChampionShip( void );
#endif
