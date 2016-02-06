/* output by ../../bin/lsgen.perl 0.41 ( 1998 May)
 * made Mon Oct  9 10:21:40 2000
 * user ttom
 * host proxy1
 * file /home/ttom/gmsv2/src/gmsv/include/saacproto_cli.h
 * util ./saacproto_util.c , include/saacproto_util.h
 * src  /home/ttom/gmsv2/src/gmsv/../../doc/saacproto.html
 */
#ifndef _SAACPROTOCLI_H_
#define _SAACPROTOCLI_H_
#include "version.h"
#include "saacproto_util.h"

#ifdef MAXLSRPCARGS
#if ( MAXLSRPCARGS <= ( 7 + 1 ) )
#undef MAXLSRPCARGS
#define MAXLSRPCARGS ( 7 + 1 )
#endif
#else
#define MAXLSRPCARGS ( 7 + 1 )
#endif

#ifdef _ALLDOMAN // (不可开) Syu ADD 排行榜NPC
void saacproto_UpdataStele_send( int fd , char *cdkey , char *name , char *title , int level , int trns , int time , int floor) ;
void saacproto_UpdataStele_recv( int fd , char *token) ;
void saacproto_S_UpdataStele_recv( int i , char *ocdkey , char *oname , char *ncdkey , 
								  char *nname , char *title	, int level , int trns , int floor ); 
#endif

void saacproto_ACGmsvDownRequest_recv( int fd,int min ) ; /* ../../doc/saacproto.html line 80 */
#ifdef _VIP_ALL
void saacproto_ACServerLogin_send( int fd,char* servername,char* serverpas,int checkvip ) ; /* ../../doc/saacproto.html line 90 */
#else
void saacproto_ACServerLogin_send( int fd,char* servername,char* serverpas ) ; /* ../../doc/saacproto.html line 90 */
#endif
void saacproto_ACServerLogin_recv( int fd,char* result,char* data ) ; /* ../../doc/saacproto.html line 101 */
void saacproto_ACServerLogout_send( int fd ) ; /* ../../doc/saacproto.html line 114 */

#ifdef _PKSEVER_VER
void saacproto_ACCharList_send( int fd,char* id,char* pas,int mesgid, int star);
#else
void saacproto_ACCharList_send( int fd,char* id,char* pas,int mesgid ) ; /* ../../doc/saacproto.html line 123 */
#endif

void saacproto_ACCharList_recv( int fd,char* result,char* output,int id ) ; /* ../../doc/saacproto.html line 135 */
void saacproto_ACCharLoad_send( int fd,char* id,char* pas,char* charname,int lock,char* opt,int mesgid ) ; /* ../../doc/saacproto.html line 158 */
void saacproto_ACCharLoad_recv( int fd,char* result,char* data,int id,int saveindex ) ; /* ../../doc/saacproto.html line 185 */
#ifdef _NEWSAVE
void saacproto_ACCharSave_send( int fd,char* id,char* charname,char* opt,char* charinfo,int unlock,int mesgid,int saveindex ) ; /* ../../doc/saacproto.html line 201 */
#else
void saacproto_ACCharSave_send( int fd,char* id,char* charname,char* opt,char* charinfo,int unlock,int mesgid ) ; /* ../../doc/saacproto.html line 201 */
#endif
void saacproto_ACCharSave_recv( int fd,char* result,char* data,int id ) ; /* ../../doc/saacproto.html line 224 */
void saacproto_ACCharDelete_send( int fd,char* id,char* passwd,char* charname,char* option,int mesgid ) ; /* ../../doc/saacproto.html line 241 */
void saacproto_ACCharDelete_recv( int fd,char* result,char* data,int id ) ; /* ../../doc/saacproto.html line 266 */
void saacproto_ACLock_send( int fd,char* id,int lock,int mesgid ) ; /* ../../doc/saacproto.html line 281 */
void saacproto_ACLock_recv( int fd,char* result,char* data,int id ) ; /* ../../doc/saacproto.html line 300 */
void saacproto_ACUCheck_send( int fd,char* mem_id,int status ) ; /* ../../doc/saacproto.html line 314 */
void saacproto_ACUCheck_recv( int fd,char* mem_id ) ; /* ../../doc/saacproto.html line 328 */
void saacproto_DBUpdateEntryString_send( int fd,char* table,char* key,char* value,int msgid,int msgid2 ) ; /* ../../doc/saacproto.html line 339 */
void saacproto_DBUpdateEntryString_recv( int fd,char* result,char* table,char* key,int msgid,int msgid2 ) ; /* ../../doc/saacproto.html line 354 */
void saacproto_DBDeleteEntryString_send( int fd,char* table,char* key,int msgid,int msgid2 ) ; /* ../../doc/saacproto.html line 367 */
void saacproto_DBDeleteEntryString_recv( int fd,char* result,char* table,char* key,int msgid,int msgid2 ) ; /* ../../doc/saacproto.html line 379 */
void saacproto_DBGetEntryString_send( int fd,char* table,char* key,int msgid,int msgid2 ) ; /* ../../doc/saacproto.html line 394 */
void saacproto_DBGetEntryString_recv( int fd,char* result,char* value,char* table,char* key,int msgid,int msgid2 ) ; /* ../../doc/saacproto.html line 406 */
void saacproto_DBUpdateEntryInt_send( int fd,char* table,char* key,int value,char* info,int msgid,int msgid2 ) ; /* ../../doc/saacproto.html line 423 */
void saacproto_DBUpdateEntryInt_recv( int fd,char* result,char* table,char* key,int msgid,int msgid2 ) ; /* ../../doc/saacproto.html line 445 */
void saacproto_DBGetEntryRank_send( int fd,char* table,char* key,int msgid,int msgid2 ) ; /* ../../doc/saacproto.html line 459 */
void saacproto_DBGetEntryRank_recv( int fd,char* result,int rank,int count,char* table,char* key,int msgid,int msgid2 ) ; /* ../../doc/saacproto.html line 473 */
void saacproto_DBDeleteEntryInt_send( int fd,char* table,char* key,int msgid,int msgid2 ) ; /* ../../doc/saacproto.html line 492 */
void saacproto_DBDeleteEntryInt_recv( int fd,char* result,char* table,char* key,int msgid,int msgid2 ) ; /* ../../doc/saacproto.html line 504 */
void saacproto_DBGetEntryInt_send( int fd,char* table,char* key,int msgid,int msgid2 ) ; /* ../../doc/saacproto.html line 518 */
void saacproto_DBGetEntryInt_recv( int fd,char* result,int value,char* table,char* key,int msgid,int msgid2 ) ; /* ../../doc/saacproto.html line 530 */
void saacproto_DBGetEntryByRank_send( int fd,char* table,int rank_start,int rank_end,int msgid,int msgid2 ) ; /* ../../doc/saacproto.html line 547 */
void saacproto_DBGetEntryByRank_recv( int fd,char* result,char* list,char* table,int msgid,int msgid2 ) ; /* ../../doc/saacproto.html line 562 */
void saacproto_DBGetEntryByCount_send( int fd,char* table,int count_start,int num,int msgid,int msgid2 ) ; /* ../../doc/saacproto.html line 581 */
void saacproto_DBGetEntryByCount_recv( int fd,char* result,char* list,char* table,int count_start,int msgid,int msgid2 ) ; /* ../../doc/saacproto.html line 593 */
void saacproto_Broadcast_send( int fd,char* id,char* charname,char* message,int flag ) ; /* ../../doc/saacproto.html line 608 */
void saacproto_Broadcast_recv( int fd,char* id,char* charname,char* message ) ; /* ../../doc/saacproto.html line 621 */
void saacproto_Message_send( int fd,char* id_from,char* charname_from,char* id_to,char* charname_to,char* message,int option ) ; /* ../../doc/saacproto.html line 634 */
void saacproto_Message_recv( int fd,char* id_from,char* charname_from,char* id_to,char* charname_to,char* message,int option,int mesgid ) ; /* ../../doc/saacproto.html line 652 */
void saacproto_MessageAck_send( int fd,char* id,char* charname,char* result,int mesgid ) ; /* ../../doc/saacproto.html line 671 */
void saacproto_MessageFlush_send( int fd,char* id,char* charname ) ; /* ../../doc/saacproto.html line 690 */
int saacproto_InitClient( int(*)(int,char*,int)  , int bufsiz , int fd);
void saacproto_SetClientLogFiles( char *read , char *write);
void saacproto_CleanupClient(void);
int saacproto_ClientDispatchMessage(int fd ,char*line);

// 成立家族
#ifdef _PERSONAL_FAME
void saacproto_ACAddFM_send(int fd, char *fmname, char *fmleadername,
	char *fmleaderid, int fmleaderlv, char *petname, char *petarrt,
	char *fmrule, int fmsprite, int fmleadergrano, int fame, int charfdid);
#else
void saacproto_ACAddFM_send(int fd, char *fmname, char *fmleadername,
	char *fmleaderid, int fmleaderlv, char *petname, char *petarrt,
	char *fmrule, int fmsprite, int fmleadergrano, int charfdid);
#endif
void saacproto_ACAddFM_recv(int fd, char *result, int fmindex, int index,
	int charfdid);
// 加入家族
// #ifdef _PERSONAL_FAME	// Arminius: 家族个人声望
void saacproto_ACJoinFM_send(int fd, char *fmname, int fmindex, char *charname,
        char *charid, int charlv, int index, int fame, int charfdid);
// #else
// void saacproto_ACJoinFM_send(int fd, char *fmname, int fmindex, char *charname,
//        char *charid, int charlv, int index, int charfdid);
// #endif
void saacproto_ACJoinFM_recv(int fd, char *result, int recv, int charfdid);
// 退出家族
void saacproto_ACLeaveFM_send(int fd, char *fmname, int fmindex, char *charname,
        char *charid, int index, int charfdid);
void saacproto_ACLeaveFM_recv(int fd, char *result, int resultflag, int charfdid);
// 解散家族
void saacproto_ACDelFM_send(int fd, char *fmname, int fmindex, int index,
	char *charname, char *charid, int charfdid);

void saacproto_ACDelFM_recv(int fd, char *result, int charfdid);
// 列出家族列表
void saacproto_ACShowFMList_send(int fd);
void saacproto_ACShowFMList_recv(int fd, char *result, int num, char *data);
// 列出成员列表
void saacproto_ACShowMemberList_send(int fd, int index);
void saacproto_ACShowMemberList_recv(int fd, char *result, int index, int num,
	int fmacceptflag, int fmjoinnum, char *data);
// 列出家族详细资料
void saacproto_ACFMDetail_send(int fd, char *fmname, int fmindex,
	int index, int charfdid);
void saacproto_ACFMDetail_recv(int fd, char *result, char *data, int charfdid);
//族长审核成员加入家族、修改家族成员职位
#ifdef _FMVER21
void saacproto_ACMemberJoinFM_send(int fd, char *fmname, int fmindex, char *charname,
	int charindex, int index, int result, int meindex, int charfdid);
#else
void saacproto_ACMemberJoinFM_send(int fd, char *fmname, int fmindex, char *charname,
	int charindex, int index, int result, int charfdid);
#endif
void saacproto_ACMemberJoinFM_recv(int fd, char *result, char *data, int charfdid);
//族长审核成员离开家族
#ifdef _FMVER21
void saacproto_ACMemberLeaveFM_send(int fd, char *fmname, int fmindex, char *charname,
	int charindex, int index, int meindex, int charfdid);
#else
void saacproto_ACMemberLeaveFM_send(int fd, char *fmname, int fmindex, char *charname,
	int charindex, int index, int charfdid);
#endif
void saacproto_ACMemberLeaveFM_recv(int fd, char *result, char *data, int charfdid);
//玩家 Login
#ifdef _FM_MODIFY
void saacproto_ACFMCharLogin_send(int fd, char *fmname, int fmindex, char *charname,
		      char *charid, int charlv, int eventflag, int charfdid,int gsnum);
#else
	#ifdef _FMVER21
	void saacproto_ACFMCharLogin_send(int fd, char *fmname, int fmindex, char *charname,
		char *charid, int result, int eventflag, int charfdid);
	#else
	void saacproto_ACFMCharLogin_send(int fd, char *fmname, int fmindex, char *charname,
		char *charid, int result, int charfdid);
	#endif
#endif
#ifdef _PERSONAL_FAME   // Arminius: 家族个人声望
void saacproto_ACFMCharLogin_recv(int fd, char *result, int index, int floor,
	int fmdp, int joinflag, int fmsetupflag, int flag, int charindex,
	int charfame, int charfdid
	);
#else
void saacproto_ACFMCharLogin_recv(int fd, char *result, int index, int floor,
	int fmdp, int joinflag, int fmsetupflag, int flag, int charindex,
	int charfdid);
#endif
// 玩家 Logout
void saacproto_ACFMCharLogout_send(int fd, char *fmname, int fmindex, char *charname,
	char *charid, int result, int index, int charfdid);
void saacproto_ACFMCharLogout_recv(int fd, char *result, int charfdid);
// 列出家族留言板
void saacproto_ACFMReadMemo_send(int fd, int index);
void saacproto_ACFMReadMemo_recv(int fd, char *result, int index, int num,
        int dataindex, char *data);
// 写入家族留言板
void saacproto_ACFMWriteMemo_send(int fd, char *fmname, int fmindex,
        char *data, int index);
void saacproto_ACFMWriteMemo_recv(int fd, char *result, int index);
// 列出家族据点
void saacproto_ACFMPointList_send(int fd);
void saacproto_ACFMPointList_recv(int fd, char *result, char *data);

// 申请家族据点
void saacproto_ACSetFMPoint_send(int fd, char* fmname, int fmindex, int index,
	int fmpointindex, int fl, int x, int y, int charfdid);
void saacproto_ACSetFMPoint_recv(int fd, char *result, int r, int charfdid);
// 设定家族据点
void saacproto_ACFixFMPoint_send(int fd, char *winfmname, int winfmindex,
        int winindex, char *losefmname, int losefmindex, int loseindex,
        int village);
void saacproto_ACFixFMPoint_recv(int fd, char *result, int r);
// 族长对星系家族成员广播
// kindflag 1:族长广播 2:家族被系统删除 3:系统通知讯息
void saacproto_ACFMAnnounce_send(int fd, char *fmname, int fmindex, int index,
	char *data, int color);
void saacproto_ACFMAnnounce_recv(int fd, char *result, char *fmname,
	int fmindex, int index, int kindflag, char *data, int color);
// 列出 TOP 50 家族列表
void saacproto_ACShowTopFMList_send(int fd, int kindflag);
void saacproto_ACShowTopFMList_recv(int fd, char *result, int kindflag,
	int num, char *data);
// 修正家族资料(是否招收成员)
// flag 1:acceptjoinflag 2:dpchanged 3:change fmpet 4:change fminfo 5:predel FM Time
void saacproto_ACFixFMData_send(int fd, char *fmname, int fmindex, int index,
	int kindflag,char *data1,char *data2,int charindex, int charfdid);

void saacproto_ACFixFMData_recv(int fd, char *result, int kindflag, char *data1,
	char *data2, int charfdid);

// ＰＫ後更新家族声望
void saacproto_ACFixFMPK_send(int fd, char *winfmname, int winfmindex,
        int winindex, char *losefmname, int losefmindex, int loseindex);
void saacproto_ACFixFMPK_recv(int fd, char *result, int data, int winindex,
	int loseindex);
// 取得家族资料
// kindflag 1:家族银行
void saacproto_ACGetFMData_send(int fd, char *fmname, int fmindex, int index,
        int kindflag, int charfdid);
void saacproto_ACGetFMData_recv(int fd, char *result, int kindflag, int data,
	int charfdid);
// ＧＭ修正家族资料
void saacproto_ACGMFixFMData_send(int fd, int index, char *charid, char *cmd,
	char *data, int charfdid);
void saacproto_ACGMFixFMData_recv(int fd, char *result, char *fmindex, int charfdid);
// 删除家族後清除对战排程
void saacproto_ACFMClearPK_recv(int fd, char *result, char *fmname, int fmindex,
        int index);

// Arminius 7.26 manor pk acknowledge
void saacproto_ACManorPKAck_send(int fd, char *data);
void saacproto_ACManorPKAck_recv(int fd, char *data);

void saacproto_ACreLoadFmData_send( int fd, int type, int data);

#ifdef _ACFMPK_LIST
void saacproto_ACLoadFmPk_recv(int fd, char *data);
void saacproto_ACLoadFmPk_send(int fd, int fmpks_pos);
void saacproto_ACSendFmPk_recv( int fd, int toindex, int flg);
void saacproto_ACSendFmPk_send(int fd, int toindex, int PkFlg, int fmpks_pos, char *msg);
#endif
void saacproto_ACAuctionSold_send(int fd, char *data);
void saacproto_ACAuctionSold_recv(int fd, char *data);

#ifdef _WAEI_KICK	//跨星球踢人
void saacproto_ACKick_send( int fd, char* kickid,int kickfd, int flg);
void saacproto_ACKick_recv(int fd , int act, char* data ,int retfd);
#endif
void saacproto_ACCharLogin_send( int fd, int clifd, char* id,char* pas,char* ip );
void saacproto_ACCharLogin_recv(int fd , int clifd, int flag); 

void saacproto_LockLogin_send( int fd, char* id, char* ip, int flag );
#endif
/* end of the generated client header code */


