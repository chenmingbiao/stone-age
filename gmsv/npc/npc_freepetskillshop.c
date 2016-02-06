#include "version.h"
#include <string.h>
#include "char.h"
#include "object.h"
#include "char_base.h"
#include "npcutil.h"
#include "lssproto_serv.h"
#include "pet_skill.h"
#include "readmap.h"
#include "battle.h"
#include "configfile.h"
#include "util.h"
#include "npc_eventaction.h"
#include "npc_freepetskillshop.h"

#ifdef _CFREE_petskill

enum {
	CHAR_WORK_SKILLSELECT	= CHAR_NPCWORKINT1,
};

enum {
	SELECTSKILL_START=10,
	SELECTSKILL_TEACH=23,
	SELECTSKILL_END=30,
};

enum {
	START_WINDOW=0,
	SKILL_WINDOW,
	MEEND_WINDOW,
};

static void NPC_FreePetSkillShop_selectWindow( int meindex, int toindex, int num,int select);
BOOL NPC_FreePetSkillMakeStr(int meindex,int toindex, int select);
//BOOL NPC_CHECKFREEPETSKILL( int toindex, int petindex, int skillID);
BOOL NPC_SkillShopItemCheck(int meindex,int talker,int itemNo, int cou);
BOOL NPC_SkillShopDelItems(int meindex,int talker, char *buf);
BOOL NPC_SkillShopPetCheck( int toindex, int petindex, int skillID );
BOOL NPC_SkillShopWarp( int meindex, int talkindex);

#define MAXNPCPOINT 10

BOOL NPC_FreePetSkillShopInit( int meindex )
{
	char npcarg[NPC_UTIL_GETARGSTR_BUFSIZE];
	char msg[256];
    CHAR_setInt( meindex , CHAR_WHICHTYPE , CHAR_FREESKILLSHOP );
	CHAR_setWorkInt( meindex, CHAR_WORK_SKILLSELECT, 0);

	if(NPC_Util_GetArgStr( meindex, npcarg, sizeof(npcarg))==NULL){
		print("GetArgStrErr NO arg !!");
		return FALSE;
	}

	if( NPC_Util_GetStrFromStrWithDelim( npcarg, "pet_skill", msg, sizeof( msg)) != NULL){
		char buf[256], filename[256];
		int k=1, skillID, skillarray;
		while( getStringFromIndexWithDelim( msg, ",", k, buf, sizeof( buf) ) ){
			k++;
			skillID=atoi(buf);
			skillarray = PETSKILL_getPetskillArray( skillID);
			if( PETSKILL_CHECKINDEX( skillarray) == FALSE ) continue;
			if( PETSKILL_getInt( skillarray, PETSKILL_ILLEGAL) == 1){
				if( NPC_Util_CheckAssignArgFile( meindex, filename) != NULL ){
						print("\n宠物技能不规范:[%d-%s] ->文件:%s\n",
						skillID,
						PETSKILL_getChar( skillarray, PETSKILL_NAME),
						filename );
				}
			}
		}
	}else{
		return FALSE;
	}

    return TRUE;
}
void NPC_FreePetSkillShopTalked( int meindex , int talkerindex , char *szMes ,int color )
{
    if( CHAR_getInt( talkerindex , CHAR_WHICHTYPE ) != CHAR_TYPEPLAYER ) {
    	return;
    }
	if(NPC_Util_isFaceToFace(talkerindex,meindex,2 )==FALSE){
		if( NPC_Util_CharDistance( talkerindex, meindex ) > 1) return;
	}
	CHAR_setWorkInt( talkerindex, CHAR_WORKSHOPRELEVANT, 1 );

	NPC_FreePetSkillShop_selectWindow( meindex, talkerindex, START_WINDOW,-1);
}

static void NPC_FreePetSkillShop_selectWindow( int meindex, int toindex, int num,int select)
{
	char npcarg[NPC_UTIL_GETARGSTR_BUFSIZE];
	char token[1024];
	int fd = getfdFromCharaIndex( toindex);
	int buttontype = 0, windowtype = 0, windowno = 0;
	char buf1[256];
	
	if(NPC_Util_GetArgStr( meindex, npcarg, sizeof(npcarg))==NULL){
		print("GetArgStrErr");
		return ;
	}

	switch(num)	{
	  case START_WINDOW:
		  {
		  BOOL Evflg = TRUE;
		if( NPC_Util_GetStrFromStrWithDelim( npcarg, "start_msg", token, sizeof( token)) == NULL)	{
			CHAR_setWorkInt( toindex, CHAR_WORKSHOPRELEVANT, 0);
			return;
		}
		if( CHAR_getWorkInt( toindex, CHAR_WORKSHOPRELEVANT) != 1 )	{
			CHAR_setWorkInt( toindex, CHAR_WORKSHOPRELEVANT, 0);
			return;
		}
		//玩家条件判断
		if( NPC_Util_GetStrFromStrWithDelim( npcarg, "FREE", buf1, sizeof( buf1)) != NULL ) {
			if( NPC_ActionPassCheck( meindex, toindex, buf1) == FALSE )	{
				 Evflg = FALSE;
			}
		}
		if( Evflg == FALSE)	{
			CHAR_talkToCli( toindex, -1, "你想让宠物学特殊技能？可你的条件还不够喔！", CHAR_COLORYELLOW);
			CHAR_setWorkInt( toindex, CHAR_WORKSHOPRELEVANT, 0);
			return;
		}else	{
			windowtype = WINDOW_MESSAGETYPE_MESSAGE;
			buttontype = WINDOW_BUTTONTYPE_NEXT;
			windowno = SELECTSKILL_START;
			CHAR_setWorkInt( toindex, CHAR_WORKSHOPRELEVANT, 2);
			lssproto_WN_send( fd, windowtype, buttontype, windowno,
					CHAR_getWorkInt( meindex, CHAR_WORKOBJINDEX), token	);
		}
		  }
		break;
	  case SKILL_WINDOW:
		  if( NPC_FreePetSkillMakeStr( meindex, toindex, select) == FALSE )	{
			print("\n npc_freepetskillshop.c 错误");
		  }
		break;
	  case MEEND_WINDOW:
		break;
	}
}

void NPC_FreePetSkillShopWindowTalked( int meindex, int talkerindex, int seqno, int select, char *data)
{
	int skill;
	int pet;
	int slot;
	int cost;
	int skillID=0;
	int petskillindex;
	char buf[64];
	char argstr[NPC_UTIL_GETARGSTR_BUFSIZE];
	char msg[512];
	int petindex;
	char msgbuf[128];
	double rate= 1.0;

	if(NPC_Util_GetArgStr( meindex, argstr, sizeof(argstr))==NULL){
		print("GetArgStrErr");
		return;
	}

    if( CHAR_getInt( talkerindex , CHAR_WHICHTYPE ) != CHAR_TYPEPLAYER ) {
    	return;
    }
	if(NPC_Util_isFaceToFace(talkerindex,meindex,2 )==FALSE){
		if( NPC_Util_CharDistance( talkerindex, meindex ) > 1) return;
	}
	if( CHAR_getWorkInt( talkerindex, CHAR_WORKBATTLEMODE) != BATTLE_CHARMODE_NONE)
		return ;


	switch( seqno)	{
	case SELECTSKILL_START:
		if( CHAR_getWorkInt( talkerindex, CHAR_WORKSHOPRELEVANT) != 2 )	{
			CHAR_setWorkInt( talkerindex, CHAR_WORKSHOPRELEVANT, 0);
			return;
		}
		NPC_FreePetSkillShop_selectWindow( meindex, talkerindex, SKILL_WINDOW, -1);
		break;
	case SELECTSKILL_TEACH:
		if( CHAR_getWorkInt( talkerindex, CHAR_WORKSHOPRELEVANT) != 3 )	{
			CHAR_setWorkInt( talkerindex, CHAR_WORKSHOPRELEVANT, 0);
			return;
		}
		makeStringFromEscaped( data);
		getStringFromIndexWithDelim(data,"|",1,buf,sizeof(buf));
		skill=atoi(buf);
		getStringFromIndexWithDelim(data,"|",2,buf,sizeof(buf));
		pet=atoi(buf);
		getStringFromIndexWithDelim(data,"|",3,buf,sizeof(buf));
		slot=atoi(buf);
		getStringFromIndexWithDelim(data,"|",4,buf,sizeof(buf));
		cost=atoi(buf);

		if( NPC_Util_GetStrFromStrWithDelim( argstr, "pet_skill", msg, sizeof( msg)) != NULL){
			getStringFromIndexWithDelim(msg,",",skill,buf,sizeof(buf));
			skillID=atoi(buf);
		}
		slot--;
		if( slot < 0 )	return;

		petindex = CHAR_getCharPet( talkerindex, pet-1);
		if( !CHAR_CHECKINDEX(petindex) ){
    		CHAR_send_P_StatusString( talkerindex, CHAR_P_STRING_GOLD);
    		return ;
		}
		if( NPC_Util_GetStrFromStrWithDelim( argstr, "skill_rate", msg, sizeof( msg)) != NULL){
			rate = atof( msg);
		}
		petskillindex =  PETSKILL_getPetskillArray( skillID );
		if( !PETSKILL_CHECKINDEX( petskillindex)){
	 		CHAR_send_P_StatusString( talkerindex, CHAR_P_STRING_GOLD);
			return ;
		}

		cost = PETSKILL_getInt( petskillindex, PETSKILL_COST );
		cost = cost * rate;

		//条件判断
		if(CHAR_getInt(talkerindex,CHAR_GOLD) < cost){
			CHAR_send_P_StatusString( talkerindex, CHAR_P_STRING_GOLD);
			return;
		}

		if( NPC_CHECKFREEPETSKILL( talkerindex, petindex, skillID ) == TRUE ){

			if( Action_RunDoEventAction( meindex, talkerindex, argstr) == FALSE ){
				CHAR_talkToCli( talkerindex, -1, "所需物品不足!!", CHAR_COLORYELLOW);
				CHAR_send_P_StatusString( talkerindex, CHAR_P_STRING_GOLD);
				CHAR_setWorkInt( talkerindex, CHAR_WORKSHOPRELEVANT, 0);
				return;
			}

			CHAR_setPetSkill( petindex, slot, skillID);
			CHAR_setInt( talkerindex, CHAR_GOLD, ( CHAR_getInt( talkerindex, CHAR_GOLD) - cost) );
			snprintf( msgbuf, sizeof( msgbuf ), "W%d",pet-1);
			CHAR_sendStatusString( talkerindex, msgbuf );
			CHAR_sendStatusString( talkerindex, "P");
			CHAR_send_P_StatusString( talkerindex, CHAR_P_STRING_GOLD);
			//WARP
			if( NPC_SkillShopWarp( meindex, talkerindex) == TRUE )
					return;
		}else	{
			CHAR_send_P_StatusString( talkerindex, CHAR_P_STRING_GOLD);
			CHAR_talkToCli( talkerindex, -1, "条件不足!!", CHAR_COLORYELLOW);
		}
		//ADD
		NPC_FreePetSkillShop_selectWindow( meindex, talkerindex, SKILL_WINDOW, -1);
		break;
	case SELECTSKILL_END:
		break;
	}
	
	return;
}

BOOL NPC_CHECKFREEPETSKILL( int toindex, int petindex, int skillID)
{
	int skillindex=-1;
	char SCode[256];
	char Free[256];
	int i, petID;
	skillindex =  PETSKILL_getPetskillArray( skillID );
	memset( Free, 0, sizeof( Free));
	if( !PETSKILL_CHECKINDEX( skillindex) )	{
		return FALSE;
	}

	petID = CHAR_getInt( petindex, CHAR_PETID);
	sprintf( SCode, "%s", PETSKILL_getChar( skillindex, PETSKILL_KINDCODE));
	sprintf( Free, "%s", PETSKILL_getChar( skillindex, PETSKILL_FREE));

	//CHECK CODE
	if( !strcmp( SCode, "\0"))	{
		return TRUE;
	}

	for( i=0; i<arraysizeof( Code_skill); i++)	{
		if( Code_skill[i].PetId == petID )	{
			break;
		}
	}

	//找不到这只宠物
	if( i == arraysizeof( Code_skill) )	{
		{
			char buff1[256];
			sprintf( buff1, "%s不能学特殊技能!!", CHAR_getChar( petindex, CHAR_NAME));
			CHAR_talkToCli( toindex, -1, buff1, CHAR_COLORYELLOW);
		}
		return FALSE;

	}

	if( strstr( Code_skill[i].Code, SCode ) != NULL )	{
		if( NPC_SkillShopPetCheck( toindex, petindex, skillID) == FALSE )	{
			char buff1[256];
			sprintf( buff1, "%s现在还不能学%s!", CHAR_getChar( petindex, CHAR_NAME),
									PETSKILL_getChar( skillindex, PETSKILL_NAME) );
			CHAR_talkToCli( toindex, -1, buff1, CHAR_COLORYELLOW);
			return  FALSE;
		}
		return TRUE;
	}else	{
			char buff1[256];
			sprintf( buff1, "%s不能学%s!!", CHAR_getChar( petindex, CHAR_NAME),
						PETSKILL_getChar( skillindex, PETSKILL_NAME) );
			CHAR_talkToCli( toindex, -1, buff1, CHAR_COLORYELLOW);
	}
	return FALSE;
}

BOOL NPC_FreePetSkillMakeStr(int meindex,int toindex,int select)
{
	char argstr[NPC_UTIL_GETARGSTR_BUFSIZE];
	char msg[256];
	double rate=1.0;
	int i=0;
	char token[NPC_UTIL_GETARGSTR_LINEMAX];
	int fd = getfdFromCharaIndex( toindex);

	if(select==0){
		sprintf(token,"0|0");
		lssproto_WN_send( fd, WINDOW_MESSAGETYPE_PETSKILLSHOP, 
			WINDOW_BUTTONTYPE_NONE, 
			CHAR_WINDOWTYPE_WINDOWPETSKILLSHOP,
			CHAR_getWorkInt( meindex, CHAR_WORKOBJINDEX),
			token);
	}

	if(NPC_Util_GetArgStr( meindex, argstr, sizeof(argstr))==NULL){
		print("GetArgStrErr");
		return FALSE;
	}
	if( NPC_Util_GetStrFromStrWithDelim( argstr, "main_msg", msg, sizeof( msg)) == NULL){
		print("\n CAN'T FIND main_msg !!");
		return FALSE;
	}

	sprintf(token,"1|%s|%s", CHAR_getChar(meindex,CHAR_NAME), msg);
	if( NPC_Util_GetStrFromStrWithDelim( argstr, "skill_rate", msg, sizeof( msg)) != NULL){
		rate=atof(msg);
	}
	if( NPC_Util_GetStrFromStrWithDelim( argstr, "pet_skill", msg, sizeof( msg)) != NULL)	{
		int skillarray;
		int skillID;
		int cost;
		char token2[265];
		char buf[64];
		i=1;
	    while( getStringFromIndexWithDelim(msg,",",i,buf,sizeof(buf)) !=FALSE )	{
			i++;
			skillID=atoi(buf);
			skillarray = PETSKILL_getPetskillArray( skillID);

			if( PETSKILL_CHECKINDEX( skillarray) == FALSE )
				continue;
			if( PETSKILL_getInt( skillarray, PETSKILL_ILLEGAL) == 1) continue;
			cost = PETSKILL_getInt(skillarray,PETSKILL_COST);
			cost = (int)cost*rate;

			sprintf(token2,"|%s|%d|%s|0",	PETSKILL_getChar(skillarray, PETSKILL_NAME), 
															cost, PETSKILL_getChar(skillarray, PETSKILL_COMMENT));

			strcat(token,token2);
			}
		}
		CHAR_setWorkInt( toindex, CHAR_WORKSHOPRELEVANT, 3);
		lssproto_WN_send( fd, WINDOW_MESSAGETYPE_PETSKILLSHOP, 
				WINDOW_BUTTONTYPE_NONE, 
				SELECTSKILL_TEACH,
				CHAR_getWorkInt( meindex, CHAR_WORKOBJINDEX),
				token);

	return TRUE;
}

BOOL NPC_SkillShopItemCheck(int meindex,int talker,int itemNo, int cou)
{
	int i;
	int itemindex=-1;
	int num=0;
	for( i=CHAR_STARTITEMARRAY;i<CHAR_MAXITEMHAVE;i++ ){
		itemindex = CHAR_getItemIndex( talker , i );
		if( ITEM_CHECKINDEX( itemindex) )	{
			if( itemNo == ITEM_getInt(itemindex ,ITEM_ID ) )	{
				num++;
			}
		}
		if( num >= cou )
			return TRUE;
	}
	return FALSE;

}

BOOL NPC_SkillShopDelItems(int meindex,int talker, char *buf)
{
	char buf1[256];
	char item[256], cout[256];
	int i=1;
	BOOL Evflg=TRUE;
	while( getStringFromIndexWithDelim( buf, ",", i,buf1, sizeof( buf1)) != FALSE )	{
		i++;
		if( strstr( buf1, "*") != NULL )	{
			getStringFromIndexWithDelim( buf1, "*", 1, item, sizeof( item));
			getStringFromIndexWithDelim( buf1, "*", 2, cout, sizeof( cout));
		}else	{
			strcpy( item, buf1);
			strcpy( cout, "1");
		}
		if( NPC_SkillShopItemCheck( meindex, talker, atoi( item), atoi( cout)) == FALSE )	{
			Evflg=FALSE;
			break;
		}
		if( Evflg == FALSE )
			break;
	}
	if( Evflg == FALSE )
		return FALSE;
	if( NPC_ActionDelItem( talker, buf) == TRUE )	{
		return TRUE;
	}
	return FALSE;
}

BOOL NPC_SkillShopPetCheck( int toindex, int petindex, int skillID )
{
	char Free[256];
	int i;
	char data[256], msg[256];
	int skillindex =  PETSKILL_getPetskillArray( skillID );

	memset( Free, 0, sizeof( Free));
	sprintf( Free, "%s", PETSKILL_getChar( skillindex, PETSKILL_FREE));

	if( NPC_Util_GetStrFromStrWithDelim( Free, "FREE", data, sizeof( data)) != NULL ) {
		BOOL EvFlg = TRUE;
		i=1;
		while( getStringFromIndexWithDelim( data, "|", i, msg, sizeof( msg)) != FALSE )	{
			i++;
			if( strstr( msg, "LV") != NULL )	{
				char LvStr[256];
				int LV=0;
				if( strstr( msg, ">" ) != NULL )	{
					if( getStringFromIndexWithDelim( msg, ">", 2, LvStr, sizeof( LvStr)) != FALSE )	{
						LV = atoi( LvStr);
						if( CHAR_getInt( petindex, CHAR_LV) <= LV )
							EvFlg = FALSE;
					}
				}else if( strstr( msg, "<")) {
					if( getStringFromIndexWithDelim( msg, "<", 2, LvStr, sizeof( LvStr)) != FALSE )	{
						LV = atoi( LvStr);
						if( CHAR_getInt( petindex, CHAR_LV) >= LV )
							EvFlg = FALSE;
					}
				}
			}else if( strstr( msg, "SK") != NULL )	{
				int j=0, PskId=-1, ID;
				char strSK[256];
				int petskillindex;

				getStringFromIndexWithDelim( msg, "=", 2, strSK, sizeof( strSK) );
				ID = atoi( strSK);
				for( j=0; j < CHAR_MAXPETSKILLHAVE; j++)	{
					PskId = CHAR_getPetSkill( petindex, j);
					petskillindex =  PETSKILL_getPetskillArray( PskId );
					if( !PETSKILL_CHECKINDEX( petskillindex))
						continue;
					if( ID == PskId )	{
						if( strstr( msg, "!=") !=NULL )
							EvFlg = FALSE;
						break;
					}
				}
				if( j == CHAR_MAXPETSKILLHAVE )	{
					EvFlg = FALSE;
					break;
				}
			}
			if( EvFlg == FALSE )
				break;
		}
		if( EvFlg == FALSE )	{
			return FALSE;
		}else	{
			return TRUE;
		}
	}
	return TRUE;
}

BOOL NPC_SkillShopWarp( int meindex, int talkindex)
{
	char npcarg[NPC_UTIL_GETARGSTR_BUFSIZE];
	char data[1024], buf1[256], buf2[256];
	int i=1, j=1;
	BOOL EvFlg=FALSE;
	struct	{
		int FLOOR;
		int X;
		int Y;
	}Points[MAXNPCPOINT];

	if(NPC_Util_GetArgStr( meindex, npcarg, sizeof(npcarg))==NULL){
		print("GetArgStrErr");
		return FALSE;
	}
	
	for( i=0;i<MAXNPCPOINT;i++)	{
		Points[i].FLOOR = -1;
		Points[i].X = -1;
		Points[i].Y = -1;
	}
	//WARP
	//NPC_POINT
	if( NPC_Util_GetStrFromStrWithDelim( npcarg, "NPC_POINT", data, sizeof( data)) != NULL ) {
		int where=0;
		i=1;
		j=0;
		while( getStringFromIndexWithDelim( data, ";", i, buf1, sizeof( buf1)) != FALSE )	{
			i++;
			getStringFromIndexWithDelim( buf1, ",", 1, buf2, sizeof( buf2));
			Points[j].FLOOR = atoi( buf2);
			getStringFromIndexWithDelim( buf1, ",", 2, buf2, sizeof( buf2));
			Points[j].X = atoi( buf2);
			getStringFromIndexWithDelim( buf1, ",", 3, buf2, sizeof( buf2));
			Points[j].Y = atoi( buf2);
			j++;
			if( j >= MAXNPCPOINT )
				break;
		}
		where = RAND( 0, i-1);
		CHAR_warpToSpecificPoint( meindex, Points[where].FLOOR,	Points[where].X, Points[where].Y);
		EvFlg = TRUE;
	}

	if( NPC_Util_GetStrFromStrWithDelim( npcarg, "WARP", data, sizeof( data)) != NULL ) {
		int P_Floor,P_X,P_Y;
		getStringFromIndexWithDelim( data, ",", 1, buf2, sizeof( buf2));
		P_Floor = atoi( buf2);
		getStringFromIndexWithDelim( data, ",", 2, buf2, sizeof( buf2));
		P_X = atoi( buf2);
		getStringFromIndexWithDelim( data, ",", 3, buf2, sizeof( buf2));
		P_Y = atoi( buf2);
		CHAR_warpToSpecificPoint( talkindex, P_Floor, P_X, P_Y);
		EvFlg = TRUE;
	}

	return EvFlg;
}

#endif





