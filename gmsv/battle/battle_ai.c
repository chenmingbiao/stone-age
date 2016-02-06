#include "version.h"
#include <stdio.h>
#include "common.h"
#include "char.h"
#include "char_base.h"
#include "battle.h"
#include "npcutil.h"
#include "pet_skill.h"

struct B_AI_RESULT {
	BATTLE_COM	command;		/* ÖÏÃ«ÔÊÔÂ¾® */
	int			target;			/* ²¾±å */
};

static int BATTLE_ai_normal( int, int, BATTLE_ENTRY *,struct B_AI_RESULT *);

static int (*functbl[])( int, int, BATTLE_ENTRY *,struct B_AI_RESULT *) = {
	NULL,
	BATTLE_ai_normal,
};

int BATTLE_ai_all( int battleindex, int side, int turn)
{
	int					i;
	int					rc = FALSE;
	BATTLE_ENTRY 		*pEntry;
	struct B_AI_RESULT	result;

	if( BATTLE_CHECKINDEX( battleindex ) == FALSE )return BATTLE_ERR_BATTLEINDEX;
	if( BATTLE_CHECKSIDE( side ) == FALSE )return BATTLE_ERR_PARAM;
	if( BATTLE_CHECKSIDE( side ^1) == FALSE )return BATTLE_ERR_PARAM;
	if( BattleArray[battleindex].Side[side].type != BATTLE_S_TYPE_ENEMY ) return FALSE;
	pEntry = BattleArray[battleindex].Side[side^1].Entry;
	for( i = 0; i < BATTLE_ENTRY_MAX; i ++ ){
		int charaindex = BattleArray[battleindex].Side[side].Entry[i].charaindex;
		int mode;
		if( CHAR_CHECKINDEX( charaindex ) == FALSE )continue;

		mode = CHAR_getWorkInt( charaindex, CHAR_WORKTACTICS);
		if( mode < 0 || mode >= arraysizeof( functbl)){
			print( "BATTLE_ai_all ÖÐ,Õ½¶·Âß¼­Ä£Ê½ºÜÆæ¹Ö(%s)(%d)\n",
				CHAR_getUseName( charaindex ), mode );
			mode = 1;
		}
		if( BATTLE_IsCharge( charaindex ) == TRUE ){
			CHAR_setWorkInt( charaindex, CHAR_WORKBATTLEMODE, BATTLE_CHARMODE_C_OK );
			continue;
		}
		if( BattleArray[battleindex].Side[side].flg & BSIDE_FLG_SURPRISE) {
			CHAR_setWorkInt( charaindex, CHAR_WORKBATTLECOM1, BATTLE_COM_NONE );
			CHAR_setWorkInt( charaindex, CHAR_WORKBATTLEMODE, BATTLE_CHARMODE_C_OK );
		}else {
			if( functbl[mode] != NULL ) {
				rc = functbl[mode]( turn, charaindex, pEntry, &result);
				if( BATTLE_CanMoveCheck( charaindex ) == FALSE ){
					result.command = BATTLE_COM_NONE;
				}
			}
			if( rc ) {
#ifdef _BATTLENPC_WARP_PLAYER
				if(result.command==BATTLE_COM_WARP){
					for( i=0; i<BATTLE_ENTRY_MAX; i++){
						int tindex = (pEntry+i)->charaindex;
						if( !CHAR_CHECKINDEX( tindex ))continue;
						if( CHAR_getFlg( tindex, CHAR_ISDIE)) continue;						
						if( CHAR_getWorkInt( tindex, CHAR_WORKBATTLEMODE)
							== BATTLE_CHARMODE_RESCUE ) continue;
						if( CHAR_getInt( tindex, CHAR_WHICHTYPE)==CHAR_TYPEPLAYER){
							char sBuff[1024]="", sBuff1[1024]="";
							int  ss[3];
							int  sii;	
							int  rc = 1;
							if( NPC_Util_GetStrFromStrWithDelim( CHAR_getWorkChar( charaindex, CHAR_WORKBATTLE_ACT_CONDITION),
								"wp", sBuff, sizeof( sBuff))!=NULL){							
								for( sii=1; sii<=3; sii++){									
									if(getStringFromIndexWithDelim( sBuff, ";", sii, sBuff1, sizeof( sBuff1)))
									{ss[sii-1]=atoi(sBuff1);}
									else
									{rc=0;}
								}
								if(rc){
									CHAR_warpToSpecificPoint( tindex, ss[0], ss[1], ss[2]);
									BATTLE_WatchStop( tindex);
								}
							}
						}
					}
				}
#endif

				CHAR_setWorkInt( charaindex, CHAR_WORKBATTLECOM1, result.command );
				CHAR_setWorkInt( charaindex, CHAR_WORKBATTLECOM2,
								result.target + (side^1)*SIDE_OFFSET);
				CHAR_setWorkInt( charaindex, CHAR_WORKBATTLEMODE, BATTLE_CHARMODE_C_OK );
				//CHAR_SETWORKINT_LOW( charaindex, CHAR_WORKBATTLECOM3, array);

			}
		}
	}
	return TRUE;
}

int BATTLE_ai_one( int charaindex, int battleindex, int side, int turn)
{
	int	rc = FALSE;
	BATTLE_ENTRY *pEntry;
	struct B_AI_RESULT	result;
	int		mode;

	/* ÓÉ·Â¶ª¡õÕýÃñÄáÓÀÛÍ */
	if( BATTLE_CHECKINDEX( battleindex ) == FALSE )return BATTLE_ERR_BATTLEINDEX;
	if( BATTLE_CHECKSIDE( side ) == FALSE )return BATTLE_ERR_PARAM;
	if( BATTLE_CHECKSIDE( side ^1) == FALSE )return BATTLE_ERR_PARAM;

	/*     ¼°ÈÓÄÌÓñËåµæµ¤ */
	/* ³ÄÆ½ÅÒ·Â¶¯Â½·´  ØêÔÂ */
	if( BattleArray[battleindex].Side[side].type != BATTLE_S_TYPE_ENEMY ) return 0;

	/*   ¸²´¡ÈÓÄÌÓñ */
	pEntry = BattleArray[battleindex].Side[side^1].Entry;

	mode = CHAR_getWorkInt( charaindex, CHAR_WORKTACTICS);
	/* ÖÊ  Ã«ÈïÔ»ÛÐØêÔÂ */
	if( mode < 0 || mode >= arraysizeof( functbl)) return FALSE;
	/* ÖÊ  Ã«ÈïÔ»ÛÐØêÔÂ */
	if( functbl[mode] != NULL ) {
		rc = functbl[mode]( turn, charaindex, pEntry, &result);
	}
	if( rc ) {
		CHAR_setWorkInt( charaindex, CHAR_WORKBATTLECOM1, result.command );
		if( result.command == BATTLE_COM_ATTACK) {
			CHAR_setWorkInt( charaindex, CHAR_WORKBATTLECOM2,
							result.target + (side^1)*SIDE_OFFSET);
		}
		CHAR_setWorkInt( charaindex, CHAR_WORKBATTLEMODE, BATTLE_CHARMODE_C_OK );
	}
	return TRUE;
}

/*   É§±åÇÂÅìÃ«è£»§ÔÂ */

#define B_AI_NORMAL_ATTACKOPTION	"at"	/*   »«×óÃóÆËÒà¼þ */
#define B_AI_NORMAL_GUARDOPTION		"gu"	/*   »¿×óÃóÆËÒà¼þ */
#define B_AI_NORMAL_MAGICOPTION		"ma"	/* ÈÈÖî×óÃóÆËÒà¼þ */
#define B_AI_NORMAL_ESCAPEOPTION	"es"	/*   ÆÍÔÂ×óÃóÆËÒà¼þ */
#define B_AI_NORMAL_WAZAOPTION		"wa"	/*   ×óÃóÆËÒà¼þ */
#ifdef _ENEMY_ATTACK_AI
#define B_AI_NORMAL_RANDAOPTION		"rn"
#define B_AI_NORMAL_RANDOMOPTIONNUM         1
#endif

#define	B_AI_NORMAL_ATTACKSUBOPTIONNUM		3
#define	B_AI_NORMAL_GUARDSUBOPTIONNUM		1
#define	B_AI_NORMAL_MAGICSUBOPTIONNUM		1
#define	B_AI_NORMAL_ESCAPESUBOPTIONNUM		1
#define	B_AI_NORMAL_WAZASUBOPTIONNUM		7

/* ×óÃóÆËÒà¼þÂ¦ÐÑ¼°  Ù¯¼°ÀÃñø */
#define	B_AI_NORMAL_TARGET_ALL			1
#define	B_AI_NORMAL_TARGET_PLAYER		2
#define	B_AI_NORMAL_TARGET_PET			3
#ifdef _ENEMY_ATTACK_AI
#define	B_AI_NORMAL_TARGET_LEADER		4
#endif
#define	B_AI_NORMAL_SELECT_RANDOM		1
#define	B_AI_NORMAL_SELECT_HP_MAX		2
#define	B_AI_NORMAL_SELECT_HP_MIN		3
#ifdef _ENEMY_ATTACK_AI
#define	B_AI_NORMAL_SELECT_STR_MAX		4
#define	B_AI_NORMAL_SELECT_DEX_MAX		5
#define	B_AI_NORMAL_SELECT_DEX_MIN		6
#define	B_AI_NORMAL_SELECT_ATT_SUBDUE	7
enum{
	AI_ATT_EARTHAT = 1,
	AI_ATT_WATERAT,
	AI_ATT_FIREAT,
	AI_ATT_WINDAT
};
int GetSubdueAttribute(int index){		
	int s_a = CHAR_getWorkInt( index, CHAR_WORKFIXEARTHAT);
    int s_b = CHAR_getWorkInt( index, CHAR_WORKFIXWATERAT);
	int s_c = CHAR_getWorkInt( index, CHAR_WORKFIXFIREAT);
	int s_d = CHAR_getWorkInt( index, CHAR_WORKFIXWINDAT);
    return ((s_a>s_c)?((s_b>s_d)?((s_a>s_b)?(2):(3)):((s_a>s_d)?(2):(1))):((s_b>s_d)?((s_c>s_b)?(4):(3)):((s_c>s_d)?(4):(1))));
}
#endif

typedef enum {
	B_AI_ATTACKMODE = 1,
	B_AI_GURADMODE,
	B_AI_MAGICMODE,
	B_AI_ESCAPEMODE,
	B_AI_WAZAMODE0,
	B_AI_WAZAMODE1,
	B_AI_WAZAMODE2,
	B_AI_WAZAMODE3,
	B_AI_WAZAMODE4,
	B_AI_WAZAMODE5,
	B_AI_WAZAMODE6,
}B_AI_MODE;

static int BATTLE_ai_normal( int turn, int charaindex,
							BATTLE_ENTRY *pEntry,
							struct B_AI_RESULT *result)
{
	int		at[B_AI_NORMAL_ATTACKSUBOPTIONNUM] = { 0,0,0}; 		/*   »«×óÃóÆËÒà¼þ */
	int		gu[B_AI_NORMAL_GUARDSUBOPTIONNUM] = {0}; 		/* ×óÃóÆËÒà¼þ */
	int		ma[B_AI_NORMAL_MAGICSUBOPTIONNUM] = {0}; 		/* ×óÃóÆËÒà¼þ */
	int		es[B_AI_NORMAL_ESCAPESUBOPTIONNUM] = {0}; 			/* ×óÃóÆËÒà¼þ */
	int		wa[B_AI_NORMAL_WAZASUBOPTIONNUM] = {0,0,0,0, 0,0,0};/* ×óÃóÆËÒà¼þ */
#ifdef _ENEMY_ATTACK_AI
    int		rn[B_AI_NORMAL_RANDOMOPTIONNUM] = {1};
#endif
	int		target[BATTLE_ENTRY_MAX];
	int		cnt;
	int		i;
	int		r;
	int		rc;
	int		mode=0;
	char	buff[256];
	char	buff2[256];

	if( turn == 1 ) {
		print( "Ó¦¸ÃÃ»Õâ»ØÊÂ¡£\n" );
		return FALSE;
	}
	if( NPC_Util_GetStrFromStrWithDelim( CHAR_getWorkChar( charaindex, CHAR_WORKBATTLE_TACTICSOPTION),
									B_AI_NORMAL_ATTACKOPTION, buff, sizeof( buff)) != NULL ){
		for( i = 1; i < B_AI_NORMAL_ATTACKSUBOPTIONNUM + 1; i ++ ) {
			rc = getStringFromIndexWithDelim( buff, ";", i, buff2, sizeof( buff2));
			if( rc != TRUE ) {
				print( "battle_ai.c:Invarid Param [%s]\n",
						CHAR_getWorkChar( charaindex, CHAR_WORKBATTLE_TACTICSOPTION));
				return FALSE;
			}
			at[i-1] = atoi( buff2);
		}
	}

	if( NPC_Util_GetStrFromStrWithDelim( CHAR_getWorkChar( charaindex, CHAR_WORKBATTLE_TACTICSOPTION),
									B_AI_NORMAL_GUARDOPTION, buff, sizeof( buff)) != NULL ){
		for( i = 1; i < B_AI_NORMAL_GUARDSUBOPTIONNUM + 1; i ++ ) {
			rc = getStringFromIndexWithDelim( buff, ";", i, buff2, sizeof( buff2));
			if( rc != TRUE ) {
				print( "battle_ai.c:Invarid Param [%s]\n",
						CHAR_getWorkChar( charaindex, CHAR_WORKBATTLE_TACTICSOPTION));
				return FALSE;
			}
			gu[i-1] = atoi( buff2);
		}
	}
	if( NPC_Util_GetStrFromStrWithDelim( CHAR_getWorkChar( charaindex, CHAR_WORKBATTLE_TACTICSOPTION),
									B_AI_NORMAL_MAGICOPTION,
									buff, sizeof( buff)) != NULL ){
		for( i = 1; i < B_AI_NORMAL_MAGICSUBOPTIONNUM + 1; i ++ ) {
			rc = getStringFromIndexWithDelim( buff, ";", i, buff2, sizeof( buff2));
			if( rc != TRUE ) {
				print( "battle_ai.c:Invarid Param [%s]\n",
						CHAR_getWorkChar( charaindex, CHAR_WORKBATTLE_TACTICSOPTION));
				return FALSE;
			}
			ma[i-1] = atoi( buff2);

		}
	}
	if( NPC_Util_GetStrFromStrWithDelim( CHAR_getWorkChar( charaindex, CHAR_WORKBATTLE_TACTICSOPTION),
									B_AI_NORMAL_ESCAPEOPTION,
									buff, sizeof( buff)) != NULL ){
		for( i = 1; i < B_AI_NORMAL_ESCAPESUBOPTIONNUM + 1; i ++ ) {
			rc = getStringFromIndexWithDelim( buff, ";", i, buff2, sizeof( buff2));

			if( rc != TRUE ) {
				print( "battle_ai.c:Invarid Param [%s]\n",
						CHAR_getWorkChar( charaindex, CHAR_WORKBATTLE_TACTICSOPTION));
				return FALSE;
			}
			es[i-1] = atoi( buff2);
		}
	}
	if( NPC_Util_GetStrFromStrWithDelim( CHAR_getWorkChar( charaindex, CHAR_WORKBATTLE_TACTICSOPTION),
									B_AI_NORMAL_WAZAOPTION,
									buff, sizeof( buff)) != NULL ){
		for( i = 1; i < B_AI_NORMAL_WAZASUBOPTIONNUM + 1; i ++ ) {
			rc = getStringFromIndexWithDelim( buff, ";", i, buff2, sizeof( buff2));
			if( rc != TRUE ) {
			}else{
				wa[i-1] = atoi( buff2);
			}
		}
	}
#ifdef _ENEMY_ATTACK_AI
	if( NPC_Util_GetStrFromStrWithDelim( CHAR_getWorkChar( charaindex, CHAR_WORKBATTLE_TACTICSOPTION),
	  								B_AI_NORMAL_RANDAOPTION,
									buff, sizeof( buff))!=NULL){
		for( i=1; i<B_AI_NORMAL_RANDOMOPTIONNUM+1; i++){
			rc = getStringFromIndexWithDelim( buff, ";", i, buff2, sizeof( buff2));			
			if(rc!=TRUE){
				print( "battle_ai.c:Invarid Param [%s]\n",
						CHAR_getWorkChar( charaindex, CHAR_WORKBATTLE_TACTICSOPTION));
				return FALSE;
			}
			rn[i-1] = atoi( buff2);
		}
	}
#endif
#ifdef _BATTLENPC_WARP_PLAYER
	if( NPC_Util_GetStrFromStrWithDelim( CHAR_getWorkChar( charaindex, CHAR_WORKBATTLE_ACT_CONDITION),
		"tn", buff, sizeof( buff))!=NULL){
		int sTurn = atoi( buff);
		if(CHAR_getWorkInt( charaindex, CHAR_WORKTURN)==0)
			CHAR_setWorkInt( charaindex, CHAR_WORKTURN, 1);
		else
			CHAR_setWorkInt( charaindex, CHAR_WORKTURN, CHAR_getWorkInt( charaindex, CHAR_WORKTURN)+1);
		if(sTurn==CHAR_getWorkInt( charaindex, CHAR_WORKTURN)){
			result->command = BATTLE_COM_WARP;
			return TRUE;	
		}
	}	
#endif
	if( at[0] == 0 && gu[0] == 0 && ma[0] == 0 && es[0] == 0 ){
		for( i = 0; i < B_AI_NORMAL_WAZASUBOPTIONNUM; i ++ ){
			if( wa[i] != 0 )break;
		}
		if( i >= B_AI_NORMAL_WAZASUBOPTIONNUM ){
			print( "ÎÞÖ¸¶¨ÈÎºÎµÄ¹¥»÷·½Ê½¡£\n" );
			return FALSE;
		}
	}
	while( !mode ) {
		int work = 0;
		work = at[0]+gu[0]+ma[0]+es[0];
		for( i = 0; i < B_AI_NORMAL_WAZASUBOPTIONNUM; i ++ ){
			work += wa[i];
		}
		r = RAND( 0, work - 1 );
		if( at[0] != 0 && r < at[0] ) 						mode = B_AI_ATTACKMODE;
		else if( gu[0] != 0 && r < at[0] + gu[0]) 			mode = B_AI_GURADMODE;
		else if( ma[0] != 0 && r < at[0] + gu[0] + ma[0]) 	mode = B_AI_MAGICMODE;
		else if( es[0] != 0 && r < at[0] + gu[0] + ma[0] + es[0] ){
			mode = B_AI_ESCAPEMODE;
		}
		if( mode ) break;
		work = at[0] + gu[0] + ma[0] + es[0];
		for( i = 0; i < B_AI_NORMAL_WAZASUBOPTIONNUM; i ++ ){
			work += wa[i];
			if( wa[i] != 0 && r < work ){
				mode = B_AI_WAZAMODE0+i;
				break;
			}
		}
	}
	if( mode == B_AI_ATTACKMODE	|| ( B_AI_WAZAMODE0 <= mode && mode <= B_AI_WAZAMODE6 )){
		while( 1 ) {
			for( i = 0 ; i < BATTLE_ENTRY_MAX; i ++ ) {
				target[i] = -1;
			}
			cnt = 0;
			for( i = 0; i < BATTLE_ENTRY_MAX ; i ++ ) {
				int tindex = (pEntry+i)->charaindex;
				if( !CHAR_CHECKINDEX( tindex ))continue;
				if( CHAR_getFlg( tindex, CHAR_ISDIE)) continue;
				if( CHAR_getWorkInt( tindex, CHAR_WORKBATTLEMODE) == BATTLE_CHARMODE_RESCUE ) continue;
				switch( at[1]) {
				  case B_AI_NORMAL_TARGET_PLAYER:
				  	if( CHAR_getInt( tindex, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER ){
				  		target[cnt++] = i;
				  	}
				  	break;
				 case B_AI_NORMAL_TARGET_PET:
				  	if( CHAR_getInt( tindex, CHAR_WHICHTYPE) == CHAR_TYPEPET){
				  		target[cnt++] = i;
				  	}
				  	break;
#ifdef _ENEMY_ATTACK_AI
                 case B_AI_NORMAL_TARGET_LEADER:					 
					 if( CHAR_getWorkInt( tindex, CHAR_WORKPARTYMODE ) == CHAR_PARTY_LEADER) {
						 target[cnt++] = i;
					 }else if(!RAND( 0, 2)) {
						 target[cnt++] = i;
					 }
					 break;
#endif
				  default:
					target[cnt++] = i;
					break;
				}
			}
			if( cnt == 0 && at[1] == B_AI_NORMAL_TARGET_ALL ) return FALSE;
			if( cnt > 0 ) break;
			at[1] = B_AI_NORMAL_TARGET_ALL;
		}
		result->command = BATTLE_COM_NONE;
		if( at[2] == B_AI_NORMAL_SELECT_RANDOM ) {
			r = RAND( 0, cnt-1);
			result->command = BATTLE_COM_ATTACK;
			result->target = target[r];
		}else if( at[2] == B_AI_NORMAL_SELECT_HP_MAX ||	at[2] == B_AI_NORMAL_SELECT_HP_MIN ){
			int top = 0;
			for( i = 0; i < cnt; i ++ ) {
				if( i ==0 ) top = target[i];
				else {
					int		tophp  = CHAR_getInt((pEntry+top)->charaindex, CHAR_HP);
					int 	comphp = CHAR_getInt((pEntry+target[i])->charaindex, CHAR_HP);
					if( at[2] == B_AI_NORMAL_SELECT_HP_MAX ) {
						if(  comphp > tophp ) top = target[i];
					}
					else {
						if( comphp < tophp ) top = target[i];
					}
				}
			}
#ifdef _ENEMY_ATTACK_AI			
			if(!RAND( 0, rn[0]))			
				result->target = target[RAND( 0, cnt-1)];
			else
				result->target  = top;            
			result->command = BATTLE_COM_ATTACK;
#else
			result->command = BATTLE_COM_ATTACK;
			result->target = top;
#endif
		}
#ifdef _ENEMY_ATTACK_AI
		else if(at[2] == B_AI_NORMAL_SELECT_STR_MAX) {
			int top = 0;
			for( i=0; i<cnt; i++) {
				if(i==0) top = target[i];
				else {
					int	topstr  = CHAR_getInt( (pEntry+top)->charaindex, CHAR_STR);
					int	compstr = CHAR_getInt( (pEntry+target[i])->charaindex, CHAR_STR);					
					if(compstr>topstr) top = target[i];					
				}
			}
			if(!RAND( 0, rn[0]))				
				result->target = target[RAND( 0, cnt-1)];
			else
				result->target  = top;            
			result->command = BATTLE_COM_ATTACK;
		}
		else if(at[2] == B_AI_NORMAL_SELECT_DEX_MAX ||
			    at[2] == B_AI_NORMAL_SELECT_DEX_MIN) {  
			int top = 0;
			for( i=0; i<cnt; i++) {
				if(i==0) top = target[i];
				else {
					int	topdex  = CHAR_getInt( (pEntry+top)->charaindex, CHAR_DEX);
					int	compdex = CHAR_getInt( (pEntry+target[i])->charaindex, CHAR_DEX);
					if(at[2]==B_AI_NORMAL_SELECT_DEX_MAX) {
						if(compdex>topdex) top = target[i];
					}else {
						if(compdex<topdex) top = target[i];
					}
				}
			}
			if(!RAND( 0, rn[0]))				
				result->target = target[RAND( 0, cnt-1)];
			else
				result->target  = top;            
			result->command = BATTLE_COM_ATTACK;
		}
		else if(at[2] == B_AI_NORMAL_SELECT_ATT_SUBDUE) {
			int top = 0;
			for( i=0; i<cnt; i++) {
				if(i==0) top = target[i];
				else {					
					switch(GetSubdueAttribute(charaindex))
					{
					case AI_ATT_EARTHAT:
						{
							int	topatt  = CHAR_getWorkInt( (pEntry+top)->charaindex, CHAR_WORKFIXEARTHAT);
					        int	compatt = CHAR_getWorkInt( (pEntry+target[i])->charaindex, CHAR_WORKFIXEARTHAT);
					        if(compatt>topatt) top = target[i];
						}
						break;
					case AI_ATT_WATERAT:
						{
							int	topatt  = CHAR_getWorkInt( (pEntry+top)->charaindex, CHAR_WORKFIXWATERAT);
					        int	compatt = CHAR_getWorkInt( (pEntry+target[i])->charaindex, CHAR_WORKFIXWATERAT);
					        if(compatt>topatt) top = target[i];
						}
						break;
					case AI_ATT_FIREAT:
						{
							int	topatt  = CHAR_getWorkInt( (pEntry+top)->charaindex, CHAR_WORKFIXFIREAT);
					        int	compatt = CHAR_getWorkInt( (pEntry+target[i])->charaindex, CHAR_WORKFIXFIREAT);
					        if(compatt>topatt) top = target[i];
						}
						break;
					case AI_ATT_WINDAT:
						{
							int	topatt  = CHAR_getWorkInt( (pEntry+top)->charaindex, CHAR_WORKFIXWINDAT);
					        int	compatt = CHAR_getWorkInt( (pEntry+target[i])->charaindex, CHAR_WORKFIXWINDAT);
					        if(compatt>topatt) top = target[i];
						}
						break;
					default:
						break;
					}
				}
			}
			if(!RAND( 0, rn[0]))				
				result->target = target[RAND( 0, cnt-1)];
			else
				result->target  = top;            
			result->command = BATTLE_COM_ATTACK;
		}
#endif
		else{
			return FALSE;
		}
		if( B_AI_WAZAMODE0 <= mode && mode <= B_AI_WAZAMODE6 ){
			if( PETSKILL_Use( charaindex, mode - B_AI_WAZAMODE0, result->target,NULL ) == TRUE ){
				result->command = CHAR_getWorkInt( charaindex, CHAR_WORKBATTLECOM1 );
				return TRUE;
			}else{
//				print( "´ËÏî¼¼ÄÜÉÐÎ´Éè¶¨(%s):(%d)\n",
//					CHAR_getUseName( charaindex), mode - B_AI_WAZAMODE0 );
				return FALSE;
			}
		}else
		if( mode == B_AI_ATTACKMODE ){
			result->command = BATTLE_COM_ATTACK;
			return TRUE;
		}
	}else if( mode == B_AI_GURADMODE ) {
		result->command = BATTLE_COM_GUARD;
		return TRUE;
	}else if( mode == B_AI_ESCAPEMODE ) {
		result->command = BATTLE_COM_ESCAPE;
		return TRUE;
	}
	return FALSE;
}
