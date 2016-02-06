#ifndef __CHAR_TALK_H__
#define __CHAR_TALK_H__

#include "common.h"
#include "util.h"
#include "net.h"
  
void CHAR_getMessageBody(char* message, char* kind, int kindlen,
            char** body);

int CHAR_getChatMagicFuncLevel(char* name,BOOL isDebug);
int CHAR_getChatMagicFuncNameAndString( int ti, char* name, char *usestring, int level, BOOL isDebug);
int CHAR_getChatMagicFuncMaxNum( void);

#endif 
