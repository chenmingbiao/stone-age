#include "version.h"
#include <string.h>
#include <math.h>
#include "char.h"
#include "object.h"
#include "char_base.h"
#include "npcutil.h"
#include "lssproto_serv.h"
#include "enemy.h"
#include "log.h"
#include "battle.h"
#include "readmap.h"
#include "handletime.h"
#include "npc_eventaction.h"
#include "npc_newnpcman.h"
#include "pet.h"
#define NEWNPCMAN_STANDBY	5000
#define NEWNPCMAN_WAITTIME	150
enum {
	WINDOW_START=1,
	WINDOW_SELECT1,
	WINDOW_SELECT2,
	WINDOW_SELECT3,
	WINDOW_SELECTLOSTPET,
	WINDOW_DEFIND,
	WINDOW_WARP,
	WINDOW_END,

	NPC_PROGRAMEGINEER_START=1,
	NPC_PROGRAMEGINEER_SELECT1,
	NPC_PROGRAMEGINEER_SELECT2,
	NPC_PROGRAMEGINEER_SELECT3,
	NPC_PROGRAMEGINEER_SELECTLOSTPET,
	NPC_PROGRAMEGINEER_DEFIND,
	NPC_PROGRAMEGINEER_END,
};
enum {
	NPC_WORK_CURRENTTIME = CHAR_NPCWORKINT1,
/*
	NPC_WORK_ROUTETOY = CHAR_NPCWORKINT2,
	NPC_WORK_ROUTEPOINT = CHAR_NPCWORKINT3,
	NPC_WORK_ROUNDTRIP = CHAR_NPCWORKINT4,
	NPC_WORK_MODE = CHAR_NPCWORKINT5,
	NPC_WORK_CURRENTROUTE = CHAR_NPCWORKINT6, 
	NPC_WORK_ROUTEMAX = CHAR_NPCWORKINT7,
	NPC_WORK_WAITTIME = CHAR_NPCWORKINT8,
	NPC_WORK_CURRENTTIME = CHAR_NPCWORKINT9,
	NPC_WORK_SEFLG = CHAR_NPCWORKINT10,
*/
};
/*
char UserBBIs[12][256]={
	"小矮子",	"赛亚人",	"辫子男孩",	"酷哥",			"熊皮男",	"大个",
	"小矮妹",	"熊皮妹",	"帽子妹",	"短法发夹妹",	"手套女",	"辣妹" };
char PetBBIs[4][256]={//格鲁西斯 贝鲁卡 金格萨贝鲁 贝鲁伊卡
	"格鲁西斯-红虎",	"贝鲁卡-绿虎",	"金格萨贝鲁-金虎",	"贝鲁伊卡-黄虎" };

int PlayerBBI[6*2][4]={
		{ 100000, 100005, 100010, 100015 },	//小矮子
		{ 100025, 100030, 100035, 100020 },	//赛亚人
		{ 100055, 100050, 100045, 100040 },	//辫子男孩
		{ 100060, 100065, 100070, 100075 },	//酷哥
		{ 100095, 100085, 100090, 100080 },	//熊皮男
		{ 100100, 100115, 100110, 100115 },	//大个

		{ 100135, 100120, 100125, 100130 },	//小矮妹
		{ 100145, 100140, 100150, 100155 },	//熊皮妹
		{ 100165, 100170, 100160, 100175 },	//帽子妹
		{ 100190, 100195, 100185, 100180 },	//短发发夹妹
		{ 100200, 100210, 100215, 100205 },	//手套女
		{ 100230, 100225, 100220, 100235 }	//辣妹
	};
*/

static int checkPc[48][3];

static void NPC_NewNpcMan_selectWindow( int meindex, int toindex, int num, int flg);

void NPC_reCheckMyPetUnusual( int meindex, int toindex);//检查宠物异常

BOOL CHECK_ReplacePET( int toindex); //更换宠物
void NPC_reCheckItemPilenum( int meindex, int toindex);//还原铁枪三堆叠
BOOL CHECK_ITEMEQUIT( int toindex);//更换灵力铠
BOOL CHECK_PETBBI( int toindex);//修正宠物图号

#ifdef _PET_LOSTPET
BOOL NPC_reFindMyLostPet( int meindex, int toindex, char *buf);
BOOL NPC_getLostPetString( int meindex, int toindex);
BOOL NPC_backupLostPetString( int toindex);
static char petstring[7][2048]={"","","","","","",""};
#endif

BOOL NPC_NewNpcManInit( int meindex )
{
	char npcarg[NPC_UTIL_GETARGSTR_BUFSIZE];
		
	if(NPC_Util_GetArgStr( meindex, npcarg, sizeof(npcarg))==NULL){
		print("TRANSER_MAN: GetArgStrErr!!");
		return FALSE;
	}

	{
		int i;
		for( i=0; i<48; i++){
			if( i == 0 ){
				checkPc[i][0]=30000;
				checkPc[i][1]=30024;
				checkPc[i][2]=100000;
			}else{
				checkPc[i][0] = checkPc[i-1][0] + 25;
				checkPc[i][1] = checkPc[i-1][1] + 25;
				checkPc[i][2] = checkPc[i-1][2] + 5;
			}
//			print("ANDY checkPc:%d[%d,%d,%d]\n", i, checkPc[i][0], checkPc[i][1],	checkPc[i][2]);
		}
	}
   	CHAR_setInt( meindex, CHAR_WHICHTYPE, CHAR_NEWNPCMAN);
	CHAR_setWorkInt( meindex, NPC_WORK_CURRENTTIME, NowTime.tv_sec);
	return TRUE;
}

void NPC_NewNpcManLoop( int meindex)
{

}

void NPC_NewNpcManTalked( int meindex , int talkerindex , char *msg , int color )
{
	char npcarg[NPC_UTIL_GETARGSTR_BUFSIZE];
	if( CHAR_getInt( talkerindex , CHAR_WHICHTYPE ) != CHAR_TYPEPLAYER )
    		return;
	if(NPC_Util_isFaceToFace( meindex ,talkerindex , 2) == FALSE) {
		if(NPC_Util_isFaceToChara( talkerindex, meindex, 1) == FALSE)
			return;
	}
	if(NPC_Util_GetArgStr( meindex, npcarg, sizeof(npcarg))==NULL){
		print("TRANSER_MAN: GetArgStrErr!!");
		return;
	}
	CHAR_setWorkInt( talkerindex, CHAR_WORKSHOPRELEVANT, 0);
	if( CHAR_getInt( talkerindex, CHAR_BASEBASEIMAGENUMBER) != 101578 ){
		NPC_NewNpcMan_selectWindow( meindex, talkerindex, WINDOW_SELECT1, 0);
		return;
	}
	NPC_NewNpcMan_selectWindow( meindex, talkerindex, WINDOW_START, 0);
}

static void NPC_NewNpcMan_selectWindow( int meindex, int toindex, int num, int flg)
{
	char npcarg[NPC_UTIL_GETARGSTR_BUFSIZE];
	char token[256];

	int buttontype = 0, windowtype = 0, windowno = 0;
	int fd = getfdFromCharaIndex( toindex);

	windowtype = WINDOW_MESSAGETYPE_MESSAGE;

	if(NPC_Util_isFaceToFace( meindex ,toindex , 2) == FALSE) {
		if(NPC_Util_isFaceToChara( toindex, meindex, 1) == FALSE)
			return;
	}

	memset( npcarg, 0, sizeof( npcarg));
	if(NPC_Util_GetArgStr( meindex, npcarg, sizeof(npcarg))==NULL){
		return;
	}

	memset( token, 0, sizeof( token));
	switch( num)	{
	case WINDOW_START:
		if( NPC_Util_GetStrFromStrWithDelim( npcarg, "START_MSG", token, sizeof( token) ) == NULL) {
			return;
		}
		buttontype = WINDOW_BUTTONTYPE_YESNO;
		windowno = NPC_PROGRAMEGINEER_DEFIND;
		break;
	case WINDOW_SELECT1:
		sprintf(token,"              请选择"
			      "\n             《修正宠物异常》"
				  "\n             《修正宠物图号》"
			      "\n              《更换灵力铠》"
#ifdef _PET_LOSTPET
				  "\n             《领回遗失宠物》"
#endif
//				  "\n            《还原铁枪三堆叠》"

				  );
		windowtype = WINDOW_MESSAGETYPE_SELECT;
		buttontype = WINDOW_BUTTONTYPE_CANCEL;
		windowno = NPC_PROGRAMEGINEER_SELECT2;
		break;
	case WINDOW_SELECT2:
		switch( flg){
		case 1://修正宠物异常
			NPC_reCheckMyPetUnusual( meindex, toindex);
			CHAR_talkToCli( toindex, -1, "身上宠物检查完毕!", CHAR_COLORYELLOW);
			return;
			break;
		case 2://修正宠物图号
			CHECK_PETBBI( toindex);
			return;
			break;
		case 3://更换灵力铠
			if( CHECK_ITEMEQUIT( toindex) == TRUE ){
				CHAR_talkToCli( toindex, -1, "检查完毕!", CHAR_COLORYELLOW);
			}
			return;
			break;

		case 4:
#ifdef _PET_LOSTPET
			if( NPC_reFindMyLostPet( meindex, toindex, token) == FALSE ){
				CHAR_talkToCli( toindex, -1, "查无资料！", CHAR_COLORYELLOW);
				return;
			}
			windowtype = WINDOW_MESSAGETYPE_SELECT;
			buttontype = WINDOW_BUTTONTYPE_CANCEL;
			windowno = NPC_PROGRAMEGINEER_SELECTLOSTPET;
#endif
			break;
		case 5: //还原铁枪三堆叠
//			NPC_reCheckItemPilenum( meindex, toindex);
			return;
			break;
		}
		break;
/*
	case WINDOW_SELECT3:
			{
				int petnum, petindex;
				petnum = atoi( data)-1;
				if( petnum >= CHAR_MAXPETHAVE || petnum < 0 ) return;
				petindex = CHAR_getCharPet( talker, petsel);
				if( !CHAR_CHECKINDEX(petindex) ) return;
				break;
			}
*/
	case WINDOW_SELECTLOSTPET:
#ifdef _PET_LOSTPET
		{
			int ret, i, ti, ltime, cost;
			Char ch;
			char msgbuf[256];
			char petstring1[2048];

			int havepetelement = CHAR_getCharPetElement( toindex);
			ti = flg;
			if( havepetelement < 0 ){
				CHAR_talkToCli( toindex, -1, "宠物栏位已满！", CHAR_COLORYELLOW);
				return;
			}
			if( NPC_getLostPetString( meindex, toindex) == FALSE ) {
				CHAR_talkToCli( toindex, -1, "领取宠物失败！", CHAR_COLORYELLOW);
				return;
			}

			if( getStringFromIndexWithDelim( petstring[ti-1], "#", 2, petstring1, sizeof( petstring1)) == FALSE ){
				CHAR_talkToCli( toindex, -1, "领取宠物失败！", CHAR_COLORYELLOW);
				return;	
			}else{
				char buf1[256];
				if( getStringFromIndexWithDelim( petstring[ti-1], "|", 6, buf1, sizeof( buf1)) == FALSE ) return;
					ltime = atoi(buf1);
					ltime = time( NULL) - ltime;
					ltime = ltime/(60*60*24);
				if( getStringFromIndexWithDelim( petstring[ti-1], "|", 4, buf1, sizeof( buf1)) == FALSE ) return;
					cost = atoi( buf1);

				cost = cost + (ltime*10000);

				if( CHAR_getInt( toindex, CHAR_GOLD) < cost ) {
					sprintf( buf1, "宠物已寄放%d天，共需%d石币才可领回。", ltime, cost);
					CHAR_talkToCli( toindex, meindex, buf1, CHAR_COLORYELLOW);
					return;	
				}
			}


			ret = CHAR_makePetFromStringToArg( petstring1, &ch, -2);
			strcpy( petstring[ti-1], "");
			if( ret ) {
				char buf1[256];
				int petindex = PET_initCharOneArray( &ch );
				if( !CHAR_CHECKINDEX( petindex) ){
					CHAR_talkToCli( toindex, -1, "领取宠物失败！", CHAR_COLORYELLOW);
					return;
				}
				print("ANDY petindex:%d[%s]\n", petindex, CHAR_getChar( petindex, CHAR_NAME) );

				CHAR_complianceParameter( petindex );
				CHAR_setInt( petindex, CHAR_HP, CHAR_getWorkInt( petindex, CHAR_WORKMAXHP));
				CHAR_setWorkInt( petindex, CHAR_WORKPLAYERINDEX, toindex);
				CHAR_setCharPet( toindex, havepetelement, petindex);
				CHAR_setChar( petindex, CHAR_OWNERCDKEY,
							CHAR_getChar( toindex, CHAR_CDKEY));
				CHAR_setChar( petindex, CHAR_OWNERCHARANAME,
							CHAR_getChar( toindex, CHAR_NAME));

				if( NPC_backupLostPetString( toindex) == FALSE ){
					print("ANDY err backup petstring !!\n");
					return;
				}
				
				sprintf( buf1, "LostPGet:%d", cost);
				LogPet(
					CHAR_getChar( toindex, CHAR_NAME ),
					CHAR_getChar( toindex, CHAR_CDKEY ),
					CHAR_getChar( petindex, CHAR_NAME),
					CHAR_getInt( petindex, CHAR_LV),
					buf1,
					CHAR_getInt( toindex,CHAR_FLOOR),
					CHAR_getInt( toindex,CHAR_X ),
					CHAR_getInt( toindex,CHAR_Y ),
					CHAR_getChar( petindex, CHAR_UNIQUECODE)   // shan 2001/12/14
				);
				sprintf( buf1, "领取宠物%s。", CHAR_getUseName( petindex));
				CHAR_talkToCli( toindex, meindex, buf1, CHAR_COLORYELLOW);
				for( i = 0; i < CHAR_MAXPETHAVE; i++){
					int petindex = CHAR_getCharPet( toindex, i);
					if( !CHAR_CHECKINDEX( petindex) )
						continue;
					CHAR_complianceParameter( petindex );
					snprintf( msgbuf, sizeof( msgbuf ), "K%d", i );
					CHAR_sendStatusString( toindex, msgbuf );
					snprintf( msgbuf, sizeof( msgbuf ), "W%d", i );
					CHAR_sendStatusString( toindex, msgbuf );
				}
				CHAR_DelGold( toindex, cost);
			}else{
					CHAR_talkToCli( toindex, -1, "领取宠物失败！", CHAR_COLORYELLOW);
					return;
			}
			return;
 		}
#endif
		return;
		break;
	case WINDOW_DEFIND:
		{
			int i;
			int face = CHAR_getInt( toindex, CHAR_FACEIMAGENUMBER);
			if( NPC_Util_GetStrFromStrWithDelim( npcarg, "CHECK_MSG", token, sizeof( token) ) == NULL) {
				return;
			}
			for( i=0; i<48; i++){
				if( face >= checkPc[i][0] && face < checkPc[i][1] )
					break;
			}
			if( i >= 48 )return;
			CHAR_setWorkInt( toindex, CHAR_WORKNPCMETAMO, meindex);
			CHAR_setInt( toindex, CHAR_BASEIMAGENUMBER, checkPc[i][2]);
			CHAR_complianceParameter( toindex );
			CHAR_sendCToArroundCharacter( CHAR_getWorkInt( toindex , CHAR_WORKOBJINDEX ));
			CHAR_send_P_StatusString( toindex , CHAR_P_STRING_BASEBASEIMAGENUMBER);

			buttontype = WINDOW_BUTTONTYPE_YESNO;
			windowno = NPC_PROGRAMEGINEER_END;
		}
		break;
	case WINDOW_END:
		{
			CHAR_setWorkInt( toindex, CHAR_WORKNPCMETAMO, -1);
			CHAR_setInt( toindex, CHAR_BASEBASEIMAGENUMBER,
				CHAR_getInt( toindex, CHAR_BASEIMAGENUMBER) );

			CHAR_complianceParameter( toindex );
			CHAR_sendCToArroundCharacter( CHAR_getWorkInt( toindex , CHAR_WORKOBJINDEX ));
			CHAR_send_P_StatusString( toindex , CHAR_P_STRING_BASEBASEIMAGENUMBER);
			return;
		}
		break;
	}

	lssproto_WN_send( fd, windowtype, buttontype, windowno,
		CHAR_getWorkInt( meindex, CHAR_WORKOBJINDEX), token);
}

void NPC_NewNpcManWindowTalked( int meindex, int talkerindex, int seqno, int select, char *data)
{
	if(NPC_Util_isFaceToFace( meindex ,talkerindex , 2) == FALSE) {
		if(NPC_Util_isFaceToChara( talkerindex, meindex, 1) == FALSE)
			return;
	}

	if( select == WINDOW_BUTTONTYPE_CANCEL || select == WINDOW_BUTTONTYPE_NO){
		CHAR_setWorkInt( talkerindex, CHAR_WORKSHOPRELEVANT, 0);
		CHAR_setWorkInt( talkerindex, CHAR_WORKNPCMETAMO, -1);
		return;
	}
	switch( seqno)	{
	case NPC_TRANSERMAN_START:
		break;
	case NPC_PROGRAMEGINEER_SELECT1:
		NPC_NewNpcMan_selectWindow( meindex, talkerindex, WINDOW_SELECT1, atoi( data));
		break;
	case NPC_PROGRAMEGINEER_SELECT2:
		NPC_NewNpcMan_selectWindow( meindex, talkerindex, WINDOW_SELECT2, atoi( data));
		break;
	case NPC_PROGRAMEGINEER_SELECT3:
		NPC_NewNpcMan_selectWindow( meindex, talkerindex, WINDOW_SELECT3, atoi( data));
		break;
	case NPC_PROGRAMEGINEER_SELECTLOSTPET:
		NPC_NewNpcMan_selectWindow( meindex, talkerindex, WINDOW_SELECTLOSTPET, atoi( data));
		break;
	case NPC_PROGRAMEGINEER_DEFIND:
		NPC_NewNpcMan_selectWindow( meindex, talkerindex, WINDOW_DEFIND, atoi( data));
		break;
	case NPC_PROGRAMEGINEER_END:
		NPC_NewNpcMan_selectWindow( meindex, talkerindex, WINDOW_END, atoi( data));
		break;
	}
}

BOOL CHECK_ITEMEQUIT( int toindex)
{
	int i, itemindex;
	char token[256];

	for (i = CHAR_STARTITEMARRAY; i < CHAR_MAXITEMHAVE; i ++) {
		itemindex = CHAR_getItemIndex( toindex, i);
		if( ITEM_getInt( itemindex, ITEM_ID) == 19646 ){
			CHAR_DelItem( toindex, i);
			itemindex = -1;
			itemindex = ITEM_makeItemAndRegist( 1292);
			if( !ITEM_CHECKINDEX( itemindex) ){
			}else{
				int ret = CHAR_addItemSpecificItemIndex( toindex, itemindex);
				if( ret < 0 || ret >= CHAR_MAXITEMHAVE ) {
					ITEM_endExistItemsOne( itemindex);
					continue;
				}
				sprintf( token,"拿到%s", ITEM_getChar( itemindex, ITEM_NAME));
				CHAR_talkToCli( toindex, -1, token, CHAR_COLORYELLOW );
				CHAR_sendItemDataOne( toindex, ret);
				continue;
			}
		}
		if( ITEM_getInt( itemindex, ITEM_ID) == 1292 &&
			ITEM_getInt( itemindex, ITEM_MAXDAMAGECRUSHE) > 0 ){

			ITEM_setInt( itemindex, ITEM_MAXDAMAGECRUSHE, 0);
			ITEM_setInt( itemindex, ITEM_DAMAGECRUSHE, 0);
			sprintf(token,"修正%s损坏度", ITEM_getChar( itemindex, ITEM_NAME));
			CHAR_sendItemDataOne( toindex, i);
			CHAR_talkToCli( toindex, -1, token, CHAR_COLORYELLOW);
		}
	}	

	// 寄放店
	for (i = 0; i < CHAR_MAXPOOLITEMHAVE; i++) {
		if( ITEM_getInt( itemindex, ITEM_ID) == 1292 &&
			ITEM_getInt( itemindex, ITEM_MAXDAMAGECRUSHE) > 0 ){

			ITEM_setInt( itemindex, ITEM_MAXDAMAGECRUSHE, 0);
			ITEM_setInt( itemindex, ITEM_DAMAGECRUSHE, 0);
			sprintf(token,"修正寄放店中%s损坏度", ITEM_getChar( itemindex, ITEM_NAME));
			CHAR_talkToCli( toindex, -1, token, CHAR_COLORYELLOW);
		}
	}

	return TRUE;
}


BOOL CHECK_PETBBI( int toindex)
{
	int i, petindex, PetID;
	BOOL Finds = FALSE;
	char token[256];
	for( i=0; i < CHAR_MAXPETHAVE; i++)	{
		int parry, array, PetBBI;
		petindex = CHAR_getCharPet( toindex, i);
		if( !CHAR_CHECKINDEX( petindex) ) continue;
		PetID = CHAR_getInt( petindex, CHAR_PETID);
		array = ENEMY_getEnemyArrayFromTempNo( PetID);
		if( !ENEMY_CHECKINDEX( array)) continue;
		parry = ENEMYTEMP_getEnemyTempArray( array);
		PetBBI = ENEMYTEMP_getInt( parry, E_T_IMGNUMBER);

		if( CHAR_getInt( petindex, CHAR_BASEBASEIMAGENUMBER) != PetBBI ||
			CHAR_getInt( petindex, CHAR_BASEIMAGENUMBER) != PetBBI ){
			CHAR_setInt( petindex, CHAR_BASEBASEIMAGENUMBER, PetBBI);
			CHAR_setInt( petindex, CHAR_BASEIMAGENUMBER, PetBBI);
			sprintf( token, "修正%s图号", CHAR_getUseName( petindex));
			CHAR_talkToCli( toindex, -1, token, CHAR_COLORYELLOW);
			Finds = TRUE;
		}
		CHAR_setInt( petindex, CHAR_EARTHAT, ENEMYTEMP_getInt( parry, E_T_EARTHAT) );
		CHAR_setInt( petindex, CHAR_WATERAT, ENEMYTEMP_getInt( parry, E_T_WATERAT) );
		CHAR_setInt( petindex, CHAR_FIREAT, ENEMYTEMP_getInt( parry, E_T_FIREAT) );
		CHAR_setInt( petindex, CHAR_WINDAT, ENEMYTEMP_getInt( parry, E_T_WINDAT) );
	}
	for( i=0; i<CHAR_MAXPETHAVE; i++){
		char szPet[128];
		petindex = CHAR_getCharPet(toindex, i);
		if( !CHAR_CHECKINDEX( petindex) )continue;
		CHAR_complianceParameter( petindex );
		snprintf( szPet, sizeof( szPet ), "K%d", i);
		CHAR_sendStatusString( toindex, szPet );
		snprintf( szPet, sizeof( szPet ), "W%d", i);
		CHAR_sendStatusString( toindex, szPet );
	}
	if( Finds == FALSE ){
		sprintf( token, "没有错误图号的宠物");
		CHAR_talkToCli( toindex, -1, token, CHAR_COLORYELLOW);
	}
	return Finds;
}

void NPC_reCheckMyPetUnusual( int meindex, int toindex)//检查宠物异常
{
	char *buf=NULL;
	int petindex,i;
	for( i=0; i<CHAR_MAXPETHAVE; i++)	{
		petindex = CHAR_getCharPet( toindex, i);
		if( !CHAR_CHECKINDEX( petindex) )continue;
		if( (buf=CHAR_getChar( petindex, CHAR_NAME)) == "\0" ||
			strlen( buf) <= 0 ){
			char *PetNAME;
			int array, parry, petID;

			petID = CHAR_getInt( petindex, CHAR_PETID);
			array = ENEMY_getEnemyArrayFromTempNo( petID);

			if( !ENEMY_CHECKINDEX( array)){
				print("ANDY Pet(ID:%d) err Array:%d (file:%s %d) !! \n",
					petID, array, __FILE__, __LINE__ );
				continue;
			}
			parry = ENEMYTEMP_getEnemyTempArray( array);
			if( (PetNAME = ENEMYTEMP_getChar( parry, E_T_NAME)) == "\0" ||
				strlen( PetNAME) <= 0 ){
				print("ANDY Pet(ID:%d) Array:%d err Name:%s (file:%s %d) !! \n",
					petID, array, (PetNAME==NULL)?"NULL":PetNAME, __FILE__, __LINE__ );
				continue;
			}
			CHAR_setChar( petindex, CHAR_NAME, PetNAME);
			CHAR_setChar( petindex, CHAR_USERPETNAME , "");
			//andy_log
			print("ANDY Pet(ID:%d) Name -> %s (%s:%s)(file:%s %d) !! \n",
				petID, PetNAME, CHAR_getUseName( toindex),
				CHAR_getChar( toindex, CHAR_CDKEY), __FILE__, __LINE__ );
		}
	}

	for( i=0; i<CHAR_MAXPETHAVE; i++){
		char szPet[256];
		petindex = CHAR_getCharPet(toindex, i);
		if( !CHAR_CHECKINDEX( petindex) )continue;
		CHAR_complianceParameter( petindex );
		snprintf( szPet, sizeof( szPet ), "K%d", i);
		CHAR_sendStatusString( toindex, szPet );
		snprintf( szPet, sizeof( szPet ), "W%d", i);
		CHAR_sendStatusString( toindex, szPet );
	}
}
//更换宠物
BOOL CHECK_ReplacePET( int toindex)
{
	int i, petindex;
	BOOL Finds = FALSE;
	char szPet[256];

	if( CHAR_getWorkInt( toindex, CHAR_WORKBATTLEMODE) != BATTLE_CHARMODE_NONE ) return FALSE;

	for( i=0; i<CHAR_MAXPETHAVE; i++)	{
		int array, ret;
		petindex = CHAR_getCharPet( toindex, i);
		if( !CHAR_CHECKINDEX( petindex) )continue;
		if( CHAR_getInt( petindex, CHAR_PETID) != 1133 ) continue;
		if( CHAR_getInt( petindex, CHAR_SLOT) != 7 ) continue;
		if( CHAR_getInt( petindex, CHAR_TRANSMIGRATION ) > 0 )continue;

		if( i == CHAR_getInt( toindex, CHAR_DEFAULTPET) )	{
			int fd = getfdFromCharaIndex( toindex);
			CHAR_setInt( toindex, CHAR_DEFAULTPET, -1);
			lssproto_KS_send( fd, -1, TRUE);
		}

		snprintf( szPet,sizeof( szPet), "交出%s。", CHAR_getUseName( petindex) );
		CHAR_talkToCli( toindex, -1, szPet, CHAR_COLORYELLOW);
		LogPet(
			CHAR_getChar( toindex, CHAR_NAME ),
			CHAR_getChar( toindex, CHAR_CDKEY ),
			CHAR_getChar( petindex, CHAR_NAME),
			CHAR_getInt( petindex, CHAR_LV),
			"TenseiDel",
			CHAR_getInt( toindex,CHAR_FLOOR),
			CHAR_getInt( toindex,CHAR_X ),
			CHAR_getInt( toindex,CHAR_Y ),
			CHAR_getChar( petindex, CHAR_UNIQUECODE)
		);

		Finds = TRUE;
		CHAR_setCharPet( toindex, i, -1);
		CHAR_endCharOneArray( petindex );

		array = ENEMY_getEnemyArrayFromTempNo( 1133);
		if( !ENEMY_CHECKINDEX( array)) continue;
		ret = ENEMY_createPetFromEnemyIndex( toindex, array);
		if( !CHAR_CHECKINDEX( ret ) ) continue;

		snprintf( szPet, sizeof( szPet), "拿到%s。", CHAR_getUseName( ret) );
		CHAR_talkToCli( toindex, -1, szPet,  CHAR_COLORWHITE);

		LogPet(
			CHAR_getChar( toindex, CHAR_NAME ),
			CHAR_getChar( toindex, CHAR_CDKEY ),
			CHAR_getChar( ret, CHAR_NAME),
			CHAR_getInt( ret, CHAR_LV),
			"TenseiGet",
			CHAR_getInt( toindex,CHAR_FLOOR),
			CHAR_getInt( toindex,CHAR_X ),
			CHAR_getInt( toindex,CHAR_Y ),
			CHAR_getChar( ret, CHAR_UNIQUECODE)
		);

	}

	for( i=0; i<CHAR_MAXPETHAVE; i++){
		petindex = CHAR_getCharPet(toindex, i);
		if( !CHAR_CHECKINDEX( petindex) )continue;
		CHAR_complianceParameter( petindex );
		snprintf( szPet, sizeof( szPet ), "K%d", i);
		CHAR_sendStatusString( toindex, szPet );
		snprintf( szPet, sizeof( szPet ), "W%d", i);
		CHAR_sendStatusString( toindex, szPet );
	}

	return Finds;
}
//还原铁枪三堆叠
void NPC_reCheckItemPilenum( int meindex, int toindex)
{
	int i, itemindex;
	for( i=0; i<CHAR_MAXITEMHAVE; i++){
		itemindex = CHAR_getItemIndex( toindex , i );
		if( !ITEM_CHECKINDEX( itemindex) ) continue;//ITEM_USEPILENUMS
		if( ITEM_getInt( itemindex, ITEM_ID) != 20284 ) continue;
		if( ITEM_getInt( itemindex, ITEM_CANBEPILE) != 1 ) continue;

		if( ITEM_getInt( itemindex, ITEM_USEPILENUMS) > 1 ){
			int itemID, pilenum=1, newindex;
			itemID = ITEM_getInt( itemindex, ITEM_ID);
			if( !ITEM_CHECKITEMTABLE( itemID) ) continue;
			pilenum = ITEM_getInt( itemindex, ITEM_USEPILENUMS);
			while( pilenum > 1 ){
				int ti = CHAR_findEmptyItemBox( toindex);
				if( ti == -1 ){
					CHAR_sendItemDataOne( toindex, i);
					CHAR_talkToCli( toindex, -1, "道具栏位已满。", CHAR_COLORYELLOW);
					return;
				}
				newindex = ITEM_makeItemAndRegist( itemID);
				if( !ITEM_CHECKINDEX( newindex) ) break;
				CHAR_setItemIndex( toindex , ti, newindex);
				CHAR_sendItemDataOne( toindex, ti);
				pilenum--;
				ITEM_setInt( itemindex, ITEM_USEPILENUMS, pilenum);
			}
			if( ITEM_getInt( itemindex, ITEM_CANBEPILE) != ITEMTBL_getInt( itemID, ITEM_CANBEPILE) )
				ITEM_setInt( itemindex, ITEM_CANBEPILE, ITEMTBL_getInt( itemID, ITEM_CANBEPILE) );
			CHAR_sendItemDataOne( toindex, i);
		}
	}
	CHAR_talkToCli( toindex, -1, "铁枪三处理完毕。", CHAR_COLORYELLOW);
}

#ifdef _PET_LOSTPET
BOOL NPC_reFindMyLostPet( int meindex, int toindex, char *buf)
{
	//地上0 溜宠 1 宠邮 2
	FILE *fp = NULL;
	char *CdKey=NULL;
	int lv=0, cost, ltime, count=0, i, type;
	char filename[256], line[2048], buf1[256];
	char petname[256], typebuf[256];
	char buf2[10][256]={"\n","\n","\n","\n","\n",
						"\n","\n","\n","\n","\n" };
	CdKey = CHAR_getChar( toindex, CHAR_CDKEY );
	if( CdKey == "\0" ) return FALSE;
	sprintf( filename, "lostpet/%s.txt", CdKey);
	strcpy( buf, "==宠物遗失纪录==\n");
	if( (fp=fopen( filename, "r")) == NULL ) return FALSE;

	while( fgets( line , sizeof( line ) , fp ) && count < 7){
		if( line == NULL ) continue;
		if( getStringFromIndexWithDelim( line, "|", 1, buf1, sizeof( buf1)) == FALSE ) continue;
		if( strcmp( CHAR_getChar( toindex, CHAR_CDKEY ), buf1 )) continue;
		if( getStringFromIndexWithDelim( line, "|", 6, buf1, sizeof( buf1)) == FALSE ) continue;
			ltime = atoi(buf1);
			ltime = time( NULL) - ltime;
			ltime = ltime/(60*60*24);
			if( ltime > 14 ) continue;//续放14天

		if( getStringFromIndexWithDelim( line, "|", 2, petname, sizeof( petname)) == FALSE ) continue;
		if( getStringFromIndexWithDelim( line, "|", 3, buf1, sizeof( buf1)) == FALSE ) continue;
		lv = atoi( buf1);
		if( getStringFromIndexWithDelim( line, "|", 4, buf1, sizeof( buf1)) == FALSE ) continue;
		cost = atoi( buf1);

		if( getStringFromIndexWithDelim( line, "#", 3, buf1, sizeof( buf1)) == FALSE ) continue;
		type = atoi( buf1);
		if( type == 1 ){
			strcpy( typebuf, "溜宠");
		}else if( type == 2 ){
			strcpy( typebuf, "宠邮");
		}else{
			strcpy( typebuf, "地上");
		}

		sprintf( buf2[count++], "%s LV:%d石币:%d(%d天%s)\n", petname, lv, cost, ltime, typebuf);
	}
	fclose( fp);
	for( i=0; i<7; i++){
		strcat( buf, buf2[i]);
	}
	if( count == 0 ) return FALSE;
	return TRUE;
}

BOOL NPC_getLostPetString( int meindex, int toindex)
{
	FILE *fp = NULL;
	char *CdKey=NULL;
	int count=0, i, ltime;
	char filename[256], line[2048], buf1[256];

	for( i=0; i<7; i++)
		strcpy( petstring[i], "");

	CdKey = CHAR_getChar( toindex, CHAR_CDKEY );
	if( CdKey == "\0" ) return FALSE;
	sprintf( filename, "lostpet/%s.txt", CdKey);
	if( (fp=fopen( filename, "r")) == NULL ) return FALSE;

	while( fgets( line , sizeof( line ) , fp ) && count < 7){
		if(strlen( line) <= 0 ) continue;
//		if( getStringFromIndexWithDelim( line, "#", 2, buf1, sizeof( buf1)) == FALSE ) return FALSE;
// Terry fix 读取时没有判断时间 2004/09/22
		if(getStringFromIndexWithDelim(line,"|",6,buf1,sizeof(buf1)) == FALSE) continue;
		ltime = atoi(buf1);
		ltime = time( NULL) - ltime;
		ltime = ltime/(60*60*24);
		if(ltime > 14) continue;//续放14天
// Terry end
		strcpy( petstring[count++], line);
	}

	fclose( fp);
	return TRUE;
}

BOOL NPC_backupLostPetString( int toindex)
{
	FILE *fp = NULL;
	char *CdKey=NULL;
	int i, count=0;
	char filename[256];

	CdKey = CHAR_getChar( toindex, CHAR_CDKEY );
	if( CdKey == "\0" ) return FALSE;
	sprintf( filename, "lostpet/%s.txt", CdKey);
	if( (fp=fopen( filename, "w")) == NULL ) return FALSE;

	for( i=0; i<7; i++){
		if( strlen( petstring[i]) <= 0 ) continue;
		fprintf( fp, "%s", petstring[i]);
		count++;
	}
	fclose( fp);

	if( count == 0 )
		remove( filename);

	return TRUE;
}

/*


#ifdef _NEW_WARPMAN
#include "npc_newnpcman.h"
#define NEWNPCMAN_STANDBY	5000
#define NEWNPCMAN_WAITTIME	150

enum{
NPC_NEWNPCMAN_START=21,
NPC_NEWNPCMAN_MAN,
NPC_NEWNPCMAN_END,
};

enum	{
	NEWNPC_START = 1,
	NEWNPC_MAN,
	NEWNPC_END,
};

enum {
	NPC_WORK_INDEX = CHAR_NPCWORKINT1,
	NPC_WORK_WORKTYPE = CHAR_NPCWORKINT2,
	NPC_WORK_CURRENTTIME = CHAR_NPCWORKINT3,
};

BOOL CHECK_YEARPET( int toindex);
static void NPC_NewNpcMan_selectWindow( int meindex, int toindex, int num, int flg);
int NPC_NewNpcManDelPet(int meindex,int talker, int petsel);
BOOL NPC_NewNpcManAddPet(int meindex, int talker, int petid);

//可换四种宠
static	int Re_Pet[4];
// shan
char uStr[128]="";

BOOL NPC_NewNpcManInit( int meindex )
{
	char npcarg[NPC_UTIL_GETARGSTR_BUFSIZE];
	char buf1[256],buf2[256];
	int k=1;

	if(NPC_Util_GetArgStr( meindex, npcarg, sizeof(npcarg)) == NULL)	{
		print("GambleBank:GetArgStrErr");
		return FALSE;
	}

	if( NPC_Util_GetStrFromStrWithDelim( npcarg, "REPET", buf1, sizeof( buf1)) == NULL )	{
		return FALSE;
	}

	while( getStringFromIndexWithDelim(buf1, ",", k, buf2, sizeof(buf2)) != FALSE )	{
		Re_Pet[k-1] = atoi( buf2);
		if( Re_Pet[k-1] < 0 )
			return FALSE;
		k++;
		if( (k-1) >= arraysizeof( Re_Pet) )
			break;
	}

    CHAR_setInt( meindex , CHAR_WHICHTYPE , CHAR_NEWNPCMAN );
	CHAR_setInt( meindex, CHAR_LOOPINTERVAL, NEWNPCMAN_STANDBY);
	CHAR_setWorkInt( meindex, NPC_WORK_CURRENTTIME, NowTime.tv_sec);
	CHAR_setWorkInt( meindex , NPC_WORK_INDEX, -1);
	return TRUE;
}

void NPC_NewNpcManLoop( int meindex)
{
	//test
	char npcarg[NPC_UTIL_GETARGSTR_BUFSIZE];
	char buf1[256],buf2[256];
	int k=1;

	int fulltime = NEWNPCMAN_WAITTIME;
	if( (CHAR_getWorkInt( meindex, NPC_WORK_CURRENTTIME) + fulltime) > NowTime.tv_sec )
		return;
	CHAR_setWorkInt( meindex , NPC_WORK_INDEX, -1);
	CHAR_setWorkInt( meindex, NPC_WORK_CURRENTTIME, NowTime.tv_sec);

	//test
	if(NPC_Util_GetArgStr( meindex, npcarg, sizeof(npcarg)) == NULL)	{
		print("GambleBank:GetArgStrErr");
		return;
	}

	if( NPC_Util_GetStrFromStrWithDelim( npcarg, "REPET", buf1, sizeof( buf1)) == NULL )	{
		return;
	}

	while( getStringFromIndexWithDelim(buf1, ",", k, buf2, sizeof(buf2)) != FALSE )	{
		Re_Pet[k-1] = atoi( buf2);
		if( Re_Pet[k-1] < 0 )
			return;
		k++;
		if( (k-1) >= arraysizeof( Re_Pet) )
			break;
	}
	//test end
}

void NPC_NewNpcManTalked( int meindex , int talkerindex , char *msg , int color )
{
	char buf1[256];

	if( CHAR_getInt( talkerindex , CHAR_WHICHTYPE ) != CHAR_TYPEPLAYER ) {
    		return;
	}

	if(NPC_Util_isFaceToFace( meindex ,talkerindex , 2) == FALSE) {
		if(NPC_Util_isFaceToChara( talkerindex, meindex, 1) == FALSE) return;
	}    

	if( CHAR_getWorkInt( meindex , NPC_WORK_INDEX) >= 0 )	{
			sprintf( buf1,"%s","我正忙着呢！");
			print("\n NPC_WORK_INDEX = %d ", CHAR_getWorkInt( meindex , NPC_WORK_INDEX));
			CHAR_talkToCli( talkerindex, meindex, buf1,  CHAR_COLORYELLOW);
		return;
	}else	{
		CHAR_setWorkInt( meindex , NPC_WORK_INDEX, talkerindex);
		print("\n start : NPC_WORK_INDEX = talkerindex");
		CHAR_setWorkInt( meindex, NPC_WORK_CURRENTTIME, NowTime.tv_sec);
		NPC_NewNpcMan_selectWindow( meindex, talkerindex, NEWNPC_START, 0);
	}
}

static void NPC_NewNpcMan_selectWindow( int meindex, int toindex, int num, int flg)
{
	char npcarg[NPC_UTIL_GETARGSTR_BUFSIZE];
	char token[256];
	char token_str[][56]={"ERROR_MSG","START_MSG","END_MSG","ERROR_ALL"};
	int buttontype = 0, windowtype = 0, windowno = 0;
	int fd = getfdFromCharaIndex( toindex);
	int petno = 718, petid =1479;
	int type_str = 0;
	windowtype = WINDOW_MESSAGETYPE_MESSAGE;
	
	if(NPC_Util_GetArgStr( meindex, npcarg, sizeof(npcarg)) == NULL)	{
		print("GambleBank:GetArgStrErr");
		CHAR_setWorkInt( meindex , NPC_WORK_INDEX, -1);
		return ;
	}

	if( CHAR_getWorkInt( meindex , NPC_WORK_INDEX) != toindex )	{
		sprintf( token,"%s","我正忙着呢！");
		CHAR_talkToCli( toindex, meindex, token,  CHAR_COLORYELLOW);
		return;
	}
	CHAR_setWorkInt( meindex, NPC_WORK_CURRENTTIME, NowTime.tv_sec);
	print("\n switch( num=%d)", num);
	switch( num)	{
	case NEWNPC_START:
		type_str = 1;
		buttontype = WINDOW_BUTTONTYPE_OKCANCEL;
		windowno = NPC_NEWNPCMAN_START;
		break;

	case NEWNPC_MAN:
		{
			if( CHECK_YEARPET( toindex) == TRUE )	{
				type_str = 1;
				windowtype = WINDOW_MESSAGETYPE_PETSELECT;
				buttontype = WINDOW_BUTTONTYPE_CANCEL;
				windowno = NPC_NEWNPCMAN_MAN;
			}else	{
				type_str = 3;
				buttontype = WINDOW_BUTTONTYPE_OK;
				windowno = NPC_NEWNPCMAN_END;
				CHAR_setWorkInt( meindex , NPC_WORK_INDEX, -1);
			}
		}
	break;

	case NEWNPC_END:
		type_str = 0;
		if( flg <= CHAR_MAXPETHAVE && flg >= 0 )	{
			petno = flg;

			if( ( petid = NPC_NewNpcManDelPet( meindex, toindex, petno)) >= 0 )	{
				if( NPC_NewNpcManAddPet( meindex, toindex, petid) == TRUE )	{
					type_str = 2;
				}
			}
		}
		CHAR_setWorkInt( meindex , NPC_WORK_INDEX, -1);
		buttontype = WINDOW_BUTTONTYPE_OK;
		windowno = NPC_NEWNPCMAN_END;

		break;
	}

	if(NPC_Util_GetStrFromStrWithDelim( npcarg, token_str[ type_str], token,sizeof( token) ) == NULL) {
		print("token_str[%d] err !", type_str);
		CHAR_setWorkInt( meindex , NPC_WORK_INDEX, -1);
		return;
	}

	lssproto_WN_send( fd, windowtype, buttontype, windowno,
		CHAR_getWorkInt( meindex, CHAR_WORKOBJINDEX), token);
}

void NPC_NewNpcManWindowTalked( int meindex, int talkerindex, int seqno, int select, char *data)
{

	if( CHAR_getInt( talkerindex , CHAR_WHICHTYPE ) != CHAR_TYPEPLAYER ) {
    		return;
	}
	if(NPC_Util_isFaceToFace( meindex ,talkerindex , 2) == FALSE) {
		if(NPC_Util_isFaceToChara( talkerindex, meindex, 1) == FALSE) return;
	}    

	print("\n seqno=%d, select=%d", seqno, select);
	switch( seqno)	{
	case NPC_NEWNPCMAN_START:
		if( select != WINDOW_BUTTONTYPE_YES && select != WINDOW_BUTTONTYPE_OK )	{
			CHAR_setWorkInt( meindex , NPC_WORK_INDEX, -1);
			return;
		}
		if(select==WINDOW_BUTTONTYPE_CANCEL)	{
			CHAR_setWorkInt( meindex , NPC_WORK_INDEX, -1);
		}else	{
			NPC_NewNpcMan_selectWindow( meindex, talkerindex, NEWNPC_MAN, -1);
		}
	break;

	case NPC_NEWNPCMAN_MAN:
		{
			int petnum;
			petnum = atoi( data)-1;

			print("\n petnum=%d", petnum);
			if(select==WINDOW_BUTTONTYPE_CANCEL)	{
				CHAR_setWorkInt( meindex , NPC_WORK_INDEX, -1);
			}else	{
				NPC_NewNpcMan_selectWindow( meindex, talkerindex, NEWNPC_END, petnum);
			}
		}
		break;

	case NPC_NEWNPCMAN_END:
		break;
	}
}

int NPC_NewNpcManDelPet(int meindex,int talker, int petsel)
{
	int defpet;
	int fd = getfdFromCharaIndex( talker );
	char msgbuf[64], szPet[128];
	int k=0;
	int petindex=-1;

	petindex = CHAR_getCharPet( talker, petsel);
	if( !CHAR_CHECKINDEX(petindex) )
		return -1;

	for( k=0;k<arraysizeof( Re_Pet); k++)	{
		if( CHAR_getInt( petindex, CHAR_PETID) == Re_Pet[k] )
			break;
	}
	if( k == arraysizeof( Re_Pet) )	{
		return -1;
	}else	{
		print("\n FIND [%d,%d] & DEL !!", k, Re_Pet[k]);
	}

	if( CHAR_getWorkInt( CONNECT_getCharaindex( fd), CHAR_WORKBATTLEMODE) != BATTLE_CHARMODE_NONE )
		return -1;
	defpet = CHAR_getInt( talker, CHAR_DEFAULTPET);
	if(defpet == petsel)	{
		CHAR_setInt( talker, CHAR_DEFAULTPET, -1);
		lssproto_KS_send( fd, -1, TRUE);
	}
	snprintf( msgbuf,sizeof( msgbuf), "交出%s。", CHAR_getChar( petindex, CHAR_NAME));
	
	// shan
	if(CHAR_getChar( petindex, CHAR_UNIQUECODE)!=NULL){
		strncpy(uStr, CHAR_getChar( petindex, CHAR_UNIQUECODE), sizeof( STRING32));
	}else{
		strncpy(uStr, "Pet_Unicode_null", sizeof( STRING32));
	}
	
	CHAR_talkToCli( talker, -1, msgbuf,  CHAR_COLORWHITE);
				LogPet(
					CHAR_getChar( talker, CHAR_NAME ),
					CHAR_getChar( talker, CHAR_CDKEY ),
					CHAR_getChar( petindex, CHAR_NAME),
					CHAR_getInt( petindex, CHAR_LV),
					"TenseiDel",
					CHAR_getInt( talker,CHAR_FLOOR),
					CHAR_getInt( talker,CHAR_X ),
					CHAR_getInt( talker,CHAR_Y ),
					CHAR_getChar( petindex, CHAR_UNIQUECODE)
				);

	CHAR_setCharPet( talker, petsel, -1);
	CHAR_endCharOneArray( petindex );
	snprintf( szPet, sizeof( szPet ), "K%d", petsel);
	CHAR_sendStatusString( talker, szPet );

	return Re_Pet[k];
}

BOOL NPC_NewNpcManAddPet(int meindex, int talker, int petid)
{
	int	ret;
	char msgbuf[64];
	int	enemynum;
	int	i,j;
	int petindex, petindex2;

	//检查宠物栏是否有空位
	for( i = 0 ;i < CHAR_MAXPETHAVE ; i++) {
		petindex = CHAR_getCharPet( talker, i);
		if( petindex == -1  )
			break;
	}

    if( i == CHAR_MAXPETHAVE )      {
		snprintf( msgbuf,sizeof( msgbuf), "宠物已满！！");
		CHAR_talkToCli( talker, -1, msgbuf,  CHAR_COLORWHITE);
		return FALSE;
	}

	enemynum = ENEMY_getEnemyNum();
	for( i = 0; i < enemynum; i ++ ) {
		//if( ENEMY_getInt( i, ENEMY_ID ) == petid )
		if( ENEMY_getInt( i, ENEMY_TEMPNO ) == petid )	{
			print("\n ADD FIND [%d,%d]", i, ENEMY_getInt( i, ENEMY_TEMPNO ));
			break;
		}
	}
	if( i == enemynum )
		return FALSE;
	ret = ENEMY_createPetFromEnemyIndex( talker, i);
	for( i = 0; i < CHAR_MAXPETHAVE; i ++ )	{
		if( CHAR_getCharPet( talker, i ) == ret )
			break;
	}

	if( i == CHAR_MAXPETHAVE )
		i = 0;

	if( CHAR_CHECKINDEX( ret ) == TRUE ){
		CHAR_setMaxExpFromLevel( ret, CHAR_getInt( ret, CHAR_LV ));
	}
	
	petindex2 = CHAR_getCharPet(talker, i);
	if( !CHAR_CHECKINDEX( petindex2) )
		return FALSE;
	snprintf( msgbuf,sizeof( msgbuf), "拿到%s。", CHAR_getChar(petindex2,CHAR_NAME));
	CHAR_talkToCli( talker, -1, msgbuf,  CHAR_COLORWHITE);
	for(j = 0; j < CHAR_MAXPETHAVE; j++){
		petindex = CHAR_getCharPet(talker, j);
		if( !CHAR_CHECKINDEX( petindex) )
			continue;
		CHAR_complianceParameter( petindex );
		snprintf( msgbuf, sizeof( msgbuf ), "K%d", j );
		CHAR_sendStatusString( talker, msgbuf );
		snprintf( msgbuf, sizeof( msgbuf ), "W%d", j );
		CHAR_sendStatusString( talker, msgbuf );
	}

	// shan
	CHAR_setChar(petindex2, CHAR_UNIQUECODE, uStr);	

	LogPet(
		CHAR_getChar( talker, CHAR_NAME ),
		CHAR_getChar( talker, CHAR_CDKEY ),
		CHAR_getChar( petindex2, CHAR_NAME),
		CHAR_getInt( petindex2, CHAR_LV),
		"TenseiGet",
		CHAR_getInt( talker,CHAR_FLOOR),
		CHAR_getInt( talker,CHAR_X ),
		CHAR_getInt( talker,CHAR_Y ),
		CHAR_getChar( petindex2, CHAR_UNIQUECODE)
	);
	return TRUE;
}

BOOL CHECK_YEARPET( int toindex)
{
			int i,k,petindex=-1;
			for( i=0;i<CHAR_MAXPETHAVE;i++)	{
				petindex = CHAR_getCharPet( toindex, i);
				if( !CHAR_CHECKINDEX(petindex) )
					continue;
				for( k=0;k<arraysizeof( Re_Pet); k++)	{
					if( CHAR_getInt( petindex, CHAR_PETID) == Re_Pet[k] )	{
						print("\n check yearpet find it");
						return TRUE;
					}
				}
			}
			return FALSE;
}

#endif
*/
#endif

