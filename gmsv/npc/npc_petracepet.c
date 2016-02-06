#include "version.h"
#include <string.h>
#include "char.h"
#include "object.h"
#include "char_base.h"
#include "npcutil.h"
#include "lssproto_serv.h"
#include "handletime.h"
 
#ifdef _PETRACE
#include "npc_petracepet.h"

#define PETRACEPET_WAIT_LOOPTIME			30000	// 30秒
#define PETRACEPET_SHOWWIN_LOOPTIME			10000	// 10秒

static void	NPC_PetRacePet_Walk(int meindex);
static int NPC_PetRacePetSetPoint(int meindex, char *argstr);

enum
{
	NPC_WORK_STATE =  CHAR_NPCWORKINT1,
	NPC_WORK_MASTER = CHAR_NPCWORKINT2,
	NPC_WORK_ROUTEPOINT = CHAR_NPCWORKINT3,
	NPC_WORK_ROUTETOX = CHAR_NPCWORKINT4,
	NPC_WORK_ROUTETOY = CHAR_NPCWORKINT5,
	NPC_WORK_WAITFLAG = CHAR_NPCWORKINT6,
	NPC_WORK_FLOOR = CHAR_NPCWORKINT7,
	NPC_WORK_X = CHAR_NPCWORKINT8,
	NPC_WORK_Y = CHAR_NPCWORKINT9,
	NPC_WORK_DIR = CHAR_NPCWORKINT10,
	NPC_WORK_STOPX = CHAR_NPCWORKINT11,
	NPC_WORK_STOPY = CHAR_NPCWORKINT12,
};
enum
{
	NPC_State_Wait,
	NPC_State_Run,
	NPC_State_ShowWin,
};

int raceflag=0;

BOOL NPC_PetRacePetInit( int meindex )
{
	char argstr[NPC_UTIL_GETARGSTR_BUFSIZE];
	char buf[256];
	int showstopx, showstopy;
	int routenum = 0, i = 0;
	
	NPC_Util_GetArgStr(meindex, argstr, sizeof(argstr));
	for (i = 1; i <= routenum; i++)
	{
		char routetostring[64];
		snprintf(routetostring, sizeof(routetostring), "routeto");
		if (NPC_Util_GetStrFromStrWithDelim(argstr, routetostring, buf, sizeof(buf)) == NULL)
		{
			print("Init_npcracepet: Didn't set route\n");
			return FALSE;
		}
	}
	showstopx = NPC_Util_GetNumFromStrWithDelim(argstr, "showstopx");
	showstopy = NPC_Util_GetNumFromStrWithDelim(argstr, "showstopy");
	if (showstopx < 0 || showstopx > 100 || showstopy < 0 || showstopy > 100)
		return FALSE;
	CHAR_setInt(meindex, CHAR_WHICHTYPE, CHAR_PETRACEPET);
	CHAR_setWorkInt(meindex, NPC_WORK_STATE, NPC_State_Wait);
	CHAR_setWorkInt(meindex, NPC_WORK_ROUTEPOINT, 2);
	CHAR_setWorkInt(meindex, NPC_WORK_WAITFLAG, 1);
	CHAR_setWorkInt(meindex, NPC_WORK_FLOOR, CHAR_getInt(meindex, CHAR_FLOOR));
	CHAR_setWorkInt(meindex, NPC_WORK_X, CHAR_getInt(meindex, CHAR_X));
	CHAR_setWorkInt(meindex, NPC_WORK_Y, CHAR_getInt(meindex, CHAR_Y));
	CHAR_setWorkInt(meindex, NPC_WORK_DIR, CHAR_getInt(meindex, CHAR_DIR));
	CHAR_setWorkInt(meindex, NPC_WORK_STOPX, showstopx);
	CHAR_setWorkInt(meindex, NPC_WORK_STOPY, showstopy);
	CHAR_setInt(meindex, CHAR_LOOPINTERVAL, PETRACEPET_WAIT_LOOPTIME);
	NPC_PetRacePetSetPoint(meindex, argstr);

	return	TRUE;
}

void NPC_PetRacePetTalked( int meindex , int talkerindex , char *szMes , int color )
{
}

void NPC_PetRacePetLoop( int meindex)
{
	switch(CHAR_getWorkInt(meindex, NPC_WORK_STATE))
	{
		case NPC_State_Wait:
			{
				if (CHAR_getWorkInt(meindex, NPC_WORK_WAITFLAG) == 0)
				{
					char argstr[NPC_UTIL_GETARGSTR_BUFSIZE];
					NPC_Util_GetArgStr(meindex, argstr, sizeof(argstr));
					CHAR_setWorkInt(meindex, NPC_WORK_ROUTEPOINT, 2);
					NPC_PetRacePetSetPoint(meindex, argstr);
					CHAR_setInt(meindex, CHAR_LOOPINTERVAL, PETRACEPET_WAIT_LOOPTIME);
					CHAR_setInt(meindex, CHAR_DIR, CHAR_getWorkInt(meindex, NPC_WORK_DIR));
					CHAR_warpToSpecificPoint(meindex,
						CHAR_getWorkInt(meindex, NPC_WORK_FLOOR),
						CHAR_getWorkInt(meindex, NPC_WORK_X),
						CHAR_getWorkInt(meindex, NPC_WORK_Y));
					CHAR_setWorkInt(meindex, NPC_WORK_WAITFLAG, 1);
				}
			}
			break;
		case NPC_State_Run:
			{
				int interval = 50;
				int random = 0;
				if (CHAR_getWorkInt(meindex, NPC_WORK_ROUTETOX) <= CHAR_getWorkInt(meindex, NPC_WORK_STOPX)
					&& CHAR_getWorkInt(meindex, NPC_WORK_ROUTETOY) >= CHAR_getWorkInt(meindex, NPC_WORK_STOPY))
						random = 3;
				else
					random = RAND(0, 8);
				if (random == 0) // 攻击
				{
					int rands = RAND(0, 3);
					interval = 1500;
					CHAR_sendAction(meindex, 0, FALSE);
					if (rands == TRUE)
					{
						char tmpbuf[256];
						int floor = CHAR_getInt(meindex, CHAR_FLOOR);
						int master = CHAR_getWorkInt(meindex, NPC_WORK_MASTER);
						int randomi = RAND(1, 9);
						if (randomi == 1)
							snprintf(tmpbuf, sizeof(tmpbuf), "哦～看来%s好像发威了唷！",
								CHAR_getChar(meindex, CHAR_NAME));
						else if (randomi == 2)
							snprintf(tmpbuf, sizeof(tmpbuf), "%s使出了奋力的一击～",
								CHAR_getChar(meindex, CHAR_NAME));
						else if (randomi == 3)
							snprintf(tmpbuf, sizeof(tmpbuf), "咦...%s发怒了？",
								CHAR_getChar(meindex, CHAR_NAME));
						if (randomi > 0 && randomi < 4)
							CHAR_talkToFloor(floor, master, tmpbuf, CHAR_COLORWHITE);
					}
				}
				else if (random == 1) // 晕倒
				{
					int rands = RAND(0, 3);
					interval = 2500;
					CHAR_sendAction(meindex, 2, FALSE);
					if (rands == TRUE)
					{
						char tmpbuf[256];
						int floor = CHAR_getInt(meindex, CHAR_FLOOR);
						int master = CHAR_getWorkInt(meindex, NPC_WORK_MASTER);
						int randomi = RAND(1, 9);
						if (randomi == 1)
							snprintf(tmpbuf, sizeof(tmpbuf), "%s好像晕倒了呢...",
								CHAR_getChar(meindex, CHAR_NAME));
						else if (randomi == 2)
							snprintf(tmpbuf, sizeof(tmpbuf), "%s受伤了吗？",
								CHAR_getChar(meindex, CHAR_NAME));
						else if (randomi == 3)
							snprintf(tmpbuf, sizeof(tmpbuf), "嘿...%s晕倒後又勇敢的爬起来了！",
								CHAR_getChar(meindex, CHAR_NAME));
						if (randomi > 0 && randomi < 4)
							CHAR_talkToFloor(floor, master, tmpbuf, CHAR_COLORWHITE);
					}
				}
				else if (random == 2) // 站立
				{
					interval = 500;
					CHAR_sendAction(meindex, 3, FALSE);
				}
				else // 一般
					interval = RAND(20, 100);
				NPC_PetRacePet_Walk(meindex);
				CHAR_setInt(meindex, CHAR_LOOPINTERVAL, interval);
			}
			break;
		case NPC_State_ShowWin:
			{
				int action = RAND(0, 11);
				CHAR_sendAction(meindex, action, FALSE);
				CHAR_setInt(meindex, CHAR_LOOPINTERVAL, PETRACEPET_SHOWWIN_LOOPTIME);
			}
			break;
		default:
			break;
	}
}

static void NPC_PetRacePet_Walk(int meindex)
{
	POINT start, end;
	int dir, i;

	start.x = CHAR_getInt(meindex, CHAR_X);
	start.y = CHAR_getInt(meindex, CHAR_Y);
	end.x = CHAR_getWorkInt(meindex, NPC_WORK_ROUTETOX);
	end.y = CHAR_getWorkInt(meindex, NPC_WORK_ROUTETOY);
	if (start.x == end.x && start.y == end.y)
	{
		char argstr[NPC_UTIL_GETARGSTR_BUFSIZE];
		NPC_Util_GetArgStr(meindex, argstr, sizeof( argstr));
		CHAR_setWorkInt(meindex, NPC_WORK_ROUTEPOINT, 
			CHAR_getWorkInt(meindex, NPC_WORK_ROUTEPOINT) + 1);

		if (NPC_PetRacePetSetPoint(meindex, argstr) == FALSE)
		{
			// 到达目的地後，初始状态
			char tmpbuf[256];
			int masterindex = CHAR_getWorkInt(meindex, NPC_WORK_MASTER);
			int petgoal = CHAR_getWorkInt(masterindex, CHAR_NPCWORKINT9);
			int floor = CHAR_getInt(masterindex, CHAR_FLOOR);
			if (CHAR_getWorkInt(masterindex, CHAR_NPCWORKINT8) == 0)
			{
				CHAR_warpToSpecificPoint(meindex, 7005,42,44);
				CHAR_setWorkInt(masterindex, CHAR_NPCWORKINT8, meindex);
				CHAR_sendAction(meindex, 3, FALSE);
				snprintf(tmpbuf, sizeof(tmpbuf), "首先到达目的地的是...%s",
					CHAR_getChar(meindex, CHAR_NAME));
				raceflag++;
			}
			else
			{
				if(raceflag == 1)
				{
					//将宠物送回到指定的坐标
					CHAR_warpToSpecificPoint(meindex, 7005,43,47);
					snprintf(tmpbuf, sizeof(tmpbuf), "接着到达目的地的是...%s",
						CHAR_getChar(meindex, CHAR_NAME));
					raceflag++;
				}
				else
				{
					//完成最后一个宠物的传送，初始化
					CHAR_warpToSpecificPoint(meindex, 7005,45,44);
					snprintf(tmpbuf, sizeof(tmpbuf), "最后到达目的地的是...%s",
						CHAR_getChar(meindex, CHAR_NAME));
					raceflag=0;
				}
			}
			CHAR_setWorkInt(meindex, NPC_WORK_STATE, NPC_State_ShowWin);
			CHAR_setInt(meindex, CHAR_LOOPINTERVAL, 500);
			CHAR_setWorkInt(meindex, NPC_WORK_WAITFLAG, 0);
			CHAR_setWorkInt(masterindex, CHAR_NPCWORKINT9, petgoal + 1);
			CHAR_talkToFloor(floor, masterindex, tmpbuf, CHAR_COLORWHITE);
			return;
		}
		else
			return;
	}
	dir = NPC_Util_getDirFromTwoPoint(&start, &end);
	end.x = CHAR_getInt(meindex, CHAR_X);
	end.y = CHAR_getInt(meindex, CHAR_Y);
	for(i = 0; i < 100; i++)
	{
		if (dir < 0)
			dir = RAND(0, 7);
		dir = NPC_Util_SuberiWalk(meindex, dir);
		if (dir >= 0 && dir <= 7) break;
	}
	if (dir >= 0 && dir <= 7)
	{
		int result = 0;
		result = CHAR_walk(meindex, dir, 0);
	}
}

static int NPC_PetRacePetSetPoint(int meindex, char *argstr)
{
	// 寻找下一步 x , y
	char buf[4096], buf2[256], buf3[256];
	int ret;
	char routetostring[64];
	snprintf(routetostring, sizeof(routetostring), "routeto");
	if (NPC_Util_GetStrFromStrWithDelim(argstr, routetostring, buf, sizeof(buf)) == NULL)
	{
		print("SetPoint_npcracepet: Didn't set route %s\n", routetostring);
		return FALSE;
	}
	ret = getStringFromIndexWithDelim(buf, ";", 
		CHAR_getWorkInt(meindex, NPC_WORK_ROUTEPOINT), buf2, sizeof(buf2));
	if (ret == FALSE)	return FALSE;
	ret = getStringFromIndexWithDelim(buf2, ",", 1, buf3, sizeof(buf3));
	if (ret == FALSE)	return FALSE;
	CHAR_setWorkInt(meindex, NPC_WORK_ROUTETOX, atoi(buf3));
	ret = getStringFromIndexWithDelim(buf2, ",", 2, buf3, sizeof(buf3));
	if (ret == FALSE)	return FALSE;
	CHAR_setWorkInt(meindex, NPC_WORK_ROUTETOY, atoi(buf3));
	return TRUE;
}

#endif


