#include "version.h"
#include <string.h>
#include "char.h"
#include "object.h"
#include "char_base.h"
#include "npcutil.h"
#include "lssproto_serv.h"
#include "handletime.h"
#include "readmap.h"
#include "log.h"
#include "npc_eventaction.h"

#ifdef _GAMBLE_ROULETTE

#include "npc_gamblemaster.h"
void defPlayerGold( int meindex, int flg);
void NPC_GAMBLEMASTER_RESET( int meindex);
void Codef_Gold( int meindex, int toindex, int stone, int flg, char *token);
void NPC_GambleRoulette_selectWindow(int meindex,int toindex,int num,int select);

enum
{
	ROULETTE_START=0,
	ROULETTE_SELECT,
	ROULETTE_LOOK,
	ROULETTE_END,
};

enum
{
	roulette1 = 11,	roulette2, roulette3, roulette4, roulette5,	roulette6, roulette7, 
	roulette8, roulette9, roulette10, roulette11, roulette12, roulette13, roulette14,
	roulette15,	roulette16,	roulette17,	roulette18,	roulette19,	roulette20,	

	roulette21 = 41, roulette22, roulette23, roulette24, roulette25,	roulette26,	
	roulette27,	roulette28,	roulette29, roulette30, roulette31, roulette32,
	roulette33,	roulette34,	roulette35,	roulette36,	roulette37,	roulette38,
	roulette39,	roulette40,
	roulette41 = 71, roulette42, roulette43, roulette44, roulette45,

	roulette51 = 101, roulette52=102, roulette53=103,
	roulette61 = 111, roulette62=112,
};

typedef struct tagGambleMaster
{
	int m_fx;
	int m_fy;
	int m_type;	//号码 
	int m_RG;	//红绿1 2
	int m_EO;	//单双1 2
	int m_SI;
	int m_IN;
	char str_type[56];
	int m_nums;
}GambleEndType;
GambleEndType EndTypedef[60]={
{22, 8 , roulette1 , roulette41, roulette43, roulette51, roulette61, "号码红１", 0},
{7 , 8 , roulette1 , roulette41, roulette43, roulette51, roulette61, "号码红１", 0},
{22, 10, roulette3 , roulette41, roulette43, roulette51, roulette61, "号码红３", 0},
{7 , 6 , roulette3 , roulette41, roulette43, roulette51, roulette61, "号码红３", 0},
{20, 11, roulette5 , roulette41, roulette43, roulette51, roulette61, "号码红５", 0},
{9 , 5 , roulette5 , roulette41, roulette43, roulette51, roulette61, "号码红５", 0},
{19, 5 , roulette6 , roulette41, roulette44, roulette53, roulette61, "号码红６", 0},
{10, 11, roulette6 , roulette41, roulette44, roulette53, roulette61, "号码红６", 0},
{17, 5 , roulette8 , roulette41, roulette44, roulette53, roulette61, "号码红８", 0},
{12, 11, roulette8 , roulette41, roulette44, roulette53, roulette61, "号码红８", 0},
{16, 5 , roulette9 , roulette41, roulette43, roulette53, roulette61, "号码红９", 0},
{13, 11, roulette9 , roulette41, roulette43, roulette53, roulette61, "号码红９", 0},
{15, 5 , roulette10, roulette41, roulette44, roulette53, roulette61, "号码红１０", 0},
{14, 11, roulette10, roulette41, roulette44, roulette53, roulette61, "号码红１０", 0},
{12, 5 , roulette13, roulette41, roulette43, roulette52, roulette62, "号码红１３", 0},
{17, 11, roulette13, roulette41, roulette43, roulette52, roulette62, "号码红１３", 0},
{22, 6 , roulette17, roulette41, roulette43, roulette53, roulette62, "号码红１７", 0},
{7 , 10, roulette17, roulette41, roulette43, roulette53, roulette62, "号码红１７", 0},
{22, 5 , roulette18, roulette41, roulette44, roulette52, roulette62, "号码红１８", 0},
{7 , 11, roulette18, roulette41, roulette44, roulette52, roulette62, "号码红１８", 0},

{22, 9 , roulette22, roulette42, roulette44, roulette51, roulette61, "号码绿２", 0},
{7 , 7 , roulette22, roulette42, roulette44, roulette51, roulette61, "号码绿２", 0},
{21, 11, roulette24, roulette42, roulette44, roulette51, roulette61, "号码绿４", 0},
{8 , 5 , roulette24, roulette42, roulette44, roulette51, roulette61, "号码绿４", 0},
{18, 5 , roulette27, roulette42, roulette43, roulette53, roulette61, "号码绿７", 0},
{11, 11, roulette27, roulette42, roulette43, roulette53, roulette61, "号码绿７", 0},
{19, 11, roulette31, roulette42, roulette43, roulette51, roulette62, "号码绿１１", 0},
{10, 5 , roulette31, roulette42, roulette43, roulette51, roulette62, "号码绿１１", 0},
{18, 11, roulette32, roulette42, roulette44, roulette51, roulette62, "号码绿１２", 0},
{11, 5 , roulette32, roulette42, roulette44, roulette51, roulette62, "号码绿１２", 0},
{16, 11, roulette34, roulette42, roulette44, roulette52, roulette62, "号码绿１４", 0},
{14, 5 , roulette34, roulette42, roulette44, roulette52, roulette62, "号码绿１４", 0},
{15, 11, roulette35, roulette42, roulette43, roulette52, roulette62, "号码绿１５", 0},
{13, 5 , roulette35, roulette42, roulette43, roulette52, roulette62, "号码绿１５", 0},
{22, 7 , roulette36, roulette42, roulette44, roulette53, roulette62, "号码绿１６", 0},
{7 , 9 , roulette36, roulette42, roulette44, roulette53, roulette62, "号码绿１６", 0},
{21, 5 , roulette39, roulette42, roulette43, roulette52, roulette62, "号码绿１９", 0},
{8 , 11, roulette39, roulette42, roulette43, roulette52, roulette62, "号码绿１９", 0},
{20, 5 , roulette40, roulette42, roulette44, roulette52, roulette62, "号码绿２０", 0},
{9 , 11, roulette40, roulette42, roulette44, roulette52, roulette62, "号码绿２０", 0},
{22,11, roulette45, 0, 0, roulette52, 0, "号码００"},

{ 7, 5, -1, -1, -1, -1, -1, "号码Ｘ", 0},
{ -1, -1, -1, -1, -1, -1, -1, "无", 0},
};

enum {
	/*
	NPC_WORK_ROUTETOX = CHAR_NPCWORKINT1,
	NPC_WORK_ROUTETOY = CHAR_NPCWORKINT2,
	NPC_WORK_ROUTEPOINT = CHAR_NPCWORKINT3,
*/
	NPC_WORK_NPCAI = CHAR_NPCWORKINT3,
	NPC_WORK_NPCACTION = CHAR_NPCWORKINT3,
	NPC_WORK_MASTERSTONE = CHAR_NPCWORKINT4,
	NPC_WORK_MODE = CHAR_NPCWORKINT5,
	NPC_WORK_SYSTIME = CHAR_NPCWORKINT6,
	NPC_WORK_GAMEFLG = CHAR_NPCWORKINT7,
	NPC_WORK_GAMBLECODE = CHAR_NPCWORKINT8,
	NPC_WORK_MASTERFLG = CHAR_NPCWORKINT9,
	NPC_WORK_ENDPOINT = CHAR_NPCWORKINT10,
};

#define		ROULETTE_ERRTIME	500
#define		ROULETTE_STANDBY1	40
#define		ROULETTE_LOOPTIME	3000

BOOL NPC_Gamble_MasterInit( int meindex )
{
	char	argstr[NPC_UTIL_GETARGSTR_BUFSIZE];
	char	buf1[256];

	if( NPC_Util_GetArgStr( meindex, argstr, sizeof( argstr)) == NULL )	{
		print("\n not found argstr ");
		return FALSE;
	}

	if( NPC_Util_GetStrFromStrWithDelim( argstr, "gamble_code",buf1, sizeof(buf1)) == NULL ) {
		print("\n gamble_code error: not found !!");
		return FALSE;
	}
	CHAR_setWorkInt( meindex, NPC_WORK_GAMBLECODE, atoi( buf1));
	//游戏阶段
	CHAR_setWorkInt( meindex, NPC_WORK_GAMEFLG, 0);
	CHAR_setInt( meindex , CHAR_WHICHTYPE , CHAR_GAMBLEMASTER );
	// NPC_WORK_MODE 主持人状态 0: 等待 1:GAME START 2:GAME END
	CHAR_setWorkInt( meindex, NPC_WORK_MODE, 0);
	CHAR_setWorkInt( meindex, NPC_WORK_MASTERFLG, 30);
	CHAR_setWorkInt( meindex, NPC_WORK_MASTERSTONE, 0);
	CHAR_setWorkInt( meindex, NPC_WORK_NPCAI, 0);
	//设定LOOP TIMER
	CHAR_setInt( meindex, CHAR_LOOPINTERVAL, ROULETTE_LOOPTIME);
	//用来记录时间点
	CHAR_setWorkInt( meindex, NPC_WORK_SYSTIME, NowTime.tv_sec);
    return TRUE;
}

void NPC_Gamble_MasterTalked( int meindex , int talkerindex , char *szMes , int color )
{
	char argstr[NPC_UTIL_GETARGSTR_BUFSIZE];

	if( CHAR_getInt( talkerindex , CHAR_WHICHTYPE ) != CHAR_TYPEPLAYER ){
    		return;
	}
	if(NPC_Util_isFaceToFace( meindex ,talkerindex , 2) == FALSE) {
		if(NPC_Util_isFaceToChara( talkerindex, meindex, 1) == FALSE) return;
	}

	if( NPC_Util_GetArgStr( meindex, argstr, sizeof( argstr)) == NULL )	{
		print("\n not found argstr ");
		return;
	}
	// 0 选单 11 - 19 规则 21 - 29 积分
	CHAR_setWorkInt(talkerindex , CHAR_WORKSHOPRELEVANT,0);
	NPC_GambleRoulette_selectWindow( meindex, talkerindex, ROULETTE_START, -1 );
}

void NPC_GambleRoulette_selectWindow(int meindex,int toindex,int num,int select)
{
	char npcarg[NPC_UTIL_GETARGSTR_BUFSIZE];
	char buf1[256],buf2[256];
	char token[256];
	int buttontype = 0, windowtype = 0, windowno = 0;
	int fd = getfdFromCharaIndex( toindex);
	int pagenum=0, i;
	windowtype = WINDOW_MESSAGETYPE_MESSAGE;

	if( NPC_Util_GetArgStr( meindex, npcarg, sizeof( npcarg)) == NULL )	{
		print("\n not found npcarg ");
		CHAR_setWorkInt( toindex, CHAR_WORKSHOPRELEVANT,0);
		return;
	}
	switch( num)	{
	case ROULETTE_START:
		i = 1;
		strcpy( token, "\0");
		sprintf( buf2,"%s%d", "gamble_msg0", i);
		while( NPC_Util_GetStrFromStrWithDelim( npcarg, buf2, buf1, sizeof( buf1) ) != NULL )	{
			if( strstr( buf1, "NULL" ) == NULL )	{
				strcat( token , buf1);
				strcat( token, "\n");
			}else	{
				strcat( token, "\n");
			}
			i++;
			sprintf( buf2,"%s%d", "gamble_msg0", i);
		}
	  	windowtype=WINDOW_MESSAGETYPE_SELECT;
	  	windowno = WINDOWTYPE_GAMBLEROULETTE_START; 
		break;
	break;
	case ROULETTE_SELECT:
		//页 select
		pagenum = CHAR_getWorkInt( toindex , CHAR_WORKSHOPRELEVANT);
		strcpy( token, "\0");
		if( select >= 0 )	{
			sprintf( buf2,"%s%2d", "page_num", select);
			CHAR_setWorkInt( toindex , CHAR_WORKSHOPRELEVANT, select+1);
		}else	{
			sprintf( buf2,"%s%2d", "page_num", pagenum );
			pagenum+=1;
			CHAR_setWorkInt( toindex , CHAR_WORKSHOPRELEVANT, pagenum);
		}
		
		if( NPC_Util_GetStrFromStrWithDelim( npcarg, buf2, token, sizeof( token) ) == NULL )	{
			print("\n error: not found token:%s", token);
			CHAR_setWorkInt( toindex , CHAR_WORKSHOPRELEVANT,0);
			return;
		}

		sprintf( buf2,"%s%2d", "page_num", CHAR_getWorkInt( toindex , CHAR_WORKSHOPRELEVANT));
		if( NPC_Util_GetStrFromStrWithDelim( npcarg, buf2, buf1, sizeof( buf1) ) == NULL )	{
			windowno = WINDOWTYPE_GAMBLEROULETTE_END;
			buttontype = WINDOW_BUTTONTYPE_OK;
		}else	{
			windowno = WINDOWTYPE_GAMBLEROULETTE_SELECT;
			buttontype = WINDOW_BUTTONTYPE_NEXT;
		}
	break;
	case ROULETTE_LOOK:
		{	//显示时补回积分
			int G_num = CHAR_getInt( toindex, CHAR_GAMBLENUM );

			if( CHAR_getWorkInt( toindex, CHAR_WORKSTAKEFLAG) != FALSE)	{
				for( i=0; i<5; i++)	{
					if( CHAR_getWorkInt( toindex, i+CHAR_WORKSTAKETYPE1) > 0 )
						G_num++;
				}
			}
			if( G_num < 0 ) G_num = 0;
			sprintf( token,"你的游乐场积分为%d。", G_num );
			windowno = WINDOWTYPE_GAMBLEROULETTE_END;
			buttontype = WINDOW_BUTTONTYPE_OK;
		}
		break;
	case ROULETTE_END:
		if( NPC_Util_GetStrFromStrWithDelim( npcarg, "page_endmsg", token, sizeof( token) ) == NULL )	{
			print("\n error: not found end_msg");
			CHAR_setWorkInt( toindex , CHAR_WORKSHOPRELEVANT,0);
			return;
		}

		windowno = WINDOWTYPE_GAMBLEROULETTE_END;
		buttontype = WINDOW_BUTTONTYPE_OK;
	break;
	}

	lssproto_WN_send( fd, windowtype, buttontype, windowno, 
		CHAR_getWorkInt( meindex, CHAR_WORKOBJINDEX), token);
}

void NPC_Gamble_MasterWindowTalked( int meindex, int talkerindex, int seqno, int select, char *data)
{
	int pagenum=0;
	
	if( CHAR_getInt( talkerindex , CHAR_WHICHTYPE ) != CHAR_TYPEPLAYER ){
    		return;
	}
	if(NPC_Util_isFaceToFace( meindex ,talkerindex , 2) == FALSE) {
		if(NPC_Util_isFaceToChara( talkerindex, meindex, 1) == FALSE) return;
	}    

	switch( seqno )	{
	case WINDOWTYPE_GAMBLEROULETTE_START:
		pagenum = atoi( data);
		if( pagenum == 4 )	{
			NPC_GambleRoulette_selectWindow( meindex, talkerindex, ROULETTE_END, -1 );
		}else	{
			if( pagenum == 3 )	{	//ROULETTE_LOOK
				NPC_GambleRoulette_selectWindow( meindex, talkerindex, ROULETTE_LOOK, -1 );
			}else	{
				pagenum = (pagenum*10)+1;
				NPC_GambleRoulette_selectWindow( meindex, talkerindex, ROULETTE_SELECT, pagenum );
			}
		}
	break;
	case WINDOWTYPE_GAMBLEROULETTE_SELECT:
			NPC_GambleRoulette_selectWindow( meindex, talkerindex, ROULETTE_SELECT, -1 );
	break;
	case WINDOWTYPE_GAMBLEROULETTE_END:
			CHAR_setWorkInt(talkerindex , CHAR_WORKSHOPRELEVANT, 0);
	break;
	}
	return;
}

void NPC_Gamble_MasterLoop( int meindex)
{
	int	objmeindex = -1;
	int full_time;
	//int Master_Stone = 0;
	int timeNum = 0;
	//int time_run=0;
	//int act;
	objmeindex = CHAR_getWorkInt( meindex, CHAR_WORKOBJINDEX);
	switch( CHAR_getWorkInt( meindex, NPC_WORK_MODE )) {
	  case 0:	// 等待
		  full_time = ROULETTE_STANDBY1;
		  timeNum = CHAR_getWorkInt( meindex, NPC_WORK_MASTERFLG );
		  if( ( CHAR_getWorkInt( meindex, NPC_WORK_SYSTIME) + full_time) < NowTime.tv_sec )	{
			  if( timeNum == 30 )	{	//倒数20秒
				//0 null 1 准备 2 跑 3 停
				CHAR_setWorkInt( meindex, NPC_WORK_GAMEFLG, 1);
				showString( meindex, "下注时间剩下２０秒。", 0);
			  }else	if( timeNum == 10 )	{	
				  showString( meindex, "standby_msg", 0);	//广播
				  SetCasinoMap( meindex, 0, 0);	//设定地图不可下注
			  }else	if( timeNum <= 6 )	{	//开始
				  //0 null 1 准备 2 跑 3 停
				  CHAR_setWorkInt( meindex, NPC_WORK_GAMEFLG, 2);
				  CHAR_setWorkInt( meindex, NPC_WORK_MODE, 1);
				  showString( meindex, "start_msg", 0);	//广播
			  }
			  CHAR_setWorkInt( meindex, NPC_WORK_MASTERFLG, timeNum - 2 );
		  }
		break;
	  case 1:	// GAME START
		  if( CHAR_getWorkInt( meindex, NPC_WORK_GAMEFLG) == 3 )	{
			  CHAR_setWorkInt( meindex, NPC_WORK_MODE, 2);
		  }
		break;
	  case 2:	// GAME END
			CHAR_setWorkInt( meindex, NPC_WORK_MODE, 3);
			defPlayerGold( meindex , TRUE);	//计算输赢
			CHAR_setWorkInt( meindex, NPC_WORK_ENDPOINT, 0);
			NPC_MAPCLEANGOLD( meindex , CHAR_getInt( meindex, CHAR_FLOOR ));

			LogGamble(
				CHAR_getChar( meindex, CHAR_NAME ), "master",
				"ROULETTE",	CHAR_getInt( meindex, CHAR_FLOOR),
				CHAR_getInt( meindex, CHAR_X ),
				CHAR_getInt( meindex, CHAR_Y ) ,
				CHAR_getWorkInt( meindex, NPC_WORK_MASTERSTONE), 0, 0, 0, 2
			);
		break;
	  case 3:
		  CHAR_setWorkInt( meindex, NPC_WORK_MODE, 0);
		  CHAR_setWorkInt( meindex, NPC_WORK_GAMEFLG, 0);
		  showString( meindex, "end_msg", 0);	//广播
		  SetCasinoMap( meindex, 0, 1);	//设定地图可下注
		  CHAR_setWorkInt( meindex, NPC_WORK_MASTERFLG, 30 );
		  CHAR_setWorkInt( meindex, NPC_WORK_SYSTIME, NowTime.tv_sec);
		break;
	  default:
		  NPC_GAMBLEMASTER_RESET( meindex);
	    break;
	}
}

void defPlayerGold( int meindex , int flg)
{
	int X,Y;
	int endpoint;
	int End_type = 0;
	int toindex=0, i, master_floor;
	int player_type;
	int gamble_num;
	//float def;
	char token[256], buff[256];
	char buf1[56],buf2[56];
	BOOL GAMBLE_YES = FALSE;
	endpoint = CHAR_getWorkInt( meindex, NPC_WORK_ENDPOINT);
	
	X = (endpoint >> 16 );
	Y = (endpoint & 0xffff);

	for( End_type = 0; End_type < arraysizeof(EndTypedef) ; End_type++ )	{
		if( (X == EndTypedef[ End_type].m_fx) && (Y == EndTypedef[ End_type].m_fy ))	{
			break;
		}
	}

	if( End_type == arraysizeof(EndTypedef) )	{
		print("\n\n **********GAMBLE MASTER ERROR !! **************");
		print("\n NOT FOUND : X=%d Y=%d EndTypedef[I] !!", X, Y);
		return;
	}
/*Log=====================================
	{	//记录用
		FILE *fp;
		int kp=0,pn=0;;
		fp = fopen("./data/npc/roulette/lookgamble.txt","w+");
		if( fp != NULL )	{
			if( EndTypedef[ End_type].m_nums < 10000 )
				EndTypedef[ End_type].m_nums = EndTypedef[ End_type].m_nums+1;
			for( kp=0; kp<arraysizeof( EndTypedef)-1; kp++ )	{
				if( EndTypedef[ kp].m_nums > 0 && EndTypedef[ kp].m_type > 0 )	{
					fprintf( fp," %s 中奖次数 [%d] ", EndTypedef[ kp].str_type, EndTypedef[ kp].m_nums );
					if( (pn+1)%2 == 0 )	{
						fprintf( fp," \n");
					}
					pn ++;
				}
			}
			fclose( fp);
		}
	}
//========================================*/

	if( EndTypedef[ End_type].m_type <= 0 )	{
		showString( meindex, "号码Ｘ，通杀！", 0);
	}else	{
		sprintf( token ,"中奖号码 ： %s ", EndTypedef[End_type].str_type );
		showString( meindex, token, 0 );
	}
	if( EndTypedef[ End_type].m_EO == roulette43 )	{
		sprintf( buf1,"单");
	}else	{
		sprintf( buf1,"双");
	}
	if( EndTypedef[ End_type].m_RG == roulette41 )	{
		sprintf( buf2,"红");
	}else	{
		sprintf( buf2,"绿");
	}
	//找在场的人
	toindex = -1;
	master_floor = CHAR_getInt( meindex , CHAR_FLOOR);
	while( toindex < 10000 )	{
		toindex++;
		if( !CHAR_CHECKINDEX( toindex) )
			continue;
		if( CHAR_getInt( toindex , CHAR_WHICHTYPE ) != CHAR_TYPEPLAYER )
			continue;
		if( master_floor != CHAR_getInt( toindex, CHAR_FLOOR ) )	//是否在赌场
			continue;
		if( CHAR_getWorkInt( toindex, CHAR_WORKSTAKEFLAG) == FALSE )	//是否有下注
			continue;
		//是否赌中
		GAMBLE_YES = FALSE;
		CHAR_setWorkInt( toindex, CHAR_WORKSTAKEFLAG, FALSE);
		for( i=0; i<5; i++)	{
			player_type = CHAR_getWorkInt( toindex, i+CHAR_WORKSTAKETYPE1);
			gamble_num = 0;
			if( player_type > 0 )	{
				if( player_type == EndTypedef[End_type].m_type )	{	//中了号码
					if( EndTypedef[End_type].m_type == roulette45 )	{
						gamble_num += 40;
					}else	{
						gamble_num += 20;
					}
					GAMBLE_YES = TRUE;
					//算钱
					Codef_Gold( meindex, toindex, gamble_num, 0, EndTypedef[End_type].str_type );
				}else if( player_type == EndTypedef[End_type].m_RG )    {	//中了红绿
					gamble_num += 1;
					GAMBLE_YES = TRUE;
					Codef_Gold( meindex, toindex, gamble_num, 0, buf2 );
				}else if( player_type == EndTypedef[End_type].m_EO )	{	//中了单双
					gamble_num += 1;
					GAMBLE_YES = TRUE;
					Codef_Gold( meindex, toindex, gamble_num, 0, buf1 );
				}else if( player_type == EndTypedef[End_type].m_SI   )	{	//横列					
					gamble_num += 2;
					GAMBLE_YES = TRUE;
					sprintf( token,"%s%d", "横列", EndTypedef[End_type].m_SI-100 );
					Codef_Gold( meindex, toindex, gamble_num, 0, token );
				}else if( player_type == EndTypedef[End_type].m_IN   )	{	//1-10 11-20
					gamble_num += 1;
					GAMBLE_YES = TRUE;
					if( (EndTypedef[End_type].m_IN - 100 ) == 11 )	{
						snprintf( buff, sizeof( buff),"１－１０");
					}else if( (EndTypedef[End_type].m_IN - 100 ) == 12 )	{
						snprintf( buff, sizeof( buff),"１１－２０");
					}
					sprintf( token,"%s%s", "范围", buff );
					Codef_Gold( meindex, toindex, gamble_num, 0, token );
				}else	{	//没中	扣积分
					if( (player_type>>16) == EndTypedef[End_type].m_type || 
						(player_type&0xffff) == EndTypedef[End_type].m_type
						)	{	//中双号
						gamble_num += 10;
						GAMBLE_YES = TRUE;
						sprintf( token,"%s%s", "双号－", EndTypedef[End_type].str_type );
						Codef_Gold( meindex, toindex, gamble_num, 0, token );
					}else if( player_type > 0 ) {	//72 73 74 75 没中	扣积分
						strcpy( token, "\0");
						if( player_type >= roulette41 && player_type <= roulette44 )	{	//红绿单双
							sprintf( token,"%s", "押注 红绿单双 没中");
						}else if( player_type == roulette45 )	{	//数字00
							sprintf( token,"%s", "押注 数字00 没中");
						}else if( player_type >= roulette51 && player_type <= roulette53 )	{	//横列
							sprintf( token,"%s", "押注 横列 没中");
						}else if( player_type >= roulette61 && player_type <= roulette62 )	{	//范围
							sprintf( token,"%s", "押注 范围 没中");
						}else if( player_type > (1<<16) )	{	//双号
							sprintf( token,"%s", "押注 双号 没中");
						}else	{	//数字
							sprintf( token,"%s", "押注 数字 没中");
						}
						gamble_num -= 1;
						Codef_Gold( meindex, toindex, gamble_num, 1, token );
					}
				}
			}
			//清空
			CHAR_setWorkInt( toindex, i+CHAR_WORKSTAKETYPE1, 0);
		}
		if( CHAR_getInt( toindex, CHAR_GAMBLENUM) < 0 )	{
			CHAR_setInt( toindex, CHAR_GAMBLENUM, 0 );
		}else if( CHAR_getInt( toindex, CHAR_GAMBLENUM) > 10000 )	{
			CHAR_setInt( toindex, CHAR_GAMBLENUM, 10000 );
		}
		sprintf( token, "你的游乐场积分累计为%d分。", CHAR_getInt( toindex, CHAR_GAMBLENUM));
		CHAR_talkToCli( toindex, meindex, token,  CHAR_COLORYELLOW);

		if( GAMBLE_YES == FALSE )	{
			sprintf( token, "你所有押注都没中奖！");
			CHAR_talkToCli( toindex, meindex, token,  CHAR_COLORYELLOW);		
            continue;
		}

	}
	return;
}

void Codef_Gold( int meindex, int toindex, int stone,int flg, char *token)
{
	char buf1[256];
	int dnum=-1;
	int Master_gnum=0;
	Master_gnum = CHAR_getWorkInt( meindex, NPC_WORK_MASTERSTONE);
	Master_gnum += stone;
	if( !flg )	{
		sprintf( buf1,"恭喜你中了：%s，可拿得 %d 点积分", token, stone);
		CHAR_talkToCli( toindex, meindex, buf1,  CHAR_COLORYELLOW);
#ifdef _FIX_GAMBLENUM
		stone += 1;	//加上一开始下注扣的积分
#endif
		CHAR_setInt( toindex, CHAR_GAMBLENUM, CHAR_getInt( toindex, CHAR_GAMBLENUM) + stone);

	}else	{
		dnum = stone;
		if( stone < 0 )	{
			dnum = (dnum*(-1));
		}

		sprintf( buf1,"%s，扣 %d 点积分", token, dnum);
		CHAR_talkToCli( toindex, meindex, buf1,  CHAR_COLORYELLOW);
#ifdef _FIX_GAMBLENUM
#else
		CHAR_setInt( toindex, CHAR_GAMBLENUM, CHAR_getInt( toindex, CHAR_GAMBLENUM) + stone);
#endif
	}
	
	LogGamble(
		CHAR_getChar( toindex, CHAR_NAME ),
		CHAR_getChar( toindex, CHAR_CDKEY ),
		"ROULETTE",
		CHAR_getInt( toindex, CHAR_FLOOR),
		CHAR_getInt( toindex, CHAR_X ),
		CHAR_getInt( toindex, CHAR_Y ),
		CHAR_getInt( toindex, CHAR_GOLD),
		stone,
		0,
		CHAR_getInt( toindex, CHAR_GAMBLENUM ),
		1
	);
	
	//纪录盈亏
	if( Master_gnum > 5000000 )	Master_gnum = 5000000;
	if( Master_gnum < 0 ) Master_gnum = 0;
	CHAR_setWorkInt( meindex, NPC_WORK_MASTERSTONE, Master_gnum);
	return;
}
//重置 错误处理
void NPC_GAMBLEMASTER_RESET( int meindex)
{
	//错误处理
	print("\n 错误处理");
	CHAR_setWorkInt( meindex, NPC_WORK_GAMEFLG, 0);//0 null 1 准备 2 跑
	CHAR_setWorkInt( meindex, NPC_WORK_MODE, 0);
	showString( meindex, "本回合不算！等５分钟後重新开始！！", 1);
	NPC_MAPCLEANGOLD( meindex , CHAR_getInt( meindex, CHAR_FLOOR ));
	//defPlayerGold( meindex , FALSE);
	CHAR_setWorkInt( meindex, NPC_WORK_SYSTIME, NowTime.tv_sec + ROULETTE_ERRTIME);
}

#endif



