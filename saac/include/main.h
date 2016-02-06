#ifndef _MAIN_H_
#define _MAIN_H_

#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>
#include <strings.h>
#include <time.h>
#include <sys/time.h>

#include <errno.h>
#include <string.h>
#include "version.h"

#ifdef _MAIN_C_
#define EXT
#else
#define EXT extern
#endif

#define SA_NODEFER	0x40000000
#define SA_NOMASK	SA_NODEFER

#define CHARDATASIZE ( 1024 * 64 )

char *chartime();
#define LOGBASE( filename , format , args...)\
{\
    FILE *f;\
    f = fopen( filename , "a" );\
    if( f != NULL ){\
        fprintf( f , format , ## args);\
        fclose(f);\
    } else{ fprintf( stderr,"fuckup! log fail!:%s:%s\n", filename,strerror(errno));}\
}

// Spock 2000/10/27
#define log(format, args...)    \
{\
    fprintf( stderr, format , ## args );\
}
// Spock end
#define SUCCESSFUL "successful"
#define FAILED "failed"

#define MAXCONNECTION 10
#define USERID_MAX 32
#define CHARNAME_MAX 32

EXT char svpass[64];
EXT char chardir[64];
EXT char logdir[64];
EXT char dbdir[64];
EXT char maildir[64];

// CoolFish: Family 2001/5/9
EXT char familydir[64];
EXT char fmpointdir[64];
EXT char fmsmemodir[64];

EXT int sameipmun;

EXT int log_rotate_interval;
EXT int total_ok_charlist, total_ng_charlist;


#ifdef _SLEEP_CHAR
EXT char sleepchardir[64];
#endif


typedef struct _gmsv
{
    int use;
	  int fd;
    char name[128];
} gmsv;

int get_rotate_count(void );
void checkGSUCheck( char *id );
int logout_game_server( int ti );
int is_game_server_login( int ti );
char * getGSName( int i );

void gmsvBroadcast( int fd, char *p1, char *p2, char *p3 , int flag );
#ifdef _VIP
int login_game_server( int ti , char *svname , char *svpas , int checkvip,
               char *result , int resultlen ,
               char *retdata , int retdatalen );
#else
int login_game_server( int ti , char *svname , char *svpas ,
               char *result , int resultlen ,
               char *retdata , int retdatalen );
#endif

#undef EXT
#endif





