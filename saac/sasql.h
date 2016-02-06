#ifndef _SASQL_H_
#define _SASQL_H_

#include "version.h"
#ifdef _SASQL

#define BOOL int
#define FALSE 0
#define TRUE 1

void sasql_close( void );
BOOL sasql_init( void );
int sasql_query(char *nm, char *pas);
BOOL sasql_update(char *nm, char *path);
BOOL sasql_logindate(char *nm);
BOOL sasql_register(char *id, char *ps);
BOOL sasql_craete_userinfo( void );
BOOL sasql_craete_lock( void );
BOOL sasql_chehk_lock( char *idip );
BOOL sasql_add_lock( char *idip );
BOOL sasql_del_lock( char *idip );
#endif

#endif
