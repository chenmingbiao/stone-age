#include "version.h"
#include <string.h>
#include "char.h"
#include "object.h"
#include "char_base.h"
#include "npcutil.h"
#include "lssproto_serv.h"
#include "npc_bus.h"
#include "handletime.h"

/* 
 * 田旦心凶中卅仪毛允月NPC
 */
 
enum {
	NPC_WORK_ROUTETOX = CHAR_NPCWORKINT1,		/* 升仇尺＂  甄   */
	NPC_WORK_ROUTETOY = CHAR_NPCWORKINT2,		/* 升仇尺＂  甄   */
	NPC_WORK_ROUTEPOINT = CHAR_NPCWORKINT3,		/* 漆窒    井 */
	NPC_WORK_ROUNDTRIP = CHAR_NPCWORKINT4,		/* 垫五井窖曰井  ㄟ“垫五 ㄠ“窖曰  */
	NPC_WORK_MODE = CHAR_NPCWORKINT5,
	NPC_WORK_CURRENTROUTE = CHAR_NPCWORKINT6, 
	NPC_WORK_ROUTEMAX = CHAR_NPCWORKINT7,
	NPC_WORK_WAITTIME = CHAR_NPCWORKINT8,
	NPC_WORK_CURRENTTIME = CHAR_NPCWORKINT9,
	NPC_WORK_SEFLG = CHAR_NPCWORKINT10,
};

/* 蛐  丢永本□斥及enum */
enum {
	NPC_BUS_MSG_GETTINGON,
	NPC_BUS_MSG_NOTPARTY,
	NPC_BUS_MSG_OVERPARTY,
	NPC_BUS_MSG_DENIEDITEM,
	NPC_BUS_MSG_ALLOWITEM,
	NPC_BUS_MSG_LEVEL,
	NPC_BUS_MSG_GOLD,
	NPC_BUS_MSG_EVENT,
	NPC_BUS_MSG_START,
	NPC_BUS_MSG_END,
};
typedef struct {
	char	option[32];
	char	defaultmsg[128];
}NPC_BUS_MSG;
NPC_BUS_MSG		busmsg[] = {
	{ "msg_gettingon",	"PAON！（你无法於中途加入我们唷！）"},
	{ "msg_notparty",	"PAPAON！！无法以团队加入唷！"},
	{ "msg_overparty",	"PAON！！人数已满。"},
	{ "msg_denieditem",		"PAPAON！！我可不要这个道具！"},
	{ "msg_allowitem",		"哇喔~(想要那个道具啊!)"},
	{ "msglevel",		"PAPAON！！你的等级还不够唷！"},
	{ "msg_stone",		"PAPAON！！金钱不足唷！"},
	{ "msg_event",		"PAON！！你无法加入唷！"},
	{ "msg_start",		"哇喔~(出发进行)"},
	{ "msg_end",		"哇喔~(到罗)"}
	
};

static int NPC_BusSetPoint( int meindex, char *argstr);
static void NPC_BusSetDestPoint( int meindex, char *argstr);
static BOOL NPC_BusCheckDeniedItem( int meindex, int charaindex, char *argstr);
static BOOL NPC_BusCheckLevel( int meindex, int charaindex, char *argstr);
static int NPC_BusCheckStone( int meindex, int charaindex, char *argstr);
static void NPC_BusSendMsg( int meindex, int talkerindex, int tablenum);
static int NPC_BusGetRoutePointNum( int meindex, char *argstr );
static void NPC_Bus_walk( int meindex);

#define		NPC_BUS_LOOPTIME		200

/* 谨切凛棉犯白巧伙玄 */
#define		NPC_BUS_WAITTIME_DEFAULT	180

#define		NPC_BUS_WAITINGMODE_WAITTIME	5000

/*********************************
* 赓渝质  
*********************************/
BOOL NPC_BusInit( int meindex )
{
	char	argstr[NPC_UTIL_GETARGSTR_BUFSIZE - 1024 * 20];
	int i;
	char	buf[256];
	int	routenum;
	int	waittime;
	int seflg;
	
	NPC_Util_GetArgStr( meindex, argstr, sizeof( argstr));

	/* 卅仃木壬中仃卅中娄醒及民尼永弁 */
	routenum = NPC_Util_GetNumFromStrWithDelim( argstr, "routenum");//行驶路线数量

	if( routenum == -1 ) {
		print( "npcbus:nothing routenum \n");
		return FALSE;
	}
	CHAR_setWorkInt( meindex, NPC_WORK_ROUTEMAX, routenum);
	
	for( i = 1; i <= routenum; i ++ ) {
		char routetostring[64];
		snprintf( routetostring, sizeof( routetostring), "routeto%d", i);
		if( NPC_Util_GetStrFromStrWithDelim( argstr, routetostring,buf, sizeof(buf))
			== NULL ) 
		{
			print( "npcbus:nothing route to \n");
			return FALSE;
		}

	}
	waittime = NPC_Util_GetNumFromStrWithDelim( argstr, "waittime");

	if( waittime == -1 ) waittime = NPC_BUS_WAITTIME_DEFAULT;
	CHAR_setWorkInt( meindex, NPC_WORK_WAITTIME, waittime);

	seflg = NPC_Util_GetNumFromStrWithDelim( argstr, "seflg");

	if( seflg == -1 ) seflg = TRUE;
	CHAR_setWorkInt( meindex, NPC_WORK_SEFLG, seflg);
    
    CHAR_setInt( meindex , CHAR_WHICHTYPE , CHAR_TYPEBUS );
	
	CHAR_setWorkInt( meindex, NPC_WORK_MODE, 0);
	CHAR_setWorkInt( meindex, NPC_WORK_ROUTEPOINT, 2);
	CHAR_setWorkInt( meindex, NPC_WORK_ROUNDTRIP, 0);
	CHAR_setWorkInt( meindex, NPC_WORK_CURRENTROUTE, 0);
			
	CHAR_setInt( meindex, CHAR_LOOPINTERVAL, 
						NPC_BUS_WAITINGMODE_WAITTIME);
    
    /* 蜇箕及凛棉毛本永玄 */
    CHAR_setWorkInt( meindex, NPC_WORK_CURRENTTIME, NowTime.tv_sec);

    for( i = 0; i < CHAR_PARTYMAX; i ++) {
    	CHAR_setWorkInt( meindex, CHAR_WORKPARTYINDEX1 + i, -1);
    }
	
	/* 伙□玄瑁烂允月 */
{
	int rev;
	int r = CHAR_getWorkInt( meindex, NPC_WORK_ROUTEMAX);
	CHAR_setWorkInt( meindex, NPC_WORK_CURRENTROUTE, RAND( 1, r));
	//print( "route:%d\n",CHAR_getWorkInt( meindex, NPC_WORK_CURRENTROUTE));

	/*   欠旦正□玄 */
	rev = NPC_Util_GetNumFromStrWithDelim( argstr, "reverse");

	if( rev == 1 ) {
		int num = NPC_BusGetRoutePointNum( meindex, argstr);

		if( num <= 0 ) {
			print( "npcbus:真奇怪！\n");
			return FALSE;
		}
		CHAR_setWorkInt( meindex, NPC_WORK_ROUTEPOINT, num-1);
		CHAR_setWorkInt( meindex, NPC_WORK_ROUNDTRIP, 1);
	}
	/* 伙□玄毛本永玄允月 */
	NPC_BusSetPoint( meindex, argstr);
	/* 垫五燮毛  憎允月 */
	NPC_BusSetDestPoint( meindex, argstr);
}

    return TRUE;
}


/*********************************
*   仄井仃日木凶凛及质  
*********************************/
void NPC_BusTalked( int meindex , int talkerindex , char *szMes ,
                     int color )
{
    int i;
    int	partyflg = FALSE;
	
    /* 皿伊奶乩□卞覆仄化分仃  杀允月 */
    if( CHAR_getInt( talkerindex , CHAR_WHICHTYPE ) != CHAR_TYPEPLAYER ) {
    	return;
    }
	/* 愤坌及由□  奴  昙菸  井升丹井譬屯月 */
	for( i = 0; i < CHAR_PARTYMAX; i ++ ) {
		int index = CHAR_getWorkInt( meindex, CHAR_WORKPARTYINDEX1+i);
		if( CHAR_CHECKINDEX(index)){
			if( index == talkerindex) {
				partyflg = TRUE;
			}
		}
	}
	if( !partyflg ) {
		//NPC_BusCheckJoinParty( meindex, talkerindex, TRUE);
	}
	else {
		if( CHAR_getWorkInt( meindex, NPC_WORK_MODE) == 0 ) {
			int i;
	//		#define NPC_BUS_DEBUGROUTINTG	"routingtable:"
			if( strstr( szMes, "出发" )  ||
				strstr( szMes, "出发" )  ||
				strstr( szMes, "Go" )  ||
				strstr( szMes, "go" ))
			{
				CHAR_setWorkInt( meindex, NPC_WORK_MODE,1);
				
				/* 伙□皿楮醒及裟请仄毛汹仁簧蘸卞允月 */
	 			CHAR_setInt( meindex, CHAR_LOOPINTERVAL, NPC_BUS_LOOPTIME);
				/* SE   日允  穴件乒旦及陲太   */
				if( CHAR_getWorkInt( meindex, NPC_WORK_SEFLG )) {
					CHAR_sendSEoArroundCharacter( 
									CHAR_getInt( meindex, CHAR_FLOOR),
									CHAR_getInt( meindex, CHAR_X),
									CHAR_getInt( meindex, CHAR_Y),
									60,
									TRUE);
				}
				/* 请  允月凛及丢永本□斥*/
				for( i = 1; i < CHAR_PARTYMAX; i ++ ) {
					int partyindex = CHAR_getWorkInt( meindex, CHAR_WORKPARTYINDEX1+i);
					if( CHAR_CHECKINDEX( partyindex)) {
						NPC_BusSendMsg( meindex, partyindex, NPC_BUS_MSG_START);
					}
				}
			}
		}
	}
}
/**************************************
 * 伙□皿楮醒
 **************************************/
void NPC_BusLoop( int meindex)
{
	int	i;
	switch( CHAR_getWorkInt( meindex, NPC_WORK_MODE )) {
	  case 0:
	    /* 谨切乒□玉及凛］凛棉毛民尼永弁允月 */
		/* 凛棉互烦匀凶及匹］请  允月 */
		if( CHAR_getWorkInt( meindex, NPC_WORK_CURRENTTIME) 
			+ CHAR_getWorkInt( meindex, NPC_WORK_WAITTIME) 
			< NowTime.tv_sec)
		{
			/* SE   日允  穴件乒旦及陲太   */
			if( CHAR_getWorkInt( meindex, NPC_WORK_SEFLG )) {
				CHAR_sendSEoArroundCharacter( 
								CHAR_getInt( meindex, CHAR_FLOOR),
								CHAR_getInt( meindex, CHAR_X),
								CHAR_getInt( meindex, CHAR_Y),
								60,
								TRUE);
			}
			/* 请  允月凛及丢永本□斥*/
			for( i = 1; i < CHAR_PARTYMAX; i ++ ) {
				int partyindex = CHAR_getWorkInt( meindex, CHAR_WORKPARTYINDEX1+i);
				if( CHAR_CHECKINDEX( partyindex)) {
					NPC_BusSendMsg( meindex, partyindex, NPC_BUS_MSG_START);
				}
			}
			
			CHAR_setWorkInt( meindex, NPC_WORK_MODE,1);
			/* 伙□皿楮醒及裟请仄毛汹仁簧蘸卞允月 */
			CHAR_setInt( meindex, CHAR_LOOPINTERVAL, NPC_BUS_LOOPTIME);
		}
		return;
	  case 1:
	  	/* 汹仁 */
	  	NPC_Bus_walk( meindex);
	  case 2:
		/* 鞅引匀化中月乒□玉 */
		/* 凛棉互烦匀凶及匹］请  允月 */
		if( CHAR_getWorkInt( meindex, NPC_WORK_CURRENTTIME) 
			+ (CHAR_getWorkInt( meindex, NPC_WORK_WAITTIME) /3)
			< NowTime.tv_sec)
		{
			CHAR_setWorkInt( meindex, NPC_WORK_MODE,1);
			/* 伙□皿楮醒及裟请仄毛汹仁簧蘸卞允月 */
			CHAR_setInt( meindex, CHAR_LOOPINTERVAL, NPC_BUS_LOOPTIME);
		
		}
		return;
	  case 3:
		/* 谗邋仄化手］弁仿奶失件玄及  樟谨切及啃卞］
		 * 剂仄仇仇匹它尼奶玄毛中木化支月
		 */
		if( CHAR_getWorkInt( meindex, NPC_WORK_CURRENTTIME) + 3
			< NowTime.tv_sec)
		{
			char	argstr[NPC_UTIL_GETARGSTR_BUFSIZE - 1024 * 20];

			NPC_Util_GetArgStr( meindex, argstr, sizeof( argstr));
			/* 伙□皿楮醒及裟请仄毛赞仁允月 */
			CHAR_setInt( meindex, CHAR_LOOPINTERVAL, 
						NPC_BUS_WAITINGMODE_WAITTIME);
			
			/* 伙□玄瑁烂允月 */
			{
				int r = CHAR_getWorkInt( meindex, NPC_WORK_ROUTEMAX);
				CHAR_setWorkInt( meindex, NPC_WORK_CURRENTROUTE, RAND( 1, r));
				//print( "route:%d\n",CHAR_getWorkInt( meindex, NPC_WORK_CURRENTROUTE));
			}
			/* 垫五窖曰白仿弘  凳 */
			CHAR_setWorkInt( meindex, NPC_WORK_ROUNDTRIP, 
							CHAR_getWorkInt( meindex, NPC_WORK_ROUNDTRIP)^1);

			/* 戚禾奶件玄及譬濡 */
			/* 窖曰反  溃质   */
			if( CHAR_getWorkInt( meindex, NPC_WORK_ROUNDTRIP) == 1)  {
				/* 公及伙□玄及  嫖禾奶件玄醒毛  月 */
				int num = NPC_BusGetRoutePointNum( meindex, argstr);
				CHAR_setWorkInt( meindex, NPC_WORK_ROUTEPOINT, num-1);
			}
			else {
				CHAR_setWorkInt( meindex, NPC_WORK_ROUTEPOINT, 
							CHAR_getWorkInt( meindex, NPC_WORK_ROUTEPOINT) +1);
			}
			/* 戚及禾奶件玄及甄  毛本永玄允月 */
			NPC_BusSetPoint( meindex, argstr);
			/* 垫五燮毛  憎允月 */
			NPC_BusSetDestPoint( meindex, argstr);
			/* 由□  奴  仃月质  毛允月 */
			CHAR_DischargeParty( meindex, 0);
		    /* 蜇箕及凛棉毛本永玄 */
		    CHAR_setWorkInt( meindex, NPC_WORK_CURRENTTIME, NowTime.tv_sec);
			/* 乒□玉弁伉失 */
			CHAR_setWorkInt( meindex, NPC_WORK_MODE, 0);
		}
		return;
	  default:
	    break;
	}
}
/**************************************
 * 汹仁［
 **************************************/
static void NPC_Bus_walk( int meindex)
{	
	POINT	start, end;
	int dir;
	int ret;
	int i;

	/* 汹仁楮溢 */
	/* 谗邋仄凶凛及质   */
	start.x = CHAR_getInt( meindex, CHAR_X);
	start.y = CHAR_getInt( meindex, CHAR_Y);
	end.x = CHAR_getWorkInt( meindex, NPC_WORK_ROUTETOX);
	end.y = CHAR_getWorkInt( meindex, NPC_WORK_ROUTETOY);
	
	/* 谗邋仄凶及匹戚及禾奶件玄卞 */
	if( start.x == end.x && start.y == end.y ) {
		int add = 1;
		char	argstr[NPC_UTIL_GETARGSTR_BUFSIZE - 1024 * 20];

		NPC_Util_GetArgStr( meindex, argstr, sizeof( argstr));

		if( CHAR_getWorkInt( meindex, NPC_WORK_ROUNDTRIP ) == 1 ) {
			add *= -1;
		}
		CHAR_setWorkInt( meindex, NPC_WORK_ROUTEPOINT, 
						CHAR_getWorkInt( meindex, NPC_WORK_ROUTEPOINT) +add);
		if( NPC_BusSetPoint( meindex, argstr) == FALSE ) {
			/*     卞谗邋*/
			/* 谨切乒□玉卞允月 */
			CHAR_setWorkInt( meindex, NPC_WORK_MODE,3);
			
			/* SE   日允  穴件乒旦及陲太   */
			if( CHAR_getWorkInt( meindex, NPC_WORK_SEFLG )) {
				CHAR_sendSEoArroundCharacter( 
								CHAR_getInt( meindex, CHAR_FLOOR),
								CHAR_getInt( meindex, CHAR_X),
								CHAR_getInt( meindex, CHAR_Y),
								60,
								TRUE);
			}
			/* 邋中凶凛及丢永本□斥*/
			for( i = 1; i < CHAR_PARTYMAX; i ++ ) {
				int partyindex = CHAR_getWorkInt( meindex, CHAR_WORKPARTYINDEX1+i);
				if( CHAR_CHECKINDEX( partyindex)) {
					NPC_BusSendMsg( meindex, partyindex, NPC_BUS_MSG_END);
				}
			}
		    /* 蜇箕及凛棉毛本永玄 */
		    CHAR_setWorkInt( meindex, NPC_WORK_CURRENTTIME, NowTime.tv_sec);
			return;
		}
		else {
			return;
		}
	}
	/*-------------------------------------------------------*/
	/* 汹井六月质   */
	
	/*   轾毛菲户月 */
	dir = NPC_Util_getDirFromTwoPoint( &start,&end );

	/* 漆中月桦赭及谨    由□  奴汹五匹银丹   */
	end.x = CHAR_getInt( meindex, CHAR_X);
	end.y = CHAR_getInt( meindex, CHAR_Y);
	
	/* 夫匀井井匀凶凛及啃及质   */
	for( i = 0; i < 100; i ++ ) {	
		if( dir < 0 ) {
			dir = RAND( 0,7);
		}	
		dir = NPC_Util_SuberiWalk( meindex, dir);
		if( dir >= 0 && dir <= 7) break;
	}
	
	if( dir >= 0 && dir <= 7 ) {
		/* 汹仁 */
		ret = CHAR_walk( meindex, dir, 0);

		if( ret == CHAR_WALKSUCCESSED ) {
			/* 愤坌互褪卅日醮棉毛汹井六月 */
			int	i;
			for( i = 1; i < CHAR_PARTYMAX; i ++ ) {
				int toindex = CHAR_getWorkInt( meindex, i + CHAR_WORKPARTYINDEX1);
				if( CHAR_CHECKINDEX(toindex) ) {
					int		parent_dir;
					/* 阂及匏  午］褪及汹五蟆及匏  井日  轾毛菲户月 */
					/* 汹仁 */
					start.x = CHAR_getInt( toindex, CHAR_X);
					start.y = CHAR_getInt( toindex, CHAR_Y);
					parent_dir = NPC_Util_getDirFromTwoPoint( &start,&end );
					/* 弘仿犯奴它旦左皿扑亦件汹五毛  蜇允月啃卞］
					 * 戚及阂反蟆及阂及  毛馨丹方丹卞允月
					 */
					end = start;
					if( parent_dir != -1 ) {
						CHAR_walk( toindex, parent_dir, 0);
					}
				}
			}
	    }
	}
}
/**************************************
 * 戚及桦赭毛本永玄允月
 **************************************/
static int NPC_BusSetPoint( int meindex, char *argstr)
{
	char	buf[4096];
	char	buf2[256];
	char	buf3[256];
	int ret;
	char routetostring[64];
	
	snprintf( routetostring, sizeof( routetostring), "routeto%d", 
				CHAR_getWorkInt( meindex, NPC_WORK_CURRENTROUTE));
	
	if( NPC_Util_GetStrFromStrWithDelim( argstr, routetostring,buf, sizeof(buf))
		== NULL ) 
	{
		print( "npcbus:nothing route \n");
		return FALSE;
	}
	ret = getStringFromIndexWithDelim( buf, ";", 
								CHAR_getWorkInt( meindex, NPC_WORK_ROUTEPOINT),
								buf2, sizeof(buf2));
		
	if( ret == FALSE ) return FALSE;

	ret = getStringFromIndexWithDelim( buf2, ",", 1,
								buf3, sizeof(buf3));
	if( ret == FALSE) return FALSE;
	CHAR_setWorkInt( meindex, NPC_WORK_ROUTETOX, atoi( buf3));
	
	ret = getStringFromIndexWithDelim( buf2, ",", 2,
								buf3, sizeof(buf3));
	if( ret == FALSE) return FALSE;
	CHAR_setWorkInt( meindex, NPC_WORK_ROUTETOY, atoi( buf3));
	return TRUE;
}
/**************************************
 * route  寞井日］  蟆互丐匀凶日公木毛
 * 惫寞及午仇卞本永玄允月［
 **************************************/
static void NPC_BusSetDestPoint( int meindex, char *argstr)
{
	char 	buf[256];
	char	routename[256];

	snprintf( routename, sizeof( routename), "routename%d", 
				CHAR_getWorkInt( meindex, NPC_WORK_CURRENTROUTE));

	if( NPC_Util_GetStrFromStrWithDelim( argstr, routename, buf, sizeof( buf))
		!= NULL ) 
	{
		CHAR_setChar( meindex, CHAR_OWNTITLE, buf);
		CHAR_sendCToArroundCharacter( CHAR_getWorkInt( meindex, CHAR_WORKOBJINDEX));
	}
}
/**************************************
 * 隙烂今木凶失奶  丞毛  匀化中月井民尼永弁允月
 *   匀化中凶日分户
 **************************************/
static BOOL NPC_BusCheckDeniedItem( int meindex, int charaindex, char *argstr)
{
	char	buf[1024];
	BOOL	found = TRUE;

	if( NPC_Util_GetStrFromStrWithDelim( argstr, "denieditem", buf, sizeof( buf))
		!= NULL ) 
	{
		int	i;
		int ret;
		for( i = 1; ; i ++) {
			int itemid;
			char buf2[64];
			int j;
			ret = getStringFromIndexWithDelim( buf, ",", i, buf2, sizeof(buf2));
			if( ret == FALSE ) break;
			itemid = atoi( buf2);
			for( j = 0; j < CHAR_MAXITEMHAVE; j ++) {
				int itemindex = CHAR_getItemIndex( charaindex, j);
				if( ITEM_CHECKINDEX( itemindex)) {
					if( ITEM_getInt( itemindex, ITEM_ID) == itemid) {
						found = FALSE;
						break;
					}
				}
			}
		}
	}
	return found;
}
/**************************************
 * 隙烂今木凶失奶  丞毛  匀化中月井民尼永弁允月
 *   匀化中卅中午分户
 **************************************/
BOOL NPC_BusCheckAllowItem( int meindex, int charaindex, BOOL pickupmode)
{
	char	buf[1024];
	BOOL	found = TRUE;
	BOOL	pickup = FALSE;
	char	argstr[NPC_UTIL_GETARGSTR_BUFSIZE - 1024 * 20];
	
	NPC_Util_GetArgStr( meindex, argstr, sizeof( argstr));
	
	if( NPC_Util_GetStrFromStrWithDelim( argstr, "pickupitem", buf, sizeof( buf))!= NULL ){
		pickup = TRUE;
	}
	if( NPC_Util_GetStrFromStrWithDelim( argstr, "allowitem", buf, sizeof( buf))!= NULL ){
		int	i;
		int ret;
		for( i = 1; ; i ++) {
			int itemid;
			char buf2[64];
			int j;
			BOOL	getflg;
			ret = getStringFromIndexWithDelim( buf, ",", i, buf2, sizeof(buf2));
			if( ret == FALSE ) break;
			itemid = atoi( buf2);
			getflg = FALSE;
			for( j = 0; j < CHAR_MAXITEMHAVE; j ++) {
				int itemindex = CHAR_getItemIndex( charaindex, j);
				if( ITEM_CHECKINDEX( itemindex)) {
					if( ITEM_getInt( itemindex, ITEM_ID) == itemid) {
						if( pickupmode && pickup && !getflg) {
							CHAR_DelItem( charaindex, j);
							getflg = TRUE;
						}
						break;
					}
				}
			}
			if( j == CHAR_MAXITEMHAVE) {
				found = FALSE;
				break;
			}
		}
	}
	return found;
}

/**************************************
 * 隙烂今木凶伊矛伙动晓井民尼永弁允月
 **************************************/
static BOOL NPC_BusCheckLevel( int meindex, int charaindex, char *argstr)
{
	int		level;
	
	/* 卅仃木壬中仃卅中娄醒及民尼永弁 */
	level = NPC_Util_GetNumFromStrWithDelim( argstr, "needlevel");
	if( level == -1 ) {
		return TRUE;
	}
	if( CHAR_getInt( charaindex, CHAR_LV) >= level ) return TRUE;
	
	return FALSE;
}
/**************************************
 * 豢嗯毛民尼永弁允月
 * -1 蛲   0动晓”    ］井勾  邰Stone
 **************************************/
static int NPC_BusCheckStone( int meindex, int charaindex, char *argstr)
{
	int		gold;
	
	/* 卅仃木壬中仃卅中娄醒及民尼永弁 */
	gold = NPC_Util_GetNumFromStrWithDelim( argstr, "needstone");
	if( gold == -1 ) {
		return 0;
	}
	if( CHAR_getInt( charaindex, CHAR_GOLD) >= gold ) return gold;
	
	return -1;
}
/**************************************
 * 丢永本□斥毛霜月
 * 娄醒及丢永本□斥互卅仃木壬犯白巧伙玄丢永本□斥毛霜月
 **************************************/
static void NPC_BusSendMsg( int meindex, int talkerindex, int tablenum)
{
	char	argstr[NPC_UTIL_GETARGSTR_BUFSIZE - 1024 * 20];
	char	buf[256];
	char	msg[256];
	if( tablenum < 0 || tablenum >= arraysizeof( busmsg)) return;
	
	NPC_Util_GetArgStr( meindex, argstr, sizeof( argstr));
	
	if( NPC_Util_GetStrFromStrWithDelim( argstr, busmsg[tablenum].option, buf, sizeof( buf))
		!= NULL ) 
	{
		strcpy( msg, buf);
	}
	else {
		snprintf( msg, sizeof(msg),busmsg[tablenum].defaultmsg);
	}
	CHAR_talkToCli( talkerindex, meindex, msg, CHAR_COLORYELLOW);
}
/**************************************
 * 伙□玄  □皮伙及禾奶件玄及醒毛潸  允月
 **************************************/
static int NPC_BusGetRoutePointNum( int meindex, char *argstr )
{
	int		i;
	char	buf[4096];
	char	buf2[256];
	int ret;
	char routetostring[64];
	
	snprintf( routetostring, sizeof( routetostring), "routeto%d", 
				CHAR_getWorkInt( meindex, NPC_WORK_CURRENTROUTE));
	
	if( NPC_Util_GetStrFromStrWithDelim( argstr, routetostring,buf, sizeof(buf))
		== NULL ) 
	{
		print( "npcbus:nothing route \n");
		return -1;
	}
	for( i = 1; ; i ++ ) {
		ret = getStringFromIndexWithDelim( buf, ";", i, buf2, sizeof(buf2));
		if( ret == FALSE) break;
	}
	return( i -1);
}
BOOL NPC_BusCheckJoinParty( int meindex, int charaindex, BOOL msgflg)
{
    //int		fd;
	char	argstr[NPC_UTIL_GETARGSTR_BUFSIZE - 1024 * 20];
	int		ret;
	NPC_Util_GetArgStr( meindex, argstr, sizeof( argstr));

	/* ㄠ弘伉永玉动  及心 */
	if( !NPC_Util_charIsInFrontOfChar( charaindex, meindex, 1 )) return FALSE; 
	/*     昙乐反蛐  允月 */
	if( CHAR_getWorkInt( meindex, NPC_WORK_MODE) != 0 ) {
		if( msgflg) NPC_BusSendMsg( meindex, charaindex, NPC_BUS_MSG_GETTINGON);
		return FALSE;
	}
	/* 天□化不分匀凶日分户 */
	if( CHAR_getWorkInt( charaindex, CHAR_WORKPARTYMODE ) != CHAR_PARTY_NONE) {
		if( msgflg) NPC_BusSendMsg( meindex, charaindex, NPC_BUS_MSG_NOTPARTY);
		return FALSE;
	}
	/* 由□  奴及谛醒毛民尼永弁允月 */
	if( CHAR_getEmptyPartyArray( meindex) == -1 ) {
		if( msgflg) NPC_BusSendMsg( meindex, charaindex, NPC_BUS_MSG_OVERPARTY);
		return FALSE;
	}
	/* 失奶  丞及民尼永弁毛允月(嗟鞅失奶  丞) */
	if( !NPC_BusCheckDeniedItem( meindex, charaindex, argstr)) {
		if( msgflg) NPC_BusSendMsg( meindex, charaindex, NPC_BUS_MSG_DENIEDITEM);
		return FALSE;
	}
#ifdef _ITEM_CHECKWARES
	if( CHAR_CheckInItemForWares( charaindex, 0) == FALSE )	{
		CHAR_talkToCli( charaindex, -1, "无法携带货物上车。", CHAR_COLORYELLOW);
		return FALSE;
	}
#endif
	/* 失奶  丞及民尼永弁毛允月(  邰失奶  丞) */
	if( !NPC_BusCheckAllowItem( meindex, charaindex, FALSE)) {
		if( msgflg) NPC_BusSendMsg( meindex, charaindex, NPC_BUS_MSG_ALLOWITEM);
		return FALSE;
	}
	/* 伊矛伙及民尼永弁毛允月 */
	if( !NPC_BusCheckLevel( meindex, charaindex, argstr)) {
		if( msgflg) NPC_BusSendMsg( meindex, charaindex, NPC_BUS_MSG_LEVEL);
		return FALSE;
	}
	/* 奶矛件玄  井民尼永弁允月 */
//	if( CHAR_getInt( charaindex, CHAR_NOWEVENT) != 0 ||
//		CHAR_getInt( charaindex, CHAR_NOWEVENT2) != 0 ||
//		CHAR_getInt( charaindex, CHAR_NOWEVENT3) != 0 )
//	{
//		if( msgflg) NPC_BusSendMsg( meindex, charaindex, NPC_BUS_MSG_EVENT);
//		return FALSE;
//	}
	/* 豢嗯及民尼永弁毛允月  云嗯毛潸月及匹］  蔽民尼永弁卞允月仇午″   */
	ret = NPC_BusCheckStone( meindex, charaindex, argstr);
	if( ret == -1 ) {
		if( msgflg) NPC_BusSendMsg( meindex, charaindex, NPC_BUS_MSG_GOLD);
		return FALSE;
	}
	if( ret != 0 ) {
		char msgbuf[128];
		/* 豢嗯毛午月 */
		CHAR_setInt( charaindex, CHAR_GOLD, 
					CHAR_getInt( charaindex, CHAR_GOLD) - ret);
		/* 霜耨 */
		CHAR_send_P_StatusString( charaindex, CHAR_P_STRING_GOLD);
		snprintf( msgbuf, sizeof( msgbuf), "支付了%d Stone！", ret);
		CHAR_talkToCli( charaindex, -1, msgbuf, CHAR_COLORYELLOW);
	}
	/* 由□  奴卞  月 */
	//CHAR_JoinParty_Main( charaindex, meindex);
	
	//fd = getfdFromCharaIndex( charaindex );
	
	//lssproto_PR_send( fd, 1, 1);
	
	
	return TRUE;
}
