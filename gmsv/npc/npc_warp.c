#include "version.h"
#include <string.h>
#include "object.h"
#include "char_base.h"
#include "char.h"
#include "map_deal.h"
#include "readmap.h"
#include "npccreate.h"
#include "npcutil.h"
#include "handletime.h"
#include "npc_eventaction.h"

//#define _RECORD_NPCMAN_ //纪录

BOOL NPC_TimeWarpCheck(int meindex,char *buf,int mode);

static void NPC_WarpsetNeverMake( int charaindex )
{
    int cindex;
    cindex = CHAR_getInt(charaindex,CHAR_NPCCREATEINDEX);
    if( NPC_CHECKCREATEINDEX(cindex) )
        NPC_create[cindex].workdata[NPC_CREATEWORKNEVERMAKE] = 1;
}

BOOL NPC_WarpInit( int charaindex )
{
	char arg[NPC_UTIL_GETARGSTR_BUFSIZE];
    int ret;
    int floor=-1,x=-1,y=-1;
    char    token[128];

#ifdef _RECORD_NPCMAN_ //纪录
	char evtype[256];
	char evtime[256];
	char argfile[256];

	if( NPC_Util_CheckAssignArgFile( charaindex, argfile) == NULL )
		sprintf( argfile, "NULL");
#endif

	CHAR_setInt( charaindex, CHAR_WHICHTYPE , CHAR_TYPEWARP );
    if( NPC_Util_GetArgStr( charaindex, arg, sizeof( arg)) == NULL )	{
		print("\n GetArgStr ERR!!");
		return FALSE;
	}
#ifdef _NEW_WARPPOINT
	if( strstr( arg, "FREEMORE") != NULL )	{
#ifdef _RECORD_NPCMAN_ //纪录
		sprintf( evtype, "FREE");
		sprintf( evtime, "NULL");
#endif
		CHAR_setWorkInt( charaindex, CHAR_WORKEVENTTYPE, CHAR_EVENT_WARP);
	}else	{
#endif
#ifdef _MAP_WARPPOINT
		return FALSE;
#endif
#ifdef _RECORD_NPCMAN_ //纪录
		sprintf( evtype, "NONE");
#endif
		ret=getStringFromIndexWithDelim(arg,"|", 1,token,sizeof(token));
		if( ret ) floor = atoi( token );
		ret=getStringFromIndexWithDelim(arg,"|", 2,token,sizeof(token));
		if( ret ) x = atoi( token );
		ret=getStringFromIndexWithDelim(arg,"|", 3,token,sizeof(token));
		if( ret ) y = atoi( token );
		if( MAP_IsValidCoordinate( floor,x,y )== FALSE ){
			NPC_WarpsetNeverMake( charaindex );
			//print( "Warp NPC:Invalid arg:%s\n", arg );
			print( "Warp Npc Err:%d:%d:%d->(%s)\n",
        		CHAR_getInt( charaindex, CHAR_FLOOR ),
        		CHAR_getInt( charaindex, CHAR_X ),
        		CHAR_getInt( charaindex, CHAR_Y ),
        		arg );
			return FALSE;
		}
		ret=getStringFromIndexWithDelim(arg,"|",4,token,sizeof(token));
		if(ret==TRUE){
			int day;
			day=NPC_TimeWarpCheck(charaindex,token,0);
				if(day==0){
#ifdef _RECORD_NPCMAN_ //纪录
					sprintf( evtime, "N");
#endif
					CHAR_setWorkInt( charaindex, CHAR_WORKEVENTTYPE, CHAR_EVENT_WARP_NIGHT);
				}else if(day==1){
#ifdef _RECORD_NPCMAN_ //纪录
					sprintf( evtime, "M");
#endif
					CHAR_setWorkInt( charaindex, CHAR_WORKEVENTTYPE, CHAR_EVENT_WARP_MORNING);
				}else if(day==2){
#ifdef _RECORD_NPCMAN_ //纪录
					sprintf( evtime, "A");
#endif
					CHAR_setWorkInt( charaindex, CHAR_WORKEVENTTYPE, CHAR_EVENT_WARP_NOON);
				}else{
#ifdef _RECORD_NPCMAN_ //纪录
					sprintf( evtime, "NULL");
#endif
					CHAR_setWorkInt( charaindex, CHAR_WORKEVENTTYPE, CHAR_EVENT_WARP);
				}
		}else{
#ifdef _RECORD_NPCMAN_ //纪录
					sprintf( evtime, "NULL");
#endif
			CHAR_setWorkInt( charaindex, CHAR_WORKEVENTTYPE, CHAR_EVENT_WARP);
		}
#ifdef _NEW_WARPPOINT
	}
#endif
    CHAR_setFlg( charaindex,CHAR_ISVISIBLE, 0 );
    CHAR_setFlg( charaindex,CHAR_ISOVERED,1 );
    CHAR_setFlg( charaindex,CHAR_ISATTACKED,0 );
    CHAR_setFlg( charaindex,CHAR_ISATTACK,0 );

#ifdef _RECORD_NPCMAN_ //纪录
	{
		FILE *fp=NULL;
		char filename[256];
		sprintf( filename,"./data/npc/%s", "mapwarp.txt");
		if( (fp = fopen( filename, "a+")) != NULL ) {
			fprintf( fp, "%s:%s:%d,%d,%d:%d,%d,%d:%s\n",
				evtype, evtime,
				CHAR_getInt( charaindex, CHAR_FLOOR),
				CHAR_getInt( charaindex, CHAR_X),
				CHAR_getInt( charaindex, CHAR_Y),
				floor, x, y, argfile );
			fclose( fp);
		}else {
		}
	}
#endif
    return TRUE;
}

void NPC_WarpWarpCharacter( int warpnpcindex, int charaindex )
{
	char	arg[NPC_UTIL_GETARGSTR_BUFSIZE];
	int i=0;
#ifdef _NEW_WARPPOINT
	char buf1[256], buf2[256], buf3[256], freemsg[256];
	BOOL nBefind = FALSE;
	struct wapepoints	{
		int m_floor;
		int m_x;
		int m_y;
	}Wpoint[10]={
		{-1,0,0}, {-1,0,0}, {-1,0,0}, {-1,0,0}, {-1,0,0},
		{-1,0,0}, {-1,0,0}, {-1,0,0}, {-1,0,0}, {-1,0,0}
	};
	int iRand=0;
#endif
    if( CHAR_getInt( charaindex,CHAR_WHICHTYPE ) != CHAR_TYPEPLAYER ){
        return;
    }

    if( NPC_Util_GetArgStr( warpnpcindex, arg, sizeof( arg)) == NULL ){
		return;
	}

	{
        char    token[16];
        int     floor=0,x=0,y=0;
        int     ret;

#ifdef _NEW_WARPPOINT
	i=1;
	if( strstr( arg, "FREEMORE") != NULL )	{
		int k=1, pw=-1;
		while( getStringFromIndexWithDelim( arg, "OVER", i, buf1, sizeof( buf1)) != FALSE )	{
			i++;
			if( NPC_Util_GetStrFromStrWithDelim( buf1, "FREE", buf2, sizeof( buf2)) == NULL )
				continue;
			if( NPC_ActionPassCheck( warpnpcindex, charaindex, buf2) == FALSE )	{
				continue;
			}else	{
				iRand=0;
				if( NPC_Util_GetStrFromStrWithDelim( buf1, "TO", buf2, sizeof( buf2)) != FALSE )	{
					k=1;
					while( getStringFromIndexWithDelim(  buf2, "&", k, freemsg, sizeof( freemsg)) != FALSE ){
						k++;
						if( getStringFromIndexWithDelim( freemsg, ",", 1, buf3, sizeof( buf3)) == FALSE )
							break;
						Wpoint[iRand].m_floor = atoi( buf3);
						if( getStringFromIndexWithDelim( freemsg, ",", 2, buf3, sizeof( buf3)) == FALSE )
							break;
						Wpoint[iRand].m_x = atoi( buf3);
						if( getStringFromIndexWithDelim( freemsg, ",", 3, buf3, sizeof( buf3)) == FALSE )
							break;
						Wpoint[iRand].m_y = atoi( buf3);
						iRand++;
						nBefind = TRUE;
						if( iRand >= arraysizeof( Wpoint))
							break;
					}
					break;
				}
			}
			if( nBefind == TRUE )
				break;
		}

		if( nBefind == FALSE || iRand <= 0){	//没找到条件
			floor = CHAR_getInt( warpnpcindex, CHAR_FLOOR);
			x = CHAR_getInt( warpnpcindex, CHAR_X);
			y = CHAR_getInt( warpnpcindex, CHAR_Y);
		}else	{
			pw = RAND( 0, (iRand-1));
			floor = Wpoint[pw].m_floor;
			x = Wpoint[pw].m_x;
			y = Wpoint[pw].m_y;
			if( NPC_Util_GetStrFromStrWithDelim( arg, "CHECKPARTY", buf1, sizeof( buf1)) != NULL ) {
				if( strstr( buf1, "TRUE") != NULL ){
					if( CHAR_getWorkInt( charaindex, CHAR_WORKPARTYMODE ) == CHAR_PARTY_LEADER )	{
						CHAR_DischargeParty( charaindex, 0); // 拆队
					}
				}
			}
			if( MAP_IsValidCoordinate( floor,x,y )== FALSE ){
				print( "*Warp NPC:Invalid %d %d %d" , floor,x,y );
				return;
			}
			k=0;
		}
	}else	{
#endif
        ret=getStringFromIndexWithDelim(arg,"|", 1,token,sizeof(token));
        if( ret == FALSE )return;
        floor = atoi( token );

        ret=getStringFromIndexWithDelim(arg,"|", 2,token,sizeof(token));
        if( ret == FALSE )return;
        x = atoi( token );

        ret=getStringFromIndexWithDelim(arg,"|", 3,token,sizeof(token));
        if( ret == FALSE )return;
        y = atoi( token );
#ifdef _NEW_WARPPOINT
	}
#endif
		{
		  int ff=floor;

		  int of=CHAR_getInt(charaindex, CHAR_FLOOR);
		  int fd=CHAR_getWorkInt( charaindex, CHAR_WORKFD);
		  int eqen=getEqNoenemy(fd);
	  		if (eqen<200) {
			  if (eqen>=120) {
				if ((ff==100)||(ff==200)||(ff==300)||(ff==400)||(ff==500)) {
				  if ((of!=100)&&(of!=200)&&(of!=300)&&(of!=400)&&(of!=500)) {
					CHAR_talkToCli(charaindex, -1,
					  "太阳神的首饰发出一道奇异的光芒，隐藏了你的行踪。", CHAR_COLORWHITE);
				  }
				} else {
				  if ((of==100)||(of==200)||(of==300)||(of==400)||(of==500)) {
					CHAR_talkToCli(charaindex, -1, "环绕着你的光芒消失了。", CHAR_COLORWHITE);
				  }
				}
			  } else if (eqen>=80) {
				if ((ff==100)||(ff==200)||(ff==300)||(ff==400)) {
				  if ((of!=100)&&(of!=200)&&(of!=300)&&(of!=400)) {
					CHAR_talkToCli(charaindex, -1,
					  "太阳神的首饰发出一道奇异的光芒，隐藏了你的行踪。", CHAR_COLORWHITE);
				  }
				} else {
				  if ((of==100)||(of==200)||(of==300)||(of==400)) {
					CHAR_talkToCli(charaindex, -1, "环绕着你的光芒消失了。", CHAR_COLORWHITE);
				  }
				}
			  } else if (eqen>=40) {
				if ((ff==100)||(ff==200)) {
				  if ((of!=100)&&(of!=200)) {
					CHAR_talkToCli(charaindex, -1,
					  "太阳神的首饰发出一道奇异的光芒，隐藏了你的行踪。", CHAR_COLORWHITE);
				  }
				} else {
				  if ((of==100)||(of==200)) {
					CHAR_talkToCli(charaindex, -1, "环绕着你的光芒消失了。", CHAR_COLORWHITE);
				  }
				}
			  }
			}
		}

#ifdef _DROPSTAKENEW
		{
			i = 0;
			for (i = 0; i < MAXSTAKENUM; i++){
				if (CHAR_getWorkInt(charaindex, CHAR_WORKSTAKETYPE1 + i) > 0){
					char tmpbuf[256];
					snprintf(tmpbuf, sizeof(tmpbuf), "由於你离开了房间，所以将无法取回彩券！");
					CHAR_setWorkInt(charaindex, CHAR_WORKSTAKETYPE1 + i, 0);
					CHAR_talkToCli(charaindex, -1, tmpbuf, CHAR_COLORYELLOW);
				}
			}
			CHAR_setWorkInt(charaindex, CHAR_WORKSTAKEFLAG, 0);
#ifdef _FIX_GAMBLENUM		
			if(CHAR_getInt(charaindex, CHAR_GAMBLENUM) < 0)
				CHAR_setInt(charaindex, CHAR_GAMBLENUM, 0);			
#endif
		}
#endif
		if( floor == -1)	{
			return;
		}else	{
			if( MAP_IsValidCoordinate( floor,x,y )== FALSE ){
				print( "Warp NPC:Invalid %d %d %d" , floor,x,y );
				return;
			}

			CHAR_warpToSpecificPoint(charaindex, floor, x, y);
		}
    }
}

void NPC_WarpWatch( int meobjindex, int objindex, CHAR_ACTION act,
                    int x,int y,int dir, int* opt,int optlen )
{
    int     meindex,moveindex;
    if( OBJECT_getType( objindex ) != OBJTYPE_CHARA )return;

    meindex = OBJECT_getIndex(meobjindex);
    moveindex = OBJECT_getIndex(objindex);

    if( CHAR_getInt( moveindex,CHAR_WHICHTYPE ) != CHAR_TYPEPLAYER ){
        return;
    }
    if( act != CHAR_ACTWALK )return;

    if( opt[0] == x && opt[1] == y )return;
    if( CHAR_getInt(meindex,CHAR_X) == x && CHAR_getInt(meindex,CHAR_Y) == y )	{
        NPC_WarpWarpCharacter( meindex, moveindex );
	}
}

void NPC_WarpPostOver( int meindex, int charaindex )
{
    NPC_WarpWarpCharacter( meindex, charaindex );
}

int NPC_WarpSearchByPosition( int fl , int x, int y)
{
    OBJECT  object;
    for( object=MAP_getTopObj(fl,x,y) ; object ;
         object=NEXT_OBJECT(object)){
        int objindex = GET_OBJINDEX(object);
        if( OBJECT_getType( objindex ) == OBJTYPE_CHARA
            && CHAR_getInt( OBJECT_getIndex(objindex), CHAR_WHICHTYPE)
            == CHAR_TYPEWARP ){
            return OBJECT_getIndex(objindex);
        }
    }
    return -1;
}


int NPC_TimeWarpCheck(int meindex,char *buf,int mode)
{
	LSTIME		nowlstime;
	char *time[4]={"N","M","A"};
	int now;
	int i=0;
	RealTimeToLSTime( NowTime.tv_sec, &nowlstime);
	now = getLSTime( &nowlstime);
	if(mode==0){
		for( i=0 ; i < 3 ; i++){
			if(strstr( buf,time[i]) != NULL){
				 return i;
			}
		}
		return -1;;
	}
	if(mode==1){
		if(now==CHAR_getWorkInt( meindex, CHAR_WORKEVENTTYPE)){
			return now;
		}
		return -1;
	}
	return -1;
}
