#define _FAMILY_C_

#include "version.h"
#include "main.h"
#include "acfamily.h"
#include "util.h"
#include "saacproto_serv.h"

#include <malloc.h>
#include <stdio.h>
#include <time.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>

// 家族 Int 资料
static char* FAMILY_INTDATA[familymaxint] = 
{
  "fmindex",
  "fmleadergrano",
  "fmnum",
  "fmjoinnum",
  "fmacceptflag",
  "fmsetupflag",
  "fmsprite",
  "fmpointindex",
  "fmpopular",
  "fmgold",
  "fmmemberindexmaxnum",
  "apply_time",
  "setup_time",
  "predel_time",
  "memonowwritenum",
  "memonum",
	"fmadv",
  "fmfeed",
  "fmsynthesize",
  "fmdealfood",
  "fmpk",
#ifdef _NEW_MANOR_LAW
	"fmmomentum",
#endif
};

// 家族 char 资料
static char* FAMILY_CHARDATA[familymaxchar] = 
{
  "fmname",
  "fmleadername",
  "fmleaderid",
  "petname",
  "petattr",
  "fmrule",
};

// 家族成员 int 资料
static char* MEMBER_INTDATA[memberdatamaxint] = 
{
	"charlv",
	"charflag",
	"onlineflag",
	"charfdid",
	"predeltime",
	"popular",
#ifdef _FMVER21
	"eventflag",
#endif
#ifdef _NEW_MANOR_LAW
	"momentum",
#endif
};

// 家族成员 char 资料
static char* MEMBER_CHARDATA[memberdatamaxchar] = 
{
	"charname",
	"charid",
};

// 成员种类 char 资料
static char* MEMBERKIND_INTDATA[FMMEMBER_KINDNUM] = 
{
	"无",
	"一般成员",
	"申请中",
	"族长",
	"长老",
};

// 家族之间留言板
struct FMSMEMO
{
   int 		num;					// 留言数量
   int		fmsnowwritenum;				// 目前留言位置
   char		memodata[FMSMEMONUM][FAMILY_MEMOLEN];	// 留言内容
};

// 庄园
struct FMPOINT
{
   int		fl;							// 庄园进入图层
   int		x;							// 庄园进入Ｘ座标
   int		y;							// 庄园进入Ｙ座标
   int		fmfl;						// 庄园族长图层
   int 		village;					// 庄园邻近之村庄
   int		hadfmindex;					// 占领庄园之家族 index
   char		hadfmname[CHARNAME_MAX];	// 占领庄园之家族 name
   int		hadfmpopular;				// 占领庄园之家族综合声望值
};

// 家族成员资料
typedef struct
{
   char		charname[CHARNAME_MAX];	// 成员名字
   char		charid[USERID_MAX];	// 成员帐号
   int		charlv;			// 成员等级
   int 		charflag;		/* 
   					   FMMEMBER_NONE   清空资料
   					   FMMEMBER_MEMBER 家族成员
   					   FMMEMBER_APPLY  申请中
   					   FMMEMBER_LEADER 族长
   					   FMMEMBER_ELDER  长老
                                        */
   int		onlineflag;		// 0:offline; gmsv index
   int		charfdid;		// 成员在 gmsv 的 fd
   int		predeltime;		// 预计删除成员时间
   int 		popular;		// 成员的声望
#ifdef _FMVER21
   int		eventflag;		// 是否拥有族长资格
#endif
#ifdef _NEW_MANOR_LAW
	 int		momentum;			// 成员气势
#endif
}  MEMBERDATA;

struct	FAMILY
{
   int 		fmindex;			// 家族 index
   char		fmname[CHARNAME_MAX];		// 家族名称
   char		fmleadername[CHARNAME_MAX];	// 族长名称

   char		fmleaderid[USERID_MAX];		// 族长帐号
   int		fmleadergrano;			// 族长照片
   char		petname[CHARNAME_MAX];		// 守护兽名称
   char		petattr[256];			// 守护兽资料

   int		fmnum; 			// 家族人数，包含申请退出、加入人数
   int		fmjoinnum; 		// 真正加入家族人数
   int		fmacceptflag; 		// 是否接收成员 0:不接受 1:接受
   int		fmsetupflag; 		// 0:尚未成立 1:已成立
   char		fmrule[256];		// 家族主旨
   int		fmsprite; 		// 0:光明精灵 1:黑暗精灵
   int		fmpointindex;		// 家族据点 index
   int 		fmpopular;		// 家族综合声望
#ifdef _PERSONAL_FAME	// Arminius: 家族个人声望
   int		fmtotalfame;		// 家族综合 + 家族个人声望总合
#endif
#ifdef _NEW_MANOR_LAW
	 int		fmmomentum;			// 家族气势
#endif
   int		fmgold;			// 家族基金
   int		fmmemberindexmaxnum;	// 家族目前最大人数
   int		apply_time;		// 家族申请时间
   int		setup_time;		// 家族成立时间
   int		predel_time;		// 预计删除家族时间

   int		memonowwritenum;	// 目前留言位置
   int 		memonum;		// 留言数量
   int		fmadv;			// 冒险任务声望
   int		fmfeed;			// 饲育声望
   int		fmsynthesize;		// 合成加工声望
   int		fmdealfood;		// 料理声望
   int		fmpk;			// ＰＫ声望
   char		memolist[FAMILY_MEMONUM][FAMILY_MEMOLEN];	// 留言内容
   MEMBERDATA	fmmemberindex[MAX_MEMBERNUM];			// 成员资料
};

int db_familyupdate[MAX_FAMILY]={0};
int db_fmpointupdate = 0;
int db_fmsmemoupdate = 0;
int fmnownum = 0;
int fmindexmaxnum = 0;
extern gmsv gs[MAXCONNECTION];

struct FAMILY family[MAX_FAMILY];
int fmpopularindex[MAX_FAMILY];	// Arminius: sort family
#ifdef _PERSONAL_FAME	// Arminius: 家族个人声望
int fmtotalfameindex[MAX_FAMILY];
#endif
#ifdef _NEW_MANOR_LAW
int fmMomentumIndex[MAX_FAMILY];
#endif
int fmadvindex[MAX_FAMILY];
int fmfeedindex[MAX_FAMILY];
int fmsynthesizeindex[MAX_FAMILY];
int fmdealfoodindex[MAX_FAMILY];
int fmpkindex[MAX_FAMILY];
struct FMPOINT fmpoint[MAX_FMPOINT];
struct FMSMEMO	fmsmemo;

// Arminius: sort family & output the sorted list
// Make a sorted index (fmindex) for the family data (family)
#ifdef _NEW_MANOR_LAW
int fmMomentumcompar(const void *indexa, const void *indexb);
#endif
#ifdef _PERSONAL_FAME	// Arminius: 家族个人声望
int fmtotalfamecompar(const void *indexa, const void *indexb);
#endif
int fmpopularcompar(const void *indexa, const void *indexb);
int fmadvcompar(const void *indexa, const void *indexb);
int fmfeedcompar(const void *indexa, const void *indexb);
int fmsynthesizecompar(const void *indexa, const void *indexb);
int fmdealfoodcompar(const void *indexa, const void *indexb);
int fmpkcompar(const void *indexa, const void *indexb);
int CheckFM(int *index, char *fmname, int fmindex);
int CheckFMUse(int index);
void setFamilyFileDataToArg(int index, char *data);
void setMemberFileDataToArg(int index, int memberindex, char *databuf);

// sortFamily: call this after family loaded
void sortFamily(void)
{
  int i;

  for(i = 0; i < MAX_FAMILY; i++)
  {
#ifdef _PERSONAL_FAME	// Arminius: 家族个人声望
	fmtotalfameindex[i] = i;
#endif
#ifdef _NEW_MANOR_LAW
	fmMomentumIndex[i] = i;
#endif
  	fmpopularindex[i] = i;
  	fmadvindex[i] = i;
  	fmfeedindex[i] = i;
  	fmsynthesizeindex[i] = i;
  	fmdealfoodindex[i] = i;
  	fmpkindex[i] = i;
  }
#ifdef _PERSONAL_FAME	// Arminius: 家族个人声望
  qsort(fmtotalfameindex, MAX_FAMILY, sizeof(int), &fmtotalfamecompar);
#endif
  qsort(fmpopularindex, MAX_FAMILY, sizeof(int), &fmpopularcompar);
  qsort(fmadvindex, MAX_FAMILY, sizeof(int), &fmadvcompar);
  qsort(fmfeedindex, MAX_FAMILY, sizeof(int), &fmfeedcompar);
  qsort(fmsynthesizeindex, MAX_FAMILY, sizeof(int), &fmsynthesizecompar);
  qsort(fmdealfoodindex, MAX_FAMILY, sizeof(int), &fmdealfoodcompar);
  qsort(fmpkindex, MAX_FAMILY, sizeof(int), &fmpkcompar);
#ifdef _NEW_MANOR_LAW
	qsort(fmMomentumIndex,MAX_FAMILY,sizeof(int),&fmMomentumcompar);
#endif
}

#ifdef _PERSONAL_FAME	// Arminius: 家族个人声望
int fmtotalfamecompar(const void *indexa, const void *indexb)
{
  int famea,fameb;
  
  if ( CheckFMUse(* (int*) indexa)==0 ) return 1;	// empty record
  if ( CheckFMUse(* (int*) indexb)==0 ) return -1;

  famea = family[ * (int*) indexa ].fmtotalfame;
  fameb = family[ * (int*) indexb ].fmtotalfame;
  return (famea>fameb)? -1 : 1;
}
#endif

#ifdef _NEW_MANOR_LAW
int fmMomentumcompar(const void *indexa, const void *indexb)
{
  int momentumA,momentumB;
  
  if(CheckFMUse(*(int*)indexa) == 0) return 1;	// empty record
  if(CheckFMUse(*(int*)indexb) == 0) return -1;

  momentumA = family[*(int*)indexa].fmmomentum;
  momentumB = family[*(int*)indexb].fmmomentum;
  return (momentumA > momentumB) ? -1:1;
}
#endif

int fmpopularcompar(const void *indexa, const void *indexb)
{
  int fmpopulara,fmpopularb;
  
  if ( CheckFMUse(* (int*) indexa)==0 ) return 1;	// empty record
  if ( CheckFMUse(* (int*) indexb)==0 ) return -1;

  fmpopulara = family[ * (int*) indexa ].fmpopular;
  fmpopularb = family[ * (int*) indexb ].fmpopular;
  return (fmpopulara>fmpopularb)? -1 : 1;
}

int fmadvcompar(const void *indexa, const void *indexb)
{
  int fmadva,fmadvb;
  
  if ( CheckFMUse(* (int*) indexa)==0 ) return 1;	// empty record
  if ( CheckFMUse(* (int*) indexb)==0 ) return -1;

  fmadva = family[ * (int*) indexa ].fmadv;
  fmadvb = family[ * (int*) indexb ].fmadv;
  return (fmadva>fmadvb)? -1 : 1;
}

int fmfeedcompar(const void *indexa, const void *indexb)
{
  int fmfeeda,fmfeedb;
  
  if ( CheckFMUse(* (int*) indexa)==0 ) return 1;	// empty record
  if ( CheckFMUse(* (int*) indexb)==0 ) return -1;

  fmfeeda = family[ * (int*) indexa ].fmfeed;
  fmfeedb = family[ * (int*) indexb ].fmfeed;
  return (fmfeeda>fmfeedb)? -1 : 1;
}

int fmsynthesizecompar(const void *indexa, const void *indexb)
{
  int fmsynthesizea,fmsynthesizeb;
  
  if ( CheckFMUse(* (int*) indexa)==0 ) return 1;	// empty record
  if ( CheckFMUse(* (int*) indexb)==0 ) return -1;

  fmsynthesizea = family[ * (int*) indexa ].fmsynthesize;
  fmsynthesizeb = family[ * (int*) indexb ].fmsynthesize;
  return (fmsynthesizea>fmsynthesizeb)? -1 : 1;
}

int fmdealfoodcompar(const void *indexa, const void *indexb)
{
  int fmdealfooda,fmdealfoodb;
  
  if ( CheckFMUse(* (int*) indexa)==0 ) return 1;	// empty record
  if ( CheckFMUse(* (int*) indexb)==0 ) return -1;

  fmdealfooda = family[ * (int*) indexa ].fmdealfood;
  fmdealfoodb = family[ * (int*) indexb ].fmdealfood;
  return (fmdealfooda>fmdealfoodb)? -1 : 1;
}

int fmpkcompar(const void *indexa, const void *indexb)
{
  int fmpka,fmpkb;
  
  if ( CheckFMUse(* (int*) indexa)==0 ) return 1;	// empty record
  if ( CheckFMUse(* (int*) indexb)==0 ) return -1;

  fmpka = family[ * (int*) indexa ].fmpk;
  fmpkb = family[ * (int*) indexb ].fmpk;
  return (fmpka>fmpkb)? -1 : 1;
}

// getFMsortedlist
// arg: buf=returned list (format: "index 名次 家族名 族长名 popular|...")
//      bufsize=sizeof(buf)
//      bp=begin point (bp=-1 -> begin at 0)
//      ep=end point   (ep=-1 -> end at MAX_FAMILY-1)
// ret: 1=success; 0=failed
// Arminius end

// 取得家族各声望值列表
// kindflag 1:综合 2:冒险 3:饲育 4:合成 5:料理 6:PK 7:总合(综合+个人)
int ACShowTopFMList(char *data, int datasize, int kindflag)
{
   int i = 0, j = 0;
   char t1[4096];
   
   strcpy(data, "");
   if (kindflag == 1)
   {
      for (i = 0; i < MAX_FAMILY; i++) {
         if (strcmp(family[fmpopularindex[i]].fmname, "") == 0)
         {
         	j++;
         	continue;
         }
#ifdef _PERSONAL_FAME   // Arminius: 家族个人声望
	#ifdef _NEW_MANOR_LAW
				 sprintf(t1, "%d|%d|%s|%s|%d|%d|%d",fmtotalfameindex[i], i + 1,
         	family[fmtotalfameindex[i]].fmname,
         	family[fmtotalfameindex[i]].fmleadername,
      		(family[fmtotalfameindex[i]].fmpopular / 100),
      		(family[fmtotalfameindex[i]].fmtotalfame / 100),
					(family[fmtotalfameindex[i]].fmmomentum));
	#else
         sprintf(t1, "%d|%d|%s|%s|%d|%d",fmtotalfameindex[i], i + 1,
         	family[fmtotalfameindex[i]].fmname,
         	family[fmtotalfameindex[i]].fmleadername,
      		(family[fmtotalfameindex[i]].fmpopular / 100),
      		(family[fmtotalfameindex[i]].fmtotalfame / 100));
	#endif
#else
         sprintf(t1, "%d|%d|%s|%s|%d", fmpopularindex[i], i + 1,
         	family[fmpopularindex[i]].fmname,
         	family[fmpopularindex[i]].fmleadername,
      		(family[fmpopularindex[i]].fmpopular / 100));
#endif
      	 if (i < MAX_FAMILY) strcat(t1, " ");
      	 if (strlen(data) + strlen(t1) >= datasize){
      	    log("getFMsortedlist: datafer overflow.");
      	    return	-1;
      	 }
      	 strcat(data, t1);
      }
   }
   else if (kindflag == 2)
   {
      for (i = 0; i < TOPFMLISTNUM; i++) {
         if (strcmp(family[fmadvindex[i]].fmname, "") == 0)
         {
         	j++;
        	continue;
         }
         sprintf(t1, "%d|%d|%s|%s|%d", fmadvindex[i], i + 1,
         	family[fmadvindex[i]].fmname,
         	family[fmadvindex[i]].fmleadername,
      		(family[fmadvindex[i]].fmadv / 100));
      	 if (i < TOPFMLISTNUM) strcat(t1, " ");
      	 if (strlen(data) + strlen(t1) >= datasize)
      	 {
      	    log("getFMsortedlist: datafer overflow.");
      	    return	-1;
      	 }
      	 strcat(data, t1);
      }
   }
   else if (kindflag == 3)
   {
      for (i = 0; i < TOPFMLISTNUM; i++) {
         if (strcmp(family[fmfeedindex[i]].fmname, "") == 0)
         {
         	j++;
         	continue;
         }
         sprintf(t1, "%d|%d|%s|%s|%d", fmfeedindex[i], i + 1,
         	family[fmfeedindex[i]].fmname,
         	family[fmfeedindex[i]].fmleadername,
      		(family[fmfeedindex[i]].fmfeed / 100));
      	 if (i < TOPFMLISTNUM) strcat(t1, " ");
      	 if (strlen(data) + strlen(t1) >= datasize)
      	 {
      	    log("getFMsortedlist: datafer overflow.");
      	    return	-1;
      	 }
      	 strcat(data, t1);
      }
   }
   else if (kindflag == 4)
   {
      for (i = 0; i < TOPFMLISTNUM; i++) {
         if (strcmp(family[fmsynthesizeindex[i]].fmname, "") == 0)
         {
         	j++;
         	continue;
         }
         sprintf(t1, "%d|%d|%s|%s|%d", fmsynthesizeindex[i], i + 1,
         	family[fmsynthesizeindex[i]].fmname,
         	family[fmsynthesizeindex[i]].fmleadername,
      		(family[fmsynthesizeindex[i]].fmsynthesize / 100));
      	 if (i < TOPFMLISTNUM) strcat(t1, " ");
      	 if (strlen(data) + strlen(t1) >= datasize)
      	 {
      	    log("getFMsortedlist: datafer overflow.");
      	    return	-1;
      	 }
      	 strcat(data, t1);
      }
   }
   else if (kindflag == 5)
   {
      for (i = 0; i < TOPFMLISTNUM; i++) {
         if (strcmp(family[fmdealfoodindex[i]].fmname, "") == 0)
         {
         	j++;
         	continue;
         }
         sprintf(t1, "%d|%d|%s|%s|%d", fmdealfoodindex[i], i + 1,
         	family[fmdealfoodindex[i]].fmname,
         	family[fmdealfoodindex[i]].fmleadername,
      		(family[fmdealfoodindex[i]].fmdealfood / 100));
      	 if (i < TOPFMLISTNUM) strcat(t1, " ");
      	 if (strlen(data) + strlen(t1) >= datasize)
      	 {
      	    log("getFMsortedlist: datafer overflow.");
      	    return	-1;
      	 }
      	 strcat(data, t1);
      }
   }
   else if (kindflag == 6)
   {
      for (i = 0; i < TOPFMLISTNUM; i++) {
         if (strcmp(family[fmpkindex[i]].fmname, "") == 0)
         {
         	j++;
         	continue;
         }
         sprintf(t1, "%d|%d|%s|%s|%d", fmpkindex[i], i + 1,
         	family[fmpkindex[i]].fmname,
         	family[fmpkindex[i]].fmleadername,
      		(family[fmpkindex[i]].fmpk / 100));
      	 if (i < TOPFMLISTNUM) strcat(t1, " ");
      	 if (strlen(data) + strlen(t1) >= datasize)
      	 {
      	    log("getFMsortedlist: datafer overflow.");
      	    return	-1;
      	 }
      	 strcat(data, t1);
      }
   }
#ifdef _PERSONAL_FAME	// Arminius: 家族个人声望
   else if (kindflag == 7)
   {
      for (i = 0; i < MAX_FAMILY; i++) {
         if (strcmp(family[fmtotalfameindex[i]].fmname, "") == 0)
         {
         	j++;
         	continue;
         }
         sprintf(t1, "%d|%d|%s|%s|%d", fmtotalfameindex[i], i + 1,
         	family[fmtotalfameindex[i]].fmname,
         	family[fmtotalfameindex[i]].fmleadername,
      		(family[fmtotalfameindex[i]].fmtotalfame / 100));
      	 if (i < MAX_FAMILY) strcat(t1, " ");
      	 if (strlen(data) + strlen(t1) >= datasize)
      	 {
      	    log("getFMsortedlist: datafer overflow.");
      	    return	-1;
      	 }
      	 strcat(data, t1);
      }
   }
#endif
#ifdef _NEW_MANOR_LAW
	 else if(kindflag == 8){ // 十大气势家族(全部气势排名都传给game server)
		 for(i=0;i<MAX_FAMILY;i++){
			 if(strcmp(family[fmMomentumIndex[i]].fmname, "") == 0){
				 j++;
				 continue;
			 }
			 sprintf(t1,"%d|%d|%s|%s|%d",fmMomentumIndex[i], i + 1,
				 family[fmMomentumIndex[i]].fmname,
				 family[fmMomentumIndex[i]].fmleadername,
					 (family[fmMomentumIndex[i]].fmmomentum)/100);
			 if(i < MAX_FAMILY) strcat(t1," ");
			 if(strlen(data) + strlen(t1) >= datasize){
				 log("getFMsortedlist: datafer overflow.");
				 return	-1;
			 }
			 strcat(data, t1);
		 }
	 }
#endif
   return i - j;
}

void AddFMMaintainSort(int index)
{
   int i = 0, j = MAX_FAMILY - 1, k;
   // modify popular sort
   while ((family[fmpopularindex[i]].fmpopular >= family[index].fmpopular)
   	&& (i < MAX_FAMILY) && CheckFMUse(fmpopularindex[i]))
   {
   		if (i == MAX_FAMILY -1)	break;
   		i++;
   }
   while ((fmpopularindex[j] != index) && (j > 0)) j--;
   if (j > i)
   {
   	for (k = j; k > i; k--) fmpopularindex[k] = fmpopularindex[k - 1];
   	fmpopularindex[i] = index;
   } else if (j < i) {
   	log("ACAddFM: fmpopularindex add error (j < i)");
   } else log("ACAddFM: fmpopularindex add error (j = i)");
   // modify adv sort
   i = 0;j = MAX_FAMILY;k = 0;
   while ((family[fmadvindex[i]].fmadv >= family[index].fmadv)
   	&& (i < MAX_FAMILY) && CheckFMUse(fmadvindex[i]))
   {
   		if (i == MAX_FAMILY -1)	break;
   		i++;
   }
   while ((fmadvindex[j] != index) && (j > 0)) j--;
   if (j > i)
   {
   	for (k = j; k > i; k--) fmadvindex[k] = fmadvindex[k - 1];
   	fmadvindex[i] = index;
   } else if (j < i) {
   	log("ACAddFM: fmadvindex add error (j < i)");
   } else log("ACAddFM: fmadvindex add error (j = i)");
   // modify feed sort
   i = 0; j = MAX_FAMILY; k = 0;
   while ((family[fmfeedindex[i]].fmfeed >= family[index].fmfeed)
   	&& (i < MAX_FAMILY) && CheckFMUse(fmfeedindex[i]))
   {
   		if (i == MAX_FAMILY -1)	break;
   		i++;
   }
   while ((fmfeedindex[j] != index) && (j > 0)) j--;
   if (j > i)
   {
   	for (k = j; k > i; k--) fmfeedindex[k] = fmfeedindex[k - 1];
   	fmfeedindex[i] = index;
   } else if (j < i) {
   	log("ACAddFM: fmfeedindex add error (j < i)");
   } else log("ACAddFM: fmfeedindex add error (j = i)");
   // modify synthesize sort
   i = 0;j = MAX_FAMILY;k = 0;
   while ((family[fmsynthesizeindex[i]].fmsynthesize >= family[index].fmsynthesize)
   	&& (i < MAX_FAMILY) && CheckFMUse(fmsynthesizeindex[i]))
   {
   		if (i == MAX_FAMILY -1)	break;
   		i++;
   }
   while ((fmsynthesizeindex[j] != index) && (j > 0)) j--;
   if (j > i)
   {
   	for (k = j; k > i; k--) fmsynthesizeindex[k] = fmsynthesizeindex[k - 1];
   	fmsynthesizeindex[i] = index;
   } else if (j < i) {
   	log("ACAddFM: fmsynthesizeindex add error (j < i)");
   } else log("ACAddFM: fmsynthesizeindex add error (j = i)");
   // modify dealfood sort
   i = 0;j = MAX_FAMILY;k = 0;
   while ((family[fmdealfoodindex[i]].fmdealfood >= family[index].fmdealfood)
   	&& (i < MAX_FAMILY) && CheckFMUse(fmdealfoodindex[i]))
   {
   		if (i == MAX_FAMILY -1)	break;
   		i++;
   }
   while ((fmdealfoodindex[j] != index) && (j > 0)) j--;
   if (j > i)
   {
   	for (k = j; k > i; k--) fmdealfoodindex[k] = fmdealfoodindex[k - 1];
   	fmdealfoodindex[i] = index;
   } else if (j < i) {
   	log("ACAddFM: fmdealfoodindex add error (j < i)");
   } else log("ACAddFM: fmdealfoodindex add error (j = i)");
   // modify pk sort
   i = 0;j = MAX_FAMILY;k = 0;
   while ((family[fmpkindex[i]].fmpk >= family[index].fmpk)
   	&& (i < MAX_FAMILY) && CheckFMUse(fmpkindex[i]))
   {
   		if (i == MAX_FAMILY -1)	break;
   		i++;
   }
   while ((fmpkindex[j] != index) && (j > 0)) j--;
   if (j > i)
   {
   	for (k = j; k > i; k--) fmpkindex[k] = fmpkindex[k - 1];
   	fmpkindex[i] = index;
   } else if (j < i) {
   	log("ACAddFM: fmpkindex add error (j < i)");
   } else log("ACAddFM: fmpkindex add error (j = i)");
#ifdef _PERSONAL_FAME	// Arminius: 家族个人声望
   // modify pk sort
   i = 0;j = MAX_FAMILY;k = 0;
   while ((family[fmtotalfameindex[i]].fmtotalfame >= family[index].fmtotalfame)
   	&& (i < MAX_FAMILY) && CheckFMUse(fmtotalfameindex[i]))
   {
   		if (i == MAX_FAMILY -1)	break;
   		i++;
   }
   while ((fmtotalfameindex[j] != index) && (j > 0)) j--;
   if (j > i)
   {
   	for (k = j; k > i; k--) fmtotalfameindex[k] = fmtotalfameindex[k - 1];
   	fmtotalfameindex[i] = index;
   } else if (j < i) {
   	log("ACAddFM: fmtotalfameindex add error (j < i)");
   } else log("ACAddFM: fmtotalfameindex add error (j = i)");
#endif
#ifdef _NEW_MANOR_LAW
   // modify momentum sort
   i = 0;j = MAX_FAMILY;k = 0;
   while ((family[fmMomentumIndex[i]].fmmomentum >= family[index].fmmomentum)
   	&& (i < MAX_FAMILY) && CheckFMUse(fmMomentumIndex[i]))
   {
   		if (i == MAX_FAMILY -1)	break;
   		i++;
   }
   while ((fmMomentumIndex[j] != index) && (j > 0)) j--;
   if (j > i)
   {
   	for (k = j; k > i; k--) fmMomentumIndex[k] = fmMomentumIndex[k - 1];
   	fmMomentumIndex[i] = index;
   } else if (j < i) {
   	log("ACAddFM: fmMomentumIndex add error (j < i)");
   } else log("ACAddFM: fmMomentumIndex add error (j = i)");
#endif
}

void DelFMMaintainSort(int index)
{
     int i = 0,j;
     // modify popular sort
     while ((fmpopularindex[i] != index) && (i < MAX_FAMILY)) i++;
     if (fmpopularindex[i] == index)
     {
        for (j = i; j < MAX_FAMILY - 1; j++) 
        	fmpopularindex[j] = fmpopularindex[j + 1];
        fmpopularindex[MAX_FAMILY - 1] = index;	// move to last one
     }
     // modify adv sort
     i = 0;j = 0;
     while ((fmadvindex[i] != index) && (i < MAX_FAMILY)) i++;
     if (fmadvindex[i] == index)
     {
        for (j = i; j < MAX_FAMILY - 1; j++) 
        	fmadvindex[j] = fmadvindex[j + 1];
        fmadvindex[MAX_FAMILY - 1] = index;	// move to last one
     }
     // modify feed sort
     i = 0;j = 0;
     while ((fmfeedindex[i] != index) && (i < MAX_FAMILY)) i++;
     if (fmfeedindex[i] == index)
     {
        for (j = i; j < MAX_FAMILY - 1; j++) 
        	fmfeedindex[j] = fmfeedindex[j + 1];
        fmfeedindex[MAX_FAMILY - 1] = index;	// move to last one
     }
     // modify synthesize sort
     i = 0;j = 0;
     while ((fmsynthesizeindex[i] != index) && (i < MAX_FAMILY)) i++;
     if (fmsynthesizeindex[i] == index)
     {
        for (j = i; j < MAX_FAMILY - 1; j++) 
        	fmsynthesizeindex[j] = fmsynthesizeindex[j + 1];
        fmsynthesizeindex[MAX_FAMILY - 1] = index;	// move to last one
     }
     // modify dealfood sort
     i = 0;j = 0;
     while ((fmdealfoodindex[i] != index) && (i < MAX_FAMILY)) i++;
     if (fmdealfoodindex[i] == index)
     {
        for (j = i; j < MAX_FAMILY - 1; j++) 
        	fmdealfoodindex[j] = fmdealfoodindex[j + 1];
        fmdealfoodindex[MAX_FAMILY - 1] = index;	// move to last one
     }
     // modify pk sort
     i = 0;j = 0;
     while ((fmpkindex[i] != index) && (i < MAX_FAMILY)) i++;
     if (fmpkindex[i] == index)
     {
        for (j = i; j < MAX_FAMILY - 1; j++) 
        	fmpkindex[j] = fmpkindex[j + 1];
        fmpkindex[MAX_FAMILY - 1] = index;	// move to last one
     }
#ifdef _PERSONAL_FAME	// Arminius: 家族个人声望
     // modify pk sort
     i = 0;j = 0;
     while ((fmtotalfameindex[i] != index) && (i < MAX_FAMILY)) i++;
     if (fmtotalfameindex[i] == index)
     {
        for (j = i; j < MAX_FAMILY - 1; j++) 
        	fmtotalfameindex[j] = fmtotalfameindex[j + 1];
        fmtotalfameindex[MAX_FAMILY - 1] = index;	// move to last one
     }
#endif
#ifdef _NEW_MANOR_LAW
		 // modify momentum sort
     i = 0;j = 0;
     while ((fmMomentumIndex[i] != index) && (i < MAX_FAMILY)) i++;
     if (fmMomentumIndex[i] == index){
        for (j = i; j < MAX_FAMILY - 1; j++) 
        	fmMomentumIndex[j] = fmMomentumIndex[j + 1];
        fmMomentumIndex[MAX_FAMILY - 1] = index;	// move to last one
     }
#endif
}

// 从档案读取家族资料（启动时读取）
int readFamily(char *dir)
{
	char dirname[256];
	DIR  *d;
	struct dirent *de;
	int i = 0, fmnum = 0, fmmaxnum = 0;
	{
		char tmp[256];
		snprintf(tmp, sizeof(tmp), "%s", dir);
		if (mkdir(tmp, 0755) == 0)
			log("create %s\n", tmp);
	}

	snprintf(dirname, sizeof(dirname), "%s", dir);
	d = opendir(dirname);
	if (d == NULL){
		log("cannot open %s\n", dirname);
			return -1;
	}
	de = readdir(d);
	if(de == NULL){
		log("de == NULL\n");
		return	-1;
	}
{
	char filename[256];
	struct stat s;
	for (i = 0; i < MAX_FAMILY; i++){
//		FILE *fp;
		snprintf(filename, sizeof(filename), "%s/Family.%d", dirname, i);
		if (stat(filename, &s) < 0)continue;

		if (!(s.st_mode & S_IFREG)){
			log("%s is not a regular file\n", filename);
			continue;
		}
		if( readOneFamily( filename, i) != 1 ){
			if (remove(filename) == -1)   
		 	 log("cannot open file %s %s\n", filename, strerror(errno));
			continue;
		}
   	   	fmnum = fmnum + 1;
   	   	fmmaxnum = i;
	}

   	fmindexmaxnum = fmmaxnum;
   	fmnownum = fmnum;
}
   closedir(d);
   
#ifdef _PERSONAL_FAME	// Arminius: 家族个人声望
   // 计算家族总合声望 = 家族综合声望 + 家族个人声望
   {
     int i,j;
     for (i = 0; i < MAX_FAMILY; i++) {
       family[i].fmpopular=family[i].fmadv+family[i].fmfeed
       	 +family[i].fmsynthesize+family[i].fmdealfood
				 +family[i].fmpk;
       family[i].fmtotalfame=family[i].fmpopular;
       for (j = 0; j < MAX_MEMBERNUM; j++) {
         if ((family[i].fmmemberindex[j].charflag!=FMMEMBER_NONE) &&
             (family[i].fmmemberindex[j].charflag!=FMMEMBER_APPLY) )
           family[i].fmtotalfame += family[i].fmmemberindex[j].popular;
       }
     }
   }
#endif
   
   sortFamily();	// Arminius: sort it
   return 0;
}

// 将家族资料写入档案
int writeFamily(char *dir)
{
   int i = 0, j = 0, k = 0;
   FILE *fp;
   char filename[256];
   for (i = 0; i < MAX_FAMILY; i++)
   {
      if (db_familyupdate[i] == 0)	continue;
      sprintf(filename, "%s/Family.%d", dir, i);
      fp = fopen(filename, "w");
      if (fp == NULL)	continue;
      if ((family[i].fmindex != -1) && (strcmp(family[i].fmname, "") != 0))
      {
   	 fprintf(fp, "fmindex=%d|fmname=%s|fmleadername=%s|fmleaderid=%s|"
	    "fmleadergrano=%d|petname=%s|petattr=%s|fmnum=%d|fmjoinnum=%d|"
	    "fmacceptflag=%d|fmsetupflag=%d|fmrule=%s|fmsprite=%d|"
	    "fmpointindex=%d|fmpopular=%d|fmgold=%d|fmmemberindexmaxnum=%d|"
	    "apply_time=%d|setup_time=%d|predel_time=%d|memonowwritenum=%d|"
	    "memonum=%d|fmadv=%d|fmfeed=%d|fmsynthesize=%d|fmdealfood=%d|fmpk=%d"
#ifdef _NEW_MANOR_LAW
			"|fmmomentum=%d"
#endif
   	    ,family[i].fmindex, family[i].fmname, family[i].fmleadername, 
   	    family[i].fmleaderid, family[i].fmleadergrano, family[i].petname,
   	    family[i].petattr, family[i].fmnum, family[i].fmjoinnum,
   	    family[i].fmacceptflag, family[i].fmsetupflag, family[i].fmrule,
   	    family[i].fmsprite, family[i].fmpointindex, family[i].fmpopular,
   	    family[i].fmgold, family[i].fmmemberindexmaxnum,
   	    family[i].apply_time, family[i].setup_time, family[i].predel_time,
   	    family[i].memonowwritenum, family[i].memonum, family[i].fmadv,
   	    family[i].fmfeed, family[i].fmsynthesize, family[i].fmdealfood,
   	    family[i].fmpk
#ifdef _NEW_MANOR_LAW
				,family[i].fmmomentum
#endif
				);
   	 for (j = 0; j < family[i].memonum; j++)
   	    fprintf(fp, "|%s%d=%s", FMMEMOLIST, j, family[i].memolist[j]);
   	 for (k = 0; k < family[i].fmmemberindexmaxnum; k++)
   	 {
	    if ((strcmp(family[i].fmmemberindex[k].charname, "") != 0)
	    	&& (strcmp(family[i].fmmemberindex[k].charid, "") != 0)
	    	&& (family[i].fmmemberindex[k].charflag != FMMEMBER_NONE))
	    {
#ifdef _FMVER21
	#ifndef _NEW_MANOR_LAW
   	       fprintf(fp, "|%s%d=charname=%s charid=%s charlv=%d charflag=%d onlineflag=%d predeltime=%d popular=%d eventflag=%d",
	#else
					 fprintf(fp, "|%s%d=charname=%s charid=%s charlv=%d charflag=%d onlineflag=%d predeltime=%d popular=%d eventflag=%d momentum=%d",
	#endif
#else
					 fprintf(fp, "|%s%d=charname=%s charid=%s charlv=%d charflag=%d onlineflag=%d predeltime=%d popular=%d",
#endif
   	       	  FMMEMBERINDEX, k, family[i].fmmemberindex[k].charname,
   	          family[i].fmmemberindex[k].charid,
   	          family[i].fmmemberindex[k].charlv,
   	          family[i].fmmemberindex[k].charflag,
   	          family[i].fmmemberindex[k].onlineflag,
   	          family[i].fmmemberindex[k].predeltime,
   	          family[i].fmmemberindex[k].popular
#ifdef _FMVER21
							,family[i].fmmemberindex[k].eventflag
#endif
#ifdef _NEW_MANOR_LAW
							,family[i].fmmemberindex[k].momentum
#endif
					 );
   	    }
   	 } // for
         fprintf(fp, "\n");
      } // if
      fclose(fp);
      db_familyupdate[i] = 0;
   }

   return 0;
}

// 从档案读取家族庄园（启动时读取）
int readFMPoint(char *dir)
{
   char dirname[256];
   DIR  *d;
   struct dirent *de;
   int i = 0;
   {
   	char tmp[256];
   	snprintf(tmp, sizeof(tmp), "%s", dir);
   	if (mkdir(tmp, 0755) == 0)
   		log("create %s\n", tmp);
   }

   snprintf(dirname, sizeof(dirname), "%s", dir);
   d = opendir(dirname);
   if (d == NULL)
   {
   	log("无法打开文件 %s\n", dirname);
   	return -1;
   }
   while(1)
   {
   	de = readdir(d);
   	if(de == NULL)	break;
   	if (de -> d_name[0] != '.')
   	{
   	   char filename[256];
   	   FILE *fp;
   	   struct stat s;
   	   snprintf(filename, sizeof(filename), "%s/%s", dirname, de -> d_name);
   	   if (stat(filename, &s) < 0){
   	      continue;
   	   }
   	   if (!(s.st_mode & S_IFREG))
   	   {
   	      log("%s is not a regular file\n", filename);
   	      continue;
   	   }
   	   fp = fopen(filename, "r");
   	   if (fp == NULL)
   	   {
   	      log("无法打开文件 %s %s\n", filename, strerror(errno));
   	      continue;
   	   }
   	   while(1)
   	   {
   	      char line[1024];
   	      char data[1024];
   	      if (fgets(line, sizeof(line), fp) == NULL)
   	         break;
   	      chop(line);
   	      easyGetTokenFromBuf(line, "|", 1, data, sizeof(data));
   	      fmpoint[i].fl = atoi(data);
   	      easyGetTokenFromBuf(line, "|", 2, data, sizeof(data));
   	      fmpoint[i].x = atoi(data);
   	      easyGetTokenFromBuf(line, "|", 3, data, sizeof(data));
   	      fmpoint[i].y = atoi(data);
   	      easyGetTokenFromBuf(line, "|", 4, data, sizeof(data));
   	      fmpoint[i].fmfl = atoi(data);
   	      easyGetTokenFromBuf(line, "|", 5, data, sizeof(data));
   	      fmpoint[i].village = atoi(data);
   	      easyGetTokenFromBuf(line, "|", 6, data, sizeof(data));
   	      fmpoint[i].hadfmindex = atoi(data);
   	      easyGetTokenFromBuf(line, "|", 7, data, sizeof(data));
   	      strcpy(fmpoint[i].hadfmname, data);
   	      easyGetTokenFromBuf(line, "|", 8, data, sizeof(data));
   	      fmpoint[i].hadfmpopular = atoi(data);
   	      i++;
   	   }
   	   fclose(fp);
   	}
   }
   closedir(d);
   return 0;
}

// 将家族庄园写入档案
int writeFMPoint(char *dir)
{
   int i = 0;
   FILE *fp;
   char filename[256];
   sprintf(filename, "%s/db_fmpoint", dir);
   if (db_fmpointupdate == 0)
      return 0;
   else
   		log("已将更新资料db_fmpoint写入文件.\n");
   fp = fopen(filename, "w");
   if (fp == NULL)
   {
	log("无法打开文件: %s %s\n", filename, strerror(errno));
	return -1;
   }
   for(i = 0; i < MAX_FMPOINT; i++)
   {
	fprintf(fp, "%d|%d|%d|%d|%d|%d|%s|%d\n",
		fmpoint[i].fl, fmpoint[i].x, fmpoint[i].y, fmpoint[i].fmfl,
		fmpoint[i].village, fmpoint[i].hadfmindex, fmpoint[i].hadfmname,
		fmpoint[i].hadfmpopular);
   } // for
   fclose(fp);
   db_fmpointupdate = 0;
   return 0;
}

// 从档案中读取家族之间留言资料（启动时读取）
int readFMSMemo(char *dir)
{
   char dirname[256];
   DIR  *d;
   struct dirent *de;
   int i = 0;
   {
   	char tmp[256];
   	snprintf(tmp, sizeof(tmp), "%s", dir);
   	if (mkdir(tmp, 0755) == 0)
   		log("create %s\n", tmp);
   }

   snprintf(dirname, sizeof(dirname), "%s", dir);
   d = opendir(dirname);
   if (d == NULL)
   {
   	log("无法打开文件 %s\n", dirname);
   	return -1;
   }
   while(1)
   {
   	de = readdir(d);
   	if(de == NULL)	break;
   	if (de -> d_name[0] != '.')
   	{
   	   char filename[256];
   	   FILE *fp;
   	   struct stat s;
   	   snprintf(filename, sizeof(filename), "%s/%s", dirname, de -> d_name);
   	   if (stat(filename, &s) < 0){
   	      continue;
   	   }
   	   if (!(s.st_mode & S_IFREG))
   	   {
   	      log("%s 不是正常文件\n", filename);
   	      continue;
   	   }
   	   fp = fopen(filename, "r");
   	   if (fp == NULL)
   	   {
   	      log("无法打开文件 %s %s\n", filename, strerror(errno));
   	      continue;
   	   }
   	   {
   	      char line[15000];
   	      char data[15000];
   	      if (fgets(line, sizeof(line), fp) == NULL)
   	         break;
   	      chop(line);
   	      easyGetTokenFromBuf(line, "|", 1, data, sizeof(data));
   	      fmsmemo.num = atoi(data);
   	      easyGetTokenFromBuf(line, "|", 2, data, sizeof(data));
   	      fmsmemo.fmsnowwritenum = atoi(data);
   	      for (i = 0; i < fmsmemo.num; i++){
   	         easyGetTokenFromBuf(line, "|", 3 + i, data, sizeof(data));
   	         strcpy(fmsmemo.memodata[i], data);
   	      }
   	   }
   	   fclose(fp);
   	}
   }
   closedir(d);
   return 0;
}

// 将家族之间留言资料写入档案
int writeFMSMemo(char *dir)
{
   int i = 0;
   FILE *fp;
   char filename[256];
   sprintf(filename, "%s/db_fmsmemo", dir);
   if (db_fmsmemoupdate == 0)
      return 0;
   else
   		log("已将更新资料db_fmsmemo写入文件.\n");
   fp = fopen(filename, "w");
   if (fp == NULL)
   {
	log("无法打开文件: %s %s\n", filename, strerror(errno));
	return -1;
   }
   fprintf(fp, "%d|%d", fmsmemo.num, fmsmemo.fmsnowwritenum);
   for(i = 0; i < fmsmemo.num; i++)
   {
	fprintf(fp, "|%s", fmsmemo.memodata[i]);
	log("%s\n", fmsmemo.memodata[i]);
   } // for
   fprintf(fp, "\n");
   fclose(fp);
   db_fmsmemoupdate = 0;
   return 0;
}

// 取得未使用的 index
int getblankindex()
{
   int i = 0;
   for(i = 0; i < MAX_FAMILY; i++)
   	if (((family[i].fmindex <= 0) || (family[i].fmindex > MAX_FAMILY))
   	   && (strcmp(family[i].fmname, "") == 0))
		return i;
   return -1;
}

// 取得未使用的 fmindex
int getblankfmindex()
{
   int i = 0, j = 0;
   for (i = 1; i <= MAX_FAMILY; i++)
   {
   	int useflag = 0;
	for (j = 0; j < MAX_FAMILY; j++)
	{
   	   if (family[j].fmindex == i)
   	   {
   	      useflag = 1;
   	      continue;
   	   }
   	}
   	if (useflag == 0)
   	      return i;
   }
   return -1;
}

// 从 fmindex, fmname 取得 index
int getindex(int fmindex, char *fmname)
{
   int i = 0;
   for (i = 0; i <= fmindexmaxnum; i++)
   {
      if ((family[i].fmindex == fmindex)
         && (strcmp(family[i].fmname, fmname) == 0))
		return i;
   }
   return -1;
}

// 删除庄园的家族 fmindex
int delfmpointindex(int fmindex)
{
   int i;
   for (i = 0; i < MAX_FMPOINT; i++)
   {
   	if (fmpoint[i].hadfmindex == fmindex)
   	{
   		fmpoint[i].hadfmindex = -1;
   		strcpy(fmpoint[i].hadfmname, "-1");
   		fmpoint[i].hadfmpopular = -1;
   		db_fmpointupdate = 1;
   		return	0;
   	}
   }
   return -1;
}

// 检查 index 是否已使用
int CheckFMUse(int index)
{
   if ((index >= MAX_FAMILY) || (index < 0))
   	return	-1;
   if ((family[index].fmindex < 0) || (strcmp(family[index].fmname, "") == 0))
   	return	0;
   else
   	return	1;
}

// 检查 index fmindex, fmname资料是否正确
int CheckFM(int *index, char *fmname, int fmindex)
{
   if (*index == -1){
      *index = getindex(fmindex, fmname);
   }
   if ((*index > MAX_FAMILY) || (*index < 0)){
      log ("CheckFM_2 *index: big small? %d\n", *index);
      return	-1;
   }else {
      if ((family[*index].fmindex != fmindex) 
   	 || strcmp(family[*index].fmname, fmname) != 0)
      {
         log("CheckFM_3 [*index].fmindex:%d fmindex:%d [index].fmname:%s fmname:%s\n",
         	family[*index].fmindex, fmindex, family[*index].fmname, fmname);
         return -1;
      }
   }
   if (*index == -1){
      log ("CheckFM_4 index: -1 ? %d\n", *index);
      return -1;
   }
   if(CheckFMUse(*index) == 0)
   {
	log ("CheckFM_5 index: -1 ? %d\n", *index);
	return	-1;
   }
   if (family[*index].fmnum > MAX_MEMBERNUM || family[*index].fmjoinnum > MAX_MEMBERNUM)
   {
   	log ("CheckFM_6 fmnum: -1 ? %d\n", family[*index].fmnum);
	return	-1;
   }
   return	0;
}

// 删除人数不足之家族或太久未上站之成员
void delovertimeFMMem(int time)
{
   int i = 0, j = 0, k = 0;
   char data[150 * MAX_FAMILY];
   extern gmsv gs[MAXCONNECTION];
   for (i = 0 ; i <= fmindexmaxnum; i++){
      if(CheckFMUse(i) == 0)
         continue;
		if ( ( (family[i].predel_time < time) && (family[i].predel_time != -1)
		  && (family[i].fmsetupflag != 1)) || (family[i].fmjoinnum==0) ){

			log("ACDelOverTimeFM fmindex:%d fmname:%s fmjoinnum:%d flag:%d \n",
				family[i].fmindex, family[i].fmname,
				family[i].fmjoinnum, family[i].fmsetupflag );

			ACDelFM(i, family[i].fmname, family[i].fmindex);

			// 传送家族已删除之讯息至各 GMSV
			for (k = 0; k < MAXCONNECTION; k++){
				if (gs[k].use && gs[k].name[0])
					saacproto_ACFMAnnounce_send(k, SUCCESSFUL,
						family[i].fmname, family[i].fmindex, i,
						2, "", 0);
			}
                                                
			db_familyupdate[i] = 1;
			continue;
		}

      for (j = 1; j < family[i].fmmemberindexmaxnum; j++){
         if (family[i].fmmemberindex[j].charflag == FMMEMBER_NONE)
         	continue;
         if ((family[i].fmmemberindex[j].predeltime < time)
            && (family[i].fmmemberindex[j].predeltime != -1)){
#ifdef _FMVER21
            ACMemberLeaveFM(i, family[i].fmname, family[i].fmindex,
            	family[i].fmmemberindex[j].charname, 0, j, 0);
#else
            ACMemberLeaveFM(i, family[i].fmname, family[i].fmindex,
            	family[i].fmmemberindex[j].charname, 0, j);
#endif         
            db_familyupdate[i] = 1;

			/*
			// 通知族长有玩家已被删除
			if (family[i].fmmemberindex[0].onlineflag > 0)
			{
				char buf[256];
				sprintf(buf, "(%s)因太久未上线而离开您的家族了！目前家族人数：%4d人",
					family[i].fmmemberindex[j].charname, family[i].fmjoinnum);
				saacproto_ACFMAnnounce_send(family[i].fmmemberindex[0].onlineflag,
					SUCCESSFUL, family[i].fmname, family[i].fmindex, i, 3, buf,
					family[i].fmmemberindex[0].charfdid);
			}
			*/

         } // if
      } // for
   } // for
   if (db_familyupdate[i] == 1)
   {
      strcpy(data, "");
      for (i = 0; i <= fmindexmaxnum; i++)
      {
         char msg[256];
         if ((strcmp(family[i].fmname, "") != 0) && (family[i].fmindex != -1))
         {
#ifdef _PERSONAL_FAME	// Arminius: 家族个人声望
            sprintf(msg, "%d %s %s %d %d %d %d %d|", family[i].fmindex, family[i].fmname,
               family[i].fmleadername, family[i].fmtotalfame, family[i].fmjoinnum, i,
               family[i].fmsetupflag, family[i].fmacceptflag);
#else
            sprintf(msg, "%d %s %s %d %d %d %d %d|", family[i].fmindex, family[i].fmname,
               family[i].fmleadername, family[i].fmpopular, family[i].fmjoinnum, i,
               family[i].fmsetupflag, family[i].fmacceptflag);
#endif
            strcat(data, msg);
         }
      }
      for (i = 0; i < MAXCONNECTION; i++)
      {
         if (gs[i].use && gs[i].name[0])
            saacproto_ACShowFMList_send(i, SUCCESSFUL, fmnownum, data);
      }
   }
}

// 成立家族
#ifdef _PERSONAL_FAME
int ACAddFM(int fd, int *workindex, char *fmname, char *fmleadername,
	char *fmleaderid, int fmleaderlv, char *petname, char *petattr,
	char *fmrule, int fmsprite, int fmleadergrano, int fame, int charfdid)
#else
int ACAddFM(int fd, int *workindex, char *fmname, char *fmleadername,
	char *fmleaderid, int fmleaderlv, char *petname, char *petattr,
	char *fmrule, int fmsprite, int fmleadergrano, int charfdid)
#endif
{
	int i = 0, index, fmindex;
	time_t t1;
	index = getblankindex();
	if (index == -1) {
   	return -1; // 家族数量已满
	}
	fmindex = getblankfmindex();
	if (fmindex == -1){
   	return -1; // 家族数量已满，无空的 fmindex
	}
	for (i = 0; i <= fmindexmaxnum; i++){
   	if (strcmp(family[i].fmname, fmname) == 0)
			return	-2; // 已有相同家族名称存在
	}
	*workindex = index;
	family[index].fmindex = fmindex;
	strcpy(family[index].fmname, fmname);
	strcpy(family[index].fmleadername, fmleadername);
	strcpy(family[index].fmleaderid, fmleaderid);
	family[index].fmleadergrano = fmleadergrano;
	strcpy(family[index].petname, petname);
	strcpy(family[index].petattr, petattr);
	family[index].fmnum = 1;
	family[index].fmjoinnum = 1;
	family[index].fmacceptflag = 1;
	strcpy(family[index].fmrule, fmrule);
	family[index].fmsprite = fmsprite;
	family[index].fmpointindex = -1;
	family[index].fmgold = 0;
	family[index].fmmemberindexmaxnum = 1;
	time(&t1);
	family[index].apply_time = t1;
	family[index].predel_time = t1 + PREDEL_TIME;
	family[index].fmmemberindex[0].charlv = fmleaderlv;
#ifdef _FMVER21
	family[index].fmmemberindex[0].charflag = FMMEMBER_LEADER;
	family[index].fmmemberindex[0].eventflag = 1;
#else
	family[index].fmmemberindex[0].charflag = 1;
#endif
	family[index].fmmemberindex[0].onlineflag = fd;
	strcpy(family[index].fmmemberindex[0].charid, fmleaderid);   
	strcpy(family[index].fmmemberindex[0].charname, fmleadername);
	family[index].fmmemberindex[0].charfdid = charfdid;
	family[index].fmmemberindex[0].predeltime = -1;
	family[index].memonowwritenum = 0;
	family[index].memonum = 0;
	family[index].fmpopular = 0;
	family[index].fmadv = 0;
	family[index].fmfeed = 0;
	family[index].fmsynthesize = 0;
	family[index].fmdealfood = 0;
	family[index].fmpk = 0;
#ifdef _PERSONAL_FAME	// Arminius: 家族个人声望
	family[index].fmmemberindex[0].popular = fame;
	family[index].fmtotalfame = fame;
#endif
#ifdef _NEW_MANOR_LAW
	family[index].fmmomentum = 0;
#endif
	
	db_familyupdate[index] = 1;
	fmnownum++;
	if (fmindexmaxnum <= index)	fmindexmaxnum = index + 1;
	log("AddFM fmindexmaxnum:%d fmnownum:%d\n", fmindexmaxnum, fmnownum);
	
	AddFMMaintainSort(index);
	
	return fmindex;
}

// 加入家族
#ifdef _PERSONAL_FAME	// Arminius: 家族个人声望
int ACJoinFM(int fd, int index, char *fmname, int fmindex,
	char *charname, char *charid, int charlv, int fame, int charfdid)
#else
int ACJoinFM(int fd, int index, char *fmname, int fmindex,
	char *charname, char *charid, int charlv, int charfdid)
#endif
{
	int i = 0;
	time_t t1;
	if (CheckFM(&index, fmname, fmindex) < 0)	return -1;
	if (family[index].fmacceptflag == 0)	return	-2;
	if (family[index].fmnum >= MAX_MEMBERNUM || family[index].fmjoinnum >= MAX_MEMBERNUM)
   	return	-3;
	
	// 防止族长加入自己的家族  code:shan
	if ((strcmp(family[index].fmmemberindex[0].charname, charname) == 0)
		&& (strcmp(family[index].fmmemberindex[0].charid, charid) == 0))
		return -1;
	
	for (i = 1; i < MAX_MEMBERNUM; i++){
		if ((strcmp(family[index].fmmemberindex[i].charname, "") == 0)
			&& (strcmp(family[index].fmmemberindex[i].charid, "") == 0)){
			strcpy(family[index].fmmemberindex[i].charname, charname);
			strcpy(family[index].fmmemberindex[i].charid, charid);
			family[index].fmmemberindex[i].charlv = charlv;
#ifdef _FMVER21
			family[index].fmmemberindex[i].charflag = FMMEMBER_APPLY;
			family[index].fmmemberindex[i].eventflag = 0;
#else
			family[index].fmmemberindex[i].charflag = 2;
#endif
			family[index].fmmemberindex[i].onlineflag = fd;
			family[index].fmmemberindex[i].charfdid = charfdid;
			time(&t1);
			family[index].fmmemberindex[i].predeltime = t1 + PREDEL_TIME;
#ifdef _PERSONAL_FAME	// Arminius: 家族个人声望
			family[index].fmmemberindex[i].popular = fame;
			log("fame:%d\n", fame);
			// 这个时候还不可以把这个人的声望加到家族
#endif
#ifdef _NEW_MANOR_LAW
			family[index].fmmemberindex[i].momentum = 0;
#endif
			family[index].fmnum = family[index].fmnum + 1;
			if (family[index].fmmemberindexmaxnum <= i) family[index].fmmemberindexmaxnum = i + 1;
			db_familyupdate[index] = 1;
#ifdef _PERSONAL_FAME	// Arminius: 家族个人声望
			// 更新玩家资料
			saacproto_ACFMCharLogin_send(
				family[index].fmmemberindex[i].onlineflag,
				SUCCESSFUL, index,ACgetFMFloor(fmindex), family[index].fmtotalfame,
				family[index].fmmemberindex[i].charflag,
				family[index].fmsetupflag, 1, i,
				family[index].fmmemberindex[i].popular,	// Arminius 9.11
				family[index].fmmemberindex[i].charfdid
#ifdef _NEW_MANOR_LAW
				,family[index].fmmemberindex[i].momentum
#endif
				);
#else
			saacproto_ACFMCharLogin_send(
				family[index].fmmemberindex[i].onlineflag,
				SUCCESSFUL, index,ACgetFMFloor(fmindex),family[index].fmpopular,
				family[index].fmmemberindex[i].charflag,
				family[index].fmsetupflag, 1, i,
				family[index].fmmemberindex[i].charfdid);
#endif
			// 通知族长有玩家愿意加入
			if (family[index].fmmemberindex[0].onlineflag > 0){
				char buf[256];
				sprintf(buf, "(%s lv:%d)正要求加入您的家族喔！目前家族人数：%4d人",
					charname, charlv, family[index].fmjoinnum);
				saacproto_ACFMAnnounce_send(family[index].fmmemberindex[0].onlineflag,
					SUCCESSFUL, fmname, fmindex, index, 3, buf,
					family[index].fmmemberindex[0].charfdid);
			}
			return 0;
		}
	}
	return -1;
}

// 离开家族
int ACLeaveFM(int index, char *fmname, int fmindex,
	char *charname, char* charid)
{
	int i = 0;
	if (CheckFM(&index, fmname, fmindex) < 0)	return -1;
	
	for (i = 1; i < family[index].fmmemberindexmaxnum; i++)
	{
   	if ((strcmp(family[index].fmmemberindex[i].charname, charname) == 0)
			&& (strcmp(family[index].fmmemberindex[i].charid, charid) == 0))
   	{
#ifdef _FMVER21
			ACMemberLeaveFM(index, fmname, fmindex, charname, 0, i, 0);
#else
			ACMemberLeaveFM(index, fmname, fmindex, charname, 0, i);
#endif
			db_familyupdate[index] = 1;
			// 通知族长有玩家已离开家族
			if (family[index].fmmemberindex[0].onlineflag > 0)
			{
				char buf[256];
				sprintf(buf, "(%s)已经离开您的家族了！目前家族人数：%4d人",
					charname, family[index].fmjoinnum);
				saacproto_ACFMAnnounce_send(family[index].fmmemberindex[0].onlineflag,
					SUCCESSFUL, fmname, fmindex, index, 3, buf,
					family[index].fmmemberindex[0].charfdid);
			}
			return	0;
   	}
	}
	return -1;
}
// 修正家族资料
// kindflag 1:是否继续招募家族成员 2:家族ＰＫ声望 3:家族守护兽 4:家族主旨
//	    5:解散家族时间 6:家族基金 7:家族冒险声望 8:家族饲育声望
//	    9:家族合成、加工声望 10:家族料理声望 11:族长禅让
int ACFixFMData(int index, char *fmname, int fmindex, int kindflag, int charindex,
	char *data1, char *data2)
{
	int recvdata = 0;
	if (CheckFM(&index, fmname, fmindex) < 0)	return -1;
	
	if (kindflag == 1 || kindflag == 2 || (kindflag >= 6 && kindflag <= 11)
#ifdef _NEW_MANOR_LAW
		|| kindflag == 12 || kindflag == 13
#endif
		){
		recvdata = atoi(data1);
		if (recvdata > MAXRECVPOP && (kindflag == 2 || (kindflag >= 7 && kindflag <= 10))) {
			log("ACDelOverTimePOP index:%d fmname:%s kindflag:%d charindex:%d recvdata:%d\n",
				index, fmname, kindflag, charindex, recvdata);
			return -1;
		}
	}
	if (kindflag == 1)
   	family[index].fmacceptflag = recvdata;
	else if (kindflag == 2){
   	family[index].fmpk = family[index].fmpk + recvdata;
   	if (family[index].fmpk < MINFMPOPNUM)	family[index].fmpk = MINFMPOPNUM;
   	if (family[index].fmpk > MAXFMPOPNUM)	family[index].fmpk = MAXFMPOPNUM;
	}else if (kindflag == 3){
   	if ((strcmp(data1, "") == 0) || (strcmp(data2, "") == 0))	return -1;
   	strcpy(family[index].petname, data1);
   	strcpy(family[index].petattr, data2);
   	if (family[index].fmsetupflag == 1)
			family[index].predel_time = -1;
	}else if (kindflag == 4){
   	if (strcmp(data1, "") == 0)	return -1;
   	strcpy(family[index].fmrule, data1);
	}else if (kindflag == 5){
   	time_t t1;
   	time(&t1);
   	family[index].predel_time = t1 + PREDEL_TIME;
	}else if (kindflag == 6){
   	if (family[index].fmgold + recvdata < 0)	return	-1;
   	family[index].fmgold = family[index].fmgold + recvdata;
   	if (family[index].fmgold > FMMAXGOLD)
			family[index].fmgold = FMMAXGOLD;
		sprintf(data2,"%d",family[index].fmgold);
	}
	else if (kindflag == 7){
   	family[index].fmadv = family[index].fmadv + recvdata;
   	if (family[index].fmadv < 0)	family[index].fmadv = 0;
   	if (family[index].fmadv > MAXFMPOPNUM)	family[index].fmadv = MAXFMPOPNUM;
#ifdef _NEW_MANOR_LAW
		family[index].fmmemberindex[charindex].momentum += recvdata;
   	if (family[index].fmmemberindex[charindex].momentum < 0)	family[index].fmmemberindex[charindex].momentum = 0;
   	if (family[index].fmmemberindex[charindex].momentum > MAXFMPOPNUM)	family[index].fmmemberindex[charindex].momentum = MAXFMPOPNUM;
		family[index].fmmemberindex[charindex].popular = atoi(data2);
#endif
	}else if (kindflag == 8){
   	family[index].fmfeed = family[index].fmfeed + recvdata;
   	if (family[index].fmfeed < 0)	family[index].fmfeed = 0;
   	if (family[index].fmfeed > MAXFMPOPNUM)	family[index].fmfeed = MAXFMPOPNUM;
#ifdef _NEW_MANOR_LAW
		family[index].fmmemberindex[charindex].momentum += recvdata;
   	if (family[index].fmmemberindex[charindex].momentum < 0)	family[index].fmmemberindex[charindex].momentum = 0;
   	if (family[index].fmmemberindex[charindex].momentum > MAXFMPOPNUM)	family[index].fmmemberindex[charindex].momentum = MAXFMPOPNUM;
		family[index].fmmemberindex[charindex].popular = atoi(data2);
#endif
	}
#ifndef _NEW_MANOR_LAW
	else if (kindflag == 9){
   	family[index].fmsynthesize = family[index].fmsynthesize + recvdata;
   	if (family[index].fmsynthesize < 0)	family[index].fmsynthesize = 0;
   	if (family[index].fmsynthesize > MAXFMPOPNUM)	family[index].fmsynthesize = MAXFMPOPNUM;
	}
	else if (kindflag == 10){
		family[index].fmdealfood = family[index].fmdealfood + recvdata;
		if (family[index].fmdealfood < 0)	family[index].fmdealfood = 0;
		if (family[index].fmdealfood > MAXFMPOPNUM)	family[index].fmdealfood = MAXFMPOPNUM;
  }
#endif
#ifdef _FMVER21
  else if (kindflag == 11){
		char	charname[256], charid[256];
		int	charlv, onlineflag, charfdid, popular;
#ifdef _NEW_MANOR_LAW
		int momentum;
#endif
		int	floor =  ACgetFMFloor(fmindex);
		if (strcmp(family[index].fmmemberindex[charindex].charname, data2) != 0
			 || strcmp(family[index].fmmemberindex[charindex].charid, "") == 0)
			 return	-1;
		 // shan 2002/01/04
		 log("\nshan-->fmindex:%d fmname:%s newleadername:%s newleaderid->%s\n", 
			 fmindex, fmname, family[index].fmmemberindex[charindex].charname,
			 family[index].fmmemberindex[charindex].charid );
		 strcpy(charname, family[index].fmmemberindex[charindex].charname);
		 strcpy(charid, family[index].fmmemberindex[charindex].charid);
		 charlv = family[index].fmmemberindex[charindex].charlv;
		 onlineflag = family[index].fmmemberindex[charindex].onlineflag;
		 charfdid = family[index].fmmemberindex[charindex].charfdid;
		 popular = family[index].fmmemberindex[charindex].popular;
#ifdef _NEW_MANOR_LAW
		 momentum = family[index].fmmemberindex[charindex].momentum;
#endif
		 // 资料转换
		 strcpy(family[index].fmmemberindex[charindex].charname,
			 family[index].fmmemberindex[0].charname);
		 strcpy(family[index].fmmemberindex[charindex].charid,
			 family[index].fmmemberindex[0].charid);
		 family[index].fmmemberindex[charindex].charlv = family[index].fmmemberindex[0].charlv;
		 family[index].fmmemberindex[charindex].charflag = FMMEMBER_MEMBER;
		 family[index].fmmemberindex[charindex].onlineflag = family[index].fmmemberindex[0].onlineflag;
		 family[index].fmmemberindex[charindex].charfdid = family[index].fmmemberindex[0].charfdid;
		 family[index].fmmemberindex[charindex].popular = family[index].fmmemberindex[0].popular;
#ifdef _NEW_MANOR_LAW
		 family[index].fmmemberindex[charindex].momentum = family[index].fmmemberindex[0].momentum;
#endif
		 
		 strcpy(family[index].fmmemberindex[0].charname, charname);
		 strcpy(family[index].fmmemberindex[0].charid, charid);
		 family[index].fmmemberindex[0].charlv = charlv;
		 family[index].fmmemberindex[0].charflag = FMMEMBER_LEADER;
		 family[index].fmmemberindex[0].onlineflag = onlineflag;
		 family[index].fmmemberindex[0].charfdid = charfdid;
		 family[index].fmmemberindex[0].popular = popular;
#ifdef _NEW_MANOR_LAW
		 family[index].fmmemberindex[0].momentum = momentum;
#endif
		 strcpy(family[index].fmleadername, charname);
		 strcpy(family[index].fmleaderid, charid);
		 family[index].fmleadergrano = recvdata;
		 strcpy(family[index].petname, "");
		 strcpy(family[index].petattr, "");
		 
		 // 更新族长资料
		 if (family[index].fmmemberindex[0].onlineflag > 0)
		 {
#ifdef _PERSONAL_FAME	// Arminius: 家族个人声望
			 saacproto_ACFMCharLogin_send(
   	   		family[index].fmmemberindex[0].onlineflag,
					SUCCESSFUL, index, floor, family[index].fmtotalfame,
					family[index].fmmemberindex[0].charflag,
					family[index].fmsetupflag, 1, 0,
					family[index].fmmemberindex[0].popular,	// Arminius 9.11
					family[index].fmmemberindex[0].charfdid
#ifdef _NEW_MANOR_LAW
					,family[index].fmmemberindex[0].momentum
#endif
					);
#else
			 saacproto_ACFMCharLogin_send(
   	   		family[index].fmmemberindex[0].onlineflag,
					SUCCESSFUL, index, floor, family[index].fmpopular,
					family[index].fmmemberindex[0].charflag,
					family[index].fmsetupflag, 1, 0,
					family[index].fmmemberindex[0].charfdid);
#endif
		 }
		 // 更新成员资料
		 if (family[index].fmmemberindex[charindex].onlineflag > 0)
		 {
#ifdef _PERSONAL_FAME	// Arminius: 家族个人声望
			 saacproto_ACFMCharLogin_send(
   	   		family[index].fmmemberindex[charindex].onlineflag,
					SUCCESSFUL, index, floor, family[index].fmtotalfame,
					family[index].fmmemberindex[charindex].charflag,
					family[index].fmsetupflag, 1, charindex,
					family[index].fmmemberindex[charindex].popular,	// Arminius 9.11
					family[index].fmmemberindex[charindex].charfdid
#ifdef _NEW_MANOR_LAW
					,family[index].fmmemberindex[charindex].momentum
#endif
					);
#else
			 saacproto_ACFMCharLogin_send(
   	   		family[index].fmmemberindex[charindex].onlineflag,
					SUCCESSFUL, index, floor, family[index].fmpopular,
					family[index].fmmemberindex[charindex].charflag,
					family[index].fmsetupflag, 1, charindex,
					family[index].fmmemberindex[charindex].charfdid);
#endif
		 }
   }
#endif
#ifdef _NEW_MANOR_LAW
		else if (kindflag == 12){
			family[index].fmmemberindex[charindex].momentum = recvdata;
			if(family[index].fmmemberindex[charindex].momentum < 0) family[index].fmmemberindex[charindex].momentum = 0;
			if(family[index].fmmemberindex[charindex].momentum > MAXFMPOPNUM)	family[index].fmmemberindex[charindex].momentum = MAXFMPOPNUM;
		}
		else if(kindflag == 13){
		 family[index].fmmemberindex[charindex].popular = recvdata;
		}
#endif
   if ((kindflag == 2) || (kindflag == 7) || (kindflag == 8) || (kindflag == 9)
		 || (kindflag == 10)
#ifdef _NEW_MANOR_LAW
		 || (kindflag == 12) || (kindflag == 13)
#endif
		 )
   {
#ifdef _PERSONAL_FAME	// Arminius: 家族个人声望
		 // 记录玩家家族声望
#ifndef _NEW_MANOR_LAW
		 if (charindex >= 0 && charindex < MAX_MEMBERNUM)
			 family[index].fmmemberindex[charindex].popular += recvdata;
#endif
		 // 计算家族声望
		 family[index].fmpopular = family[index].fmadv + family[index].fmfeed
			 + family[index].fmsynthesize + family[index].fmdealfood
			 + family[index].fmpk;
		 {
			 int j;
			 family[index].fmtotalfame=family[index].fmpopular;
#ifdef _NEW_MANOR_LAW
			 family[index].fmmomentum = 0;
#endif
			 for (j = 0; j < MAX_MEMBERNUM; j++) {
				 if ((family[index].fmmemberindex[j].charflag!=FMMEMBER_NONE) &&
					 (family[index].fmmemberindex[j].charflag!=FMMEMBER_APPLY) )
				 {
					 family[index].fmtotalfame += family[index].fmmemberindex[j].popular;
#ifdef _NEW_MANOR_LAW
					 // 家族成员个人气势加总
					 family[index].fmmomentum += family[index].fmmemberindex[j].momentum;
#endif
				 }
			 }
#ifdef _NEW_MANOR_LAW
			 // 家族气势 = (家族人数平方) / 10 + 1 + 家族成员个人气势加总
			 family[index].fmmomentum += ((family[index].fmjoinnum * family[index].fmjoinnum) / 10 + 1) * 100;
//			 log("\nACFixFMData total family momentum %d\n",family[index].fmmomentum);
#endif
		 }
		 sortFamily();
		 if (family[index].fmpointindex >= 0){
			 fmpoint[family[index].fmpointindex].hadfmpopular = family[index].fmtotalfame;
			 db_fmpointupdate = 1;
		 }
#else
		 family[index].fmpopular = family[index].fmadv + family[index].fmfeed
			 + family[index].fmsynthesize + family[index].fmdealfood
			 + family[index].fmpk;
		 sortFamily();
		 if (family[index].fmpointindex >= 0){
			 fmpoint[family[index].fmpointindex].hadfmpopular = family[index].fmpopular;
			 db_fmpointupdate = 1;
		 }
		 // 记录玩家家族声望
		 if (charindex >= 0 && charindex < MAX_MEMBERNUM)
			 family[index].fmmemberindex[charindex].popular = 
			 family[index].fmmemberindex[charindex].popular + recvdata;
#endif
		 // 更新族长资料
		 if (family[index].fmmemberindex[0].onlineflag > 0)
		 {
			 int	floor =  ACgetFMFloor(fmindex);
#ifdef _PERSONAL_FAME	// Arminius: 家族个人声望
			 saacproto_ACFMCharLogin_send(
   	   		family[index].fmmemberindex[0].onlineflag,
					SUCCESSFUL, index, floor, family[index].fmtotalfame,
#ifdef _FMVER21
					family[index].fmmemberindex[0].charflag, family[index].fmsetupflag, 1, 0,
#else   	 
					1, family[index].fmsetupflag, 1, 0,
#endif
					family[index].fmmemberindex[0].popular,	// Arminius 9.11
					family[index].fmmemberindex[0].charfdid
#ifdef _NEW_MANOR_LAW
					,family[index].fmmemberindex[0].momentum
#endif
					);
#else
			 saacproto_ACFMCharLogin_send(
   	   		family[index].fmmemberindex[0].onlineflag,
					SUCCESSFUL, index, floor, family[index].fmpopular,
#ifdef _FMVER21
					family[index].fmmemberindex[0].charflag, family[index].fmsetupflag, 1, 0,
#else   	 
					1, family[index].fmsetupflag, 1, 0,
#endif
					family[index].fmmemberindex[0].charfdid);
#endif
		 }
   }
   db_familyupdate[index] = 1;
   return kindflag;
}

// 更改ＰＫ後家族ＰＫ值
int ACFixFMPK(int winindex, char* winfmname, int winfmindex, int loseindex,
	char* losefmname, int losefmindex)
{
   int award = 0;
   if (CheckFM(&winindex, winfmname, winfmindex) < 0)	return -1;
   if (CheckFM(&loseindex, losefmname, losefmindex) < 0)	return -1;
   
   award = (family[loseindex].fmpopular / 10);
   if (family[loseindex].fmpopular - award < 0)
   	award = family[loseindex].fmpopular;

   family[loseindex].fmpk = family[loseindex].fmpk - award;
   family[winindex].fmpk = family[winindex].fmpk + award;

   if (family[loseindex].fmpk < MINFMPOPNUM)
   {
#if 1 // Robin 2003/11/27 修正输方家族PK声望到达下限时胜方可洗声望
		int overpoint;
		overpoint = MINFMPOPNUM - family[loseindex].fmpk;
		family[winindex].fmpk = family[winindex].fmpk - overpoint;
#endif
		family[loseindex].fmpk = MINFMPOPNUM;
   }
   // CoolFish: 2001/11/28 MINFMPOPNUM -> MAXFMPOPNUM
   if (family[winindex].fmpk > MAXFMPOPNUM)
		family[winindex].fmpk = MAXFMPOPNUM;

   {
   	family[winindex].fmpopular = family[winindex].fmadv + family[winindex].fmfeed
   		+ family[winindex].fmsynthesize + family[winindex].fmdealfood
   		+ family[winindex].fmpk;

#ifdef _PERSONAL_FAME	// Arminius: 家族个人声望
	{
          int j;
          family[winindex].fmtotalfame=family[winindex].fmpopular;
          for (j = 0; j < MAX_MEMBERNUM; j++) {
            if ((family[winindex].fmmemberindex[j].charflag!=FMMEMBER_NONE) &&
                (family[winindex].fmmemberindex[j].charflag!=FMMEMBER_APPLY) )
              family[winindex].fmtotalfame += family[winindex].fmmemberindex[j].popular;
          }
	}
#endif

   	if (family[winindex].fmmemberindex[0].onlineflag > 0)
   	{
   		int	floor =  ACgetFMFloor(winfmindex);
#ifdef _PERSONAL_FAME	// Arminius: 家族个人声望
   	 	saacproto_ACFMCharLogin_send(
   	   		family[winindex].fmmemberindex[0].onlineflag,
   	   		SUCCESSFUL, winindex, floor, family[winindex].fmtotalfame,
	#ifdef _FMVER21
					family[winindex].fmmemberindex[0].charflag, family[winindex].fmsetupflag, 1, 0,
	#else   	 
   	   		1, family[winindex].fmsetupflag, 1, 0,
	#endif
					family[winindex].fmmemberindex[0].popular,	// Arminius 9.11
   	   		family[winindex].fmmemberindex[0].charfdid
	#ifdef _NEW_MANOR_LAW
					,family[winindex].fmmemberindex[0].momentum
	#endif
					);
#else
   	 	saacproto_ACFMCharLogin_send(
   	   		family[winindex].fmmemberindex[0].onlineflag,
   	   		SUCCESSFUL, winindex, floor, family[winindex].fmpopular,
	#ifdef _FMVER21
					family[winindex].fmmemberindex[0].charflag, family[winindex].fmsetupflag, 1, 0,
	#else   	 
   	   		1, family[winindex].fmsetupflag, 1, 0,
	#endif
   	   		family[winindex].fmmemberindex[0].charfdid);
#endif
   	}
   	family[loseindex].fmpopular = family[loseindex].fmadv + family[loseindex].fmfeed
   		+ family[loseindex].fmsynthesize + family[loseindex].fmdealfood
   		+ family[loseindex].fmpk;

#ifdef _PERSONAL_FAME	// Arminius: 家族个人声望
	{
          int j;
          family[loseindex].fmtotalfame=family[loseindex].fmpopular;
          for (j = 0; j < MAX_MEMBERNUM; j++) {
            if ((family[loseindex].fmmemberindex[j].charflag!=FMMEMBER_NONE) &&
                (family[loseindex].fmmemberindex[j].charflag!=FMMEMBER_APPLY) )
              family[loseindex].fmtotalfame += family[loseindex].fmmemberindex[j].popular;
          }
	}
#endif

   	if (family[loseindex].fmmemberindex[0].onlineflag > 0)
   	{
   		int	floor =  ACgetFMFloor(losefmindex);
#ifdef _PERSONAL_FAME	// Arminius: 家族个人声望
   	 	saacproto_ACFMCharLogin_send(
   	   		family[loseindex].fmmemberindex[0].onlineflag,
   	   		SUCCESSFUL, loseindex, floor, family[loseindex].fmtotalfame,
	#ifdef _FMVER21
					family[loseindex].fmmemberindex[0].charflag, family[loseindex].fmsetupflag, 1, 0,
	#else   	 
   	   		1, family[loseindex].fmsetupflag, 1, 0,
	#endif
					family[loseindex].fmmemberindex[0].popular,	// Arminius 9.11
   	   		family[loseindex].fmmemberindex[0].charfdid
	#ifdef _NEW_MANOR_LAW
					,family[loseindex].fmmemberindex[0].momentum
	#endif
					);
#else
   	 	saacproto_ACFMCharLogin_send(
   	   		family[loseindex].fmmemberindex[0].onlineflag,
   	   		SUCCESSFUL, loseindex, floor, family[loseindex].fmpopular,
	#ifdef _FMVER21
					family[loseindex].fmmemberindex[0].charflag, family[loseindex].fmsetupflag, 1, 0,
	#else   	 
   	   		1, family[loseindex].fmsetupflag, 1, 0,
	#endif
   	   		family[loseindex].fmmemberindex[0].charfdid);
#endif
   	}
   }
   sortFamily();
   if (family[winindex].fmpointindex >= 0)
   {
#ifdef _PERSONAL_FAME	// Arminius: 家族个人声望
   	fmpoint[family[winindex].fmpointindex].hadfmpopular = family[winindex].fmtotalfame;
#else
   	fmpoint[family[winindex].fmpointindex].hadfmpopular = family[winindex].fmpopular;
#endif
   	db_fmpointupdate = 1;
   }
   if (family[loseindex].fmpointindex >= 0)
   {
#ifdef _PERSONAL_FAME	// Arminius: 家族个人声望
   	fmpoint[family[loseindex].fmpointindex].hadfmpopular = family[loseindex].fmtotalfame;
#else
   	fmpoint[family[loseindex].fmpointindex].hadfmpopular = family[loseindex].fmpopular;
#endif
   	db_fmpointupdate = 1;
   }
   return	award;
}

// 解散家族
int ACDelFM(int index, char *fmname, int fmindex)
{
	int i;
	if (CheckFM(&index, fmname, fmindex) < 0)	return -1;
	
	log("\nACDelFM index:%d fmname:%s fmindex:%d\n", index, fmname, fmindex);
	
	if (family[index].fmpointindex >= 0)
	{
   	delfmpointindex(family[index].fmindex);
   	family[index].fmpointindex = -1;
	}
	if (family[index].memonum > 0)
	{
   	for (i = 0; i < family[index].memonum; i++)
			memset(family[index].memolist[i], 0,
			sizeof(family[index].memolist[i]));
	}
	family[index].fmindex = -1;
	memset(family[index].fmname, 0, sizeof(family[index].fmname));
	memset(family[index].memolist, 0, sizeof(family[index].memolist));
	family[index].fmnum = 0;
	family[index].fmjoinnum = 0;
	family[index].fmsetupflag = -1;
	family[index].fmpopular = 0;
	family[index].fmadv = 0;
	family[index].fmfeed= 0;
	family[index].fmsynthesize = 0;
	family[index].fmdealfood = 0;
	family[index].fmpk = 0;
#ifdef _PERSONAL_FAME	// Arminius: 家族个人声望
	family[index].fmtotalfame = 0;
#endif
#ifdef _NEW_MANOR_LAW
	family[index].fmmomentum = 0;
#endif
	for (i = 0; i < family[index].fmmemberindexmaxnum; i++)
	{
   	// 传送最新状态给家族成员
#ifdef _PERSONAL_FAME	// Arminius: 家族个人声望
   	if (family[index].fmmemberindex[i].onlineflag > 0)
			saacproto_ACFMCharLogin_send(
   			family[index].fmmemberindex[i].onlineflag,
				FAILED, index, family[index].fmpointindex,
				family[index].fmtotalfame, -1, family[index].fmsetupflag, 0,
				i, family[index].fmmemberindex[i].popular,	// Arminius 9.11
				family[index].fmmemberindex[i].charfdid
	#ifdef _NEW_MANOR_LAW
				,family[index].fmmemberindex[i].momentum
	#endif
				);
#else
   	if (family[index].fmmemberindex[i].onlineflag > 0)
			saacproto_ACFMCharLogin_send(
   			family[index].fmmemberindex[i].onlineflag,
				FAILED, index, family[index].fmpointindex,
				family[index].fmpopular, -1, family[index].fmsetupflag, 0,
				i, family[index].fmmemberindex[i].charfdid);
#endif
   	memset(family[index].fmmemberindex[i].charname, 0,
			sizeof(family[index].fmmemberindex[i].charname));
   	memset(family[index].fmmemberindex[i].charid, 0,
			sizeof(family[index].fmmemberindex[i].charid));
   	family[index].fmmemberindex[i].charflag = FMMEMBER_NONE;
   	family[index].fmmemberindex[i].predeltime = -1;
   	family[index].fmmemberindex[i].popular = 0;
#ifdef _NEW_MANOR_LAW
		family[index].fmmemberindex[i].momentum = 0;
#endif
	}
	family[index].fmmemberindexmaxnum = 0;
	fmnownum--;
	if (fmindexmaxnum == index)	fmindexmaxnum--;
	
	// 传送家族已删除之讯息至各 GMSV 以清除对战排程
	for (i = 0; i < MAXCONNECTION; i++)
	{
   	if (gs[i].use && gs[i].name[0])
			saacproto_ACFMClearPK_send(i, SUCCESSFUL, fmname, fmindex, index);
	}
	
	DelFMMaintainSort(index);
	
	db_familyupdate[i] = 1;
	return 0;
}

// 显示家族列表
int ACShowFMList(char *data)
{
   int i = 0;
   strcpy(data, "");
   
   for (i = 0; i <= fmindexmaxnum; i++)
   {
      char msg[256];
      if ((strcmp(family[i].fmname, "") != 0) && (family[i].fmindex != -1))
      {
#ifdef _PERSONAL_FAME	// Arminius: 家族个人声望
         sprintf(msg, "%d %s %s %d %d %d %d %d|", family[i].fmindex, family[i].fmname,
         	family[i].fmleadername, family[i].fmtotalfame, family[i].fmjoinnum, i,
         	family[i].fmsetupflag, family[i].fmacceptflag);
#else
         sprintf(msg, "%d %s %s %d %d %d %d %d|", family[i].fmindex, family[i].fmname,
         	family[i].fmleadername, family[i].fmpopular, family[i].fmjoinnum, i,
         	family[i].fmsetupflag, family[i].fmacceptflag);
#endif
         strcat(data, msg);
      }
   }
   return	fmnownum;
}

// 显示家族成员列表
int ACShowFMMemberList(int index, int *fmacceptflag, int *fmjoinnum, char *data)
{
   int i = 0;
   if ((index > MAX_FAMILY) || (index < 0))	return	-1;
   if (CheckFMUse(index) == 0)	return -1;
   
   strcpy(data, "");
   *fmjoinnum = family[index].fmjoinnum;
   for (i = 0; i < family[index].fmmemberindexmaxnum; i++)
   {
      char msg[256];
      if ((strcmp(family[index].fmmemberindex[i].charname, "") != 0)
         && (strcmp(family[index].fmmemberindex[i].charid, "") != 0)
         && (family[index].fmmemberindex[i].charflag != FMMEMBER_NONE))
      {
#ifdef _FMVER21
         sprintf(msg, "%d|%s|%d|%d|%d|%d|%d ", i,
            family[index].fmmemberindex[i].charname,
            family[index].fmmemberindex[i].charlv,
            family[index].fmmemberindex[i].onlineflag,
            (family[index].fmmemberindex[i].popular / 100),
            family[index].fmmemberindex[i].eventflag,
            family[index].fmmemberindex[i].charflag);
            strcat(data, msg);
#else
         sprintf(msg, "%d|%s|%d|%d|%d|%d ", i,
            family[index].fmmemberindex[i].charname,
            family[index].fmmemberindex[i].charlv,
            family[index].fmmemberindex[i].onlineflag,
            (family[index].fmmemberindex[i].popular / 100),
            family[index].fmmemberindex[i].charflag);
            strcat(data, msg);
#endif            
     }
   } // for 
   *fmacceptflag = family[index].fmacceptflag;
   return	family[index].fmnum;
}

// 显示家族详细资料
int ACFMDetail(int index, char *fmname, int fmindex, char *data)
{
   if (CheckFM(&index, fmname, fmindex) < 0)	return -1;
   
   strcpy(data, "");
   sprintf(data, "%d|%d|%s|%s|%d|%s|%s|%d|%d|%s|%d", index, family[index].fmindex,
   	family[index].fmname, family[index].fmleadername,
   	family[index].fmleadergrano, family[index].petname, family[index].petattr,
   	family[index].fmjoinnum, family[index].fmacceptflag,
   	family[index].fmrule, family[index].fmsprite );
   return	0;
}

// 族长审核成员加入家族
#ifdef _FMVER21
int ACMemberJoinFM(int index, char *fmname, int fmindex,
	char *charname, int charindex, int result, int meindex)
#else
int ACMemberJoinFM(int index, char *fmname, int fmindex,
	char *charname, int charindex, int result)
#endif	
{
	time_t t1;
	int	floor;
	if (CheckFM(&index, fmname, fmindex) < 0)	return -1;
	floor = ACgetFMFloor(fmindex);
	
#ifdef _FIX_LEADER_ERR			// WON ADD 修正族长问题
	if (charindex == 0 )	return -1;	
#endif
	
#ifdef _FMVER21
	if (result < 0)	return	-1;
	if (meindex < 0 || meindex > MAX_MEMBERNUM)	return	-1;
#endif
	if (strcmp(family[index].fmmemberindex[charindex].charname, charname) == 0){
			if (family[index].fmjoinnum >= MAX_MEMBERNUM)	return	-1;
			time(&t1);
			family[index].fmmemberindex[charindex].predeltime = t1 + PREDELMEMBER_TIME;
			if (family[index].fmmemberindex[charindex].charflag == FMMEMBER_APPLY) family[index].fmjoinnum++;
			if (family[index].fmmemberindexmaxnum <= charindex)	family[index].fmmemberindexmaxnum = charindex + 1;
			if (family[index].fmjoinnum >= MIN_MEMBERNUM)
			{
				if (family[index].fmjoinnum == MIN_MEMBERNUM)	family[index].setup_time = t1;
				family[index].fmsetupflag = 1;
				family[index].predel_time = -1;
			}
			if (family[index].fmjoinnum >= MAX_MEMBERNUM)	family[index].fmacceptflag = 0;
#ifdef _PERSONAL_FAME	// Arminius: 家族个人声望
			{
				int j;
				family[index].fmtotalfame=family[index].fmpopular;
				for (j = 0; j < MAX_MEMBERNUM; j++) {
					if ((family[index].fmmemberindex[j].charflag!=FMMEMBER_NONE) &&
						(family[index].fmmemberindex[j].charflag!=FMMEMBER_APPLY) )
						family[index].fmtotalfame += family[index].fmmemberindex[j].popular;
				}
			}
#endif
#ifdef _FMVER21
			if (result != FMMEMBER_MEMBER)
			{
				if (result == FMMEMBER_ELDER && family[index].fmmemberindex[charindex].eventflag != 1)
					return	-1;
				if (ACFMAssignOcp(index, fmname, fmindex, charname, charindex, result) >= 0)
					return	0;
				else	return	-1;
			}
#endif
			if (family[index].fmmemberindex[charindex].onlineflag > 0){
				char buf[256];
   	   	// 通知玩家
#ifdef _FMVER21
				if (family[index].fmmemberindex[charindex].charflag != FMMEMBER_APPLY)
					sprintf(buf, "%s族长已经将你的家族职等改为%s！",
					family[index].fmname, MEMBERKIND_INTDATA[result]);
				else
					sprintf(buf, "恭喜你！%s已经审核完毕您的加入申请！",
					family[index].fmmemberindex[meindex].charname);
#else
   	   	sprintf(buf, "恭喜你！%s族长已经审核完毕您的加入申请！", family[index].fmname);
#endif
   	   	saacproto_ACFMAnnounce_send(family[index].fmmemberindex[charindex].onlineflag,
					SUCCESSFUL, fmname, fmindex, index, 3, buf,
					family[index].fmmemberindex[charindex].charfdid);
#ifdef _FMVER21
				family[index].fmmemberindex[charindex].charflag = FMMEMBER_MEMBER;
				// 计算家族声望
				family[index].fmpopular = family[index].fmadv + family[index].fmfeed
        		+ family[index].fmsynthesize + family[index].fmdealfood
						+ family[index].fmpk;
				{
					int j;
					family[index].fmtotalfame = family[index].fmpopular;
					for (j = 0; j < MAX_MEMBERNUM; j++)
					{
						if ((family[index].fmmemberindex[j].charflag != FMMEMBER_NONE) &&
							(family[index].fmmemberindex[j].charflag != FMMEMBER_APPLY) )
							family[index].fmtotalfame += family[index].fmmemberindex[j].popular;
					}
				}
#else
				family[index].fmmemberindex[charindex].charflag = 1;
#endif
#ifdef _NEW_MANOR_LAW
				{
					int i;
					
					family[index].fmmomentum = 0;
					for(i=0;i<MAX_MEMBERNUM;i++){
						if((family[index].fmmemberindex[i].charflag != FMMEMBER_NONE) &&
							(family[index].fmmemberindex[i].charflag != FMMEMBER_APPLY))
							// 家族成员个人气势加总
							family[index].fmmomentum += family[index].fmmemberindex[i].momentum;
					}
					// 家族气势 = (家族人数平方) / 10 + 1 + 家族成员个人气势加总
					family[index].fmmomentum += ((family[index].fmjoinnum * family[index].fmjoinnum) / 10 + 1) * 100;
				}
#endif
#ifdef _PERSONAL_FAME	// Arminius: 家族个人声望
				saacproto_ACFMCharLogin_send(
					family[index].fmmemberindex[charindex].onlineflag,
					SUCCESSFUL, index, floor, family[index].fmtotalfame,
	#ifdef _FMVER21
					family[index].fmmemberindex[charindex].charflag, family[index].fmsetupflag, 1, charindex,
	#else   	 
					2, family[index].fmsetupflag, 1, charindex,
	#endif
					family[index].fmmemberindex[charindex].popular,	// Arminius 9.11
					family[index].fmmemberindex[charindex].charfdid
	#ifdef _NEW_MANOR_LAW
					,family[index].fmmemberindex[charindex].momentum
	#endif
					);
#else
				saacproto_ACFMCharLogin_send(
					family[index].fmmemberindex[charindex].onlineflag,
					SUCCESSFUL, index, floor, family[index].fmpopular,
	#ifdef _FMVER21
					family[index].fmmemberindex[charindex].charflag, family[index].fmsetupflag, 1, charindex,
	#else   	 
					2, family[index].fmsetupflag, 1, charindex,
	#endif
					family[index].fmmemberindex[charindex].charfdid);
#endif
			}
			// 更新族长状态
			if (family[index].fmmemberindex[charindex].onlineflag > 0)
			{
#ifdef _PERSONAL_FAME	// Arminius: 家族个人声望
				saacproto_ACFMCharLogin_send(
					family[index].fmmemberindex[0].onlineflag,
					SUCCESSFUL, index, floor, family[index].fmtotalfame,
	#ifdef _FMVER21
					family[index].fmmemberindex[0].charflag, family[index].fmsetupflag, 1, 0,
	#else   	 
					1, family[index].fmsetupflag, 1, 0,
	#endif
					family[index].fmmemberindex[0].popular,	// Arminius 9.11
					family[index].fmmemberindex[0].charfdid
	#ifdef _NEW_MANOR_LAW
					,family[index].fmmemberindex[0].momentum
	#endif
					);
#else
				saacproto_ACFMCharLogin_send(
					family[index].fmmemberindex[0].onlineflag,
					SUCCESSFUL, index, floor, family[index].fmpopular,
	#ifdef _FMVER21
					family[index].fmmemberindex[0].charflag, family[index].fmsetupflag, 1, 0,
	#else   	 
					1, family[index].fmsetupflag, 1, 0,
	#endif
					family[index].fmmemberindex[0].charfdid);
#endif
			}
#ifdef _FMVER21
			family[index].fmmemberindex[charindex].charflag = FMMEMBER_MEMBER;
#else
			family[index].fmmemberindex[charindex].charflag = 1;
#endif
   }
	 db_familyupdate[index] = 1;
	 return 0;
   return -1;
}

// 族长审核成员离开家族
#ifdef _FMVER21
int ACMemberLeaveFM(int index, char *fmname, int fmindex,
	char *charname, int flag, int charindex, int meindex)
#else
int ACMemberLeaveFM(int index, char *fmname, int fmindex,
	char *charname, int flag, int charindex)
#endif	
{
	time_t t1;
	if (CheckFM(&index, fmname, fmindex) < 0)	return -1;
#ifdef _FMVER21
	if (meindex < 0 || meindex > MAX_MEMBERNUM)	return	-1;
#endif
	
#ifdef _FIX_LEADER_ERR			// WON ADD 修正族长问题
	if (charindex == 0 )	return -1;	
#endif
	
	if (strcmp(family[index].fmmemberindex[charindex].charname, charname) == 0)
	{
		if (family[index].fmmemberindex[charindex].charflag <= 0)	return	-1;
		if (family[index].fmmemberindex[charindex].onlineflag > 0)
		{
   	 	char	buf[256];
#ifdef _PERSONAL_FAME	// Arminius: 家族个人声望
   	 	saacproto_ACFMCharLogin_send(
				family[index].fmmemberindex[charindex].onlineflag,
				FAILED, index, family[index].fmpointindex,
				0, -1, family[index].fmsetupflag, 1,
				charindex, 
				family[index].fmmemberindex[charindex].popular,	// Arminius 9.11
				family[index].fmmemberindex[charindex].charfdid
	#ifdef _NEW_MANOR_LAW
				,family[index].fmmemberindex[charindex].momentum
	#endif
				);
#else
   	 	saacproto_ACFMCharLogin_send(
				family[index].fmmemberindex[charindex].onlineflag,
				FAILED, index, family[index].fmpointindex,
				family[index].fmpopular, -1, family[index].fmsetupflag, 1,
				charindex, family[index].fmmemberindex[charindex].charfdid);
#endif
			// 通知玩家
			if (flag == 1)
			{
#ifdef _FMVER21
				sprintf(buf, "%s已经将你踢出家族了！", family[index].fmmemberindex[meindex].charname);
#else
				sprintf(buf, "%s族长已经将你踢出家族了！", family[index].fmname);
#endif
				saacproto_ACFMAnnounce_send(family[index].fmmemberindex[charindex].onlineflag,
					SUCCESSFUL, fmname, fmindex, index, 3, buf,
					family[index].fmmemberindex[charindex].charfdid);
			}
		}
		memset(family[index].fmmemberindex[charindex].charname, 0,
			sizeof(family[index].fmmemberindex[charindex].charname));
		memset(family[index].fmmemberindex[charindex].charid, 0,
			sizeof(family[index].fmmemberindex[charindex].charid));
		family[index].fmmemberindex[charindex].onlineflag = -1;
		family[index].fmmemberindex[charindex].popular = 0;
#ifdef _FMVER21
		family[index].fmmemberindex[charindex].eventflag = 0;
		if (family[index].fmmemberindex[charindex].charflag != FMMEMBER_APPLY)
#else
		if(family[index].fmmemberindex[charindex].charflag != 2)
#endif 
			family[index].fmjoinnum = family[index].fmjoinnum - 1;
		family[index].fmnum = family[index].fmnum - 1;
		family[index].fmmemberindex[charindex].charflag = FMMEMBER_NONE;
#ifdef _PERSONAL_FAME	// Arminius: 家族个人声望
		{
			int j;
			family[index].fmtotalfame=family[index].fmpopular;
			for (j = 0; j < MAX_MEMBERNUM; j++) {
				if ((family[index].fmmemberindex[j].charflag!=FMMEMBER_NONE) &&
					(family[index].fmmemberindex[j].charflag!=FMMEMBER_APPLY) )
					family[index].fmtotalfame += family[index].fmmemberindex[j].popular;
			}
		}
#endif
		if (family[index].fmnum < 1 || family[index].fmjoinnum < 1)
		{
			int i = 0, fmnum1 = 0, fmnum2 = 0;
			for (i = 0; i < MAX_MEMBERNUM; i++)
			{
				if (family[index].fmmemberindex[i].charflag <= 0)	continue;
#ifdef _FMVER21
				if (family[index].fmmemberindex[i].charflag != FMMEMBER_APPLY)
#else
				if (family[index].fmmemberindex[i].charflag == 1)
#endif           
       		fmnum1 = fmnum1 + 1;
				if (family[index].fmmemberindex[i].charflag > 0)
	    		fmnum2 = fmnum2 + 1;
			}
			log("MemberLeave_fmnum err!\n index:%d fmname:%s"
				" orifmnum:%d orifmjoinnum:%d\n fmnum1:%d fmnum2:%d\n",
				index, fmname, family[index].fmnum,
				family[index].fmjoinnum, fmnum1, fmnum2);
			family[index].fmjoinnum = fmnum1;
			family[index].fmnum = fmnum2;
		}
		if (family[index].fmmemberindexmaxnum == charindex) family[index].fmmemberindexmaxnum--;
		if (family[index].fmjoinnum < MIN_MEMBERNUM){
			time(&t1);
			if( family[index].fmsetupflag != 0 ){
				family[index].fmsetupflag = 0;
				family[index].predel_time = t1 + PREDEL_TIME;
			}
			if (family[index].fmmemberindex[charindex].onlineflag > 0 ){
				char token[256];
				sprintf( token, "在七天之内要赶快召集１０名成员，否则家族会被解散喔！剩馀%d天。",
					(int)((family[index].predel_time - t1)/(60*60*24)) );
				saacproto_ACFMAnnounce_send(
       			family[index].fmmemberindex[0].onlineflag,
						SUCCESSFUL, fmname, fmindex, index, 3,
						token, family[index].fmmemberindex[0].charfdid );
			}
		}
		db_familyupdate[index] = 1;
		return	0;
	}
  return -1;
}

// 族长分配职务
int ACFMAssignOcp(int index, char *fmname, int fmindex,
	char *charname, int charindex, int result)
{
#ifdef _FMVER21
   int	floor, count = 0, i = 0;
   char buf[256];
   if (CheckFM(&index, fmname, fmindex) < 0)	return -1;
   floor = ACgetFMFloor(fmindex);
   if (result < 0)	return	-1;
 
#ifdef _FIX_LEADER_ERR			// WON ADD 修正族长问题
   if (charindex == 0 )	return -1;	
#endif  
 
   log("AsssignOcp index:%d fmindex:%d fmname:%s charname:%s charindex:%d result:%d\n",
   	index, fmindex, fmname, charname, charindex, result);
   for (i = 0; i < MAX_MEMBERNUM; i++)
   {
	if (family[index].fmmemberindex[i].charflag == result
		&& result != FMMEMBER_MEMBER)
		count++;
   }
   if (count >= FMELDERNUM)
   {
   	sprintf(buf, "\n只能指派%d位家族成员成为%s喔！",
   		FMELDERNUM, MEMBERKIND_INTDATA[result]);
   	saacproto_ACFMAnnounce_send(family[index].fmmemberindex[0].onlineflag,
   		SUCCESSFUL, fmname, fmindex, index, 4, buf,
   		family[index].fmmemberindex[0].charfdid);
   	return	-1;
   }
   if (strcmp(family[index].fmmemberindex[charindex].charname, charname) == 0)
   {
   	family[index].fmmemberindex[charindex].charflag = result;
        if (family[index].fmmemberindex[charindex].onlineflag > 0)
        {
#ifdef _PERSONAL_FAME	// Arminius: 家族个人声望
        	saacproto_ACFMCharLogin_send(
			   		family[index].fmmemberindex[charindex].onlineflag,
			   		SUCCESSFUL, index, floor, family[index].fmtotalfame,
						family[index].fmmemberindex[charindex].charflag,
						family[index].fmsetupflag, 1, charindex,
						family[index].fmmemberindex[charindex].popular,	// Arminius 9.11
			   		family[index].fmmemberindex[charindex].charfdid
						);
#else
        	saacproto_ACFMCharLogin_send(
   	   			family[index].fmmemberindex[charindex].onlineflag,
   	   			SUCCESSFUL, index, floor, family[index].fmpopular,
						family[index].fmmemberindex[charindex].charflag,
						family[index].fmsetupflag, 1, charindex,
   					family[index].fmmemberindex[charindex].charfdid);
#endif
   	   	// 通知玩家
   	   	sprintf(buf, "%s族长已经将你的家族职等改为%s！", family[index].fmname,
   	   		MEMBERKIND_INTDATA[result]);
   	   	saacproto_ACFMAnnounce_send(family[index].fmmemberindex[charindex].onlineflag,
   	   		SUCCESSFUL, fmname, fmindex, index, 3, buf,
   	   		family[index].fmmemberindex[charindex].charfdid);
	}
        if (family[index].fmmemberindex[0].onlineflag > 0)
        {
   	   	// 通知族长
   	   	sprintf(buf, "\n你已经将%s的职等改为%s了！", charname,
   	   		MEMBERKIND_INTDATA[result]);
   	   	saacproto_ACFMAnnounce_send(family[index].fmmemberindex[0].onlineflag,
   	   		SUCCESSFUL, fmname, fmindex, index, 4, buf,
   	   		family[index].fmmemberindex[0].charfdid);
   	}
        db_familyupdate[index] = 1;
        return 0;
   }
   log("AssignOcp Failed\n");
#endif
   return -1;
}

// 家族成员 login
#ifdef _PERSONAL_FAME   // Arminius: 家族个人声望
int ACFMCharLogin(int fd, int index, char *fmname, int fmindex,
	char *charname, char *charid, int charlv, int *floor, int *fmpopular, 
	int *joinflag, int *fmsetupflag, int *charindex, int charfdid,
	int *charfame, int eventflag)
#else
int ACFMCharLogin(int fd, int index, char *fmname, int fmindex,
	char *charname, char *charid, int charlv, int *floor, int *fmpopular, 
	int *joinflag, int *fmsetupflag, int *charindex, int charfdid)
#endif
{
	int i = 0;
	time_t t1;
   
	if (CheckFM(&index, fmname, fmindex) < 0)	return -1;
   
	if (family[index].fmpointindex != -1)
		*floor = ACgetFMFloor(fmindex);
#ifdef _PERSONAL_FAME	// Arminius: 家族个人声望
	*fmpopular = family[index].fmtotalfame;
#else
	*fmpopular = family[index].fmpopular;
#endif
	*fmsetupflag = family[index].fmsetupflag;
   
	for (i = 0; i < family[index].fmmemberindexmaxnum; i++){
		if ((strcmp(family[index].fmmemberindex[i].charname, charname) == 0)
				&& (strcmp(family[index].fmmemberindex[i].charid, charid) == 0)){
			*charindex = i;
			family[index].fmmemberindex[i].onlineflag = fd;
			family[index].fmmemberindex[i].charfdid = charfdid;
			family[index].fmmemberindex[i].charlv = charlv;
#ifdef _PERSONAL_FAME   // Arminius: 家族个人声望
			*charfame = family[index].fmmemberindex[i].popular;
#endif
			time(&t1);
			family[index].fmmemberindex[i].predeltime = t1 + PREDELMEMBER_TIME;
#ifdef _FMVER21
			*joinflag = family[index].fmmemberindex[i].charflag;
			family[index].fmmemberindex[i].eventflag = eventflag;
#else
			if (i == 0)  *joinflag = 1;
			else if ((family[index].fmmemberindex[i].charflag == 1)
					|| (family[index].fmmemberindex[i].charflag == 3))
				*joinflag = 2;
			else *joinflag = 0;
#endif
			db_familyupdate[index] = 1;
   
			return index;
		}
	}
	return -1;
}

// 家族成员离线
int ACFMCharLogout(int index, char *fmname, int fmindex, char *charname,
        char *charid, int charlv, int charfdid)
{
   int i = 0;
   time_t t1;
   if (CheckFM(&index, fmname, fmindex) < 0)	return -1;
   
   for (i = 0; i < family[index].fmmemberindexmaxnum; i++)
   {
   	if ((strcmp(family[index].fmmemberindex[i].charname, charname) == 0)
   	   && (strcmp(family[index].fmmemberindex[i].charid, charid) == 0))
   	{
   	   family[index].fmmemberindex[i].onlineflag = 0;
   	   family[index].fmmemberindex[i].charfdid = 0;
   	   family[index].fmmemberindex[i].charlv = charlv;
   	   time(&t1);
   	   family[index].fmmemberindex[i].predeltime = t1 + PREDELMEMBER_TIME;
	   db_familyupdate[index] = 1;
	   return 0;
	}
   }
   return -1;
}

// 取得家族庄园图层
int ACgetFMFloor(fmindex)
{
   int i = 0;
   for (i = 0; i < MAX_FMPOINT; i++)
   {
      if (fmindex == fmpoint[i].hadfmindex)
      return	fmpoint[i].fmfl;
   }
   return	-1;
}

// 玩家手动删除角色
int ACgetFMInfoFromChar(char *fmname, int *fmindex,
	char *charname, char *charid, int *charindex)
{
   int i = 0, j = 0;
   for (i = 0; i < MAX_FAMILY; i++)
   {
      if (family[i].fmindex <= 0)	continue;
      for (j = 0; j < MAX_MEMBERNUM; j++)
      {
         if (family[i].fmmemberindex[j].charflag <= 0)	continue;
         if ((strcmp(family[i].fmmemberindex[j].charname, charname) == 0)
            && (strcmp(family[i].fmmemberindex[j].charid, charid) == 0))
         {
               strcpy(fmname, "");
               strcpy(fmname, family[i].fmname);
               *fmindex = family[i].fmindex;
               *charindex = j;
               db_familyupdate[i] = 1;
               return	i;
         }
      } // for
   } // for
   return -1;
}

// 阅读家族留言
int ACFMReadMemo(int index, int *dataindex, char *data)
{
   int i = 0;
   char buf[256];
   strcpy(data, "");
   if (index == -1){
		return	-1;
   } else if (index == FMSMEMOINDEX){
   	*dataindex = fmsmemo.fmsnowwritenum;
   	for (i = 0; i < fmsmemo.num; i++){
	   sprintf(buf, "%s|", fmsmemo.memodata[i]);
   	   strcat(data, buf);
   	}
   	return	fmsmemo.num;
   }
   if ((index >= MAX_FAMILY) || (index < 0))
   	return	-1;
   if (CheckFMUse(index) == 0)
   	return	-1;
   *dataindex = family[index].memonowwritenum;
   for (i = 0; i < family[index].memonum; i++){
   	strcpy(buf, "");
   	sprintf(buf, "%s|", family[index].memolist[i]);
   	strcat(data, buf);
   }
   db_familyupdate[index] = 1;
   return	family[index].memonum;
}

// 写入家族留言板
int ACFMWriteMemo(int index, char *fmname, int fmindex, char *data)
{
   if (index == -1){
   	return	-1;
   }else if (index == FMSMEMOINDEX){
   	fmsmemo.num++;
   	if (fmsmemo.num > FMSMEMONUM)	fmsmemo.num = FMSMEMONUM;
   	strcpy(fmsmemo.memodata[fmsmemo.fmsnowwritenum], data);
   	fmsmemo.fmsnowwritenum++;
   	if (fmsmemo.fmsnowwritenum >= FMSMEMONUM)	fmsmemo.fmsnowwritenum = 0;
   	db_fmsmemoupdate = 1;
   	return	0;
   }
   if ((index > MAX_FAMILY) || (index < 0))
   	return	-1;
   if (CheckFMUse(index) == 0)
   	return	-1;
   if ((family[index].fmindex != fmindex) || (strcmp(family[index].fmname, fmname) != 0)) {
   	return	-1;
   }
   if (strlen(data) > FAMILY_MEMOLEN)	return	-1;
   family[index].memonum++;
   if (family[index].memonum > FAMILY_MEMONUM)	family[index].memonum = FAMILY_MEMONUM;
   strcpy(family[index].memolist[family[index].memonowwritenum], data);
   family[index].memonowwritenum++;
   if (family[index].memonowwritenum >= FAMILY_MEMONUM)	family[index].memonowwritenum = 0;
   db_familyupdate[index] = 1;
   return	0;
}

// 取得庄园列表
int ACFMPointList(char *data)
{
   int i = 0;
   char msg[256];
   strcpy(data, "");
   for (i = 0; i < MAX_FMPOINT; i++)
   {
      sprintf(msg, "%d|%d|%d|%d|%d|%s|%d|%d|%d ",
         i, fmpoint[i].fl, fmpoint[i].x, fmpoint[i].y,
         fmpoint[i].hadfmindex, fmpoint[i].hadfmname,
         fmpoint[i].hadfmpopular, fmpoint[i].hadfmindex - 1,
         fmpoint[i].village);
      strcat(data, msg);
   }
   return	0;
}

// 申请庄园
int ACSetFMPoint(int index, char *fmname, int fmindex, int fmpointindex,
	int fl, int x, int y)
{
	int i = 0;
	if (CheckFM(&index, fmname, fmindex) < 0)	return -1;
	if (family[index].fmpointindex >= 0) return	-2;
	if ((fmpoint[fmpointindex].x != x) || (fmpoint[fmpointindex].y != y))return	-1;
	if (fmpoint[fmpointindex].hadfmindex > 0)return	-4;
	if (family[index].fmjoinnum < MINAPPLYPNUM)return	-5;
	fmpoint[fmpointindex].hadfmindex = fmindex;
	family[index].fmpointindex = fmpointindex;
	strcpy(fmpoint[fmpointindex].hadfmname, fmname);
#ifdef _PERSONAL_FAME	// Arminius: 家族个人声望
	fmpoint[fmpointindex].hadfmpopular = family[index].fmtotalfame;
#else
	fmpoint[fmpointindex].hadfmpopular = family[index].fmpopular;
#endif
	// 更新线上家族成员状况
	for (i = 0; i < MAX_MEMBERNUM; i++){
		if (strcmp(family[index].fmmemberindex[i].charname, "") == 0)	continue;
		if (family[index].fmmemberindex[i].onlineflag > 0){
			int floor =  ACgetFMFloor(fmindex);
#ifdef _FMVER21
#ifdef _PERSONAL_FAME	// Arminius: 家族个人声望
   	 	saacproto_ACFMCharLogin_send(
				family[index].fmmemberindex[i].onlineflag,
				SUCCESSFUL, index, floor, family[index].fmtotalfame,
				family[index].fmmemberindex[i].charflag, family[index].fmsetupflag, 1, i,
				family[index].fmmemberindex[i].popular,	// Arminuis 9.11
				family[index].fmmemberindex[i].charfdid
				);
#else
   	 	saacproto_ACFMCharLogin_send(
				family[index].fmmemberindex[i].onlineflag,
				SUCCESSFUL, index, floor, family[index].fmpopular,
				family[index].fmmemberindex[i].charflag, family[index].fmsetupflag, 1, i,
				family[index].fmmemberindex[i].charfdid);
#endif
#else   	 
			{
				int joinflag = 0;
				if (i == 0)	joinflag = 1;
				else	joinflag = 2;
				saacproto_ACFMCharLogin_send(
					family[index].fmmemberindex[i].onlineflag,
					SUCCESSFUL, index, floor, family[index].fmpopular,
					joinflag, family[index].fmsetupflag, 1, i,
					family[index].fmmemberindex[i].charfdid);
			}
#endif
		}
	}
  // 更新庄园状况
  {
		char data[15000];
    if (ACFMPointList(data) >= 0){
			for (i = 0; i < MAXCONNECTION; i++)
				if (gs[i].use && gs[i].name[0])
					saacproto_ACFMPointList_send(i, SUCCESSFUL, data);
		}
	}
	
  db_fmpointupdate = 1;
  return	0;
}

// 设定庄园
int ACFixFMPoint(int winindex, char *winfmname, int winfmindex, int loseindex,
	char* losefmname, int losefmindex, int village)
{
	int i = 0;
	if (CheckFM(&winindex, winfmname, winfmindex) < 0)	return -1;
	if (CheckFM(&loseindex, losefmname, losefmindex) < 0)	return -1;
	delfmpointindex(losefmindex);
	family[loseindex].fmpointindex = -1;
	fmpoint[village -1].hadfmindex = winfmindex;
	family[winindex].fmpointindex = village - 1;
	strcpy(fmpoint[village - 1].hadfmname, winfmname);
#ifdef _PERSONAL_FAME	// Arminius: 家族个人声望
	fmpoint[village -1].hadfmpopular = family[winindex].fmtotalfame;
#else
	fmpoint[village -1].hadfmpopular = family[winindex].fmpopular;
#endif
	// 更新线上两个家族成员资料
	for (i = 0; i < MAX_MEMBERNUM; i++){
		if (strcmp(family[winindex].fmmemberindex[i].charname, "") == 0)	continue;
		if (family[winindex].fmmemberindex[i].onlineflag > 0){
			int	floor =  ACgetFMFloor(winfmindex);
#ifdef _FMVER21
	#ifdef _PERSONAL_FAME	// Arminius: 家族个人声望
			saacproto_ACFMCharLogin_send(
					family[winindex].fmmemberindex[i].onlineflag,
					SUCCESSFUL, winindex, floor, family[winindex].fmtotalfame,
					family[winindex].fmmemberindex[i].charflag, family[winindex].fmsetupflag, 1, i,
					family[winindex].fmmemberindex[i].popular,	// Arminius 9.11
					family[winindex].fmmemberindex[i].charfdid
			);
	#else
			saacproto_ACFMCharLogin_send(
					family[winindex].fmmemberindex[i].onlineflag,
					SUCCESSFUL, winindex, floor, family[winindex].fmpopular,
					family[winindex].fmmemberindex[i].charflag, family[winindex].fmsetupflag, 1, i,
					family[winindex].fmmemberindex[i].charfdid);
	#endif
#else 	 
			{
				int joinflag = 0;
				if (i == 0)	joinflag = 1;
				else	joinflag = 2;
				saacproto_ACFMCharLogin_send(
					family[winindex].fmmemberindex[i].onlineflag,
					SUCCESSFUL, winindex, floor, family[winindex].fmpopular,
					joinflag, family[winindex].fmsetupflag, 1, i,
					family[winindex].fmmemberindex[i].charfdid);
			}
#endif
		}
	}
		for (i = 0; i < MAX_MEMBERNUM; i++){
			if (strcmp(family[loseindex].fmmemberindex[i].charname, "") == 0)	continue;
			if (family[loseindex].fmmemberindex[i].onlineflag > 0){
#ifdef _FMVER21
	#ifdef _PERSONAL_FAME	// Arminius: 家族个人声望
				saacproto_ACFMCharLogin_send(
					family[loseindex].fmmemberindex[i].onlineflag,
					SUCCESSFUL, loseindex, -1, family[loseindex].fmtotalfame,
					family[loseindex].fmmemberindex[i].charflag, family[loseindex].fmsetupflag, 1, i,
					family[loseindex].fmmemberindex[i].popular,	// Arminius 9.11
					family[loseindex].fmmemberindex[i].charfdid
				);
	#else
				saacproto_ACFMCharLogin_send(
					family[loseindex].fmmemberindex[i].onlineflag,
					SUCCESSFUL, loseindex, -1, family[loseindex].fmpopular,
					family[loseindex].fmmemberindex[i].charflag, family[loseindex].fmsetupflag, 1, i,
					family[loseindex].fmmemberindex[i].charfdid);
	#endif
#else   	 
				{
					int joinflag = 0;
					if (i == 0)	joinflag = 1;
					else	joinflag = 2;
					saacproto_ACFMCharLogin_send(
						family[loseindex].fmmemberindex[i].onlineflag,
						SUCCESSFUL, loseindex, -1, family[loseindex].fmpopular,
						joinflag, family[loseindex].fmsetupflag, 1, i,
						family[loseindex].fmmemberindex[i].charfdid);
				}
#endif
			}
		}
	// 更新庄园状况
	{
		char data[15000];
		if (ACFMPointList(data) >= 0){
			for (i = 0; i < MAXCONNECTION; i++)
				if (gs[i].use && gs[i].name[0])
				saacproto_ACFMPointList_send(i, SUCCESSFUL, data);
		}
	}
	db_fmpointupdate = 1;
	return	0;
}

// 族长广播
int ACFMAnnounce(char *fmname, int fmindex, int index, char *data, int color)
{
   if (CheckFM(&index, fmname, fmindex) < 0)	return -1;
   return	0;
}

// 取得家族资料
// kindflag 1:家族银行
int ACGetFMData(int index, char *fmname, int fmindex, int kindflag, int *data)
{
   if (CheckFM(&index, fmname, fmindex) < 0)	return -1;

   if (kindflag == 1)
   	*data = family[index].fmgold;
   return	0;
}

void setMemberFileDataToArg(int index, int memberindex, char *data)
{
   int i = 0;
   char value[256];
   char *ptr;
   if (index < 0 || index > MAX_FAMILY)	return;
   if (strcmp(data, "")	== 0)	return;
   ptr = (char *)strstr(data, "=");
   if (ptr == NULL)	return;
   strcpy(ptr, "");
   strcpy(value, ptr + 1);
   for (i = 0; i < memberdatamaxint; i++){
      if (strcmp(data, MEMBER_INTDATA[i]) == 0){
         switch(i){
            case 0:
            	family[index].fmmemberindex[memberindex].charlv = atoi(value);
            	break;
            case 1:
            	family[index].fmmemberindex[memberindex].charflag = atoi(value);
            	break;
            case 2:
            	family[index].fmmemberindex[memberindex].onlineflag = atoi(value);
            	break;
            case 3:
            	family[index].fmmemberindex[memberindex].charfdid = atoi(value);
            	break;
            case 4:
            	family[index].fmmemberindex[memberindex].predeltime = atoi(value);
            	break;
            case 5:
            	family[index].fmmemberindex[memberindex].popular = atoi(value);
            	break;
#ifdef _FMVER21
						case 6:
            	family[index].fmmemberindex[memberindex].eventflag = atoi(value);
            	break;
#endif
            default:
            	break;
         }
         return;
      }
   }
   for (i = 0; i < memberdatamaxchar; i++){
      if (strcmp(data, MEMBER_CHARDATA[i]) == 0){
         switch(i){
            case 0:
            	strcpy(family[index].fmmemberindex[memberindex].charname, value);
            	break;
            case 1:
            	strcpy(family[index].fmmemberindex[memberindex].charid, value);
            	break;
            default:
            	break;
         }
         return;
      }
   }
}

void setFamilyFileDataToArg(int index, char *data)
{
   int i = 0, j = 0;
   char value[1024];
   char *ptr;
   if (index < 0 || index > MAX_FAMILY)	return;
   if (strcmp(data, "")	== 0)	return;
   ptr = (char *)strstr(data, "=");
   if (ptr == NULL)	return;
   strcpy(ptr, "");
   strcpy(value, ptr + 1);
   for (i = 0; i < familymaxint; i++){
      if (strcmp(data, FAMILY_INTDATA[i]) == 0){
      	 switch(i){
      	    case 0:
      	    	family[index].fmindex = atoi(value);
      	    	break;
      	    case 1:
      	    	family[index].fmleadergrano = atoi(value);
      	    	break;
      	    case 2:
      	    	family[index].fmnum = atoi(value);
      	    	break;
      	    case 3:
      	    	family[index].fmjoinnum = atoi(value);
      	    	break;
      	    case 4:
      	    	family[index].fmacceptflag = atoi(value);
      	    	break;
      	    case 5:
      	    	family[index].fmsetupflag = atoi(value);
      	    	break;
      	    case 6:
      	    	family[index].fmsprite = atoi(value);
      	    	break;
      	    case 7:
      	    	family[index].fmpointindex = atoi(value);
      	    	break;
      	    case 8:
      	    	family[index].fmpopular = atoi(value);
      	    	break;
      	    case 9:
      	    	family[index].fmgold = atoi(value);
      	    	break;
      	    case 10:
      	    	family[index].fmmemberindexmaxnum = atoi(value);
      	    	break;
      	    case 11:
      	    	family[index].apply_time = atoi(value);
      	    	break;
      	    case 12:
      	    	family[index].setup_time = atoi(value);
      	    	break;
      	    case 13:
      	    	family[index].predel_time = atoi(value);
      	    	break;
      	    case 14:
      	    	family[index].memonowwritenum = atoi(value);
      	    	break;
      	    case 15:
      	    	family[index].memonum = atoi(value);
      	    	break;
      	    case 16:
      	    	family[index].fmadv = atoi(value);
      	    	break;
      	    case 17:
      	    	family[index].fmfeed = atoi(value);
      	    	break;
      	    case 18:
      	    	family[index].fmsynthesize = atoi(value);
      	    	break;
      	    case 19:
      	    	family[index].fmdealfood = atoi(value);
      	    	break;
      	    case 20:
      	    	family[index].fmpk = atoi(value);
      	    	break;
      	    default:
      	    	break;
      	 }
      	 return;
      }
   }
   for (i = 0; i < familymaxchar; i++){
      if (strcmp(data, FAMILY_CHARDATA[i]) == 0){
      	 switch(i){
      	    case 0:
      	    	strcpy(family[index].fmname, value);
      	    	break;
      	    case 1:
      	    	strcpy(family[index].fmleadername, value);
      	    	break;
      	    case 2:
      	    	strcpy(family[index].fmleaderid, value);
      	    	break;
      	    case 3:
      	    	strcpy(family[index].petname, value);
      	    	break;
      	    case 4:
      	    	strcpy(family[index].petattr, value);
      	    	break;
      	    case 5:
      	    	strcpy(family[index].fmrule, value);
      	    	break;
      	    default:
      	    	break;
      	 }
      	 return;
      }
   }
   for (i = 0; i < FAMILY_MEMONUM; i ++){
      char memolist[256];
      sprintf(memolist, "%s%d", FMMEMOLIST, i);
      if (strcmp(data, memolist) == 0){
      	 strcpy(family[index].memolist[i], value);
      	 return;
      }
   }
   for (i = 0; i < MAX_MEMBERNUM; i ++){
      char memberdata[256];
      sprintf(memberdata, "%s%d", FMMEMBERINDEX, i);
      if (strcmp(data, memberdata) == 0){
	 for (j = 1; j < memberdatamaxint + memberdatamaxchar; j++){
	    char databuf[256];
	    easyGetTokenFromBuf(value, " ", j, databuf, sizeof(databuf));
	    if (strcmp(databuf, "") == 0)	continue;
	    setMemberFileDataToArg(index, i, databuf);
	 }
	 return;
      }
   }
   
}

// GM修改家族资料
int ACGMFixFMData(int index, char *fmname, char *charid, char *cmd,
	char *data)
{
   int i = 0,charindex = -1;
	 int	floor =  ACgetFMFloor(index);
   if (index < 0 || index > MAX_FAMILY)	return	-1;
   log("GMFixFMData index:%d charid:%s cmd:%s data:%s\n",
   	index, charid, cmd, data);
   strcpy(fmname, family[index].fmname);
   if (charid[0] == '-' && charid[1] == '1')
   {
      for (i = 0; i < familymaxint; i++)
      {
         if (strcmp(cmd, FAMILY_INTDATA[i]) == 0)
         {
            switch(i)
            {
               case 2:
                  family[index].fmnum = atoi(data);
                  log("fmnum:%s\n", data);
                  break;
               case 3:
               	  family[index].fmjoinnum = atoi(data);
               	  log("fmjoinnum:%s\n", data);
               	  break;
               case 4:
               	  family[index].fmacceptflag = atoi(data);
               	  log("fmacceptflag:%s\n", data);
               	  break;
               case 5:
                  family[index].fmsetupflag = atoi(data);
                  log("fmsetupflag:%s\n", data);
                  break;
               case 6:
                  family[index].fmsprite = atoi(data);
                  log("fmsprite:%s\n", data);
                  break;
               case 7:
                  family[index].fmpointindex = atoi(data);
                  log("fmpointindex:%s\n", data);
                  break;
               case 8:
                  family[index].fmpopular = atoi(data);
                  log("fmpopular:%s\n", data);
                  break;
               case 9:
                  family[index].fmgold = atoi(data);
                  log("fmgold:%s\n", data);
                  break;
               case 11:
                  family[index].apply_time = atoi(data);
                  log("apply_time:%s\n", data);
                  break;
               case 12:
                  family[index].setup_time = atoi(data);
                  log("setup_time:%s\n", data);
                  break;
               case 13:
                  family[index].predel_time = atoi(data);
                  log("predel_time:%s\n", data);
                  break;
               case 16:
                  family[index].fmadv = atoi(data);
                  log("adv:%s\n", data);
                  break;
               case 17:
                  family[index].fmfeed = atoi(data);
                  log("feed:%s\n", data);
                  break;
               case 18:
                  family[index].fmsynthesize = atoi(data);
                  log("synthesize:%s\n", data);
                  break;
               case 19:
                  family[index].fmdealfood = atoi(data);
                  log("dealfood:%s\n", data);
                  break;
               case 20:
                  family[index].fmpk = atoi(data);
                  log("pk:%s\n", data);
                  break;
               default:
                  break;
            }
            db_familyupdate[index] = 1;
						// 计算家族声望
						family[index].fmpopular = family[index].fmadv + family[index].fmfeed
							+ family[index].fmsynthesize + family[index].fmdealfood
							+ family[index].fmpk;
						family[index].fmtotalfame = family[index].fmpopular;
						for(i = 0;i < MAX_MEMBERNUM;i++){
							if((family[index].fmmemberindex[i].charflag != FMMEMBER_NONE) &&
								(family[index].fmmemberindex[i].charflag != FMMEMBER_APPLY))
								family[index].fmtotalfame += family[index].fmmemberindex[i].popular;
						}
#ifdef _PERSONAL_FAME	// Arminius: 家族个人声望
						saacproto_ACFMCharLogin_send(
							family[index].fmmemberindex[0].onlineflag,
							SUCCESSFUL,index,floor,family[index].fmtotalfame,
							family[index].fmmemberindex[0].charflag,
							family[index].fmsetupflag, 1, 0,
							family[index].fmmemberindex[0].popular,
							family[index].fmmemberindex[0].charfdid
							);
#else
						saacproto_ACFMCharLogin_send(
							family[index].fmmemberindex[0].onlineflag,
							SUCCESSFUL, index, floor, family[index].fmpopular,
							family[index].fmmemberindex[0].charflag,
							family[index].fmsetupflag, 1, 0,
							family[index].fmmemberindex[0].charfdid);
#endif
            return	0;
         }
      }
      for (i = 0; i < familymaxchar; i++)
      {
         if (strcmp(cmd, FAMILY_CHARDATA[i]) == 0)
         {
            switch(i)
            {
               case 0:
                  strncpy(family[index].fmname,data,sizeof(family[index].fmname));
                  log("fmname:%s\n", data);
                  break;
               case 1:
                  strncpy(family[index].fmleadername, data,sizeof(family[index].fmleadername));
                  log("fmleadername:%s\n", data);
                  break;
               case 2:
                  strncpy(family[index].fmleaderid, data,sizeof(family[index].fmleaderid));
                  log("fmleaderid:%s\n", data);
                  break;
               case 3:
                  strncpy(family[index].petname, data,sizeof(family[index].petname));
                  log("petname:%s\n", data);
                  break;
               case 4:
                  strncpy(family[index].petattr, data,sizeof(family[index].petattr));
                  log("petattr:%s\n", data);
                  break;
               case 5:
                  strncpy(family[index].fmrule, data,sizeof(family[index].fmrule));
                  log("fmrule:%s\n", data);
                  break;
               default:
                  break;
            }
            db_familyupdate[index] = 1; 
            sortFamily();
#ifdef _PERSONAL_FAME	// Arminius: 家族个人声望
						saacproto_ACFMCharLogin_send(
							family[index].fmmemberindex[0].onlineflag,
							SUCCESSFUL,index,floor,family[index].fmtotalfame,
							family[index].fmmemberindex[0].charflag,
							family[index].fmsetupflag, 1, 0,
							family[index].fmmemberindex[0].popular,
							family[index].fmmemberindex[0].charfdid
							);
#else
						saacproto_ACFMCharLogin_send(
							family[index].fmmemberindex[0].onlineflag,
							SUCCESSFUL, index, floor, family[index].fmpopular,
							family[index].fmmemberindex[0].charflag,
							family[index].fmsetupflag, 1, 0,
							family[index].fmmemberindex[0].charfdid);
#endif
            return	0;
         }
      }
   }
   else{
		 for(i=0;i<MAX_MEMBERNUM;i++){
			 if(strcmp(family[index].fmmemberindex[i].charid,charid) == 0 && family[index].fmmemberindex[i].onlineflag > 0){
				 charindex = i;
				 break;
			 }
		 }
		 // 找不到家族成员
		 if(charindex == -1){
			 log("FMName:%s,id:%s\n",fmname,charid);
			 return -1;
		 }
		 for(i = 0; i < memberdatamaxint; i++){
			 if (strcmp(cmd, MEMBER_INTDATA[i]) == 0)
			 {
				 switch(i)
				 {
				 case 0:
					 family[index].fmmemberindex[charindex].charlv = atoi(data);
					 log("charlv:%s\n", data);
					 break;
				 case 1:
					 family[index].fmmemberindex[charindex].charflag = atoi(data);
					 log("charflag:%s\n", data);
					 break;
				 case 2:
					 family[index].fmmemberindex[charindex].onlineflag = atoi(data);
					 log("onlineflag:%s\n", data);
					 break;
				 case 3:
					 family[index].fmmemberindex[charindex].charfdid = atoi(data);
					 log("charfdid:%s\n", data);
					 break;
				 case 4:
					 family[index].fmmemberindex[charindex].predeltime = atoi(data);
					 log("predeltime:%s\n", data);
					 break;
				 case 5:
					 family[index].fmmemberindex[charindex].popular = atoi(data);
					 log("popular:%s\n", data);
					 // 计算家族声望
					 family[index].fmpopular = family[index].fmadv + family[index].fmfeed
						 + family[index].fmsynthesize + family[index].fmdealfood
						 + family[index].fmpk;
					 family[index].fmtotalfame = family[index].fmpopular;
					 for(i = 0;i < MAX_MEMBERNUM;i++){
						 if((family[index].fmmemberindex[i].charflag != FMMEMBER_NONE) &&
							 (family[index].fmmemberindex[i].charflag != FMMEMBER_APPLY))
							 family[index].fmtotalfame += family[index].fmmemberindex[i].popular;
					 }
					 break;
				 case 6:
					 family[index].fmmemberindex[charindex].eventflag = atoi(data);
					 log("eventflag:%s\n", data);
					 break;
				 default:
					 break;
				 }
				 db_familyupdate[index] = 1; 
#ifdef _PERSONAL_FAME	// Arminius: 家族个人声望
				 saacproto_ACFMCharLogin_send(
					 family[index].fmmemberindex[charindex].onlineflag,
					 SUCCESSFUL,index,floor,family[index].fmtotalfame,
					 family[index].fmmemberindex[charindex].charflag,
					 family[index].fmsetupflag, 1, 0,
					 family[index].fmmemberindex[charindex].popular,
					 family[index].fmmemberindex[charindex].charfdid
					 );
#else
				 saacproto_ACFMCharLogin_send(
					 family[index].fmmemberindex[charindex].onlineflag,
					 SUCCESSFUL, index, floor, family[index].fmpopular,
					 family[index].fmmemberindex[charindex].charflag,
					 family[index].fmsetupflag, 1, 0,
					 family[index].fmmemberindex[charindex].charfdid);
#endif
				 return	0;
			 }
		 }
		 for (i = 0; i < memberdatamaxchar; i++)
		 {
			 if (strcmp(cmd, MEMBER_CHARDATA[i]) == 0)
			 {
				 switch(i)
				 {
				 case 0:
					 strncpy(family[index].fmmemberindex[charindex].charname, data,sizeof(family[index].fmmemberindex[charindex].charname));
					 log("charname:%s\n", data);
					 break;
				 case 1:
					 strncpy(family[index].fmmemberindex[charindex].charid, data,sizeof(family[index].fmmemberindex[charindex].charid));
					 log("charid:%s\n", data);
					 break;
				 default:
					 break;
				 }
				 db_familyupdate[index] = 1; 
				 sortFamily();
#ifdef _PERSONAL_FAME	// Arminius: 家族个人声望
				 saacproto_ACFMCharLogin_send(
					 family[index].fmmemberindex[charindex].onlineflag,
					 SUCCESSFUL,index,floor,family[index].fmtotalfame,
					 family[index].fmmemberindex[charindex].charflag,
					 family[index].fmsetupflag, 1, 0,
					 family[index].fmmemberindex[charindex].popular,
					 family[index].fmmemberindex[charindex].charfdid
					 );
#else
				 saacproto_ACFMCharLogin_send(
					 family[index].fmmemberindex[charindex].onlineflag,
					 SUCCESSFUL, index, floor, family[index].fmpopular,
					 family[index].fmmemberindex[charindex].charflag,
					 family[index].fmsetupflag, 1, 0,
					 family[index].fmmemberindex[charindex].charfdid);
#endif
				 return	0;
			 }
		 }
   }
   return	-1;
}

int ChangeFMLeader(int index, char *fmname, int fmindex)
{
   int i = 0;
   int tmpindex = 0;
   char buf[256];
   if (CheckFM(&index, fmname, fmindex) < 0)	return	-1;
   for (i = 0; i < MAX_MEMBERNUM; i++)
   {
   	if (family[index].fmmemberindex[i].charflag == FMMEMBER_ELDER)
   	{
   	   if ((strcmp(family[index].fmmemberindex[i].charname, "") == 0)
   	      || (strcmp(family[index].fmmemberindex[i].charid, "") == 0))
   	         continue;
   	   if (tmpindex == 0)	tmpindex = i;
   	   else
   	   {
   	   	if (family[index].fmmemberindex[i].popular > 
   	   		family[index].fmmemberindex[tmpindex].popular)
   	   			tmpindex = i;
   	   }
   	}
   }
   log("ChangeFMLeader_1 tmpindex:%d\n", tmpindex);
   if (tmpindex == 0)	return -1;   // shan return 0 -> -1
   sprintf(buf, "%d", family[index].fmleadergrano);
   if (ACFixFMData(index, fmname, fmindex, 11, tmpindex, buf, 
	   family[index].fmmemberindex[tmpindex].charname) < 0)
   		return	-1;
   log("ChangeFMLeader_2 tmpindex:%d\n", tmpindex);
   if (family[index].fmmemberindex[tmpindex].onlineflag > 0)
   {
      char tmpbuf[256];
      sprintf(tmpbuf, "您已经退出家族了～\n族长职位已让给%s，辛苦你了！",
      	 family[index].fmmemberindex[0].charname);
      saacproto_ACFMAnnounce_send(
      	 family[index].fmmemberindex[tmpindex].onlineflag,
      	 SUCCESSFUL, fmname, fmindex, index, 4, tmpbuf,
      	 family[index].fmmemberindex[tmpindex].charfdid);
   }
   ACMemberLeaveFM(index, fmname, fmindex,
   	family[index].fmmemberindex[tmpindex].charname, 0, tmpindex, 0);
   return	0;
}

#ifdef _LEADERFUNCHECK
int CheckLeaderQ(int index, char *fmname, int fmindex, char *charname, char *charid)
{
	int result = -1;
   if(CheckFM(&index, fmname, fmindex) < 0)	return -1;
  	if ((strcmp(family[index].fmmemberindex[0].charname, charname) == 0)
  	   && (strcmp(family[index].fmmemberindex[0].charid, charid) == 0))
			result = 0;
	return result;
}
#endif

#ifdef _ACFMPK_LIST
FamilyPKSchedule FMPK_List[FMPKLIST_MAXNUM];
void FMPK_InitList( void)
{
	int i;
	for( i=0; i<FMPKLIST_MAXNUM; i++){
		FMPK_List[i].pkflg = 0;
		memset( FMPK_List[i].Data, 0, sizeof(FMPK_List[i].Data) );
	}
}
void FMPK_LoadList()
{
	char buf[256];
	int i;
	char *filename="data/family/fm_pk_list.txt";
	FILE *fp;

	FMPK_InitList();
	if( !(fp=fopen(filename, "r")))return;

	for( i=0; i<FMPKLIST_MAXNUM; i++){
		if( fscanf( fp, "%s", buf) == EOF ) break;
		buf[strlen(buf)+1] = 0;
		memcpy( FMPK_List[i].Data, buf, strlen(buf)+1);
		log("庄园PK列表[%d]:%d,%s\n", i, FMPK_List[i].pkflg, FMPK_List[i].Data);
	}
	fclose( fp);
}
void FMPK_BackUpList()
{
	int i;
	char *filename="data/family/fm_pk_list.txt";
	FILE *fp;	
	if( !(fp=fopen(filename, "w"))  )return;
	for( i=0; i<FMPKLIST_MAXNUM; i++){	
		fprintf(fp, "%s\n", FMPK_List[i].Data);
		printf("\n%s",FMPK_List[i].Data);
	}
	fclose( fp);
}
int FMPK_GetTypeFlg( int ti)
{
	if( ti <0 || ti >= FMPKLIST_MAXNUM ) return -1;
	return FMPK_List[ti].pkflg;
}
char *FMPK_GetData( int ti)
{
	if( ti <0 || ti >= FMPKLIST_MAXNUM ) return NULL;
	return FMPK_List[ti].Data;
}
int FMPK_SetData( int ti, int Pkflg, char *Data, int sizes)
{
	if( ti <0 || ti >= FMPKLIST_MAXNUM ) return 0;
	memcpy( FMPK_List[ti].Data, Data, sizes+1);
	FMPK_List[ti].pkflg = Pkflg;
	return 1;
}

#endif

int readOneFamilyFromTi( int ti)
{
	char filename[256];
	struct stat s;

	snprintf(filename, sizeof(filename), "%s/Family.%d", familydir, ti);

	if (stat(filename, &s) < 0)return 0;
	if (!(s.st_mode & S_IFREG)){
		log("%s is not a regular file\n", filename);
		return 0;
	}
	if( readOneFamily( filename, ti) != 1 ){
		log("cannot open file %s %s\n", filename, strerror(errno));
		return 0;
	}
/*
	if( readOneFamily( filename, ti) != 1 ){
		log("read:%s Error !! \n", filename);
	}else{
		log("read:%s OK !! \n", filename);
	}
*/
	return 1;
}

//andy_add 2003/06/16
int readOneFamily( char *filename, int i)
{
	char line[40000], data[40000];
	FILE *fp=NULL;
	int j;
	int m = 0, fmnum1 = 0, fmnum2 = 0;

	if( (fp = fopen(filename, "r") ) == NULL )	return 0;
	if (fgets(line, sizeof(line), fp) == NULL)	return 0;
	fclose( fp);

	chop(line);
	for (j = 1; j <= familymaxint + familymaxchar + MAX_MEMBERNUM + FAMILY_MEMONUM; j ++){
		easyGetTokenFromBuf(line, "|", j, data, sizeof(data));
		if (strcmp(data, "") == 0)	continue;
		setFamilyFileDataToArg( i, data);
	}
	if (family[i].fmgold > FMMAXGOLD) family[i].fmgold = FMMAXGOLD;
	if (family[i].fmgold < 0) family[i].fmgold = 0;
	for (m = 0; m < MAX_MEMBERNUM; m++){
		if (family[i].fmmemberindex[m].charflag <= 0) continue;
		if (family[i].fmmemberindex[m].charflag != FMMEMBER_APPLY)
			fmnum1 = fmnum1 + 1;
		if (family[i].fmmemberindex[m].charflag > 0 )
			fmnum2 = fmnum2 + 1;
	}
	family[i].fmjoinnum = fmnum1;
	family[i].fmnum = fmnum2;
	if (family[i].fmjoinnum >= MIN_MEMBERNUM){
		family[i].fmsetupflag = 1;
		family[i].predel_time = -1;
		db_familyupdate[i] = 1;
	}else if (family[i].fmjoinnum < MIN_MEMBERNUM){
		family[i].fmsetupflag = 0;
		if (family[i].predel_time == -1) {
			time_t t1;
			time(&t1);
			family[i].predel_time = t1 + PREDEL_TIME;
			db_familyupdate[i] = 1;
		}
	}
	return 1;
}
