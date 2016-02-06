#ifndef __COMMON_H__
#define __COMMON_H__

#include <stdio.h>
#include <errno.h>

/* MACROS */
#define RETURNFALSEIFFALSE(x) if(!x)return FALSE;
#define EXITWITHEXITCODEIFFALSE(x,code) if(!x)exit(code);
#ifdef __GNUC__
#define print(format,arg...) fprintf( stderr, format ,##arg)
#define fprint(format,arg...) fprintf( stderr, "%s:%d " format , __FILE__ , __LINE__ , ##arg)
#endif
#define debug(x,y) fprintf( stderr, #x " = %" #y "\n" , x)
#define arraysizeof( x ) (sizeof(x)/sizeof(x[0]))

#define errorprint    {extern int errno;fprint( "%s\n" ,strerror(errno));}

#define BACKSLASH '\\'
#define NEWLINE '\n'
#define TAB '\t'
#define SPACE ' '

/*票及 define 方曰兜蝈卞允月    */
/*#define min( x,y ) ((x)>(y)?(y):(x))
  #define max( x,y ) ((x)>(y)?(x):(y))*/
#define min( x,y ) ({typeof(x) __x=(x),__y=(y);(__x < __y) ? __x : __y; })
#define max( x,y ) ({typeof(x) __x=(x),__y=(y);(__x < __y) ? __y : __x; })
#define swap( x,y )({typeof(x) __x=(y);(y)=(x);(x)=__x;})
#define SUCCESSFUL "successful"
#define FAILED "failed"

#define LOCK 1
#define UNLOCK 0

#define STRINGBUFSIZ    4096

#define OPEN {FILE* f;f=fopen("a.txt","a");
#define CLOSE fclose(f);}

#define time_diff_us(new,old) ((unsigned)( (new.tv_sec - old.tv_sec)*1000000 + ( new.tv_usec - old.tv_usec  ) ))




extern int snprintf (char* , size_t, const char* , ...)
#ifdef __GNUC__
    __attribute__ ((format (printf,3,4)));
#else
    ;
#endif

#define ON  1
#define OFF 0
#define BOOL int
#define FALSE 0
#define TRUE  1
#define INLINE inline

#endif
