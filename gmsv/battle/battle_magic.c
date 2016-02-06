#include "version.h"
#include<string.h>
#include"char.h"
#include"char_base.h"
#include"battle.h"
#include"battle_event.h"
#include"magic_base.h"
#include"battle_magic.h"
#include"item_event.h"
#include"anim_tbl.h"
#include"common.h"
#include"lssproto_serv.h"

#ifdef _ATTACK_MAGIC

extern AttMagic *ATTMAGIC_magic;
extern int       ATTMAGIC_magicnum;

#define AJ_SAME	(1.0)
#define AJ_UP	(1.5)
#define AJ_DOWN	(0.6)

#define ATTR_MAX 100
#define D_ATTR (1.0/(ATTR_MAX*ATTR_MAX))
#ifdef _FIX_MAGICDAMAGE
static int BATTLE_getMagicAdjustInt( int attackindex, int defindex, int MagicLv, int flg,int damage );
void Magic_ComputeAttExp( int charindex, int Mnum, int MagicLv, int Expoint);
void Magic_ComputeDefExp( int charindex, int Mnum, int MagicLv, int Damage);
#endif

#ifdef _MAGIC_TOCALL
/*
typedef struct tagToCallMagic
{
  unsigned int  uiSpriteNum;			// 此咒术在Spr_x.bin的编号
  unsigned int  uiAttackType;			// 攻击的方式：单人，整排( 轮流 ) ,  整排( 轮流 ) , 整排( 同时 ) , 全体( 轮流 ) , 全体( 同时 )
  unsigned int  uiSliceTime;			// 轮流攻击时的时间差
  unsigned int  uiShowType;             // 显示的位置方式：中央、指定
  int           siSx;                   // 显示的位置 - X轴
  int           siSy;                   // 显示的位置 - Y轴
  unsigned int  uiShowBehindChar;       // 显示在人物的前方或下方
  unsigned int  uiShakeScreen;          // 是否震动画面
  unsigned int  uiShakeFrom;            // 震动画面的起始时间( 毫秒 )
  unsigned int  uiShakeTo;              // 震动画面的结束时间( 毫秒 _
  unsigned int  uiPrevMagicNum;         // 前置咒术的索引号( 0XFFFFFFFFFF 表示无前置咒术 )
  int           siPrevMagicSx;          // 前置咒术的显示位置 - X轴
  int           siPrevMagicSy;          // 前置咒术的显示位置 - Y轴
  int           siPrevMagicOnChar;      // 前置咒术显示在人物的前方或下方
  unsigned int  uiPostMagicNum;         // 後置咒术的索引号( 0XFFFFFFFF 表示无後置咒术 )
  int           siPostMagicSx;          // 後置咒术的显示位置 - X轴
  int           siPostMagicSy;          // 後置咒术的显示位置 - Y轴
  int           siPostMagicOnChar;      // 後置咒术显示在人物的前方或下方
  int			isPostDisappear;		// 咒术一般攻击完时是否马上消失
  int			ToCallMagicNo;			// 召唤术的编号
}ToCallMagic;
*/
  
ToCallMagic TOCALL_magic[3] = { {100354,0,0,0,50,25,1,0,0,0,101120,65528,65485,0,-1,0,0,0,1,0},
								{100354,1,0,0,50,25,1,0,0,0,101120,65528,65485,0,-1,0,0,0,1,1},
								{100354,5,0,0,0,0,1,1,1000,4000,101120,65528,65485,0,-1,0,0,0,0,2},
								};

#endif

// 魔法熟练度及抗性升级所需经验值
int Magic_Level_Table[101] =
{
	   1,   3,   6,  10,  15,  21,  28,  36,  45,  55,	//lv  1~10
		66,  78,	91, 105, 120, 136, 153, 171, 190, 210,  //lv 11~20
	 231, 253, 276, 300, 325, 351, 378, 406, 435, 465,  //lv 21~30
	 496, 528, 561, 595, 630, 666, 703, 741, 780, 820,  //lv 31~40
	 861, 903, 946, 990,1035,1081,1128,1176,1225,1275,  //lv 41~50
	1326,1378,1431,1485,1540,1596,1653,1711,1770,1830,  //lv 51~60
	1891,1953,2016,2080,2145,2211,2278,2346,2415,2485,  //lv 61~70
	2556,2628,2701,2775,2850,2926,3003,3081,3160,3240,  //lv 71~80
	3321,3403,3486,3570,3655,3741,3828,3916,4005,4095,  //lv 81~90
	4186,4278,4371,4465,4560,4656,4753,4851,4950,5050,  //lv 91~100
	9999
};
#endif

int BATTLE_MagicEffect( int battleindex, int attackNo, int ToList[], int MyEffect, int ToEffect )
{
	int i;
	char szCommand[256];
	int attackindex;

	attackindex = BATTLE_No2Index( battleindex, attackNo );
	if( CHAR_CHECKINDEX( attackindex ) == FALSE )return 0;

	snprintf( szCommand, sizeof(szCommand),
		"BJ|a%X|m%X|e%X|e%X|",
		attackNo,
		CHAR_getInt( attackindex, CHAR_MP ),
		MyEffect,
		ToEffect
	);

	
	BATTLESTR_ADD( szCommand );

	for( i = 0; ToList[i] != -1; i ++ ){
		snprintf( szCommand, sizeof(szCommand), "r%X|",ToList[i]);
		BATTLESTR_ADD( szCommand );
	}
	BATTLESTR_ADD( "FF|" );

	return 0;
}

#ifdef _ATTACK_MAGIC

int BATTLE_AttMagicEffect( int battleindex , int attackNo , int ToList[] , int AttackMgcNo )
{
   int     i;
   char    szcommand[256];
   int     attackindex;

   attackindex = BATTLE_No2Index( battleindex , attackNo );
   if( FALSE == CHAR_CHECKINDEX( attackindex ) )
       return 0;

   if( attackNo >= 10 )
      i = AttackMgcNo * 2;
   else
      i = AttackMgcNo * 2 + 1;

   snprintf(
   szcommand , sizeof( szcommand ) , "BJ|a%X|i%X|m%X|%X|%X|%X|s%X|t%X|l%X|%X|%X|%X|%X|%X|%X|o%X|o%X|o%X|s%X|%X|%X|" ,
   attackNo , 12345678 , CHAR_getInt( attackindex , CHAR_MP ) ,
   ATTMAGIC_magic[i].uiPrevMagicNum ,
   ATTMAGIC_magic[i].uiSpriteNum ,
   ATTMAGIC_magic[i].uiPostMagicNum ,
   ATTMAGIC_magic[i].uiAttackType ,
   ATTMAGIC_magic[i].uiSliceTime ,
   ATTMAGIC_magic[i].uiShowType ,
   ATTMAGIC_magic[i].siSx ,
   ATTMAGIC_magic[i].siSy ,
   ATTMAGIC_magic[i].siPrevMagicSx ,
   ATTMAGIC_magic[i].siPrevMagicSy ,
   ATTMAGIC_magic[i].siPostMagicSx ,
   ATTMAGIC_magic[i].siPostMagicSy ,
   ATTMAGIC_magic[i].siPrevMagicOnChar ,
   ATTMAGIC_magic[i].uiShowBehindChar ,
   ATTMAGIC_magic[i].siPostMagicOnChar ,
   ATTMAGIC_magic[i].uiShakeScreen ,
   ATTMAGIC_magic[i].uiShakeFrom ,
   ATTMAGIC_magic[i].uiShakeTo
   );

   BATTLESTR_ADD( szcommand );

   for( i = 0 ; ToList[i] != -1 ; i++ )
   {
      snprintf( szcommand , sizeof( szcommand ) , "r%X|" , ToList[i] );
      BATTLESTR_ADD( szcommand );
   }

   BATTLESTR_ADD( "FF|" );

   return 0;
}

#endif


#ifdef _MAGIC_TOCALL

int BATTLE_ToCallDragonEffect( int battleindex , int attackNo , int ToList[] , int AttackMgcNo )
{
   int     i;
   char    szcommand[256];
   int     attackindex;

   attackindex = BATTLE_No2Index( battleindex , attackNo );
   if( FALSE == CHAR_CHECKINDEX( attackindex ) )
       return 0;
/*
   if( attackNo >= 10 )
      i = AttackMgcNo * 2;
   else
      i = AttackMgcNo * 2 + 1;
*/
   snprintf(
   szcommand , sizeof( szcommand ) , "B$|a%X|i%X|m%X|%X|%X|%X|s%X|t%X|l%X|%X|%X|%X|%X|%X|%X|o%X|o%X|o%X|s%X|%X|%X|%X|%X|" ,
   attackNo , 5711438 , CHAR_getInt( attackindex , CHAR_MP ) ,
	TOCALL_magic[AttackMgcNo].uiPrevMagicNum ,
	TOCALL_magic[AttackMgcNo].uiSpriteNum ,
	TOCALL_magic[AttackMgcNo].uiPostMagicNum ,
	TOCALL_magic[AttackMgcNo].uiAttackType ,
	TOCALL_magic[AttackMgcNo].uiSliceTime ,
	TOCALL_magic[AttackMgcNo].uiShowType ,
	TOCALL_magic[AttackMgcNo].siSx ,
	TOCALL_magic[AttackMgcNo].siSy ,
	TOCALL_magic[AttackMgcNo].siPrevMagicSx ,
	TOCALL_magic[AttackMgcNo].siPrevMagicSy ,
	TOCALL_magic[AttackMgcNo].siPostMagicSx ,
	TOCALL_magic[AttackMgcNo].siPostMagicSy ,
	TOCALL_magic[AttackMgcNo].siPrevMagicOnChar ,
	TOCALL_magic[AttackMgcNo].uiShowBehindChar ,
	TOCALL_magic[AttackMgcNo].siPostMagicOnChar ,
	TOCALL_magic[AttackMgcNo].uiShakeScreen ,
	TOCALL_magic[AttackMgcNo].uiShakeFrom ,
	TOCALL_magic[AttackMgcNo].uiShakeTo ,
	TOCALL_magic[AttackMgcNo].isPostDisappear ,
	TOCALL_magic[AttackMgcNo].ToCallMagicNo
   );

   BATTLESTR_ADD( szcommand );

   for( i = 0 ; ToList[i] != -1 ; i++ )
   {
      snprintf( szcommand , sizeof( szcommand ) , "r%X|" , ToList[i] );
      BATTLESTR_ADD( szcommand );
   }

   BATTLESTR_ADD( "FF|" );

   return 0;
}

#endif
#ifdef _IMPRECATE_ITEM
void BATTLE_ImprecateRecovery(
	int battleindex, int attackNo, int toNo, int kind, int powers,
	int rounds, int UseEffect, int RecevEffect )
{
	int i, toindex, flg;
	int ToList[SIDE_OFFSET*2+1];
	//char szBuffer[256]="";
	BATTLE_MultiList( battleindex, toNo, ToList );

	BATTLE_MagicEffect(
		battleindex, attackNo, ToList,
		UseEffect, RecevEffect );
	for( i = 0; ToList[i] != -1; i ++ ){
		toindex = BATTLE_No2Index( battleindex, ToList[i] );
		if( CHAR_CHECKINDEX( toindex ) == FALSE )continue;
		switch( kind ){
#ifdef _IMPRECATE_ITEM
		case BD_KIND_CURSE:
			if( CHAR_getWorkInt( toindex, CHAR_WORKIMPRECATENUM1 ) <= 0 && 
				CHAR_getInt( toindex, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER ) {
				CHAR_setWorkInt( toindex, CHAR_WORKHURTMP, powers);	//伤害 MP
				CHAR_setWorkInt( toindex, CHAR_WORKIMPRECATENUM1, rounds);
			}
			break;
		case BD_KIND_BESTOW:
			if( CHAR_getWorkInt( toindex, CHAR_WORKIMPRECATENUM2 ) <= 0 ){
#ifdef _TYPE_TOXICATION
				if( CHAR_CanCureFlg( toindex, "HP") == FALSE ) break;
#endif
				CHAR_setWorkInt( toindex, CHAR_WORKWISHESHP, powers);	//祝福 hp
				CHAR_setWorkInt( toindex, CHAR_WORKIMPRECATENUM2, rounds);
			}
			break;
		case BD_KIND_WISHES:
			if( CHAR_getWorkInt( toindex, CHAR_WORKIMPRECATENUM3 ) <= 0 &&
				CHAR_getInt( toindex, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER ) {
				CHAR_setWorkInt( toindex, CHAR_WORKWISHESMP, powers);	//祝福 MP
				CHAR_setWorkInt( toindex, CHAR_WORKIMPRECATENUM3, rounds);
			}
			break;
#endif
		}
		//BATTLE_BroadCast( battleindex, szBuffer,
		//	(attackNo >= 10)? CHAR_COLORGRAY : CHAR_COLORPURPLE ) ;
		if( BattleArray[battleindex].norisk == 0
		&&  CHAR_getInt( toindex, CHAR_WHICHTYPE ) == CHAR_TYPEPET){
			flg = CHAR_getWorkInt( toindex, CHAR_WORKBATTLEFLG );
			if(  flg & CHAR_BATTLEFLG_RECOVERY ){
			}else{
				CHAR_PetAddVariableAi( toindex, AI_FIX_PETRECOVERY );
				CHAR_setWorkInt( toindex, CHAR_WORKBATTLEFLG, flg | CHAR_BATTLEFLG_RECOVERY );
			}
		}
	}
	return;
}
#endif

void BATTLE_MultiRecovery(
	int battleindex,
	int attackNo,
	int toNo,
	int kind,
	int power,
	int per,
	int UseEffect,
	int RecevEffect
)
{
	int i, toindex, UpPoint = 0, workhp, oldhp, flg;
	int ToList[SIDE_OFFSET*2+1];

	//char szBuffer[256]="";
	char szCommand[256];
	
	int ridepet, petUpPoint =0;


	BATTLE_MultiList( battleindex, toNo, ToList );

	BATTLE_MagicEffect( battleindex, attackNo, ToList, UseEffect, RecevEffect );

	for( i = 0; ToList[i] != -1; i ++ ){
		toindex = BATTLE_No2Index( battleindex, ToList[i] );

		if( CHAR_CHECKINDEX( toindex ) == FALSE )	continue;

		ridepet = BATTLE_getRidePet( toindex );

		switch( kind ){
#ifdef _ITEM_UNBECOMEPIG
		case BD_KIND_UNBECOMEPIG:
            if( CHAR_getInt( toindex, CHAR_BECOMEPIG ) > -1 ){
		        CHAR_setInt( toindex, CHAR_BECOMEPIG, -1 );
                CHAR_setInt( toindex, CHAR_BASEIMAGENUMBER, CHAR_getInt( toindex, CHAR_BASEBASEIMAGENUMBER));
				CHAR_complianceParameter( toindex );
		        CHAR_sendCToArroundCharacter( CHAR_getWorkInt( toindex , CHAR_WORKOBJINDEX ));
		        CHAR_send_P_StatusString( toindex , CHAR_P_STRING_BASEBASEIMAGENUMBER);
		        CHAR_talkToCli( toindex,-1,"乌力化失效了。",CHAR_COLORWHITE);
			}
		break;
#endif 
#ifdef _ITEM_PROPERTY
		case BD_KIND_PROPERTY:
		{
			if (CHAR_getInt(toindex, CHAR_WHICHTYPE)!=CHAR_TYPEPLAYER)
            return;
		//print("地水火风:%d %d %d %d", CHAR_getInt( toindex, CHAR_EARTHAT ), CHAR_getInt( toindex, CHAR_WATERAT ), CHAR_getInt( toindex, CHAR_FIREAT ), CHAR_getInt( toindex, CHAR_WINDAT ) );
		if( power == 1 ){//旋转属性 正转
	        if( CHAR_getInt( toindex, CHAR_EARTHAT ) == 100 )
                CHAR_setInt( toindex, CHAR_EARTHAT, CHAR_getInt( toindex, CHAR_EARTHAT )-10 ),
			    CHAR_setInt( toindex, CHAR_WATERAT, CHAR_getInt( toindex, CHAR_WATERAT )+10 );
		    else if( CHAR_getInt( toindex, CHAR_WATERAT ) == 100 )
                CHAR_setInt( toindex, CHAR_WATERAT, CHAR_getInt( toindex, CHAR_WATERAT )-10 ),
			    CHAR_setInt( toindex, CHAR_FIREAT, CHAR_getInt( toindex, CHAR_FIREAT )+10 );
            else if( CHAR_getInt( toindex, CHAR_FIREAT ) == 100 )
                CHAR_setInt( toindex, CHAR_FIREAT, CHAR_getInt( toindex, CHAR_FIREAT )-10 ),
			    CHAR_setInt( toindex, CHAR_WINDAT, CHAR_getInt( toindex, CHAR_WINDAT )+10 );
            else if( CHAR_getInt( toindex, CHAR_WINDAT ) == 100 )
                CHAR_setInt( toindex, CHAR_WINDAT, CHAR_getInt( toindex, CHAR_WINDAT )-10 ),
			    CHAR_setInt( toindex, CHAR_EARTHAT, CHAR_getInt( toindex, CHAR_EARTHAT )+10 );
			else if( CHAR_getInt( toindex, CHAR_EARTHAT ) > 0 && CHAR_getInt( toindex, CHAR_WATERAT ) > 0 )
                CHAR_setInt( toindex, CHAR_EARTHAT, CHAR_getInt( toindex, CHAR_EARTHAT )-10 ),
			    CHAR_setInt( toindex, CHAR_WATERAT, CHAR_getInt( toindex, CHAR_WATERAT )+10 );
            else if( CHAR_getInt( toindex, CHAR_WATERAT ) > 0 && CHAR_getInt( toindex, CHAR_FIREAT ) > 0 )
                CHAR_setInt( toindex, CHAR_WATERAT, CHAR_getInt( toindex, CHAR_WATERAT )-10 ),
			    CHAR_setInt( toindex, CHAR_FIREAT, CHAR_getInt( toindex, CHAR_FIREAT )+10 );
		    else if( CHAR_getInt( toindex, CHAR_FIREAT ) > 0 && CHAR_getInt( toindex, CHAR_WINDAT ) > 0 )
                CHAR_setInt( toindex, CHAR_FIREAT, CHAR_getInt( toindex, CHAR_FIREAT )-10 ),
			    CHAR_setInt( toindex, CHAR_WINDAT, CHAR_getInt( toindex, CHAR_WINDAT )+10 );
            else if( CHAR_getInt( toindex, CHAR_WINDAT ) > 0 && CHAR_getInt( toindex, CHAR_EARTHAT ) > 0 )
                CHAR_setInt( toindex, CHAR_WINDAT, CHAR_getInt( toindex, CHAR_WINDAT )-10 ),
			    CHAR_setInt( toindex, CHAR_EARTHAT, CHAR_getInt( toindex, CHAR_EARTHAT )+10 );
		}
        if( power == 2 ){//旋转属性 反转
			if( CHAR_getInt( toindex, CHAR_EARTHAT ) == 100 )
                CHAR_setInt( toindex, CHAR_EARTHAT, CHAR_getInt( toindex, CHAR_EARTHAT )-10 ),
			    CHAR_setInt( toindex, CHAR_WINDAT, CHAR_getInt( toindex, CHAR_WINDAT )+10 );
		    else if( CHAR_getInt( toindex, CHAR_WATERAT ) == 100 )
                CHAR_setInt( toindex, CHAR_WATERAT, CHAR_getInt( toindex, CHAR_WATERAT )-10 ),
			    CHAR_setInt( toindex, CHAR_EARTHAT, CHAR_getInt( toindex, CHAR_EARTHAT )+10 );
            else if( CHAR_getInt( toindex, CHAR_FIREAT ) == 100 )
                CHAR_setInt( toindex, CHAR_FIREAT, CHAR_getInt( toindex, CHAR_FIREAT )-10 ),
			    CHAR_setInt( toindex, CHAR_WATERAT, CHAR_getInt( toindex, CHAR_WATERAT )+10 );
            else if( CHAR_getInt( toindex, CHAR_WINDAT ) == 100 )
                CHAR_setInt( toindex, CHAR_WINDAT, CHAR_getInt( toindex, CHAR_WINDAT )-10 ),
			    CHAR_setInt( toindex, CHAR_FIREAT, CHAR_getInt( toindex, CHAR_FIREAT )+10 );
		    else if( CHAR_getInt( toindex, CHAR_EARTHAT ) > 0 && CHAR_getInt( toindex, CHAR_WATERAT ) > 0 )
                CHAR_setInt( toindex, CHAR_EARTHAT, CHAR_getInt( toindex, CHAR_EARTHAT )+10 ),
			    CHAR_setInt( toindex, CHAR_WATERAT, CHAR_getInt( toindex, CHAR_WATERAT )-10 );
            else if( CHAR_getInt( toindex, CHAR_WATERAT ) > 0 && CHAR_getInt( toindex, CHAR_FIREAT ) > 0 )
                CHAR_setInt( toindex, CHAR_WATERAT, CHAR_getInt( toindex, CHAR_WATERAT )+10 ),
			    CHAR_setInt( toindex, CHAR_FIREAT, CHAR_getInt( toindex, CHAR_FIREAT )-10 );
		    else if( CHAR_getInt( toindex, CHAR_FIREAT ) > 0 && CHAR_getInt( toindex, CHAR_WINDAT ) > 0 )
                CHAR_setInt( toindex, CHAR_FIREAT, CHAR_getInt( toindex, CHAR_FIREAT )+10 ),
			    CHAR_setInt( toindex, CHAR_WINDAT, CHAR_getInt( toindex, CHAR_WINDAT )-10 );
            else if( CHAR_getInt( toindex, CHAR_WINDAT ) > 0 && CHAR_getInt( toindex, CHAR_EARTHAT ) > 0 )
                CHAR_setInt( toindex, CHAR_WINDAT, CHAR_getInt( toindex, CHAR_WINDAT )+10 ),
			    CHAR_setInt( toindex, CHAR_EARTHAT, CHAR_getInt( toindex, CHAR_EARTHAT )-10 );
		}
		}
#endif
		case BD_KIND_HP:
#ifdef _TYPE_TOXICATION
			if( CHAR_CanCureFlg( toindex, "HP") == FALSE ) continue;
#endif
		    // Robin 0728 ride Pet
		    if( ridepet == -1 ) {
				UpPoint = RAND( (power*0.9), (power*1.1) );
#ifdef _MAGIC_REHPAI
#else
				if( per ){
					UpPoint *= CHAR_getWorkInt( toindex, CHAR_WORKMAXHP ) * 0.01;
				}
				UpPoint *= GetRecoveryRate( toindex );
#endif
				workhp = CHAR_getInt( toindex, CHAR_HP );
				oldhp = workhp; 
				workhp += (int)UpPoint;
				CHAR_setInt( toindex, CHAR_HP,
				min( workhp, CHAR_getWorkInt( toindex, CHAR_WORKMAXHP ) ) );
				
				//snprintf( szBuffer, sizeof(szBuffer), "(%s)的耐久力回复(%d)", CHAR_getUseName( toindex ), UpPoint );

		    }else {
				int allUpPoint;
				UpPoint = RAND( (power*0.9), (power*1.1) );
#ifdef _MAGIC_REHPAI
				allUpPoint = UpPoint;
				UpPoint = (allUpPoint * Magic_RideGetHP( toindex, ridepet, 1 ))/100;
				petUpPoint = allUpPoint - UpPoint;
#else		
				petUpPoint = UpPoint;
				if( per ){
					UpPoint *= CHAR_getWorkInt( toindex, CHAR_WORKMAXHP ) * 0.01;
					petUpPoint *= CHAR_getWorkInt( ridepet, CHAR_WORKMAXHP ) * 0.01;
					
				}
				UpPoint *= GetRecoveryRate( toindex );
				petUpPoint *= GetRecoveryRate( ridepet );
#endif


				allUpPoint = UpPoint + petUpPoint;
#ifdef _MAGIC_REHPAI
#else
				UpPoint = (UpPoint * UpPoint) / allUpPoint;
				petUpPoint = (petUpPoint * petUpPoint) / allUpPoint;
#endif
				workhp = CHAR_getInt( toindex, CHAR_HP );
				oldhp = workhp;
				workhp += (int)UpPoint;
				CHAR_setInt( toindex, CHAR_HP,
					min( workhp, CHAR_getWorkInt( toindex, CHAR_WORKMAXHP ) ) );
				workhp = CHAR_getInt( ridepet, CHAR_HP );
				oldhp = workhp;
				workhp += (int)petUpPoint;
				CHAR_setInt( ridepet, CHAR_HP,
				min( workhp, CHAR_getWorkInt( ridepet, CHAR_WORKMAXHP ) ) );			
				//snprintf( szBuffer, sizeof(szBuffer),
				//	"(%s)的耐久力回复(%d),(%s的骑宠)的耐久力回复(%d)",
				//		CHAR_getUseName( toindex ),
				//		UpPoint,
				//		CHAR_getUseName( toindex ),
				//		petUpPoint
				//	);
		    }
		    break;
		case BD_KIND_MP:
			UpPoint = RAND( (power*0.9), (power*1.1) );
			if( per ){
				UpPoint *= CHAR_getWorkInt( toindex, CHAR_WORKMAXMP ) * 0.01;
			}
			workhp = CHAR_getInt( toindex, CHAR_MP );
			oldhp = workhp;
			workhp += (int)UpPoint;
			CHAR_setInt( toindex, CHAR_MP,
			min( workhp, CHAR_getWorkInt( toindex, CHAR_WORKMAXMP ) ) );
			//snprintf( szBuffer, sizeof(szBuffer),
			//	"(%s)的气力回复(%d)",
			//		CHAR_getUseName( toindex ),
			//		UpPoint
			//	);
			break;
		}
		//BATTLE_BroadCast( battleindex, szBuffer,
		//	(attackNo >= 10)? CHAR_COLORGRAY : CHAR_COLORPURPLE ) ;
		{
			int Sign, Damage, petDamage;
			Sign = (UpPoint >= 0)?(1):(0);
			Damage = ABS( UpPoint );
			petDamage = ABS( petUpPoint );
#ifdef _NOTRIDE_
			snprintf( szCommand, sizeof(szCommand),
				"BD|r%X|%X|%X|d%X|", ToList[i], kind, Sign, Damage );
#else
			snprintf( szCommand, sizeof(szCommand),
				"BD|r%X|%X|%X|d%X|p%X|", ToList[i], kind, Sign, Damage, petDamage );
#endif //_NOTRIDE_
			BATTLESTR_ADD( szCommand );
		}

		if( BattleArray[battleindex].norisk == 0
		&&  CHAR_getInt( toindex, CHAR_WHICHTYPE ) == CHAR_TYPEPET){
			flg = CHAR_getWorkInt( toindex, CHAR_WORKBATTLEFLG );
			if(  flg & CHAR_BATTLEFLG_RECOVERY ){
			}else{
				CHAR_PetAddVariableAi( toindex, AI_FIX_PETRECOVERY );
				CHAR_setWorkInt( toindex, CHAR_WORKBATTLEFLG, flg | CHAR_BATTLEFLG_RECOVERY );
			}
		}
	}
	return;
}

#ifdef _MAGIC_REHPAI
int Magic_RideGetHP( int toindex, int petindex, int flg)
{
	int allDHp=1;
	int petDHp=1;
	int CharDHp=1;
	int ISIZE = 100;

	CharDHp	= CHAR_getWorkInt( toindex, CHAR_WORKMAXHP ) - CHAR_getInt( toindex, CHAR_HP);
	petDHp	= CHAR_getWorkInt( petindex, CHAR_WORKMAXHP ) - CHAR_getInt( petindex, CHAR_HP);
	allDHp = (CharDHp + petDHp);

	if( allDHp <= 0 ) allDHp=1;
	if( flg == 1)
		return (int)((CharDHp*ISIZE)/allDHp);
	else
		return (int)((petDHp*ISIZE)/allDHp);
}
#endif

int	MAGIC_Recovery_Battle( int charaindex, int toNo, int marray, int mp )
{
	char *magicarg;
	float power;
	int battleindex, attackNo, 	HealedEffect = 0, per = 0;
	int range;
	if( !CHAR_CHECKINDEX( charaindex) )	return FALSE;
	battleindex = CHAR_getWorkInt( charaindex, CHAR_WORKBATTLEINDEX );
	attackNo = BATTLE_Index2No( battleindex, charaindex );

#if 1	// Robin fix 防封包改Magic范围
	range = MAGIC_getInt( marray, MAGIC_TARGET );
	//print(" MAGIC_TARGET:%d toNo:%d ", range, toNo);
	if( range == 0 && toNo != attackNo ) { // 只能对自己使用
//		print("\n 改封包!MAGIC只能对自己使用!!:%s ", CHAR_getChar( charaindex, CHAR_CDKEY) );
		return FALSE;
	}
	if( range == 1 && toNo >= 20 ) { // 只能对单体使用
//		print("\n 改封包!MAGIC只能对单体使用!!:%s ", CHAR_getChar( charaindex, CHAR_CDKEY) );
		return FALSE;
	}
#endif

	// shan(对全体使用魔法的bug)，修改者jinchao+2001/12/07
	if( BATTLE_CHECKINDEX( battleindex ) == FALSE ) return FALSE;
	magicarg = MAGIC_getChar( marray, MAGIC_OPTION );
	// shan(对全体使用魔法的bug)，修改者jinchao+2001/12/07
	if (magicarg == "\0") return FALSE;

	power =	atoi( magicarg );
	if( strstr( magicarg, "%" ) ){
		per = 1;
	}
	
	if( power <= 100 ){
		HealedEffect = SPR_heal;
	}else
	if( power <= 300 ){
		HealedEffect = SPR_heal2;
	}else{
		HealedEffect = SPR_heal3;
	}
	BATTLE_MultiRecovery( battleindex, attackNo, toNo,BD_KIND_HP, (int)power,  per, MAGIC_EFFECT_USER, HealedEffect );
	return TRUE;
}

void BATTLE_MultiResAndDef( 
	int battleindex, 	// 田玄伙奶件犯永弁旦
	int attackNo,		// 井仃月谛  寞
	int toNo,			// 井仃日木月谛  寞
	int power,			// 湘  荚汊袄
	int per,			// ⊙井＂
	int kind, 			//   芊  豢及潘  
	int count, 			// 窒荚  什井
	int UseEffect,		// 银丹谛及巨白尼弁玄
	int RecevEffect		// 井仃日木月谛及巨白尼弁玄
)
{
	int i, toindex, charaindex, UpPoint = 0, workhp;
	int ToList[SIDE_OFFSET*2+1];

	//char szBuffer[256]="";
	char szCommand[256];


	BATTLE_MultiListDead( battleindex, toNo, ToList );

	BATTLE_MagicEffect(
		battleindex, 			// 田玄伙奶件犯永弁旦
		attackNo, 				// 井仃月谛  寞(ㄟ  ㄠㄨ)
		ToList, 				// 井仃日木月谛  寞伉旦玄(ㄟ  ㄠㄨ)
		UseEffect, 				// 井仃月谛巨白尼弁玄
		RecevEffect				// 井仃日木月谛巨白尼弁玄
	);
	charaindex = BATTLE_No2Index( battleindex, attackNo );
	for( i = 0; ToList[i] != -1; i ++ ){
		toindex = BATTLE_No2Index( battleindex, ToList[i] );
		if( BattleArray[battleindex].type == BATTLE_TYPE_P_vs_P
		&& CHAR_getInt( toindex, CHAR_WHICHTYPE ) == CHAR_TYPEPLAYER){
			continue;
		}
		if( CHAR_getFlg( toindex, CHAR_ISDIE ) == FALSE ){
			continue;
		}
		UpPoint = power;
#ifdef _MAGIC_REHPAI
#else
		if( per ){
			UpPoint *= CHAR_getWorkInt( toindex, CHAR_WORKMAXHP ) * 0.01;
		}
#endif
		if( power == 0 ){
			UpPoint = CHAR_getWorkInt( toindex, CHAR_WORKMAXHP );
		}else{
			UpPoint = RAND( (power*0.9), (power*1.1) );
		}
		UpPoint = max( 1, UpPoint );
		workhp = CHAR_getInt( toindex, CHAR_HP ) + UpPoint;
		CHAR_setInt( toindex, CHAR_HP,
			min( workhp, CHAR_getWorkInt( toindex, CHAR_WORKMAXHP ) ) );
		CHAR_setFlg( toindex, CHAR_ISDIE, 0 );
		CHAR_setWorkInt( toindex, MagicDefTbl[kind], count );

		//snprintf( szBuffer, sizeof(szBuffer),
		//	"(%s)恢复意识",
		//		CHAR_getUseName( toindex ) );
		//snprintf( szBuffer, sizeof(szBuffer),
		//	"(%s)将(%s)遭受(损伤%s)",
		//	CHAR_getUseName( charaindex ),
		//	CHAR_getUseName( toindex ),
		//	aszMagicDefFull[kind]
		//);
		//BATTLE_BroadCast( battleindex, szBuffer,
		//	(attackNo >= 10)? CHAR_COLORGRAY : CHAR_COLORPURPLE ) ;
		snprintf( szCommand, sizeof(szCommand),
				"BL|r%X|h%X|", ToList[i], CHAR_getInt( toindex, CHAR_HP ) );
		BATTLESTR_ADD( szCommand );
		if( BattleArray[battleindex].norisk == 0
		 &&	BattleArray[battleindex].type == BATTLE_TYPE_P_vs_E	 ){
			if( CHAR_getInt( toindex, CHAR_WHICHTYPE ) == CHAR_TYPEPET ){
				CHAR_PetAddVariableAi( toindex, AI_FIX_PETRESSURECT );
			}
		}
	}
}

#ifdef _ATTACK_MAGIC

static int CharTable[4][5] =
{
   { 13 , 11 , 10 , 12 , 14 } ,
   { 18 , 16 , 15 , 17 , 19 } ,
   {  8 ,  6 ,  5 ,  7 ,  9 } ,
   {  3 ,  1 ,  0 ,  2 ,  4 }
};

static int CharTableIdx[20][2] =
{
   { 3 , 2 } , { 3 , 1 } , { 3 , 3 } , { 3 , 0 } , { 3 , 4 } ,
   { 2 , 2 } , { 2 , 1 } , { 2 , 3 } , { 2 , 0 } , { 2 , 4 } ,
   { 0 , 2 } , { 0 , 1 } , { 0 , 3 } , { 0 , 0 } , { 0 , 4 } ,
   { 1 , 2 } , { 1 , 1 } , { 1 , 3 } , { 1 , 0 } , { 1 , 4 }
};



typedef int ( *FUNC )( const void* , const void* );

static int SortLoc( const int *pEle1 , const int *pEle2 )
{
   int ele1basex , ele1basey;
   int ele2basex , ele2basey;

   ele1basex = CharTableIdx[*pEle1][1];
   ele1basey = CharTableIdx[*pEle1][0];
   ele2basex = CharTableIdx[*pEle2][1];
   ele2basey = CharTableIdx[*pEle2][0];

   // 左上方
   if( *pEle1 >= 10 )
   {
      if( ele1basey != ele2basey )
         return ( ele1basey - ele2basey );

      return ( ele1basex - ele2basex );
   }
   // 右下方
   else
   {
      if( ele1basey != ele2basey )
         return ( ele2basey - ele1basey );

      return ( ele2basex - ele1basey );
   }

   return 0;
}


static void BATTLE_GetAttr( int charaindex , int *pAt_Fire , int *pAt_Water , int *pAt_Earth , int *pAt_Wind , int *pAt_None )
{

   int petindex = BATTLE_getRidePet( charaindex );

   if( petindex == -1 )
   {
        *pAt_Fire  = CHAR_getWorkInt( charaindex , CHAR_WORKFIXFIREAT );
        *pAt_Water = CHAR_getWorkInt( charaindex , CHAR_WORKFIXWATERAT );
        *pAt_Earth = CHAR_getWorkInt( charaindex , CHAR_WORKFIXEARTHAT );
        *pAt_Wind  = CHAR_getWorkInt( charaindex , CHAR_WORKFIXWINDAT );
   }
   else
   {
        *pAt_Fire  = ( CHAR_getWorkInt( charaindex , CHAR_WORKFIXFIREAT )
                   + CHAR_getWorkInt( petindex , CHAR_WORKFIXFIREAT ) ) / 2;
        *pAt_Water = ( CHAR_getWorkInt( charaindex , CHAR_WORKFIXWATERAT )
                   + CHAR_getWorkInt( petindex , CHAR_WORKFIXWATERAT ) ) / 2;
        *pAt_Earth = ( CHAR_getWorkInt( charaindex , CHAR_WORKFIXEARTHAT )
                   + CHAR_getWorkInt( petindex , CHAR_WORKFIXEARTHAT ) ) / 2;
        *pAt_Wind  = ( CHAR_getWorkInt( charaindex , CHAR_WORKFIXWINDAT )
                   + CHAR_getWorkInt( petindex , CHAR_WORKFIXWINDAT ) ) / 2;
   }

   if( *pAt_Fire < 0 )  *pAt_Fire  = 0;
   if( *pAt_Water < 0 ) *pAt_Water = 0;
   if( *pAt_Earth < 0 ) *pAt_Earth = 0;
   if( *pAt_Wind < 0 )  *pAt_Wind  = 0;

   *pAt_None = 100 - *pAt_Fire - *pAt_Water - *pAt_Earth - *pAt_Wind;

   if( *pAt_None < 0 )
      *pAt_None = 0;
}

static void BATTLE_GetPureAttr( int charaindex , int *pAt_Fire , int *pAt_Water, int *pAt_Earth , int *pAt_Wind )
{
   *pAt_Fire  = CHAR_getWorkInt( charaindex , CHAR_WORKFIXFIREAT );
   *pAt_Water = CHAR_getWorkInt( charaindex , CHAR_WORKFIXWATERAT );
   *pAt_Earth = CHAR_getWorkInt( charaindex , CHAR_WORKFIXEARTHAT );
   *pAt_Wind  = CHAR_getWorkInt( charaindex , CHAR_WORKFIXWINDAT );

   if( *pAt_Fire < 0 )  *pAt_Fire  = 0;
   if( *pAt_Water < 0 ) *pAt_Water = 0;
   if( *pAt_Earth < 0 ) *pAt_Earth = 0;
   if( *pAt_Wind < 0 )  *pAt_Wind  = 0;
}



int BATTLE_CalAttMagicEffect( int AttEle , int DefPosEleValue , int DefNegEleValue )
{
    int value;

    value = ( 100 * DefPosEleValue - 100 * DefNegEleValue ) / 1000;

    if( value > 10 )
        return 10;
    else if( value < -10 )
        return -10;

    return value;
}



int BATTLE_CalcCharaRatio( int AttEle , int charaidx )
{
   int petidx , charahurt , pethurt;
   int charaattr[4] , petattr[4];

   // 没有宠物
   if( -1 == ( petidx = BATTLE_getRidePet( charaidx ) ) )
      return 10;

   // Nuke 20040330: Fix bug
   if ( -1 == AttEle )
       return 5;

   BATTLE_GetPureAttr( charaidx , &charaattr[2] , &charaattr[1] , &charaattr[0], &charaattr[3] );
   BATTLE_GetPureAttr( petidx , &petattr[2] , &petattr[1] , &petattr[0] , &petattr[3] );

   //print("\nAttEle: %d %d %d %d %d ",AttEle, charaattr[( AttEle + 1 ) % 4], charaattr[AttEle],petattr[( AttEle + 1 ) %4] , petattr[AttEle]);

   charahurt = 20 + BATTLE_CalAttMagicEffect( AttEle , charaattr[( AttEle + 1 ) % 4] , charaattr[AttEle] );
   pethurt   = 20 + BATTLE_CalAttMagicEffect( AttEle , petattr[( AttEle + 1 ) %4] , petattr[AttEle] );

   charahurt = 10 * charahurt / ( charahurt + pethurt );

   if( charahurt < 2 )
      return 2;
   else if( charahurt > 8 )
      return 8;

   return charahurt;
}

static int	BATTLE_AttrCalc(
		int My_Fire,	// 愤坌础(膜恳熬仃月幻丹)
		int My_Water,
		int My_Earth,
		int My_Wind,
		int My_None,
		int Vs_Fire,	// 锹澎础
		int Vs_Water,
		int Vs_Earth,
		int Vs_Wind,
		int Vs_None
)
{
	int iRet = 0;

	// 绍及  猾  宁煌允月［
	My_Fire = My_Fire * Vs_None * AJ_UP		// 绍     雄中
			+ My_Fire * Vs_Fire * AJ_SAME   // 绍  绍   元
			+ My_Fire * Vs_Water * AJ_DOWN  // 绍     浇中
			+ My_Fire * Vs_Earth * AJ_SAME  // 绍  萝   元
			+ My_Fire * Vs_Wind * AJ_UP;    // 绍  氘 雄中

	//   及  猾  宁煌允月［
	My_Water = My_Water * Vs_None * AJ_UP	//        雄中
			+ My_Water * Vs_Fire * AJ_UP    //     绍 雄中
			+ My_Water * Vs_Water * AJ_SAME  //          元
			+ My_Water * Vs_Earth * AJ_DOWN  //     萝 浇中
			+ My_Water * Vs_Wind * AJ_SAME;  //     氘   元

	// 萝
	My_Earth = My_Earth * Vs_None * AJ_UP	 // 萝     雄中
			+ My_Earth  * Vs_Fire * AJ_SAME  // 萝  绍   元
			+ My_Earth * Vs_Water * AJ_UP    // 萝     雄中
			+ My_Earth * Vs_Earth * AJ_SAME  // 萝  萝   元
			+ My_Earth * Vs_Wind * AJ_DOWN;  // 萝  氘 浇中

	// 氘
	My_Wind = My_Wind * Vs_None * AJ_UP		 // 氘     雄中
			+ My_Wind  * Vs_Fire * AJ_DOWN   // 氘  绍  浇中
			+ My_Wind * Vs_Water * AJ_SAME   // 氘        元
			+ My_Wind * Vs_Earth * AJ_UP     // 氘  萝  雄中
			+ My_Wind * Vs_Wind * AJ_SAME;   // 氘  氘    元

	//   箪岭
	My_None = My_None * Vs_None * AJ_SAME	//          元
			+ My_None * Vs_Fire * AJ_DOWN   //     绍  浇中
			+ My_None * Vs_Water * AJ_DOWN  //         浇中
			+ My_None * Vs_Earth * AJ_DOWN  //     萝  浇中
			+ My_None * Vs_Wind * AJ_DOWN;  //     氘  浇中

	// 蝈  宁煌允月
	iRet = (My_Fire + My_Water + My_Earth + My_Wind + My_None) ;

	// 喘仃遥壬匀井曰匹  五仁卅匀凶及匹公及坌尺日允
	return (iRet * D_ATTR);
}

extern float BATTLE_FieldAttAdjust(int battleindex,int pAt_Fire,int pAt_Water,int pAt_Earth,int pAt_Wind);
#ifndef _FIX_MAGICDAMAGE
static int BATTLE_AttrAdjust(
	int attackindex,
	int defindex,
	int damage
)
{
	int At_Fire, At_Earth, At_Water, At_Wind, At_none;
	int Df_Fire, Df_Earth, Df_Water, Df_Wind, Df_none;
	float At_FieldPow, Df_FieldPow;

	BATTLE_GetAttr(attackindex,&At_Fire,&At_Water,&At_Earth,&At_Wind,&At_none);

	At_FieldPow = BATTLE_FieldAttAdjust(
		CHAR_getWorkInt( attackindex, CHAR_WORKBATTLEINDEX ),
		At_Fire, At_Water, At_Earth, At_Wind );

	At_Fire *= damage;	At_Water *= damage;	At_Earth *= damage;
	At_Wind *= damage;	At_none *= damage;
	BATTLE_GetAttr( defindex, &Df_Fire, &Df_Water,
		&Df_Earth,&Df_Wind, &Df_none );
	Df_FieldPow = BATTLE_FieldAttAdjust(
		CHAR_getWorkInt( defindex, CHAR_WORKBATTLEINDEX ),
		Df_Fire, Df_Water, Df_Earth, Df_Wind );
	damage = BATTLE_AttrCalc(
		At_Fire, At_Water, At_Earth, At_Wind, At_none,
		Df_Fire, Df_Water, Df_Earth, Df_Wind, Df_none );
	damage *= (At_FieldPow / Df_FieldPow);
	return damage;
}
#endif
// 计算闪避率
// return: 0:没闪过 1:闪过
int BATTLE_MagicDodge(int charindex,int nDefKind,int nFieldAttr)
{
	float fLuck = 0,fResist = 0;
#ifdef _EQUIT_DEFMAGIC
	float Dluck=0.0;
#endif
	int charType = CHAR_getInt( charindex, CHAR_WHICHTYPE);
	// 如果是防守者是玩家
	if( charType == CHAR_TYPEPLAYER ){
		fLuck = (float)CHAR_getInt( charindex, CHAR_LUCK) * 3;
		fResist = (float)CHAR_getInt( charindex, CHAR_EARTH_RESIST + nFieldAttr) * 0.15;
		fLuck += fResist;
#ifdef _EQUIT_DEFMAGIC
		Dluck = (float)(CHAR_getWorkInt( charindex, CHAR_EQUITQUIMAGIC)*0.9);
	    fLuck += Dluck;
#endif
	}else {// 如果是防守者是宠物
		fLuck = (float)CHAR_getInt( charindex, CHAR_LV) * 0.2;
		if(fLuck > 30) fLuck = 30;
	}
	if(rand()%100+1 > (int)fLuck)
		return 0;
	else
		return 1;
}


#define DEF_MAGIC_NUM	4

extern void BATTLE_changeRideImage( int index );

#ifdef _FIX_MAGICDAMAGE
void BATTLE_MultiAttMagic( int battleindex, int attackNo, int toNo,
						  int attIdx ,int FieldAttr ,int Power, int MagicLv)
#else
void BATTLE_MultiAttMagic( int battleindex, int attackNo, int toNo , int attIdx,
						  int FieldAttr , int Power)
#endif
{
    int  list[SIDE_OFFSET * 2 + 1];
    int  listidx,i,j,k,z;
    int  basex , basey , magicattidx,def_is_player[10],def_be_hit[10];
    int  att_magic_lv[4];
	int  def_magic_resist[DEF_MAGIC_NUM];
    int  attvalue , charahp , pethp , charaidx , petidx , charahurt,pet_att_lv = 1,pet_def_lv = 1,charahurt_temp;
    char szcommand[256];
	int  AttIsPlayer=0,DefIsPlayer=0;
	int getexp = 0;

#ifndef _FIX_MAGICDAMAGE
	int att_magic_exp_add,att_magic_exp_sub,def_magic_exp_add,def_magic_exp_sub;
	int lv_up_exp,DefFieldAttr = 0;
	float temp = 0.0f;
//	int attattr[5], defattr[5];
	char msgbuf[64];
	char kind[4][3] = {"地","水","火","风"};
#else
	BOOL TrueMagic=FALSE;
#endif

    // terry 


    if((z = BATTLE_MultiList(battleindex,toNo,list)) == -1)
		return;
	else
	{
		if(z != toNo) toNo = z;
	}

	BATTLE_AttMagicEffect( battleindex , attackNo , list , attIdx );

    // 计算攻击的人物清单
    if( attackNo < 10 )
        magicattidx = attIdx * 2 + 1;
    else
        magicattidx = attIdx * 2;

    listidx = 0;
		memset(def_is_player,-1,sizeof(def_is_player));
		memset(def_be_hit,-1,sizeof(def_be_hit));

    // 单人攻击
    if( toNo < 20 ){
		toNo = list[0];
       basex = CharTableIdx[toNo][1];
       basey = CharTableIdx[toNo][0];

       for( i = 0 , j = basey - 1 ; j <= basey + 1 ; i++ , j++ ){
          if( toNo < 10 && ( j < 2 || j > 3 ) )
             continue;
          else if( toNo >= 10 && ( j < 0 || j > 1 ) )
             continue;
          // 计算那些人需要加入清单
          for( k = 0 ; k < 5 ; k++ ){
             if( basex - 2 + k < 0 || basex - 2 + k > 4 )
                continue;

             if( ATTMAGIC_magic[magicattidx].siField[i][k] && TRUE == BATTLE_TargetCheck( battleindex , CharTable[j][basex - 2 + k] ) )
             {
                list[listidx] = CharTable[j][basex - 2 + k];
                listidx++;
             }
          }
       }
    }else if( 20 == toNo ){	    // 右下全体
       for( i = 0 ; i < 2 ; i++ ){
          for( j = 0 ; j < 5 ; j++ ){
             if( ATTMAGIC_magic[magicattidx].siField[i][j] && TRUE == BATTLE_TargetCheck( battleindex , CharTable[i + 2][j] ) ){
                list[listidx] = CharTable[i + 2][j];
                listidx++;
             }
          }
       }
    }else if( 21 == toNo ){	// 左上全体
       for( i = 0 ; i < 2 ; i++ ){
          for( j = 0 ; j < 5 ; j++ ){
             if( ATTMAGIC_magic[magicattidx].siField[i][j] && TRUE == BATTLE_TargetCheck( battleindex , CharTable[i][j] ) ){
                list[listidx] = CharTable[i][j];
                listidx++;
             }
          }
       }
    }
    // 左上第一列 , 左上第二列 , 右下第一列 , 右下第二列
    else if( 23 == toNo || 24 == toNo || 25 == toNo || 26 == toNo ){
       basey = toNo - 23;

       for( i = 0 , j = basey - 1 ; j <= basey + 1 ; i++ , j++ ){
          if( ( 25 == toNo || 26 == toNo ) && ( j < 2 || j > 3 ) )
             continue;
          else if( ( 23 == toNo || 24 == toNo ) && ( j < 0 || j > 1 ) )
             continue;

          // 计算那些人需要加入清单
          for( k = 0 ; k < 5 ; k++ ){
             if( ATTMAGIC_magic[magicattidx].siField[i][k] && TRUE == BATTLE_TargetCheck( battleindex , CharTable[j][k] ) )
             {
                list[listidx] = CharTable[j][k];
                listidx++;
             }
          }
       }
    }

		
		qsort( list , listidx , sizeof( list[0] ) , ( FUNC )SortLoc );// 排序位置

		// 取得人物的属性(不论玩家或是宠物)
		//BATTLE_GetAttr( BATTLE_No2Index( battleindex , attackNo ) , &attattr[2] , &attattr[1] , &attattr[0] , &attattr[3] , &attattr[4] );
		// 如果攻击者是玩家,取得人物的魔法熟练度及魔法抗性
		{
			int attType = CHAR_getInt(BATTLE_No2Index(battleindex,attackNo),CHAR_WHICHTYPE);
			int Check=0;
			AttIsPlayer=0;
			if( attType == CHAR_TYPEPLAYER)	{
				AttIsPlayer = 1;
				for(i=0;i<4;i++){	// att_magic_lv[i]: i = 0:地 1:水 2:火 3:风
					att_magic_lv[i] = CHAR_getInt(BATTLE_No2Index(battleindex,attackNo),CHAR_EARTH_EXP+i);
				}
			}else if( attType == CHAR_TYPEENEMY )	{
				for(i=0;i<4;i++){
					att_magic_lv[i] = (CHAR_getInt(BATTLE_No2Index(battleindex,attackNo), CHAR_LV)*0.9);
				}
			}else	{
#ifdef _FIX_MAGICDAMAGE
				AttIsPlayer = 1;
#endif
				for(i=0;i<4;i++){
#ifdef _FIX_MAGICDAMAGE
					att_magic_lv[i] = CHAR_getInt(BATTLE_No2Index(battleindex,attackNo),CHAR_EARTH_EXP+i);
#else
					att_magic_lv[i] = 0;
#endif
				}
			}
			Check = rand()%100;
#ifdef _FIX_MAGICDAMAGE
			if( Check > att_magic_lv[ FieldAttr] ) TrueMagic = FALSE;
			else TrueMagic = TRUE;
#endif
		}
		pet_att_lv = CHAR_getInt(BATTLE_No2Index(battleindex,attackNo), CHAR_LV);

		z = 0;
    for(i=0;i<listidx;i++){
	  charaidx = BATTLE_No2Index( battleindex , list[i] );
      petidx   = BATTLE_getRidePet( charaidx );
      //BATTLE_GetAttr( charaidx , &defattr[2] , &defattr[1] , &defattr[0] , &defattr[3] , &defattr[4] );
		{//andy_fix
			int defType = CHAR_getInt(charaidx,CHAR_WHICHTYPE);
			DefIsPlayer = 0;
			if( defType == CHAR_TYPEPLAYER)	{
				DefIsPlayer = 1;
				for(j=0;j<DEF_MAGIC_NUM;j++){	// def_magic_resist[i]: i = 0:地 1:水 2:火 3:风
					def_magic_resist[j] = CHAR_getInt(charaidx,CHAR_EARTH_RESIST+j);
#ifdef _EQUIT_DEFMAGIC
					def_magic_resist[j] += CHAR_getWorkInt( charaidx, CHAR_EQUITDEFMAGIC_E+j);
#endif
				}
			}else if( defType == CHAR_TYPEENEMY )	{
				for(j=0;j<DEF_MAGIC_NUM;j++){
					def_magic_resist[j] = (CHAR_getInt(charaidx,CHAR_LV)*0.5);
				}
			}else	{	//PET
#ifdef _FIX_MAGICDAMAGE
				DefIsPlayer = 1;
#endif
				for(j=0;j<DEF_MAGIC_NUM;j++){
#ifdef _FIX_MAGICDAMAGE
					def_magic_resist[j] = CHAR_getInt(charaidx,CHAR_EARTH_RESIST+j);
					//def_magic_resist[j] += CHAR_getWorkInt( charaidx, CHAR_EQUITDEFMAGIC_E+j);
#else
					def_magic_resist[j] = 0;
#endif
				}
			}

#ifdef _MAGIC_DEFMAGICATT
			if( CHAR_getWorkInt( charaidx, CHAR_DEFMAGICSTATUS ) > 0 ){
				float def = (float)(CHAR_getWorkInt( charaidx, CHAR_OTHERSTATUSNUMS ));
				def = def/100;
				for(j=0;j<DEF_MAGIC_NUM;j++){
					if( def_magic_resist[j] <= 0 ) continue;
					def_magic_resist[j] += def_magic_resist[j] * def;
				}
			}
#endif

		}
		pet_def_lv = CHAR_getInt(charaidx,CHAR_LV);
			if(BATTLE_MagicDodge(charaidx,DefIsPlayer,FieldAttr)){	// 计算闪避率
				attvalue = pethp = 0;
				if( 0 == ATTMAGIC_magic[magicattidx].uiAttackType )	{// 单人攻击
					sprintf( szcommand , "%X|%X|%X|%X|" , toNo , list[i] , attvalue , pethp );
				}else{	// 多人攻击
					sprintf( szcommand , "%X|%X|%X|%X|" , list[i] , list[i] , attvalue, pethp );
				}
				BATTLESTR_ADD( szcommand );
				continue;
			}else{	// 没闪过,计算攻击力
#ifdef _FIX_MAGICDAMAGE
				float Kmagic = ((float)att_magic_lv[FieldAttr]*1.4 - (float)def_magic_resist[FieldAttr]);
				float Mmagic = (float)att_magic_lv[FieldAttr];
				float Amagic=0.00;
				int APower=0;
				if( Kmagic < 0 ) Kmagic = 0;
				if( Mmagic < 1 ) Mmagic = 1;
				Amagic = (Kmagic*Kmagic) / (Mmagic*Mmagic);
				Amagic += ((float)(rand()%20))/100;
				APower = (int)(Power*(1+(float)MagicLv/10) * Amagic);	//MagicLv

				//andy_log
				//print("\natt_magic_lv:%d", att_magic_lv[FieldAttr]);
				//print( "Player:%s\n", CHAR_getUseName( charaidx));
				//print( "ANDY Magic Power:%d Amagic:%f APower:%d \n", Power, Amagic, APower);
 

				def_be_hit[getexp++] = charaidx;	// 记下被打的人的index
				attvalue = BATTLE_getMagicAdjustInt( BATTLE_No2Index(battleindex,attackNo), charaidx,
					MagicLv, FieldAttr, APower);

				//andy_log
//				print("attvalue:%d \n", attvalue);

				if( TrueMagic == FALSE )	{//使用失败
					attvalue*=0.7;
				}
				if( DefIsPlayer )	{
					Magic_ComputeDefExp( charaidx, FieldAttr, MagicLv, attvalue);
				}
#else
				def_be_hit[getexp++] = charaidx;	// 记下被打的人的index
				// 计算属性相克
				attvalue = BATTLE_AttrAdjust(BATTLE_No2Index(battleindex,attackNo),charaidx,Power);
				if(DefIsPlayer){	// 被打的是玩家
					def_is_player[z++] = list[i];
					if(AttIsPlayer){	// 如果是玩家用魔法攻击
						temp = ((float)att_magic_lv[FieldAttr] - (float)def_magic_resist[FieldAttr])/(float)def_magic_resist[FieldAttr] / 100;
						attvalue = Power + Power * temp + attvalue;
					}else{
						temp = ((float)pet_att_lv - (float)def_magic_resist[FieldAttr]) / (float)def_magic_resist[FieldAttr] / 100;
						attvalue = Power + Power * temp + attvalue;
					}
				}else{// 被打的是宠物
					if(AttIsPlayer){// 如果是玩家用魔法攻击
						temp = ((float)att_magic_lv[FieldAttr] - (float)pet_def_lv) / (float)pet_def_lv / 100;
						attvalue = Power + Power * temp + attvalue;
					}else	{
						temp = ((float)pet_att_lv - (float)pet_def_lv) / (float)pet_def_lv / 100;
						attvalue = Power + Power * temp + attvalue;
					}
				}
				// 上下取10
				attvalue += rand()%2 ? (rand()%10)+1 : -(rand()%10)-1;
				if( attvalue <= 0 ) attvalue = 1;
#endif
			}

      charahp = CHAR_getInt( charaidx , CHAR_HP );

       // 没有宠物
       if( -1 == petidx || CHAR_getInt( petidx , CHAR_HP ) <= 0 )
       {
		   if( ( charahp -= attvalue ) < 0 )	{
             charahp = 0;
			 //ANDY_ADD
			 //CHAR_setFlg( charaidx, CHAR_ISDIE, TRUE);
		   }

          CHAR_setInt( charaidx , CHAR_HP , charahp );
          pethp = 0;

          // 单人攻击
          if( 0 == ATTMAGIC_magic[magicattidx].uiAttackType )
             sprintf( szcommand , "%X|%X|%X|%X|" , toNo , list[i] , attvalue , pethp );
          // 多人攻击
          else
             sprintf( szcommand , "%X|%X|%X|%X|" , list[i] , list[i] , attvalue, pethp );
       }
       else
       {
          pethp     = CHAR_getInt( petidx , CHAR_HP ); 
          charahurt = attvalue * BATTLE_CalcCharaRatio( FieldAttr , charaidx ) / 10;

					charahurt_temp = charahurt;
          if( ( charahp -= charahurt ) < 0 )
          {
            charahurt = charahp;
            charahp   = 0;
			//ANDY_ADD
			//CHAR_setFlg( charaidx, CHAR_ISDIE, TRUE);
          }
					
          attvalue = attvalue - charahurt;
          if( ( pethp -= attvalue ) < 0 )
          {
            pethp = 0;
						// 宠物没血了退出战场
						CHAR_setInt(charaidx,CHAR_RIDEPET,-1);
						BATTLE_changeRideImage(charaidx);
						CHAR_setWorkInt(charaidx,CHAR_WORKPETFALL,1);
          }
          CHAR_setInt( charaidx , CHAR_HP , charahp );
          CHAR_setInt( petidx , CHAR_HP , pethp );

					charahurt = charahurt_temp;

          // 单人攻击
          if( 0 == ATTMAGIC_magic[magicattidx].uiAttackType )
             sprintf( szcommand , "%X|%X|%X|%X|" , toNo , list[i] , charahurt , attvalue );
          // 多人攻击
          else
             sprintf( szcommand , "%X|%X|%X|%X|" , list[i] , list[i] , charahurt, attvalue );
       }

		{
				//Change fix 加上这个先拿到DP值
				int aAttackList[BATTLE_ENTRY_MAX*2+1];
				aAttackList[0] = attackNo;
				aAttackList[1] = -1;
				BATTLE_AddProfit( battleindex, aAttackList);
		}
		//change fix 让替身娃娃可正常运作
		if(	CHAR_getInt( charaidx , CHAR_HP ) <= 0 
			&& CHAR_getInt( charaidx, CHAR_WHICHTYPE ) == CHAR_TYPEPLAYER 
			/*&& !BattleArray[battleindex].dpbattle*/ )
			CHAR_setFlg( charaidx, CHAR_ISDIE, 1 );
       
		BATTLESTR_ADD( szcommand );
    }

    sprintf( szcommand , "%X|" , 0x12345678 );
    BATTLESTR_ADD( szcommand );
#ifdef _FIX_MAGICDAMAGE
	if( (TrueMagic == FALSE) && AttIsPlayer)	{	//计算攻击方经验值
		Magic_ComputeAttExp( BATTLE_No2Index(battleindex,attackNo), FieldAttr, MagicLv, getexp);
	}
	for(i=0; i<listidx; i++)	{
		// 如果被攻击者中了睡眠
		if(CHAR_getWorkInt(def_be_hit[i],CHAR_WORKSLEEP) > 0) {
			CHAR_setWorkInt(def_be_hit[i],CHAR_WORKSLEEP,0);
			z = BATTLE_Index2No(battleindex,def_be_hit[i]);
			sprintf(szcommand,"BM|%X|%X|",z,0);
			BATTLESTR_ADD(szcommand);
		}
	}
#else
		//TrueMagic == FALSE
		// 攻击方的经验值-----------------------------------------------------------------------------
		if(AttIsPlayer){
			// 被克的魔法
			DefFieldAttr = (FieldAttr+1)%4;
			// 取得玩家所用的魔法的经验值
			att_magic_exp_add = CHAR_getInt(BATTLE_No2Index(battleindex,attackNo),CHAR_EARTH_ATTMAGIC_EXP+FieldAttr);
			// 取得玩家所用的魔法的被克魔法经验值
			att_magic_exp_sub = CHAR_getInt(BATTLE_No2Index(battleindex,attackNo),CHAR_EARTH_ATTMAGIC_EXP+DefFieldAttr);
			// 依所使用的魔法加该属性的exp,打几只算几点exp
			att_magic_exp_add += getexp;									
			// 依所使用的魔法扣被克属性的exp,打几只扣几点exp
			att_magic_exp_sub -= getexp;
			lv_up_exp = Magic_Level_Table[att_magic_lv[FieldAttr]];
			while(att_magic_exp_add >= lv_up_exp || att_magic_exp_sub < 0){
				// 如果玩家所用的魔法的经验值现在可以升级了
				if(att_magic_exp_add >= lv_up_exp){
					// 如果超过了100级
					if(att_magic_lv[FieldAttr] + 1 > 100){
						att_magic_lv[FieldAttr] = 100;
						// 经验值清为0
						att_magic_exp_add = 0;
					}else{
						att_magic_lv[FieldAttr]++;
						// 把现在的经验值扣掉升了级所用的经验值
						att_magic_exp_add -= lv_up_exp;
						snprintf(msgbuf,sizeof(msgbuf),"你的 %s 魔法熟练度升级为 %d。",kind[FieldAttr],att_magic_lv[FieldAttr]);
		        CHAR_talkToCli(BATTLE_No2Index(battleindex,attackNo),-1,msgbuf,CHAR_COLORRED);
					}
				}
				// 如果玩家所用的魔法的相克魔法的经验值现在可以降级了
				if(att_magic_exp_sub < 0 ){
					// 如果等级已经是1级
					if(att_magic_lv[DefFieldAttr] <= 1){
						att_magic_lv[DefFieldAttr] = 1;
						// 经验值清为0
						att_magic_exp_sub = 0;
					}else{
						att_magic_lv[DefFieldAttr]--;
						// 把下一个等级的经验值扣掉现在的经验值
						att_magic_exp_sub = Magic_Level_Table[att_magic_lv[DefFieldAttr]] + att_magic_exp_sub;
						snprintf(msgbuf,sizeof(msgbuf),"你的 %s 魔法熟练度降级为 %d。",kind[DefFieldAttr],att_magic_lv[DefFieldAttr]);
		        CHAR_talkToCli(BATTLE_No2Index(battleindex,attackNo),-1,msgbuf,CHAR_COLORRED);
					}
				}
				lv_up_exp = Magic_Level_Table[att_magic_lv[FieldAttr]];
			}
			CHAR_setInt(BATTLE_No2Index(battleindex,attackNo),CHAR_EARTH_EXP+FieldAttr,att_magic_lv[FieldAttr]);
			CHAR_setInt(BATTLE_No2Index(battleindex,attackNo),CHAR_EARTH_ATTMAGIC_EXP+FieldAttr,att_magic_exp_add);
			CHAR_setInt(BATTLE_No2Index(battleindex,attackNo),CHAR_EARTH_EXP+DefFieldAttr,att_magic_lv[DefFieldAttr]);
			CHAR_setInt(BATTLE_No2Index(battleindex,attackNo),CHAR_EARTH_ATTMAGIC_EXP+DefFieldAttr,att_magic_exp_sub);
		}

		// 防守方的经验值-----------------------------------------------------------------------------
		for(i=0;i<listidx;i++){
			if(def_is_player[i] != -1){
				charaidx = BATTLE_No2Index(battleindex,def_is_player[i]);
				// 取得防守方玩家对此魔法的抗性等级
				def_magic_resist[FieldAttr] = CHAR_getInt(charaidx,CHAR_EARTH_RESIST+FieldAttr);
				// 取得防守方玩家对此魔法的抗性经验值
				def_magic_exp_add = CHAR_getInt(charaidx,CHAR_EARTH_DEFMAGIC_EXP+FieldAttr);
				// 取得防守方玩家对此魔法被克的抗性等级
				def_magic_resist[DefFieldAttr] = CHAR_getInt(charaidx,CHAR_EARTH_RESIST+DefFieldAttr);
				// 取得防守方玩家对此魔法被克的抗性经验值
				def_magic_exp_sub = CHAR_getInt(charaidx,CHAR_EARTH_DEFMAGIC_EXP+DefFieldAttr);
				def_magic_exp_add++;
				def_magic_exp_sub--;
				
				lv_up_exp = Magic_Level_Table[def_magic_resist[FieldAttr]];
				
				while(def_magic_exp_add >= lv_up_exp || def_magic_exp_sub < 0){
					// 如果防守方玩家对此魔法的抗性的经验值现在可以升级了
					if(def_magic_exp_add >= lv_up_exp){
						// 如果超过了100级
						if(def_magic_resist[FieldAttr] + 1 > 100) def_magic_resist[FieldAttr] = 100;
						else {
							def_magic_resist[FieldAttr]++;
							snprintf(msgbuf,sizeof(msgbuf),"你的 %s 魔法抗性升级为 %d。",kind[FieldAttr],def_magic_resist[FieldAttr]);
							CHAR_talkToCli(charaidx,-1,msgbuf,CHAR_COLORRED);
						}
						// 经验值清为0
						def_magic_exp_add = 0;
					}
					// 如果防守方玩家对此魔法的抗性的相克魔法的经验值现在可以降级了
					if(def_magic_exp_sub < 0 ) {
						// 如果等级已经是1级
						if(def_magic_resist[DefFieldAttr] <= 1) def_magic_exp_sub = 0; // 经验值清为0
						else {
							def_magic_resist[DefFieldAttr]--;
							// 经验值变成下一个等级的经验值
							def_magic_exp_sub = Magic_Level_Table[def_magic_resist[DefFieldAttr]];
							snprintf(msgbuf,sizeof(msgbuf),"你的 %s 魔法抗性降级为 %d。",kind[DefFieldAttr],def_magic_resist[DefFieldAttr]);
							CHAR_talkToCli(charaidx,-1,msgbuf,CHAR_COLORRED);
						}
					}
					lv_up_exp = Magic_Level_Table[def_magic_resist[FieldAttr]];
				}
				CHAR_setInt(charaidx,CHAR_EARTH_RESIST+FieldAttr,def_magic_resist[FieldAttr]);
				CHAR_setInt(charaidx,CHAR_EARTH_DEFMAGIC_EXP+FieldAttr,def_magic_exp_add);
				CHAR_setInt(charaidx,CHAR_EARTH_RESIST+DefFieldAttr,def_magic_resist[DefFieldAttr]);
				CHAR_setInt(charaidx,CHAR_EARTH_DEFMAGIC_EXP+DefFieldAttr,def_magic_exp_sub);
			}
			// 如果被攻击者中了睡眠
			if(CHAR_getWorkInt(def_be_hit[i],CHAR_WORKSLEEP) > 0) {
				CHAR_setWorkInt(def_be_hit[i],CHAR_WORKSLEEP,0);
				z = BATTLE_Index2No(battleindex,def_be_hit[i]);
				sprintf(szcommand,"BM|%X|%X|",z,0);
				BATTLESTR_ADD(szcommand);
			}
		}
#endif
}

#endif

#ifdef _MAGIC_TOCALL
void BATTLE_MultiToCallDragonMagic( int battleindex, int attackNo, int toNo,
						  int attIdx ,int FieldAttr ,int Power, int ImageNo)
{
    int  list[SIDE_OFFSET * 2 + 1];
    int  listidx,i,j,k,z;
    int  /*basex , */basey , magicattidx,def_is_player[10],def_be_hit[10];
    int  attvalue , charahp , pethp , charaidx , petidx , charahurt,charahurt_temp;
    char szcommand[256];
	int  /*AttIsPlayer = 0,*/DefIsPlayer = 0;

#ifndef _FIX_MAGICDAMAGE
	int att_magic_exp_add,att_magic_exp_sub,def_magic_exp_add,def_magic_exp_sub;
	int lv_up_exp,DefFieldAttr = 0;
	float temp = 0.0f;
	char msgbuf[64];
#else
#endif

	int icindex, ioindex;
    // terry 
    if((z = BATTLE_MultiList(battleindex,toNo,list)) == -1)
		return;
	else{
		if(z != toNo) toNo = z;
	}

	TOCALL_magic[attIdx].uiSpriteNum = ImageNo;

	BATTLE_ToCallDragonEffect( battleindex , attackNo , list , attIdx);

    // 计算攻击的人物清单
	if( attackNo < 10 )
		magicattidx = attIdx * 2 + 1;
	else
		magicattidx = attIdx * 2;


	listidx = 0;
	memset(def_is_player,-1,sizeof(def_is_player));
	memset(def_be_hit,-1,sizeof(def_be_hit));

    // 单人攻击
	if( toNo < 20 ){
		toNo = list[0];
		listidx++;

	} else if( 20 == toNo ){	    // 右下全体
		for( i = 0 ; i < 2 ; i++ ){
			for( j = 0 ; j < 5 ; j++ ){
				if(TRUE == BATTLE_TargetCheck( battleindex , CharTable[i + 2][j] ) ){
					list[listidx] = CharTable[i + 2][j];
					listidx++;
				}
			}
		}
	} else if( 21 == toNo ){	// 左上全体
		for( i = 0 ; i < 2 ; i++ ){
			for( j = 0 ; j < 5 ; j++ ){
				if(TRUE == BATTLE_TargetCheck( battleindex , CharTable[i][j] ) ){
					list[listidx] = CharTable[i][j];
					listidx++;
				}
			}
		}
	} // 左上第一列 , 左上第二列 , 右下第一列 , 右下第二列
		else if( 23 == toNo || 24 == toNo || 25 == toNo || 26 == toNo ){
		basey = toNo - 23;
		for( i = 0 , j = basey - 1 ; j <= basey + 1 ; i++ , j++ ){
			if( ( 25 == toNo || 26 == toNo ) && ( j < 2 || j > 3 ) )
				continue;
			else if( ( 23 == toNo || 24 == toNo ) && ( j < 0 || j > 1 ) )
				continue;
			// 计算那些人需要加入清单
			for( k = 0 ; k < 5 ; k++ ){
				if(TRUE == BATTLE_TargetCheck( battleindex , CharTable[j][k] ) )
				{
					list[listidx] = CharTable[j][k];
					listidx++;
				}
			}
		}
	}
	qsort( list , listidx , sizeof( list[0] ) , ( FUNC )SortLoc );// 排序位置
	for( i = 0 ; i < listidx ; i++ ) {
		int defType;
		charaidx = BATTLE_No2Index( battleindex , list[i] );
		petidx   = BATTLE_getRidePet( charaidx );
		defType = CHAR_getInt(charaidx, CHAR_WHICHTYPE);
		DefIsPlayer = 0;
		if (defType == CHAR_TYPEPLAYER) {
			DefIsPlayer = 1;
		}
		if (CHAR_createCharacter( 100354 /*image no*/, 100, 600, 370, 0,
			&icindex, &ioindex, 0) == TRUE) {

//		CHAR_setInt( icindex, CHAR_STR, 100);
			CHAR_setInt( icindex, CHAR_LUCK, 1);
			CHAR_setWorkInt( icindex, CHAR_WORKATTACKPOWER,
					Power );
			CHAR_setWorkInt( icindex, CHAR_WORKOTHERDMAGE, 
					0);

			if (BATTLE_MagicDodge(charaidx, DefIsPlayer, FieldAttr)) {
// 没击中
				attvalue = 0;
			} else {
				if( 0 == TOCALL_magic[attIdx].uiAttackType )	{// 单人攻击
					attvalue = BATTLE_DamageCalc(icindex, charaidx);
				}
				else {
					attvalue = BATTLE_DamageCalc(icindex, charaidx);
				}
			}
			CHAR_CharaDelete(icindex);
			charahp = CHAR_getInt( charaidx , CHAR_HP );
			if (-1 == petidx || CHAR_getInt(petidx, CHAR_HP) <= 0) {
// 没骑宠物
				if( ( charahp -= attvalue ) < 0 )	{
					charahp = 0;
				}
				CHAR_setInt( charaidx , CHAR_HP , charahp );
				pethp = 0;
				if( 0 == TOCALL_magic[attIdx].uiAttackType )	{// 单人攻击
					sprintf( szcommand , "%X|%X|%X|%X|" , toNo , list[i] , attvalue , pethp);
				} else
					sprintf( szcommand , "%X|%X|%X|%X|" , list[i] , list[i] , attvalue , pethp);
			}else{
				pethp     = CHAR_getInt( petidx , CHAR_HP ); 
				charahurt = attvalue * BATTLE_CalcCharaRatio( FieldAttr , charaidx ) / 10;

				charahurt_temp = charahurt;
				if( ( charahp -= charahurt ) < 0 )
				{
					charahurt = charahp;
					charahp   = 0;
				}
					
				attvalue = attvalue - charahurt;
				if( ( pethp -= attvalue ) < 0)
				{
					pethp = 0;
					// 宠物没血了退出战场
					CHAR_setInt(charaidx,CHAR_RIDEPET,-1);
					BATTLE_changeRideImage(charaidx);
					CHAR_setWorkInt(charaidx,CHAR_WORKPETFALL,1);
				}
				CHAR_setInt( charaidx , CHAR_HP , charahp );
				CHAR_setInt( petidx , CHAR_HP , pethp );

				charahurt = charahurt_temp;

				if( 0 == TOCALL_magic[attIdx].uiAttackType )	{// 单人攻击
					sprintf( szcommand , "%X|%X|%X|%X|" , toNo , list[i] , charahurt , attvalue);
				} else
					sprintf( szcommand , "%X|%X|%X|%X|" , list[i] , list[i] , charahurt , attvalue);

			}

			BATTLESTR_ADD( szcommand );
		}
	}

	sprintf( szcommand , "%X|" , 0x5711438 );
	BATTLESTR_ADD( szcommand );

}

#endif

//***********************************************************
//
//    芊羁匹  醒及平乓仿弁正□毛汊唾今六月
//
void BATTLE_MultiRessurect(
	int battleindex, 	// 田玄伙奶件犯永弁旦
	int attackNo,		// 井仃月谛  寞
	int toNo,			// 井仃日木月谛  寞
	int power,			// 湘  荚汊袄
	int per,			// ⊙井＂
	int UseEffect,		// 银丹谛及巨白尼弁玄
	int RecevEffect		// 井仃日木月谛及巨白尼弁玄
)
//
//***********************************************************
{
	int i, toindex, UpPoint = 0, workhp;
	int ToList[SIDE_OFFSET*2+1];

	//char szBuffer[256]="";
	char szCommand[256];


	BATTLE_MultiListDead( battleindex, toNo, ToList );

	BATTLE_MagicEffect(
		battleindex, 			// 田玄伙奶件犯永弁旦
		attackNo, 				// 井仃月谛  寞(ㄟ  ㄠㄨ)
		ToList, 				// 井仃日木月谛  寞伉旦玄(ㄟ  ㄠㄨ)
		UseEffect, 				// 井仃月谛巨白尼弁玄
		RecevEffect				// 井仃日木月谛巨白尼弁玄
	);

	// 汊唾今六月
	for( i = 0; ToList[i] != -1; i ++ ){
		// 锹澎及奶件犯永弁旦
		toindex = BATTLE_No2Index( battleindex, ToList[i] );

		//           反皿伊奶乩□及戏五忒曰  仄
		if( BattleArray[battleindex].type == BATTLE_TYPE_P_vs_P
		&& CHAR_getInt( toindex, CHAR_WHICHTYPE ) == CHAR_TYPEPLAYER
		){
			continue;
		}

		// 竣濮仄化中卅仃木壬汊唾今六卅中
		if( CHAR_getFlg( toindex, CHAR_ISDIE ) == FALSE ){
			continue;
		}

		// 公及引引及煌遥
		UpPoint = power;

		if( per ){
			// ⊙煌遥
			UpPoint *= CHAR_getWorkInt( toindex, CHAR_WORKMAXHP ) * 0.01;
		}

		if( power == 0 ){
			// 仇及桦宁反敦蝈荚汊
			UpPoint = CHAR_getWorkInt( toindex, CHAR_WORKMAXHP );
		}else{
			// 荚汊汹卞汔毛  凶六月(漆反赝癫)
			UpPoint = RAND( (power*0.9), (power*1.1) );
		}
		//   斓匹手ㄠ
		UpPoint = max( 1, UpPoint );

		workhp = CHAR_getInt( toindex, CHAR_HP ) + UpPoint;

		//     毛译尹卅中方丹卞荚汊
		CHAR_setInt( toindex, CHAR_HP,
			min( workhp, CHAR_getWorkInt( toindex, CHAR_WORKMAXHP ) ) );

		// 汊唾
		CHAR_setFlg( toindex, CHAR_ISDIE, 0 );

		//snprintf( szBuffer, sizeof(szBuffer),
		//	"(%s)恢复意识",
		//		CHAR_getUseName( toindex ) );

		//   须  煤
		//BATTLE_BroadCast( battleindex, szBuffer,
		//	(attackNo >= 10)? CHAR_COLORGRAY : CHAR_COLORPURPLE ) ;

		// (汊唾)戊穴件玉毛中木月
		snprintf( szCommand, sizeof(szCommand),
				"BL|r%X|h%X|", ToList[i], CHAR_getInt( toindex, CHAR_HP ) );

		//   侬  馨笛
		BATTLESTR_ADD( szCommand );
/*
		{	int Sign, Damage;
			Sign = (UpPoint >= 0)?(1):(0);
			Damage = ABS( UpPoint );
			// (    荚汊)戊穴件玉毛中木月
			snprintf( szCommand, sizeof(szCommand),
				"BD|r%X|%X|%X|d%X|", ToList[i], 0, Sign, Damage );
		}
		//   侬  馨笛
		BATTLESTR_ADD( szCommand );
*/

		// 衬午及爵    匹伉旦弁互卅井匀凶日
		if( BattleArray[battleindex].norisk == 0
		 &&	BattleArray[battleindex].type == BATTLE_TYPE_P_vs_E
		 ){
			// 仇仇匹矢永玄分匀凶日袄毛皿仿旦允月
			if( CHAR_getInt( toindex, CHAR_WHICHTYPE ) == CHAR_TYPEPET ){
				CHAR_PetAddVariableAi( toindex, AI_FIX_PETRESSURECT );
			}
		}
	}

	return;

}

#ifdef _Item_ReLifeAct
void BATTLE_MultiReLife( int battleindex, int attackNo, int toNo, 
						int power, int RecevEffect )
{
	int i, toindex, UpPoint = 0, workhp;
	int ToList[SIDE_OFFSET*2+1];
	//char szBuffer[256]="";
	char szCommand[256];
	char buf1[256];

	memset(szCommand, 0, sizeof(szCommand));
	BATTLE_MultiListDead( battleindex, toNo, ToList );


	//snprintf( szCommand, sizeof(szCommand),	"Bj|");
	for( i = 0; ToList[i] != -1; i ++ ){
		toindex = BATTLE_No2Index( battleindex, ToList[i] );
		//if( BattleArray[battleindex].type == BATTLE_TYPE_P_vs_P
		//&& CHAR_getInt( toindex, CHAR_WHICHTYPE ) == CHAR_TYPEPLAYER ){
		//	continue;
		//}
		if( CHAR_getFlg( toindex, CHAR_ISDIE ) == FALSE ){
			continue;
		}
		UpPoint = power;
		workhp = max( 1, UpPoint );

		CHAR_setInt( toindex, CHAR_HP, min( workhp, CHAR_getWorkInt( toindex, CHAR_WORKMAXHP ) ) );
		CHAR_setFlg( toindex, CHAR_ISDIE, 0 );
		//snprintf( szBuffer, sizeof(szBuffer), "(%s)恢复意识", CHAR_getUseName( toindex ) );
		snprintf( buf1, sizeof(buf1),
			"BJ|a%X|m%X|e%X|e%X|FF|",
			ToList[i],
			CHAR_getInt( toindex, CHAR_MP ),
			RecevEffect, //MyEffect,
			0  //ToEffect
		);
		strcat( szCommand, buf1);

		snprintf( buf1, sizeof(buf1),
			"BL|r%X|h%X|", ToList[i], CHAR_getInt( toindex, CHAR_HP ) );
		strcat( szCommand, buf1);

		if( BattleArray[battleindex].norisk == 0
		 &&	BattleArray[battleindex].type == BATTLE_TYPE_P_vs_E ){
			if( CHAR_getInt( toindex, CHAR_WHICHTYPE ) == CHAR_TYPEPET ){
				CHAR_PetAddVariableAi( toindex, AI_FIX_PETRESSURECT );
			}
		}
	}
	//strcat( szCommand, "FF|");
	BATTLESTR_ADD( szCommand );
	return;
}
#endif

void BATTLE_MultiStatusChange(
	int battleindex, 	// 田玄伙奶件犯永弁旦
	int attackNo,		// 井仃月谛  寞
	int toNo,			// 井仃日木月谛  寞
	int status,			// 旦  □正旦潘  
	int turn,			// 正□件醒
	int UseEffect,		// 银丹谛及巨白尼弁玄
	int RecevEffect,	// 井仃日木月谛及巨白尼弁玄
	int Success			// 岳    
)
{
	int i, toindex, charaindex;
	int perStatus;
	int ToList[SIDE_OFFSET*2+1];
	//char szBuffer[256]="";

	BATTLE_MultiList( battleindex, toNo, ToList );
	BATTLE_MagicEffect(
		battleindex, 			// 田玄伙奶件犯永弁旦
		attackNo, 				// 井仃月谛  寞(ㄟ  ㄠㄨ)
		ToList, 				// 井仃日木月谛  寞伉旦玄(ㄟ  ㄠㄨ)
		UseEffect, 				// 井仃月谛巨白尼弁玄
		RecevEffect				// 井仃日木月谛巨白尼弁玄
	);

	charaindex = BATTLE_No2Index( battleindex, attackNo );
	for( i = 0; ToList[i] != -1; i ++ ){
		toindex = BATTLE_No2Index( battleindex, ToList[i] );
		if( BATTLE_StatusAttackCheck( charaindex,
			toindex, status, Success, 30, 1.0, &perStatus ) == FALSE ){
			//snprintf( szBuffer, sizeof(szBuffer),
			//	"(%s)无法将(%s)做成(%s)(%d%%)",
			//	CHAR_getUseName( charaindex ),
			//	CHAR_getUseName( toindex ),
			//	aszStatusFull[status],
			//	perStatus
			//);
			//BATTLE_BroadCast( battleindex, szBuffer,
			//	(attackNo >= 10)? CHAR_COLORGRAY : CHAR_COLORPURPLE ) ;
		}else{
			CHAR_setWorkInt( toindex, StatusTbl[status], turn );

			if( status == BATTLE_ST_PARALYSIS
			||  status == BATTLE_ST_SLEEP
			||  status == BATTLE_ST_STONE
			||  status == BATTLE_ST_BARRIER	){
				CHAR_setWorkInt( toindex, CHAR_WORKBATTLECOM1, BATTLE_COM_NONE );
			}

			//snprintf( szBuffer, sizeof(szBuffer),
			//	"(%s)将(%s)做成(%s)(%d%%)",
			//	CHAR_getUseName( charaindex ),
			//	CHAR_getUseName( toindex ),
			//	aszStatusFull[status],
			//	perStatus
			//);
			//BATTLE_BroadCast( battleindex, szBuffer,
			//	(attackNo >= 10)? CHAR_COLORGRAY : CHAR_COLORPURPLE ) ;
			BATTLE_BadStatusString( ToList[i], status );
		}
	}
	return;
}

#ifdef _OTHER_MAGICSTAUTS
void BATTLE_MultiMagicStatusChange( int battleindex, int attackNo, int toNo, int status, int turn,
	int UseEffect, int RecevEffect, int nums )
{
	int i, j, toindex, charaindex;
	int ToList[SIDE_OFFSET*2+1];

	BATTLE_MultiList( battleindex, toNo, ToList );
	BATTLE_MagicEffect( battleindex, attackNo, ToList, UseEffect, RecevEffect );

	charaindex = BATTLE_No2Index( battleindex, attackNo );
	for( i = 0; ToList[i] != -1; i ++ ){
		toindex = BATTLE_No2Index( battleindex, ToList[i] );
		for( j=1; j<MAXSTATUSTYPE; j++)	{
			if( CHAR_getWorkInt( toindex, MagicTbl[j]) <= 0 ) continue;
			break;
		}
		if( j == MAXSTATUSTYPE )	{
//			char szBuffer[256]="";
			CHAR_setWorkInt( toindex, MagicTbl[status], turn );
			CHAR_setWorkInt( toindex, CHAR_OTHERSTATUSNUMS, nums );
//			sprintf( szBuffer, "Bm|%X|%X|", ToList[i], status );
//			BATTLESTR_ADD( szBuffer );
		}
	}
	return;
}
#endif



//***********************************************************
//
//    芊羁匹  醒及平乓仿弁正□毛旦  □正旦荚汊
//
void BATTLE_MultiStatusRecovery(
	int battleindex, 	// 田玄伙奶件犯永弁旦
	int attackNo,		// 井仃月谛  寞
	int toNo,			// 井仃日木月谛  寞
	int status,			// 旦  □正旦潘  
	int UseEffect,		// 银丹谛及巨白尼弁玄
	int RecevEffect		// 井仃日木月谛及巨白尼弁玄
)
//
//***********************************************************
{

	int i, j, toindex, charaindex, tostatus;
	int ToList[SIDE_OFFSET*2+1];


	//char szBuffer[256]="";


	BATTLE_MultiList( battleindex, toNo, ToList );

	BATTLE_MagicEffect(
		battleindex, 			// 田玄伙奶件犯永弁旦
		attackNo, 				// 井仃月谛  寞(ㄟ  ㄠㄨ)
		ToList, 				// 井仃日木月谛  寞伉旦玄(ㄟ  ㄠㄨ)
		UseEffect, 				// 井仃月谛巨白尼弁玄
		RecevEffect				// 井仃日木月谛巨白尼弁玄
	);

	charaindex = BATTLE_No2Index( battleindex, attackNo );

	// 旦  □正旦唱橘荚汊今六月
	for( i = 0; ToList[i] != -1; i ++ ){
		// 锹澎及奶件犯永弁旦
		toindex = BATTLE_No2Index( battleindex, ToList[i] );
		// 锹澎互升及旦  □正旦井＂
		tostatus = 0;
		//检查异常状态
		for( j = 1; j < BATTLE_ST_END; j ++ ){
			if( CHAR_getWorkInt( toindex, StatusTbl[j] ) > 0 ){
				tostatus = j;
			}
		}
		//======== 仇仇匹岳  井升丹井毛  烂今六月 ==========
		if( tostatus == 0 ){//正常
			// 撩  凛
//	print("\n vincent--很健康");
			//snprintf( szBuffer, sizeof(szBuffer),
			//	"(%s)很健康",
			//	CHAR_getUseName( toindex ) );
		}else
		// 蝈莒隙烂匹尕麻僖分匀凶日
		if( ( status == 0 && tostatus != 0 && tostatus <= CHAR_WORKCONFUSION ) // 蝈莒隙烂匹尕麻僖//找到异常
		||	( status == tostatus ) // 唱橘午荚汊椭互穴永民仄凶
		){
			// 荚汊允月
			CHAR_setWorkInt( toindex, StatusTbl[tostatus], 0 );
#ifdef _MAGIC_NOCAST
			if( StatusTbl[tostatus] == CHAR_WORKNOCAST ){
            //print("\nvincent-->lssproto_NC_send");
			lssproto_NC_send( getfdFromCharaIndex( toindex ), 0);//沉默解除
			}
#endif
//	print("\n vincent--解除了:%d",tostatus);
			// 岳  凛
			//snprintf( szBuffer, sizeof(szBuffer),
			//	"(%s)解除了(%s)的(%s)",
			//	CHAR_getUseName( charaindex ),
			//	CHAR_getUseName( toindex ),
			//	aszStatusFull[tostatus]
			//);
			// 仇仇匹旦  □正旦唱橘迕  侬  戊穴件玉毛中木月
			BATTLE_BadStatusString( ToList[i], 0 );//解除异常

		}else{
//	print("\n vincent--不");
			// 撩  凛
/*			snprintf( szBuffer, sizeof(szBuffer),
				"(%s)不(%s)",
				CHAR_getUseName( toindex ),
				aszStatusFull[status]
			);
*/
		}
		//   须  煤
		//BATTLE_BroadCast( battleindex, szBuffer,
		//	(attackNo >= 10)? CHAR_COLORGRAY : CHAR_COLORPURPLE ) ;
	}

	return;
}

//***********************************************************
//
//    芊羁匹  醒及平乓仿弁正□毛  芊  豢
//
void BATTLE_MultiMagicDef(
	int battleindex, 	// 田玄伙奶件犯永弁旦
	int attackNo,		// 井仃月谛  寞
	int toNo,			// 井仃日木月谛  寞
	int kind,			// 升及  芊  豢井
	int count,			// 荚醒
	int UseEffect,		// 银丹谛及巨白尼弁玄
	int RecevEffect		// 井仃日木月谛及巨白尼弁玄
)
//
//***********************************************************
{

	int i, toindex, charaindex;
	int ToList[SIDE_OFFSET*2+1];

	//char szBuffer[256]="";


	BATTLE_MultiList( battleindex, toNo, ToList );

	BATTLE_MagicEffect(
		battleindex, 			// 田玄伙奶件犯永弁旦
		attackNo, 				// 井仃月谛  寞(ㄟ  ㄠㄨ)
		ToList, 				// 井仃日木月谛  寞伉旦玄(ㄟ  ㄠㄨ)
		UseEffect, 				// 井仃月谛巨白尼弁玄
		RecevEffect				// 井仃日木月谛巨白尼弁玄
	);

	charaindex = BATTLE_No2Index( battleindex, attackNo );
	for( i = 0; ToList[i] != -1; i ++ ){
		toindex = BATTLE_No2Index( battleindex, ToList[i] );
		CHAR_setWorkInt( toindex, MagicDefTbl[kind], count );
		//snprintf( szBuffer, sizeof(szBuffer),
		//	"(%s)将(%s)遭受(损伤%s)",
		//	CHAR_getUseName( charaindex ),
		//	CHAR_getUseName( toindex ),
		//	aszMagicDefFull[kind]
		//);
		//BATTLE_BroadCast( battleindex, szBuffer,
		//	(attackNo >= 10)? CHAR_COLORGRAY : CHAR_COLORPURPLE ) ;
	}

	return;



}


#if 1

//***********************************************************
//
//    芊羁匹  醒及平乓仿弁正□及由仿丢□正毛晓票今六月
//
void BATTLE_MultiParamChange(
	int battleindex, 	// 田玄伙奶件犯永弁旦
	int attackNo,		// 井仃月谛  寞
	int toNo,			// 井仃日木月谛  寞
	int kind,			// 升及由仿丢□正井
	int power,			// 禾奶件玄
	int par,			// ⊙煌遥允月
	int UseEffect,		// 银丹谛及巨白尼弁玄
	int RecevEffect		// 井仃日木月谛及巨白尼弁玄
)
//
//***********************************************************
{

	int i, toindex, charaindex;
	int ToList[SIDE_OFFSET*2+1];
	int res = 0, work;
	//char szBuffer[256]="";


	BATTLE_MultiList( battleindex, toNo, ToList );

	BATTLE_MagicEffect(
		battleindex, 			// 田玄伙奶件犯永弁旦
		attackNo, 				// 井仃月谛  寞(ㄟ  ㄠㄨ)
		ToList, 				// 井仃日木月谛  寞伉旦玄(ㄟ  ㄠㄨ)
		UseEffect, 				// 井仃月谛巨白尼弁玄
		RecevEffect				// 井仃日木月谛巨白尼弁玄
	);

	charaindex = BATTLE_No2Index( battleindex, attackNo );

	// 升及由仿丢□正卞允月井
	for( i = 0; ToList[i] != -1; i ++ ){

		// 锹澎及奶件犯永弁旦
		toindex = BATTLE_No2Index( battleindex, ToList[i] );

		switch( kind ){
		case PC_KIND_ATTACK:
			if( par ){
				// ⊙煌遥卅凶户｝ㄠㄟㄟ反井仃卅中
				work = CHAR_getWorkInt( toindex, CHAR_WORKFIXSTR );
				work = work * power;
			}else{
				// 公及引引及煌遥卅凶户ㄠㄟㄟ井仃月
				work = power * 100;
			}
			CHAR_setWorkInt( toindex, CHAR_WORKMODATTACK,
				CHAR_getWorkInt( toindex, CHAR_WORKMODATTACK ) + work );
			res = work * 0.01;
			break;
		case PC_KIND_DEFENSE:
			if( par ){
				// ⊙煌遥卅凶户｝ㄠㄟㄟ反井仃卅中
				work = CHAR_getWorkInt( toindex, CHAR_WORKFIXTOUGH );
				work = work * power;
			}else{
				// 公及引引及煌遥卅凶户ㄠㄟㄟ井仃月
				work = power * 100;
			}
			CHAR_setWorkInt( toindex, CHAR_WORKMODDEFENCE,
				CHAR_getWorkInt( toindex, CHAR_WORKMODDEFENCE ) + work );
			res = work * 0.01;
			break;
		case PC_KIND_QUICK:
			if( par ){
				// ⊙煌遥卅凶户｝ㄠㄟㄟ反井仃卅中
				work = CHAR_getWorkInt( toindex, CHAR_WORKFIXDEX );
				work = work * power;
			}else{
				// 公及引引及煌遥卅凶户ㄠㄟㄟ井仃月
				work = power * 100;
			}
			CHAR_setWorkInt( toindex, CHAR_WORKMODQUICK,
				CHAR_getWorkInt( toindex, CHAR_WORKMODQUICK ) + work );
			res = work * 0.01;
			break;
		case PC_KIND_CHARM:
			if( par ){
				// CHARM 反 100  今木化卅中凶户｝100坌及ㄠ允月
				work = CHAR_getWorkInt( toindex, CHAR_WORKFIXCHARM );
				work = work * power * 0.01;
			}else{
				// 公及引引及煌遥
				work = power;
			}
			CHAR_setWorkInt( toindex, CHAR_WORKMODCHARM,
				CHAR_getWorkInt( toindex, CHAR_WORKMODCHARM ) + work );
			res = work;
			break;
		case PC_KIND_CAPTURE:
			// CAPTURE 反 % 及心
			work = power;
			CHAR_setWorkInt( toindex, CHAR_WORKMODCAPTURE,
				CHAR_getWorkInt( toindex, CHAR_WORKMODCAPTURE ) + work );
			res = work;
			break;
		}

		// 仇仇引匹  凶日  内岳  
		//snprintf( szBuffer, sizeof(szBuffer),
		//	"(%s)被(%s)(%+d)",
		//	CHAR_getUseName( toindex ),
		//	aszParamChangeFull[kind],
		//	res
		//);

		//   须  煤
		//BATTLE_BroadCast( battleindex, szBuffer,
		//	(attackNo >= 10)? CHAR_COLORGRAY : CHAR_COLORPURPLE ) ;

	}

	return;
}

#endif


//***********************************************************
//
//    芊羁匹  醒及箪岭  鳖
//
void BATTLE_MultiAttReverse(
	int battleindex, 	// 田玄伙奶件犯永弁旦
	int attackNo,		// 井仃月谛  寞
	int toNo,			// 井仃日木月谛  寞
	int UseEffect,		// 银丹谛及巨白尼弁玄
	int RecevEffect		// 井仃日木月谛及巨白尼弁玄
)
//
//***********************************************************
{

	int i, toindex, charaindex;
	int ToList[SIDE_OFFSET*2+1];
	int flg, OnOff;
	char szBuffer[256]="";

	BATTLE_MultiList( battleindex, toNo, ToList );

	BATTLE_MagicEffect(
		battleindex, 			// 田玄伙奶件犯永弁旦
		attackNo, 				// 井仃月谛  寞(ㄟ  ㄠㄨ)
		ToList, 				// 井仃日木月谛  寞伉旦玄(ㄟ  ㄠㄨ)
		UseEffect, 				// 井仃月谛巨白尼弁玄
		RecevEffect				// 井仃日木月谛巨白尼弁玄
	);

	charaindex = BATTLE_No2Index( battleindex, attackNo );

	// 升及由仿丢□正卞允月井
	for( i = 0; ToList[i] != -1; i ++ ){

		// 锹澎及奶件犯永弁旦
		toindex = BATTLE_No2Index( battleindex, ToList[i] );

		flg = CHAR_getWorkInt( toindex, CHAR_WORKBATTLEFLG );
		flg ^= CHAR_BATTLEFLG_REVERSE;	//   鳖
		CHAR_setWorkInt( toindex, CHAR_WORKBATTLEFLG, flg );
		OnOff = ( flg & CHAR_BATTLEFLG_REVERSE )?(1):(0);

		// 仇仇匹允井今内  鳖今六月
		BATTLE_AttReverse( toindex );

		if( OnOff ){
			//snprintf( szBuffer, sizeof(szBuffer),
			//	"(%s)的属性变成相反。",
			//	CHAR_getUseName( toindex ) );
		}else{
			//snprintf( szBuffer, sizeof(szBuffer),
			//	"(%s)回复成原本的属性。",
			//	CHAR_getUseName( toindex ) );
		}

		//   须  煤
		//BATTLE_BroadCast( battleindex, szBuffer,
		//	(attackNo >= 10)? CHAR_COLORGRAY : CHAR_COLORPURPLE ) ;

		// 仇仇匹箪岭  鳖及  侬  毛霜月
		sprintf( szBuffer, "BR|%X|%X|", ToList[i], OnOff );
		BATTLESTR_ADD( szBuffer );
	}

	return;

}

void BATTLE_MultiCaptureUp(
	int battleindex,
	int attackNo,
	int toNo,
	int power,
	int UseEffect,
	int RecevEffect
)
{
	int i, toindex, UpPoint = 0, workhp;
	int ToList[SIDE_OFFSET*2+1];

	//char szBuffer[256]="";
	char szCommand[256];

	BATTLE_MultiList( battleindex, toNo, ToList );

	BATTLE_MagicEffect(
		battleindex,
		attackNo,
		ToList,
		UseEffect,
		RecevEffect
	);

	for( i = 0; ToList[i] != -1; i ++ ){
		toindex = BATTLE_No2Index( battleindex, ToList[i] );

		if( CHAR_getInt( toindex, CHAR_WHICHTYPE ) != CHAR_TYPEPLAYER ){
			continue;
		}

		if( CHAR_getFlg( toindex, CHAR_ISDIE ) == TRUE ){
			continue;
		}

		UpPoint = power;

		UpPoint = RAND( (power*0.9), (power*1.1) );

		workhp = CHAR_getWorkInt( toindex, CHAR_WORKMODCAPTURE ) + UpPoint;

		CHAR_setWorkInt( toindex, CHAR_WORKMODCAPTURE, workhp );

		//snprintf( szBuffer, sizeof(szBuffer),
		//	"(%s)的捕获率变成(%d)",
		//		CHAR_getUseName( toindex ), UpPoint );

		//BATTLE_BroadCast( battleindex, szBuffer,
		//	(attackNo >= 10)? CHAR_COLORGRAY : CHAR_COLORPURPLE ) ;

		snprintf( szCommand, sizeof( szCommand ),
			"B%%|%X|%X|", ToList[i], BFUKI_CAPTUREUP );

		BATTLESTR_ADD( szCommand );
	}
	return;
}





//****************************************************************
//
// 白奴□伙玉箪岭  凳热诸
//
int	BATTLE_FieldAttChange(
	int charaindex, // 井仃月谛及奶件犯永弁旦
	char *pArg
)
//
// 岳  允木壬 TRUE
// 撩  卅日   FALSE
//
//****************************************************************

{
	
	int attr = -1, i, attackNo, turn = 3;
	int battleindex, power = 30;
	int FieldAttr[] = { BATTLE_ATTR_NONE, BATTLE_ATTR_EARTH, BATTLE_ATTR_WATER, BATTLE_ATTR_FIRE, BATTLE_ATTR_WIND };
	char szCommand[256],
		// Robin 03/20 debug
		//*pszP, *aszAttr[] = { "无", "土", "水", "火", "风" };
		*pszP, *aszAttr[] = { "无", "地", "水", "火", "风" };		
	// Robin 03/21 debug
	//char szTurn[] = "回";	
	char szTurn[] = "turn";


	pszP = pArg;
	// 箪岭毛潸  
	for( ;attr == -1 && pszP[0] != 0; pszP++ ){
		for( i = 0; i < 5; i ++ ){
			// 箪岭疋永正伉井＂
			if( strncmp( pszP, aszAttr[i], 2 ) == 0 ){
				attr = i;
				pszP +=2;
				break;
			}
		}
	}
	// 箪岭  中及匹撩  
	if( attr == -1 ) 
	{
		return FALSE;
	}

	// 升木仁日中  凳今六月井＂
	sscanf( pszP, "%d", &power );
	if( power < 0 || power > 100 )power = 30;

	// 窒正□件  凳今六月井＂
	if( ( pszP = strstr( pszP, szTurn ) ) != NULL){
		pszP += sizeof( szTurn );
		sscanf( pszP, "%d", &turn );
	}

	battleindex = CHAR_getWorkInt( charaindex, CHAR_WORKBATTLEINDEX );
	if( BATTLE_CHECKINDEX( battleindex ) == FALSE )return FALSE;
	//   祭今六月箪岭
	BattleArray[battleindex].field_att = FieldAttr[attr];
	// 膜恳允月由伐□
	BattleArray[battleindex].att_pow = power;
	// 膜恳允月正□件醒
	BattleArray[battleindex].att_count = turn;

	// 仇仇引匹  凶日  内岳  
	if( attr >= 0 ){
		//snprintf( szBuffer, sizeof(szBuffer),
		//	"(%s)将field的属性改为(%s)",
		//	CHAR_getUseName( charaindex ),
		//	aszAttr[attr]
		//);
	}

	attackNo = BATTLE_Index2No( battleindex, charaindex );

	//   须  煤
	//BATTLE_BroadCast( battleindex, szBuffer,
	//	(attackNo >= 10)? CHAR_COLORGRAY : CHAR_COLORPURPLE ) ;

	// 仇仇匹  侬  戊穴件玉毛中木月
	sprintf( szCommand, "BV|%X|%X|",attackNo, attr );
	BATTLESTR_ADD( szCommand );

	return TRUE;

}




//****************************************************************
//
// 白奴□伙玉箪岭  凳热诸
//
int	MAGIC_FieldAttChange_Battle(
	int charaindex, // 井仃月谛及奶件犯永弁旦
	int toNo, 	// 井仃日木月谛及奶件犯永弁旦(  银迕)
	int marray, 	// magicindex
	int mp 			// MP
)
// 岳  允木壬 TRUE
// 撩  卅日   FALSE
//
//****************************************************************

{
	char *pArg;

	// 由仿丢□正毛潸  
	pArg = MAGIC_getChar( marray, MAGIC_OPTION );
	// 由仿丢□正  中及匹撩  
	if( pArg == "\0" )return FALSE;

	return BATTLE_FieldAttChange( charaindex, pArg );

}
//****************************************************************
//
// 旦  □正旦唱橘毛芨尹月热诸
//
int	MAGIC_StatusChange_Battle(
	int charaindex, // 井仃月谛及奶件犯永弁旦
	int toNo, 	// 井仃日木月谛及奶件犯永弁旦
	int marray, 	// magicindex
	int mp 			// MP
)
//  岳  仄凶日 TRUE
//  撩  仄凶日 FALSE
//****************************************************************

{
	char *magicarg;
	int status = -1, i, attackNo, turn = 3;
	int battleindex, ReceveEffect, Success = 15;
	char *pszP;
	char szTurn[] = "turn";
	char szSuccess[] = "成";

	magicarg = MAGIC_getChar( marray, MAGIC_OPTION );
	pszP = magicarg;
	for( ;status == -1 && pszP[0] != 0; pszP++ ){
		for( i = 1; i < BATTLE_ST_END; i ++ ){
			if( strncmp( pszP, aszStatus[i], 2 ) == 0 ){
				status = i;
				pszP +=2;
				break;
			}
		}
	}
	if( status == -1 ) return FALSE;
	if( ( pszP = strstr( pszP, szTurn ) ) != NULL){
		pszP += sizeof( szTurn );
		sscanf( pszP, "%d", &turn );
	}
	if( ( pszP = strstr( pszP, szSuccess ) ) != NULL){
		pszP += sizeof( szSuccess );
		sscanf( pszP, "%d", &Success );
	}
	battleindex = CHAR_getWorkInt( charaindex, CHAR_WORKBATTLEINDEX );
	attackNo = BATTLE_Index2No( battleindex, charaindex );
	if( status == BATTLE_ST_NONE ){
		ReceveEffect = SPR_tyusya;	//   月午五反仇木
	}else{
		ReceveEffect = SPR_hoshi;	// 井井月午五反仇木
	}

	BATTLE_MultiStatusChange( battleindex, attackNo, toNo,
		status, turn, MAGIC_EFFECT_USER, ReceveEffect, Success );


	return TRUE;

}

#ifdef _MAGIC_DEEPPOISON
int	MAGIC_StatusChange_Battle2(
	int charaindex, // 井仃月谛及奶件犯永弁旦
	int toNo, 	// 井仃日木月谛及奶件犯永弁旦
	int marray, 	// magicindex
	int mp 			// MP
)
//  岳  仄凶日 TRUE
//  撩  仄凶日 FALSE
//****************************************************************

{
	char *magicarg;
	int status = -1, i, attackNo, turn = 3;
	int battleindex, ReceveEffect, Success = 15;
	char *pszP;
	char szTurn[] = "turn";
	char szSuccess[] = "成";
	magicarg = MAGIC_getChar( marray, MAGIC_OPTION );
	if( magicarg == "\0" )	{
		print("\n magicarg == NULL ");
		return FALSE;
	}
	pszP = magicarg;
	// 躲绊毛潸  
	for( ;status == -1 && pszP[0] != 0; pszP++ ){
		// ㄠ井日腹绸
		for( i = 1; i < BATTLE_ST_END; i ++ ){
			// 躲绊疋永正伉井＂
			if( strncmp( pszP, aszStatus[i], 2 ) == 0 ){
				status = i;
				pszP +=2;
				break;
			}
		}
	}
	// 躲绊卅中及匹撩  
	if( status == -1 ) return FALSE;

	// 窒正□件  凳今六月井＂
	if( ( pszP = strstr( pszP, szTurn ) ) != NULL){
		pszP += sizeof( szTurn );
		sscanf( pszP, "%d", &turn );
	}

	// 岳    涩烂
	if( ( pszP = strstr( pszP, szSuccess ) ) != NULL){
		pszP += sizeof( szSuccess );
		sscanf( pszP, "%d", &Success );
	}



	// 田玄伙  寞
	battleindex = CHAR_getWorkInt( charaindex, CHAR_WORKBATTLEINDEX );
	attackNo = BATTLE_Index2No( battleindex, charaindex );

	if( status == BATTLE_ST_NONE ){
		ReceveEffect = SPR_tyusya;	//   月午五反仇木
	}else{
		ReceveEffect = SPR_hoshi;	// 井井月午五反仇木
	}
	// 蝈够
	BATTLE_MultiStatusChange( battleindex, attackNo, toNo,
		status, turn+2, MAGIC_EFFECT_USER, ReceveEffect, Success );
	return TRUE;
}
#endif
#ifdef _OTHER_MAGICSTAUTS
int	MAGIC_MagicStatusChange_Battle( int charaindex, int toNo, int marray, int mp )
{
	char *magicarg;
	int status = -1, i, attackNo, turn = 3, nums=0;
	int battleindex, ReceveEffect;
	char buf1[256];

	magicarg = MAGIC_getChar( marray, MAGIC_OPTION );

	if( getStringFromIndexWithDelim( magicarg,"|",1, buf1, sizeof( buf1)) == FALSE )
		return FALSE;
	for( i=0; i<MAXSTATUSTYPE; i++)	{
		if( strcmp( buf1, MagicStatus[i])) continue;
		status = i;
		break;
	}
	if( i == MAXSTATUSTYPE ) return FALSE;
	if( getStringFromIndexWithDelim( magicarg,"|",2, buf1, sizeof( buf1)) == FALSE )
		return FALSE;
	turn = atoi( buf1);
	//CHAR_OTHERSTATUSNUMS
	if( getStringFromIndexWithDelim( magicarg,"|",3, buf1, sizeof( buf1)) == FALSE )
		return FALSE;
	nums = atoi( buf1);
	if( getStringFromIndexWithDelim( magicarg,"|",4, buf1, sizeof( buf1)) == FALSE )
		return FALSE;
	if( strstr( buf1, "单") != 0 )	{
		if( toNo >= 20 )
			return FALSE;
	}
	battleindex = CHAR_getWorkInt( charaindex, CHAR_WORKBATTLEINDEX );
	attackNo = BATTLE_Index2No( battleindex, charaindex );
  //动画图号决定
#define	SPR_mic_def			101411
	ReceveEffect = SPR_heal2;
	if( status == 1 )
		ReceveEffect = SPR_mic_def;
#ifdef _MAGICSTAUTS_RESIST
	else if( status == 3 )
		ReceveEffect = 101802;
	else if( status == 4 )
		ReceveEffect = 101804;	
	else if( status == 5 )
		ReceveEffect = 101803;
#endif
	//else{
	//	ReceveEffect = SPR_hoshi;
	//}

	BATTLE_MultiMagicStatusChange(
		battleindex, attackNo, toNo, status,
		turn, MAGIC_EFFECT_USER, ReceveEffect, nums );

	return TRUE;
}
#endif


int	MAGIC_MagicDef_Battle(
	int charaindex,
	int toNo,
	int marray,
	int mp 
)
{
	char *magicarg;
	int status = -1, i, attackNo, turn = 3;
	int battleindex;
	char *pszP;

	char szTurn[] = "turn";

	magicarg = MAGIC_getChar( marray, MAGIC_OPTION );

	pszP = magicarg;

	for( ;status == -1 && pszP[0] != '\0'; pszP++ ){
		for( i = 1; i < BATTLE_MD_END; i ++ ){
			if( strncmp( pszP, aszMagicDef[i], 2 ) == 0 ){
				status = i;
				pszP +=2;
				break;
			}
		}
	}

	if( status == -1 ) return FALSE;

	if( ( pszP = strstr( pszP, szTurn ) ) != NULL){
		pszP += sizeof( szTurn );
		sscanf( pszP, "%d", &turn );
	}

	battleindex = CHAR_getWorkInt( charaindex, CHAR_WORKBATTLEINDEX );
	attackNo =  BATTLE_Index2No( battleindex, charaindex );

	BATTLE_MultiMagicDef( battleindex, attackNo, toNo,
		status, turn, MAGIC_EFFECT_USER, SPR_difence );


	return TRUE;
}







#if 1

//****************************************************************
//
// 由仿丢□正  祭毛芨尹月热诸
//
int	MAGIC_ParamChange_Battle(
	int charaindex, // 井仃月谛及奶件犯永弁旦
	int toNo, 		// 井仃日木月谛及奶件犯永弁旦
	int marray, 	// magicindex
	int mp 			// MP
)
//  岳  仄凶日 TRUE
//  撩  仄凶日 FALSE
//****************************************************************

{
	char *magicarg;
	int kind = -1, i, attackNo;
	int battleindex, pow, par = 0;

	char *pszP;

	// 由仿丢□正毛潸  //魔法名称
	magicarg = MAGIC_getChar( marray, MAGIC_OPTION );

	pszP = magicarg;
	// 躲绊毛潸  //取得更改参数(无,攻,防,早,魅,捕)
	for( ;kind == -1 && pszP[0] != 0; pszP++ ){
		for( i = 1; i < BATTLE_MD_END; i ++ ){
			// 躲绊疋永正伉井＂
			if( strncmp( pszP, aszParamChange[i], 2 ) == 0 ){
				kind = i;
				pszP +=2;
				break;
			}
		}
	}
	// 躲绊卅中及匹撩  
	if( kind == -1 ) return FALSE;

	if( strstr( pszP, "%" ) ){	// 仇及桦宁反⊙煌遥
		par = 1;
	}

	if( sscanf( pszP, "%d", &pow ) != 1 ){
		// 窒禾奶件玄丐仆月井
		pow = 30;
	}

	// 田玄伙  寞
	battleindex = CHAR_getWorkInt( charaindex, CHAR_WORKBATTLEINDEX );
	attackNo =  BATTLE_Index2No( battleindex, charaindex );

	// 蝈够
	BATTLE_MultiParamChange( battleindex, attackNo, toNo,
		kind, pow, par, MAGIC_EFFECT_USER, SPR_hoshi );


	return TRUE;
}



#endif







//****************************************************************
//
// 蜊谛及箪岭  鳖毛芨尹月热诸
//
int	MAGIC_AttReverse_Battle(
	int charaindex, // 井仃月谛及奶件犯永弁旦
	int toNo, 		// 井仃日木月谛及奶件犯永弁旦
	int marray, 	// magicindex
	int mp 			// MP
)
//  岳  仄凶日 TRUE
//  撩  仄凶日 FALSE
//****************************************************************

{
	int attackNo;
	int battleindex;

	// 由仿丢□正反漆及午仇欠  中
	// 田玄伙  寞
	battleindex = CHAR_getWorkInt( charaindex, CHAR_WORKBATTLEINDEX );
	attackNo =  BATTLE_Index2No( battleindex, charaindex );

	// 蝈够
	BATTLE_MultiAttReverse( battleindex, attackNo, toNo,
		MAGIC_EFFECT_USER, SPR_kyu );


	return TRUE;
}



//****************************************************************
//
// 旦  □正旦唱橘毛荚汊允月热诸
//
int	MAGIC_StatusRecovery_Battle(
	int charaindex, // 井仃月谛及奶件犯永弁旦
	int toNo, 		// 井仃日木月谛及奶件犯永弁旦
	int marray, 	// magicindex
	int mp 			// MP
)
//  岳  仄凶日 TRUE
//  撩  仄凶日 FALSE
//****************************************************************
{
	char *magicarg;
	int status = -1, i, attackNo;
	int battleindex, ReceveEffect;
	char *pszP;

	// 由仿丢□正毛潸  
	magicarg = MAGIC_getChar( marray, MAGIC_OPTION );

	pszP = magicarg;
	// 躲绊毛潸  
	for( ;status == -1 && pszP[0] != 0; pszP++ ){
		// 蝈莒手丐月及匹ㄟ井日腹绸
		for( i = 0; i < BATTLE_ST_END; i ++ ){
			// 躲绊疋永正伉井＂
			if( strncmp( pszP, aszStatus[i], 2 ) == 0 ){
				status = i;
				pszP +=2;
				break;
			}
		}
	}
	// 躲绊卅中及匹撩  
	if( status == -1 ) return FALSE;


	// 田玄伙  寞
	battleindex = CHAR_getWorkInt( charaindex, CHAR_WORKBATTLEINDEX );
	attackNo = BATTLE_Index2No( battleindex, charaindex );

	ReceveEffect = SPR_tyusya;	//   月午五反仇木

	// 蝈够
	BATTLE_MultiStatusRecovery( battleindex, attackNo, toNo,
		status, MAGIC_EFFECT_USER, ReceveEffect );


	return TRUE;

}


//****************************************************************
//
// 竣濮井日汊唾允月热诸
//
int	MAGIC_Ressurect_Battle(
	int charaindex, // 井仃月谛及奶件犯永弁旦
	int toNo, 		// 井仃日木月谛及奶件犯永弁旦
	int marray, 	// magicindex
	int mp 			// MP
)
//  岳  仄凶日 TRUE
//  撩  仄凶日 FALSE
//****************************************************************
{
	char *magicarg;
	int attackNo, ReceveEffect;
	int battleindex, pow = 0, par = 0;

	char *pszP;

	// 由仿丢□正毛潸  
	magicarg = MAGIC_getChar( marray, MAGIC_OPTION );

	pszP = magicarg;

	if( strstr( pszP, "%" ) ){	// 仇及桦宁反⊙煌遥
		par = 1;
	}
	if( sscanf( pszP, "%d", &pow ) != 1 ){
		// 窒禾奶件玄荚汊允月井＂
		pow = 0;	// ㄟ及桦宁反敦蝈荚汊
	}

	if( pow <= 0 ){
		ReceveEffect = SPR_fukkatu3;
	}else
	if( pow <= 100 ){
		ReceveEffect = SPR_fukkatu1;
	}else
	if( pow <= 300 ){
		ReceveEffect = SPR_fukkatu2;
	}else{
		ReceveEffect = SPR_fukkatu3;
	}

	// 田玄伙  寞
	battleindex = CHAR_getWorkInt( charaindex, CHAR_WORKBATTLEINDEX );
	attackNo =  BATTLE_Index2No( battleindex, charaindex );

	// 蝈够
	BATTLE_MultiRessurect( battleindex, attackNo, toNo,
		pow, par, MAGIC_EFFECT_USER, ReceveEffect );

	return TRUE;
}


int	MAGIC_ResAndDef_Battle(
	int charaindex,
	int toNo,
	int marray,
	int mp
)
{
	char *magicarg;
	int attackNo, i;
	int battleindex, pow = 0, par = 0, turn = 3, status =  -1;
	// Robin 03/21 debug
	//char szTurn[] = "回";
	char szTurn[] = "turn";

	char *pszP;

// Terry add 2004/12/7
	// 复活光..不得使用敌方
	battleindex = CHAR_getWorkInt(charaindex, CHAR_WORKBATTLEINDEX);

// end
	// 由仿丢□正毛潸  
	magicarg = MAGIC_getChar( marray, MAGIC_OPTION );

	pszP = magicarg;

	if( strstr( pszP, "%" ) ){	// 仇及桦宁反⊙煌遥
		par = 1;
	}
	if( sscanf( pszP, "%d", &pow ) != 1 ){
		// 窒禾奶件玄荚汊允月井＂
		pow = 0;	// ㄟ及桦宁反敦蝈荚汊
	}

	// 馨笛允月  芊  豢躲绊毛潸  
	for( ;status == -1 && pszP[0] != 0; pszP++ ){
		for( i = 1; i < BATTLE_MD_END; i ++ ){
			char *p;
			// 躲绊疋永正伉井＂
			p = strstr( pszP, aszMagicDef[i] );
			if( p != NULL ){
				status = i;
				pszP = p;
				break;
			}
		}
	}

	// 躲绊卅中及匹撩  
	if( status == -1 ) return FALSE;

	// 窒荚  凳今六月井＂
	if( ( pszP = strstr( pszP, szTurn ) ) != NULL){
		pszP += sizeof( szTurn );
		sscanf( pszP, "%d", &turn );
	}

	// 田玄伙  寞
	battleindex = CHAR_getWorkInt( charaindex, CHAR_WORKBATTLEINDEX );
	attackNo =  BATTLE_Index2No( battleindex, charaindex );

	// 蝈够
	BATTLE_MultiResAndDef( battleindex, attackNo, toNo,
		pow, par, status, turn, MAGIC_EFFECT_USER, SPR_difence );

	return TRUE;
}

#ifdef _ATTACK_MAGIC

int MAGIC_AttMagic_Battle( int charaindex , int toNo , int marray , int mp )
{
	int  attno , attidx , battleindex;
	int  attr = -1, i , power;
	char aszattr[][32] = { "地", "水", "火", "风" };
	char buf1[256];
	char *magicarg;
	int magiclv=0;
	magicarg = MAGIC_getChar( marray , MAGIC_OPTION );
	if( magicarg == "\0" )	{
		print("\n Magic magicarg == NULL ");
		return FALSE;
	}

	for( i=0; i<4; i++)	{
		if( strstr( magicarg, aszattr[i]) != NULL )	{
			attr = i;
			break;
		}
	}
	if( i == 4 )	{
		print("\n not find aszattr !!");
		return FALSE;
	}
	memset( buf1, 0, sizeof( buf1));
	if( getStringFromIndexWithDelim( magicarg, "|", 2, buf1, sizeof( buf1)) == FALSE )	return FALSE;
	power = atoi( buf1);

	if( getStringFromIndexWithDelim( magicarg, "|", 3, buf1, sizeof( buf1)) == FALSE )	return FALSE;
	magiclv = atoi( buf1);

	battleindex   = CHAR_getWorkInt( charaindex , CHAR_WORKBATTLEINDEX );
	attno         = BATTLE_Index2No( battleindex , charaindex );
	attidx        = MAGIC_getInt( marray , MAGIC_IDX );
	if( -1 == attidx || attidx >= ATTMAGIC_magicnum )
		return FALSE;
#ifdef _FIX_MAGICDAMAGE
	BATTLE_MultiAttMagic( battleindex , attno , toNo , attidx , attr , power, magiclv);
#else
	BATTLE_MultiAttMagic( battleindex , attno , toNo , attidx , attr , power );
#endif
  return TRUE;
}

#endif


#ifdef _MAGIC_TOCALL

int MAGIC_ToCallDragon_Battle( int charaindex , int toNo , int marray , int mp )
{
	int  attno , attidx , battleindex;
	int  attr = -1,/* i , */power, imageno;
//	char aszattr[][32] = { "地", "水", "火", "风" };
	char buf1[256];
	char *magicarg;
	//int magiclv=0;
	magicarg = MAGIC_getChar( marray , MAGIC_OPTION );

	if( magicarg == "\0" )	{
		print("\n Magic magicarg == NULL ");
		return FALSE;
	}
/*
	for( i=0; i<4; i++)	{
		if( strstr( magicarg, aszattr[i]) != NULL )	{
			attr = i;
			break;
		}
	}
	if( i == 4 )	{
		print("\n not find aszattr !!");
		return FALSE;
	}
*/
	memset( buf1, 0, sizeof( buf1));
	if( getStringFromIndexWithDelim( magicarg, "|", 1, buf1, sizeof( buf1)) == FALSE )	return FALSE;
	imageno = atoi( buf1);
	if( getStringFromIndexWithDelim( magicarg, "|", 2, buf1, sizeof( buf1)) == FALSE )	return FALSE;
	power = atoi( buf1);

	battleindex   = CHAR_getWorkInt( charaindex , CHAR_WORKBATTLEINDEX );
	attno         = BATTLE_Index2No( battleindex , charaindex );
	attidx        = MAGIC_getInt( marray , MAGIC_IDX );
	if( -1 == attidx || attidx >= ATTMAGIC_magicnum )
		return FALSE;

	BATTLE_MultiToCallDragonMagic( battleindex , attno , toNo , attidx , attr , power, imageno);

  return TRUE;
}

#endif

//****************************************************************
//
//   凯    祭允月热诸
//
int	MAGIC_CaptureUp_Battle(
	int charaindex, // 井仃月谛及奶件犯永弁旦
	int toNo, 		// 井仃日木月谛及奶件犯永弁旦
	int marray, 	// magicindex
	int mp 			// MP
)
//  岳  仄凶日 TRUE
//  撩  仄凶日 FALSE
//****************************************************************
{
	char *magicarg;
	int attackNo, ReceveEffect;
	int battleindex, pow = 5;

	// 由仿丢□正毛潸  
	magicarg = MAGIC_getChar( marray, MAGIC_OPTION );

	if( sscanf( magicarg, "%d", &pow ) != 1 ){
		// 窒禾奶件玄荚汊允月井＂
		pow = 5;
	}

	ReceveEffect = SPR_hoshi;

	// 田玄伙  寞
	battleindex = CHAR_getWorkInt( charaindex, CHAR_WORKBATTLEINDEX );
	attackNo =  BATTLE_Index2No( battleindex, charaindex );

	// 蝈够
	BATTLE_MultiCaptureUp( battleindex, attackNo, toNo,
		pow, MAGIC_EFFECT_USER, ReceveEffect );

	return TRUE;
}
#ifdef _FIX_MAGICDAMAGE
static int BATTLE_getMagicAdjustInt( int attackindex, int defindex, int MagicLv, int flg,int damage )
{
	int At_Fire, At_Earth, At_Water, At_Wind, At_none;
	int Df_Fire, Df_Earth, Df_Water, Df_Wind, Df_none;
	float At_FieldPow, Df_FieldPow;

	//取得施法者属性
	BATTLE_GetAttr(attackindex,&At_Fire,&At_Water,&At_Earth,&At_Wind,&At_none);
	MagicLv *=10;
	switch( flg)	{	//将属性置换为魔法牵引属性
	case 0:
		At_Earth = MagicLv+(MagicLv*(At_Earth/50));
		At_Water=0;	At_Fire=0; At_Wind=0;
		break;
	case 1:
		At_Water = MagicLv+(MagicLv*(At_Water/50));
		At_Earth=0;	At_Fire=0; At_Wind=0;
		break;
	case 2:
		At_Fire = MagicLv+(MagicLv*(At_Fire/50));
		At_Earth=0;	At_Water=0; At_Wind=0;
		break;
	case 3:
		At_Wind = MagicLv+(MagicLv*(At_Wind/50));
		At_Earth=0;	At_Water=0; At_Fire=0;
		break;
	}
	//取得该场属性加成
	At_FieldPow = BATTLE_FieldAttAdjust(
		CHAR_getWorkInt( attackindex, CHAR_WORKBATTLEINDEX ),
		At_Fire, At_Water,
		At_Earth, At_Wind );

	At_Fire *= damage;	At_Water *= damage;	At_Earth *= damage;
	At_Wind *= damage;	At_none *= damage;

	//取得防守方属性
	BATTLE_GetAttr( defindex, &Df_Fire, &Df_Water,
		&Df_Earth,&Df_Wind, &Df_none );
	//取得该场属性加成
	Df_FieldPow = BATTLE_FieldAttAdjust(
		CHAR_getWorkInt( defindex, CHAR_WORKBATTLEINDEX ),
		Df_Fire, Df_Water, Df_Earth, Df_Wind );
	//四属相克
	damage = BATTLE_AttrCalc(
		At_Fire, At_Water, At_Earth, At_Wind, At_none,
		Df_Fire, Df_Water, Df_Earth, Df_Wind, Df_none );
	damage *= (At_FieldPow / Df_FieldPow);
	return damage;
}

void Magic_ComputeAttExp( int charindex, int Mnum, int MagicLv, int Expoint)
{
	int magiclv1, magicex1;
	int addEx = (MagicLv * 3) * Expoint;
	char MagicStr[][128]={"地","水","火","风"};
	char buf1[256];
	memset( buf1, 0, sizeof( buf1));
	magiclv1 = CHAR_getInt( charindex, CHAR_EARTH_EXP+Mnum);
	magicex1 = CHAR_getInt( charindex, CHAR_EARTH_ATTMAGIC_EXP+Mnum);
	magicex1 += addEx;
	if( magicex1 > 100 )	{
		magicex1 = 0;
		if( magiclv1 < 100 )	{
			magiclv1 += 1;
			sprintf( buf1,"%s魔法熟练度提升为%d。", MagicStr[Mnum], magiclv1);
			CHAR_talkToCli( charindex, -1, buf1, CHAR_COLORYELLOW);
		}
	}
	if( magicex1 < 0 ) magicex1 = 0;
	if( magiclv1 < 0 ) magiclv1 = 0;
	if( magiclv1 > 100 ) magiclv1 = 100;
	CHAR_setInt( charindex, CHAR_EARTH_EXP+Mnum, magiclv1);
	CHAR_setInt( charindex, CHAR_EARTH_ATTMAGIC_EXP+Mnum, magicex1);

	Mnum = (Mnum+1)%4;	//相克的魔法
	magiclv1 = CHAR_getInt( charindex, CHAR_EARTH_EXP+Mnum);
	magicex1 = CHAR_getInt( charindex, CHAR_EARTH_ATTMAGIC_EXP+Mnum);
	if( magiclv1 > 1 )	{
		magicex1 -= addEx*0.5;
		if( magicex1 < 0 )	{
			magicex1 = 0;
			magiclv1 -= 1;
			if( magiclv1 < 0 ) magiclv1 = 0;
			CHAR_setInt( charindex, CHAR_EARTH_EXP+Mnum, magiclv1);
			sprintf( buf1,"%s魔法熟练度下降为%d。", MagicStr[Mnum], magiclv1);
			CHAR_talkToCli( charindex, -1, buf1, CHAR_COLORYELLOW);
		}
		CHAR_setInt( charindex, CHAR_EARTH_ATTMAGIC_EXP+Mnum, magicex1);
	}
	return;
}

void Magic_ComputeDefExp( int charindex, int Mnum, int MagicLv, int Damage)
{
	int magiclv1;
	int magicex1;
	int addEx;
	char buf1[256];
	char MagicStr[][128]={"地","水","火","风"};
	if( Damage < 200 )	return;
	memset( buf1, 0, sizeof( buf1));
	magiclv1 = CHAR_getInt( charindex, CHAR_EARTH_RESIST+Mnum);
	magicex1 = CHAR_getInt( charindex, CHAR_EARTH_DEFMAGIC_EXP+Mnum);
	addEx = (Damage/20)*(MagicLv*2);
	magicex1+=addEx;
	if( magiclv1 < 0 ) magiclv1 = 0;
	if( magicex1 > 100 )	{
		magicex1 = 0;
		if( magiclv1 < 100 )	{
			magiclv1 +=1;
			if( magiclv1 < 0 ) magiclv1 = 0;
			if( magiclv1 > 100 ) magiclv1 = 100;
			CHAR_setInt( charindex, CHAR_EARTH_RESIST+Mnum, magiclv1);
			sprintf( buf1,"%s抗魔值提升为%d。", MagicStr[Mnum], magiclv1);
			CHAR_talkToCli( charindex, -1, buf1, CHAR_COLORYELLOW);
		}
	}
	if( magicex1 < 0 ) magicex1 = 0;
	CHAR_setInt( charindex, CHAR_EARTH_DEFMAGIC_EXP+Mnum, magicex1);
	Mnum = (Mnum+1)%4;	//相克的魔法
	magiclv1 = CHAR_getInt( charindex, CHAR_EARTH_RESIST+Mnum);
	magicex1 = CHAR_getInt( charindex, CHAR_EARTH_DEFMAGIC_EXP+Mnum);
	if( magiclv1 > 1 )	{
		magicex1 -=2;
		if( magicex1 < 0 )	{
			magicex1 = 90;
			magiclv1 -=1;
			if( magiclv1 < 0 )
				magiclv1 = 0;
			sprintf( buf1,"%s抗魔值下降为%d。", MagicStr[Mnum], magiclv1);
			CHAR_talkToCli( charindex, -1, buf1, CHAR_COLORYELLOW);
			CHAR_setInt( charindex, CHAR_EARTH_RESIST+Mnum, magiclv1);
		}
		CHAR_setInt( charindex, CHAR_EARTH_DEFMAGIC_EXP+Mnum, magicex1);
	}
	return;
}

#endif

#ifdef _MAGIC_WEAKEN
int	MAGIC_ParamChange_Turn_Battle(
	int charaindex, 
	int toNo, 		
	int marray, 	
	int mp 			
)
{
	char *magicarg;
	int status = -1, i, attackNo, turn = 3;
	int battleindex, Success = 15;
	char *pszP;
	char szTurn[] = "turn";
	char szSuccess[] = "成";
	magicarg = MAGIC_getChar( marray, MAGIC_OPTION );
	if( magicarg == "\0" )	{
		print("\n magicarg == NULL ");
		return FALSE;
	}
	pszP = magicarg;
	// 躲绊毛潸  
	for( ;status == -1 && pszP[0] != 0; pszP++ ){
		// ㄠ井日腹绸
		for( i = 1; i < BATTLE_ST_END; i ++ ){
			// 躲绊疋永正伉井＂
			if( strncmp( pszP, aszStatus[i], 2 ) == 0 ){
				status = i;
				pszP +=2;
				break;
			}
		}
	}

	if( status == -1 ) return FALSE;

	if( ( pszP = strstr( pszP, szTurn ) ) != NULL){
		pszP += sizeof( szTurn );
		sscanf( pszP, "%d", &turn );
	}

	if( ( pszP = strstr( pszP, szSuccess ) ) != NULL){
		pszP += sizeof( szSuccess );
		sscanf( pszP, "%d", &Success );
	}
	battleindex = CHAR_getWorkInt( charaindex, CHAR_WORKBATTLEINDEX );
	attackNo =  BATTLE_Index2No( battleindex, charaindex );
	BATTLE_MultiParamChangeTurn( battleindex, attackNo, toNo, status, MAGIC_EFFECT_USER, SPR_hoshi, turn, Success );
	return TRUE;
}

void BATTLE_MultiParamChangeTurn(
	int battleindex, 	
	int attackNo,		
	int toNo,
	int status,
	int UseEffect,		
	int RecevEffect,		
	int turn,
	int Success
)
{

	int i, toindex,perStatus,charaindex;
	int ToList[SIDE_OFFSET*2+1];
    charaindex = BATTLE_No2Index( battleindex, attackNo );	//攻方index
	BATTLE_MultiList( battleindex, toNo, ToList );
	
	//施魔法动画效果
	BATTLE_MagicEffect(battleindex, attackNo, ToList, UseEffect, RecevEffect);

	for( i = 0; ToList[i] != -1; i ++ )
	{
		toindex = BATTLE_No2Index( battleindex, ToList[i] );

		if( BATTLE_StatusAttackCheck( charaindex,
			toindex, status, Success, 30, 1.0, &perStatus ) == TRUE )
		{
			CHAR_setWorkInt( toindex, CHAR_WORKWEAKEN, turn+1 );
		}
	}
	return;

}
#endif


/*
typedef struct tagToCallMagic
{
  unsigned int  uiSpriteNum;			// 此咒术在Spr_x.bin的编号
  unsigned int  uiAttackType;			// 攻击的方式：单人，整排( 轮流 ) ,  整排( 轮流 ) , 整排( 同时 ) , 全体( 轮流 ) , 全体( 同时 )
  unsigned int  uiSliceTime;			// 轮流攻击时的时间差
  unsigned int  uiShowType;             // 显示的位置方式：中央、指定
  int           siSx;                   // 显示的位置 - X轴
  int           siSy;                   // 显示的位置 - Y轴
  unsigned int  uiShowBehindChar;       // 显示在人物的前方或下方
  unsigned int  uiShakeScreen;          // 是否震动画面
  unsigned int  uiShakeFrom;            // 震动画面的起始时间( 毫秒 )
  unsigned int  uiShakeTo;              // 震动画面的结束时间( 毫秒 _
  unsigned int  uiPrevMagicNum;         // 前置咒术的索引号( 0XFFFFFFFFFF 表示无前置咒术 )
  int           siPrevMagicSx;          // 前置咒术的显示位置 - X轴
  int           siPrevMagicSy;          // 前置咒术的显示位置 - Y轴
  int           siPrevMagicOnChar;      // 前置咒术显示在人物的前方或下方
  unsigned int  uiPostMagicNum;         // 後置咒术的索引号( 0XFFFFFFFF 表示无後置咒术 )
  int           siPostMagicSx;          // 後置咒术的显示位置 - X轴
  int           siPostMagicSy;          // 後置咒术的显示位置 - Y轴
  int           siPostMagicOnChar;      // 後置咒术显示在人物的前方或下方
  int			isPostDisappear;		// 咒术一般攻击完时是否马上消失
  int			ToCallMagicNo;			// 召唤术的编号
}ToCallMagic;
*/
  
ToCallMagic PROFESSION_magic[3] = { {0,0,0,1,0,0,1,1,0,0,0,65528,65485,0,-1,0,0,0,1,0},	// 单人
									{0,3,0,1,0,0,1,1,0,0,0,65528,65485,0,-1,0,0,0,1,1},	// 单排
									{0,5,0,1,0,0,1,1,0,0,0,65528,65485,0,-1,0,0,0,0,2},	// 全体
								};


#ifdef _PETSKILL_FIREKILL
/*
     . ..·. ..··. .○
     *. ..· ★.* ·. .
   ★  .  ·● ● ·. .★. 
     ..· /█\_/█\_ ·. 
       ??√√ √√??? 
        中秋节快乐 
*/
void BATTLE_MultiAttMagic_Fire( int battleindex, int attackNo, int defNo,
						  int FieldAttr , int Power)

{
    int  list[SIDE_OFFSET * 2 + 1];
    int  listidx=0,i,j,z,def_be_hit[10],att_magic_lv[4];
	int  def_magic_resist[DEF_MAGIC_NUM];
    int  attvalue , charahp , pethp , charaidx , petidx , charahurt,pet_att_lv = 1,pet_def_lv = 1,charahurt_temp;
    char szcommand[256];
	int  AttIsPlayer=0,DefIsPlayer=0;
	int getexp = 0;

#ifndef _FIX_MAGICDAMAGE
	int att_magic_exp_add,att_magic_exp_sub,def_magic_exp_add,def_magic_exp_sub;
	int lv_up_exp,DefFieldAttr = 0;
	float temp = 0.0f;
//	int attattr[5], defattr[5];
	char msgbuf[64];
	char kind[4][3] = {"地","水","火","风"};
#else
	BOOL TrueMagic=FALSE;
#endif
	int f_num=0;
	int MagicLv=4;
	
	memset(def_be_hit,-1,sizeof(def_be_hit));

	if(defNo < 5)
		f_num = 0;
	else if(defNo >=5 && defNo < 10)
		f_num = 5;
	else if(defNo >=10 && defNo < 15)
		f_num = 10;
	else 
		f_num = 15;

	// 取出战场上存活的人
	for( i=f_num; i<f_num+5; i++ ){			
		if( BATTLE_TargetCheck(battleindex, i) != FALSE ){
		    list[listidx]	= i;
			listidx++;
		}
	}
	
    sprintf( szcommand, "n%X|", listidx ); //被打的人数
	BATTLESTR_ADD( szcommand );			

		// 取得人物的属性(不论玩家或是宠物)
		//BATTLE_GetAttr( BATTLE_No2Index( battleindex , attackNo ) , &attattr[2] , &attattr[1] , &attattr[0] , &attattr[3] , &attattr[4] );
		// 如果攻击者是玩家,取得人物的魔法熟练度及魔法抗性
		{
			int attType = CHAR_getInt(BATTLE_No2Index(battleindex,attackNo),CHAR_WHICHTYPE);
			int Check=0;
			AttIsPlayer=0;
			if( attType == CHAR_TYPEPLAYER)	{
				AttIsPlayer = 1;
				for(i=0;i<4;i++){	// att_magic_lv[i]: i = 0:地 1:水 2:火 3:风
					att_magic_lv[i] = CHAR_getInt(BATTLE_No2Index(battleindex,attackNo),CHAR_EARTH_EXP+i);
				}
			}else if( attType == CHAR_TYPEENEMY )	{
				for(i=0;i<4;i++){
					att_magic_lv[i] = (CHAR_getInt(BATTLE_No2Index(battleindex,attackNo), CHAR_LV)*0.9);
				}
			}else	{ //pet
                for(i=0;i<4;i++){
					att_magic_lv[i] = 5;
				}

			}
			Check = rand()%100;
#ifdef _FIX_MAGICDAMAGE
			if( Check > att_magic_lv[ FieldAttr] ) TrueMagic = FALSE;
			else TrueMagic = TRUE;
#endif
		}
		pet_att_lv = CHAR_getInt(BATTLE_No2Index(battleindex,attackNo), CHAR_LV);

		z = 0;
    for(i=0;i<listidx;i++){
charaidx = BATTLE_No2Index( battleindex , list[i] );
      petidx   = BATTLE_getRidePet( charaidx );
      //BATTLE_GetAttr( charaidx , &defattr[2] , &defattr[1] , &defattr[0] , &defattr[3] , &defattr[4] );
		{//andy_fix
			int defType = CHAR_getInt(charaidx,CHAR_WHICHTYPE);
			DefIsPlayer = 0;
			if( defType == CHAR_TYPEPLAYER)	{
				DefIsPlayer = 1;
				for(j=0;j<DEF_MAGIC_NUM;j++){	// def_magic_resist[i]: i = 0:地 1:水 2:火 3:风
					def_magic_resist[j] = CHAR_getInt(charaidx,CHAR_EARTH_RESIST+j);
#ifdef _EQUIT_DEFMAGIC
					def_magic_resist[j] += CHAR_getWorkInt( charaidx, CHAR_EQUITDEFMAGIC_E+j);
#endif
				}
			}else if( defType == CHAR_TYPEENEMY )	{
				for(j=0;j<DEF_MAGIC_NUM;j++){
					def_magic_resist[j] = (CHAR_getInt(charaidx,CHAR_LV)*0.5);
				}
			}else	{	//PET
#ifdef _FIX_MAGICDAMAGE
				DefIsPlayer = 1;
#endif
				for(j=0;j<DEF_MAGIC_NUM;j++){
#ifdef _FIX_MAGICDAMAGE
					def_magic_resist[j] = CHAR_getInt(charaidx,CHAR_EARTH_RESIST+j);
					//def_magic_resist[j] += CHAR_getWorkInt( charaidx, CHAR_EQUITDEFMAGIC_E+j);
#else
					def_magic_resist[j] = 0;
#endif
				}
			}

#ifdef _MAGIC_DEFMAGICATT
			if( CHAR_getWorkInt( charaidx, CHAR_DEFMAGICSTATUS ) > 0 ){
				float def = (float)(CHAR_getWorkInt( charaidx, CHAR_OTHERSTATUSNUMS ));
				def = def/100;
				for(j=0;j<DEF_MAGIC_NUM;j++){
					if( def_magic_resist[j] <= 0 ) continue;
					def_magic_resist[j] += def_magic_resist[j] * def;
				}
			}
#endif

		}
		pet_def_lv = CHAR_getInt(charaidx,CHAR_LV);
			if(BATTLE_MagicDodge(charaidx,DefIsPlayer,FieldAttr)){	// 计算闪避率
				attvalue = pethp = 0;
				sprintf( szcommand, 
			    "r%X|f%X|d%X|p%X|",
			    list[i],
			    0,
			    attvalue,
			    pethp );
				BATTLESTR_ADD( szcommand );
				continue;
			}else{	// 没闪过,计算攻击力
#ifdef _FIX_MAGICDAMAGE
				float Kmagic = ((float)att_magic_lv[FieldAttr]*1.4 - (float)def_magic_resist[FieldAttr]);
				float Mmagic = (float)att_magic_lv[FieldAttr];
				float Amagic=0.00;
				int APower=0;
				if( Kmagic < 0 ) Kmagic = 0;
				if( Mmagic < 1 ) Mmagic = 1;
				Amagic = (Kmagic*Kmagic) / (Mmagic*Mmagic);
				Amagic += ((float)(rand()%20))/100;
				APower = (int)(Power*(1+(float)MagicLv/10) * Amagic);	//MagicLv

				//andy_log
/*				print("att_magic_lv:%d",att_magic_lv[FieldAttr]);
				print( "Player:%s\n", CHAR_getUseName( charaidx));
				print( "ANDY Magic Power:%d Amagic:%f APower:%d \n", Power, Amagic, APower);
*/               
				def_be_hit[getexp++] = charaidx;	// 记下被打的人的index 给後面 "被攻击者中了睡眠"那一段使用

				attvalue = BATTLE_getMagicAdjustInt( BATTLE_No2Index(battleindex,attackNo), charaidx,
					                                     MagicLv, FieldAttr, APower);
				//andy_log
//				print("attvalue:%d \n", attvalue);
				//if( TrueMagic == FALSE )//使用失败
				//	attvalue*=0.7;
				
				if( DefIsPlayer )	{
					Magic_ComputeDefExp( charaidx, FieldAttr, MagicLv, attvalue);
				}
				
#else
				def_be_hit[getexp++] = charaidx;	// 记下被打的人的index

				// 计算属性相克
				attvalue = BATTLE_AttrAdjust(BATTLE_No2Index(battleindex,attackNo),charaidx,Power);
				if(DefIsPlayer){	// 被打的是玩家
					if(AttIsPlayer){	// 如果是玩家用魔法攻击
						temp = ((float)att_magic_lv[FieldAttr] - (float)def_magic_resist[FieldAttr])/(float)def_magic_resist[FieldAttr] / 100;
						attvalue = Power + Power * temp + attvalue;
					}else{
						temp = ((float)pet_att_lv - (float)def_magic_resist[FieldAttr]) / (float)def_magic_resist[FieldAttr] / 100;
						attvalue = Power + Power * temp + attvalue;
					}
				}else{// 被打的是宠物
					if(AttIsPlayer){// 如果是玩家用魔法攻击
						temp = ((float)att_magic_lv[FieldAttr] - (float)pet_def_lv) / (float)pet_def_lv / 100;
						attvalue = Power + Power * temp + attvalue;
					}else	{
						temp = ((float)pet_att_lv - (float)pet_def_lv) / (float)pet_def_lv / 100;
						attvalue = Power + Power * temp + attvalue;
					}
				}
				// 上下取10
				attvalue += rand()%2 ? (rand()%10)+1 : -(rand()%10)-1;
				if( attvalue <= 0 ) attvalue = 1;
#endif
			}

      charahp = CHAR_getInt( charaidx , CHAR_HP );
       // 没有宠物
       if( -1 == petidx || CHAR_getInt( petidx , CHAR_HP ) <= 0 ){
		   if( ( charahp -= attvalue ) < 0 ){
             charahp = 0;
		   }
          CHAR_setInt( charaidx , CHAR_HP , charahp );
          pethp = 0;

          sprintf( szcommand, "r%X|f%X|d%X|p%X|", list[i], 0, attvalue, pethp );

       }else{
          pethp     = CHAR_getInt( petidx , CHAR_HP ); 
          charahurt = attvalue * BATTLE_CalcCharaRatio( FieldAttr , charaidx ) / 10;

		  charahurt_temp = charahurt;
          if( ( charahp -= charahurt ) < 0 )
          {
            charahurt = charahp;
            charahp   = 0;
			//ANDY_ADD
			//CHAR_setFlg( charaidx, CHAR_ISDIE, TRUE);
          }
					
          attvalue = attvalue - charahurt;

          if( ( pethp -= attvalue ) < 0 )
          {
            pethp = 0;
						// 宠物没血了退出战场
						CHAR_setInt(charaidx,CHAR_RIDEPET,-1);
						BATTLE_changeRideImage(charaidx);
						CHAR_setWorkInt(charaidx,CHAR_WORKPETFALL,1);
          }
          CHAR_setInt( charaidx , CHAR_HP , charahp );
          CHAR_setInt( petidx , CHAR_HP , pethp );

					charahurt = charahurt_temp;

          sprintf( szcommand, 
			    "r%X|f%X|d%X|p%X|",
			    list[i],
			    0,
			    charahurt,
			    attvalue );

       }
       BATTLESTR_ADD( szcommand );
    }

	if( (TrueMagic == FALSE) && AttIsPlayer)	{	//计算攻击方经验值
		Magic_ComputeAttExp( BATTLE_No2Index(battleindex,attackNo), FieldAttr, MagicLv, getexp);
	}
	for(i=0; i<listidx; i++)	{
		// 如果被攻击者中了睡眠
		if(def_be_hit[i] < 0) continue;
		if(CHAR_getWorkInt(def_be_hit[i],CHAR_WORKSLEEP) > 0) {
			CHAR_setWorkInt(def_be_hit[i],CHAR_WORKSLEEP,0);
			z = BATTLE_Index2No(battleindex,def_be_hit[i]);
			sprintf(szcommand,"BM|%X|%X|",z,0);
			BATTLESTR_ADD(szcommand);
		}
	}
}
#endif