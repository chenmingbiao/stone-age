#define _LSSPROTOUTIL_C_
#include "version.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifndef WIN32
#include <unistd.h>
#include <strings.h>
#endif
#include "lssproto_util.h"
//ttom +2 for degug
#define IS_2BYTEWORD( _a_ ) ( (char)(0x80) <= (_a_) && (_a_) <= (char)(0xFF) )

#include "common.h"
#ifdef lssproto__ENCRYPT
long lssproto_ringoCompressor( unsigned char *code , long codelen , unsigned char *text , long textlen);
long lssproto_ringoDecompressor( unsigned char *text , long textlen , unsigned char *code , long codelen);
#endif
/*
  lsrpc routines
*/
int lssproto_AllocateCommonWork(int bufsiz)
{
	lssproto.workbufsize = bufsiz;
	lssproto.work = NULL;
	lssproto.arraywork = NULL;
	lssproto.escapework = NULL;
	lssproto.val_str = NULL;
	lssproto.token_list = NULL;
	lssproto.cryptwork = NULL;
	lssproto.jencodecopy = NULL;
	lssproto.jencodeout = NULL;
	lssproto.compresswork = NULL;
	lssproto.work = (char*)calloc( 1, lssproto.workbufsize );
	lssproto.arraywork = (char*)calloc( 1, lssproto.workbufsize );
	lssproto.escapework = (char*)calloc( 1, lssproto.workbufsize );
	lssproto.val_str = (char*)calloc( 1, lssproto.workbufsize );
	lssproto.token_list = (char**)calloc( 1, lssproto.workbufsize *sizeof( char** ) );
	lssproto.cryptwork = (char*)calloc( 1, lssproto.workbufsize * 3 );
	lssproto.jencodecopy = (char*)calloc( 1, lssproto.workbufsize * 3 );
	lssproto.jencodeout = (char*)calloc( 1, lssproto.workbufsize * 3 );
	lssproto.compresswork = (char*)calloc( 1, lssproto.workbufsize * 3 );
	memset( lssproto.work , 0, lssproto.workbufsize );
	memset( lssproto.arraywork , 0, lssproto.workbufsize );
	memset( lssproto.escapework , 0, lssproto.workbufsize );
	memset( lssproto.val_str , 0, lssproto.workbufsize );
	memset( (char*)lssproto.token_list ,0,  lssproto.workbufsize*sizeof(char**) );
	memset( lssproto.cryptwork , 0, lssproto.workbufsize*3 );
	memset( lssproto.jencodecopy , 0, lssproto.workbufsize*3 );
	memset( lssproto.jencodeout , 0, lssproto.workbufsize*3 );
	memset( lssproto.compresswork , 0, lssproto.workbufsize*3 );
	if( lssproto.work == NULL ||
	   lssproto.arraywork == NULL ||
	   lssproto.escapework == NULL ||
	   lssproto.val_str == NULL ||
	   lssproto.token_list == NULL ||
	   lssproto.cryptwork == NULL ||
	   lssproto.jencodecopy == NULL ||
	   lssproto.jencodeout == NULL ||
	   lssproto.compresswork == NULL ){
	    free( lssproto.work);free( lssproto.val_str);
	    free( lssproto.escapework);free( lssproto.arraywork );
	    free( lssproto.token_list);free( lssproto.cryptwork );
	    free( lssproto.jencodecopy);free( lssproto.jencodeout );
	    free( lssproto.compresswork );
	    return -1;
	}
	return 0;
}
/**********
  Get message information from a network input
**********/
void lssproto_GetMessageInfo( int *id , char *funcname , int len, char **tk )
{
	if( tk[0] == NULL || tk[1] == NULL ){
		*id = 0;
		lssproto_strcpysafe( funcname , "" , len );
		return;
	}
	*id =  strtoul( tk[0] ,NULL,10);
	lssproto_strcpysafe( funcname , tk[1] , len );
	return;
}
/********************************************
  string utilities
*********************************************/
void lssproto_strcpysafe( char *dest, char *src, int maxlen )
{
	int i;
	for(i=0;i<maxlen-1;i++){
		dest[i] = src[i];
		if( src[i]==0)break;
	}
	dest[i]=0;
}
void lssproto_strcatsafe( char *dest , char *src , int maxlen )
{
	int i,j;
	for(i=0;i<maxlen-1;i++){
		if( dest[i] == 0 ){
			for(j=i;j<maxlen-1;j++){
				dest[j]= src[j-i];
				if( src[j-i] == 0 )break;
			}
			dest[j]=0;
			break;
		}
	}
}
char*lssproto_mkstr_int( int i )
{
#define MKSTR_INT(v) lssproto_Ltoa( (long)(v)) 
	lssproto_strcpysafe( lssproto.val_str ,  (char*)MKSTR_INT(i) , lssproto.workbufsize );
	lssproto_strcatsafe( lssproto.val_str ," " , lssproto.workbufsize );
	return lssproto.val_str;
}
char*lssproto_mkstr_u_int( unsigned int i )
{
#define MKSTR_U_INT(v) lssproto_Ultoa( (unsigned long ) (v) )
	lssproto_strcpysafe( lssproto.val_str , MKSTR_U_INT(i) , lssproto.workbufsize );
	lssproto_strcatsafe( lssproto.val_str ," " , lssproto.workbufsize );
	return lssproto.val_str;
}
char*lssproto_mkstr_long( long l )
{
#define MKSTR_LONG(v) lssproto_Ltoa( v ) 
	lssproto_strcpysafe( lssproto.val_str , MKSTR_LONG(l) , lssproto.workbufsize );
	lssproto_strcatsafe( lssproto.val_str ," " , lssproto.workbufsize );
	return lssproto.val_str;
}
char*lssproto_mkstr_u_long( unsigned long l )
{
#define MKSTR_U_LONG(v) lssproto_Ultoa(v ) 
	lssproto_strcpysafe( lssproto.val_str , MKSTR_U_LONG(l) , lssproto.workbufsize );
	lssproto_strcatsafe( lssproto.val_str ," " , lssproto.workbufsize );
	return lssproto.val_str;	
}
char*lssproto_mkstr_short( short s )
{
#define MKSTR_SHORT16(v)    lssproto_Ltoa( (long)((int)(v) ))
	lssproto_strcpysafe( lssproto.val_str , MKSTR_SHORT16(s) , lssproto.workbufsize );
	lssproto_strcatsafe( lssproto.val_str ," " , lssproto.workbufsize );
	return lssproto.val_str;	
}
char*lssproto_mkstr_u_short( short s )
{
#define MKSTR_U_SHORT16(v)   lssproto_Ltoa( (long)(  ((long)(v)) & 0xffff ))
	lssproto_strcpysafe( lssproto.val_str , MKSTR_U_SHORT16(s) , lssproto.workbufsize );
	lssproto_strcatsafe( lssproto.val_str ," " , lssproto.workbufsize );
	return lssproto.val_str;	
}
char*lssproto_mkstr_char( char c )
{
#define MKSTR_CHAR8(v)   lssproto_Ltoa( ( long)((int)(v)))
	lssproto_strcpysafe( lssproto.val_str , MKSTR_CHAR8(c) , lssproto.workbufsize );
	lssproto_strcatsafe( lssproto.val_str ," ", lssproto.workbufsize );
	return lssproto.val_str;	
}
char*lssproto_mkstr_u_char( char c)
{
#define MKSTR_U_CHAR8(v)   lssproto_Ltoa( (long)( ((long)(v)) & 0xff ))
	lssproto_strcpysafe( lssproto.val_str , MKSTR_U_CHAR8(c) , lssproto.workbufsize );
	lssproto_strcatsafe( lssproto.val_str ," "  , lssproto.workbufsize );
	return lssproto.val_str;
}
char*lssproto_mkstr_string( char*a )
{
	char *ret = lssproto_escapeString( a );
	lssproto_strcatsafe( ret , " ", lssproto.workbufsize );
	return ret;
}
char*lssproto_mkstr_float(float f )
{
	sprintf( lssproto.val_str , "%f " , f );
	return lssproto.val_str;
}
char*lssproto_mkstr_double( double d )
{
	sprintf( lssproto.val_str , "%f " , d );
	return lssproto.val_str;
}
char*lssproto_mkstr_int_array( int size , int *array )
{
#define MKSTR_ARRAYMACRO( func) \
	{\
		 int i;\
         lssproto.arraywork[0] = '\0';\
		 for(i=0;i<size;i++){   \
             lssproto_strcatsafe( lssproto.arraywork , func(array[i]) , lssproto.workbufsize );\
		 }\
         return lssproto.arraywork;\
    }\

	MKSTR_ARRAYMACRO( lssproto_mkstr_int );
}
char*lssproto_mkstr_u_int_array( int size , int *array )
{
	MKSTR_ARRAYMACRO( lssproto_mkstr_u_int );
}
char*lssproto_mkstr_short_array( int size , short *array )
{
	MKSTR_ARRAYMACRO( lssproto_mkstr_short );
}
char*lssproto_mkstr_u_short_array( int size , short *array )
{
	MKSTR_ARRAYMACRO( lssproto_mkstr_u_short );
}
char *lssproto_mkstr_char_array( int size , char *array )
{
	MKSTR_ARRAYMACRO( lssproto_mkstr_char );
}
char *lssproto_mkstr_u_char_array( int size , unsigned char *array )
{
	MKSTR_ARRAYMACRO( lssproto_mkstr_u_char );
}
char *lssproto_mkstr_float_array( int size , float *array )
{
	MKSTR_ARRAYMACRO( lssproto_mkstr_float );
}
char *lssproto_mkstr_double_array( int size , double *array )
{
	MKSTR_ARRAYMACRO( lssproto_mkstr_double );
}
/*********************
   translate string into base types
*********************/
int lssproto_demkstr_int( char*a )
{
	if( a == (char*)NULL)return 0;
/*	return (int)strtol( a ,NULL , 10);*/
	return lssproto_a62toi( a );
}
unsigned int lssproto_demkstr_u_int( char*a )
{
	if( a == (char*)NULL)return 0;
	return (unsigned int ) strtoul( a ,NULL,10);
}
long lssproto_demkstr_long( char*a )
{
	if( a == (char*)NULL)return 0;
	return (long)strtol( a , NULL , 10 );
}
unsigned long lssproto_demkstr_u_long(char*a )
{
	if( a == (char*)NULL)return 0;
	return (unsigned long ) strtoul( a , NULL , 10 ) ;
}
short lssproto_demkstr_short( char*a )
{
	if( a == (char*)NULL)return 0;
	return (short) strtol( a , NULL , 10 );
}
unsigned short lssproto_demkstr_u_short( char*a )
{
	if( a == (char*)NULL)return 0;
	return (unsigned short) strtoul( a , NULL , 10 );
}
char lssproto_demkstr_char( char*a )
{
	if( a == (char*)NULL)return 0;
	return (char) strtol(  a  , NULL , 10 );
}
unsigned char lssproto_demkstr_u_char( char*a )
{
	if( a == (char*)NULL)return 0;
	return (unsigned char ) strtoul( a,NULL , 10 );
}
float lssproto_demkstr_float( char*a )
{
	if( a == (char*)NULL)return 0.0F;
	return (float) atof( a);
}
double lssproto_demkstr_double(char*a )
{
	if( a == (char*)NULL)return 0.0F;
	return (double) strtod( a , NULL );
}
char* lssproto_demkstr_string( char*a)
{
	if( a == (char*)NULL){
		lssproto_strcpysafe( lssproto.escapework , "" , lssproto.workbufsize );
		return lssproto.escapework;
	}
	return lssproto_descapeString( a );
}
int *lssproto_demkstr_int_array( char**tk ,int *buf  ,int start , int size )
{
#define DEMKSTR_ARRAYMACRO( func, defaultvalue )          \
    {\
        int i;\
        for(i=start;i<(start+size);i++){\
            if( tk[i] == NULL ){   \
		        buf[i-start]=defaultvalue ;\
            } else {\
     	 	    buf[i-start] = func( tk[i] );\
            }\
	    }\
        return buf;\
    }
    DEMKSTR_ARRAYMACRO( lssproto_demkstr_int , 0 );
}
int *lssproto_demkstr_u_int_array( char **tk , int *buf , int start , int size )
{
	DEMKSTR_ARRAYMACRO( lssproto_demkstr_u_int , 0 );
}
unsigned int *lssproto_demkstr_long_array(
 char **tk , unsigned int *buf , int start , int size )
{
	DEMKSTR_ARRAYMACRO( lssproto_demkstr_long  , 0);
}
unsigned long *lssproto_demkstr_u_long_array(
 char **tk , unsigned long *buf , int start , int size )
{
	DEMKSTR_ARRAYMACRO( lssproto_demkstr_u_long , 0);
}
short *lssproto_demkstr_short_array( char **tk , short *buf , int start , int size )
{
	DEMKSTR_ARRAYMACRO( lssproto_demkstr_short , 0);
}
unsigned short* lssproto_demkstr_u_short_array( 
 char **tk , unsigned short *buf , int start , int size )
{
	DEMKSTR_ARRAYMACRO( lssproto_demkstr_u_short , 0);
}
char *lssproto_demkstr_char_array( char **tk , char *buf , int start , int size )
{
	DEMKSTR_ARRAYMACRO( lssproto_demkstr_u_char , 0);
}
unsigned char *lssproto_demkstr_u_char_array( 
char **tk , unsigned char*buf , int start , int size )
{
	DEMKSTR_ARRAYMACRO( lssproto_demkstr_u_char , 0);
}
float *lssproto_demkstr_float_array( char **tk , float *buf , int start , int size )
{
	DEMKSTR_ARRAYMACRO( lssproto_demkstr_float , (float)0.0);
}
double *lssproto_demkstr_u_double_array( char **tk , double *buf , int start , int size )
{
	DEMKSTR_ARRAYMACRO( lssproto_demkstr_double , (double)0.0);
}
char*  lssproto_escapeString( char*a )
{
	int i,c=0;
	lssproto.escapework[0] = '\0';
	for(i=0;;i++){
		if( a[i] == '\0' ){
			lssproto.escapework[c++] = '\0';
			break;
		} else if( ( char )0x80 <= a[i] && a[i] <= ( char )0xFF ){
		        // for 2 Byte Word
		        lssproto.escapework[c++] = a[i++];
		        lssproto.escapework[c++] = a[i];
		} else if( a[i] == '\\' ){
			lssproto.escapework[c++] = '\\';
			lssproto.escapework[c++] = '\\';
		} else if( a[i] == ' ' ){
			lssproto.escapework[c++] = '\\';
			lssproto.escapework[c++] = 'S';
		} else if( a[i] == '\n' ){
			lssproto.escapework[c++] = '\\';
			lssproto.escapework[c++] = 'n';
		} else if( a[i] == '\r' ){
			lssproto.escapework[c++] = '\\';
			lssproto.escapework[c++] = 'r';
		} else {
			lssproto.escapework[c++] = a[i];
		}
	}
	return lssproto.escapework;
}
char* lssproto_descapeString( char*a )
{
	int i ,c =0;
	lssproto.escapework[0] = '\0';
	for(i=0;;i++){
		if( a[i] == '\0' ){
			lssproto.escapework[c++]='\0';
			break;
		} else if( (char)0x80 <= a[i] && a[i] <= (char)0xFF ){
		        // for 2 Byte Word
		        lssproto.escapework[c++] = a[i++];
		        lssproto.escapework[c++] = a[i];
		} else if( a[i] == '\\' ){
		        if( a[i+1] == 0 ){     /* null */
		                lssproto.escapework[c++] = a[i];
		                continue;
		        }
		        if( a[i+1] == 'S' ){     /* space */
				lssproto.escapework[c++] = ' ';
			} else if( a[i+1] == 'n' ){
				lssproto.escapework[c++] = '\n';
			} else if( a[i+1] == 'r' ){
				lssproto.escapework[c++] = '\r';
			} else if( a[i+1] == '\\' ){
				lssproto.escapework[c++] = '\\';
			} else {
				lssproto.escapework[c++] = a[i];
			}
			i++;           
		} else {
			lssproto.escapework[c++] = a[i];
		}
	}
	return lssproto.escapework;
}
/*
   This function works only when char*src is escaped
   NOTICE: Effects and Modifies the contents of char*src!
   NOTICE : Ends the output token list with NULL pointer
Ex:
        v out[0]       v out[1]
  "     asdjfhasfdasdf asdf asf asdf "
 */
#ifdef lssproto__ENCRYPT
static void lssproto_decodeString( char *src , char *out );
static void lssproto_encodeString( char *src , char *out , int maxoutlen );
#endif
void lssproto_splitString( char *src  )
{
	int i,c=0;
	char *decoded;
	
#ifdef lssproto__ENCRYPT
	decoded = lssproto.cryptwork;
	lssproto_decodeString( src , decoded );
#else
	decoded = src;
#endif


	if( lssproto_readlogfilename[0] != '\0'){
		FILE *rfp;
		rfp = fopen( lssproto_readlogfilename , "a+" );
		if(rfp)fprintf( rfp, "%s\n",decoded );
		if(rfp)fclose(rfp);
	}
	for(i = 0 ; ; i++ ){
		if( decoded[i] == '\0' ) break;
		if( i==0){
		    lssproto.token_list[c++]=&(decoded[i]);
		}
		if( decoded[i]== ' '){
		    lssproto.token_list[c++]=&(decoded[i+1]);
		}
	}
	while( *decoded ){
		if( ( *decoded  ) == ' ' || (*decoded ) == '\n' || (*decoded ) == '\r' ) *decoded = '\0';
		decoded++;
 	}
	lssproto.token_list[c] = (char*)NULL;
}
int lssproto_default_write_wrap( int fd , char *buf , int size )
{
#ifndef WIN32
	return write( fd , buf , size );
#else
	return 0;
#endif
}

void lssproto_consumeLine(char *buf , int offset )
{
	int i;
	int shift=0;
	buf+= offset;
	for(i=0;;i++){
		if( buf[i] == '\n' ){
			shift = i + 1;
			break;
		}
	}
    if( shift == 0 )return;
	for(i=shift;;i++){
		buf[i - shift] = buf[i];
		if( buf[i] == '\0')break;
	}
}
void lssproto_copyLine( char*src , char *out , int outlen )
{
	int i;
	for(i=0;;i++){
		out[i] = src[i];
		if( src[i] == '\n' ){
			out[i+1] = '\0' ;
			return;
		}
		if( src[i] == '\0' )return;
	}
	lssproto_strcpysafe( out , "" , outlen );
}
unsigned int lssproto_GetNewMessageID(void)
{
	return lssproto.message_id++;
}
/*****************
  WRITE
   int flg : if 1, actually write. Otherwise no Network access
*****************/
void lssproto_DebugSend( int fd , char *msg )
{
	lssproto_Send( fd, msg );
}
void lssproto_Send( int fd , char *msg )
{
    char *encoded;

	if( lssproto_writelogfilename[0] != '\0' ){
		FILE *wfp = fopen( lssproto_writelogfilename , "a+" );
		if(wfp)fprintf( wfp , "%s\n", msg );
		if(wfp)fclose(wfp);
		
	}
	
#ifdef lssproto__ENCRYPT
    encoded = lssproto.cryptwork;
    lssproto_encodeString( msg , encoded , lssproto.workbufsize*3 );
#else
    encoded = msg;
#endif
	{
		/* add a newline character*/
		unsigned int l = strlen( encoded );
		if( l < lssproto.workbufsize *3){
			encoded[l] = '\n';
			encoded[l+1] = 0;
			l++;
		}
		else
		{
			print("\n lssproto.workbufsize:%d len:%d ", lssproto.workbufsize, l);
		}
		lssproto.write_func( fd , encoded , l);
	}
}
/****************
  create a header which has function name and new Message ID
****************/
void lssproto_CreateHeader( char *out ,char *fname )
{
	sprintf( out ,"%u %s " , lssproto_GetNewMessageID() , fname );
}
void lssproto_CreateHeaderID( char *out,unsigned long msgid , char *fname )
{
	sprintf( out ,"%u %s " , (unsigned int)msgid , fname );
}
char *lssproto_Ltoa( long v )
{
	static char _ltoa_out[64];
	lssproto_cnv10to62( (int)v , _ltoa_out , sizeof( _ltoa_out ));
/*	sprintf( _ltoa_out , "%d" , (int)v );*/
	return _ltoa_out;
}
char *lssproto_Ultoa( unsigned long v )
{
	static char _ultoa_out[64];
	sprintf( _ultoa_out , "%u" , (unsigned int)v );
	return _ultoa_out;
}
/****************
   string address wrapper
****************/
char *lssproto_wrapStringAddr( char *copy , int maxcopylen , char*src )
{
	lssproto_strcpysafe( copy , src , maxcopylen );
	return copy;
}
/***************
 bzero buffer ( some OSs like win32 don't have bzero )
***************/
void lssproto_bzero( char *b , int siz )
{
	unsigned int i;
	int *p;
	p = (int*)b;
	for(i=0;i<siz/sizeof(int);i++) {
		*(p+i)=0;
	}
	for(i=0;i<siz%sizeof(int);i++) {
		*(b+siz-1-i)=0;
	}
}
/***************
    copy buffer
***************/
void lssproto_bcopy(char*s , char *d , int siz )
{
    unsigned int i;
    int *ps,*pd;
    ps = (int*)s;
    pd = (int*)d;
    for(i=0;i<siz/sizeof(int);i++) {
        *(pd+i) = *(ps+i);
    }
    for(i=0;i<siz%sizeof(int);i++) {
        *(d+siz-1-i)=*(s+siz-1-i);
    }
}
//#define JENCODE_KEY    9932 
int JENCODE_KEY=9922;
#ifdef lssproto__ENCRYPT
/* define function body only if the macro is set( but it's default) */
static void lssproto_encode64( unsigned char *in , int i, unsigned char *out );
static int lssproto_decode64( unsigned char *in , unsigned char *out );
static void lssproto_jDecode(char *src,int srclen,int key,char *decoded,int *decodedlen);
static void lssproto_jEncode(char *src,int srclen,int key,char *encoded,int *encodedlen,int maxencodedlen);
/* translate original lsrpc text to code64 text */
static void lssproto_encodeString( char *src , char *out , int maxoutlen )
{
    int jencodedlen=0;
    long compressed_l = 0;
	int srclen = strlen( src ) + 1;
    int flag=srclen;
	if( srclen < 100 ){
		if( (int)srclen > (int)( lssproto.workbufsize*3-2) ){
			fprintf( stderr, "lsgen: badly configured work buflen\n" );
			exit(1);
		}
		if( (flag%2) == 1 ) flag ++;
		lssproto.compresswork[0] = flag;
		memcpy( lssproto.compresswork+1,src,srclen );
		compressed_l = srclen + 1;
	} else {
		if((flag%2)==0)flag++;
		lssproto.compresswork[0] = flag;
        compressed_l = lssproto_ringoCompressor(
            (unsigned char*)lssproto.compresswork + 1 ,
		    (long)lssproto.workbufsize*3 - 1,
            (unsigned char*)src ,
           (long)strlen(src) ) + 1;    /* be careful! */
	}
	/* return empty line if error or buffer excess */
	if( compressed_l <= 0 ){
		lssproto_strcpysafe( out , "\n" , maxoutlen );
		return;
	}
    memcpy( lssproto.jencodecopy ,lssproto.compresswork ,compressed_l );
    lssproto_jEncode( lssproto.jencodecopy , compressed_l , JENCODE_KEY ,
	    lssproto.jencodeout, &jencodedlen , lssproto.workbufsize*3 -1 );
    lssproto_encode64( (unsigned char*)lssproto.jencodeout , jencodedlen,  (unsigned char*)out );
}
/* translate code64 text to original lsrpc text */
static void lssproto_decodeString( char *src , char *out )
{
    int compressed_l =0, outlen64;
    int l;
    long decompressed_l = 0;
    /* copy src to copybuffer because jencoder modifies the input buffer */
    l = strlen( src );
    if( src[l-1]=='\n' || src[l-1]=='\r' )src[l-1]=0;
    if( src[l-2]=='\n' || src[l-2]=='\r' )src[l-2]=0;
    outlen64 = lssproto_decode64( (unsigned char*)src , (unsigned char*)lssproto.jencodecopy );
    lssproto_jDecode( lssproto.jencodecopy , outlen64 , JENCODE_KEY,
			lssproto.compresswork , &compressed_l);
    /*out[outlen]=0;  PENDING*/
    if( (lssproto.compresswork[0] % 2 ) == 0 ){
	    if( compressed_l <= 0 ){
			decompressed_l = 0;
			fprintf( stderr, "LSRPC: too short:[%s]\n", src );
		} else {
			memcpy( out, lssproto.compresswork+1, compressed_l -1 );
			decompressed_l = compressed_l -1;
		}
	} else {
        decompressed_l =
			lssproto_ringoDecompressor( (unsigned char*)out ,
                  	(long)lssproto.workbufsize ,
					(unsigned char*)lssproto.compresswork+1 ,
					(long)compressed_l -1 );
	}
    out[decompressed_l] = 0;
}
/* followings are taken from code64.c */
char lssproto_charset[64]={
    'A','B','C','D',    'E','F','G','H',
    'I','J','K','L',    'M','N','O','P',
    'Q','R','S','T',    'U','V','W','X',
    'Y','Z','a','b',    'c','d','e','f',
    'g','h','i','j',    'k','l','m','n',
    'o','p','q','r',    's','t','u','v',
    'w','x','y','z',    '0','1','2','3',
    '4','5','6','7',    '8','9','+','-'
};
char lssproto_reversecharset[256]={
    0,0,0,0, 0,0,0,0,
    0,0,0,0, 0,0,0,0,
    0,0,0,0, 0,0,0,0,
    0,0,0,0, 0,0,0,0,
    0,0,0,0, 0,0,0,0,
    0,0,0,62, 0,63,0,0,
    52,53,54,55, 56,57,58,59,
    60,61,0,0, 0,0,0,0,
    0,0,1,2, 3,4,5,6,
    7,8,9,10, 11,12,13,14,
    15,16,17,18, 19,20,21,22,
    23,24,25,0, 0,0,0,0,
    0,26,27,28, 29,30,31,32,
    33,34,35,36, 37,38,39,40,
    41,42,43,44, 45,46,47,48,
    49,50,51,0, 0,0,0,0,
    0,0,0,0,  0,0,0,0,
    0,0,0,0,  0,0,0,0,
    0,0,0,0,  0,0,0,0,
    0,0,0,0,  0,0,0,0,
    0,0,0,0,  0,0,0,0,
    0,0,0,0,  0,0,0,0,
    0,0,0,0,  0,0,0,0,
    0,0,0,0,  0,0,0,0,
    0,0,0,0,  0,0,0,0,
    0,0,0,0,  0,0,0,0,
    0,0,0,0,  0,0,0,0,
    0,0,0,0,  0,0,0,0,
    0,0,0,0,  0,0,0,0,
    0,0,0,0,  0,0,0,0,
    0,0,0,0,  0,0,0,0,
    0,0,0,0,  0,0,0,0
};

static void lssproto_encode64( unsigned char *in , int len , unsigned char *out )
{
    int i;
    int use_bytes;
    int address = 0;
    out[0] = 0;
    for(i=0;;i+=3){
        unsigned char in1 , in2 , in3;
        unsigned char out1 ,out2 , out3 , out4;
        if( i >= len ) break;
        if( i >= (len-1)){   /* the last letter ( to be thrown away ) */
            in1 = in[i] & 0xff;
            in2 = in3 = 0;
            use_bytes = 2;
        } else if( i >= (len-2)){ /* the last 2 letters ( process only 1 byte)*/
            in1 = in[i] & 0xff;
            in2 = in[i+1] & 0xff;
            in3 = 0;
            use_bytes = 3;
        } else {                /* there are more or equal than 3 letters */
            in1 = in[i] & 0xff;
            in2 = in[i+1] & 0xff;
            in3 = in[i+2] & 0xff;
            use_bytes = 4;
        }
        out1 = ((in1 & 0xfc)>>2) & 0x3f;
        out2 = ((in1 & 0x03)<<4) | ((( in2 & 0xf0)>>4)&0x0f);
        out3 = ((in2 & 0x0f)<<2) | ((( in3 & 0xc0)>>6)&0x03);
        out4 = (in3 & 0x3f );
        if( use_bytes >= 2 ){
            out[address++] = lssproto_charset[out1];
            out[address++] = lssproto_charset[out2];
            out[address]=0;
        }
        if( use_bytes >= 3 ){
            out[address++] = lssproto_charset[out3];
            out[address]=0;
        }
        if( use_bytes >= 4 ){
            out[address++] = lssproto_charset[out4];
            out[address]=0;
        }
    }
}
/*
 * Decode it
 * char *in : encoded ascii chars
 * char *out : decoded(  output)
 * return value : output byte count
 * 
 * note: no need to have bigger buffer. because output is to 
 * be smaller than input string size 
 */
static int lssproto_decode64( unsigned char *in , unsigned char *out )
{
    unsigned char in1 , in2 , in3 , in4;
    unsigned char out1 , out2 , out3;
    int use_bytes;
    int address= 0;
    int i;
    for(i=0;;i+=4 ){
        if( in[i] == 0 ){
            break;
        } else if( in[i+1] == 0 ){   /* the last letter */
            break;
        } else if( in[i+2] == 0 ){   /* the last 2 letters */
            in1 = lssproto_reversecharset[in[i]];
            in2 = lssproto_reversecharset[in[i+1]];
            in3 = in4 = 0;
            use_bytes = 1;
        } else if( in[i+3] == 0 ){   /* the last  3 letters */
            in1 = lssproto_reversecharset[in[i]];
            in2 = lssproto_reversecharset[in[i+1]];
            in3 = lssproto_reversecharset[in[i+2]];
            in4 = 0;
            use_bytes = 2;
        } else {   /* process 4 letters */
            in1 = lssproto_reversecharset[in[i]];
            in2 = lssproto_reversecharset[in[i+1]];
            in3 = lssproto_reversecharset[in[i+2]];
            in4 = lssproto_reversecharset[in[i+3]];
            use_bytes = 3;
        }
        out1 =  (in1<<2) | (((in2 & 0x30)>>4)&0x0f) ;
        out2 =  ((in2 & 0x0f )<<4) | ((( in3 & 0x3c)>>2)&0x0f);
        out3 =  ( (in3 &0x03)<<6) |  ( in4 & 0x3f );
        if( use_bytes >= 1 ){
            out[address++] = out1;
        }
        if( use_bytes >= 2 ){
            out[address++] = out2;
        }
        if( use_bytes >= 3 ){
            out[address++] = out3;
        }
        if( use_bytes != 3 ){
            break;
        }
    }
    return address;
}

/* followings are taken from Jencode.c by jun */
static void lssproto_jEncode(char *src,int srclen,int key,char *encoded,int *encodedlen,int maxencodedlen)
{
	char sum=0;
	int i;
	if(srclen+1 > maxencodedlen){
		*encodedlen = maxencodedlen;
		for(i=0;i<(*encodedlen);i++)encoded[i] = src[i];
	}
	if(srclen+1 <= maxencodedlen){
		*encodedlen=srclen+1;
		for(i=0;i<srclen;i++){
			sum = sum + src[i];
			if(((key%7) == (i%5))||((key%2) == (i%2))) src[i] = ~src[i];
		}
		for(i=0;i<(*encodedlen);i++){
			if(abs((key%srclen)) > i) encoded[i] = src[i] + sum*((i*i)%3); 
			if(abs((key%srclen)) == i) encoded[i] = sum;
			if(abs((key%srclen)) < i) encoded[i] = src[i-1] + sum*((i*i)%7);	
		}
	}
}
static void lssproto_jDecode(char *src,int srclen,int key,char *decoded,int *decodedlen)
{
	char sum=0;
	int i;
	*decodedlen=srclen-1;
	if( *decodedlen == 0 ){
		return;		/* return error if length is 0 */
	}
	sum = src[abs(key%(*decodedlen))];
	for(i=0;i<srclen;i++){
		if(abs((key%(*decodedlen))) > i) decoded[i] = src[i] - sum*((i*i)%3);
		if(abs((key%(*decodedlen))) < i) decoded[i-1] = src[i] - sum*((i*i)%7);
	}
	for(i=0;i<(*decodedlen);i++){
		if(((key%7) == (i%5))||((key%2) == (i%2)))decoded[i] = ~decoded[i];
	}
}

/*****************************************************************/
/*       Compress / Decompress routine                           */
/*****************************************************************/	
#define B00000000 0
#define B00000001 1
#define B00000010 2
#define B00000011 3
#define B00000100 4
#define B00000101 5
#define B00000110 6
#define B00000111 7
#define B00001000 8
#define B00001001 9
#define B00001010 10
#define B00001011 11
#define B00001100 12
#define B00001101 13
#define B00001110 14
#define B00001111 15
#define B00010000 16
#define B00010001 17
#define B00010010 18
#define B00010011 19
#define B00010100 20
#define B00010101 21
#define B00010110 22
#define B00010111 23
#define B00011000 24
#define B00011001 25
#define B00011010 26
#define B00011011 27
#define B00011100 28
#define B00011101 29
#define B00011110 30
#define B00011111 31
#define B00100000 32
#define B00100001 33
#define B00100010 34
#define B00100011 35
#define B00100100 36
#define B00100101 37
#define B00100110 38
#define B00100111 39
#define B00101000 40
#define B00101001 41
#define B00101010 42
#define B00101011 43
#define B00101100 44
#define B00101101 45
#define B00101110 46
#define B00101111 47
#define B00110000 48
#define B00110001 49
#define B00110010 50
#define B00110011 51
#define B00110100 52
#define B00110101 53
#define B00110110 54
#define B00110111 55
#define B00111000 56
#define B00111001 57
#define B00111010 58
#define B00111011 59
#define B00111100 60
#define B00111101 61
#define B00111110 62
#define B00111111 63
#define B01000000 64
#define B01000001 65
#define B01000010 66
#define B01000011 67
#define B01000100 68
#define B01000101 69
#define B01000110 70
#define B01000111 71
#define B01001000 72
#define B01001001 73
#define B01001010 74
#define B01001011 75
#define B01001100 76
#define B01001101 77
#define B01001110 78
#define B01001111 79
#define B01010000 80
#define B01010001 81
#define B01010010 82
#define B01010011 83
#define B01010100 84
#define B01010101 85
#define B01010110 86
#define B01010111 87
#define B01011000 88
#define B01011001 89
#define B01011010 90
#define B01011011 91
#define B01011100 92
#define B01011101 93
#define B01011110 94
#define B01011111 95
#define B01100000 96
#define B01100001 97
#define B01100010 98
#define B01100011 99
#define B01100100 100
#define B01100101 101
#define B01100110 102
#define B01100111 103
#define B01101000 104
#define B01101001 105
#define B01101010 106
#define B01101011 107
#define B01101100 108
#define B01101101 109
#define B01101110 110
#define B01101111 111
#define B01110000 112
#define B01110001 113
#define B01110010 114
#define B01110011 115
#define B01110100 116
#define B01110101 117
#define B01110110 118
#define B01110111 119
#define B01111000 120
#define B01111001 121
#define B01111010 122
#define B01111011 123
#define B01111100 124
#define B01111101 125
#define B01111110 126
#define B01111111 127
#define B10000000 128
#define B10000001 129
#define B10000010 130
#define B10000011 131
#define B10000100 132
#define B10000101 133
#define B10000110 134
#define B10000111 135
#define B10001000 136
#define B10001001 137
#define B10001010 138
#define B10001011 139
#define B10001100 140
#define B10001101 141
#define B10001110 142
#define B10001111 143
#define B10010000 144
#define B10010001 145
#define B10010010 146
#define B10010011 147
#define B10010100 148
#define B10010101 149
#define B10010110 150
#define B10010111 151
#define B10011000 152
#define B10011001 153
#define B10011010 154
#define B10011011 155
#define B10011100 156
#define B10011101 157
#define B10011110 158
#define B10011111 159
#define B10100000 160
#define B10100001 161
#define B10100010 162
#define B10100011 163
#define B10100100 164
#define B10100101 165
#define B10100110 166
#define B10100111 167
#define B10101000 168
#define B10101001 169
#define B10101010 170
#define B10101011 171
#define B10101100 172
#define B10101101 173
#define B10101110 174
#define B10101111 175
#define B10110000 176
#define B10110001 177
#define B10110010 178
#define B10110011 179
#define B10110100 180
#define B10110101 181
#define B10110110 182
#define B10110111 183
#define B10111000 184
#define B10111001 185
#define B10111010 186
#define B10111011 187
#define B10111100 188
#define B10111101 189
#define B10111110 190
#define B10111111 191
#define B11000000 192
#define B11000001 193
#define B11000010 194
#define B11000011 195
#define B11000100 196
#define B11000101 197
#define B11000110 198
#define B11000111 199
#define B11001000 200
#define B11001001 201
#define B11001010 202
#define B11001011 203
#define B11001100 204
#define B11001101 205
#define B11001110 206
#define B11001111 207
#define B11010000 208
#define B11010001 209
#define B11010010 210
#define B11010011 211
#define B11010100 212
#define B11010101 213
#define B11010110 214
#define B11010111 215
#define B11011000 216
#define B11011001 217
#define B11011010 218
#define B11011011 219
#define B11011100 220
#define B11011101 221
#define B11011110 222
#define B11011111 223
#define B11100000 224
#define B11100001 225
#define B11100010 226
#define B11100011 227
#define B11100100 228
#define B11100101 229
#define B11100110 230
#define B11100111 231
#define B11101000 232
#define B11101001 233
#define B11101010 234
#define B11101011 235
#define B11101100 236
#define B11101101 237
#define B11101110 238
#define B11101111 239
#define B11110000 240
#define B11110001 241
#define B11110010 242
#define B11110011 243
#define B11110100 244
#define B11110101 245
#define B11110110 246
#define B11110111 247
#define B11111000 248
#define B11111001 249
#define B11111010 250
#define B11111011 251
#define B11111100 252
#define B11111101 253
#define B11111110 254
#define B11111111 255
/* masks for first byte ( write )*/
int  lssproto_modifymask_first[8][9]={
    {0, B00000001,B00000011,B00000111,B00001111,B00011111,B00111111,B01111111,B11111111},/* mod 0*/
    {0, B00000011,B00000111,B00001111,B00011111,B00111111,B01111111,B11111111,B11111111},/* mod 1*/
    {0, B00000111,B00001111,B00011111,B00111111,B01111111,B11111111,B11111111,B11111111},/* mod 2*/
    {0, B00001111,B00011111,B00111111,B01111111,B11111111,B11111111,B11111111,B11111111},/* mod 3*/
    {0, B00011111,B00111111,B01111111,B11111111,B11111111,B11111111,B11111111,B11111111},/* mod 4*/
    {0, B00111111,B01111111,B11111111,B11111111,B11111111,B11111111,B11111111,B11111111},/* mod 5*/
    {0, B01111111,B11111111,B11111111,B11111111,B11111111,B11111111,B11111111,B11111111},/* mod 6*/
    {0, B11111111,B11111111,B11111111,B11111111,B11111111,B11111111,B11111111,B11111111},/* mod 7*/
};
/* masks for second byte ( write ) */
int lssproto_modifymask_second[8][9]={
    {0, B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000},/* mod 0 */
    {0, B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000001},/* mod 1 */
    {0, B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000001,B00000011},/* mod 2 */
    {0, B00000000,B00000000,B00000000,B00000000,B00000000,B00000001,B00000011,B00000111},/* mod 3 */
    {0, B00000000,B00000000,B00000000,B00000000,B00000001,B00000011,B00000111,B00001111},/* mod 4 */
    {0, B00000000,B00000000,B00000000,B00000001,B00000011,B00000111,B00001111,B00011111},/* mod 5 */
    {0, B00000000,B00000000,B00000001,B00000011,B00000111,B00001111,B00011111,B00111111},/* mod 6 */
    {0, B00000000,B00000001,B00000011,B00000111,B00001111,B00011111,B00111111,B01111111},/* mod 7 */
};
/*
 * used by bitstream routines
 */
int bitstream_maxbyte, bitstream_bitaddr ;
char *bitstream_buf;
/* initialize bitstream for output */
static int initOutputBitStream( char *buf ,int buflen)
{
    bitstream_bitaddr = 0;
    bitstream_maxbyte = buflen;
    bitstream_buf = buf;
    memset( buf,0, buflen);
    return 0;
}
/* initialize bitstream for input */
static int initInputBitStream( char *buf , int buflen)
{
    bitstream_bitaddr = 0;
    bitstream_maxbyte = buflen;
    bitstream_buf = buf;
    return 0;
}
/*
 * read from bit stream. used only from 1 bit to 8 bits
 * this is a base routine
 */
static unsigned int readInputBitStreamBody( int bwidth )
{
    int mod = bitstream_bitaddr % 8;
    int byteaddr = bitstream_bitaddr / 8;
    /* return if excess */
    if( byteaddr >= bitstream_maxbyte)return 0;
    if( bwidth >= 1 && bwidth <= 8){
        int b1 = (( bitstream_buf[byteaddr] & lssproto_modifymask_first[mod][bwidth] )>>mod);
        int b2 = (( bitstream_buf[byteaddr+1] & lssproto_modifymask_second[mod][bwidth])<<(8-mod));
        bitstream_bitaddr += bwidth;
        return  b1 | b2;
    } else {
        return 0;
    }
}
/*
 *  read from bit stream. used from 1 bit to 32 bits
 *
 */
static unsigned int readInputBitStream( int bwidth )
{
    if( bwidth <= 0 ){
        return 0;
    } else if( bwidth >= 1 && bwidth <= 8 ){
        return readInputBitStreamBody( bwidth );
    } else if( bwidth >= 9 && bwidth <= 16 ){
        unsigned int first = readInputBitStreamBody(8);
        unsigned int second = readInputBitStreamBody( bwidth-8);
        return first + (second << 8 );
    } else if( bwidth >= 17 && bwidth <= 24 ){
        unsigned int first = readInputBitStreamBody(8);
        unsigned int second = readInputBitStreamBody(8);
        unsigned int third = readInputBitStreamBody(bwidth-8);
        return first + ( second << 8 ) + ( third << 16 );
    } else if( bwidth >= 25 && bwidth <= 32 ){
        unsigned int first = readInputBitStreamBody(8);
        unsigned int second = readInputBitStreamBody(8);
        unsigned int third = readInputBitStreamBody(8);
        unsigned int forth = readInputBitStreamBody(bwidth-8);
        return first + ( second << 8 ) + ( third << 16 ) + ( forth << 24 );
    }
	return 0;
}
/*
 * write to a bitstream. only used from 1 bit to 8 bits
 * this is a base routine.
 */
static int writeOutputBitStreamBody( int bwidth , unsigned char b)
{
    int mod = bitstream_bitaddr % 8;
    int byteaddr = bitstream_bitaddr / 8;
    /* return error if excess */
    if( bitstream_maxbyte <= (byteaddr+1)) return -1;
    bitstream_buf[byteaddr] &= lssproto_modifymask_first[mod][bwidth];
    bitstream_buf[byteaddr] |= (b << mod) & lssproto_modifymask_first[mod][bwidth];
    bitstream_buf[byteaddr+1] &= lssproto_modifymask_second[mod][bwidth];
    bitstream_buf[byteaddr+1] |= (b>>(8-mod))& lssproto_modifymask_second[mod][bwidth];
    bitstream_bitaddr += bwidth;
    return byteaddr+1;
}
/*
 * write to a bitstream. used from 1 bits to 32 bits
 * returns -1 if error or buffer excession
 */
static int writeOutputBitStream( int bwidth, unsigned int dat)
{
    int ret;
    if( bwidth <= 0){
        return -1;
    } else if( bwidth >= 1 && bwidth <= 8 ){
        if((ret=writeOutputBitStreamBody( bwidth , (unsigned char)dat))<0)return -1;
    } else if( bwidth > 8 && bwidth <= 16 ){
        if(writeOutputBitStreamBody( 8 , (unsigned char)(dat&0xff))<0)return -1;
        if((ret=writeOutputBitStreamBody( bwidth - 8 , ( unsigned char)((dat>>8)&0xff)))<0)return -1;
    } else if( bwidth > 16 && bwidth <= 24 ){
        if(writeOutputBitStreamBody( 8 , (unsigned char)(dat&0xff))<0)return -1;
        if(writeOutputBitStreamBody( 8 , (unsigned char)((dat>>8)&0xff))<0)return -1;
        if((ret=writeOutputBitStreamBody( bwidth-16,(unsigned char)((dat>>16)&0xff)))<0)return -1;
    } else if( bwidth > 24 && bwidth <= 32 ){
        if(writeOutputBitStreamBody( 8 , (unsigned char)(dat&0xff))<0)return -1;
        if(writeOutputBitStreamBody( 8 , (unsigned char)((dat>>8)&0xff))<0)return -1;
        if(writeOutputBitStreamBody( 8 , (unsigned char)((dat>>16)&0xff))<0)return -1;
        if((ret=writeOutputBitStreamBody( bwidth-24,(unsigned char)((dat>>24)&0xff)))<0)return -1;
    } else {
        return -1;
    }
    return ret;
}
#define CHAR_SIZE 256
#define NODE_SIZE 512
#define BITS_LEN 9          /* 9 bit lzw compression */
typedef struct {
    unsigned char chr;
    int parent;
    int brother;
    int child;
}NODE;
long lssproto_ringoCompressor( unsigned char *code , long codelen ,
            unsigned char *text , long textlen)
{
    NODE node[NODE_SIZE];
    int freeNode;        
    int w,k;        /* used in this algo */
    int textind;    /* index to text buffer */
    int i;
    int position = 0;   /* indicates the last byte of code buffer */
    if( textlen <= 0 ) return -1;
    initOutputBitStream((char*) code,codelen);
    /* fill characters ( 0 ~ 255 ) in the beggining part of
       Node list */
    for(i=0; i<= CHAR_SIZE; i++){
        node[i].chr = (unsigned char)i;
        node[i].brother = i + 1;
        node[i].parent = 0;
        node[i].child = 0;
    }
    node[CHAR_SIZE].brother = 0;
    freeNode = CHAR_SIZE + 1;
    w = text[0];
    textind = 1;
    while(1){
        int rv;
        if( textind >= textlen ){
            k = CHAR_SIZE;      /* indicates EOF */
        } else {
            k = text[textind];
        }
        /* search if pattern 'wk' is registered or not. */
        rv = node[w].child;
        while(1){
            if( rv <= 0 ) break;
            if( node[rv].chr == k ) break;
            rv = node[rv].brother;
        }
        if( rv > 0 ){
            /* found it */
            w = rv;
        } else {
            position = writeOutputBitStream( BITS_LEN ,w  );
            /* return if buffer excession  */
            if( position > codelen ) return -1;
            /* register pattern 'wk' in the dictionary */
            if( freeNode < NODE_SIZE ){
                node[freeNode].parent = w;
                node[freeNode].chr = k;
                node[freeNode].brother = node[w].child;
                node[freeNode].child = 0;
                node[w].child = freeNode;
                freeNode++;
            }
            w = k;
        }
        if( textind == ( textlen + 1 )  ) break;
        textind++;
    }
    return position;
}

/*
 * Decoder.
 * return -1 if buffer excession. Notice buffer text
 * is modified .
 */
long lssproto_ringoDecompressor( unsigned char *text , long textlen ,
            unsigned char *code , long codelen)
{
    NODE node[NODE_SIZE];
    int stack[NODE_SIZE];
    int sp;
    int freeNode;
    int len;
    int i;
    int k = 0;
    int w = 0;
    if( codelen <= 0 ) return -1;
    initInputBitStream( (char*)code , codelen );
    for(i=0;i<CHAR_SIZE;i++){
        node[i].chr = (unsigned char)i;
        node[i].brother = i+1;
        node[i].parent = 0;
        node[i].child = 0;
    }
    node[CHAR_SIZE].brother = 0;
    freeNode = CHAR_SIZE + 1;
    len=0;  /* index to text buffer */
    sp=0;   /* stack pointer */
    while(1){
        int rv = 0;
        rv = readInputBitStream( BITS_LEN );
        /* return OK if EOF */
        if( rv == CHAR_SIZE ) break;        
        if( rv >= freeNode ){
            stack[sp++] = k;            /* exception */
            if( sp >=( sizeof( stack ) /sizeof(stack[0] )) )return -1;   
            k = w;
        } else {
            k = rv;
        }
        while(k> CHAR_SIZE ){
            if( k >= (sizeof(node)/sizeof(node[0]))) return -1;
            stack[sp++] = node[k].chr;
            k = node[k].parent;
            if( sp >=( sizeof( stack ) /sizeof(stack[0] )) ) return -1;
        }
        stack[sp++] = k;
        if( sp >= ( sizeof( stack ) /sizeof(stack[0] )) ) return -1;
        /* output to text buffer from stack.*/
        while(sp){
            if( ++len > textlen ) return -1;
            *text++ = stack[--sp];
        }
        /* register the pattern 'wk'*/
        if( len > 1 && freeNode < NODE_SIZE ){
            node[freeNode].parent = w;
            node[freeNode].chr = k;
            if( w >= (sizeof(node)/sizeof(node[0])))return -1;
            node[freeNode].brother = node[w].child;
            node[freeNode].child = 0;
            node[w].child = freeNode;
            freeNode++;
        }
        w = rv;
    }
    return len;
}
#endif  /* ifdef lssproto__ENCRYPT */

/* Convert 62-base digits to 10 digits */
int lssproto_a62toi( char *a )
{
	int ret = 0;
	int minus ;
	if( a[0] == '-' ){
		minus = -1;
        a++;
	} else {
		minus = 1;
	}
	
	while( *a != '\0' )
	{
		ret *= 62;
		if( '0' <= (*a) && (*a) <= '9' )
			ret += (*a)-'0';
		else
		if( 'a' <= (*a) && (*a) <= 'z' )
			ret += (*a)-'a'+10;
		else
		if( 'A' <= (*a) && (*a) <= 'Z' )
			ret += (*a)-'A'+36;
		else
			return 0;
		a++;
	}
	return ret * minus;
}
	
/*  Convert 10-base digits into 62-base digits. */
char *lssproto_cnv10to62( int a, char *out, int outlen )
{
	int		i, j;
    char    base[] = { "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"};
    int     tmp[64];
    int     src;
    int minus;
	int baselen = sizeof( base)-1;
    if( a < 0 ){
		minus = 1;
		a *= -1;
	} else {
		minus = 0;
	}
    /* special case */					   
    if( a < baselen) {
		if( minus ){
			*(out) = '-';
			*(out+1) = base[a];
			*(out+2) = '\0';
			return (out);
		} else {
			*out = base[a];
			*(out+1) = '\0';
			return( out);
		}
    }
    src = a;
    for( i = 0; src >= baselen; i ++ ) {
        tmp[i] = src % baselen;
        src /= baselen;
    }
    i--;
    if( minus ){
		*out = '-';
     	*(out+1) = base[src];
		for( j = 2; i >= 0; i --, j ++ ) {
			if( j > outlen - 2 ) return NULL;
			*(out+j) = base[tmp[i]];
		}		
	} else {
     	*out = base[src];
		for( j = 1; i >= 0; i --, j ++ ) {
			if( j > outlen - 2 ) return NULL;
			*(out+j) = base[tmp[i]];
		}		
	}
	*(out+j) = '\0';
    return( out);
}
