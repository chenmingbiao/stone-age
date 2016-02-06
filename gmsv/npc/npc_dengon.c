#include "version.h"
#include <time.h>
#include "char.h"
#include "object.h"
#include "lssproto_serv.h"
#include "npcutil.h"
#include "handletime.h"

static BOOL NPC_DengonReadString( int meindex, int id, char *str );
static void NPC_DengonWriteString( int meindex, int talkerindex, int id, char *str );

#define DENGONFILEDIR "./Dengon/"
#define DENGONFILELINENUM 1000
#define DENGONFILESTRINGSIZE 256
#define DENGONFILECOUNTERSIZE 11
#define DENGONFILEENTRYSIZE (DENGONFILECOUNTERSIZE+DENGONFILESTRINGSIZE+1)
#define MESSAGEINONEWINDOW 7

char NPC_sendbuf[DENGONFILEENTRYSIZE*MESSAGEINONEWINDOW];

BOOL NPC_DengonInit( int meindex )
{
    char filename[256], tmp[256];
    FILE *f;
    int i,j;
    int id, maxid;

    CHAR_setInt( meindex, CHAR_WHICHTYPE, CHAR_TYPEDENGON );

    snprintf( filename,sizeof(filename), "%s%d_%d_%d",
              DENGONFILEDIR, CHAR_getInt( meindex, CHAR_FLOOR ),
              CHAR_getInt( meindex, CHAR_X ),
              CHAR_getInt( meindex, CHAR_Y ) );

    if( ! (f=fopen( filename, "r" )) ){
        f = fopen( filename, "w" );
        if( f ){
        }else{
            print( "ERROR:Can't create Dengonfile %s!\n",filename );
            return FALSE;
        }
        for( j = 0; j < DENGONFILELINENUM; j++ ){
            fwrite( "0000000000:", sizeof(char),
                    DENGONFILECOUNTERSIZE, f);
            for( i=0;i<DENGONFILESTRINGSIZE;i++ ){
                fwrite( "\0", sizeof(char),1, f );
            }
            fwrite( "\n", sizeof(char), 1, f);
        }
    }

    id =0;
    maxid = 0;
    
    for( i = 0; i < DENGONFILELINENUM; i++ ){
        fseek( f, i*DENGONFILEENTRYSIZE, SEEK_SET );
        fread( tmp, sizeof(char), DENGONFILECOUNTERSIZE, f );
        tmp[DENGONFILECOUNTERSIZE-1] = '\0';
        if( (id=atoi(tmp)) > maxid ) {
            maxid = id;
        }
    }

    CHAR_setWorkInt( meindex, CHAR_WORKDENGONMAXID, maxid );
    
    fclose(f);
    return TRUE;
}


void NPC_DengonWindowTalked( int index, int talker, 
								int seqno, int select, char *data)
{
    char	buf[DENGONFILEENTRYSIZE*MESSAGEINONEWINDOW*2];

    int id;

#define DENGONDISTANCE 3
	
    if(NPC_Util_CharDistance(index,talker) > DENGONDISTANCE ) return;
	if( seqno == CHAR_WINDOWTYPE_DENGON) {
		int buttontype = 0;
		int		maxid;
        switch( select ){
          case WINDOW_BUTTONTYPE_NEXT:
          case WINDOW_BUTTONTYPE_PREV:
            id = CHAR_getWorkInt( talker, CHAR_WORKSHOPRELEVANT) 
             	+ MESSAGEINONEWINDOW 
             	 * (( select == WINDOW_BUTTONTYPE_NEXT) ? 1 : -1);
			
			maxid = CHAR_getWorkInt( index, CHAR_WORKDENGONMAXID );
			
			if( id > maxid) id = maxid;
			else if( id < 2 ) id = 1;
			
			buttontype = WINDOW_BUTTONTYPE_OKCANCEL;
			if( id == maxid ) buttontype |= WINDOW_BUTTONTYPE_PREV;
			else if( id == 1 ) buttontype |= WINDOW_BUTTONTYPE_NEXT;
			else  {
				buttontype |= WINDOW_BUTTONTYPE_PREV;
				buttontype |= WINDOW_BUTTONTYPE_NEXT;
            }
            CHAR_setWorkInt( talker, CHAR_WORKSHOPRELEVANT, id);
            if( NPC_DengonReadString( index, id, NPC_sendbuf ) ){
                int fd;
                fd = getfdFromCharaIndex( talker );
                if( fd == -1 )return;
		        lssproto_WN_send(fd, WINDOW_MESSAGETYPE_WIDEMESSAGEANDLINEINPUT,
        					buttontype,
        					CHAR_WINDOWTYPE_DENGON,
							CHAR_getWorkInt( index, CHAR_WORKOBJINDEX),
							makeEscapeString( NPC_sendbuf, buf, sizeof(buf)));
            }
            break;
          case WINDOW_BUTTONTYPE_OK:
            if( strlen( data) == 0 ) break;
            id = CHAR_getWorkInt(index, CHAR_WORKDENGONMAXID );
            id++;
            NPC_DengonWriteString( index, talker, id, data );
            CHAR_setWorkInt(index, CHAR_WORKDENGONMAXID, id );
            CHAR_setWorkInt( talker, CHAR_WORKSHOPRELEVANT, id);
            if( NPC_DengonReadString( index, id, NPC_sendbuf ) ){
                int fd;
                fd = getfdFromCharaIndex( talker );
                if( fd == -1 )return;
		        lssproto_WN_send(fd, WINDOW_MESSAGETYPE_WIDEMESSAGEANDLINEINPUT,
        					WINDOW_BUTTONTYPE_OKCANCEL|
        					WINDOW_BUTTONTYPE_PREV,
        					CHAR_WINDOWTYPE_DENGON,
							CHAR_getWorkInt( index, CHAR_WORKOBJINDEX),
							makeEscapeString( NPC_sendbuf, buf, sizeof(buf)));
            }
            break;
          default:
            break;
        }
    }
}




void NPC_DengonLooked( int meindex, int lookedindex )
{
    int maxid;
    char	buf[DENGONFILEENTRYSIZE*MESSAGEINONEWINDOW*2];
	if( NPC_Util_CharDistance( lookedindex, meindex ) > 1) return;
    maxid = CHAR_getWorkInt( meindex, CHAR_WORKDENGONMAXID );
    if( NPC_DengonReadString( meindex, maxid, NPC_sendbuf ) ){
        int fd;
        fd = getfdFromCharaIndex( lookedindex );
        if( fd == -1 )return;
        lssproto_WN_send(fd, WINDOW_MESSAGETYPE_WIDEMESSAGEANDLINEINPUT,
        					WINDOW_BUTTONTYPE_OKCANCEL|
        					WINDOW_BUTTONTYPE_PREV,
        					CHAR_WINDOWTYPE_DENGON,
							CHAR_getWorkInt( meindex, CHAR_WORKOBJINDEX),
							makeEscapeString( NPC_sendbuf, buf, sizeof(buf)));
        					
        CHAR_setWorkInt( lookedindex, CHAR_WORKSHOPRELEVANT, maxid);
        
    }
    
}

static BOOL NPC_DengonReadString( int meindex, int id, char *str )
{
    char filename[256];
    char readbuf[DENGONFILEENTRYSIZE*MESSAGEINONEWINDOW];

    FILE *f;
    int maxid, i, sendid, len;
    char *writeptr, *readptr;


    maxid = CHAR_getWorkInt( meindex, CHAR_WORKDENGONMAXID );
    if( id > maxid ) id = maxid;
    sendid = id - MESSAGEINONEWINDOW + 1;

    if( sendid < maxid - DENGONFILELINENUM + 1 ){
        sendid = maxid - DENGONFILELINENUM + 1;
    }

    if( sendid < 1 ) sendid = 1;

    snprintf( filename,sizeof(filename), "%s%d_%d_%d",
              DENGONFILEDIR, CHAR_getInt( meindex, CHAR_FLOOR ),
              CHAR_getInt( meindex, CHAR_X ),
              CHAR_getInt( meindex, CHAR_Y ) );

    writeptr = str;
	strcpy( writeptr, "                         ´«ÑÔ°å\n");
    len = strlen(writeptr);
    writeptr += len;

    f = fopen( filename, "r" );
    if( !f ) return FALSE;
{
	char	buf[DENGONFILEENTRYSIZE];
	int		wptr = 0;
	readbuf[0] = '\0';
	for( i = 0; i < MESSAGEINONEWINDOW; i ++ ) {
	    fseek( f, ((sendid+i)%DENGONFILELINENUM)*DENGONFILEENTRYSIZE, SEEK_SET );
	    fread( buf, sizeof(char),
	           DENGONFILEENTRYSIZE , f );
		memcpy( &readbuf[wptr], buf,sizeof( buf));
		wptr += DENGONFILEENTRYSIZE;
	}
}
    fclose(f);
    readptr = readbuf + DENGONFILECOUNTERSIZE;
    for( i=0;i<MESSAGEINONEWINDOW;i++ ){
        snprintf( writeptr, DENGONFILESTRINGSIZE, "%s\n", readptr );
        writeptr+= strlen(readptr)+1; 
        readptr+=DENGONFILEENTRYSIZE;
    }
    
    return TRUE;

}

static void NPC_DengonWriteString( int meindex, int talker, int id, char *str )
{
    char buf[256];
    FILE *f;
    int line;
	struct  tm tm1;

    snprintf( buf,sizeof(buf), "%s%d_%d_%d",
              DENGONFILEDIR, CHAR_getInt( meindex, CHAR_FLOOR ),
              CHAR_getInt( meindex, CHAR_X ),
              CHAR_getInt( meindex, CHAR_Y ) );
    
    f = fopen( buf, "r+" );
    if( !f ) return;

    line = id%DENGONFILELINENUM;
    fseek( f, line*DENGONFILEENTRYSIZE, SEEK_SET );

    fprintf( f, "%010d:",id );
	memcpy( &tm1, localtime( (time_t *)&NowTime.tv_sec), sizeof( tm1));
    fprintf( f, "%s\n                             %2d/%02d %2d:%02d %s%c", 
    		str, 
    		tm1.tm_mon +1, tm1.tm_mday, tm1.tm_hour, tm1.tm_min,
    		CHAR_getChar( talker, CHAR_NAME),
    		'\0');
    fclose(f);
}
