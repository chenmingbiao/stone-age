#include "version.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#include "readmap.h"
#include "object.h"
#include "char.h"
#include "char_base.h"
#include "lssproto_serv.h"
#include "saacproto_cli.h"
#include "npcutil.h"
#include "family.h"
#include "log.h"
#include "handletime.h"
#include "buf.h"
#include "net.h"
#include "char_base.h"
#include "battle.h"
#include "npc_bus.h"
#include "char_talk.h"
#include "npc_scheduleman.h"
#ifdef _FM_MODIFY
#include "npc_fmdengon.h"
#endif

#define CHAR_MAXNAME 32
#define CHAR_MAXID 20
#define MINFMLEVLEFORPOINT	3	// 3 申请庄园最低等级
#ifdef _FAMILY_MANORNUM_CHANGE
#else
#define	MANORNUM		4
#endif

struct FAMILY
{
	int		fmindex;
	char	name[CHAR_MAXNAME];
	char	leadername[CHAR_MAXNAME];
	char	leaderid[CHAR_MAXID];
	int		leadergraph;
	char	petname[CHAR_MAXNAME];
	char	petattr[256];
	int		fmnum;
	int		acceptflag;
	char	rule[256];
	int		village;
	int		pointindex;
	int		dp;
	char	memolist[35][100];
};

int	familyNumTotal = 0;
char	familyListBuf[MAXFAMILYLIST];

int	channelMember[FAMILY_MAXNUM][FAMILY_MAXCHANNEL][FAMILY_MAXMEMBER];
int	familyMemberIndex[FAMILY_MAXNUM][FAMILY_MAXMEMBER];

int	familyTax[FAMILY_MAXNUM];

extern	tagRidePetTable ridePetTable[296];

void LeaveMemberIndex( int charaindex, int fmindexi);


// Arminius: 取得家族 pk dp 增加/损失值
// getFMdpAward
// arg: windp=winner's fmdp	losedp=loser's fmdp
// ret: dp award
//
#ifdef _MERGE_NEW_8
int fmdplevelexp[]={0,			// 0
					150000,		// 1
					500000,		// 2
					1000000,	// 3
					1500000,	// 4
					2000000,	// 5
					2500000,	// 6
					3500000,	// 7
					4500000,	// 8
					5000000,	// 9
					7000000		//10
					};
#else
int fmdplevelexp[]={0,			// 0
					10000,		// 1
					30000,		// 2
					100000,		// 3
					500000,		// 4
					1500000,	// 5
					5000000,	// 6
					15000000,	// 7
					50000000,	// 8
					200000000,	// 9
					500000000	//10
					};
#endif
// Arminius end

// shan begin
int getFmLv(int playerindex)	// 合成时专用
{
    int i, dp;
    dp = CHAR_getWorkInt(playerindex, CHAR_WORKFMDP);
    if( dp > fmdplevelexp[10] ){
//        print("\n player DP->%d",dp);
        return 10;
    }
    for(i=0; i<=10; i++)
        if( dp <= fmdplevelexp[i+1] ) break;
	// Nuke 20040217: Open the merge limit
	//if(i>=9) i = 8;
	if (i>=10) i=10;
	
    return i;
}

struct FMMEMBER_LIST memberlist[FAMILY_MAXNUM];
struct FMS_MEMO      fmsmemo;
struct FM_POINTLIST  fmpointlist;
struct FMS_DPTOP     fmdptop; 
struct FM_PKFLOOR    fmpkflnum[FAMILY_FMPKFLOOR]=
{
	{142},
	{143},
	{144},
	{145},
	{146},
	{1042},
	{2032},
	{3032},
	{4032},
#ifdef _FAMILY_MANORNUM_CHANGE	// CoolFish 用来修改装园数量
	{5032},
	{6032},
	{7032},
	{8032},
	{9032},
	{10032},
#endif
};
int leaderdengonindex = 0;
// shan end 新增图层需到 family.h 增加 FAMILY_FMPKFLOOR 数量

void SetFMPetVarInit(int meindex)
{
	int i = 0, petindex = 0;
#ifdef _FMVER21	
	if (CHAR_getInt(meindex, CHAR_FMLEADERFLAG) != FMMEMBER_LEADER)
#else
	if (CHAR_getInt(meindex, CHAR_FMLEADERFLAG) != 1)
#endif	
	{
		for (i = 0; i < CHAR_MAXPETHAVE; i++)
		{
			petindex = CHAR_getCharPet(meindex, i);
			if (!CHAR_CHECKINDEX(petindex))	continue;
			CHAR_setInt(petindex, CHAR_PETFAMILY, -1);
		}
   	}
}

void SetFMVarInit(int meindex)
{
	SetFMPetVarInit(meindex); // 清除守护兽 Flag
   	CHAR_setInt(meindex, CHAR_FMINDEX, -1);
   	CHAR_setChar(meindex, CHAR_FMNAME, "");
   	CHAR_setInt(meindex, CHAR_FMSPRITE, -1);
#ifdef _FMVER21   	
   	CHAR_setInt(meindex, CHAR_FMLEADERFLAG, FMMEMBER_NONE);
#else
   	CHAR_setInt(meindex, CHAR_FMLEADERFLAG, -1);
#endif   	
   	CHAR_setWorkInt(meindex, CHAR_WORKFMSETUPFLAG, -1);
   	CHAR_setWorkInt(meindex, CHAR_WORKFMINDEXI, -1);
   	CHAR_setWorkInt(meindex, CHAR_WORKFMCHARINDEX, -1);
}

void FAMILY_Init( void )
{
	int i, j ,k;

	for( i=0; i<FAMILY_MAXNUM; i++)	{
	    for( j=0; j<FAMILY_MAXCHANNEL; j++)
	        for( k=0; k<FAMILY_MAXMEMBER; k++)
	        	channelMember[i][j][k] = -1;
	}

	for( i=0; i<FAMILY_MAXNUM; i++)
	    for( j=0; j<FAMILY_MAXMEMBER; j++ )
	    	familyMemberIndex[i][j] = -1;	    
	familyListBuf[0] = '\0';
	saacproto_ACShowFMList_send( acfd );

	//print( "FamilyData_Init:%s ", familyListBuf );
}


void CHAR_Family(int fd, int index, char *message)
{
   char		firstToken[64];
   char		messageeraseescape[512];
   char*	messagebody;

   {
      if (*message == 0)	return;
      CHAR_getMessageBody(message, firstToken,
         sizeof(firstToken), &messagebody);
         
      if (!messagebody)		return;
      
      strcpysafe(messageeraseescape, sizeof(messageeraseescape),
         messagebody);
      makeStringFromEscaped(messageeraseescape);

      switch(tolower(firstToken[0]))
      {
			case 'a':
				// 成立家族
				FAMILY_Add(fd, index, message);
				break;
			case 'j':
				// 加入家族
				FAMILY_Join(fd, index, message);
				break;
			case 'e':
				// 离开、退出家族
				FAMILY_Leave(fd, index, message);
				break;
			case 'm':
				// 族长审核
				FAMILY_CheckMember(fd, index, message);
				break;         
			case 's':
				// 取得家族相关资料
				FAMILY_Detail(fd, index, message);
				break;
			case 'c':
				// 家族频道
				FAMILY_Channel(fd, index, message);
				break;
			case 'b':
				// 家族银行
				FAMILY_Bank(fd, index, message);
				break;
			case 'p':
				// 申请家族据点
				FAMILY_SetPoint(fd, index, message);
				break;
			case 't':
				// 是否继续招募成员
				FAMILY_SetAcceptFlag(fd, index, message);
				break;
			case 'x':
				// 修改家族主旨
				FAMILY_FixRule( fd, index, message );
				break;
			case 'r':
				// 骑乘宠物
				FAMILY_RidePet( fd, index, message );
				break;
			case 'l':
				// 族长功能
				FAMILY_LeaderFunc( fd, index, message );
				break;
#ifdef _FM_MODIFY
				// 家族布告栏功能
			case 'd':
#ifdef _UN_FMMEMO
#else
				NPC_FmDengonLooked(0,index);
#endif
				break;
#endif
			default:
				break;
      }
   }
}

int CheckFMLeader(int meindex)
{
   if (CHAR_getInt(meindex, CHAR_FMINDEX) > 0)	return -1;
   if (strcmp(CHAR_getChar(meindex, CHAR_FMNAME), "") != 0)	return -1;
#ifdef _FMVER21
   if (CHAR_getInt(meindex, CHAR_FMLEADERFLAG) == FMMEMBER_LEADER)	return -1;
#else
   if (CHAR_getInt(meindex, CHAR_FMLEADERFLAG) == 1)	return -1;
#endif   
   return 1;
}

int CheckFMMember(int meindex)
{
   if (CHAR_getInt(meindex, CHAR_FMINDEX) > 0)   	return -1;
   if (strcmp(CHAR_getChar(meindex, CHAR_FMNAME), "") != 0)	return -1;
#ifdef _FMVER21
   if (CHAR_getInt(meindex, CHAR_FMLEADERFLAG) > 0 &&
       CHAR_getInt(meindex, CHAR_FMLEADERFLAG) != FMMEMBER_APPLY )	return -1;
#else
   if (CHAR_getInt(meindex, CHAR_FMLEADERFLAG) > 0 )	return -1;
#endif       
   return 1;
}

int CheckLeaderQ(int meindex)
{
   if (CHAR_getInt(meindex, CHAR_LV) < FMLEADERLV
   	&& CHAR_getInt(meindex, CHAR_TRANSMIGRATION) <= 0)
   	return	-1;
   if (!NPC_EventCheckFlg(meindex, 4))
   	return	-2;
   return	0;
}

void FAMILY_Add(int fd, int meindex, char* message)
{
	char token[128], fmname[128], charname[128], charid[128];
	char petname[128], fmrule[256], petattr[256], buf[1024];
	int charlv, havepetindex, petindex, fmsprite = 0, chargrano;
	int gold, tmpflag;
	
	if (!CHAR_CHECKINDEX(meindex))	return;
	
	if (CHAR_getWorkInt(meindex, CHAR_WORKBATTLEMODE) != BATTLE_CHARMODE_NONE)
		return;
	if (CHAR_getInt(meindex, CHAR_FMINDEX) >= 0 
		&& strcmp(CHAR_getChar(meindex, CHAR_FMNAME), "") != 0)
	{
		lssproto_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,
			WINDOW_BUTTONTYPE_OK,
			-1, -1,
			makeEscapeString( "\n你已经加入家族罗～无法再成立家族！", buf, sizeof(buf)));
   	return;
	}
	tmpflag = CheckLeaderQ(meindex);
	if(tmpflag == -1)
	{
		lssproto_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,
			WINDOW_BUTTONTYPE_OK,
			-1, -1,
			makeEscapeString( "\n很抱歉喔！你的等级不足！", buf, sizeof(buf)));
   	return;
	}
	
	if(tmpflag == -2)
	{
		lssproto_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,
			WINDOW_BUTTONTYPE_OK,
			-1, -1,
			makeEscapeString( "\n很抱歉喔！你必须先完成成人礼才行！", buf, sizeof(buf)));
   	return;   
	}

	gold = CHAR_getInt(meindex, CHAR_GOLD);
	if( gold < 10000 )
	{
		lssproto_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,
			WINDOW_BUTTONTYPE_OK,
			-1, -1,
			makeEscapeString( "\n很抱歉喔！成立家族需要一万元石币的手续费！", buf, sizeof(buf)));
   	return;
	}
	else {
   	CHAR_setInt( meindex, CHAR_GOLD, gold-10000 );
   	CHAR_send_P_StatusString( meindex , CHAR_P_STRING_GOLD);
	}
	
	if (getStringFromIndexWithDelim(message, "|", 2, token,
   	sizeof(token)) == FALSE)	return;
	sprintf(fmname, "%s", token);
	if ((strstr(fmname, " ")) || (strcmp(fmname, "") == 0) || (strstr(fmname, "　")))
	{
		lssproto_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,
			WINDOW_BUTTONTYPE_OK,
			-1, -1,
			makeEscapeString( "\n家族的名称请勿输入空格！", buf, sizeof(buf)));
   	return;
	}
	if (getStringFromIndexWithDelim(message, "|", 3, token,
   	sizeof(token)) == FALSE)	return;
	havepetindex = atoi(token);
	petindex = CHAR_getCharPet(meindex, havepetindex);
	if (!CHAR_CHECKINDEX(petindex))
	{
		lssproto_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,
			WINDOW_BUTTONTYPE_OK,
			-1, -1,
			makeEscapeString( "\n请选择一只宠物作为家族守护兽！", buf, sizeof(buf)));
   	return;
	}
	if (getStringFromIndexWithDelim(message, "|", 4, token,
   	sizeof(token)) == FALSE)	return;
	fmsprite = atoi(token);
	if (getStringFromIndexWithDelim(message, "|", 5, token,
   	sizeof(token)) == FALSE)	return;
	if (strcmp(token, "") == 0)
   	sprintf(fmrule, "无");
	else
   	sprintf(fmrule, "%s", token);
	sprintf(charname, "%s", CHAR_getChar(meindex, CHAR_NAME));
	sprintf(charid, "%s", CHAR_getChar(meindex, CHAR_CDKEY));
	charlv = CHAR_getInt(meindex, CHAR_LV);
	chargrano = CHAR_getInt(meindex, CHAR_FACEIMAGENUMBER);
	if (strlen(CHAR_getChar(petindex, CHAR_USERPETNAME)) == 0)
   	sprintf(petname, "%s", CHAR_getChar(petindex, CHAR_NAME));
	else
   	sprintf(petname, "%s", CHAR_getChar(petindex, CHAR_USERPETNAME));
	sprintf(petattr, "%d %d %d %d", 
   	CHAR_getInt(petindex, CHAR_BASEIMAGENUMBER),
   	CHAR_getWorkInt(petindex, CHAR_WORKATTACKPOWER),
   	CHAR_getWorkInt(petindex, CHAR_WORKDEFENCEPOWER),
   	CHAR_getWorkInt(petindex, CHAR_WORKQUICK));
	CHAR_setInt(petindex, CHAR_PETFAMILY, 1);
	CHAR_setChar(meindex, CHAR_FMNAME, fmname);
#ifdef _FMVER21
	CHAR_setInt(meindex, CHAR_FMLEADERFLAG, FMMEMBER_LEADER);
#else
	CHAR_setInt(meindex, CHAR_FMLEADERFLAG, 1);
#endif   
	CHAR_setInt(meindex, CHAR_FMSPRITE, fmsprite);

	//   print("%s %s %s %d %s %s %s %d %d\n", fmname, charname, charid, charlv, petname,
	//   	petattr, fmrule, fmsprite, chargrano);
#ifdef _PERSONAL_FAME
	saacproto_ACAddFM_send(acfd, fmname, charname, charid, charlv,
   	petname, petattr, fmrule, fmsprite, chargrano,
   	CHAR_getInt(meindex, CHAR_FAME), CONNECT_getFdid(fd));
	//   print("ACAddFM acfd:%d meindex:%d fmname:%s charname:%s fame:%d Connectfd:%d fd:%d\n",
	//   	acfd, meindex, fmname, charname, CHAR_getInt(meindex, CHAR_FAME), CONNECT_getFdid(fd), fd);
#else
	saacproto_ACAddFM_send(acfd, fmname, charname, charid, charlv,
   	petname, petattr, fmrule, fmsprite, chargrano, CONNECT_getFdid(fd));
#endif
	
	// 要求最新家族列表
	//saacproto_ACShowFMList_send( acfd );
	
}

/*
  ╭┐┌╮ 
╭┘└┘└╮
└┐．．┌┘─╮ 
╭┴──┤★~~├╮ 
│ｏ　ｏ│　　│● 　 
╰┬──╯　　│ ~~~~~~~~~哞 
▲△▲△▲△▲△▲△▲△▲△▲△ 

*/

void ACAddFM(int fd, int result, int fmindex, int index)
{
   int meindex = CONNECT_getCharaindex(fd);
   char buf[1024];
   
//   print("ACAddFM result:%d fmindex:%d meindex:%d\n", result, fmindex, meindex); // test

   if (!CHAR_CHECKINDEX(meindex))	return;

//   print("ACAddFM_2!\n");
   
   if (CHAR_getWorkInt(meindex, CHAR_WORKBATTLEMODE) != BATTLE_CHARMODE_NONE)
         return;

//   print("ACAddFM_3!\n");

  if(result == 1)
	{
		CHAR_setInt(meindex, CHAR_FMINDEX, fmindex);
		CHAR_setWorkInt(meindex, CHAR_WORKFMINDEXI, index);
		CHAR_setWorkInt(meindex, CHAR_WORKFMSETUPFLAG, 0);
		 lssproto_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,
			 WINDOW_BUTTONTYPE_OK,
			 -1, -1,
			 makeEscapeString( "\n恭喜你成立了新的家族！但请在７天之内召集到１０名族人加入，不然会取消家族资格喔。", buf, sizeof(buf)));
		 JoinMemberIndex( meindex, index);
		 CHAR_charSaveFromConnect(fd, FALSE);
		 
		 // 要求最新家族资料
		 saacproto_ACShowFMList_send( acfd );
		 saacproto_ACShowMemberList_send( acfd, index );
		 saacproto_ACShowTopFMList_send(acfd, FM_TOP_INTEGRATE);
	
		 
		 LogFamily(
			 CHAR_getChar( meindex, CHAR_FMNAME),
			 CHAR_getInt( meindex, CHAR_FMINDEX),
			 CHAR_getChar( meindex, CHAR_NAME),
			 CHAR_getChar( meindex, CHAR_CDKEY),
			 "ADDFAMILY(成立家族)",
			 ""
			 );
   }
   else
   {
   	int i = 0, petindex = 0;
   	char tmpbuf[256];
   	int gold = CHAR_getInt(meindex, CHAR_GOLD);
   	CHAR_setInt(meindex, CHAR_GOLD, gold + 10000);
   	CHAR_send_P_StatusString( meindex , CHAR_P_STRING_GOLD);
   	SetFMVarInit(meindex);
   	
   	for (i = 0; i < CHAR_MAXPETHAVE; i++)
   	{
   	   petindex = CHAR_getCharPet(meindex, i);
   	   if (!CHAR_CHECKINDEX(petindex))	continue;
   	   CHAR_setInt(petindex, CHAR_PETFAMILY, -1);
   	}
   	if (fmindex == -2)
   		sprintf(tmpbuf, "\n已经有相同名字的家族成立了！");
   	else
   		sprintf(tmpbuf, "\n申请成立家族失败！");
	lssproto_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,
		WINDOW_BUTTONTYPE_OK,
		-1, -1,
		makeEscapeString(tmpbuf, buf, sizeof(buf)));
   }
   CHAR_sendStatusString( meindex, "F");
}

void FAMILY_Join(int fd, int meindex, char *message)
{
   int fmindex, charlv, index, fmsprite;
   char token[128], fmname[128], charname[128], charid[128], buf[1024];

   if (!CHAR_CHECKINDEX(meindex))	return;
   
   if ((CHAR_getWorkInt(meindex, CHAR_WORKPARTYMODE) != CHAR_PARTY_NONE)
      || (CHAR_getWorkInt(meindex, CHAR_WORKBATTLEMODE) != BATTLE_CHARMODE_NONE))
         return;

   if (CheckFMMember(meindex) < 0){
		lssproto_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,
			WINDOW_BUTTONTYPE_OK,
			-1, -1,
			makeEscapeString( "\n你已经加入其他家族了喔！", buf, sizeof(buf)));
		return;
   }

#ifdef _FM_JOINLIMIT
	if( CHAR_getInt( meindex, CHAR_FMTIMELIMIT ) > (int)time(NULL) ){
		char buff[255];
		sprintf(buff, "\n如之前退出家族，\n需满%d小时才能再加入家族喔！",(CHAR_getInt( meindex, CHAR_FMTIMELIMIT )-(int)time(NULL))/3600+1);
		lssproto_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,
			WINDOW_BUTTONTYPE_OK, -1, -1,
			makeEscapeString( buff, buf, sizeof(buf)));
		return;
	}
#endif

   if (getStringFromIndexWithDelim(message, "|", 2, token,
   	sizeof(token)) == FALSE)	return;
   index = atoi(token);
   if (getStringFromIndexWithDelim(message, "|", 3, token,
   	sizeof(token)) == FALSE)	return;
   fmindex = atoi(token);
   if (getStringFromIndexWithDelim(message, "|", 4, token,
   	sizeof(token)) == FALSE)	return;
   sprintf(fmname, "%s", token);
   if (getStringFromIndexWithDelim(message, "|", 5, token,
   	sizeof(token)) == FALSE)	return;
   fmsprite = atoi(token);
   sprintf(charname, "%s", CHAR_getChar(meindex, CHAR_NAME));
   sprintf(charid, "%s", CHAR_getChar(meindex, CHAR_CDKEY));
   charlv = CHAR_getInt(meindex, CHAR_LV);
   CHAR_setInt(meindex, CHAR_FMINDEX, fmindex);
   CHAR_setChar(meindex, CHAR_FMNAME, fmname);
#ifdef _FMVER21
   CHAR_setInt(meindex, CHAR_FMLEADERFLAG, FMMEMBER_APPLY);
#else
   CHAR_setInt(meindex, CHAR_FMLEADERFLAG, 0);
#endif   
   CHAR_setInt(meindex, CHAR_FMSPRITE, fmsprite);
   CHAR_setWorkInt(meindex, CHAR_WORKFMINDEXI, index);

//   print("JoinFM index:%d fmindex:%d fmname:%s charname:%s charid:%s charlv:%d sprite:%d\n",
//   	index, fmindex, fmname, charname, charid, charlv, fmsprite);
#ifdef _PERSONAL_FAME	// Arminius: 家族个人声望
//   print("fame:%d charfdid:%d\n", CHAR_getInt(meindex, CHAR_FAME),
//   	CONNECT_getFdid(fd));
   saacproto_ACJoinFM_send(acfd, fmname, fmindex, charname, charid, charlv,
   	index, CHAR_getInt(meindex, CHAR_FAME), CONNECT_getFdid(fd));
#else   
//   print("charfdid:%d\n", CONNECT_getFdid(fd));
   saacproto_ACJoinFM_send(acfd, fmname, fmindex, charname, charid, charlv,
   	index, CONNECT_getFdid(fd));
#endif
}

void ACJoinFM(int fd, int result, int recv)
{
  int meindex = CONNECT_getCharaindex(fd);
  char buf[1024];
   
  if(!CHAR_CHECKINDEX(meindex))	return;
  
   if ((CHAR_getWorkInt(meindex, CHAR_WORKPARTYMODE) != CHAR_PARTY_NONE)
      || (CHAR_getWorkInt(meindex, CHAR_WORKBATTLEMODE) != BATTLE_CHARMODE_NONE))
         return;

   if (result == 1) {
		 lssproto_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,
			WINDOW_BUTTONTYPE_OK,
			-1, -1,
			makeEscapeString( "\n谢谢你的加入申请！请先等族长对你的审核通过之後，才算正式加入。", buf, sizeof(buf)));
		 
		  JoinMemberIndex( meindex, CHAR_getWorkInt(meindex, CHAR_WORKFMINDEXI) );
		 
		sprintf(buf,"fame:%d",CHAR_getInt(meindex,CHAR_FAME));
		 
		 LogFamily(
			 CHAR_getChar( meindex, CHAR_FMNAME),
			 CHAR_getInt( meindex, CHAR_FMINDEX),
			 CHAR_getChar( meindex, CHAR_NAME),
			 CHAR_getChar( meindex, CHAR_CDKEY),
			 "JOINFAMILY(申请加入家族)",
			 buf
			 );
		 
   }
   else
   {
	SetFMVarInit(meindex);
   	if (recv == -2)
   	{
		lssproto_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,
			WINDOW_BUTTONTYPE_OK,
			-1, -1,
			makeEscapeString( "\n此家族目前不愿意招收成员！", buf, sizeof(buf)));
	}
	else if (recv == -3)
	{
		lssproto_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,
			WINDOW_BUTTONTYPE_OK,
			-1, -1,
			makeEscapeString( "\n此家族目前无法招收成员，家族成员人数已到达上限！", buf, sizeof(buf)));
	}
	else
		lssproto_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,
			WINDOW_BUTTONTYPE_OK,
			-1, -1,
			makeEscapeString( "\n申请加入家族失败！", buf, sizeof(buf)));
   }
   
   CHAR_sendStatusString( meindex, "F");   
}

void FAMILY_Leave(int fd, int meindex, char *message)
{
   int result, fmindex, index;
   char token[128], fmname[128], charname[128], charid[128], buf[1024];
   
   if (!CHAR_CHECKINDEX(meindex))	return;
   
   if (CHAR_getWorkInt(meindex, CHAR_WORKBATTLEMODE) != BATTLE_CHARMODE_NONE)
         return;
   
   if ((CHAR_getInt(meindex, CHAR_FMINDEX) == -1)
      || (strcmp(CHAR_getChar(meindex, CHAR_FMNAME), "") == 0)
#ifdef _FMVER21
      || (CHAR_getInt(meindex, CHAR_FMLEADERFLAG) == FMMEMBER_NONE))
#else
      || (CHAR_getInt(meindex, CHAR_FMLEADERFLAG) == -1))
#endif      
   {
	lssproto_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,
		WINDOW_BUTTONTYPE_OK,
		-1, -1,
		makeEscapeString( "\n你并没有加入家族喔！", buf, sizeof(buf)));
      	return;
   }

   {
	   int i, fmpks_pos;
		for( i=1; i<=MANORNUM; i++){ // CoolFish 4->MANORNUM 2002/2/25
		   fmpks_pos = i * MAX_SCHEDULE;
#ifdef _FMVER21
           if (CHAR_getInt(meindex, CHAR_FMLEADERFLAG) == FMMEMBER_LEADER){
#else
           if (CHAR_getInt(meindex, CHAR_FMLEADERFLAG) == 1){
#endif         
		       if( (fmpks[fmpks_pos+1].host_index+1)  == CHAR_getInt(meindex, CHAR_FMINDEX) ||
    			   (fmpks[fmpks_pos+1].guest_index+1) == CHAR_getInt(meindex, CHAR_FMINDEX) ){
	    		   lssproto_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,
		                             WINDOW_BUTTONTYPE_OK,
		                             -1, -1,
		                             makeEscapeString( "\n你目前的家族正约战中，因此无法解散家族！", buf, sizeof(buf)));
      	           return;
			   }
		   }
	   }
   }
   
   if (getStringFromIndexWithDelim(message, "|", 2, token,
   	sizeof(token)) == FALSE)	return;
   result = atoi(token);
   if (result == 1)
   {
      fmindex = CHAR_getInt(meindex, CHAR_FMINDEX);
      index = CHAR_getWorkInt(meindex, CHAR_WORKFMINDEXI);
      sprintf(fmname, "%s", CHAR_getChar(meindex, CHAR_FMNAME));
      sprintf(charname, "%s", CHAR_getChar(meindex, CHAR_NAME));
      sprintf(charid, "%s", CHAR_getChar(meindex, CHAR_CDKEY));
#ifdef _FMVER21
      if (CHAR_getInt(meindex, CHAR_FMLEADERFLAG) == FMMEMBER_LEADER) {
#else
      if (CHAR_getInt(meindex, CHAR_FMLEADERFLAG) == 1) {
#endif      
//         print("DelFM index:%d fmindex:%d fmname:%s\n", index, fmindex, fmname);
         saacproto_ACDelFM_send(acfd, fmname, fmindex, index, charname, charid,
				CONNECT_getFdid(fd));

         // 要求最新家族列表
         //saacproto_ACShowFMList_send( acfd );
      }
      else {
//         print("LeaveFM index:%d fmindex:%d fmname:%s charname:%s charid:%s\n",
//         	index, fmindex, fmname, charname, charid);
	 saacproto_ACLeaveFM_send(acfd, fmname, fmindex, charname, charid, index,
		 CONNECT_getFdid(fd));
      }

   }
}

void ACLeaveFM( int fd, int result, int resultflag)
{
	int meindex = CONNECT_getCharaindex(fd);
	char buf[1024];
	
	if (!CHAR_CHECKINDEX(meindex))	return;
	if (result == 1){
		// won 2002/01/05
		LogFamily(		
			CHAR_getChar( meindex, CHAR_FMNAME),
			CHAR_getInt( meindex, CHAR_FMINDEX),
			CHAR_getChar( meindex, CHAR_NAME),
			CHAR_getChar( meindex, CHAR_CDKEY),
	    	"LEAVEFAMILY(离开家族)",
				""
				);
		if(  CHAR_getWorkInt( meindex, CHAR_WORKFMCHANNEL) != -1 )
			CHAR_setWorkInt( meindex, CHAR_WORKFMCHANNEL, -1 );
		LeaveMemberIndex( meindex, CHAR_getWorkInt(meindex, CHAR_WORKFMINDEXI) );
		SetFMVarInit(meindex);
		lssproto_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,
			WINDOW_BUTTONTYPE_OK, -1, -1,
			makeEscapeString( "\n申请退出家族ＯＫ！", buf, sizeof(buf)));
		CHAR_setWorkInt( meindex, CHAR_WORKFMFLOOR, -1);
#ifdef _FM_JOINLIMIT
		CHAR_setInt( meindex, CHAR_FMTIMELIMIT, (int)time(NULL)+getJoinFamilyTime()*(60*60) );
#endif
		CHAR_setInt( meindex , CHAR_RIDEPET, -1 );
		CHAR_setInt( meindex , CHAR_BASEIMAGENUMBER , CHAR_getInt( meindex , CHAR_BASEBASEIMAGENUMBER) );
		CHAR_complianceParameter( meindex );
		CHAR_sendCToArroundCharacter( CHAR_getWorkInt( meindex, CHAR_WORKOBJINDEX ));
		CHAR_send_P_StatusString( meindex , CHAR_P_STRING_RIDEPET);
#ifdef _FM_METAMO
			CHAR_ReMetamo(meindex);
#endif
	}else
		lssproto_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,
		WINDOW_BUTTONTYPE_OK, -1, -1,
		makeEscapeString( "\n申请退出家族失败！", buf, sizeof(buf)));
	
	CHAR_sendStatusString( meindex, "F" );
}

void ACDelFM(int fd, int result)
{
   char buf[1024];
   int meindex = CONNECT_getCharaindex(fd);
   if (!CHAR_CHECKINDEX(meindex))	return;
   
   //if ((CHAR_getWorkInt(meindex, CHAR_WORKPARTYMODE) != CHAR_PARTY_NONE)
   //   || (CHAR_getWorkInt(meindex, CHAR_WORKBATTLEMODE) != BATTLE_CHARMODE_NONE))
   //      return;
         
   if (result == 1)
   {
// WON ADD
	LogFamily(
		CHAR_getChar( meindex, CHAR_FMNAME),
		CHAR_getInt( meindex, CHAR_FMINDEX),
		CHAR_getChar( meindex, CHAR_NAME),
		CHAR_getChar( meindex, CHAR_CDKEY),
		"DELFAMILY(家族解散)",
		""
	);

        LeaveMemberIndex( meindex, CHAR_getWorkInt(meindex, CHAR_WORKFMINDEXI) );
        /*
        for( i=0; i<FAMILY_MAXMEMBER; i++)
        {
      			familyMemberIndex[ CHAR_getWorkInt(meindex, CHAR_WORKFMINDEXI) ][i] = -1;
        }
        */
        
	SetFMVarInit(meindex);
	lssproto_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,
		WINDOW_BUTTONTYPE_OK,
		-1, -1,
		makeEscapeString( "\n家族已经解散了！", buf, sizeof(buf)));

	// 要求最新家族列表
	saacproto_ACShowFMList_send( acfd );

// won 移到前面去
/*
	LogFamily(
		CHAR_getChar( meindex, CHAR_FMNAME),
		CHAR_getInt( meindex, CHAR_FMINDEX),
		CHAR_getChar( meindex, CHAR_NAME),
		CHAR_getChar( meindex, CHAR_CDKEY),
		"DELFAMILY(家族解散)",
		""
	);
*/
   }
   else
	 lssproto_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,
		WINDOW_BUTTONTYPE_OK,
		-1, -1,
		makeEscapeString( "\n申请解散家族失败！", buf, sizeof(buf)));

   CHAR_sendStatusString( meindex, "F");
}

void ACShowFMList(int result, int fmnum, char *data)
{
   if( result != 1 )
   	return;

   //print("\ngetFamilyListFromAC:%d", fmnum );
   familyNumTotal = fmnum;
   strcpy( familyListBuf, data );   
   
//   print( "FamilyData:%s ", familyListBuf );
}

void ACShowMemberList(int result, int index, int fmnum, int fmacceptflag, int fmjoinnum,char *data)
{
   int  j;
   char numberid[3];
   char tmpbuf[128];
   if(result==0){
       return;
   }else{
       if(fmnum == -1){
           print("\n FamilyWorkIndex Error!!");
       }
       for(j=1;j<=FAMILY_MAXMEMBER;j++){
           strcpy(memberlist[index].numberlistarray[j-1],"");
       }
       for(j=1;j<=fmnum;j++){
           if(getStringFromIndexWithDelim(data," ",j,tmpbuf,sizeof(tmpbuf)) == FALSE) 
               return;
           strcpy(memberlist[index].numberlistarray[j-1],tmpbuf);    
           getStringFromIndexWithDelim(tmpbuf,"|",1,numberid,sizeof(numberid));
           memberlist[index].memberindex[j-1] = atoi(numberid);
       }
       memberlist[index].fmnum  = fmnum;
       memberlist[index].fmjoinnum = fmjoinnum;
       memberlist[index].accept = fmacceptflag;
       return;
   }
}

void ACShowDpTop(int result,int num, char *data, int kindflag)
{
    int i;
    char tmpbuf[256],tmpbuf1[64];
    if(result==0){
        return;
    }else{
        switch( kindflag )
        {
            case FM_TOP_INTEGRATE:
            {
                for(i=0; i<FAMILY_MAXNUM; i++){
                    strcpy(fmdptop.topmemo[i], "");
                    fmdptop.fmtopid[i] = -1;
#ifdef _FMVER21
                    fmdptop.fmtopdp[i] = -1;
#endif
                }
                fmdptop.num = num;
                for(i=0; i<fmdptop.num; i++){
                   if(getStringFromIndexWithDelim(data," ",i+1,tmpbuf,sizeof(tmpbuf)) == FALSE)
                       return;
                   strcpy(fmdptop.topmemo[i], tmpbuf);
                   // family index
                   getStringFromIndexWithDelim( tmpbuf, "|", 1, tmpbuf1, sizeof(tmpbuf1));
                   fmdptop.fmtopid[i] = atoi(tmpbuf1);
#ifdef _FMVER21
                   // family popularity
                   getStringFromIndexWithDelim( tmpbuf, "|", 6, tmpbuf1, sizeof(tmpbuf1));
                   fmdptop.fmtopdp[i] = atoi(tmpbuf1);                   
#endif
                }
            }
            break;
            case FM_TOP_ADV:
            {
                for(i=0; i<30; i++)
                    strcpy(fmdptop.adv_topmemo[i], "");
                fmdptop.adv_num = num;
                for(i=0; i<fmdptop.adv_num; i++){
                    if(getStringFromIndexWithDelim(data," ",i+1,tmpbuf,sizeof(tmpbuf)) == FALSE)
                        return;
                    strcpy(fmdptop.adv_topmemo[i], tmpbuf);
                }                
            }
            break;
            case FM_TOP_FEED:
            {
                for(i=0; i<30; i++)
                    strcpy(fmdptop.feed_topmemo[i], "");
                fmdptop.feed_num = num;
                for(i=0; i<fmdptop.feed_num; i++){
                    if(getStringFromIndexWithDelim(data," ",i+1,tmpbuf,sizeof(tmpbuf)) == FALSE)
                        return;
                    strcpy(fmdptop.feed_topmemo[i], tmpbuf);
                }                
            }
            break;
            case FM_TOP_SYNTHESIZE:
            {
                for(i=0; i<30; i++)
                    strcpy(fmdptop.syn_topmemo[i], "");
                fmdptop.syn_num = num;
                for(i=0; i<fmdptop.syn_num; i++){
                    if(getStringFromIndexWithDelim(data," ",i+1,tmpbuf,sizeof(tmpbuf)) == FALSE)
                        return;
                    strcpy(fmdptop.syn_topmemo[i], tmpbuf);
                }                
            }
            break;
            case FM_TOP_DEALFOOD:
            {
                for(i=0; i<30; i++)
                    strcpy(fmdptop.food_topmemo[i], ""); 
                fmdptop.food_num = num;
                for(i=0; i<fmdptop.food_num; i++){
                    if(getStringFromIndexWithDelim(data," ",i+1,tmpbuf,sizeof(tmpbuf)) == FALSE)
                        return;
                    strcpy(fmdptop.food_topmemo[i], tmpbuf);
                }                
            }
            break;
            case FM_TOP_PK:
            {
                for(i=0; i<30; i++)
                    strcpy(fmdptop.pk_topmemo[i], "");
                fmdptop.pk_num = num;
                for(i=0; i<fmdptop.pk_num; i++){
                    if(getStringFromIndexWithDelim(data," ",i+1,tmpbuf,sizeof(tmpbuf)) == FALSE)
                        return;
                    strcpy(fmdptop.pk_topmemo[i], tmpbuf);
                }                
            }
            break;
            default:
            break;
        }
    }
}

void ACShowPointList(int result, char *data)
{
    int i;
    char tmpbuf[100];
    if(result==0){
       return;
    }else{
        for(i=0;i<FAMILY_MAXHOME;i++){
            if(getStringFromIndexWithDelim(data," ",i+1,tmpbuf,sizeof(tmpbuf)) == FALSE)
               return;
            strcpy(fmpointlist.pointlistarray[i],tmpbuf);    
        }
    }
}

void ACShowFMMemo(int result, int index, int num, int dataindex, char *data)
{
   int  j;
   char tmpbuf[220];
   
   if(index==10000)
   {
       if(result==0){
           return;
       }else{
           for(j=1;j<=140;j++){
               strcpy(fmsmemo.memo[j-1],"");
           }
           for(j=1;j<=num;j++){
               if(getStringFromIndexWithDelim(data,"|",j,tmpbuf,sizeof(tmpbuf)) == FALSE) 
                    return;
               makeStringFromEscaped(tmpbuf);    
               strcpy(fmsmemo.memo[j-1],tmpbuf);    
           }
           fmsmemo.memonum   = num;
           fmsmemo.memoindex = dataindex-1;
           return;
       }
   }else{
       if(result==0){
           return;
       }else{
           if(num == -1){
                 print("\n FamilyWorkIndex Error!!");
           }

           for(j=1;j<=35;j++){
               strcpy(memberlist[index].memo[j-1],"");
           }
           for(j=1;j<=num;j++){
               if(getStringFromIndexWithDelim(data,"|",j,tmpbuf,sizeof(tmpbuf)) == FALSE) 
                   return;
               makeStringFromEscaped(tmpbuf);
               strcpy(memberlist[index].memo[j-1],tmpbuf);    
           }
           memberlist[index].memonum = num;
           memberlist[index].memoindex = dataindex-1;
           return;
       }
   }
}

#ifdef _PERSONAL_FAME   // Arminius: 家族显\\个人声望
void ACFMCharLogin(int fd, int result, int index, int floor, int fmdp,
	int joinflag, int fmsetupflag, int flag, int charindex, int charfame
	)
#else
void ACFMCharLogin(int fd, int result, int index, int floor, int fmdp,
	int joinflag, int fmsetupflag, int flag, int charindex)
#endif
{
   char buf[1024];
   int i, petindex;
   int meindex = CONNECT_getCharaindex(fd);
   if (!CHAR_CHECKINDEX(meindex))	return;
   if (result == 1){
		 if(charfame != CHAR_getInt(meindex,CHAR_FAME)){
			 sprintf(buf,"server fame:%d,ac fame:%d",CHAR_getInt(meindex,CHAR_FAME),charfame);
			 LogFamily(
				 CHAR_getChar(meindex, CHAR_FMNAME),
				 CHAR_getInt(meindex, CHAR_FMINDEX),
				 CHAR_getChar(meindex, CHAR_NAME),
				 CHAR_getChar(meindex, CHAR_CDKEY),
				 "ACFMCharLogin",
				 buf
				 );
		 }
		 //CHAR_setInt(meindex,CHAR_FAME,charfame);
		 CHAR_setWorkInt(meindex, CHAR_WORKFMINDEXI, index);
		 CHAR_setWorkInt(meindex, CHAR_WORKFMFLOOR, floor);
#ifdef _FM_LEADER_RIDE
			if(floor==-1){
				int i;
				int petindex = CHAR_getCharPet( meindex,CHAR_getInt( meindex , CHAR_RIDEPET));
				int petmetamo= CHAR_getInt( petindex , CHAR_BASEIMAGENUMBER);
				for(i=0;i<FMPOINTNUM;i++){
					if(CHAR_getInt( meindex, CHAR_FMLEADERFLAG ) == FMMEMBER_LEADER){

					}else if(CHAR_getInt( meindex, CHAR_FMLEADERFLAG ) == FMMEMBER_ELDER){

					}else if(CHAR_getInt( meindex, CHAR_FMLEADERFLAG ) == FMMEMBER_MEMBER){

					}
				}
				if(i<FMPOINTNUM){
					CHAR_setInt( meindex , CHAR_RIDEPET, -1 );
					CHAR_setInt( meindex , CHAR_BASEIMAGENUMBER , CHAR_getInt( meindex , CHAR_BASEBASEIMAGENUMBER) );
					CHAR_sendCToArroundCharacter( CHAR_getWorkInt( meindex, CHAR_WORKOBJINDEX ));
					CHAR_send_P_StatusString( meindex, CHAR_P_STRING_RIDEPET );
				}
			}
#endif
		 CHAR_setWorkInt(meindex, CHAR_WORKFMDP, fmdp);
		 CHAR_setWorkInt(meindex, CHAR_WORKFMSETUPFLAG, fmsetupflag);
		 CHAR_setWorkInt(meindex, CHAR_WORKFMCHARINDEX, charindex);
		 if (CHAR_getInt(meindex, CHAR_FMLEADERFLAG) != joinflag)
			 SetFMPetVarInit(meindex);
		 CHAR_setInt(meindex, CHAR_FMLEADERFLAG, joinflag);
		 
		 JoinMemberIndex(meindex, index);
		 
		 CHAR_sendStatusString(meindex, "f");
		 
		 CHAR_complianceParameter( meindex );
		 CHAR_sendCToArroundCharacter( CHAR_getWorkInt( meindex , CHAR_WORKOBJINDEX ));
#ifdef _FMVER21
		 if (CHAR_getInt(meindex, CHAR_FMLEADERFLAG) == FMMEMBER_LEADER)
#else
		 if (CHAR_getInt(meindex, CHAR_FMLEADERFLAG) == 1)
#endif        
		 {
			 for (i = 0; i < CHAR_MAXPETHAVE; i++)
			 {
				 petindex = CHAR_getCharPet(meindex, i);
				 if (!CHAR_CHECKINDEX(petindex))     continue;
				 if (CHAR_getInt(petindex, CHAR_PETFAMILY) == 1)
					 return;
			 }
			 for (i = 0; i < CHAR_MAXPOOLPETHAVE; i++)
			 {
				 petindex = CHAR_getCharPoolPet(meindex, i);
				 if (!CHAR_CHECKINDEX(petindex))     continue;
				 if (CHAR_getInt(petindex, CHAR_PETFAMILY) == 1)
					 return;
			 }
			 lssproto_WN_send(fd, WINDOW_MESSAGETYPE_MESSAGE,
				 WINDOW_BUTTONTYPE_OK,
				 -1, -1,
				 makeEscapeString("\n家族守护兽消失了！\n请立刻再选定一只守护兽，\n否则家族在七天之後会消失唷！\n", buf, sizeof(buf)));
			 saacproto_ACFixFMData_send(acfd,
				 CHAR_getChar(meindex, CHAR_FMNAME),
				 CHAR_getInt(meindex, CHAR_FMINDEX),
				 CHAR_getWorkInt(meindex, CHAR_WORKFMINDEXI), FM_FIX_DELFMTIME,
				 "", "", CHAR_getWorkInt(meindex, CHAR_WORKFMCHARINDEX),
				 CONNECT_getFdid(fd));
		 }
   }
   else
   {
   	   SetFMVarInit(meindex);
#ifdef _FM_JOINLIMIT
			 CHAR_setInt( meindex, CHAR_FMTIMELIMIT, (int)time(NULL)+getJoinFamilyTime()*(60*60) );
#endif
   	   if (flag == 0)
   		   CHAR_talkToCli(meindex, -1, "你已经退出家族或家族已经不存在了！", CHAR_COLORYELLOW);

	   // Robin 0928 ride bobo check
	   if( CHAR_getInt( meindex, CHAR_RIDEPET ) != -1 )
	   {
		   int rideindex = CHAR_getCharPet( meindex, CHAR_getInt( meindex, CHAR_RIDEPET) );
		
		   if( CHAR_getInt( rideindex, CHAR_BASEBASEIMAGENUMBER) == 100372 || CHAR_getInt( rideindex, CHAR_BASEBASEIMAGENUMBER) == 100373 )
		   {
			   CHAR_setInt( meindex, CHAR_RIDEPET, -1);
			   CHAR_send_P_StatusString( meindex, CHAR_P_STRING_RIDEPET );
			
			   CHAR_sendStatusString(meindex, "f");
			   CHAR_complianceParameter( meindex );
			   CHAR_sendCToArroundCharacter( CHAR_getWorkInt( meindex , CHAR_WORKOBJINDEX ));
		   }
		  }
   }
}

void FAMILY_Detail(int fd, int meindex, char *message)
{
	char token[128], token2[128], fmname[128];
	char buf[1024], subbuf[256], sendbuf[2048];
	int pindex1, i, j;
	int fmindex, tempindex;
	
	if (!CHAR_CHECKINDEX(meindex))	return;
	
	if ((CHAR_getWorkInt(meindex, CHAR_WORKPARTYMODE) != CHAR_PARTY_NONE)
		|| (CHAR_getWorkInt(meindex, CHAR_WORKBATTLEMODE) != BATTLE_CHARMODE_NONE))
		return;
	
	if (getStringFromIndexWithDelim(message, "|", 2, token,
   	sizeof(token)) == FALSE)	return;
	
	if ( strcmp(token,"F") ==0 )	{
		if (getStringFromIndexWithDelim(message, "|", 3, token2,
			sizeof(token)) == FALSE)	return;
		
		strcpy( buf, "");
		j = 0;	
		
		pindex1 = (atoi(token2) - 1)*10 +1;
		
		for( i=pindex1 ; i< pindex1+10 ; i++  ) {
			if( i > familyNumTotal )	break;
			if( getStringFromIndexWithDelim( familyListBuf, "|", i, subbuf,
				sizeof(subbuf) ) == FALSE)	break;
			strcat( buf, "|" );
			strcat( buf, subbuf );
			j++;
			
			// print(" |%s| ", subbuf);
		}
		
		sprintf( sendbuf, "S|F|%d|%d|%d%s", familyNumTotal, atoi(token2), j, buf );
		lssproto_FM_send( fd, sendbuf );
		
		return;	   	
		
	}
	
	// shan add
	if (strcmp(token, "P") == 0 ){
		int  personfame;
		char sendbuf[512];
#ifdef _PERSONAL_FAME
		personfame = (CHAR_getInt( meindex, CHAR_FAME)/100);
#else
		personfame = CHAR_getWorkInt( meindex, CHAR_WORKFMDP);
#endif       
		sprintf( sendbuf, "你目前的个人声望点数为：%d", personfame);
		CHAR_talkToCli(meindex, -1, sendbuf, CHAR_COLORYELLOW);
#ifdef _VIP_SERVER
		sprintf(sendbuf,"你目前的个人会员点数为：%d",CHAR_getInt(meindex,CHAR_AMPOINT));
		CHAR_talkToCli(meindex, -1, sendbuf, CHAR_COLORYELLOW);
#endif
	}
	
	if(strcmp(token,"D") == 0){
		if(getStringFromIndexWithDelim(message, "|", 3, fmname, sizeof(fmname)) == FALSE)	return;
		if(getStringFromIndexWithDelim(message, "|", 4, token2, sizeof(token2)) == FALSE)	return;
		fmindex = atoi( token2 );
		if(getStringFromIndexWithDelim(message, "|", 5, token2, sizeof(token2)) == FALSE)	return;
		tempindex = atoi( token2 );
		
		//print(" send_fmname_ac:%s ", fmname);
		saacproto_ACFMDetail_send( acfd, fmname, fmindex, tempindex, CONNECT_getFdid(fd) );
	}
	
	// shan begin
	else if (strcmp(token, "D2") ==0 ) {
		char sendbuf[2048], tmpbuf[1024], leadernamebuf[64];
		int h, i = 0;
		int meindex = CONNECT_getCharaindex(fd);
		int  fmindex_wk = CHAR_getWorkInt( meindex, CHAR_WORKFMINDEXI);
		if( fmindex_wk < 0 || fmindex_wk >= FAMILY_MAXNUM) return;
		
		for( h=0; h<FAMILY_MAXNUM; h++)
			if( fmdptop.fmtopid[h] == fmindex_wk )
				break;
			strcpy(tmpbuf, "");
			for (i = 0 + 1; i < FAMILY_MAXHOME + 1; i++)
			{
				int fmpks_pos = i * MAX_SCHEDULE, index;
				sprintf(fmname, "%s", CHAR_getChar(meindex, CHAR_FMNAME));
				index = CHAR_getWorkInt(meindex, CHAR_WORKFMINDEXI);
				if (fmpks[fmpks_pos + 1].flag == FMPKS_FLAG_MANOR_BATTLEBEGIN
					|| fmpks[fmpks_pos + 1].flag == FMPKS_FLAG_MANOR_PREPARE
					|| fmpks[fmpks_pos + 1].flag == FMPKS_FLAG_MANOR_OTHERPLANET)
				{
					if ((fmpks[fmpks_pos + 1].host_index == index
          		&& strcmp(fmname, fmpks[fmpks_pos + 1].host_name) == 0)
          	   || (fmpks[fmpks_pos + 1].guest_index == index
          	   	&& strcmp(fmname, fmpks[fmpks_pos + 1].guest_name) == 0))
					{
          		struct tm tm1;
							memcpy(&tm1, localtime((time_t *)&fmpks[fmpks_pos + 1].dueltime), sizeof(tm1));
							sprintf(tmpbuf, "%d/%d %d:%d【%s】 %s ｖｓ %s",
								tm1.tm_mon + 1, tm1.tm_mday, tm1.tm_hour, tm1.tm_min,
								fmpks[fmpks_pos + 2].host_name,
								fmpks[fmpks_pos + 1].guest_name,
								fmpks[fmpks_pos + 1].host_name);
					}
				}
			}
			if (strcmp(tmpbuf, "") == 0)
				sprintf(tmpbuf, "无挑战排程");
      
      getStringFromIndexWithDelim(memberlist[fmindex_wk].numberlistarray[0],
				"|",2,leadernamebuf,sizeof(leadernamebuf));
      // sendbuf -> 家族名称|人数|族长名称|家族排行|家族声望|个人声望|个人职位|家族精灵|PK
				sprintf( sendbuf, "%s|%d|%s|%d|%d|%d|%d|%d|%s", 
				CHAR_getChar(meindex, CHAR_FMNAME),
				memberlist[fmindex_wk].fmjoinnum,
				leadernamebuf,
				h+1,
#ifdef _FMVER21
				fmdptop.fmtopdp[h],
#else                
				(CHAR_getWorkInt( meindex, CHAR_WORKFMDP)/100),
#endif                
#ifdef _PERSONAL_FAME
				(CHAR_getInt( meindex, CHAR_FAME)/100),
#else
				(CHAR_getWorkInt( meindex, CHAR_WORKFMDP)/100),
#endif
				CHAR_getInt( meindex, CHAR_FMLEADERFLAG),
				CHAR_getInt( meindex, CHAR_FMSPRITE ),
				tmpbuf
				);
			
			lssproto_WN_send( fd, WINDOW_MESSAGETYPE_FAMILYDETAIL,
				WINDOW_BUTTONTYPE_OK,
				-1,
				-1,
				makeEscapeString( sendbuf, buf, sizeof(buf)));
	}
	// shan end
}

void ACFMDetail(int ret, char *data, int clifd)
{
	char sendbuf[1024];
	
	//print(" Detail:%s ", data);
	
	if( ret != 1 )
	{
		print(" ACFMDetailError!:%d ", clifd );
		return;
	}
	//print(" ACFMDetail:%d:%s ", clifd, data );
	
	/*	
	len = strlen(data);
	strcpy( buf, data );
	
	for( i=0 ; i<len ; i++ )
	{
		if( data[i] == '|' )	buf[i] = ' ';
		else 
		if( data[i] == ' ' )    buf[i] = '|';
	}
	*/
	//if (getStringFromIndexWithDelim(message, "|", 10, fmname,
   	//	sizeof(fmname)) == FALSE)	return;	

	//makeStringFromEscaped( buf );
	//buf2 = lssproto_demkstr_string( buf );

	
	sprintf(sendbuf, "S|D|%s", data); 
	lssproto_FM_send( clifd, sendbuf );
	//print(" Detail:%s ", sendbuf);
	/*
	lssproto_WN_send( clifd, WINDOW_MESSAGETYPE_FAMILYDETAIL,
		WINDOW_BUTTONTYPE_OK,
		-1, -1,
		sendbuf );
	*/
	
	// from saac data
	/*	
	sprintf(data, "%d|%d|%s|%s|%d|%s|%s|%d|%d|%s|%d", index, family[index].fmindex,
           family[index].fmname, family[index].fmleadername,
           family[index].fmleadergrano, family[index].petname, family[index].petattr,
           family[index].fmjoinnum, family[index].fmacceptflag,
           family[index].fmrule, family[index].fmsprite );
        */                           
	
}

void FAMILY_CheckMember(int fd, int meindex, char *message)
{
   int result, charindex, i, index = 0;
   char token[128], charname[128], fmname[128], buf[1024];
   if (!CHAR_CHECKINDEX(meindex)){
   	return;
   }
   if (CHAR_getWorkInt(meindex, CHAR_WORKBATTLEMODE) != BATTLE_CHARMODE_NONE)
   {
         return;
   }
   index = CHAR_getWorkInt(meindex, CHAR_WORKFMINDEXI);
   sprintf(fmname, "%s", CHAR_getChar(meindex, CHAR_FMNAME));
   if ((index == -1) || (strcmp(fmname, "") == 0)
#ifdef _FMVER21
//      || ((CHAR_getInt(meindex, CHAR_FMLEADERFLAG) != FMMEMBER_LEADER)
//          && (CHAR_getInt(meindex, CHAR_FMLEADERFLAG) != FMMEMBER_ELDER)
//          && (CHAR_getInt(meindex, CHAR_FMLEADERFLAG) != FMMEMBER_VICELEADER)))
      || ((CHAR_getInt(meindex, CHAR_FMLEADERFLAG) != FMMEMBER_LEADER)
          && (CHAR_getInt(meindex, CHAR_FMLEADERFLAG) != FMMEMBER_ELDER)))
#else
      || (CHAR_getInt(meindex, CHAR_FMLEADERFLAG) != 1))
#endif      
   {
//   	print("leaderflag:%d\n", CHAR_getInt(meindex, CHAR_FMLEADERFLAG));
	lssproto_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,
		WINDOW_BUTTONTYPE_OK,
		-1, -1,
		makeEscapeString( "\n你不是族长，所以没有修改的权力唷！", buf, sizeof(buf)));
      	return;
   }   
   if (getStringFromIndexWithDelim(message, "|", 2, token,
   	sizeof(token)) == FALSE)	return;
   sprintf(charname, "%s", token);
   if (getStringFromIndexWithDelim(message, "|", 3, token,
   	sizeof(token)) == FALSE)	return;
   charindex = atoi(token);
   if (getStringFromIndexWithDelim(message, "|", 4, token,
   	sizeof(token)) == FALSE)	return;
   result = atoi(token);
//   print("MemberCheck charname:%s charindex:%d mename:%s meworki:%d\n",
//   	charname, charindex, CHAR_getChar(meindex, CHAR_NAME),
//   	CHAR_getWorkInt(meindex, CHAR_WORKFMINDEXI));
   if ((strcmp(charname, CHAR_getChar(meindex, CHAR_NAME)) == 0)
   	&& (charindex == CHAR_getWorkInt(meindex, CHAR_WORKFMINDEXI)))
   		return;
//   print("MemberCheck message:%s\n", message);
   if (result == FMMEMBER_MEMBER)
#ifdef _FMVER21
   {
	// shan begin
    char sbuf[1024];
	sprintf( sbuf, "族长代号:%d -> 人物名称:%s 人物索引 (设该人物为族员):%d\n", CHAR_getInt(meindex, CHAR_FMLEADERFLAG), charname, charindex);
	LogFamily(
		CHAR_getChar(meindex, CHAR_FMNAME),
		CHAR_getInt(meindex, CHAR_FMINDEX),
		CHAR_getChar(meindex, CHAR_NAME),
		CHAR_getChar(meindex, CHAR_CDKEY),
		"CheckMember",
		sbuf
		);
	// shan end

   	saacproto_ACMemberJoinFM_send(acfd,
   		CHAR_getChar(meindex, CHAR_FMNAME),
   		CHAR_getInt(meindex, CHAR_FMINDEX), charname, charindex,
   		CHAR_getWorkInt(meindex, CHAR_WORKFMINDEXI), result,
   		CHAR_getWorkInt(meindex, CHAR_WORKFMCHARINDEX),
   		CONNECT_getFdid(fd));	
   }
#else
   {
   	saacproto_ACMemberJoinFM_send(acfd,
   		CHAR_getChar(meindex, CHAR_FMNAME),
   		CHAR_getInt(meindex, CHAR_FMINDEX), charname, charindex,
   		CHAR_getWorkInt(meindex, CHAR_WORKFMINDEXI), result,
   		CONNECT_getFdid(fd));
   }
#endif
#ifdef _FMVER21
   else if (result == FMMEMBER_NONE)
#else
   else if (result == 4)
#endif
   {
      for (i = 0 + 1; i < FAMILY_MAXHOME + 1; i++)
      {
      	 int fmpks_pos = i * MAX_SCHEDULE;
      	 if ((fmpks[fmpks_pos].host_index == index
      	 	&& strcmp(fmname, fmpks[fmpks_pos].host_name) == 0)
      	    || (fmpks[fmpks_pos].guest_index == index
      	    	&& strcmp(fmname, fmpks[fmpks_pos].guest_name) == 0))
      	 {
      	 	lssproto_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,
      	 		WINDOW_BUTTONTYPE_OK,
      	 		-1, -1,
      	 		makeEscapeString( "\n家族目前正在战斗中，所以无法审核成员。", buf, sizeof(buf)));
//      	 	print("fmpks_pos:%d index:%d host:%d guest:%d\n", fmpks_pos,
//      	 		index, fmpks[fmpks_pos].host_index, fmpks[fmpks_pos].guest_index);
      	 	return;
      	 }
      }
#ifdef _FMVER21
	  {	  
	  // shan begin
      char sbuf[1024];	  
	  sprintf( sbuf, "族长代号:%d -> 人物名称:%s 人物索引:%d (将该人物退出家族)\n", CHAR_getInt(meindex, CHAR_FMLEADERFLAG), charname, charindex);
	  LogFamily(
		  CHAR_getChar(meindex, CHAR_FMNAME),
		  CHAR_getInt(meindex, CHAR_FMINDEX),
		  CHAR_getChar(meindex, CHAR_NAME),
		  CHAR_getChar(meindex, CHAR_CDKEY),
		  "CheckMember",
		  sbuf
		  );
	  // shan end      
	  saacproto_ACMemberLeaveFM_send(acfd,
      		CHAR_getChar(meindex, CHAR_FMNAME),
      	 	CHAR_getInt(meindex, CHAR_FMINDEX), charname, charindex,
   		CHAR_getWorkInt(meindex, CHAR_WORKFMINDEXI),
   		CHAR_getWorkInt(meindex, CHAR_WORKFMCHARINDEX),
   		CONNECT_getFdid(fd));
	  }
#else
	  {
      saacproto_ACMemberLeaveFM_send(acfd,
      		CHAR_getChar(meindex, CHAR_FMNAME),
      	 	CHAR_getInt(meindex, CHAR_FMINDEX), charname, charindex,
   		CHAR_getWorkInt(meindex, CHAR_WORKFMINDEXI),
   		CONNECT_getFdid(fd));
	  }
#endif
   }
#ifdef _FMVER21
// else if (result == FMMEMBER_ELDER || result == FMMEMBER_INVITE
//   	|| result == FMMEMBER_BAILEE || result == FMMEMBER_VICELEADER )
   else if (result == FMMEMBER_ELDER )
   {
      if (CHAR_getInt(meindex, CHAR_FMLEADERFLAG) != FMMEMBER_LEADER)
      		return;
#ifdef _FMVER21
	  {
	  // shan begin
      char sbuf[1024];
	  sprintf( sbuf, "族长代号:%d -> 人物名称:%s 人物索引:%d (设该人物为长老)\n", CHAR_getInt(meindex, CHAR_FMLEADERFLAG), charname, charindex);
	  LogFamily(
		  CHAR_getChar(meindex, CHAR_FMNAME),
		  CHAR_getInt(meindex, CHAR_FMINDEX),
		  CHAR_getChar(meindex, CHAR_NAME),
		  CHAR_getChar(meindex, CHAR_CDKEY),
		  "CheckMember",
		  sbuf
		  );
	  // shan end      
   	saacproto_ACMemberJoinFM_send(acfd,
   		CHAR_getChar(meindex, CHAR_FMNAME),
   		CHAR_getInt(meindex, CHAR_FMINDEX), charname, charindex,
   		CHAR_getWorkInt(meindex, CHAR_WORKFMINDEXI), result,
   		CHAR_getWorkInt(meindex, CHAR_WORKFMCHARINDEX),
   		CONNECT_getFdid(fd));
	  }
#else
   {
   	saacproto_ACMemberJoinFM_send(acfd,
      		CHAR_getChar(meindex, CHAR_FMNAME),
      	 	CHAR_getInt(meindex, CHAR_FMINDEX), charname, charindex,
   		CHAR_getWorkInt(meindex, CHAR_WORKFMINDEXI), result,
   		CONNECT_getFdid(fd));
	  }
#endif
   }
#endif
   // 要求最新家族列表
   saacproto_ACShowFMList_send( acfd );
}

void FAMILY_Channel(int fd, int meindex, char *message)
{
	char token[128], token2[128];
	char buf[4096], subbuf[4096], sendbuf[4096];
	int i, tempindex, fmindexi, channel, nowchannel, num;
	
	fmindexi = CHAR_getWorkInt( meindex, CHAR_WORKFMINDEXI);
	nowchannel = CHAR_getWorkInt( meindex, CHAR_WORKFMCHANNEL );
	
	//   print(" channelFM:%d ", fmindexi);
	if( fmindexi < 0 ) {
		lssproto_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,
			WINDOW_BUTTONTYPE_OK,
			-1, -1,
			makeEscapeString( "\n你还没有加入任何家族！", buf, sizeof(buf)));
		return;
	}
	
	if (getStringFromIndexWithDelim(message, "|", 2, token,
		sizeof(token)) == FALSE)	return;
	 
	if (getStringFromIndexWithDelim(message, "|", 3, token2,
		sizeof(token2)) == FALSE)        return;
	 
	channel = atoi( token2 );
	 
	if( strcmp( token, "J") == 0) {
		if( channel < -1 || channel > FAMILY_MAXCHANNEL )return;
		if( nowchannel >= 0 && nowchannel < FAMILY_MAXCHANNEL ) {
			i = 0;
			while( i < FAMILY_MAXMEMBER ) {
				if( channelMember[fmindexi][nowchannel][i] == meindex ) {
					channelMember[fmindexi][nowchannel][i] = -1;
					break;
				}
				i++;
			}
		}
		
		if( channel > 0 && channel < FAMILY_MAXCHANNEL ) {
			i = 0;
			while( i < FAMILY_MAXCHANNELMEMBER ) {
				if( channelMember[fmindexi][channel][i] < 0 ) {
					channelMember[fmindexi][channel][i] = meindex;
					break;
				}
				i++;
			}
			if( i >= FAMILY_MAXCHANNELMEMBER ) {
				CHAR_talkToCli( meindex, -1, "此频道人数已满。", CHAR_COLORWHITE);
				return;
			}
			sprintf( buf, "加入家族频道 [%d]。", channel );
			CHAR_talkToCli( meindex, -1, buf, CHAR_COLORWHITE);
			
			if( nowchannel >=0 && nowchannel < FAMILY_MAXCHANNEL ) {
				sprintf( buf, "%s 退出频道。", CHAR_getChar( meindex, CHAR_NAME) );
				for( i=0; i < FAMILY_MAXCHANNELMEMBER; i++ ) {
					if( CHAR_CHECKINDEX(channelMember[fmindexi][nowchannel][i])
						&& channelMember[fmindexi][nowchannel][i] != meindex ) {
						CHAR_talkToCli( channelMember[fmindexi][nowchannel][i], -1, buf, CHAR_COLORWHITE);
					}
				}
			}
			sprintf( buf, "%s 加入频道。", CHAR_getChar( meindex, CHAR_NAME) );
			for( i=0; i < FAMILY_MAXCHANNELMEMBER; i++ ) {
				if( CHAR_CHECKINDEX(channelMember[fmindexi][channel][i])
					&& channelMember[fmindexi][channel][i] != meindex ) {
					CHAR_talkToCli( channelMember[fmindexi][channel][i], -1, buf, CHAR_COLORWHITE);
				}
			}
			
		}
		else if( channel == 0 ) {
			i = 0;
			while( i < FAMILY_MAXMEMBER ) {
				if( channelMember[fmindexi][0][i] < 0 ) {
					channelMember[fmindexi][0][i] = meindex;
					break;
				}
				i++;
			}
			if( i >= FAMILY_MAXMEMBER ) {
				CHAR_talkToCli( meindex, -1, "此频道人数已满。", CHAR_COLORWHITE);
				return;
			}
			sprintf( buf, "加入家族频道 [全]。");
			CHAR_talkToCli( meindex, -1, buf, CHAR_COLORWHITE);
			if( nowchannel >=0 && nowchannel < FAMILY_MAXCHANNEL ) {
				sprintf( buf, "%s 退出频道。", CHAR_getChar( meindex, CHAR_NAME) );
				for( i=0; i < FAMILY_MAXCHANNELMEMBER; i++ ) {
					if( CHAR_CHECKINDEX(channelMember[fmindexi][nowchannel][i])
						&& channelMember[fmindexi][nowchannel][i] != meindex ) {
						CHAR_talkToCli( channelMember[fmindexi][nowchannel][i], -1, buf, CHAR_COLORWHITE);
					}
				}
			}
			sprintf( buf, "%s 加入频道。", CHAR_getChar( meindex, CHAR_NAME) );
			for( i=0; i < FAMILY_MAXCHANNELMEMBER; i++ ) {
				if( CHAR_CHECKINDEX(channelMember[fmindexi][channel][i])
					&& channelMember[fmindexi][channel][i] != meindex ) {
					CHAR_talkToCli( channelMember[fmindexi][channel][i], -1, buf, CHAR_COLORWHITE);
				}
			}
		}
#ifdef _FMVER21
		else if( channel == FAMILY_MAXCHANNEL && CHAR_getInt( meindex, CHAR_FMLEADERFLAG ) == FMMEMBER_LEADER )
#else
		else if( channel == FAMILY_MAXCHANNEL && CHAR_getInt( meindex, CHAR_FMLEADERFLAG ) == 1 )
#endif
		{
			CHAR_talkToCli( meindex, -1, "启动族长广播。", CHAR_COLORWHITE);
		}
		else {
			channel = -1;
			CHAR_talkToCli( meindex, -1, "退出家族频道。", CHAR_COLORWHITE);
			
			sprintf( buf, "%s 退出频道。", CHAR_getChar( meindex, CHAR_NAME) );
			for( i=0; i < FAMILY_MAXCHANNELMEMBER; i++ ) {
				if( CHAR_CHECKINDEX(channelMember[fmindexi][nowchannel][i])
					&& channelMember[fmindexi][nowchannel][i] != meindex ) {
					CHAR_talkToCli( channelMember[fmindexi][nowchannel][i], -1, buf, CHAR_COLORWHITE);
				}
			}
		}
		
		CHAR_setWorkInt( meindex, CHAR_WORKFMCHANNEL, channel);
		if( channel != -1 ) CHAR_setWorkInt( meindex, CHAR_WORKFMCHANNELQUICK, channel);
		
		sprintf( sendbuf, "C|J|%d", channel);
		lssproto_FM_send( fd, sendbuf);
		
	}
	else if( strcmp( token, "L") == 0) {
		
		int j, membernum, bFind = 0;
		if( channel < 0 || channel >= FAMILY_MAXCHANNEL ) return;
		
		subbuf[0] = '\0';
		num = 0;
		if( channel != 0 )
			membernum = FAMILY_MAXCHANNELMEMBER;
		else
			membernum = FAMILY_MAXMEMBER;
		
		for( j = 0 ; j < FAMILY_MAXMEMBER ; j++ ) {
			bFind = 0;
			tempindex = familyMemberIndex[fmindexi][j];
			//if( tempindex >= 0 ) {
			if( CHAR_CHECKINDEX(tempindex) ) {
				for( i=0; i< membernum ; i++) {
					if( tempindex == channelMember[fmindexi][channel][i] ) {
						//if( CHAR_getChar( tempindex, CHAR_NAME ) == NULL ) {
						//	familyMemberIndex[fmindexi][j] = -1;
						//	channelMember[fmindexi][channel][i] = -1;
						//	continue;
						//}
						bFind = 1;
						break;
					}
				}
				if( bFind )
					strcat( subbuf, "|1|" );
				else
					strcat( subbuf, "|0|" );
				makeEscapeString( CHAR_getChar( tempindex, CHAR_NAME ), buf, sizeof(buf));
				strcat( subbuf, buf );
				num++;
			}
		}
		sprintf( sendbuf, "C|L|%d|%d%s", channel, num, subbuf);
		lssproto_FM_send( fd, sendbuf);
		//print(" CList:%s ", sendbuf);
	}
}

void FAMILY_Bank(int fd, int meindex, char *message)
{
	char token[128], token2[128], buf[1024];
	int fmindex, cash, bank, toBank;
	int MaxGold;
	MaxGold = CHAR_getMaxHaveGold( meindex);
	fmindex = CHAR_getInt( meindex, CHAR_FMINDEX);
	// add shan
	if( fmindex <= 0 && CHAR_getInt( meindex, CHAR_BANKGOLD) < 1) {
		CHAR_talkToCli( meindex, -1, "你必须先加入家族。", CHAR_COLORWHITE);
		return;
	}
	
	if (getStringFromIndexWithDelim(message, "|", 2, token, sizeof(token)) == FALSE)
	   return;
	
	if( strcmp(token, "G" )==0 )	{
		if (getStringFromIndexWithDelim(message, "|", 3, token2, sizeof(token)) == FALSE)
			return;
		
		toBank = atoi( token2 );
		cash = CHAR_getInt( meindex, CHAR_GOLD);
		bank = CHAR_getInt( meindex, CHAR_BANKGOLD);
		if( ((cash - toBank) >= 0) && ((cash - toBank) <= MaxGold )
			&&((bank + toBank) >= 0)&&((bank + toBank) <= CHAR_MAXBANKGOLDHAVE) ) {    
			// shan add       
			if( toBank > 0 && CHAR_getInt( meindex, CHAR_FMINDEX ) < 1 ) {
				sprintf(buf, "抱歉！你没有加入任何家族，所以仅能领取存款");
				CHAR_talkToCli( meindex, -1, buf, CHAR_COLORWHITE);
				return;
			}
			
			CHAR_setInt( meindex, CHAR_GOLD, cash - toBank);
			CHAR_setInt( meindex, CHAR_BANKGOLD, bank + toBank);
			CHAR_send_P_StatusString( meindex , CHAR_P_STRING_GOLD);
			
			if( toBank >= 0 ) {
				sprintf(buf, "存入%d到家族银行个人帐户。", toBank);
				CHAR_talkToCli( meindex, -1, buf, CHAR_COLORWHITE);
				// Syu ADD 新增家族个人银行存取Log (不含家族银行)
				LogFamilyBankStone(
					CHAR_getChar( meindex, CHAR_NAME ), 
					CHAR_getChar( meindex, CHAR_CDKEY ),
					toBank,                            
					CHAR_getInt( meindex, CHAR_GOLD ),
					"myBank(存款)(家族个人银行)",
					CHAR_getInt( meindex,CHAR_FLOOR),
					CHAR_getInt( meindex,CHAR_X ),
					CHAR_getInt( meindex,CHAR_Y ),
					CHAR_getInt( meindex,CHAR_BANKGOLD)
					);
				
			}
			else {
				sprintf(buf, "从家族银行个人帐户取出%d。", -toBank);
				CHAR_talkToCli( meindex, -1, buf, CHAR_COLORWHITE);
				// Syu ADD 新增家族个人银行存取Log (不含家族银行)
				LogFamilyBankStone(
					CHAR_getChar( meindex, CHAR_NAME ), 
					CHAR_getChar( meindex, CHAR_CDKEY ),
					toBank,                            
					CHAR_getInt( meindex, CHAR_GOLD ),
					"myBank(提款)(家族个人银行)",
					CHAR_getInt( meindex,CHAR_FLOOR),
					CHAR_getInt( meindex,CHAR_X ),
					CHAR_getInt( meindex,CHAR_Y ),
					CHAR_getInt( meindex,CHAR_BANKGOLD)
					);
				
			}
			// Syu ADD 新增家族个人银行存取Log (不含家族银行)
			LogStone(
				-1,
				CHAR_getChar( meindex, CHAR_NAME ), /* 平乓仿   */
				CHAR_getChar( meindex, CHAR_CDKEY ), /* 交□扒□ID */
				-toBank,                                 /* 嗯喊 */
				CHAR_getInt( meindex, CHAR_GOLD ),
				"myBank(家族个人银行)",
				CHAR_getInt( meindex,CHAR_FLOOR),
				CHAR_getInt( meindex,CHAR_X ),
				CHAR_getInt( meindex,CHAR_Y )
				);
			
		}
		else
			print(" bank_error ");
		
	}
	if( strcmp(token, "I" )==0 ) {
		
	}
	if( strcmp(token, "T" )==0 ) {
		int toTax;
		int mygold;
		int FMindex;
		
		if (getStringFromIndexWithDelim(message, "|", 3, token2, sizeof(token)) == FALSE)
			return;
		toTax = atoi( token2 );
		
#ifdef _FMVER21
		if( CHAR_getInt( meindex, CHAR_FMLEADERFLAG ) != FMMEMBER_LEADER &&
			CHAR_getInt( meindex, CHAR_FMLEADERFLAG ) != FMMEMBER_ELDER && toTax < 0 )
			return;
#endif
		
		
		FMindex = CHAR_getWorkInt( meindex, CHAR_WORKFMINDEXI );
		mygold = CHAR_getInt( meindex, CHAR_GOLD);
		if( mygold < 0 || mygold > MaxGold || toTax == 0 )	return;
		if( toTax > 0 )	{//+存款
			if( ((mygold-toTax) < 0) || (familyTax[ FMindex] + toTax) > CHAR_MAXFMBANKGOLDHAVE )	{
				return;
			}
		}else if( toTax < 0 ){ //-取款
			if( ((mygold-toTax)>MaxGold) || (familyTax[ FMindex] + toTax) < 0 )	{
				return;
			}
		}
		
		
		if( toTax>0 ) {	//存款预先扣款
			CHAR_setInt( meindex, CHAR_GOLD, CHAR_getInt( meindex, CHAR_GOLD)-toTax );
		}
		sprintf( buf, "家族银行%s处理中....", (toTax>0)?"存款":"取款");
		CHAR_talkToCli( meindex , -1, buf, CHAR_COLORYELLOW);
		
		CHAR_send_P_StatusString( meindex , CHAR_P_STRING_GOLD);
		sprintf( buf, "%d", toTax );
		
		saacproto_ACFixFMData_send(acfd,
			CHAR_getChar(meindex, CHAR_FMNAME),
			CHAR_getInt(meindex, CHAR_FMINDEX),
			CHAR_getWorkInt(meindex, CHAR_WORKFMINDEXI), FM_FIX_FMGOLD, buf,
			"", CHAR_getWorkInt(meindex, CHAR_WORKFMCHARINDEX), CONNECT_getFdid(fd));
		//print(" getTax:%s=%d ", CHAR_getChar(meindex, CHAR_FMNAME), toTax );
	}
}

void ACFMPointList(int ret, char *data)
{
}

#ifdef _ADD_FAMILY_TAX			   // WON ADD 增加庄园税收	
// GS 启动及定时向 AC 要求庄园税率
void GS_ASK_TAX(void)
{
	saacproto_GS_ASK_TAX_send(acfd);
}
	 
// 庄园族长修改税率
void FAMILY_FIX_TAX( int fd, int index, char* message)
{
    int fmpointindex=0, tax=0, fmindex=-1, i;
    char token[256];
    char pointbuf[256];

//	extern struct  FM_POINTLIST fmpointlist;  // 家族据点
	// 判断资格
	if (!CHAR_CHECKINDEX(index))	return;
   
	if ((CHAR_getInt(index, CHAR_FMINDEX) == -1)
		|| (strcmp(CHAR_getChar(index, CHAR_FMNAME), "") == 0)
#ifdef _FMVER21      
		|| (CHAR_getInt(index, CHAR_FMLEADERFLAG) != FMMEMBER_LEADER))
#else
		|| (CHAR_getInt(index, CHAR_FMLEADERFLAG) != 1))
#endif
	{
		return;
	}
	
	// 家族编号
	fmindex = CHAR_getInt(index, CHAR_FMINDEX);
	
	// 检查是否为庄园的家族
	for( i=0 ; i<FAMILY_MAXHOME ; i++ ) {
		if( getStringFromIndexWithDelim(fmpointlist.pointlistarray[i], "|", 5, pointbuf, sizeof(pointbuf)) == FALSE )
			continue;
		if( fmindex == atoi(pointbuf) ){
			if( getStringFromIndexWithDelim(message, "|", 3, token, sizeof(token)) == FALSE)	return;
			fmpointindex = atoi(token);
			if( getStringFromIndexWithDelim( message, "|", 4, token, sizeof(token)) == FALSE)	return;
			tax = atoi(token);
			saacproto_ACFMSetTAX_send( fd, tax, index, fmindex );
			return;
		}
	}
}
#endif


void FAMILY_SetPoint(int fd, int meindex, char *message)
{
   int i, fmpointindex, fl, x, y, fmdp, fmlevel = 0;
   char token[128], buf[1024];
   
   if (!CHAR_CHECKINDEX(meindex))	return;
   
   if (CHAR_getWorkInt(meindex, CHAR_WORKBATTLEMODE) != BATTLE_CHARMODE_NONE)
         return;
   
   if ((CHAR_getInt(meindex, CHAR_FMINDEX) == -1)
      || (strcmp(CHAR_getChar(meindex, CHAR_FMNAME), "") == 0)
#ifdef _FMVER21      
      || (CHAR_getInt(meindex, CHAR_FMLEADERFLAG) != FMMEMBER_LEADER))
#else
      || (CHAR_getInt(meindex, CHAR_FMLEADERFLAG) != 1))
#endif
   {
	lssproto_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,
		WINDOW_BUTTONTYPE_OK,
		-1, -1,
		makeEscapeString( "\n你不是族长，所以没有修改的权力唷！", buf, sizeof(buf)));
      	return;
   }
   fmdp = CHAR_getWorkInt(meindex, CHAR_WORKFMDP);
   for (i = 0; i < arraysizeof(fmdplevelexp); i++){
   	if (fmdp < fmdplevelexp[i + 1] && fmdp >= fmdplevelexp[i]){
   		fmlevel = i;
   	}
   }
   for (i = 1; i <= MANORNUM; i++) {
   	if (((strcmp(fmpks[i * MAX_SCHEDULE + 1].guest_name,
   		CHAR_getChar(meindex, CHAR_FMNAME)) == 0))
   	   && ((fmpks[i * MAX_SCHEDULE + 1].flag = FMPKS_FLAG_MANOR_BATTLEBEGIN)
   	   	|| (fmpks[i * MAX_SCHEDULE + 1].flag == FMPKS_FLAG_MANOR_PREPARE))){
   		char	tmpbuf[256];
   		sprintf(tmpbuf, "\n你跟%s已经有预约庄园争夺赛了～\n无法再申请庄园了喔！",
   			fmpks[i * MAX_SCHEDULE + 1].host_name);
   		lssproto_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,
   			WINDOW_BUTTONTYPE_OK,
   			-1, -1,
   			makeEscapeString(tmpbuf, buf, sizeof(buf)));
   	   	return;
   	}
   }
   if (getStringFromIndexWithDelim(message, "|", 2, token,
   	sizeof(token)) == FALSE)	return;
   fmpointindex = atoi(token);
   if (getStringFromIndexWithDelim(message, "|", 3, token,
   	sizeof(token)) == FALSE)	return;
   fl = atoi(token);
   if (getStringFromIndexWithDelim(message, "|", 4, token,
   	sizeof(token)) == FALSE)	return;
   x = atoi(token);
   if (getStringFromIndexWithDelim(message, "|", 5, token,
   	sizeof(token)) == FALSE)	return;
   y = atoi(token);
/*
   print("SetFMPoint charname:%s fmindex:%d index:%d pointindex:%d fl:%d x:%d y:%d\n",
   	CHAR_getChar(meindex, CHAR_FMNAME),
   	CHAR_getInt(meindex, CHAR_FMINDEX),
   	CHAR_getWorkInt(meindex, CHAR_WORKFMINDEXI),
   	fmpointindex, fl, x, y);
*/
   saacproto_ACSetFMPoint_send(acfd,
   	CHAR_getChar(meindex, CHAR_FMNAME),
   	CHAR_getInt(meindex, CHAR_FMINDEX),
   	CHAR_getWorkInt(meindex, CHAR_WORKFMINDEXI),
   	fmpointindex, fl, x, y, CONNECT_getFdid(fd));
}

void ACSetFMPoint(int ret, int r, int clifd)
{
   int meindex = CONNECT_getCharaindex(clifd);
   char message[256], buf[512];
   
   if (!CHAR_CHECKINDEX(meindex))	return;
   
   if ((CHAR_getWorkInt(meindex, CHAR_WORKPARTYMODE) != CHAR_PARTY_NONE)
      || (CHAR_getWorkInt(meindex, CHAR_WORKBATTLEMODE) != BATTLE_CHARMODE_NONE))
         return;
   if (ret == 0){
   		if (r == -1)
   			sprintf(message, "申请家族失败！");
   		else if (r == -2)
   			sprintf(message, "你已经有家族据点了～不得重复申请！");
   		else if (r == -3)
   			sprintf(message, "尚未到达申请家族据点的资格！");
   		else if (r == -4)
   			sprintf(message, "家族据点已经有家族在使用中罗！");
   		else if (r == -5)
   			sprintf(message, "您的家族人数未达申请标准唷！");
   }
   else if (ret == 1)
			sprintf(message, "申请家族据点ＯＫ！");
   
   lssproto_WN_send( clifd, WINDOW_MESSAGETYPE_MESSAGE,
   	WINDOW_BUTTONTYPE_OK,
   	-1, -1,
   	makeEscapeString(message, buf, sizeof(buf)));
}

void ACFMAnnounce(int ret, char *fmname, int fmindex, int index,
	int kindflag, char *data, int color)
{
   // kindflag 1:族长广播 2:系统公告家族被删除 3:系统通知讯息
   int i, chindex;
   if( ret != 1 )	return;
//   print("fmname:%s fmindex:%d index:%d kindflag:%d data:%s color:%d\n",
//   	fmname, fmindex, index, kindflag, data, color);
   for( i=0; i < FAMILY_MAXMEMBER; i++)
   {
      chindex = familyMemberIndex[index][i];
      if( chindex >= 0 ) {
         if( CHAR_getCharUse(chindex) )
         {
            if (kindflag == 1)
            {
#ifdef _FMVER21            
                // shan 2001/12/13
				//if( CHAR_getInt( chindex, CHAR_FMLEADERFLAG ) == FMMEMBER_MEMBER )
                if( CHAR_getInt( chindex, CHAR_FMLEADERFLAG ) == FMMEMBER_MEMBER ||
					CHAR_getInt( chindex, CHAR_FMLEADERFLAG ) == FMMEMBER_ELDER )
#else
               if( CHAR_getInt( chindex, CHAR_FMLEADERFLAG ) == 2 )
#endif               
                  CHAR_talkToCli( chindex, -1, data, color );
            }
            else if (kindflag == 2)
            {
               int fd = getfdFromCharaIndex( chindex );
               if (fd == -1)	return;
               SetFMVarInit( chindex );
               CHAR_talkToCli( chindex , -1, "由於您的家族在七天之内没有召收到１０名家族成员，所以被迫解散了！",
               		CHAR_COLORRED);
            }
         }else
            familyMemberIndex[index][i] = -1;
      }
   }
   if (kindflag == 3)
   {
      int meindex = 0;
      int clifd = getfdFromFdid(color);
      if (CONNECT_checkfd(clifd) == FALSE)	return;
      meindex = CONNECT_getCharaindex(clifd);
      if (!CHAR_CHECKINDEX(meindex))	return;
      CHAR_talkToCli(meindex, -1, data, CHAR_COLORRED);
      if(strstr(data,"已经审核完毕您的加入申请！")!=NULL){

			}else if(strstr(data,"已经将你踢出家族了！")!=NULL){
				CHAR_setInt( meindex , CHAR_RIDEPET, -1 );
				CHAR_setInt( meindex , CHAR_BASEIMAGENUMBER , CHAR_getInt( meindex , CHAR_BASEBASEIMAGENUMBER) );
				CHAR_complianceParameter( meindex );
				CHAR_sendCToArroundCharacter( CHAR_getWorkInt( meindex, CHAR_WORKOBJINDEX ));
				CHAR_send_P_StatusString( meindex , CHAR_P_STRING_RIDEPET);
#ifdef _FM_METAMO
				CHAR_ReMetamo(meindex);
#endif
			}
   }
   if (kindflag == 4)
   {
      int meindex = 0;
      char buf[1024];
      int clifd = getfdFromFdid(color);
//      print("Here1\n");
      if (CONNECT_checkfd(clifd) == FALSE)	return;
      meindex = CONNECT_getCharaindex(clifd);
      if (!CHAR_CHECKINDEX(meindex))	return;
//      print("Here2\n");
      lssproto_WN_send(clifd, WINDOW_MESSAGETYPE_MESSAGE,
      		WINDOW_BUTTONTYPE_OK,
      		-1, -1,
      		makeEscapeString(data, buf, sizeof(buf)));
   }
}

void FAMILY_SetAcceptFlag(int fd, int meindex, char *message)
{	
   int result;
   char token[128], buf[1024];
   
   if (!CHAR_CHECKINDEX(meindex))	return;
   
   if ((CHAR_getWorkInt(meindex, CHAR_WORKPARTYMODE) != CHAR_PARTY_NONE)
      || (CHAR_getWorkInt(meindex, CHAR_WORKBATTLEMODE) != BATTLE_CHARMODE_NONE))
         return;
   
   if ((CHAR_getInt(meindex, CHAR_FMINDEX) == -1)
      || (strcmp(CHAR_getChar(meindex, CHAR_FMNAME), "") == 0)
#ifdef _FMVER21      
//    || ((CHAR_getInt(meindex, CHAR_FMLEADERFLAG) != FMMEMBER_LEADER)
//       && (CHAR_getInt(meindex, CHAR_FMLEADERFLAG) != FMMEMBER_ELDER)
//       && (CHAR_getInt(meindex, CHAR_FMLEADERFLAG) != FMMEMBER_VICELEADER)))
    || ((CHAR_getInt(meindex, CHAR_FMLEADERFLAG) != FMMEMBER_LEADER)
       && (CHAR_getInt(meindex, CHAR_FMLEADERFLAG) != FMMEMBER_ELDER)))
#else
      || (CHAR_getInt(meindex, CHAR_FMLEADERFLAG) != 1))
#endif      
   {
	lssproto_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,
		WINDOW_BUTTONTYPE_OK,
		-1, -1,
		makeEscapeString( "\n你不是族长，所以没有修改的权力唷！", buf, sizeof(buf)));
      	return;
   }
   
   if (getStringFromIndexWithDelim(message, "|", 2, token,
   	sizeof(token)) == FALSE)	return;
   result = atoi(token);
   if ((result == 0) || (result == 1))
   {
   	saacproto_ACFixFMData_send(acfd,
   		CHAR_getChar(meindex, CHAR_FMNAME),
   		CHAR_getInt(meindex, CHAR_FMINDEX),
   		CHAR_getWorkInt(meindex, CHAR_WORKFMINDEXI), FM_FIX_ACCEPTFLAG,
   		token, "", CHAR_getWorkInt(meindex, CHAR_WORKFMCHARINDEX),
   		CONNECT_getFdid(fd));
   }
}

void FAMILY_FixRule( int fd, int meindex, char* message )
{

   char token[1024], buf[1024];
      
   if (!CHAR_CHECKINDEX(meindex))return;

   if ((CHAR_getInt(meindex, CHAR_FMINDEX) == -1)
      || (strcmp(CHAR_getChar(meindex, CHAR_FMNAME), "") == 0)
#ifdef _FMVER21      
      || (CHAR_getInt(meindex, CHAR_FMLEADERFLAG) != FMMEMBER_LEADER))
#else
      || (CHAR_getInt(meindex, CHAR_FMLEADERFLAG) != 1))
#endif      
   {
	lssproto_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,
		WINDOW_BUTTONTYPE_OK,
		-1, -1,
		makeEscapeString( "\n你不是族长，所以没有修改的权力唷！", buf, sizeof(buf)));
      	return;
   }

   if (getStringFromIndexWithDelim(message, "|", 2, token,
   	sizeof(token)) == FALSE)	return;
   
   if( strcmp( token, "R") == 0 )
   {
	if (getStringFromIndexWithDelim(message, "|", 3, buf,
		sizeof( buf ) ) == FALSE)return;
 
	   if (strcmp( buf, "") == 0)
	   {
		lssproto_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,
			WINDOW_BUTTONTYPE_OK,
			-1, -1,
			makeEscapeString( "\n家族主旨不可为空白唷！", buf, sizeof(buf)));
	      	return;
	   }

//	   print(" new_rule:%s ", buf);
	   saacproto_ACFixFMData_send(acfd,
   		CHAR_getChar(meindex, CHAR_FMNAME),
	   	CHAR_getInt(meindex, CHAR_FMINDEX),
   		CHAR_getWorkInt(meindex, CHAR_WORKFMINDEXI), FM_FIX_FMRULE,
   		buf, "", CHAR_getWorkInt(meindex, CHAR_WORKFMCHARINDEX),
   		CONNECT_getFdid(fd));
	   return;
   }
   if( strcmp( token, "P") == 0 )
   {
	   int havepetindex, petindex, i;
	   char petname[20], petattr[512];

	   // 检查是否已有守护兽
	   for( i =0; i< CHAR_MAXPETHAVE; i++ )
	   {
	   	int petindex = CHAR_getCharPet(meindex, i);
	   	if (!CHAR_CHECKINDEX(petindex))     continue;
	   	if( CHAR_getInt( petindex , CHAR_PETFAMILY ) ==1 )
	   	{
			lssproto_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,
				WINDOW_BUTTONTYPE_OK,
				-1, -1,
				makeEscapeString( "\n原本的守护兽还在唷。", buf, sizeof(buf)));
		      	return;
	   	}
	   }
	   // 检查是否已有守护兽(寄宠)
	   for( i =0; i< CHAR_MAXPOOLPETHAVE; i++ )
	   {
	   	int petindex = CHAR_getCharPoolPet(meindex, i);
	   	if (!CHAR_CHECKINDEX(petindex))     continue;
	   	if( CHAR_getInt( petindex , CHAR_PETFAMILY ) ==1 )
	   	{
			lssproto_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,
				WINDOW_BUTTONTYPE_OK,
				-1, -1,
				makeEscapeString( "\n原本的守护兽还在唷。", buf, sizeof(buf)));
		      	return;
	   	}
	   }

	   if (getStringFromIndexWithDelim(message, "|", 3, buf,
		sizeof( buf ) ) == FALSE) return;
	   havepetindex = atoi( buf );
	   
	   petindex = CHAR_getCharPet(meindex, havepetindex);
	   
	   if (!CHAR_CHECKINDEX(petindex))	return;

 	   if (strlen(CHAR_getChar(petindex, CHAR_USERPETNAME)) == 0)
	   	sprintf(petname, "%s", CHAR_getChar(petindex, CHAR_NAME));
	   else
   		sprintf(petname, "%s", CHAR_getChar(petindex, CHAR_USERPETNAME));
	   sprintf(petattr, "%d %d %d %d",
   		CHAR_getInt(petindex, CHAR_BASEIMAGENUMBER),
	   	CHAR_getWorkInt(petindex, CHAR_WORKATTACKPOWER),
   		CHAR_getWorkInt(petindex, CHAR_WORKDEFENCEPOWER),
	   	CHAR_getWorkInt(petindex, CHAR_WORKQUICK));
	   	
	   CHAR_setInt(petindex, CHAR_PETFAMILY, 1);
	   saacproto_ACFixFMData_send(acfd,
   		CHAR_getChar(meindex, CHAR_FMNAME),
	   	CHAR_getInt(meindex, CHAR_FMINDEX),
   		CHAR_getWorkInt(meindex, CHAR_WORKFMINDEXI), FM_FIX_FMPET,
   		petname, petattr, CHAR_getWorkInt(meindex, CHAR_WORKFMCHARINDEX),
   		CONNECT_getFdid(fd));
	   return;
	   
   }
   
}

void JoinMemberIndex( int meindex, int fmindexi )
{
	int i;
	
	for( i = 0 ; i < FAMILY_MAXMEMBER; i++){
		if( familyMemberIndex[fmindexi][i] == meindex ) familyMemberIndex[fmindexi][i] = -1;
	}
	
	for( i = 0 ; i < FAMILY_MAXMEMBER; i++){
		if( familyMemberIndex[fmindexi][i] < 0 ){
			familyMemberIndex[fmindexi][i] = meindex;
			break;
		}
	}
}

void LeaveMemberIndex( int meindex, int fmindexi )
{
	int i;
   
  for( i = 0 ; i < FAMILY_MAXMEMBER; i++){
    if( familyMemberIndex[fmindexi][i] == meindex ) familyMemberIndex[fmindexi][i] = -1;
  }
}

void FAMILY_RidePet( int fd, int meindex, char* message )
{
	char token[64], token2[64];
	int petindex, rideGraNo = 0, leaderimageNo;
	// Arminius 8.25 recover
	int i;
	int big4fm = 0;
	if (!CHAR_CHECKINDEX(meindex))return 0;

	// Robin fix 战斗中不可骑
	if( CHAR_getWorkInt( meindex, CHAR_WORKBATTLEMODE) != BATTLE_CHARMODE_NONE )
	{
		CHAR_talkToCli( meindex, -1, "战斗中不可骑宠！", CHAR_COLORYELLOW );
		return 0;
	}
	// Robin fix 交易中不可骑
	if( CHAR_getWorkInt(meindex, CHAR_WORKTRADEMODE) != CHAR_TRADE_FREE){
		CHAR_talkToCli( meindex, -1, "交易中不可骑宠！", CHAR_COLORYELLOW );
		return 0;
	}
#ifdef _PETSKILL_BECOMEPIG
    if( CHAR_getInt( meindex, CHAR_BECOMEPIG) > -1 ){ //处於乌力化状态
	    CHAR_setInt( meindex, CHAR_RIDEPET, -1 );
		//宠物选项的状态依然为"骑乘",这里修正过来 
		CHAR_complianceParameter( meindex );
		CHAR_send_P_StatusString( meindex, CHAR_P_STRING_RIDEPET);
		CHAR_talkToCli( meindex, -1, "目前你处于乌力化状态，不能骑乘宠物。", CHAR_COLORYELLOW );
		return 0;
	}
#endif
	if (getStringFromIndexWithDelim(message, "|", 2, token, sizeof(token)) == FALSE)	return 0;
	if( strcmp( token, "P") == 0) {
		if (getStringFromIndexWithDelim(message, "|", 3, token2, sizeof(token2)) == FALSE)
			return 0;
		if( atoi(token2) != -1 ) {
			petindex = CHAR_getCharPet( meindex, atoi( token2 ) );
			if(!CHAR_CHECKINDEX(petindex))return;
		
			if( CHAR_getInt( meindex, CHAR_DEFAULTPET ) == atoi( token2 ) )	return 0;
			if( CHAR_getInt( meindex, CHAR_RIDEPET) != -1 ) return 0;
			if( CHAR_getInt( meindex, CHAR_LEARNRIDE) < CHAR_getInt( petindex, CHAR_LV )  )
			{ 
				char buff[255];
				sprintf(buff,"你目前只能骑乘等级小于%d级的宠。",CHAR_getInt( meindex, CHAR_LEARNRIDE));
				CHAR_talkToCli( meindex, -1, buff, CHAR_COLORYELLOW );
				return 0;
			}	
			if( CHAR_getWorkInt( petindex, CHAR_WORKFIXAI ) < 100 )
			{
				CHAR_talkToCli( meindex, -1, "该骑宠的忠小于100", CHAR_COLORYELLOW );
				return 0;
			}
			printf("%d\n",__LINE__);
#ifdef _RIDELEVEL
			if( CHAR_getInt( meindex, CHAR_LV)+getRideLevel() < CHAR_getInt( petindex, CHAR_LV )  )
			{ 
				char buff[255];
				sprintf(buff,"你最高只能骑宠等级比你大%d级的宠。",getRideLevel());
				CHAR_talkToCli( meindex, -1, buff, CHAR_COLORYELLOW );
				return 0;
			}	
#else
			if( CHAR_getInt( meindex, CHAR_LV)+5 < CHAR_getInt( petindex, CHAR_LV )  ) return 0;
			{ 
				char buff[255];
				sprintf(buff,"你最高只能骑宠等级比你大5级的宠。");
				CHAR_talkToCli( meindex, -1, buff, CHAR_COLORYELLOW );
				return;
			}	
#endif
			printf("%d\n",__LINE__);
#ifdef _PET_2TRANS
			if( CHAR_getInt( petindex, CHAR_TRANSMIGRATION) > 2 ) return 0;
#endif
			leaderimageNo = 100700
				+ ((CHAR_getInt( meindex, CHAR_BASEBASEIMAGENUMBER)-100000)/20)*10
				+ CHAR_getInt( meindex, CHAR_FMSPRITE)*5;	
			switch( CHAR_getWorkInt( meindex, CHAR_WORKFMFLOOR) ){
				case 1041:
					big4fm = 1;
					break;
				case 2031:
					big4fm = 2;
					break;
				case 3031:
					big4fm = 3;
					break;
				case 4031:
					big4fm = 4;
					break;
				default:
					big4fm = 0;
			}
		// Arminius 8.25 recover
			for( i=0; i< arraysizeof(ridePetTable) ; i++ ){
#ifdef _NEW_RIDEPETS
				if( (( CHAR_getInt( meindex, CHAR_BASEIMAGENUMBER) == ridePetTable[i].charNo ) ||
					( CHAR_getInt( meindex, CHAR_BASEBASEIMAGENUMBER) == ridePetTable[i].charNo ))
#else
				if( ( CHAR_getInt( meindex, CHAR_BASEBASEIMAGENUMBER) == ridePetTable[i].charNo )
#endif
					&& ( CHAR_getInt( petindex, CHAR_BASEBASEIMAGENUMBER) == ridePetTable[i].petNo ) ){
					rideGraNo = ridePetTable[i].rideNo;
					break;
				}
#ifndef _NEW_RIDEPETS
				if( ( leaderimageNo == ridePetTable[i].charNo )
					&& ( CHAR_getInt( petindex, CHAR_BASEBASEIMAGENUMBER) == ridePetTable[i].petNo )
#ifdef _EVERYONE_RIDE
					&& big4fm != 0
					&& CHAR_getInt( meindex, CHAR_FMLEADERFLAG ) != FMMEMBER_NONE
					&& CHAR_getInt( meindex, CHAR_FMLEADERFLAG ) != FMMEMBER_APPLY )
#else
					&& CHAR_getInt( meindex, CHAR_FMLEADERFLAG ) == FMMEMBER_LEADER )
#endif
				{
						rideGraNo = ridePetTable[i].rideNo;
						break;
				}
#endif				
			}

#ifdef _NEW_RIDEPETS
			if( rideGraNo == 0 )	{
				int ti=-1, index, image=-1;
				int petNo = CHAR_getInt( petindex, CHAR_BASEBASEIMAGENUMBER);
				int playerNo = CHAR_getInt( meindex, CHAR_BASEBASEIMAGENUMBER);
				int playerlowsride = CHAR_getInt( meindex, CHAR_LOWRIDEPETS);
#ifdef _RIDE_CF
				int playerlowsride1 = CHAR_getInt( meindex, CHAR_LOWRIDEPETS1);
				if( (ti = RIDEPET_getPETindex( petNo, playerlowsride, playerlowsride1 )) >= 0 )
#else
				if( (ti = RIDEPET_getPETindex( petNo, playerlowsride )) >= 0 )
#endif
				{
					if( (index = RIDEPET_getNOindex( playerNo)) >= 0 ){
						if( (image = RIDEPET_getRIDEno( index,ti)) >= 0 )	{
							rideGraNo = image;
						}
					}
				}
			}
#endif
			if( rideGraNo != 0 ){
#ifdef _ITEM_METAMO
			//	CHAR_setWorkInt( meindex, CHAR_WORKITEMMETAMO, 0);
#endif
				CHAR_setInt( meindex , CHAR_RIDEPET, atoi( token2 ) );
				CHAR_setInt( meindex , CHAR_BASEIMAGENUMBER , rideGraNo );
				CHAR_complianceParameter( meindex );
				CHAR_sendCToArroundCharacter( CHAR_getWorkInt( meindex, CHAR_WORKOBJINDEX ));
				CHAR_send_P_StatusString( meindex, CHAR_P_STRING_RIDEPET );
				return 1;
			}else{
				printf("%d\n",__LINE__);
				return;
				int floor = CHAR_getWorkInt( meindex, CHAR_WORKFMFLOOR);
				if( floor !=-1 && CHAR_getInt( meindex, CHAR_FMLEADERFLAG ) != FMMEMBER_NONE
					&& CHAR_getInt( meindex, CHAR_FMLEADERFLAG ) != FMMEMBER_APPLY){
#ifdef _FM_LEADER_RIDE
					if(CHAR_FmLeaderRide( meindex, atoi( token2 )))return 1;
#endif
#ifdef _RIDEMODE_20
		 		if(getRideMode()>0){
					int ti=-1, index, image=-1;
					int petNo = CHAR_getInt( petindex, CHAR_BASEBASEIMAGENUMBER);
					int playerNo = CHAR_getInt( meindex, CHAR_BASEBASEIMAGENUMBER);
					int petindex = CHAR_getCharPet( meindex, atoi( token2 ));
					if(getRideMode()==1 || getRideMode()==2 ){
						if(floor == 1041 || floor == 2031 || floor == 3031 || floor == 4031 
							|| floor == 5031 || floor == 6031 || floor == 7031 
							|| floor == 8031 || floor == 9031 || floor == 10031){
							if( CHAR_getInt( meindex, CHAR_FMSPRITE ) == 0){
								if(petNo==100372){
#ifdef _RIDE_CF
									if( (ti = RIDEPET_getPETindex( petNo, RIDE_PET8, 0 )) >= 0 )
#else
									if( (ti = RIDEPET_getPETindex( petNo, RIDE_PET8 )) >= 0 )
#endif
									{
										if( (index = RIDEPET_getNOindex( playerNo)) >= 0 ){
											if( (image = RIDEPET_getRIDEno( index,ti)) >= 0 ){
												CHAR_setInt( meindex , CHAR_BASEIMAGENUMBER , image );
											}
										}
									}
								}
							}else if( CHAR_getInt( meindex, CHAR_FMSPRITE ) == 1){
								if(petNo==100373){
#ifdef _RIDE_CF
									if( (ti = RIDEPET_getPETindex( petNo, RIDE_PET9, 0 )) >= 0 )
#else
									if( (ti = RIDEPET_getPETindex( petNo, RIDE_PET9 )) >= 0 )
#endif
									{
										if( (index = RIDEPET_getNOindex( playerNo)) >= 0 ){
											if( (image = RIDEPET_getRIDEno( index,ti)) >= 0 ){
												CHAR_setInt( meindex , CHAR_BASEIMAGENUMBER , image );
											}
										}
									}
								}
							}
						}
					}else{
#ifdef _RIDE_CF
						if( (ti = RIDEPET_getPETindex( petNo, RIDE_PET_ALL, 0 )) >= 0 )
#else
						if( (ti = RIDEPET_getPETindex( petNo, RIDE_PET_ALL )) >= 0 )
#endif
						{
							if( (index = RIDEPET_getNOindex( playerNo)) >= 0 ){
								if( (image = RIDEPET_getRIDEno( index,ti)) >= 0 ){
									CHAR_setInt( meindex , CHAR_BASEIMAGENUMBER , image );
								}
							}
						}
					}
					if(image!=-1){
						CHAR_setInt( meindex , CHAR_RIDEPET, atoi( token2 ) );
						CHAR_complianceParameter( meindex );
						CHAR_sendCToArroundCharacter( CHAR_getWorkInt( meindex, CHAR_WORKOBJINDEX ));
						CHAR_send_P_StatusString( meindex, CHAR_P_STRING_RIDEPET );
						return 1;
					}
				}
#endif
			}
#ifdef _ITEM_RIDE
				{
					int itemindex = CHAR_getItemIndex( meindex, 0 );
					if(!ITEM_CHECKINDEX(itemindex)) return 0;
					if( !strcmp( ITEM_getChar( itemindex, ITEM_USEFUNC), "ITEM_RIDE") ) {
						char petmetamo[12],ridemetamo[12];
						char *itemarg = ITEM_getChar( itemindex, ITEM_ARGUMENT);
						getStringFromIndexWithDelim( itemarg, "|", 1, petmetamo, sizeof(petmetamo));
						getStringFromIndexWithDelim( itemarg, "|", 2, ridemetamo, sizeof(ridemetamo));
						int metamo= CHAR_getInt( petindex , CHAR_BASEIMAGENUMBER);
						if(metamo==atoi(petmetamo)){
							CHAR_setInt( meindex , CHAR_RIDEPET, atoi( token2 ) );
							CHAR_setInt( meindex , CHAR_BASEIMAGENUMBER , atoi( ridemetamo ) );
							CHAR_complianceParameter( meindex );
							CHAR_sendCToArroundCharacter( CHAR_getWorkInt( meindex, CHAR_WORKOBJINDEX ));
							CHAR_send_P_StatusString( meindex, CHAR_P_STRING_RIDEPET );
							return 1;
						}
					}
				}
#endif
			}
		}else    {	//还原人物 basebaseimage
			CHAR_setInt( meindex , CHAR_RIDEPET, -1 );
			CHAR_setInt( meindex , CHAR_BASEIMAGENUMBER , CHAR_getInt( meindex , CHAR_BASEBASEIMAGENUMBER) );
			CHAR_complianceParameter( meindex );
			CHAR_sendCToArroundCharacter( CHAR_getWorkInt( meindex, CHAR_WORKOBJINDEX ));
			CHAR_send_P_StatusString( meindex , CHAR_P_STRING_RIDEPET);
		}
	}
	return 0;
}

void ACFixFMPK(int winindex, int loseindex, int data)
{
   int i = 0, charindex = 0;
   char msg1[256], msg2[256];
   
   sprintf(msg1, "恭喜您！家族声望提高了%8d点！", (data / 100));
   sprintf(msg2, "家族声望减少了%8d点！", (data / 100));
   for (i = 0; i < FAMILY_MAXMEMBER; i++)
   {
      charindex = familyMemberIndex[winindex][i];
      if( charindex >= 0 ) {
         if( CHAR_getCharUse(charindex) )
               CHAR_talkToCli( charindex , -1, msg1, CHAR_COLORYELLOW);
         else
            familyMemberIndex[winindex][i] = -1;
      }
      charindex = familyMemberIndex[loseindex][i];
      if( charindex >= 0 ) {
         if( CHAR_getCharUse(charindex) )
               CHAR_talkToCli( charindex , -1, msg2, CHAR_COLORRED);
         else
            familyMemberIndex[loseindex][i] = -1;
      }
   }
}

void getNewFMList()
{
	saacproto_ACShowFMList_send( acfd );
}

//int     channelMember[FAMILY_MAXNUM][FAMILY_MAXCHANNEL][FAMILY_MAXMEMBER];
//int     familyMemberIndex[FAMILY_MAXNUM][FAMILY_MAXMEMBER];

void checkFamilyIndex( void )
{
	int i, j, k, charaindex, err1=0, err2=0;
//	print(" checkFamilyIndex! ");
	
	for( i=0; i<FAMILY_MAXNUM; i++){
		for( j=0; j<FAMILY_MAXMEMBER; j++){
			charaindex = familyMemberIndex[i][j];
			if( charaindex == -1 )	continue;
			if( !CHAR_CHECKINDEX(charaindex) ){
				familyMemberIndex[i][j] = -1;
				err1++;
				continue;
			}
			if( CHAR_getWorkInt( charaindex, CHAR_WORKFMINDEXI ) != i )
			{
				familyMemberIndex[i][j] = -1;
				err1++;
				continue;
			}
		}
		
		for( j=0; j<FAMILY_MAXCHANNEL; j++ )
			for( k=0; k<FAMILY_MAXMEMBER; k++)
			{
				charaindex = channelMember[i][j][k];
				if( charaindex == -1 )  continue;
				if( !CHAR_CHECKINDEX(charaindex) )
				{
					channelMember[i][j][k] = -1;
					err2++;
					continue;
				}
				if( CHAR_getWorkInt(charaindex, CHAR_WORKFMINDEXI) != i )
				{
					channelMember[i][j][k] = -1;
					err2++;
					continue;
				}
			}
	}
	
//	if( err1 )
//		print("家族索引建立错误:%d\n", err1);
//	if( err2 )
//		print("成员建立错误:%d\n", err2);
	if( ! (err1&&err2) )
		print("ok!\n");

}

void FAMILY_LeaderFunc( int fd, int meindex, char *message )
{
   char token[1024], token2[1024], buf[1024];

   if (!CHAR_CHECKINDEX(meindex))return;
   if ((CHAR_getInt(meindex, CHAR_FMINDEX) == -1)
      || (strcmp(CHAR_getChar(meindex, CHAR_FMNAME), "") == 0)
      //|| (CHAR_getInt(meindex, CHAR_FMLEADERFLAG) != FMMEMBER_LEADER)
   )
   {
	lssproto_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,
		WINDOW_BUTTONTYPE_OK,
		-1, -1,
		makeEscapeString( "你还未加入家族，所以不能使用唷！", buf, sizeof(buf) ));
      	return;
   }
   
   if (getStringFromIndexWithDelim(message, "|", 2, token,
   	sizeof(token)) == FALSE)	return;
   if( strcmp( token, "F") == 0 ){
       int  fmindex_wk;
       char sendbuf[1024],buf[1024];
       fmindex_wk = CHAR_getWorkInt( meindex, CHAR_WORKFMINDEXI);
#ifdef _FMVER21       
//     if( CHAR_getInt(meindex, CHAR_FMLEADERFLAG) != FMMEMBER_LEADER &&
//         CHAR_getInt(meindex, CHAR_FMLEADERFLAG) != FMMEMBER_ELDER &&
//         CHAR_getInt(meindex, CHAR_FMLEADERFLAG) != FMMEMBER_VICELEADER )  return;
     if( CHAR_getInt(meindex, CHAR_FMLEADERFLAG) != FMMEMBER_LEADER &&
         CHAR_getInt(meindex, CHAR_FMLEADERFLAG) != FMMEMBER_ELDER )  return;
#else
       if( CHAR_getInt(meindex, CHAR_FMLEADERFLAG) != 1)  return;
#endif       
       if( CHAR_getInt(meindex, CHAR_FMINDEX) > 0 ){
           if( fmindex_wk < 0 || fmindex_wk >= FAMILY_MAXNUM){
               print("FamilyNumber Data Error!!");
               return;
           }
       }
       saacproto_ACShowMemberList_send( acfd, fmindex_wk);           
	    
       sprintf( sendbuf, "               『族 长 需 知』\n请小心处理族员的资料，一经修改後就无法回复原态，敬请小心。");
	    
       lssproto_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,
	                 WINDOW_BUTTONTYPE_OK,
	                 CHAR_WINDOWTYPE_FM_MESSAGE2,
	                 CHAR_getWorkInt( leaderdengonindex, CHAR_WORKOBJINDEX),
	                 makeEscapeString( sendbuf, buf, sizeof(buf)));
   }
   if( strcmp( token, "L") == 0 ){
   	int i, kind, letterNo = 0;
   	char subtoken[256];

#ifdef _FMVER21
//   	if( CHAR_getInt(meindex, CHAR_FMLEADERFLAG) != FMMEMBER_LEADER &&
//   	    CHAR_getInt(meindex, CHAR_FMLEADERFLAG) != FMMEMBER_INVITE &&
//   	    CHAR_getInt(meindex, CHAR_FMLEADERFLAG) != FMMEMBER_VICELEADER)  return;
   	if( CHAR_getInt(meindex, CHAR_FMLEADERFLAG) != FMMEMBER_LEADER &&
   	    CHAR_getInt(meindex, CHAR_FMLEADERFLAG) != FMMEMBER_ELDER )  return;
#else
   	if( CHAR_getInt(meindex, CHAR_FMLEADERFLAG) != 1)  return;
#endif   	
   	
   	if (getStringFromIndexWithDelim(message, "|", 3, token2,
	   sizeof(token2)) == FALSE)	return;
	kind = atoi( token2 );
	
	for( i=0 ; i<FMPOINTNUM ; i++ ) {
		getStringFromIndexWithDelim(fmpointlist.pointlistarray[i], "|", 5, subtoken, sizeof(subtoken));
		if( CHAR_getInt( meindex, CHAR_FMINDEX ) == atoi(subtoken) ) {
			getStringFromIndexWithDelim(fmpointlist.pointlistarray[i], "|", 9, subtoken, sizeof(subtoken));
			if( kind == 1 ) {
				switch( atoi(subtoken) ) {
				case 1:
				letterNo = 19001;	break;
				case 2:
				letterNo = 19002;	break;
				case 3:
				letterNo = 19003;	break;
				case 4:
				letterNo = 19004;	break;
				}
			} else
			if( kind == 2 ) {
				switch( atoi(subtoken) ) {
				case 1:
				letterNo = 19005;	break;
				case 2:
				letterNo = 19006;	break;
				case 3:
				letterNo = 19007;	break;
				case 4:
				letterNo = 19008;	break;
				}			
			}
		}	
	}
	
	if( letterNo == 0 ) {
		lssproto_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,
			WINDOW_BUTTONTYPE_OK,
			-1, -1,
			makeEscapeString("\n只有拥有庄园的族长，才能制作唷！", buf, sizeof(buf)));
      		return;
	}else {
		int emptyitemindexinchara = CHAR_findEmptyItemBox( meindex );
		int itemindex = ITEM_makeItemAndRegist( letterNo );
		
		if( itemindex == -1 )	return;
		if( emptyitemindexinchara < 0 ) {
			CHAR_talkToCli( meindex, -1, "道具栏已满。", CHAR_COLORWHITE);
			return;
		}
		
		CHAR_setItemIndex( meindex, emptyitemindexinchara, itemindex );
		ITEM_setWorkInt( itemindex, ITEM_WORKOBJINDEX,-1);
		ITEM_setWorkInt( itemindex, ITEM_WORKCHARAINDEX, meindex);
		CHAR_sendItemDataOne( meindex, emptyitemindexinchara);
		snprintf( buf, sizeof( buf), "制作%s成功。",ITEM_getChar( itemindex, ITEM_NAME));
		CHAR_talkToCli( meindex, -1, buf, CHAR_COLORWHITE);
	}
  }
   if( strcmp( token, "CHANGE") == 0 ){
   	int fmindexi, j, num=0;
   	char subbuf[2048], sendbuf[2048];
   	
   	if (getStringFromIndexWithDelim(message, "|", 3, token2,
	   sizeof(token2)) == FALSE)	return;
	
	fmindexi = CHAR_getWorkInt( meindex, CHAR_WORKFMINDEXI );

	// 要求族长候选人列表
	if( strcmp( token2, "L") == 0 ){
		char subsub[128];
	
#ifdef _FMVER21
		if( CHAR_getInt(meindex, CHAR_FMLEADERFLAG) != FMMEMBER_LEADER)  return;
#else
		if( CHAR_getInt(meindex, CHAR_FMLEADERFLAG) != 1)  return;
#endif 		
		
		strcpy( subbuf, "");
		for( j = 0 ; j < FAMILY_MAXMEMBER ; j++ ) {
			int tempindex = familyMemberIndex[fmindexi][j];
			
			// CoolFish: 2001/9/22 
			if (!CHAR_CHECKINDEX(tempindex))	continue;
			
			if (CheckLeaderQ(tempindex) >= 0 && tempindex != meindex )
			{
				char	tmpbuf[1024];
				sprintf(tmpbuf, "%s", CHAR_getChar(tempindex, CHAR_NAME));
				makeEscapeString( tmpbuf, buf, sizeof(buf));
				sprintf( subsub, "|%d|%s", j, buf );
				strcat( subbuf, subsub );
				num++;
			}
		}
		sprintf( sendbuf, "L|CHANGE|L|%d%s", num, subbuf );
		//lssproto_FM_send( fd, sendbuf);
		
		lssproto_WN_send( fd, WINDOW_MESSAGETYPE_LEADERSELECT,
			WINDOW_BUTTONTYPE_OK,
			-1, -1,
			sendbuf );
		
	}
	// 询问族长候选人是否愿意接受
	if( strcmp( token2, "Q") == 0 )	
	{
		char token3[64], token4[64];
		int toindex;

#ifdef _FMVER21		
		if( CHAR_getInt(meindex, CHAR_FMLEADERFLAG) != FMMEMBER_LEADER)  return;
#else
		if( CHAR_getInt(meindex, CHAR_FMLEADERFLAG) != 1)  return;
#endif 		
		if (getStringFromIndexWithDelim(message, "|", 4, token3,
			sizeof(token3)) == FALSE)return;
		if (getStringFromIndexWithDelim(message, "|", 5, token4,
			sizeof(token4)) == FALSE)return;
		makeStringFromEscaped( token4 );
		
		if( atoi(token3) < 0 || atoi(token3) > FAMILY_MAXMEMBER ) return;

		toindex = familyMemberIndex[fmindexi][atoi(token3)];
		if( !CHAR_CHECKINDEX( toindex ) )	return;
		if( strcmp( token4, CHAR_getChar( toindex, CHAR_NAME)) != 0 )	return;
		if( CheckLeaderQ(toindex) < 0 )	return;
		
		// 双方都决定让位时，CHAR_WORKLEADERCHANGE存放对方的charaindex
		CHAR_setWorkInt( toindex, CHAR_WORKLEADERCHANGE, meindex);
		CHAR_setWorkInt( meindex, CHAR_WORKLEADERCHANGE, toindex);
		
		sprintf( sendbuf, "%s|%d", makeEscapeString( CHAR_getChar( meindex, CHAR_NAME ), buf, sizeof(buf)), meindex );
		
		lssproto_WN_send( CHAR_getWorkInt( toindex, CHAR_WORKFD ), WINDOW_MESSAGETYPE_LEADERSELECTA,
			WINDOW_BUTTONTYPE_OK,
			-1, -1,
			sendbuf );

	}
	// 候选人的答覆
	if( strcmp( token2, "A") == 0 )
	{
		int leaderindex, answerflag;
		char leadername[64], token3[64], token4[64];

//		print( "%s", message );
		
		if (getStringFromIndexWithDelim(message, "|", 4, token3,
			sizeof(token3)) == FALSE) return;
		answerflag = atoi( token3 );
		
		if (getStringFromIndexWithDelim(message, "|", 5, leadername,
			sizeof( leadername )) == FALSE)return;
		makeStringFromEscaped( leadername );
		
		if (getStringFromIndexWithDelim(message, "|", 6, token4,
			sizeof(token4)) == FALSE)return;
		
		//if( atoi(token4) < 0 || atoi(token4) > FAMILY_MAXMEMBER )  return;
		
		leaderindex = atoi( token4 );
		
		// 检查双方的CHAR_WORKLEADERCHANGE是否相符
		if( CHAR_getWorkInt( meindex, CHAR_WORKLEADERCHANGE ) != leaderindex )	return;
		if( !CHAR_CHECKINDEX(leaderindex) )	return;
		if( strcmp( leadername, CHAR_getChar( leaderindex, CHAR_NAME) ) != 0 )	return;
		if( CHAR_getWorkInt( leaderindex, CHAR_WORKLEADERCHANGE ) != meindex )  return;
		CHAR_setWorkInt( leaderindex, CHAR_WORKLEADERCHANGE, -1 );
#ifdef _FMVER21		
		if( CHAR_getInt(leaderindex, CHAR_FMLEADERFLAG ) != FMMEMBER_LEADER )  return;
#else
		if( CHAR_getInt(leaderindex, CHAR_FMLEADERFLAG ) != 1 )  return;
#endif 		
		if( CHAR_getInt(meindex, CHAR_FMINDEX) != CHAR_getInt(leaderindex, CHAR_FMINDEX) )  return;

		if( answerflag == 0 )
		{
			CHAR_setWorkInt( meindex, CHAR_WORKLEADERCHANGE, 0);
			CHAR_setWorkInt( leaderindex, CHAR_WORKLEADERCHANGE, 0);
			lssproto_WN_send( CHAR_getWorkInt( leaderindex, CHAR_WORKFD) , WINDOW_MESSAGETYPE_MESSAGE,
				WINDOW_BUTTONTYPE_OK,
				-1, -1,
				makeEscapeString( "\n对不起！对方不愿意接受！", buf, sizeof(buf)) );
		   	return;
		}
		
		if( answerflag == 1 )
		{
			char	tmpbuf[1024];
			sprintf( buf, "%d", CHAR_getInt( meindex, CHAR_FACEIMAGENUMBER ) );
			// CoolFish: add charname 2001/9/27
			sprintf( tmpbuf, "%s", CHAR_getChar( meindex, CHAR_NAME ) );
			saacproto_ACFixFMData_send(acfd,
				CHAR_getChar(meindex, CHAR_FMNAME),
	   			CHAR_getInt(meindex, CHAR_FMINDEX),
			   	CHAR_getWorkInt(meindex, CHAR_WORKFMINDEXI), FM_FIX_FMLEADERCHANGE , buf,
			   	tmpbuf, CHAR_getWorkInt(meindex, CHAR_WORKFMCHARINDEX), CONNECT_getFdid(fd));
			   	// "", CHAR_getWorkInt(meindex, CHAR_WORKFMCHARINDEX), CONNECT_getFdid(fd));
			return;
		}
	}	
   }
}

void ACFMJob( int fd, int ret, char* data1, char* data2 )
{
	
	int charaindex = CONNECT_getCharaindex( fd );
	if( !CHAR_CHECKINDEX(charaindex) ) return;
	
	
        if( 1 ){
        	
        	int leaderindex = CHAR_getWorkInt( charaindex, CHAR_WORKLEADERCHANGE );
        	char buf[256], buf2[256];

        	CHAR_setWorkInt( charaindex, CHAR_WORKLEADERCHANGE, 0 );
        	print("leaderindex:%d:%s\n", leaderindex,CHAR_getChar(leaderindex,CHAR_NAME) );
        	
        	if( !CHAR_CHECKINDEX(leaderindex) ) return;
        	//if( CHAR_getWorkInt( leaderindex, CHAR_WORKLEADERCHANGE ) != charaindex ) return;
        	CHAR_setWorkInt( leaderindex, CHAR_WORKLEADERCHANGE, 0 );
        	
        	if( ret == 0 ){
        		CHAR_talkToCli( charaindex, -1, "族长让位失败！", CHAR_COLORYELLOW );
        		CHAR_talkToCli( leaderindex, -1, "族长让位失败！", CHAR_COLORYELLOW );
	        	return;
	        }
	        
	        // Robin 10/02 debug
        	if( CHAR_getInt( leaderindex, CHAR_FMINDEX) != CHAR_getInt( charaindex, CHAR_FMINDEX)
#ifdef _FMVER21        	
			// || CHAR_getInt( leaderindex, CHAR_FMLEADERFLAG) != FMMEMBER_LEADER )
#else
			// || CHAR_getInt( leaderindex, CHAR_FMLEADERFLAG) != 1
#endif
		)
		{
			sprintf( buf, "leaderindex:%d:%s\n", leaderindex, CHAR_getChar( leaderindex, CHAR_NAME) );
			LogFamily(
				CHAR_getChar(charaindex, CHAR_FMNAME),
				CHAR_getInt(charaindex, CHAR_FMINDEX),
				CHAR_getChar(charaindex, CHAR_NAME),
				CHAR_getChar(charaindex, CHAR_CDKEY),
				"LEADERCHANGE_ERROR(族长让位失败)",
				buf
			);
			return;
		}
		
		//CHAR_setInt( leaderindex, CHAR_FMLEADERFLAG, FMMEMBER_MEMBER);
		//CHAR_setInt( charaindex, CHAR_FMLEADERFLAG, FMMEMBER_LEADER);
		SetFMPetVarInit( leaderindex );
		SetFMPetVarInit( charaindex );
		CHAR_sendStatusString( leaderindex, "F");
		CHAR_sendStatusString( charaindex, "F");
		
		lssproto_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,
			WINDOW_BUTTONTYPE_OK,
			-1, -1,
			makeEscapeString( "\n恭喜你！你已经是新任的族长了。\n请好好的努力吧！\n对了～记得请先到村长家的家族管理员选择\n新的家族守护兽，否则家族将会被解散唷！", buf, sizeof(buf)));
			
		sprintf( buf2, "\n辛苦你了！你已经将族长的位子交给%s了。", CHAR_getChar( charaindex, CHAR_NAME) );
		lssproto_WN_send( CHAR_getWorkInt( leaderindex, CHAR_WORKFD) , WINDOW_MESSAGETYPE_MESSAGE,
			WINDOW_BUTTONTYPE_OK,
			-1, -1,
			makeEscapeString( buf2, buf, sizeof(buf)));

//		print(" LeaderChange!! [%s]->[%s] ", CHAR_getChar(leaderindex, CHAR_CDKEY), CHAR_getChar(charaindex, CHAR_CDKEY) );
		
		sprintf( buf, "%s\t%s\t%s",
			CHAR_getChar(leaderindex, CHAR_FMNAME),
			CHAR_getChar(leaderindex, CHAR_NAME),
			CHAR_getChar(leaderindex, CHAR_CDKEY)
		);
		
		LogFamily(
			CHAR_getChar(charaindex, CHAR_FMNAME),
			CHAR_getInt(charaindex, CHAR_FMINDEX),
			CHAR_getChar(charaindex, CHAR_NAME),
			CHAR_getChar(charaindex, CHAR_CDKEY),
			"LEADERCHANGE(族长让位)",
			buf);
  }
}

