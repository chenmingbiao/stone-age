#ifndef _SAACPROTOUTIL_H_
#define _SAACPROTOUTIL_H_
#include <stdlib.h>
#include <string.h>
#ifndef WIN32
#include <strings.h>
#include <unistd.h>
#endif
#define saacproto__NOENCRYPT
#define saacproto__NODEBUG
struct saacproto_ {
	int (*write_func)(int,char*,int) ;    /* write function */
	size_t workbufsize;                   /* size of work area */
    char  *work,*arraywork,*escapework,*val_str,*ret_work;      /* work areas which have same size  */
	char *cryptwork,*jencodecopy,*jencodeout,*compresswork;     /* these work has bigger size (3times)*/
    char** token_list;                    /* token list */
    unsigned long message_id; /*= 1,  */       /* for generating message IDs */

};
#ifdef _SAACPROTOUTIL_C_
struct saacproto_ saacproto = {
	NULL,
	0,
	NULL,NULL,NULL,NULL,NULL,
	NULL,NULL,NULL,NULL,
	NULL,
	1,
};
char **saacproto_stringwrapper;
char saacproto_readlogfilename[1024];
char saacproto_writelogfilename[1024];
#else
extern char **saacproto_stringwrapper;
extern struct saacproto_ saacproto;
extern char saacproto_readlogfilename[1024];
extern char saacproto_writelogfilename[1024];
#endif
char* saacproto_escapeString( char*a );
char* saacproto_descapeString( char*a );
void saacproto_splitString( char *src  );
void saacproto_strcpysafe( char *dest, char *src, int len );
void saacproto_strcatsafe( char *dest , char *src , int maxlen );
char*saacproto_mkstr_int( int i );
char*saacproto_mkstr_u_int( unsigned int i );
char*saacproto_mkstr_long( long l );
char*saacproto_mkstr_u_long( unsigned long l );
char*saacproto_mkstr_short( short s );
char*saacproto_mkstr_u_short( short s );
char*saacproto_mkstr_char( char c );
char*saacproto_mkstr_u_char( char c);
char*saacproto_mkstr_string( char*a );
char*saacproto_mkstr_float( float f );
char*saacproto_mkstr_double( double d );
char*saacproto_mkstr_int_array( int size , int *array );
char*saacproto_mkstr_u_int_array( int size , int *array );
char*saacproto_mkstr_short_array( int size , short *array );
char*saacproto_mkstr_u_short_array(int size , short *array );
char *saacproto_mkstr_char_array( int size , char *array );
char *saacproto_mkstr_u_char_array( int size , unsigned char *array );
char *saacproto_mkstr_float_array( int size , float *array );
char *saacproto_mkstr_double_array( int size , double *array );
int saacproto_demkstr_int( char*a );
unsigned int saacproto_demkstr_u_int( char*a );
long saacproto_demkstr_long( char*a );
unsigned long saacproto_demkstr_u_long(char*a );
short saacproto_demkstr_short( char*a );
unsigned short saacproto_demkstr_u_short( char*a );
char saacproto_demkstr_char( char*a );
unsigned char saacproto_demkstr_u_char( char*a );
float saacproto_demkstr_float( char*a );
double saacproto_demkstr_double(char*a );
char* saacproto_demkstr_string( char*a);
int *saacproto_demkstr_int_array( char**tk ,int *buf  ,int start , int size );
int *saacproto_demkstr_u_int_array( char **tk , int *buf , int start , int size );
unsigned int *saacproto_demkstr_long_array(
	char **tk , unsigned int *buf , int start , int size );
unsigned long *saacproto_demkstr_u_long_array(
	char **tk , unsigned long *buf , int start , int size );
short *saacproto_demkstr_short_array( char **tk , short *buf , int start , int size );
unsigned short*saacproto_demkstr_u_short_array( 
	char **tk , unsigned short *buf , int start , int size );
char *saacproto_demkstr_char_array( char **tk , char *buf , int start , int size );
unsigned char *saacproto_demkstr_u_char_array( 
	char **tk , unsigned char*buf , int start , int size );
float *saacproto_demkstr_float_array( char **tk , float *buf , int start , int size );
double *saacproto_demkstr_u_double_array( char **tk , double *buf , int start , int size );
char *saacproto_wrapStringAddr( char *copy , int maxcopylen , char*src );

void saacproto_GetMessageInfo( int *id , char *funcname , int len,char **tk );
int saacproto_ClientRead(void);
void saacproto_consumeLine(char *buf , int ofs );
void saacproto_copyLine( char*src , char *out , int maxoutlen );
void saacproto_Send( int fd , char *msg );
int saacproto_AllocateCommonWork(int bufsiz);
int saacproto_StringRest();

// WON ADD
unsigned long saacproto_GetNewMessageID(void);


void saacproto_CreateHeader(char*out, char *fname );
void saacproto_CreateHeaderID( char *out,unsigned long msgid , char *fname );
int saacproto_default_write_wrap( int fd , char *buf , int size );
void saacproto_bzero( char *b , int siz );
void saacproto_bcopy(char*s , char *d , int siz );
char *saacproto_Ltoa( long v );
char *saacproto_Ultoa( unsigned long v );
void saacproto_DebugSend( int fd , char *msg );
/* Modified by ringo to fasten int type transfer */
char *saacproto_cnv10to62( int a, char *out, int outlen );
int saacproto_a62toi( char *a );
extern int JENCODE_KEY;
#endif

