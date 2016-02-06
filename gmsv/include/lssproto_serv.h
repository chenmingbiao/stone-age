#ifndef _PROTOCOL_H_
#define _PROTOCOL_H_

#include "lssproto_util.h"	// for StoneAge

#ifdef MAXLSRPCARGS
#if ( MAXLSRPCARGS <= ( 13 + 1 )  )
#undef MAXLSRPCARGS
#define MAXLSRPCARGS ( 13 + 1 ) 
#endif
#else
#define MAXLSRPCARGS ( 13 + 1 ) 
#endif

int lssproto_InitServer(int (*writefunc)(int,char*,int) , int worksiz );
void lssproto_SetServerLogFiles( char *read , char *write );
void lssproto_CleanupServer( void );
int lssproto_ServerDispatchMessage(int fd, char *encoded);

#define SEPARATOR	";"

#define LSSPROTO_W_RECV 	0
#define LSSPROTO_W2_RECV 	1
#define LSSPROTO_XYD_SEND 	2
#define LSSPROTO_EV_RECV 	3
#define LSSPROTO_EV_SEND 	4
#define LSSPROTO_EN_RECV 	5
#define LSSPROTO_DU_RECV 	6
#define LSSPROTO_EN_SEND 	7
#define LSSPROTO_EO_RECV 	8
#define LSSPROTO_BU_RECV 	9
#define LSSPROTO_JB_RECV 	10
#define LSSPROTO_LB_RECV 	11
#define LSSPROTO_RS_SEND 	12
#define LSSPROTO_RD_SEND 	13
#define LSSPROTO_B_RECV 	14
#define LSSPROTO_B_SEND 	15
#define LSSPROTO_SKD_RECV 	16
#define LSSPROTO_ID_RECV 	17
#define LSSPROTO_PI_RECV 	18
#define LSSPROTO_DI_RECV 	19
#define LSSPROTO_DG_RECV 	20
#define LSSPROTO_DP_RECV 	21
#define LSSPROTO_I_SEND 	22
#define LSSPROTO_MI_RECV 	23
#define LSSPROTO_SI_SEND 	24
#define LSSPROTO_MSG_RECV 	25
#define LSSPROTO_MSG_SEND 	26
#define LSSPROTO_PMSG_RECV 	27
#define LSSPROTO_PME_SEND 	28
#define LSSPROTO_AB_RECV 	29
#define LSSPROTO_AB_SEND 	30
#define LSSPROTO_ABI_SEND 	31
#define LSSPROTO_DAB_RECV 	32
#define LSSPROTO_AAB_RECV 	33
#define LSSPROTO_L_RECV 	34
#define LSSPROTO_TK_RECV 	35
#define LSSPROTO_TK_SEND 	36
#define LSSPROTO_MC_SEND 	37
#define LSSPROTO_M_RECV 	38
#define LSSPROTO_M_SEND 	39
#define LSSPROTO_C_RECV 	40
#define LSSPROTO_C_SEND 	41
#define LSSPROTO_CA_SEND 	42
#define LSSPROTO_CD_SEND 	43
#define LSSPROTO_R_SEND 	44
#define LSSPROTO_S_RECV 	45
#define LSSPROTO_S_SEND 	46
#define LSSPROTO_D_SEND 	47
#define LSSPROTO_FS_RECV 	48
#define LSSPROTO_FS_SEND 	49
#define LSSPROTO_HL_RECV 	50
#define LSSPROTO_HL_SEND 	51
#define LSSPROTO_PR_RECV 	52
#define LSSPROTO_PR_SEND 	53
#define LSSPROTO_KS_RECV 	54
#define LSSPROTO_KS_SEND 	55
#define LSSPROTO_AC_RECV 	56
#define LSSPROTO_MU_RECV 	57
#define LSSPROTO_PS_RECV 	58
#define LSSPROTO_PS_SEND 	59
#define LSSPROTO_ST_RECV 	60
#define LSSPROTO_DT_RECV 	61
#define LSSPROTO_FT_RECV 	62
#define LSSPROTO_SKUP_SEND 	63
#define LSSPROTO_SKUP_RECV 	64
#define LSSPROTO_KN_RECV 	65
#define LSSPROTO_WN_SEND 	66
#define LSSPROTO_WN_RECV 	67
#define LSSPROTO_EF_SEND 	68
#define LSSPROTO_SE_SEND 	69
#define LSSPROTO_SP_RECV 	70
#define LSSPROTO_CLIENTLOGIN_RECV 	71
#define LSSPROTO_CLIENTLOGIN_SEND 	72
#define LSSPROTO_CREATENEWCHAR_RECV 	73
#define LSSPROTO_CREATENEWCHAR_SEND 	74
#define LSSPROTO_CHARDELETE_RECV 	75
#define LSSPROTO_CHARDELETE_SEND 	76
#define LSSPROTO_CHARLOGIN_RECV 	77
#define LSSPROTO_CHARLOGIN_SEND 	78
#define LSSPROTO_CHARLIST_RECV 	79
#define LSSPROTO_CHARLIST_SEND 	80
#define LSSPROTO_CHARLOGOUT_RECV 	81
#define LSSPROTO_CHARLOGOUT_SEND 	82
#define LSSPROTO_PROCGET_RECV 	83
#define LSSPROTO_PROCGET_SEND 	84
#define LSSPROTO_PLAYERNUMGET_RECV 	85
#define LSSPROTO_PLAYERNUMGET_SEND 	86
#define LSSPROTO_ECHO_RECV 	87
#define LSSPROTO_ECHO_SEND 	88
#define LSSPROTO_SHUTDOWN_RECV 	89
#define LSSPROTO_NU_SEND 	90
#define LSSPROTO_TD_RECV 	91
#define LSSPROTO_TD_SEND 	92
#define LSSPROTO_FM_SEND 	93
#define LSSPROTO_FM_RECV 	94
#define LSSPROTO_WO_SEND 	95
#define LSSPROTO_PETST_RECV	96

#ifdef _MIND_ICON
#define LSSPROTO_MA_RECV    98
#endif

#ifdef _ITEM_CRACKER
#define LSSPROTO_IC_SEND    100
#endif

#ifdef _MAGIC_NOCAST             // ¾«Áé:³ÁÄ¬
#define LSSPROTO_NC_SEND    101
#endif

#ifdef _PETS_SELECTCON
#define LSSPROTO_PETST_SEND 107
#endif
#define LSSPROTO_SPET_RECV 	114		// Robin ´ý»ú³è
#define LSSPROTO_SPET_SEND 	115


void lssproto_W_recv(int fd,int x,int y,char* direction);
void lssproto_W2_recv(int fd,int x,int y,char* direction);
void lssproto_XYD_send(int fd,int x,int y,int dir);
void lssproto_EV_recv(int fd,int event,int seqno,int x,int y,int dir);
void lssproto_EV_send(int fd,int seqno,int result);
void lssproto_EN_recv(int fd,int x,int y);
void lssproto_DU_recv(int fd,int x,int y);
void lssproto_EN_send(int fd,int result,int field);
void lssproto_EO_recv(int fd,int dummy);
void lssproto_BU_recv(int fd,int dummy);
void lssproto_JB_recv(int fd,int x,int y);
void lssproto_LB_recv(int fd,int x,int y);
void lssproto_RS_send(int fd,char* data);
void lssproto_RD_send(int fd,char* data);
void lssproto_B_recv(int fd,char* command);
void lssproto_B_send(int fd,char* command);
void lssproto_SKD_recv(int fd,int dir,int index);
void lssproto_ID_recv(int fd,int x,int y,int haveitemindex,int toindex);
void lssproto_PI_recv(int fd,int x,int y,int dir);
void lssproto_DI_recv(int fd,int x,int y,int itemindex);
void lssproto_DG_recv(int fd,int x,int y,int amount);
void lssproto_DP_recv(int fd,int x,int y,int petindex);
void lssproto_I_send(int fd,char* data);
void lssproto_MI_recv(int fd,int fromindex,int toindex);
void lssproto_SI_send(int fd,int fromindex,int toindex);
void lssproto_MSG_recv(int fd,int index,char* message,int color);
void lssproto_MSG_send(int fd,int aindex,char* text,int color);
void lssproto_PMSG_recv(int fd,int index,int petindex,int itemindex,char* message,int color);
void lssproto_PME_send(int fd,int objindex,int graphicsno,int x,int y,int dir,int flg,int no,char* cdata);
void lssproto_AB_recv(int fd);
void lssproto_AB_send(int fd,char* data);
void lssproto_ABI_send(int fd,int num,char* data);
void lssproto_DAB_recv(int fd,int index);
void lssproto_AAB_recv(int fd,int x,int y);
void lssproto_L_recv(int fd,int dir);
void lssproto_TK_recv(int fd,int x,int y,char* message,int color,int area);
void lssproto_TK_send(int fd,int index,char* message,int color);
void lssproto_MC_send(int fd,int fl,int x1,int y1,int x2,int y2,int tilesum,int objsum,int eventsum,char* data);
void lssproto_M_recv(int fd,int fl,int x1,int y1,int x2,int y2);
void lssproto_M_send(int fd,int fl,int x1,int y1,int x2,int y2,char* data);
void lssproto_C_recv(int fd,int index);
void lssproto_C_send(int fd,char* data);
void lssproto_CA_send(int fd,char* data);
void lssproto_CD_send(int fd,char* data);
void lssproto_R_send(int fd,char* data);
void lssproto_S_recv(int fd,char* category);
void lssproto_S_send(int fd,char* data);
void lssproto_D_send(int fd,int category,int dx,int dy,char* data);
void lssproto_FS_recv(int fd,int flg);
void lssproto_FS_send(int fd,int flg);
void lssproto_HL_recv(int fd,int flg);
void lssproto_HL_send(int fd,int flg);
void lssproto_PR_recv(int fd,int x,int y,int request);
void lssproto_PR_send(int fd,int request,int result);
void lssproto_KS_recv(int fd,int petarray);
void lssproto_KS_send(int fd,int petarray,int result);

#ifdef _STANDBYPET
void lssproto_SPET_recv(int fd,int standbypet);
void lssproto_SPET_send(int fd,int standbypet,int result);
#endif

void lssproto_AC_recv(int fd,int x,int y,int actionno);
void lssproto_MU_recv(int fd,int x,int y,int array,int toindex);
void lssproto_PS_recv(int fd,int havepetindex,int havepetskill,int toindex,char* data);
void lssproto_PS_send(int fd,int result,int havepetindex,int havepetskill,int toindex);
void lssproto_ST_recv(int fd,int titleindex);
void lssproto_DT_recv(int fd,int titleindex);
void lssproto_FT_recv(int fd,char* data);
void lssproto_SKUP_send(int fd,int point);
void lssproto_SKUP_recv(int fd,int skillid);
void lssproto_KN_recv(int fd,int havepetindex,char* data);
void lssproto_WN_send(int fd,int windowtype,int buttontype,int seqno,int objindex,char* data);
void lssproto_WN_recv(int fd,int x,int y,int seqno,int objindex,int select,char* data);
void lssproto_EF_send(int fd,int effect,int level,char* option);
void lssproto_SE_send(int fd,int x,int y,int senumber,int sw);
void lssproto_SP_recv(int fd,int x,int y,int dir);
void lssproto_ClientLogin_recv(int fd,char* cdkey,char* passwd);
void lssproto_ClientLogin_send(int fd,char* result);
void lssproto_CreateNewChar_recv(int fd,int dataplacenum,char* charname,int imgno,int faceimgno,int vital,int str,int tgh,int dex,int earth,int water,int fire,int wind,int hometown);
void lssproto_CreateNewChar_send(int fd,char* result,char* data);
void lssproto_CharDelete_recv(int fd,char* charname);
void lssproto_CharDelete_send(int fd,char* result,char* data);
void lssproto_CharLogin_recv(int fd,char* charname);
void lssproto_CharLogin_send(int fd,char* result,char* data);

#ifdef _PKSEVER_VER
void lssproto_CharList_recv( int fd, int star);
#else
void lssproto_CharList_recv(int fd);
#endif

void lssproto_CharList_send(int fd,char* result,char* data);
void lssproto_CharLogout_recv(int fd, int flg);
void lssproto_CharLogout_send(int fd,char* result,char* data);
void lssproto_ProcGet_recv(int fd);
void lssproto_ProcGet_send(int fd,char* data);
void lssproto_PlayerNumGet_recv(int fd);
void lssproto_PlayerNumGet_send(int fd,int logincount,int player);
void lssproto_Echo_recv(int fd,char* test);
void lssproto_Echo_send(int fd,char* test);
void lssproto_Shutdown_recv(int fd,char* passwd,int min);

void lssproto_TD_send(int fd, int index, char* message);
void lssproto_TD_recv(int fd, char* message);

void lssproto_NU_send(int fd, int nu);

void lssproto_FM_send(int fd, char* message);
void lssproto_FM_recv(int fd, char* message);

void lssproto_WO_send(int fd,int effect);
void lssproto_PETST_recv( int fd,  int nPet, int sPet);
void lssproto_BM_recv(int fd, int iindex);

#ifdef _MIND_ICON
void lssproto_MA_recv(int fd, int x, int y, int nMind);
#endif

#ifdef _ITEM_CRACKER
void lssproto_IC_send(int fd, int x, int y);
#endif

#ifdef _ITEM_CRACKER
void lssproto_NC_send(int fd,int flg);
#endif

#ifdef _PETS_SELECTCON
void lssproto_PETS_send(int fd,int petarray,int result);
//#define LSSPROTO_PETST_SEND 107
#endif


#endif


