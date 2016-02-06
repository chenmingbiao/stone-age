#ifndef __TITLE_H__
#define __TITLE_H__

#include "common.h"
#include "util.h"

INLINE BOOL TITLE_CHECKTABLEINDEX( int index );
int TITLE_getTitleIndex( int index);

char* TITLE_makeTitleStatusString( int charaindex, int havetitleindex );
char* TITLE_makeSkillFalseString( void );

BOOL TITLE_addtitle( int charaindex, int titleindex );
BOOL TITLE_deltitle( int charaindex, int titleindex );
BOOL TITLE_initTitleName( char* filename );
BOOL TITLE_reinitTitleName( void);
BOOL TITLE_initTitleConfig( char* filename );
BOOL TITLE_TitleCheck( int charaindex, BOOL mode);
BOOL TITLE_TitleCheck_Nomsg( int charaindex, BOOL mode, int *addcnt, int *delcnt);
#endif
 /*__TITLE_H__*/
