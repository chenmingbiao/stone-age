#include "version.h"
#include <string.h>
#include "char.h"
#include "object.h"
#include "char_base.h"
#include "npcutil.h"
#include "net.h"
#include "lssproto_serv.h"
#include "saacproto_cli.h"
#include "npc_duelranking.h"
#include "handletime.h"

static void NPC_Duelranking_selectWindow( int meindex, int toindex, int num, int select );
#define		NPC_DUELRANKING_WINDOWLINENUM	10


enum{
	WINDOWTYPE_PKLIST_START=10,
	WINDOWTYPE_PKLIST_SELECT,
	WINDOWTYPE_PKLIST_SELECT1,
	WINDOWTYPE_PKLIST_SELECT2,

	WINDOWTYPE_FAMILYCONTENDSTART,
	WINDOWTYPE_FAMILYCONTENDSELECT,
	WINDOWTYPE_FAMILYCONTENDLIST,
	WINDOWTYPE_FAMILYCONTENDJOIN,

	NPC_WORK_CURRENTTIME = CHAR_NPCWORKINT1,
	NPC_WORK_BATTLELOOP = CHAR_NPCWORKINT2,
};

BOOL NPC_DuelrankingInit( int meindex )
{
	CHAR_setInt( meindex , CHAR_WHICHTYPE , CHAR_TYPEDUELRANKING );
	return TRUE;
}

void NPC_DuelrankingLooked( int meindex , int lookedindex)
{
	NPC_Duelranking_selectWindow( meindex, lookedindex, 1, -1);
}

static void NPC_Duelranking_selectWindow( int meindex, int toindex, int num, int select)
{
	
	int		fd;
	if( CHAR_getInt( toindex , CHAR_WHICHTYPE ) != CHAR_TYPEPLAYER ) {
		return;
	}
	if( NPC_Util_CharDistance( toindex, meindex ) > 1) return;
	if( (fd = getfdFromCharaIndex( toindex)) == -1 ) return;

	switch( num){
	case 1:
		{
			char	message[1024];
			char	buf[2048];
			strcpy( message, 
				"1\n"
				"             要看哪一个？              \n\n\n"
				"             ＜强者们＞              \n\n"
				"             ＜ 自己 ＞              \n\n"
				"             ＜都不看＞              \n\n" );
			lssproto_WN_send( fd, WINDOW_MESSAGETYPE_SELECT, 
							WINDOW_BUTTONTYPE_NONE,
							CHAR_WINDOWTYPE_DUELRANKING_START,
							CHAR_getWorkInt( meindex, CHAR_WORKOBJINDEX),
							makeEscapeString( message, buf, sizeof(buf)));
		}
		break;
	}
}

void NPC_DuelrankingWindowTalked( int meindex, int talkerindex, int seqno, int select, char *data)
{
	int fdid = getFdidFromCharaIndex( talkerindex);
	if( fdid == -1 ) return;

	if( NPC_Util_CharDistance( talkerindex, meindex ) > 1) return;
	switch( seqno){
	case CHAR_WINDOWTYPE_DUELRANKING_START:
		if( atoi( data) == 3 ) {
			saacproto_DBGetEntryByCount_send( acfd, 
											DB_DUELPOINT, 0, 
								NPC_DUELRANKING_WINDOWLINENUM, fdid, 
								CHAR_getWorkInt( meindex, CHAR_WORKOBJINDEX));
		}
		if( atoi( data) == 5 ) {
			char dbkey[256];
			CHAR_makeDBKey( talkerindex, dbkey, sizeof( dbkey));
			saacproto_DBGetEntryRank_send( acfd, DB_DUELPOINT, dbkey, fdid,
								CHAR_getWorkInt( meindex, CHAR_WORKOBJINDEX));
		}
		break;
	case CHAR_WINDOWTYPE_DUELRANKING_TOPRANKING:
		if( select == WINDOW_BUTTONTYPE_PREV || select == WINDOW_BUTTONTYPE_NEXT) {
			int	count = CHAR_getWorkInt( talkerindex, CHAR_WORKSHOPRELEVANT);
			if( select == WINDOW_BUTTONTYPE_NEXT ) {
				count += NPC_DUELRANKING_WINDOWLINENUM;
			}else {
				count -= NPC_DUELRANKING_WINDOWLINENUM;
			}
	
			if( count < 0) count = 0;
			saacproto_DBGetEntryByCount_send( acfd, 
											DB_DUELPOINT, 
											count,
								NPC_DUELRANKING_WINDOWLINENUM, fdid,
								CHAR_getWorkInt( meindex, CHAR_WORKOBJINDEX));
		}else if( select == WINDOW_BUTTONTYPE_OK ) {
			if( strlen( data) != 0 ) {
				int count;
				count = atoi( data);
				if( count < 1 ) count = 1;
				saacproto_DBGetEntryByCount_send( acfd, 
												DB_DUELPOINT, 
												count-1,
									NPC_DUELRANKING_WINDOWLINENUM, fdid,
									CHAR_getWorkInt( meindex, CHAR_WORKOBJINDEX));
			}
		}
		break;
	}
}

void NPC_Duelranking_PrintRanking( char *data, int count_start, int fdid, int msgid2)
{
	char message[1024];
	char buf[2048];
	int		i;
	int	 cnt = 0;
	int	button = 0;	
	int fd = getfdFromFdid( fdid);
	if( fd == -1 ) return;
	message[0] = '\0';
	snprintf( message, sizeof( message), 
			"                        世界的强者们\n\n"
			"      Rank       DuelPoint      Lv     Name\n\n" );
	for( i = 1; ; i ++ ) {
		char	rankdata[256];
		char	cdandname[64];
		char	charaname[32];
		char	rank[16];
		char	score[64];
		char	msgbuf[512];
		char	cdkey[32];
		char	lv[16];
		char	hosi[6];
		
		if( !getStringFromIndexWithDelim( data, "|", i, rankdata, sizeof( rankdata))) {
			break;
		}
		if( !getStringFromIndexWithDelim( rankdata, ",", 2, rank, sizeof( rank)) ){
			continue;
		}
		if( !getStringFromIndexWithDelim( rankdata, ",", 3, cdandname, sizeof( cdandname)) ){			continue;
		}
		makeStringFromEscaped( cdandname);
		if( !getStringFromIndexWithDelim( cdandname, "_", 1, cdkey, sizeof( cdkey)) ){
			continue;
		}
		if( !getStringFromIndexWithDelim( cdandname, "_", 2, charaname, sizeof( charaname)) ){
			continue;
		}
		if( !getStringFromIndexWithDelim( rankdata, ",", 4, score, sizeof( score)) ){
			continue;
		}
		if( !getStringFromIndexWithDelim( rankdata, ",", 5, lv, sizeof( lv)) ){
			lv[0] = '\0';
		}
		{
			char *mycdkey = CHAR_getChar( CONNECT_getCharaindex(fd), CHAR_CDKEY);
			char *myname  = CHAR_getChar( CONNECT_getCharaindex(fd), CHAR_NAME);
			char engnum[4][3] = { "st","nd","rd","th"};
			int  dsprank = atoi(rank);
			if( dsprank > 3) dsprank = 3;
			if( mycdkey != NULL && myname != NULL && strcmp( cdkey, mycdkey) == 0 && 
				strcmp( charaname, myname) == 0 ) {
				strcpy( hosi, "★");
			}else {
				strcpy( hosi, "　");
			}
			snprintf( msgbuf, sizeof( msgbuf), "%s %5d%s      %10s     %3s     %s\n",
											hosi,
											atoi(rank)+1, engnum[dsprank],
											score, lv,
											charaname );
		}
		strcatsafe( message, sizeof( message), msgbuf);
		cnt ++;
	}
	if( cnt == 0 ) {
		strcpy( message, "\n      没有其他的资料了。");
		button = WINDOW_BUTTONTYPE_PREV|WINDOW_BUTTONTYPE_OK;
	}else if( cnt < NPC_DUELRANKING_WINDOWLINENUM ) {
		if( count_start >= NPC_DUELRANKING_WINDOWLINENUM) {
			button = WINDOW_BUTTONTYPE_OK | WINDOW_BUTTONTYPE_PREV;
		}else {
			button = WINDOW_BUTTONTYPE_OK;
		}
	}else if( count_start == 0 ) {
		button = WINDOW_BUTTONTYPE_NEXT|WINDOW_BUTTONTYPE_OK;
	}else {
		button = WINDOW_BUTTONTYPE_NEXT|WINDOW_BUTTONTYPE_PREV|WINDOW_BUTTONTYPE_OK;
	}
	lssproto_WN_send( fd, WINDOW_MESSAGETYPE_WIDEMESSAGEANDLINEINPUT, 
					button,
					CHAR_WINDOWTYPE_DUELRANKING_TOPRANKING,
					msgid2,
					makeEscapeString( message, buf, sizeof(buf)));

	if( cnt == 0 ) {
		CHAR_setWorkInt( CONNECT_getCharaindex(fd), CHAR_WORKSHOPRELEVANT, -1);
	}else {
		CHAR_setWorkInt( CONNECT_getCharaindex(fd), CHAR_WORKSHOPRELEVANT, count_start);
	}
}

void NPC_Duelranking_CallMyRanking( int count, int msgid, int msgid2)
{
	count -= 5;
	if( count < 0 ) count = 0;
	saacproto_DBGetEntryByCount_send( acfd,  DB_DUELPOINT, 
									count, NPC_DUELRANKING_WINDOWLINENUM, 
									msgid, msgid2 );
}

