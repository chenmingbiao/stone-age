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

enum {
	CHAR_WORK_STONESELECT	= CHAR_NPCWORKINT1,
};

static void NPC_PetSkillShop_selectWindow( int meindex, int toindex, int num,int select);
BOOL NPC_PetSkillMakeStr(int meindex,int toindex,int select);

BOOL NPC_PetSkillShopInit( int meindex )
{
	char argstr[NPC_UTIL_GETARGSTR_BUFSIZE];
	char msg[256];

    CHAR_setInt( meindex , CHAR_WHICHTYPE , CHAR_TYPESTONESHOP );
	if(NPC_Util_GetArgStr( meindex, argstr, sizeof(argstr))==NULL){
		print("GetArgStrErr");
		return FALSE;
	}
	if( NPC_Util_GetStrFromStrWithDelim( argstr, "pet_skill", msg, sizeof( msg)) != NULL){
		char buf[256], filename[256];
		int k=1, skillID, skillarray;
		while( getStringFromIndexWithDelim( msg, ",", k, buf, sizeof( buf) ) ){
			k++;
			skillID=atoi(buf);
			skillarray = PETSKILL_getPetskillArray( skillID);
			if( PETSKILL_CHECKINDEX( skillarray) == FALSE ) continue;
			if( PETSKILL_getInt( skillarray, PETSKILL_ILLEGAL) == 1){
				if( NPC_Util_CheckAssignArgFile( meindex, filename) != NULL ){
					print("PetSkillShop illegal skill:[%d-%s] ->file:%s",
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

void NPC_PetSkillShopTalked( int meindex , int talkerindex , char *szMes ,int color )
{
    if( CHAR_getInt( talkerindex , CHAR_WHICHTYPE ) != CHAR_TYPEPLAYER ) {
    	return;
    }
	if(NPC_Util_isFaceToFace(talkerindex,meindex,2 )==FALSE){
		if( NPC_Util_CharDistance( talkerindex, meindex ) > 1) return;
	}
	NPC_PetSkillShop_selectWindow( meindex, talkerindex,0,-1);
}

void NPC_PetSkillShopLooked( int meindex , int lookedindex)
{
    if( CHAR_getInt( lookedindex , CHAR_WHICHTYPE ) != CHAR_TYPEPLAYER ) {
    	return;
    }
	if( NPC_Util_CharDistance( lookedindex, meindex ) > 2) return;
}

static void NPC_PetSkillShop_selectWindow( int meindex, int toindex, int num,int select)
{
	switch(num){
	  case 0:
		NPC_PetSkillMakeStr(meindex,toindex,select);
		break;
	  case 1:
		break;
	}
}

void NPC_PetSkillShopWindowTalked( int meindex, int talkerindex, 
								int seqno, int select, char *data)
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
	int fd = getfdFromCharaIndex( talkerindex );
	double rate= 1.0;

	if( NPC_Util_CharDistance( talkerindex, meindex ) > 2) {
		return;
	}

	if(NPC_Util_GetArgStr( meindex, argstr, sizeof(argstr))==NULL){
		print("GetArgStrErr");
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
	if( !CHAR_CHECKPETSKILLINDEX( slot)){
	 	CHAR_send_P_StatusString( talkerindex, CHAR_P_STRING_GOLD);
		return ; 
	}
	petindex = CHAR_getCharPet( talkerindex, pet-1);
    if( !CHAR_CHECKINDEX(petindex) ){
    	CHAR_send_P_StatusString( talkerindex, CHAR_P_STRING_GOLD);
    	return ;
	}
	if(skillID < 0) return ;

	if(petindex == -1){
		print("petskillshop.c:170 PetindexError");
		return;
	}

	if( CHAR_getWorkInt( CONNECT_getCharaindex(fd),
                         CHAR_WORKBATTLEMODE) != BATTLE_CHARMODE_NONE) return ;
	if( NPC_Util_GetStrFromStrWithDelim( argstr, "skill_rate", msg, sizeof( msg)) != NULL){
		rate = atof( msg);
	}

	petskillindex =  PETSKILL_getPetskillArray( skillID );
	if(petskillindex == -1){
		CHAR_send_P_StatusString( talkerindex, CHAR_P_STRING_GOLD);
		return ;
	}

	cost = PETSKILL_getInt( petskillindex, PETSKILL_COST );
	cost = cost * rate;

	if(CHAR_getInt(talkerindex,CHAR_GOLD) < cost){
		CHAR_send_P_StatusString( talkerindex, CHAR_P_STRING_GOLD);
		return;
	}
	CHAR_setPetSkill( petindex, slot, skillID);

	CHAR_DelGold( talkerindex, cost );

	snprintf( msgbuf, sizeof( msgbuf ), "W%d",pet-1);
	CHAR_sendStatusString( talkerindex, msgbuf );
	CHAR_sendStatusString( talkerindex, "P");
	NPC_PetSkillShop_selectWindow( meindex, talkerindex,0,-1);
}

BOOL NPC_PetSkillMakeStr(int meindex,int toindex,int select)
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
		print("mainERR");
		return FALSE;
	}

	sprintf(token,"1|%s|%s",
	CHAR_getChar(meindex,CHAR_NAME),msg);
		
	if( NPC_Util_GetStrFromStrWithDelim( argstr, "skill_rate", msg, sizeof( msg)) != NULL){
		rate=atof(msg);
	}

		
	if( NPC_Util_GetStrFromStrWithDelim( argstr, "pet_skill", msg, sizeof( msg) ) != NULL){
		int skillarray;
		int skillID;
		int cost;
		char token2[265];
		char buf[64];

		i=1;
	    while( getStringFromIndexWithDelim(msg,",",i,buf,sizeof(buf)) !=FALSE ){
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
		lssproto_WN_send( fd, WINDOW_MESSAGETYPE_PETSKILLSHOP, 
				WINDOW_BUTTONTYPE_NONE, 
				CHAR_WINDOWTYPE_WINDOWPETSKILLSHOP,
				CHAR_getWorkInt( meindex, CHAR_WORKOBJINDEX),
				token);
	return TRUE;
}

