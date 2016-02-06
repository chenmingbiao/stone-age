#include "version.h"
#include <string.h>
#include "object.h"
#include "char_base.h"
#include "char.h"
#include "item.h"
#include "util.h"
#include "handletime.h"
#include "npc_doorman.h"
#include "npc_door.h"
#include "npcutil.h"
#include "configfile.h"
/*
 *
 *    曰卞玉失互丐月午五卞公及玉失毛窒日井及椭瘀卞方匀化钒仁NPC.
 *  玉失及午卅曰卞create允月分仃匹｝公及玉失毛丑综今六月仇午互匹五月［
 *  反卅仄井仃日木凶午五卞蝈平乓仿毛腹绸仄化｝引歹曰8穴旦卞玉失互中月
 *  桦宁反公及允屯化卞覆仄化排黍允月［孔凶勾及玉失互  凛卞钒仁仇午卞卅月［
 *
 *  奶件正□白尼奶旦反Talk匹
 *
 * 玉失毛钒仁凶户卞
 *
 * 1 云嗯毛蔼璋允月［蔼璋匹五凶日夫日仁     gold|100
 * 2 失奶  丞毛1蜊蔼璋允月 ［蔼璋匹五凶日夫日仁  item|45
 * 3 失奶  丞毛  匀化中月井升丹井譬屯月［   匀化中凶日钒仁［itemhave|44
 * 4 失奶  丞毛  匀化中卅中井升丹井譬屯月［  匀化中卅井匀凶日钒仁［
 *          itemnothave|333
 * 5 惫寞毛手匀化中月井升丹井譬屯月［  匀化中凶日钒仁［ titlehave|string
 * 6 惫寞毛手匀化中卅中井升丹井譬屯月［  匀化中卅井匀凶日钒仁［
 *      titlenothave|string
 *
 * 井卅日内撙  卞蚕尹月午钒仁［嗯及桦宁反｝
 *＞100打□伙玉中凶分五引允互中中匹允井＂＝匹＞反中＝午中丹午100打□伙玉
 * 午日木月［中五卅曰＞反中＝分仃蜕匀化手午日木月［匹｝＞100打□伙玉
 * 中凶分五引仄凶［＝午蜕歹木月［
 *
 * 失奶  丞蔼璋及桦宁反｝＞窒★毛域蜊中凶分五引允互中中匹允井＂＝午五仁［
 *  3井日6及桦宁反｝窒井反卅仄井仃化椭瘀互公欠匀化凶日钒仁［
 *
 *
 *
 *    旦玄及  芊
 *
 *1  玉失毛化五午丹卞  仁
 *2  仇及NPC毛赝癫卞玉失及午卅曰卞  仁［娄醒毛 gold|100 卞允月
 *3  仇及NPC卞覆仄化｝100打□伙玉动晓手匀化中月橇谪匹＞反中＝午蜕丹
 *4  玉失互夫日中化嗯互蛹匀凶日岳  ［
 *
 */

static void NPC_DoormanOpenDoor( char *nm  );

BOOL NPC_DoormanInit( int meindex )
{
	char	arg[NPC_UTIL_GETARGSTR_BUFSIZE];
    char dname[1024];

	/* 奶矛件玄及正奶皿涩烂 */
	CHAR_setWorkInt( meindex, CHAR_WORKEVENTTYPE,CHAR_EVENT_NPC);

    CHAR_setInt( meindex , CHAR_HP , 0 );
    CHAR_setInt( meindex , CHAR_MP , 0 );
    CHAR_setInt( meindex , CHAR_MAXMP , 0 );
    CHAR_setInt( meindex , CHAR_STR , 0 );
    CHAR_setInt( meindex , CHAR_TOUGH, 0 );
    CHAR_setInt( meindex , CHAR_LV , 0 );

    CHAR_setInt( meindex , CHAR_WHICHTYPE , CHAR_TYPETOWNPEOPLE );
    CHAR_setFlg( meindex , CHAR_ISOVERED , 0 );
    CHAR_setFlg( meindex , CHAR_ISATTACKED , 0 );  /*   猾今木卅中方氏 */

	NPC_Util_GetArgStr( meindex, arg, sizeof( arg));

    if(!getStringFromIndexWithDelim( arg, "|", 3, dname, sizeof(dname ))){
        print("RINGO: 设定看门者时需要门的名字唷！:%s:\n",
              arg );
        return FALSE;
    }
    print( "RINGO: Doorman create: arg: %s dname: %s\n",arg,dname);
    CHAR_setWorkChar( meindex , CHAR_WORKDOORMANDOORNAME , dname );

    return TRUE;
}

void NPC_DoormanTalked( int meindex , int talkerindex , char *msg ,
                     int color )
{
    char mode[128];
    char opt[256];
    char	arg[NPC_UTIL_GETARGSTR_BUFSIZE];

    /* 皿伊奶乩□互玉失穴件及1弘伉永玉动  卅日反氏及丹 */
    if(NPC_Util_CharDistance( talkerindex, meindex ) > 1)return;

	NPC_Util_GetArgStr( meindex, arg, sizeof( arg));

    if( !getStringFromIndexWithDelim( arg, "|", 1, mode, sizeof( mode )))
        return;

    if( !getStringFromIndexWithDelim( arg, "|", 2, opt, sizeof( opt ) ))
        return;

    if( strcmp( mode , "gold" ) == 0 ){
        int g = atoi( opt );
        int yn = NPC_Util_YN( msg );
        /*char *nm = CHAR_getChar( meindex , CHAR_NAME );*/
        char msg[256];

        if( g > 0 && yn < 0 ){
            snprintf( msg ,sizeof( msg ) ,
                      "打开门需要给我%d的金子这样可以吗？", g );
            CHAR_talkToCli( talkerindex, meindex , msg, CHAR_COLORWHITE );
        } else if( g > 0 && yn == 0 ){
            snprintf( msg , sizeof( msg ),
                      "打开门 %d的金子是必要的。", g );
        } else if( g > 0 && yn == 1 ){
            int now_g = CHAR_getInt( talkerindex, CHAR_GOLD );
            if( now_g < g ){
                snprintf( msg , sizeof( msg ) ,
                          "打开门 %d的金子是必要的。", g );
            	CHAR_talkToCli( talkerindex, meindex , msg, CHAR_COLORWHITE );
            } else {
                snprintf( msg , sizeof( msg ),
                          "%d 收到金子了。现在就来开门。", g );
            	CHAR_talkToCli( talkerindex, meindex , msg, CHAR_COLORWHITE );

                /* 云嗯毛必永玄 */
                now_g -= g;
                CHAR_setInt( talkerindex , CHAR_GOLD , now_g );
                /* 丐凶日仄中旦  □正旦毛霜耨 */
                CHAR_send_P_StatusString(talkerindex, CHAR_P_STRING_GOLD);

                /* 玉失夫日仁 */
                NPC_DoormanOpenDoor(
                    CHAR_getWorkChar( meindex, CHAR_WORKDOORMANDOORNAME));
            }
        }
    } else if( strcmp( mode , "item" ) == 0 ){
        CHAR_talkToCli( talkerindex, meindex ,
                        "尚在未支援模式。",
                        CHAR_COLORWHITE);
    } else if( strcmp( mode , "itemhave" ) == 0 ){
        CHAR_talkToCli( talkerindex, meindex ,
                        "尚在未支援模式。",
                        CHAR_COLORWHITE);
    } else if( strcmp( mode , "itemnothave" ) == 0 ){
        CHAR_talkToCli( talkerindex, meindex ,
                        "尚在未支援模式。",
                        CHAR_COLORWHITE);
    } else if( strcmp( mode , "titlehave" ) == 0 ){
        CHAR_talkToCli( talkerindex, meindex ,
                        "尚在未支援模式。",
                        CHAR_COLORWHITE);

    } else if( strcmp( mode , "roomlimit" ) == 0 ){

		/*   盒及谛醒孺蜃互丐月桦宁 */
		char szOk[256], szNg[256], szBuf[32];
		int checkfloor;
		int maxnum, i, iNum;

	    if( !getStringFromIndexWithDelim( arg, "|", 2, szBuf, sizeof( szBuf ) ))
    	    return;

		/* 譬屯月白夫失午    谛醒 */
		if( sscanf( szBuf, "%d:%d", &checkfloor, &maxnum ) != 2 ){
			return;
		}

		for( iNum = 0,i = 0; i < getFdnum(); i ++ ){
			/* 皿伊奶乩□动陆卞反饬  互  中 */
			if( CHAR_getCharUse( i ) == FALSE )continue;
			if( CHAR_getInt( i, CHAR_WHICHTYPE ) != CHAR_TYPEPLAYER )continue;
			/* 隙烂及白夫失动陆卞饬  互  中 */
			if( CHAR_getInt( i, CHAR_FLOOR ) != checkfloor )continue;
			iNum++;
		}
	    if( !getStringFromIndexWithDelim( arg, "|", 5, szNg, sizeof( szNg ))){
   			strcpy( szNg, "。。。。" );	/* 凤伞卅仄及本伉白 */
		}
    	if( !getStringFromIndexWithDelim( arg, "|", 4, szOk, sizeof( szOk ))){
   			strcpy( szOk, "开门吧。。。" );	/* 凤伞丐曰及本伉白 */
   		}

		if( iNum >= maxnum ){
			/*     毛译尹化中月桦宁 */
	        CHAR_talkToCli( talkerindex, meindex ,szNg, CHAR_COLORWHITE);
		}else{
			/*     卞  凶卅中桦宁 */
	        CHAR_talkToCli( talkerindex, meindex ,szOk, CHAR_COLORWHITE);
            NPC_DoormanOpenDoor(
                    CHAR_getWorkChar( meindex, CHAR_WORKDOORMANDOORNAME));
		}

    } else if( strcmp( mode , "titlenothave" ) == 0 ){
        CHAR_talkToCli( talkerindex, meindex ,
                        "尚在未支援模式。",
                        CHAR_COLORWHITE);
    }
}

/*
 *    蟆匹腹绸仄化甲永玄仄凶及毛允屯化钒仁［
 *
 */
static void NPC_DoormanOpenDoor( char *nm)
{
    int doori = NPC_DoorSearchByName( nm );
    print( "RINGO: Doorman's Door: index: %d\n", doori );

    NPC_DoorOpen( doori , -1 );

}

