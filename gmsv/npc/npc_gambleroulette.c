#include "version.h"
#include <string.h>
#include "char.h"
#include "object.h"
#include "char_base.h"
#include "npcutil.h"
#include "readmap.h"
#include "lssproto_serv.h"
#include "handletime.h"

#ifdef _GAMBLE_ROULETTE 
#include "npc_gambleroulette.h"

#define _OTHER_ROUND	//另一种跑法

static void Gamble_Roulette_walk( int meindex);
static int Gamble_RouletteSetPoint( int meindex );
static void Find_Master( int meindex);
int RunRand( int meindex, int flg );
BOOL SetEndPoint( int meindex );
void Gamble_Roulette_Reset( int meindex, int flg);
void ResetDataStart( int meindex);
BOOL ReadPointData( int meindex );
enum {
	NPC_WORK_ROUTETOX = CHAR_NPCWORKINT1,
	NPC_WORK_ROUTETOY = CHAR_NPCWORKINT2,
	NPC_WORK_ROUTEPOINT = CHAR_NPCWORKINT3,
	NPC_WORK_ROUNDTRIP = CHAR_NPCWORKINT4,
	NPC_WORK_MODE = CHAR_NPCWORKINT5,
	NPC_WORK_CURRENTROUTE = CHAR_NPCWORKINT6, 
	NPC_WORK_ROUTEMAX = CHAR_NPCWORKINT7,
	NPC_WORK_WAITTIME = CHAR_NPCWORKINT8,
	NPC_WORK_CURRENTTIME = CHAR_NPCWORKINT9,
	NPC_WORK_SEFLG = CHAR_NPCWORKINT10,	//记录主持人index
};

typedef struct tagRoulettePoint {
	int x;
	int y;
	int flg;
}RoulettePoint;
RoulettePoint PointData[]={
	{-1,-1,-1},{-1,-1,-1},{-1,-1,-1},{-1,-1,-1},	
};

RoulettePoint TestPointData[4*8];

#define		ROULETTE_STANDBY	1500
#define		ROULETTE_LOOPTIME	1000

#define		ROULETTE_RUNTIME1	50

BOOL NPC_Gamble_RouletteInit( int meindex )
{
	char	argstr[NPC_UTIL_GETARGSTR_BUFSIZE];

	NPC_Util_GetArgStr( meindex, argstr, sizeof( argstr));

	CHAR_setInt( meindex , CHAR_WHICHTYPE , CHAR_GAMBLEROULETTE );
	//CHAR_setWorkInt( meindex, NPC_WORK_ROUTEMAX, routenum);
	//小猪要走几格才停
	CHAR_setWorkInt( meindex, NPC_WORK_ROUTEMAX, 0);
	CHAR_setWorkInt( meindex, NPC_WORK_SEFLG, -1);
	// NPC_WORK_MODE 小猪的状态 0:等待 1:走(顺)
	CHAR_setWorkInt( meindex, NPC_WORK_MODE, 0);
	//第几个地图点
	CHAR_setWorkInt( meindex, NPC_WORK_ROUTEPOINT, 0);
	//顺向 1  逆向  0
	CHAR_setWorkInt( meindex, NPC_WORK_ROUNDTRIP, 1);
	//第几个路线
	CHAR_setWorkInt( meindex, NPC_WORK_CURRENTROUTE, 0);
	//设定LOOP TIMER
	CHAR_setInt( meindex, CHAR_LOOPINTERVAL, ROULETTE_STANDBY);
	//用来记录时间点
	CHAR_setWorkInt( meindex, NPC_WORK_CURRENTTIME, NowTime.tv_sec);
	CHAR_sendCToArroundCharacter( CHAR_getWorkInt( meindex, CHAR_WORKOBJINDEX));
	if( ReadPointData( meindex) == FALSE )
		return FALSE;
    return TRUE;
}

void NPC_Gamble_RouletteTalked( int meindex , int talkerindex , char *szMes , int color )
{
	return;
}

void NPC_Gamble_RouletteWindowTalked( int meindex, int talkerindex, int seqno, int select, char *data)
{
	return;
}

void NPC_Gamble_RouletteLoop( int meindex)
{
	int	objmeindex = -1, index = 0, act = 0;
	objmeindex = CHAR_getWorkInt( meindex, CHAR_WORKOBJINDEX);
	switch( CHAR_getWorkInt( meindex, NPC_WORK_MODE )) {
	  case 0:	//停止时等待	为等待主持人下命令
		  
		  //抓主持人index 检查主持人 是否喊开始
		  if( CHAR_getWorkInt( meindex, NPC_WORK_SEFLG) < 0 )	{
			CHAR_setWorkInt( meindex, NPC_WORK_MODE,5);
		  }else	{
			index = CHAR_getWorkInt( meindex, NPC_WORK_SEFLG);
			//CHAR_NPCWORKINT7 0 null 1 准备 2 跑 3 停
			if( CHAR_getWorkInt( index, CHAR_NPCWORKINT7 ) == 2 )	{
				CHAR_sendCToArroundCharacter( objmeindex);
				CHAR_setWorkInt( meindex, NPC_WORK_MODE,1);
				CHAR_setInt( meindex, CHAR_LOOPINTERVAL, ROULETTE_RUNTIME1);
				//决定小猪走几格
				CHAR_setWorkInt( meindex, NPC_WORK_ROUTEMAX, RunRand( meindex, 0 ) );
				ResetDataStart( meindex);//决定进点
				if( CHAR_getInt( meindex, CHAR_X) == 14 &&
					CHAR_getInt( meindex, CHAR_Y) == 8 )	{
				}else	{
					print("\n\n####################\n 小猪不在原点!![%d,%d]",
						CHAR_getInt( meindex, CHAR_X),
						CHAR_getInt( meindex, CHAR_Y)
					);

				}
			}else if( CHAR_getWorkInt( index, CHAR_NPCWORKINT7 ) == 1 ||
				( CHAR_getWorkInt( index, CHAR_NPCWORKINT9) < 30 &&
				CHAR_getWorkInt( index, CHAR_NPCWORKINT9) > 24 )
				)	{	//准备
				if( CHAR_getInt( meindex, CHAR_X) == 14 && CHAR_getInt( meindex, CHAR_Y) == 8 )	{
				}else	{
					CHAR_warpToSpecificPoint( meindex, CHAR_getInt( meindex, CHAR_FLOOR),
					14, 8);
					CHAR_setInt( meindex, CHAR_X, 14);
					CHAR_setInt( meindex, CHAR_Y, 8);
					CHAR_sendCToArroundCharacter( objmeindex);
				}
			}
		  }
		break;
	  case 1:
	  		Gamble_Roulette_walk( meindex);
		break;
	  case 2:
		break;
	  case 3:
		  //点归零
		  CHAR_setWorkInt( meindex, NPC_WORK_MODE, 0);
		  //动作 
		  act = CHAR_ACTATTACK;
		  CHAR_sendWatchEvent( objmeindex, act, NULL,0, FALSE);
		  CHAR_setWorkInt( meindex, CHAR_WORKACTION, act);
		  CHAR_setWorkInt( meindex, NPC_WORK_ROUTEMAX, 0);
		  CHAR_setInt( meindex, CHAR_LOOPINTERVAL, ROULETTE_STANDBY);

		  //送人物资料给视觉范围内的人
		  CHAR_sendCToArroundCharacter( objmeindex);
		  if( SetEndPoint( meindex ) == FALSE )	{
			print("\n not Set EndPoint !!");
		  }
#ifdef _OTHER_ROUND
#else
		  ReadPointData( meindex);	//load round data
#endif
		  //还原主持人
		  index = CHAR_getWorkInt( meindex, NPC_WORK_SEFLG);
		  CHAR_setWorkInt( index, CHAR_NPCWORKINT7, 3);
		break;
	  case 5:	//寻找主持人
			Find_Master( meindex);
			if( CHAR_getWorkInt( meindex, NPC_WORK_SEFLG) < 0 )	{
				CHAR_setInt( meindex, CHAR_LOOPINTERVAL, 0);
				CHAR_setWorkInt( meindex, NPC_WORK_MODE,-1);
			}else	{
				CHAR_setWorkInt( meindex, NPC_WORK_MODE, 0);			
			}
		break;
	  default:
		  //重置 错误处理
		  Gamble_Roulette_Reset( meindex, 0);
	    break;
	}
}

static void Gamble_Roulette_walk( int meindex)
{	
	POINT	start, end;
	int dir;
	int ret;
	int i,run_num = 0;
	int	objmeindex = -1;
	objmeindex = CHAR_getWorkInt( meindex, CHAR_WORKOBJINDEX);

	start.x = CHAR_getInt( meindex, CHAR_X);
	start.y = CHAR_getInt( meindex, CHAR_Y);
	end.x = CHAR_getWorkInt( meindex, NPC_WORK_ROUTETOX);
	end.y = CHAR_getWorkInt( meindex, NPC_WORK_ROUTETOY);
	// loop timer分为两部分 1. 处理下一点 2. 走到下一点

	if( start.x == end.x && start.y == end.y ) {
#ifdef _OTHER_ROUND
#else
		int add = 1;
		//如果为 NPC_WORK_ROUNDTRIP != 1  逆向
		if( CHAR_getWorkInt( meindex, NPC_WORK_ROUNDTRIP ) != 1 ) {
			add *= -1;
		}
		CHAR_setWorkInt( meindex, NPC_WORK_ROUTEPOINT, 
						CHAR_getWorkInt( meindex, NPC_WORK_ROUTEPOINT) +add);
#endif
		if( Gamble_RouletteSetPoint( meindex ) == FALSE ) {
#ifdef _OTHER_ROUND
			CHAR_setWorkInt( meindex, NPC_WORK_MODE,3);
			CHAR_setWorkInt( meindex, NPC_WORK_CURRENTTIME, NowTime.tv_sec);
			return;
#else
			if( CHAR_getWorkInt( meindex, NPC_WORK_ROUNDTRIP ) != 1 ) {
				CHAR_setWorkInt( meindex, NPC_WORK_ROUTEPOINT, arraysizeof( PointData) );	//逆向
			}else	{
				CHAR_setWorkInt( meindex, NPC_WORK_ROUTEPOINT, -1);	//顺向
			}
			return;
#endif
		}else	{
			return;
		}
	}
//--------------------------------------------------------------------
		run_num = CHAR_getWorkInt( meindex, NPC_WORK_ROUTEMAX);
		if( run_num > 0 ){
			CHAR_setWorkInt( meindex, NPC_WORK_ROUTEMAX, (run_num - 1) );
		}else	{
#ifdef _OTHER_ROUND
			CHAR_setWorkInt( meindex, NPC_WORK_MODE,3);
			CHAR_setWorkInt( meindex, NPC_WORK_CURRENTTIME, NowTime.tv_sec);
			return;
#else
			int npc_ai=0;
			int masterindex;
			masterindex = CHAR_getWorkInt( meindex, NPC_WORK_SEFLG);
			npc_ai = CHAR_getWorkInt( masterindex, CHAR_NPCWORKINT3);
			if( npc_ai > 10 )	{
				if( start.x == 7 && start.y == 5 )	{	// 7,5
					CHAR_setWorkInt( masterindex, CHAR_NPCWORKINT3, 0);
				}else	{
					CHAR_setWorkInt( meindex, NPC_WORK_ROUTEMAX, 1);
					return;
				}
			}
			CHAR_setWorkInt( meindex, NPC_WORK_MODE,3);
			CHAR_setWorkInt( meindex, NPC_WORK_CURRENTTIME, NowTime.tv_sec);
			return;
#endif
		}
//--------------------------------------------------------------------
	dir = NPC_Util_getDirFromTwoPoint( &start,&end );
	for( i = 0; i < 100; i ++ ) {	
		if( dir < 0 ) {
			dir = RAND( 0,7);
		}
		dir = NPC_Util_SuberiWalk( meindex, dir);
		if( dir >= 0 && dir <= 7) break;
	}
	if( dir >= 0 && dir <= 7 ) {
		ret = CHAR_walk( meindex, dir, 0);
	}
}

static void Find_Master( int meindex)
{
	int floor, x=14, y=3;
	OBJECT  object;
	floor = CHAR_getInt( meindex , CHAR_FLOOR);
	for( object = MAP_getTopObj( floor, x, y) ; object ; object = NEXT_OBJECT(object ) ){
		int findex;
		int objindex = GET_OBJINDEX(object);
		if( !CHECKOBJECTUSE( objindex)) continue;
		findex = OBJECT_getIndex( objindex);
		if( !CHAR_CHECKINDEX( findex)) continue;
		if( CHAR_getInt( findex , CHAR_WHICHTYPE ) != CHAR_GAMBLEMASTER ) continue;
		if( CHAR_getWorkInt( findex, CHAR_NPCWORKINT8) != 10001 ) continue;
		CHAR_setWorkInt( meindex, NPC_WORK_SEFLG, findex) ;
		return;
	}
}

int RunRand( int meindex, int flg )
{
#define RAND(x,y)   ((x-1)+1+ (int)( (double)(y-(x-1))*rand()/(RAND_MAX+1.0)) )
	int run_num1=0;
	int i;
	
	run_num1 = RAND( 90, 150);
	for( i=0; i<6; i++ )	{
		run_num1 += RAND( 0, 20);
	}
	return run_num1;
}

BOOL SetEndPoint( int meindex )
{
	int x,y;
	int master;
	int endpoint;

	master = CHAR_getWorkInt( meindex, NPC_WORK_SEFLG);
	if( !CHAR_CHECKINDEX( master) )
		return FALSE;

	if( CHAR_getWorkInt( master, CHAR_NPCWORKINT7) != 2 )	{
		print("\n error( master,CHAR_NPCWORKINT7) != 2 ");
		return FALSE;
	}

	x = CHAR_getInt( meindex, CHAR_X);
	y = CHAR_getInt( meindex, CHAR_Y);
	endpoint = (x<<16)+(y<<0);
	CHAR_setWorkInt( master, CHAR_NPCWORKINT10, endpoint);
	return TRUE;
}

void Gamble_Roulette_Reset( int meindex, int flg)
{
	//错误处理
	int index;
	CHAR_setWorkInt( meindex, NPC_WORK_ROUTEMAX, 0);
	CHAR_setWorkInt( meindex, NPC_WORK_MODE, 0);
	CHAR_setInt( meindex, CHAR_LOOPINTERVAL, ROULETTE_STANDBY);

	index = CHAR_getWorkInt( meindex, NPC_WORK_SEFLG);
	if( !CHAR_CHECKINDEX( index) )	{
		print("\n Gamble_Roulette error: no master !!");
		return;
	}
	CHAR_setWorkInt( index, CHAR_NPCWORKINT7, 3);
	CHAR_setWorkInt( index, CHAR_NPCWORKINT10, 0);
}

void ResetDataStart( int meindex )
{
#define RAND(x,y)   ((x-1)+1+ (int)( (double)(y-(x-1))*rand()/(RAND_MAX+1.0)) )
	int Start=1;
	struct tagWalkStartPoint{
		int x;
		int y;
		int rip;
	}WalkStart[]={
		{7,5,1}, {7,11,0}, {22,11,1}, {22,5,0},
	};
	Start = RAND( 0, 3);

	CHAR_setWorkInt( meindex, NPC_WORK_ROUNDTRIP, WalkStart[Start].rip );
	CHAR_setWorkInt( meindex, NPC_WORK_ROUTEPOINT, Start);
	CHAR_setWorkInt( meindex, NPC_WORK_ROUTETOX, WalkStart[Start].x );
	CHAR_setWorkInt( meindex, NPC_WORK_ROUTETOY, WalkStart[Start].y );
	
	{
		int i=0,k;
		int RIP = CHAR_getWorkInt( meindex, NPC_WORK_ROUNDTRIP);
		k = CHAR_getWorkInt( meindex, NPC_WORK_ROUTEPOINT);
		if( RIP )	{
			while( i < (arraysizeof( TestPointData)))	{
				k++;
				if( k > 3 ) k = 0;
				TestPointData[i].x = PointData[k].x;
				TestPointData[i].y = PointData[k].y;
				i++;
			}
		}else	{
			while( i < (arraysizeof( TestPointData)))	{
				k--;
				if( k < 0 ) k = 3;
				TestPointData[i].x = PointData[k].x;
				TestPointData[i].y = PointData[k].y;
				i++;
			}
		}
	}
#ifdef _OTHER_ROUND
	CHAR_setWorkInt( meindex, NPC_WORK_ROUTEPOINT, 0);
#endif
}

BOOL ReadPointData( int meindex )
{

	char	buf1[256], buf2[32], buf3[16];
	char	argstr[NPC_UTIL_GETARGSTR_BUFSIZE];
	int k=1,i=0;
	NPC_Util_GetArgStr( meindex, argstr, sizeof( argstr));

	if( NPC_Util_GetStrFromStrWithDelim( argstr, "roulette",buf1, sizeof(buf1)) == NULL ) {
		print("\n roulette error: not found !!");
		return FALSE;
	}
	k=1;
	while( getStringFromIndexWithDelim( buf1, ";", k, buf2, sizeof(buf2) ) != FALSE )	{
		k++;
		if( strstr( buf2,",") == NULL )
			continue;

		if( getStringFromIndexWithDelim( buf2, ",", 1, buf3, sizeof(buf3)) == FALSE)	{
			return FALSE;
		}
		PointData[i].x = atoi( buf3);
		if( getStringFromIndexWithDelim( buf2, ",", 2, buf3, sizeof(buf3)) == FALSE)	{
			return FALSE;
		}
		PointData[i].y = atoi( buf3);
		if( i > 9 )	{
			break;
		}
		i++;
	}
#ifdef _OTHER_ROUND
#else
	Gamble_RouletteSetPoint( meindex);
#endif
	return TRUE;//arraysizeof( PointData)
}

static int Gamble_RouletteSetPoint( int meindex )
{
#ifdef _OTHER_ROUND
	int i;
	i = CHAR_getWorkInt( meindex, NPC_WORK_ROUTEPOINT);
	CHAR_setWorkInt( meindex, NPC_WORK_ROUTETOX, TestPointData[i].x );
	CHAR_setWorkInt( meindex, NPC_WORK_ROUTETOY, TestPointData[i].y );
	i++;
	CHAR_setWorkInt( meindex, NPC_WORK_ROUTEPOINT, i);
	if( i >= arraysizeof( TestPointData) )	{
		return FALSE;
	}
#else
	int i;
	i = CHAR_getWorkInt( meindex, NPC_WORK_ROUTEPOINT);
	if( i<0 || i>arraysizeof( PointData) -1)	{
		return FALSE;
	}
	CHAR_setWorkInt( meindex, NPC_WORK_ROUTETOX, PointData[i].x );
	CHAR_setWorkInt( meindex, NPC_WORK_ROUTETOY, PointData[i].y );
#endif
	{//检查路线
		struct tagWalkStartPoint{
			int x;
			int y;
		}WalkStart[]={
			{22,5}, {7,5}, {7,11}, {22,11}, {22,5}, {7,5}
		};
		int ch_x,ch_y;
		int next_x,next_y;
		int h;
		ch_x = CHAR_getInt( meindex, CHAR_X);
		ch_y = CHAR_getInt( meindex, CHAR_Y);
		for( h=1; h<5; h++)	{
			if( ch_x == WalkStart[h].x &&
				ch_y == WalkStart[h].y)	{
				next_x = CHAR_getWorkInt( meindex, NPC_WORK_ROUTETOX);
				next_y = CHAR_getWorkInt( meindex, NPC_WORK_ROUTETOY);
				if( next_x == WalkStart[h+1].x &&
					next_y == WalkStart[h+1].y)	{

				}else if( next_x == WalkStart[h-1].x &&
							next_y == WalkStart[h-1].y)	{
				}else	{
					print("\n ##############################");
					print("\n  ERR : [%d,%d] -> [%d,%d]",ch_x,ch_y,next_x,next_y);
				}
				break;
			}
		}
	}
	return TRUE;
}

#endif



