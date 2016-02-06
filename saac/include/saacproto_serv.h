/* output by ../../bin/lsgen.perl 0.41 ( 1998 May)
 * made Thu Feb 17 17:30:27 2000
 * user ringo
 * host indy
 * file /opt/ringo/chsa/src/saac/./saacproto_serv.h
 * util ./saacproto_util.c , ./saacproto_util.h
 * src  /opt/ringo/chsa/src/saac/../../doc/saacproto.html
 */
#ifndef _SAACPROTOSERV_H_
#define _SAACPROTOSERV_H_
#include "version.h"
#include "saacproto_util.h"

#ifdef MAXLSRPCARGS
#if ( MAXLSRPCARGS <= ( 7 + 1 )  )
#undef MAXLSRPCARGS
#define MAXLSRPCARGS ( 7 + 1 ) 
#endif
#else
#define MAXLSRPCARGS ( 7 + 1 ) 
#endif
#ifdef _ALLDOMAN					// Syu ADD 排行榜NPC
void saacproto_UpdataStele_recv( int fd , char *cdkey , char *name , char *title , int level , int trns , int time , int floor) ;
void saacproto_UpdataStele_send( int fd , char *data ) ;
void saacproto_S_UpdataStele_send( int fd , char *ocdkey , char *oname , char *ncdkey , 
					  char *nname , char *title , int level , int trns , int floor ) ;

#endif
#ifdef _VIP
void saacproto_ACServerLogin_recv( int ti,char* servername , char* serverpas, int checkvip );
#else
void saacproto_ACServerLogin_recv( int fd,char* servername,char* serverpas ) ;
#endif
void saacproto_ACServerLogin_send( int fd,char* result,char* data ) ;
void saacproto_ACServerLogout_recv( int fd ) ;

void saacproto_ACCharList_recv( int ti,char* id,char* pas , int mesgid);

void saacproto_ACCharList_send( int fd,char* result,char* output,int id ) ; /* ../../doc/saacproto.html line 124 */
void saacproto_ACCharLoad_recv( int fd,char* id,char* pas,char* charname,int lock,char* opt,int mesgid ) ; /* ../../doc/saacproto.html line 148 */
// CoolFish: 2001/10/16
#ifdef _NewSave
void saacproto_ACCharLoad_send( int fd,char* result,char* data,int id,int charindex ) ; /* ../../doc/saacproto.html line 175 */
void saacproto_ACCharSave_recv( int fd,char* id,char* charname,char* opt,char* charinfo,int unlock,int mesgid,int charindex ) ; /* ../../doc/saacproto.html line 191 */
#else
void saacproto_ACCharLoad_send( int fd,char* result,char* data,int id ) ; /* ../../doc/saacproto.html line 175 */
void saacproto_ACCharSave_recv( int fd,char* id,char* charname,char* opt,char* charinfo,int unlock,int mesgid ) ; /* ../../doc/saacproto.html line 191 */
#endif
void saacproto_ACCharSave_send( int fd,char* result,char* data,int id ) ; /* ../../doc/saacproto.html line 214 */

void saacproto_ACCharDelete_recv( int fd,char* id,char* passwd,char* charname,char* option,int mesgid ) ; /* ../../doc/saacproto.html line 231 */
void saacproto_ACCharDelete_send( int fd,char* result,char* data,int id ) ; /* ../../doc/saacproto.html line 256 */
void saacproto_ACLock_recv( int fd,char* id,int lock,int mesgid ) ; /* ../../doc/saacproto.html line 271 */
void saacproto_ACLock_send( int fd,char* result,char* data,int id ) ; /* ../../doc/saacproto.html line 290 */
void saacproto_ACUCheck_recv( int fd,char* mem_id,int status ) ; /* ../../doc/saacproto.html line 304 */
void saacproto_ACUCheck_send( int fd,char* mem_id ) ; /* ../../doc/saacproto.html line 318 */
void saacproto_DBUpdateEntryString_recv( int fd,char* table,char* key,char* value,int msgid,int msgid2 ) ; /* ../../doc/saacproto.html line 329 */
void saacproto_DBUpdateEntryString_send( int fd,char* result,char* table,char* key,int msgid,int msgid2 ) ; /* ../../doc/saacproto.html line 344 */
void saacproto_DBDeleteEntryString_recv( int fd,char* table,char* key,int msgid,int msgid2 ) ; /* ../../doc/saacproto.html line 357 */
void saacproto_DBDeleteEntryString_send( int fd,char* result,char* table,char* key,int msgid,int msgid2 ) ; /* ../../doc/saacproto.html line 369 */
void saacproto_DBGetEntryString_recv( int fd,char* table,char* key,int msgid,int msgid2 ) ; /* ../../doc/saacproto.html line 384 */
void saacproto_DBGetEntryString_send( int fd,char* result,char* value,char* table,char* key,int msgid,int msgid2 ) ; /* ../../doc/saacproto.html line 396 */
void saacproto_DBUpdateEntryInt_recv( int fd,char* table,char* key,int value,char* info,int msgid,int msgid2 ) ; /* ../../doc/saacproto.html line 413 */
void saacproto_DBUpdateEntryInt_send( int fd,char* result,char* table,char* key,int msgid,int msgid2 ) ; /* ../../doc/saacproto.html line 435 */
void saacproto_DBGetEntryRank_recv( int fd,char* table,char* key,int msgid,int msgid2 ) ; /* ../../doc/saacproto.html line 449 */
void saacproto_DBGetEntryRank_send( int fd,char* result,int rank,int count,char* table,char* key,int msgid,int msgid2 ) ; /* ../../doc/saacproto.html line 463 */
void saacproto_DBDeleteEntryInt_recv( int fd,char* table,char* key,int msgid,int msgid2 ) ; /* ../../doc/saacproto.html line 482 */
void saacproto_DBDeleteEntryInt_send( int fd,char* result,char* table,char* key,int msgid,int msgid2 ) ; /* ../../doc/saacproto.html line 494 */
void saacproto_DBGetEntryInt_recv( int fd,char* table,char* key,int msgid,int msgid2 ) ; /* ../../doc/saacproto.html line 508 */
void saacproto_DBGetEntryInt_send( int fd,char* result,int value,char* table,char* key,int msgid,int msgid2 ) ; /* ../../doc/saacproto.html line 520 */
void saacproto_DBGetEntryByRank_recv( int fd,char* table,int rank_start,int rank_end,int msgid,int msgid2 ) ; /* ../../doc/saacproto.html line 537 */
void saacproto_DBGetEntryByRank_send( int fd,char* result,char* list,char* table,int msgid,int msgid2 ) ; /* ../../doc/saacproto.html line 552 */
void saacproto_DBGetEntryByCount_recv( int fd,char* table,int count_start,int num,int msgid,int msgid2 ) ; /* ../../doc/saacproto.html line 571 */
void saacproto_DBGetEntryByCount_send( int fd,char* result,char* list,char* table,int count_start,int msgid,int msgid2 ) ; /* ../../doc/saacproto.html line 583 */
void saacproto_Broadcast_recv( int fd,char* id,char* charname,char* message,int flag ) ; /* ../../doc/saacproto.html line 598 */
void saacproto_Broadcast_send( int fd,char* id,char* charname,char* message ) ; /* ../../doc/saacproto.html line 611 */
void saacproto_Message_recv( int fd,char* id_from,char* charname_from,char* id_to,char* charname_to,char* message,int option ) ; /* ../../doc/saacproto.html line 624 */
void saacproto_Message_send( int fd,char* id_from,char* charname_from,char* id_to,char* charname_to,char* message,int option,int mesgid ) ; /* ../../doc/saacproto.html line 642 */
void saacproto_MessageAck_recv( int fd,char* id,char* charname,char* result,int mesgid ) ; /* ../../doc/saacproto.html line 661 */
void saacproto_MessageFlush_recv( int fd,char* id,char* charname ) ; /* ../../doc/saacproto.html line 680 */
int saacproto_InitServer(int (*writefunc)(int,char*,int) ,	int worksiz );
void saacproto_SetServerLogFiles( char *read , char *write );
void saacproto_CleanupServer( void );
//int saacproto_ServerDispatchMessage( int fd, char *line );
int saacproto_ServerDispatchMessage( int fd , char *encoded, char *debugfun);

// CoolFish: Family 2001/5/9
// 成立家族
#ifdef _PERSONAL_FAME
void saacproto_ACAddFM_recv(int fd, char *fmname, char *fmleadername,
	char *fmleaderid, int fmleaderlv, char *petname, char *petarrt,
	char *fmrule, int fmsprite, int fmleadergrano, int fame, int charfdid);
#else
void saacproto_ACAddFM_recv(int fd, char *fmname, char *fmleadername,
	char *fmleaderid, int fmleaderlv, char *petname, char *petarrt,
	char *fmrule, int fmsprite, int fmleadergrano, int charfdid);
#endif
void saacproto_ACAddFM_send(int fd, char *result, int fmindex, int index,
	int charfdid);
// 加入家族
// #ifdef _PERSONAL_FAME	// Arminius: 家族个人声望
void saacproto_ACJoinFM_recv(int fd, char *fmname, int fmindex, char *charname,
	char *charid, int charlv, int index, int fame, int charfdid);
// #else
// void saacproto_ACJoinFM_recv(int fd, char *fmname, int fmindex, char *charname,
//	char *charid, int charlv, int index, int charfdid);
// #endif
void saacproto_ACJoinFM_send(int fd, char *result, int recv, int charfdid);
// 退出家族
void saacproto_ACLeaveFM_recv(int fd, char *fmname, int fmindex, char *charname,
	char *charid, int index, int charfdid);
void saacproto_ACLeaveFM_send(int fd, char *result, int resultflag, int charfdid);
// 解散家族
#ifdef _LEADERFUNCHECK
void saacproto_ACDelFM_recv(int fd, char *fmname, int fmindex, int index, char *charname,
	char *charid, int charfdid);
#else
void saacproto_ACDelFM_recv(int fd, char *fmname, int fmindex, int index, int charfdid);
#endif
void saacproto_ACDelFM_send(int fd, char *result, int charfdid);
// 列出家族列表
void saacproto_ACShowFMList_recv(int fd);
void saacproto_ACShowFMList_send(int fd, char *result, int num, char *data);
// 列出成员列表
void saacproto_ACShowMemberList_recv(int fd, int index);
void saacproto_ACShowMemberList_send(int fd, char *result, int index, int num,
	int acceptflag, int fmjoinnum, char *data);
// 列出家族详细资料
void saacproto_ACFMDetail_recv(int fd, char *fmname, int fmindex,
	int index,  int charfdid);
void saacproto_ACFMDetail_send(int fd, char *result, char *data, int charfdid);
// 列出家族留言板
void saacproto_ACFMReadMemo_recv(int fd, int index);
void saacproto_ACFMReadMemo_send(int fd, char *result, int index, int num,
	int dataindex, char *data);
// 写入家族留言板
void saacproto_ACFMWriteMemo_recv(int fd, char *fmname, int fmindex,
	char *data, int index);
void saacproto_ACFMWriteMemo_send(int fd, char *result, int index);
// 玩家login
#ifdef _FMVER21
void saacproto_ACFMCharLogin_recv(int fd, char *fmname, int fmindex,
	char *charname, char *charid, int charlv, int eventflag, int charfdid);
#else
void saacproto_ACFMCharLogin_recv(int fd, char *fmname, int fmindex,
	char *charname, char *charid, int charlv, int charfdid);
#endif
#ifdef _PERSONAL_FAME   // Arminius: 家族个人声望
void saacproto_ACFMCharLogin_send(int fd, char *result, int index, int floor,
	int fmpopular, int joinflag, int fmsetupflag, int flag, int charindex,
	int charfame, int charfdid
	);
#else
void saacproto_ACFMCharLogin_send(int fd, char *result, int index, int floor,
	int fmpopular, int joinflag, int fmsetupflag, int flag, int charindex,
	int charfdid);
#endif

// 玩家logout
void saacproto_ACFMCharLogout_recv(int fd, char *fmname, int fmindex,
	char *charname, char *charid, int charlv, int index, int charfdid);
void saacproto_ACFMCharLogout_send(int fd, char *result, int charfdid);
// 族长审核成员加入家族、修改家族成员职位
#ifdef _FMVER21
void saacproto_ACMemberJoinFM_recv(int fd, char* fmname, int fmindex,
	char* charname, int charindex, int index, int result, int meindex,
	int charfdid);
#else
void saacproto_ACMemberJoinFM_recv(int fd, char* fmname, int fmindex,
	char* charname, int charindex, int index, int result, int charfdid);
#endif
void saacproto_ACMemberJoinFM_send(int fd, char* result, int charfdid);
// 族长审核成员离开家族
#ifdef _FMVER21
void saacproto_ACMemberLeaveFM_recv(int fd, char* fmname, int fmindex,
	char* charname, int charindex, int index, int meindex, int charfdid);
#else
void saacproto_ACMemberLeaveFM_recv(int fd, char* fmname, int fmindex,
	char* charname, int charindex, int index, int charfdid);
#endif
void saacproto_ACMemberLeaveFM_send(int fd, char* result, int charfdid);
// 列出家族据点
void saacproto_ACFMPointList_recv(int fd);
void saacproto_ACFMPointList_send(int fd, char *result, char *data);

// 申请家族据点
void saacproto_ACSetFMPoint_recv(int fd, char *fmname, int fmindex, int index,
	int fmpointindex, int fl, int x, int y, int charfdid);
void saacproto_ACSetFMPoint_send(int fd, char *result, int r, int charfdid);
// 设定家族据点
void saacproto_ACFixFMPoint_recv(int fd, char *winfmname, int winfmindex,
	int winindex, char *losefmname, int losefmindex, int loseindex,
	int village);
void saacproto_ACFixFMPoint_send(int fd, char *result, int r);
// 族长对星系家族成员广播
// kindflag 1:族长广播 2:家族被系统删除 3:系统通知讯息
void saacproto_ACFMAnnounce_recv(int fd, char *fmname, int fmindex, int index,
	char *data, int color);
void saacproto_ACFMAnnounce_send(int fd, char *result, char *fmname,
	int fmindex, int index, int kindflag, char *data, int color);
// 列出家族排行榜
void saacproto_ACShowTopFMList_recv(int fd, int kindflag);
void saacproto_ACShowTopFMList_send(int fd, char *result, int kindflag, int num, char *data);
// 修正家族资料(是否招收成员)
// kindflag 1:是否继续招募家族成员 2:家族ＰＫ声望 3:家族守护兽 4:家族主旨
//          5:解散家族时间 6:家族基金 7:家族冒险声望 8:家族饲育声望
//          9:家族合成、加工声望 10:家族料理声望
void saacproto_ACFixFMData_recv(int fd, char *fmname, int fmindex, int index,
        int kindflag, char *data1, char *data2, int charindex, int charfdid);
void saacproto_ACFixFMData_send(int fd, char *result, int kindflag,
	char *data1, char *data2, int charfdid);
// ＰＫ後更新家族声望
void saacproto_ACFixFMPK_recv(int fd, char *winfmname, int winfmindex,
	int winindex, char *losefmname, int losefmindex, int loseindex);
void saacproto_ACFixFMPK_send(int fd, char *result, int data, int winindex,
	int loseindex);
// 取得家族资料
// kindflag 1:家族银行
void saacproto_ACGetFMData_recv(int fd, char *fmname, int fmindex, int index,
	int kindflag, int charfdid);
void saacproto_ACGetFMData_send(int fd, char *result, int kindflag, int data,
	int charfdid);
// ＧＭ修正家族资料
void saacproto_ACGMFixFMData_recv(int fd, int fmindex, char *charid,
	char *cmd, char *data, int charfdid);
void saacproto_ACGMFixFMData_send(int fd, char *result, char *fmname, int charfdid);
// 清除对战排程
void saacproto_ACFMClearPK_send(int fd, char *result, char *fmname, int fmindex,
	int index);
// CoolFish end

#ifdef _AC_SEND_FM_PK		 // WON ADD 庄园对战列表储存在AC
void saacproto_ACLoadFmPk_send(int fd, char *data);
#ifdef _ACFMPK_LIST//ACSendFmPk
void saacproto_ACSendFmPk_send( int fd, int userindex, int flg);
#endif
#endif

// Arminius 8.1 manor pk
void saacproto_ACManorPKAck_recv(int fd, char *data);
void saacproto_ACManorPKAck_send(int fd, char *data);

void saacproto_ACreLoadFmData_recv(int fd, int type, int data);

#ifdef _AC_SEND_FM_PK		 // WON ADD 庄园对战列表储存在AC
void saacproto_ACLoadFmPk_recv(int fd, int fmpks_pos);
#ifdef _ACFMPK_LIST
void saacproto_ACSendFmPk_recv(int fd, int fmpks_pos, int userindex, int flg, char *data);
#else
void saacproto_ACSendFmPk_recv(int fd, int fmpks_pos, char *data);
#endif
#endif

#ifdef _WAEI_KICK
void saacproto_ACKick_recv( int ti , char* id, int lock, int mesgid );
void saacproto_ACKick_send( int fd, int act, char* data, int id );
#endif

#ifdef _SEND_EFFECT		   // WON ADD 送下雪、下雨等特效
void saacproto_SendEffect_send(int fd, char *effect);
#endif

void saacproto_ACCharLogin_recv( int fd, int clifd, char* id, char* pas, char* ip );
void saacproto_ACCharLogin_send( int fd, int clifd, int flag );
#endif                	
/* end of the generated server header code */


