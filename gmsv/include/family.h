#ifndef __FAMILY_H__
#define __FAMILY_H__

#include "version.h"
#include "common.h"
#include "util.h"
#include "net.h"
#include "time.h"

#define FAMILY_MAXNUM			1000	// 家族数量
#ifdef _FMVER21
#define FAMILY_MAXMEMBER		200	// 家族人数
#define FAMILY_MAXCHANNELMEMBER		50	// 频道人数
#else
#define FAMILY_MAXMEMBER		50	// 家族人数
#define FAMILY_MAXCHANNELMEMBER		10	// 频道人数
#endif
#define FAMILY_MAXCHANNEL 		5	// 家族频道

#define CHAR_MAXNAME			32
#define CHAR_MAXID			20
#define MINFMLEVLEFORPOINT      	3       // 3 申请庄园最低等级
#define FMLEADERLV			30	// 族长等级

#ifdef _FAMILY_MANORNUM_CHANGE
#define FAMILY_FMPKFLOOR		15	// 家族ＰＫ图层
#else
#define FAMILY_FMPKFLOOR		9	// 家族ＰＫ图层
#define FMPOINTNUM			4       // 有据点家族的最大数量
#define MANORNUM                	4
#define FAMILY_MAXHOME			4	// 家族据点
#endif

enum
{
    FM_TOP_INTEGRATE = 1,    // DPTOP 综合
    FM_TOP_ADV,              // DPTOP 冒险
    FM_TOP_FEED,             // DPTOP 饲育
    FM_TOP_SYNTHESIZE,       // DPTOP 合成
    FM_TOP_DEALFOOD,         // DPTOP 料理
    FM_TOP_PK,               // DPTOP ＰＫ
    FM_TOP_NUM,              // DPTOP 数量
};

enum
{
	FM_FIX_ACCEPTFLAG = 1,
	FM_FIX_FMPK,
	FM_FIX_FMPET,
	FM_FIX_FMRULE,
	FM_FIX_DELFMTIME,
	FM_FIX_FMGOLD,
	FM_FIX_FMADV,
	FM_FIX_FMFEED,
	FM_FIX_FMSYNTHESIZE,
	FM_FIX_FMDEALFOOD,
	FM_FIX_FMLEADERCHANGE,
};

/*
 * 扔□田    及职及桦赭午及 cdkey charname 及赢今毛宁六月凶户卞
 * CHEKEYLEN, CHARNAMELEN
 * 毛银丹方丹卞  凳［
 */
    
void CHAR_Family(int fd, int index, char* message);
void ACAddFM(int fd, int result, int fmindex, int index);
void ACJoinFM(int fd, int result, int recv);
void ACLeaveFM(int fd, int result, int resultflag);
void ACDelFM(int fd, int result);
void ACShowFMList(int ret, int fmnum, char *data);
void ACFMDetail(int ret, char *data, int charfdid);
void ACShowMemberList(int result, int index, int fmnumm,
int fmacceptflag, int fmjoinnum, char *data);
void ACShowDpTop(int result,int num, char *data, int kindflag);
void ACShowPointList(int result, char *data);
void ACShowFMMemo(int result, int index, int num, int dataindex, char *data);

#ifdef _PERSONAL_FAME   // Arminius: 家族个人声望
void ACFMCharLogin(int fd, int ret, int index, int floor, int fmdp,
	int joinflag, int fmsetupflag, int flag, int charindex, int charfame
	);
#else
void ACFMCharLogin(int fd, int ret, int index, int floor, int fmdp,
	int joinflag, int fmsetupflag, int flag, int charindex);
#endif

void ACFMPointList(int ret, char *data);
void ACSetFMPoint(int ret, int r, int clifd);
void ACFMAnnounce(int ret, char *fmname, int fmindex, int index, int kindflag,
	char *data, int color);
void ACFixFMPK(int winindex, int loseindex, int data);
void ACFMJob( int fd, int ret, char* data1, char* data2 );

void FAMILY_Add(int fd, int meindex, char *message);
void FAMILY_Join(int fd, int meindex, char *message);
void FAMILY_Leave(int fd, int meindex, char *message);
void FAMILY_Detail(int fd, int meindex, char *message);
void FAMILY_CheckMember(int fd, int meindex, char *message);
void FAMILY_Channel(int fd, int meindex, char *message);
void FAMILY_Bank(int fd, int meindex, char *message);
void FAMILY_SetPoint(int fd, int meindex, char *message);
void FAMILY_Init(void);
void FAMILY_SetAcceptFlag(int fd, int meindex, char *message);
void FAMILY_FixRule( int fd, int meindex, char* message );
void FAMILY_RidePet( int fd, int meindex, char* message );
void FAMILY_LeaderFunc( int fd, int meindex, char* message );

#ifdef _ADD_FAMILY_TAX			   // WON ADD 增加庄园税收	
void GS_ASK_TAX(void);
void FAMILY_FIX_TAX( int fd, int index, char* message);
#endif

void SetFMPetVarInit(int meindex);
void SetFMVarInit(int meindex);


int CheckFMLeader(int meindex);
int getFmLv(int playerindex);

void getNewFMList( void );
void checkFamilyIndex( void );

int CheckLeaderQ(int meindex);


// shan add Begin
struct FMMEMBER_LIST
{
//  int  fmindex;
    int  fmnum; 
    int  fmjoinnum;
//  BOOL use;                                       // 0->没使用   1->使用
    int  memberindex[FAMILY_MAXMEMBER];             
    char numberlistarray[FAMILY_MAXMEMBER][64];
    char memo[35][220];                             // family dengon
    int  accept;                                    // 召募成员与否
    int  memonum;
    int  memoindex;
};
// 家族之间的留言板
struct FMS_MEMO
{
    char memo[140][220];
    int  memonum;
    int  memoindex;
};
// 家族强者表
struct FMS_DPTOP
{
    int  num;															// 记录有多少个家族(综合)
    char topmemo[FAMILY_MAXNUM][128];
    int  fmtopid[FAMILY_MAXNUM];          // 家族索引
#ifdef _FMVER21    
    int  fmtopdp[FAMILY_MAXNUM];          // 家族综合声望
#endif    
    int  adv_num;                // 冒险
    char adv_topmemo[30][96];    
    int  feed_num;               // 伺育
    char feed_topmemo[30][96];        
    int  syn_num;                // 合成
    char syn_topmemo[30][96];        
    int  food_num;               // 料理
    char food_topmemo[30][96];        
    int  pk_num;                 // ＰＫ
    char pk_topmemo[30][96];        
};
// 家族据点
struct FM_POINTLIST
{
  char pointlistarray[FAMILY_MAXHOME][1024];	// Arminius: 32->1024
};
// 家族PK图层
struct FM_PKFLOOR
{
    int fl;
};
// End

#define MAXFAMILYLIST 64*1024
extern char    familyListBuf[MAXFAMILYLIST];

void JoinMemberIndex( int charaindex, int fmindexi);


#endif
