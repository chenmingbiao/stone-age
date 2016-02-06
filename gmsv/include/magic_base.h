#ifndef __MAGIC_BASE_H__
#define __MAGIC_BASE_H__

#include "util.h"

typedef enum
{
	MAGIC_FIELD_ALL,			/* 允屯化及桦赭匹银尹月 */
	MAGIC_FIELD_BATTLE,				/* 爵    及心 */
	MAGIC_FIELD_MAP,				/* 骚橘穴永皿晓及心 */

}MAGIC_FIELDTYPE;

typedef enum
{
	MAGIC_TARGET_MYSELF,		/* 愤坌及心 */
	MAGIC_TARGET_OTHER,			/* 职及谛  愤坌殖戈) */
	MAGIC_TARGET_ALLMYSIDE,		/*     蝈   */
	MAGIC_TARGET_ALLOTHERSIDE,	/* 锹澎础蝈   */
	MAGIC_TARGET_ALL,			/* 蝈化 */
	MAGIC_TARGET_NONE,			/* 簿手蓟  请  卅中［  豢支凶户及凛 */
	MAGIC_TARGET_OTHERWITHOUTMYSELF,/* 职及谛  愤坌殖引卅中) */
	MAGIC_TARGET_WITHOUTMYSELFANDPET,  /* 愤坌午矢永玄动陆 */
	MAGIC_TARGET_WHOLEOTHERSIDE,/*     及扔奶玉蝈   */

#ifdef _ATTACK_MAGIC

        MAGIC_TARGET_SINGLE,            // 针对敌方的某一人
        MAGIC_TARGET_ONE_ROW,           // 针对敌方的某一列
        MAGIC_TARGET_ALL_ROWS,          // 针对敌方的所有人

#endif
}MAGIC_TARGETTYPE;

typedef enum
{
	MAGIC_ID,					/* 栋    寞 */
	MAGIC_FIELD,				/* 银尹月桦赭 */
	MAGIC_TARGET,				/* 覆擂 */
	MAGIC_TARGET_DEADFLG,		/* 韶氏分樊手覆擂卞殖户月井 */
#ifdef _ATTACK_MAGIC
  MAGIC_IDX ,
#endif
	MAGIC_DATAINTNUM,
}MAGIC_DATAINT;

typedef enum
{
	MAGIC_NAME,					/* 热诸   */
	MAGIC_COMMENT,				/* 戊丢件玄*/
	MAGIC_FUNCNAME,				/* 楮醒   */
	MAGIC_OPTION,				/* 左皿扑亦件 */
	MAGIC_DATACHARNUM,
}MAGIC_DATACHAR;

typedef struct tagMagic
{
	int			data[MAGIC_DATAINTNUM];
	STRING64	string[MAGIC_DATACHARNUM];

}Magic;

#ifdef _ATTACK_MAGIC

typedef struct tagAttMagic
{
  unsigned int  uiSpriteNum;// 此咒术在Spr_x.bin的编号
  unsigned int  uiAttackType;// 攻击的方式：单人，整排( 轮流 ) ,  整排( 轮流 ) , 整排( 同时 ) , 全体( 轮流 ) , 全体( 同时 )
  unsigned int  uiSliceTime;// 轮流攻击时的时间差
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
  int           siField[3][5];          // 攻击索引
}AttMagic;

#endif

#ifdef _MAGIC_TOCALL

typedef struct tagToCallMagic
{
  unsigned int  uiSpriteNum;// 此咒术在Spr_x.bin的编号
  unsigned int  uiAttackType;// 攻击的方式：单人，整排( 轮流 ) ,  整排( 轮流 ) , 整排( 同时 ) , 全体( 轮流 ) , 全体( 同时 )
  unsigned int  uiSliceTime;// 轮流攻击时的时间差
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

#endif

typedef int (*MAGIC_CALLFUNC)( int, int, int, int );

INLINE BOOL MAGIC_CHECKINDEX( int index );
INLINE int MAGIC_getInt( int index, MAGIC_DATAINT element);
INLINE int MAGIC_setInt( int index, MAGIC_DATAINT element, int data);
INLINE char* MAGIC_getChar( int index, MAGIC_DATACHAR element);
INLINE BOOL MAGIC_setChar( int index ,MAGIC_DATACHAR element, char* new );
int MAGIC_getMagicNum( void);
BOOL MAGIC_initMagic( char *filename);
BOOL MAGIC_reinitMagic( void );

#ifdef _ATTACK_MAGIC

BOOL ATTMAGIC_initMagic( char *filename );
BOOL ATTMAGIC_reinitMagic( void );

#endif

int MAGIC_getMagicArray( int magicid);
MAGIC_CALLFUNC MAGIC_getMagicFuncPointer(char* name);
// Nuke +1 08/23 : For checking the validity of magic target
int MAGIC_isTargetValid( int magicid, int toindex);

#endif

