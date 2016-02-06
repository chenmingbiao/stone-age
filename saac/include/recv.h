#ifndef _RECV_H_
#define _RECV_H_
#include "acfamily.h"
#ifdef _SEND_EFFECT		   // WON ADD 送下雪、下雨等特效
void SendEffect(char *ffect);
#endif

#ifdef _AC_SEND_FM_PK		 // WON ADD 庄园对战列表储存在AC
void load_fm_pk_list(void);
void save_fm_pk_list(void);
char fm_pk_list[FMPKLIST_MAXNUM][255];
#endif

#ifdef _ALLDOMAN // Syu ADD 排行榜NPC
void LOAD_herolist(); 
void SAVE_herolist(int);
void Send_A_herolist(int);

void Send_S_herolist( char *ocdkey , char *oname , char *ncdkey , char *nname , 
					 char *title , int level , int trns , int floor );
#define MAX_HERO_COLUMN 7
#define MAX_HERO_LIST 100
char Herolist[MAX_HERO_LIST][MAX_HERO_COLUMN][72];
#endif

typedef struct tagUNLOCKMENUS{
	char PlayerId[256];
	int use;
	time_t time;
}UNLockMenus;

int UNlockM_isBe( char *id);
int UNlockM_addPlayer( char *id);
int UNlockM_UnlockPlayer( void);
int UNlockM_Init( void);

#endif

