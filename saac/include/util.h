#ifndef _UTIL_H_
#define _UTIL_H_
int strcatsafe( char *dest, int destlen , char *append );
void prepareDirectories(char *base);
void easyGetTokenFromString( char *src,int count,char*output,int len );
void remove_r( char *s );
void makeDirFilename( char *out , int outlen ,
                  char *base , int dirchar , char *child );
int isFile( char *fn );
int createFile( char *fn , char *line );

char *   makeStringFromEscaped( char* src );
char *   makeEscapeString( char* src , char* dest, int sizeofdest);
char *   makeEscapeString1( char* src , char* dest, int sizeofdest);
int getHash ( char* s );
char * chop( char *s );
int hashpjw ( char* s );

// CoolFish: Family 2001/5/30
//void easyGetTokenFromBuf(char *src, char delim, int count, char*output, int len);

int easyGetTokenFromBuf( char* src ,char* delim ,int count, char* output , int len );
#endif /* ifndef _UTIL_H_ */








