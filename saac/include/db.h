#ifndef _DB_H_
#define _DB_H_

int dbUpdateEntryInt( char *table , char *key , int value, char *info );
int dbUpdateEntryString( char *table, char *key, char *value );
int dbDeleteEntryInt( char *table, char *key );
int dbDeleteEntryString( char *table, char *key );
int dbGetEntryInt( char *table, char *key, int *output );
int dbGetEntryString( char *table, char *key, char *output, int outlen );
int dbGetEntryRank( char *table, char *key , int *r_out, int *c_out );
int dbGetEntryRankRange( char *table,
                     int start, int end, char *output, int outlen );
int dbGetEntryCountRange( char *table, int count_start, int  num,
                      char *output, int outlen );

int dbRead( char *dir );
int dbFlush( char *dir );

#endif
