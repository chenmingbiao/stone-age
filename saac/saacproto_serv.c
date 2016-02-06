/* output by ../../bin/lsgen.perl 0.41 ( 1998 May)
 * made Thu Feb 17 17:30:27 2000
 * user ringo
 * host indy
 * file /opt/ringo/chsa/src/saac/./saacproto_serv.c
 * util ./saacproto_util.c , ./saacproto_util.h
 * src  /opt/ringo/chsa/src/saac/../../doc/saacproto.html
 */
#define _SAACPROTOSERV_C_
#include "version.h"
#include "saacproto_serv.h"
#include "main.h"

// Nuke : This 'fd' is not a socket, it's the index of structure connect
int saacproto_ServerDispatchMessage( int fd , char *encoded, char *debugfun)
{
	unsigned int msgid;
	char funcname[1024];
	saacproto_splitString( encoded );
	saacproto_GetMessageInfo(&msgid , funcname ,sizeof(funcname), saacproto.token_list );

	if( strcmp( funcname , "ACServerLogin" ) == 0 ){
		char* servername;
		char* serverpas;
#ifdef _VIP
		int checkvip;
#endif
		servername = saacproto_wrapStringAddr( saacproto_stringwrapper[1] , saacproto.workbufsize , saacproto_demkstr_string( saacproto.token_list[2] ));
		serverpas = saacproto_wrapStringAddr( saacproto_stringwrapper[2] , saacproto.workbufsize , saacproto_demkstr_string( saacproto.token_list[3] ));
#ifdef _VIP
		checkvip = saacproto_demkstr_int( saacproto.token_list[4] );
		saacproto_ACServerLogin_recv( fd,servername,serverpas,checkvip);
#else
	saacproto_ACServerLogin_recv( fd,servername,serverpas);
#endif
		return 0;
	}

	if( strcmp( funcname , "ACServerLogout" ) == 0 ){

		saacproto_ACServerLogout_recv( fd);
		return 0;
	}

	if( strcmp( funcname , "ACCharList" ) == 0 ){
		char* id;
		char* pas;
		int mesgid;

		id = saacproto_wrapStringAddr( saacproto_stringwrapper[1] , saacproto.workbufsize , saacproto_demkstr_string( saacproto.token_list[2] ));
		pas = saacproto_wrapStringAddr( saacproto_stringwrapper[2] , saacproto.workbufsize , saacproto_demkstr_string( saacproto.token_list[3] ));
		mesgid = saacproto_demkstr_int( saacproto.token_list[4] );
		saacproto_ACCharList_recv( fd,id,pas,mesgid);
		return 0;
	}

	if( strcmp( funcname , "ACCharLoad" ) == 0 ){
		char* id;
		char* pas;
		char* charname;
		int lock;
		char* opt;
		int mesgid;
		id = saacproto_wrapStringAddr( saacproto_stringwrapper[1] , saacproto.workbufsize , saacproto_demkstr_string( saacproto.token_list[2] ));
		pas = saacproto_wrapStringAddr( saacproto_stringwrapper[2] , saacproto.workbufsize , saacproto_demkstr_string( saacproto.token_list[3] ));
		charname = saacproto_wrapStringAddr( saacproto_stringwrapper[3] , saacproto.workbufsize , saacproto_demkstr_string( saacproto.token_list[4] ));
		lock = saacproto_demkstr_int( saacproto.token_list[5] );
		opt = saacproto_wrapStringAddr( saacproto_stringwrapper[5] , saacproto.workbufsize , saacproto_demkstr_string( saacproto.token_list[6] ));
		mesgid = saacproto_demkstr_int( saacproto.token_list[7] );
		saacproto_ACCharLoad_recv( fd,id,pas,charname,lock,opt,mesgid);
		return 0;
	}

	if( strcmp( funcname , "ACCharSave" ) == 0 ){
		char* id;
		char* charname;
		char* opt;
		char* charinfo;
		int unlock;
		int mesgid;
#ifdef _NewSave
		int charindex;
#endif		
		id = saacproto_wrapStringAddr( saacproto_stringwrapper[1] , saacproto.workbufsize , saacproto_demkstr_string( saacproto.token_list[2] ));
		charname = saacproto_wrapStringAddr( saacproto_stringwrapper[2] , saacproto.workbufsize , saacproto_demkstr_string( saacproto.token_list[3] ));
		opt = saacproto_wrapStringAddr( saacproto_stringwrapper[3] , saacproto.workbufsize , saacproto_demkstr_string( saacproto.token_list[4] ));
		charinfo = saacproto_wrapStringAddr( saacproto_stringwrapper[4] , saacproto.workbufsize , saacproto_demkstr_string( saacproto.token_list[5] ));
		unlock = saacproto_demkstr_int( saacproto.token_list[6] );
		mesgid = saacproto_demkstr_int( saacproto.token_list[7] );

#ifdef _NewSave
		charindex = saacproto_demkstr_int( saacproto.token_list[8] );
		saacproto_ACCharSave_recv( fd,id,charname,opt,charinfo,unlock,mesgid,charindex);
#else
		saacproto_ACCharSave_recv( fd,id,charname,opt,charinfo,unlock,mesgid);
#endif
		return 0;
	}

	if( strcmp( funcname , "ACCharDelete" ) == 0 ){
		char* id;
		char* passwd;
		char* charname;
		char* option;
		int mesgid;
		id = saacproto_wrapStringAddr( saacproto_stringwrapper[1] , saacproto.workbufsize , saacproto_demkstr_string( saacproto.token_list[2] ));
		passwd = saacproto_wrapStringAddr( saacproto_stringwrapper[2] , saacproto.workbufsize , saacproto_demkstr_string( saacproto.token_list[3] ));
		charname = saacproto_wrapStringAddr( saacproto_stringwrapper[3] , saacproto.workbufsize , saacproto_demkstr_string( saacproto.token_list[4] ));
		option = saacproto_wrapStringAddr( saacproto_stringwrapper[4] , saacproto.workbufsize , saacproto_demkstr_string( saacproto.token_list[5] ));
		mesgid = saacproto_demkstr_int( saacproto.token_list[6] );
		saacproto_ACCharDelete_recv( fd,id,passwd,charname,option,mesgid);
		return 0;
	}

	if( strcmp( funcname , "ACLock" ) == 0 ){
		char* id;
		int lock;
		int mesgid;
		id = saacproto_wrapStringAddr( saacproto_stringwrapper[1] , saacproto.workbufsize , saacproto_demkstr_string( saacproto.token_list[2] ));
		lock = saacproto_demkstr_int( saacproto.token_list[3] );
		mesgid = saacproto_demkstr_int( saacproto.token_list[4] );
		saacproto_ACLock_recv( fd,id,lock,mesgid);
		return 0;
	}

#ifdef _WAEI_KICK
	if( strcmp( funcname , "ACKick" ) == 0 ){
		char* id;	//欲踢的帐号
		int flg;	//事件flg
		int clifd;	//GM
		id = saacproto_wrapStringAddr( saacproto_stringwrapper[1] , saacproto.workbufsize , saacproto_demkstr_string( saacproto.token_list[2] ));
		clifd = saacproto_demkstr_int( saacproto.token_list[3] );
		flg = saacproto_demkstr_int( saacproto.token_list[4] );
		saacproto_ACKick_recv( fd, id, flg, clifd);
		return 0;
	}
#endif

	if( strcmp( funcname , "ACUCheck" ) == 0 ){
		char* mem_id;
		int status;
		mem_id = saacproto_wrapStringAddr( saacproto_stringwrapper[1] , saacproto.workbufsize , saacproto_demkstr_string( saacproto.token_list[2] ));
		status = saacproto_demkstr_int( saacproto.token_list[3] );
		saacproto_ACUCheck_recv( fd,mem_id,status);
		return 0;
	}

	if( strcmp( funcname , "DBUpdateEntryString" ) == 0 ){
		char* table;
		char* key;
		char* value;
		int msgid;
		int msgid2;
		table = saacproto_wrapStringAddr( saacproto_stringwrapper[1] , saacproto.workbufsize , saacproto_demkstr_string( saacproto.token_list[2] ));
		key = saacproto_wrapStringAddr( saacproto_stringwrapper[2] , saacproto.workbufsize , saacproto_demkstr_string( saacproto.token_list[3] ));
		value = saacproto_wrapStringAddr( saacproto_stringwrapper[3] , saacproto.workbufsize , saacproto_demkstr_string( saacproto.token_list[4] ));
		msgid = saacproto_demkstr_int( saacproto.token_list[5] );
		msgid2 = saacproto_demkstr_int( saacproto.token_list[6] );
		saacproto_DBUpdateEntryString_recv( fd,table,key,value,msgid,msgid2);
		return 0;
	}

	if( strcmp( funcname , "DBDeleteEntryString" ) == 0 ){
		char* table;
		char* key;
		int msgid;
		int msgid2;
		table = saacproto_wrapStringAddr( saacproto_stringwrapper[1] , saacproto.workbufsize , saacproto_demkstr_string( saacproto.token_list[2] ));
		key = saacproto_wrapStringAddr( saacproto_stringwrapper[2] , saacproto.workbufsize , saacproto_demkstr_string( saacproto.token_list[3] ));
		msgid = saacproto_demkstr_int( saacproto.token_list[4] );
		msgid2 = saacproto_demkstr_int( saacproto.token_list[5] );
		saacproto_DBDeleteEntryString_recv( fd,table,key,msgid,msgid2);
		return 0;
	}

	if( strcmp( funcname , "DBGetEntryString" ) == 0 ){
		char* table;
		char* key;
		int msgid;
		int msgid2;
		table = saacproto_wrapStringAddr( saacproto_stringwrapper[1] , saacproto.workbufsize , saacproto_demkstr_string( saacproto.token_list[2] ));
		key = saacproto_wrapStringAddr( saacproto_stringwrapper[2] , saacproto.workbufsize , saacproto_demkstr_string( saacproto.token_list[3] ));
		msgid = saacproto_demkstr_int( saacproto.token_list[4] );
		msgid2 = saacproto_demkstr_int( saacproto.token_list[5] );
		saacproto_DBGetEntryString_recv( fd,table,key,msgid,msgid2);
		return 0;
	}

	if( strcmp( funcname , "DBUpdateEntryInt" ) == 0 ){
		char* table;
		char* key;
		int value;
		char* info;
		int msgid;
		int msgid2;
		table = saacproto_wrapStringAddr( saacproto_stringwrapper[1] , saacproto.workbufsize , saacproto_demkstr_string( saacproto.token_list[2] ));
		key = saacproto_wrapStringAddr( saacproto_stringwrapper[2] , saacproto.workbufsize , saacproto_demkstr_string( saacproto.token_list[3] ));
		value = saacproto_demkstr_int( saacproto.token_list[4] );
		info = saacproto_wrapStringAddr( saacproto_stringwrapper[4] , saacproto.workbufsize , saacproto_demkstr_string( saacproto.token_list[5] ));
		msgid = saacproto_demkstr_int( saacproto.token_list[6] );
		msgid2 = saacproto_demkstr_int( saacproto.token_list[7] );
		saacproto_DBUpdateEntryInt_recv( fd,table,key,value,info,msgid,msgid2);
		return 0;
	}

	if( strcmp( funcname , "DBGetEntryRank" ) == 0 ){
		char* table;
		char* key;
		int msgid;
		int msgid2;
		table = saacproto_wrapStringAddr( saacproto_stringwrapper[1] , saacproto.workbufsize , saacproto_demkstr_string( saacproto.token_list[2] ));
		key = saacproto_wrapStringAddr( saacproto_stringwrapper[2] , saacproto.workbufsize , saacproto_demkstr_string( saacproto.token_list[3] ));
		msgid = saacproto_demkstr_int( saacproto.token_list[4] );
		msgid2 = saacproto_demkstr_int( saacproto.token_list[5] );
		saacproto_DBGetEntryRank_recv( fd,table,key,msgid,msgid2);
		return 0;
	}

#ifdef _ALLDOMAN					// Syu ADD 排行榜NPC
	if ( strcmp( funcname , "UpdataStele" ) == 0 ) {
		char *cdkey;
		char *name;
		char *title;
		int level;
		int trns;
		int time;
		int floor;
		cdkey = saacproto_wrapStringAddr( saacproto_stringwrapper[1] , saacproto.workbufsize , saacproto_demkstr_string( saacproto.token_list[2] ));
		name = saacproto_wrapStringAddr( saacproto_stringwrapper[2] , saacproto.workbufsize , saacproto_demkstr_string( saacproto.token_list[3] ));
		title = saacproto_wrapStringAddr( saacproto_stringwrapper[3] , saacproto.workbufsize , saacproto_demkstr_string( saacproto.token_list[4] ));		
		level = saacproto_demkstr_int( saacproto.token_list[5] );
		trns = saacproto_demkstr_int( saacproto.token_list[6] );
		time = saacproto_demkstr_int( saacproto.token_list[7] );
		floor = saacproto_demkstr_int( saacproto.token_list[8] );
		saacproto_UpdataStele_recv ( fd , cdkey , name , title , level , trns , time , floor) ; 
		return 0;
	}
#endif

	if( strcmp( funcname , "DBDeleteEntryInt" ) == 0 ){
		char* table;
		char* key;
		int msgid;
		int msgid2;
		table = saacproto_wrapStringAddr( saacproto_stringwrapper[1] , saacproto.workbufsize , saacproto_demkstr_string( saacproto.token_list[2] ));
		key = saacproto_wrapStringAddr( saacproto_stringwrapper[2] , saacproto.workbufsize , saacproto_demkstr_string( saacproto.token_list[3] ));
		msgid = saacproto_demkstr_int( saacproto.token_list[4] );
		msgid2 = saacproto_demkstr_int( saacproto.token_list[5] );
		saacproto_DBDeleteEntryInt_recv( fd,table,key,msgid,msgid2);
		return 0;
	}

	if( strcmp( funcname , "DBGetEntryInt" ) == 0 ){
		char* table;
		char* key;
		int msgid;
		int msgid2;
		table = saacproto_wrapStringAddr( saacproto_stringwrapper[1] , saacproto.workbufsize , saacproto_demkstr_string( saacproto.token_list[2] ));
		key = saacproto_wrapStringAddr( saacproto_stringwrapper[2] , saacproto.workbufsize , saacproto_demkstr_string( saacproto.token_list[3] ));
		msgid = saacproto_demkstr_int( saacproto.token_list[4] );
		msgid2 = saacproto_demkstr_int( saacproto.token_list[5] );
		saacproto_DBGetEntryInt_recv( fd,table,key,msgid,msgid2);
		return 0;
	}

	if( strcmp( funcname , "DBGetEntryByRank" ) == 0 ){
		char* table;
		int rank_start;
		int rank_end;
		int msgid;
		int msgid2;
		table = saacproto_wrapStringAddr( saacproto_stringwrapper[1] , saacproto.workbufsize , saacproto_demkstr_string( saacproto.token_list[2] ));
		rank_start = saacproto_demkstr_int( saacproto.token_list[3] );
		rank_end = saacproto_demkstr_int( saacproto.token_list[4] );
		msgid = saacproto_demkstr_int( saacproto.token_list[5] );
		msgid2 = saacproto_demkstr_int( saacproto.token_list[6] );
		saacproto_DBGetEntryByRank_recv( fd,table,rank_start,rank_end,msgid,msgid2);
		return 0;
	}

	if( strcmp( funcname , "DBGetEntryByCount" ) == 0 ){
		char* table;
		int count_start;
		int num;
		int msgid;
		int msgid2;
		table = saacproto_wrapStringAddr( saacproto_stringwrapper[1] , saacproto.workbufsize , saacproto_demkstr_string( saacproto.token_list[2] ));
		count_start = saacproto_demkstr_int( saacproto.token_list[3] );
		num = saacproto_demkstr_int( saacproto.token_list[4] );
		msgid = saacproto_demkstr_int( saacproto.token_list[5] );
		msgid2 = saacproto_demkstr_int( saacproto.token_list[6] );
		saacproto_DBGetEntryByCount_recv( fd,table,count_start,num,msgid,msgid2);
		return 0;
	}

	if( strcmp( funcname , "Broadcast" ) == 0 ){
		char* id;
		char* charname;
		char* message;
		int flag;
		id = saacproto_wrapStringAddr( saacproto_stringwrapper[1] , saacproto.workbufsize , saacproto_demkstr_string( saacproto.token_list[2] ));
		charname = saacproto_wrapStringAddr( saacproto_stringwrapper[2] , saacproto.workbufsize , saacproto_demkstr_string( saacproto.token_list[3] ));
		message = saacproto_wrapStringAddr( saacproto_stringwrapper[3] , saacproto.workbufsize , saacproto_demkstr_string( saacproto.token_list[4] ));
		flag = saacproto_demkstr_int( saacproto.token_list[5] );
		saacproto_Broadcast_recv( fd,id,charname,message,flag);

		return 0;
	}

	if( strcmp( funcname , "Message" ) == 0 ){
		char* id_from;
		char* charname_from;
		char* id_to;
		char* charname_to;
		char* message;
		int option;
		id_from = saacproto_wrapStringAddr( saacproto_stringwrapper[1] , saacproto.workbufsize , saacproto_demkstr_string( saacproto.token_list[2] ));
		charname_from = saacproto_wrapStringAddr( saacproto_stringwrapper[2] , saacproto.workbufsize , saacproto_demkstr_string( saacproto.token_list[3] ));
		id_to = saacproto_wrapStringAddr( saacproto_stringwrapper[3] , saacproto.workbufsize , saacproto_demkstr_string( saacproto.token_list[4] ));
		charname_to = saacproto_wrapStringAddr( saacproto_stringwrapper[4] , saacproto.workbufsize , saacproto_demkstr_string( saacproto.token_list[5] ));
		message = saacproto_wrapStringAddr( saacproto_stringwrapper[5] , saacproto.workbufsize , saacproto_demkstr_string( saacproto.token_list[6] ));
		option = saacproto_demkstr_int( saacproto.token_list[7] );
		saacproto_Message_recv( fd,id_from,charname_from,id_to,charname_to,message,option);
		return 0;
	}

	if( strcmp( funcname , "MessageAck" ) == 0 ){
		char* id;
		char* charname;
		char* result;
		int mesgid;
		id = saacproto_wrapStringAddr( saacproto_stringwrapper[1] , saacproto.workbufsize , saacproto_demkstr_string( saacproto.token_list[2] ));
		charname = saacproto_wrapStringAddr( saacproto_stringwrapper[2] , saacproto.workbufsize , saacproto_demkstr_string( saacproto.token_list[3] ));
		result = saacproto_wrapStringAddr( saacproto_stringwrapper[3] , saacproto.workbufsize , saacproto_demkstr_string( saacproto.token_list[4] ));
		mesgid = saacproto_demkstr_int( saacproto.token_list[5] );
		saacproto_MessageAck_recv( fd,id,charname,result,mesgid);
		return 0;
	}

	if( strcmp( funcname , "MessageFlush" ) == 0 ){
		char* id;
		char* charname;
		id = saacproto_wrapStringAddr( saacproto_stringwrapper[1] , saacproto.workbufsize , saacproto_demkstr_string( saacproto.token_list[2] ));
		charname = saacproto_wrapStringAddr( saacproto_stringwrapper[2] , saacproto.workbufsize , saacproto_demkstr_string( saacproto.token_list[3] ));
		saacproto_MessageFlush_recv( fd,id,charname);
		return 0;
	}
	// 成立家族
	if ( strcmp( funcname, "ACAddFM") == 0){
		int 	fmleaderlv, fmsprite, charfdid, fmleadergrano;
		char	*fmname, *fmleadername, *fmleaderid, *petname, *petattr, *fmrule;
#ifdef _PERSONAL_FAME
		int 	fame;
#endif
		fmname = saacproto_wrapStringAddr(saacproto_stringwrapper[1], saacproto.workbufsize, saacproto_demkstr_string(saacproto.token_list[2]));
		fmleadername = saacproto_wrapStringAddr(saacproto_stringwrapper[2], saacproto.workbufsize, saacproto_demkstr_string(saacproto.token_list[3]));
		fmleaderid = saacproto_wrapStringAddr(saacproto_stringwrapper[3], saacproto.workbufsize, saacproto_demkstr_string(saacproto.token_list[4]));
		fmleaderlv = saacproto_demkstr_int(saacproto.token_list[5]);
		petname = saacproto_wrapStringAddr(saacproto_stringwrapper[5], saacproto.workbufsize, saacproto_demkstr_string(saacproto.token_list[6]));
		petattr = saacproto_wrapStringAddr(saacproto_stringwrapper[6], saacproto.workbufsize, saacproto_demkstr_string(saacproto.token_list[7]));
		fmrule = saacproto_wrapStringAddr(saacproto_stringwrapper[7], saacproto.workbufsize, saacproto_demkstr_string(saacproto.token_list[8]));
		fmsprite = saacproto_demkstr_int(saacproto.token_list[9]);
		fmleadergrano = saacproto_demkstr_int(saacproto.token_list[10]);
#ifdef _PERSONAL_FAME
		fame = saacproto_demkstr_int(saacproto.token_list[11]);
		charfdid = saacproto_demkstr_int(saacproto.token_list[12]);
		saacproto_ACAddFM_recv(fd, fmname, fmleadername, fmleaderid,
			fmleaderlv, petname, petattr, fmrule, fmsprite,
			fmleadergrano, fame, charfdid);
#else
		charfdid = saacproto_demkstr_int(saacproto.token_list[11]);
		saacproto_ACAddFM_recv(fd, fmname, fmleadername, fmleaderid,
			fmleaderlv, petname, petattr, fmrule, fmsprite,
			fmleadergrano, charfdid);
#endif
		return	0;
	}	
	// 加入家族
	if ( strcmp( funcname, "ACJoinFM") == 0){
#ifdef _PERSONAL_FAME	// Arminius: 家族个人声望
		int 	fmindex, charlv, charfdid, index, fame;
#else
		int 	fmindex, charlv, charfdid, index;
#endif
		char	*fmname, *charname, *charid;
		fmname = saacproto_wrapStringAddr(saacproto_stringwrapper[1], saacproto.workbufsize, saacproto_demkstr_string(saacproto.token_list[2]));
		fmindex = saacproto_demkstr_int(saacproto.token_list[3]);
		charname = saacproto_wrapStringAddr(saacproto_stringwrapper[3], saacproto.workbufsize, saacproto_demkstr_string(saacproto.token_list[4]));
		charid = saacproto_wrapStringAddr(saacproto_stringwrapper[4], saacproto.workbufsize, saacproto_demkstr_string(saacproto.token_list[5]));
		charlv = saacproto_demkstr_int(saacproto.token_list[6]);
		index = saacproto_demkstr_int(saacproto.token_list[7]);
#ifdef _PERSONAL_FAME	// Arminius: 家族个人声望
		fame = saacproto_demkstr_int(saacproto.token_list[8]);
		charfdid = saacproto_demkstr_int(saacproto.token_list[9]);
		saacproto_ACJoinFM_recv(fd, fmname, fmindex, charname,
			charid, charlv, index, fame, charfdid);
#else
		charfdid = saacproto_demkstr_int(saacproto.token_list[8]);
		saacproto_ACJoinFM_recv(fd, fmname, fmindex, charname,
			charid, charlv, index, charfdid);
#endif
		return	0;
	}	
	// 退出家族
	if ( strcmp( funcname, "ACLeaveFM") == 0){
		int 	fmindex, charfdid, index;
		char	*fmname, *charname, *charid;
		fmname = saacproto_wrapStringAddr(saacproto_stringwrapper[1], saacproto.workbufsize, saacproto_demkstr_string(saacproto.token_list[2]));
		fmindex = saacproto_demkstr_int(saacproto.token_list[3]);
		charname = saacproto_wrapStringAddr(saacproto_stringwrapper[3], saacproto.workbufsize, saacproto_demkstr_string(saacproto.token_list[4]));
		charid = saacproto_wrapStringAddr(saacproto_stringwrapper[4], saacproto.workbufsize, saacproto_demkstr_string(saacproto.token_list[5]));
		index = saacproto_demkstr_int(saacproto.token_list[6]);
		charfdid = saacproto_demkstr_int(saacproto.token_list[7]);
		saacproto_ACLeaveFM_recv(fd, fmname, fmindex, charname,
			charid, index, charfdid);
		return	0;
	}
	// 解散家族
#ifdef _LEADERFUNCHECK
	if ( strcmp( funcname, "ACDelFM") == 0){
		int 	fmindex, charfdid, index;
		char	*fmname, *charname, *charid;
		fmname = saacproto_wrapStringAddr(saacproto_stringwrapper[1], saacproto.workbufsize, saacproto_demkstr_string(saacproto.token_list[2]));
		fmindex = saacproto_demkstr_int(saacproto.token_list[3]);
		index = saacproto_demkstr_int(saacproto.token_list[4]);
		charname = saacproto_wrapStringAddr(saacproto_stringwrapper[4], saacproto.workbufsize, saacproto_demkstr_string(saacproto.token_list[5]));
		charid = saacproto_wrapStringAddr(saacproto_stringwrapper[5], saacproto.workbufsize, saacproto_demkstr_string(saacproto.token_list[6]));
		charfdid = saacproto_demkstr_int(saacproto.token_list[7]);
		saacproto_ACDelFM_recv(fd, fmname, fmindex, index, charname, charid, charfdid);
		return	0;
	}
#else
	if ( strcmp( funcname, "ACDelFM") == 0){
		int 	fmindex, charfdid, index;
		char	*fmname;
		fmname = saacproto_wrapStringAddr(saacproto_stringwrapper[1], saacproto.workbufsize, saacproto_demkstr_string(saacproto.token_list[2]));
		fmindex = saacproto_demkstr_int(saacproto.token_list[3]);
		index = saacproto_demkstr_int(saacproto.token_list[4]);
		charfdid = saacproto_demkstr_int(saacproto.token_list[5]);
		saacproto_ACDelFM_recv(fd, fmname, fmindex, index, charfdid);
		return	0;
	}
#endif
	// 列出家族列表
	if ( strcmp( funcname, "ACShowFMList") == 0){
		saacproto_ACShowFMList_recv(fd);
		return	0;
	}
	// 列出成员列表
	if ( strcmp( funcname, "ACShowMemberList") == 0){
		int 	index;
		char	*tmpbuf;
		tmpbuf = saacproto_wrapStringAddr(saacproto_stringwrapper[1], saacproto.workbufsize, saacproto_demkstr_string(saacproto.token_list[2]));
		index = saacproto_demkstr_int(saacproto.token_list[3]);
		saacproto_ACShowMemberList_recv(fd, index);
		return	0;
	}
	// 列出家族详细资料
	if ( strcmp( funcname, "ACFMDetail") == 0){
		int 	fmindex, charfdid, index;
		char	*fmname;
		fmname = saacproto_wrapStringAddr(saacproto_stringwrapper[1], saacproto.workbufsize, saacproto_demkstr_string(saacproto.token_list[2]));
		fmindex = saacproto_demkstr_int(saacproto.token_list[3]);
		index = saacproto_demkstr_int(saacproto.token_list[4]);
		charfdid = saacproto_demkstr_int(saacproto.token_list[5]);
		saacproto_ACFMDetail_recv(fd, fmname, fmindex, index, charfdid);
		return	0;
	}
	// 列出家族留言
	if (strcmp( funcname, "ACFMReadMemo") == 0){
		int 	index;
		char	*tmpbuf;
		tmpbuf = saacproto_wrapStringAddr(saacproto_stringwrapper[1], saacproto.workbufsize, saacproto_demkstr_string(saacproto.token_list[2]));
		index = saacproto_demkstr_int(saacproto.token_list[3]);
		saacproto_ACFMReadMemo_recv(fd, index);
		return	0;
	}
	// 写入家族留言
	if (strcmp( funcname, "ACFMWriteMemo") == 0){
		int 	index, fmindex;
		char	*fmname, *data;
		fmname = saacproto_wrapStringAddr(saacproto_stringwrapper[1], saacproto.workbufsize, saacproto_demkstr_string(saacproto.token_list[2]));
		fmindex = saacproto_demkstr_int(saacproto.token_list[3]);
		data = saacproto_wrapStringAddr(saacproto_stringwrapper[3], saacproto.workbufsize, saacproto_demkstr_string(saacproto.token_list[4]));
		index = saacproto_demkstr_int(saacproto.token_list[5]);
		saacproto_ACFMWriteMemo_recv(fd, fmname, fmindex, data, index);
		return	0;
	}

	// 玩家login
	if ( strcmp( funcname, "ACFMCharLogin") == 0){
		int 	fmindex, charlv, charfdid;
		char	*fmname, *charname, *charid;
#ifdef _FMVER21
		int eventflag;
#endif		
		fmname = saacproto_wrapStringAddr(saacproto_stringwrapper[1], saacproto.workbufsize, saacproto_demkstr_string(saacproto.token_list[2]));
		fmindex = saacproto_demkstr_int(saacproto.token_list[3]);
		charname = saacproto_wrapStringAddr(saacproto_stringwrapper[3], saacproto.workbufsize, saacproto_demkstr_string(saacproto.token_list[4]));
		charid = saacproto_wrapStringAddr(saacproto_stringwrapper[4], saacproto.workbufsize, saacproto_demkstr_string(saacproto.token_list[5]));
		charlv = saacproto_demkstr_int(saacproto.token_list[6]);

#ifdef _FMVER21
		eventflag = saacproto_demkstr_int(saacproto.token_list[7]);
		charfdid = saacproto_demkstr_int(saacproto.token_list[8]);
		saacproto_ACFMCharLogin_recv(fd, fmname, fmindex, charname,charid, charlv, eventflag, charfdid);
#else
		charfdid = saacproto_demkstr_int(saacproto.token_list[7]);
		saacproto_ACFMCharLogin_recv(fd, fmname, fmindex, charname,charid, charlv, charfdid);
#endif
		return	0;
	}	
	// 玩家logout
	if ( strcmp( funcname, "ACFMCharLogout") == 0){
		int 	fmindex, charfdid, charlv, index;
		char	*fmname, *charname, *charid;
		fmname = saacproto_wrapStringAddr(saacproto_stringwrapper[1], saacproto.workbufsize, saacproto_demkstr_string(saacproto.token_list[2]));
		fmindex = saacproto_demkstr_int(saacproto.token_list[3]);
		charname = saacproto_wrapStringAddr(saacproto_stringwrapper[3], saacproto.workbufsize, saacproto_demkstr_string(saacproto.token_list[4]));
		charid = saacproto_wrapStringAddr(saacproto_stringwrapper[4], saacproto.workbufsize, saacproto_demkstr_string(saacproto.token_list[5]));
		charlv = saacproto_demkstr_int(saacproto.token_list[6]);
		index = saacproto_demkstr_int(saacproto.token_list[7]);
		charfdid = saacproto_demkstr_int(saacproto.token_list[8]);
		saacproto_ACFMCharLogout_recv(fd, fmname, fmindex, charname,
			charid, charlv, index, charfdid);
		return	0;
	}
	// 族长审核成员加入家族、修改家族成员职位
	if (strcmp( funcname, "ACMemberJoinFM") == 0){
		int	fmindex, charfdid, index, charindex, result;
		char	*fmname, *charname;
#ifdef _FMVER21
		int	meindex;
#endif
		fmname = saacproto_wrapStringAddr(saacproto_stringwrapper[1], saacproto.workbufsize, saacproto_demkstr_string(saacproto.token_list[2]));
		fmindex = saacproto_demkstr_int(saacproto.token_list[3]);
		charname = saacproto_wrapStringAddr(saacproto_stringwrapper[3], saacproto.workbufsize, saacproto_demkstr_string(saacproto.token_list[4]));
		charindex = saacproto_demkstr_int(saacproto.token_list[5]);
		index = saacproto_demkstr_int(saacproto.token_list[6]);
		result = saacproto_demkstr_int(saacproto.token_list[7]);
#ifdef _FMVER21
		meindex = saacproto_demkstr_int(saacproto.token_list[8]);
		charfdid = saacproto_demkstr_int(saacproto.token_list[9]);
		saacproto_ACMemberJoinFM_recv(fd, fmname, fmindex, charname,
			charindex, index, result, meindex, charfdid);
#else
		charfdid = saacproto_demkstr_int(saacproto.token_list[8]);
		saacproto_ACMemberJoinFM_recv(fd, fmname, fmindex, charname,
			charindex, index, result, charfdid);
#endif
		return	0;
	}
	// 族长审核成员离开家族
	if (strcmp( funcname, "ACMemberLeaveFM") == 0){
		int	fmindex, charfdid, index, charindex;
		char	*fmname, *charname;
#ifdef _FMVER21
		int meindex;
#endif		
		fmname = saacproto_wrapStringAddr(saacproto_stringwrapper[1], saacproto.workbufsize, saacproto_demkstr_string(saacproto.token_list[2]));
		fmindex = saacproto_demkstr_int(saacproto.token_list[3]);
		charname = saacproto_wrapStringAddr(saacproto_stringwrapper[3], saacproto.workbufsize, saacproto_demkstr_string(saacproto.token_list[4]));
		charindex = saacproto_demkstr_int(saacproto.token_list[5]);
		index = saacproto_demkstr_int(saacproto.token_list[6]);
#ifdef _FMVER21
		meindex = saacproto_demkstr_int(saacproto.token_list[7]);
		charfdid = saacproto_demkstr_int(saacproto.token_list[8]);
		saacproto_ACMemberLeaveFM_recv(fd, fmname, fmindex, charname,
			charindex, index, meindex, charfdid);
#else
		charfdid = saacproto_demkstr_int(saacproto.token_list[7]);
		saacproto_ACMemberLeaveFM_recv(fd, fmname, fmindex, charname,
			charindex, index, charfdid);
#endif
		return	0;
	}
	// 列出家族据点
	if ( strcmp( funcname, "ACFMPointList") == 0){
		saacproto_ACFMPointList_recv(fd);
		return	0;
	}

	// 申请家族据点
	if (strcmp( funcname, "ACSetFMPoint") == 0){
		int	fl, x, y, fmindex, index, fmpointindex, charfdid;
		char	*fmname;
		fmname = saacproto_wrapStringAddr(saacproto_stringwrapper[1], saacproto.workbufsize, saacproto_demkstr_string(saacproto.token_list[2]));
		fmindex = saacproto_demkstr_int(saacproto.token_list[3]);
		index = saacproto_demkstr_int(saacproto.token_list[4]);
		fmpointindex = saacproto_demkstr_int(saacproto.token_list[5]);
		fl = saacproto_demkstr_int(saacproto.token_list[6]);
		x = saacproto_demkstr_int(saacproto.token_list[7]);
		y = saacproto_demkstr_int(saacproto.token_list[8]);
		charfdid = saacproto_demkstr_int(saacproto.token_list[9]);
		saacproto_ACSetFMPoint_recv(fd, fmname, fmindex, index,
			fmpointindex, fl, x, y, charfdid);
		return	0;
	}
	// 设定家族据点
	if (strcmp( funcname, "ACFixFMPoint") == 0){
		int	winindex, winfmindex, loseindex, losefmindex, village;
		char	*winfmname, *losefmname;
		winfmname = saacproto_wrapStringAddr(saacproto_stringwrapper[1], saacproto.workbufsize, saacproto_demkstr_string(saacproto.token_list[2]));
		winfmindex = saacproto_demkstr_int(saacproto.token_list[3]);
		winindex = saacproto_demkstr_int(saacproto.token_list[4]);
		losefmname =  saacproto_wrapStringAddr(saacproto_stringwrapper[4], saacproto.workbufsize, saacproto_demkstr_string(saacproto.token_list[5]));
		losefmindex = saacproto_demkstr_int(saacproto.token_list[6]);
		loseindex = saacproto_demkstr_int(saacproto.token_list[7]);
		village= saacproto_demkstr_int(saacproto.token_list[8]);
		saacproto_ACFixFMPoint_recv(fd, winfmname, winfmindex, winindex,
			losefmname, losefmindex, loseindex, village);
		return	0;
	}
	// 族长对全员广播
	if (strcmp( funcname, "ACFMAnnounce") == 0){
		int 	index, fmindex, color;
		char	*fmname, *data;
		fmname = saacproto_wrapStringAddr(saacproto_stringwrapper[1], saacproto.workbufsize, saacproto_demkstr_string(saacproto.token_list[2]));
		fmindex = saacproto_demkstr_int(saacproto.token_list[3]);
		index = saacproto_demkstr_int(saacproto.token_list[4]);
		data = saacproto_wrapStringAddr(saacproto_stringwrapper[4], saacproto.workbufsize, saacproto_demkstr_string(saacproto.token_list[5]));
		color = saacproto_demkstr_int(saacproto.token_list[6]);
		saacproto_ACFMAnnounce_recv(fd, fmname, fmindex, index, data, color);
		return	0;
	}
	// 列出家族排行榜
	if ( strcmp( funcname, "ACShowTopFMList") == 0){
		int	kindflag;
		char	*tmpbuf;
		tmpbuf = saacproto_wrapStringAddr(saacproto_stringwrapper[1], saacproto.workbufsize, saacproto_demkstr_string(saacproto.token_list[2]));
		kindflag = saacproto_demkstr_int(saacproto.token_list[3]);
		saacproto_ACShowTopFMList_recv(fd, kindflag);
		return	0;
	}
	// 修正家族资料
	if (strcmp( funcname, "ACFixFMData") == 0){
		int	fmindex, charfdid, index, kindflag, charindex;
		char	*fmname, *data1, *data2;
		fmname = saacproto_wrapStringAddr(saacproto_stringwrapper[1], saacproto.workbufsize, saacproto_demkstr_string(saacproto.token_list[2]));
		fmindex = saacproto_demkstr_int(saacproto.token_list[3]);
		index = saacproto_demkstr_int(saacproto.token_list[4]);
		kindflag = saacproto_demkstr_int(saacproto.token_list[5]);
		data1 = saacproto_wrapStringAddr(saacproto_stringwrapper[5], saacproto.workbufsize, saacproto_demkstr_string(saacproto.token_list[6]));
		data2 = saacproto_wrapStringAddr(saacproto_stringwrapper[6], saacproto.workbufsize, saacproto_demkstr_string(saacproto.token_list[7]));
		charindex = saacproto_demkstr_int(saacproto.token_list[8]);
		charfdid = saacproto_demkstr_int(saacproto.token_list[9]);
		saacproto_ACFixFMData_recv(fd, fmname, fmindex, index, kindflag,
			data1, data2, charindex, charfdid);
		return	0;
	}

	// 家族ＰＫ後修正声望
	if (strcmp( funcname, "ACFixFMPK") == 0){
		int	winindex, winfmindex, loseindex, losefmindex;
		char	*winfmname, *losefmname;
		winfmname = saacproto_wrapStringAddr(saacproto_stringwrapper[1], saacproto.workbufsize, saacproto_demkstr_string(saacproto.token_list[2]));
		winfmindex = saacproto_demkstr_int(saacproto.token_list[3]);
		winindex = saacproto_demkstr_int(saacproto.token_list[4]);
		losefmname =  saacproto_wrapStringAddr(saacproto_stringwrapper[4], saacproto.workbufsize, saacproto_demkstr_string(saacproto.token_list[5]));
		losefmindex = saacproto_demkstr_int(saacproto.token_list[6]);
		loseindex = saacproto_demkstr_int(saacproto.token_list[7]);
		saacproto_ACFixFMPK_recv(fd, winfmname, winfmindex, winindex,
			losefmname, losefmindex, loseindex);
		return	0;
	}
	// GM修正家族资料
	if (strcmp( funcname, "ACGMFixFMData") == 0){
		int	index, charfdid;
		char	*cmd, *data, *tmpbuf,*charid;
		tmpbuf = saacproto_wrapStringAddr(saacproto_stringwrapper[1], saacproto.workbufsize, saacproto_demkstr_string(saacproto.token_list[2]));
		index = saacproto_demkstr_int(saacproto.token_list[3]);
		charid = saacproto_wrapStringAddr(saacproto_stringwrapper[3], saacproto.workbufsize, saacproto_demkstr_string(saacproto.token_list[4]));
		cmd = saacproto_wrapStringAddr(saacproto_stringwrapper[4], saacproto.workbufsize, saacproto_demkstr_string(saacproto.token_list[5]));
		data = saacproto_wrapStringAddr(saacproto_stringwrapper[5], saacproto.workbufsize, saacproto_demkstr_string(saacproto.token_list[6]));
		charfdid = saacproto_demkstr_int(saacproto.token_list[7]);
		saacproto_ACGMFixFMData_recv(fd, index, charid, cmd, data, charfdid);
		return	0;
	}
	// 取得家族资料
	if (strcmp( funcname, "ACGetFMData") == 0){
		int	fmindex, charfdid, index, kindflag;
		char	*fmname;
		fmname = saacproto_wrapStringAddr(saacproto_stringwrapper[1], saacproto.workbufsize, saacproto_demkstr_string(saacproto.token_list[2]));
		fmindex = saacproto_demkstr_int(saacproto.token_list[3]);
		index = saacproto_demkstr_int(saacproto.token_list[4]);
		kindflag = saacproto_demkstr_int(saacproto.token_list[5]);
		charfdid = saacproto_demkstr_int(saacproto.token_list[6]);
		saacproto_ACGetFMData_recv(fd, fmname, fmindex, index, kindflag,
			charfdid);
		return	0;
	}
	// 广播踢馆人的资料
	if (strcmp( funcname,"ACManorPKAck") == 0){
		char *data;
		data = saacproto_wrapStringAddr(saacproto_stringwrapper[1], saacproto.workbufsize, saacproto_demkstr_string(saacproto.token_list[2]));
		saacproto_ACManorPKAck_recv(fd, data);
		return 0;
	}

	if (strcmp( funcname,"ACreLoadFmData") ==0){
		int type, data;
		type = saacproto_demkstr_int(saacproto.token_list[2]);	
		data = saacproto_demkstr_int(saacproto.token_list[3]);
		saacproto_ACreLoadFmData_recv( fd, type, data);
		return 0;
	}

#ifdef _AC_SEND_FM_PK
	if (strcmp( funcname,"ACLoadFmPk") ==0)
	{
		int fmpks_pos;
		fmpks_pos = saacproto_demkstr_int(saacproto.token_list[2]);	
		saacproto_ACLoadFmPk_recv(fd, fmpks_pos);
		return 0;
	}
#ifdef _ACFMPK_LIST
	if (strcmp( funcname,"ACSendFmPk") ==0){
		int fmpks_pos, userindex, flg;
		char *data;
		fmpks_pos = saacproto_demkstr_int(saacproto.token_list[2]);	
		userindex = saacproto_demkstr_int(saacproto.token_list[3]);
		flg = saacproto_demkstr_int(saacproto.token_list[4]);
		data = saacproto_wrapStringAddr( saacproto_stringwrapper[4] , saacproto.workbufsize , saacproto_demkstr_string( saacproto.token_list[5] ));
		saacproto_ACSendFmPk_recv( fd, fmpks_pos, userindex, flg, data);
		return 0;
	}
#else
	if (strcmp( funcname,"ACSendFmPk") ==0){
		int fmpks_pos;
		char *data;
		fmpks_pos = saacproto_demkstr_int(saacproto.token_list[2]);	
		data = saacproto_wrapStringAddr( saacproto_stringwrapper[2] , saacproto.workbufsize , saacproto_demkstr_string( saacproto.token_list[3] ));
		saacproto_ACSendFmPk_recv(fd, fmpks_pos, data);
		return 0;
	}
#endif

#endif

	if( strcmp( funcname , "ACCharLogin" ) == 0 ){
		int clifd;
		char *id , *pas, *ip;

		clifd = saacproto_demkstr_int( saacproto.token_list[2] );
		id = saacproto_wrapStringAddr( saacproto_stringwrapper[2] , saacproto.workbufsize , saacproto_demkstr_string( saacproto.token_list[3] ));
		pas = saacproto_wrapStringAddr( saacproto_stringwrapper[3] , saacproto.workbufsize , saacproto_demkstr_string( saacproto.token_list[4] ));
		ip = saacproto_wrapStringAddr( saacproto_stringwrapper[4] , saacproto.workbufsize , saacproto_demkstr_string( saacproto.token_list[5] ));
		saacproto_ACCharLogin_recv( fd, clifd, id, pas, ip );
		return 0;
	}
	
	if( strcmp( funcname , "LockLogin" ) == 0 ){
		int flag;
		char *id , *ip;
		id = saacproto_wrapStringAddr( saacproto_stringwrapper[1] , saacproto.workbufsize , saacproto_demkstr_string( saacproto.token_list[2] ));
		ip = saacproto_wrapStringAddr( saacproto_stringwrapper[2] , saacproto.workbufsize , saacproto_demkstr_string( saacproto.token_list[3] ));
		flag = saacproto_demkstr_int( saacproto.token_list[4] );
		saacproto_LockLogin_recv( fd, id, ip, flag );
		return 0;
	}
	
	sprintf( debugfun, "%s", funcname);

	return -1;	
}

void saacproto_ACServerLogin_send( int fd,char* result,char* data )
{
	saacproto_CreateHeader( saacproto.work , "ACServerLogin" );
	saacproto_strcatsafe( saacproto.work , saacproto_mkstr_string( result ) ,saacproto.workbufsize );
	saacproto_strcatsafe( saacproto.work , saacproto_mkstr_string( data ) ,saacproto.workbufsize );
	saacproto_Send( fd , saacproto.work );
}

void saacproto_ACCharList_send( int fd,char* result,char* output,int id )
{
	saacproto_CreateHeader( saacproto.work , "ACCharList" );
	saacproto_strcatsafe( saacproto.work , saacproto_mkstr_string( result ) ,saacproto.workbufsize );
	saacproto_strcatsafe( saacproto.work , saacproto_mkstr_string( output ) ,saacproto.workbufsize );
	saacproto_strcatsafe( saacproto.work , saacproto_mkstr_int( id ) ,saacproto.workbufsize );
	saacproto_Send( fd , saacproto.work );
}
static int CharDataLens = 0;
#ifdef _NewSave
void saacproto_ACCharLoad_send( int fd,char* result,char* data,int id,int charindex )
#else
void saacproto_ACCharLoad_send( int fd,char* result,char* data,int id )
#endif
{
	if( strstr( result, "successful") != NULL && strlen( data) > CharDataLens ){
		CharDataLens = strlen( data);
		//andy_log
		log("最大数据长度:%d\n", CharDataLens );
	}

	saacproto_CreateHeader( saacproto.work , "ACCharLoad" );
	saacproto_strcatsafe( saacproto.work , saacproto_mkstr_string( result ) ,saacproto.workbufsize );
	saacproto_strcatsafe( saacproto.work , saacproto_mkstr_string( data ) ,saacproto.workbufsize );
	saacproto_strcatsafe( saacproto.work , saacproto_mkstr_int( id ) ,saacproto.workbufsize );
#ifdef _NewSave
	saacproto_strcatsafe( saacproto.work , saacproto_mkstr_int( charindex ) ,saacproto.workbufsize );
#endif
	saacproto_Send( fd , saacproto.work );
}

/*
214 :  <li><a name="ACCharSaveS"><font color=blue>servertoclient ACCharSave( string result , string data,int id);<br></font></a>
215 :      ACCharSave卞覆允月忒蚕［
216 :      <br><br>
217 :      <dl>
218 :        <dt> string result
219 :        <dd>"successful" , "failed" 及中勿木井［
220 :        <dt> string data
221 :        <dd>"failed"及凛反    毛憎允谛棉及苇化坌月  侬  匹丐月［
222 :        <dt> int id
223 :        <dd> 娄醒及id毛公及引引戊疋□仄化忒允
224 :      </dl>
225 :      <br>
226 :      <br>
227 : 
228 : 
229 : 
230 : 
*/

void saacproto_ACCharSave_send( int fd,char* result,char* data,int id )
{
	saacproto_CreateHeader( saacproto.work , "ACCharSave" );
	saacproto_strcatsafe( saacproto.work , saacproto_mkstr_string( result ) ,saacproto.workbufsize );
	saacproto_strcatsafe( saacproto.work , saacproto_mkstr_string( data ) ,saacproto.workbufsize );
	saacproto_strcatsafe( saacproto.work , saacproto_mkstr_int( id ) ,saacproto.workbufsize );
	saacproto_Send( fd , saacproto.work );
}

/*
256 :  <li><a name="ACCharDeleteS"><font color=blue>servertoclient ACCharDelete(string result, string data,int id);<br></font></a>
257 :      ACCharDelete卞覆允月忒蚕［
258 :      <br><br>
259 :      <dl>
260 :        <dt> string result
261 :        <dd>"successful" , "failed" 及中勿木井［
262 :        <dt> string data
263 :        <dd>"failed"及凛反    毛憎允谛棉及苇化坌月  侬  匹丐月［
264 :        <dt> int id
265 :        <dd> 娄醒及id及公及引引及戊疋□
266 : 			   
267 :      </dl>
268 :      <br>
269 :      <br>
270 : 
*/

void saacproto_ACCharDelete_send( int fd,char* result,char* data,int id )
{
	saacproto_CreateHeader( saacproto.work , "ACCharDelete" );
	saacproto_strcatsafe( saacproto.work , saacproto_mkstr_string( result ) ,saacproto.workbufsize );
	saacproto_strcatsafe( saacproto.work , saacproto_mkstr_string( data ) ,saacproto.workbufsize );
	saacproto_strcatsafe( saacproto.work , saacproto_mkstr_int( id ) ,saacproto.workbufsize );
	saacproto_Send( fd , saacproto.work );
}
/*
290 :  <LI>    <a name="ACLockS"><font color=blue>servertoclient ACLock( string result, string data ,int id);<br></font></a>
291 :      ACLock卞覆允月忒蚕［
292 :      <br><br>
293 :      <dl>
294 :        <dt>string result
295 :        <dd>"successful" , "failed" 及中勿木井［
296 :        <dt>string data
297 :        <dd>"failed"及凛反    毛憎允谛棉及苇化坌月  侬  匹丐月［
298 :        <dt>int id
299 :        <dd>娄醒及id及公及引引戊疋□
300 :      </dl>
301 :      <br>
302 :      <br>
303 : 
*/

void saacproto_ACLock_send( int fd,char* result,char* data,int id )
{
	saacproto_CreateHeader( saacproto.work , "ACLock" );
	saacproto_strcatsafe( saacproto.work , saacproto_mkstr_string( result ) ,saacproto.workbufsize );
	saacproto_strcatsafe( saacproto.work , saacproto_mkstr_string( data ) ,saacproto.workbufsize );
	saacproto_strcatsafe( saacproto.work , saacproto_mkstr_int( id ) ,saacproto.workbufsize );
	saacproto_Send( fd , saacproto.work );
}
/*
318 :  <LI><a name="ACUCheckS"><font color=blue>servertoclient ACUCheck( string mem_id );</font></a><BR>
319 : 	 失市它件玄扔□田□井日必□丞扔□田□卞覆仄化｝丐月交□扒□互蜇箕夫弘奶件
320 : 	 仄化中月井升丹井毛民尼永弁允月［
321 : 	 <BR><BR>
322 : 	 <dl>
323 : 	   <dt>string mem_id
324 : 	   <dd>譬屯月覆擂及交□扒□及gamersDream member id
325 : 	 </dl>	 
326 : 	 <BR><BR>
327 : 
328 : 
*/

void saacproto_ACUCheck_send( int fd,char* mem_id )
{
	saacproto_CreateHeader( saacproto.work , "ACUCheck" );
	saacproto_strcatsafe( saacproto.work , saacproto_mkstr_string( mem_id ) ,saacproto.workbufsize );
	saacproto_Send( fd , saacproto.work );
}
/*
344 :  <LI><a name="DBUpdateEntryStringS"><font color=blue>servertoclient DBUpdateEntryString( string result, string table, string key, int msgid, int msgid2 );</font></a><BR>
345 : 	 晓及戊穴件玉及瑛绊［撩  仄化手荚汊尕第  卅及匹  骰仄化方中冗
346 : 	 <dl>
347 : 	   <dt>string result
348 : 	   <dD>successful 井failed	   
349 : 	   <dt>string table
350 : 	   <dD>
351 : 	   <dt>int msgid
352 : 	   <dD>
353 : 	   <dt>int msgid2
354 : 	   <dD>
355 : 	 </dL>
356 : 
*/

void saacproto_DBUpdateEntryString_send( int fd,char* result,char* table,char* key,int msgid,int msgid2 )
{
	saacproto_CreateHeader( saacproto.work , "DBUpdateEntryString" );
	saacproto_strcatsafe( saacproto.work , saacproto_mkstr_string( result ) ,saacproto.workbufsize );
	saacproto_strcatsafe( saacproto.work , saacproto_mkstr_string( table ) ,saacproto.workbufsize );
	saacproto_strcatsafe( saacproto.work , saacproto_mkstr_string( key ) ,saacproto.workbufsize );
	saacproto_strcatsafe( saacproto.work , saacproto_mkstr_int( msgid ) ,saacproto.workbufsize );
	saacproto_strcatsafe( saacproto.work , saacproto_mkstr_int( msgid2 ) ,saacproto.workbufsize );
	saacproto_Send( fd , saacproto.work );
}
/*
369 :  <LI><a name="DBDeleteEntryStringS"><font color=blue>servertoclient DBDeleteEntryString( string result, string table, string key , int msgid , int msgid2 );</font></a><BR>
370 : 	   侬  巨件玄伉及绰轮及瑛绊
371 : 	 <dl>
372 : 	   <dt>string result
373 : 	   <dd>successful / failed
374 : 	   <dt>string table
375 : 	   <dd>
376 : 	   <dt>string key
377 : 	   <dd>
378 : 	   <dt>int msgid
379 : 	   <dd>
380 : 	   <dt>int msgid2
381 : 	   <dd>		   
382 : 	 </dl>
383 : 	 
*/

void saacproto_DBDeleteEntryString_send( int fd,char* result,char* table,char* key,int msgid,int msgid2 )
{
	saacproto_CreateHeader( saacproto.work , "DBDeleteEntryString" );
	saacproto_strcatsafe( saacproto.work , saacproto_mkstr_string( result ) ,saacproto.workbufsize );
	saacproto_strcatsafe( saacproto.work , saacproto_mkstr_string( table ) ,saacproto.workbufsize );
	saacproto_strcatsafe( saacproto.work , saacproto_mkstr_string( key ) ,saacproto.workbufsize );
	saacproto_strcatsafe( saacproto.work , saacproto_mkstr_int( msgid ) ,saacproto.workbufsize );
	saacproto_strcatsafe( saacproto.work , saacproto_mkstr_int( msgid2 ) ,saacproto.workbufsize );
	saacproto_Send( fd , saacproto.work );
}
/*
396 :  <Li><a name="DBGetEntryStringS"><font color=blue>servertoclient DBGetEntryString( string result, string value, string table, string key, int msgid, int msgid2 );</font></a><BR>
397 : 	 潸曰分仄及忒曰袄
398 : 	 <dl>
399 : 	   <dt>string result
400 : 	   <dd>successful / failed
401 : 	   <dt>string value
402 : 	   <dd>袄
403 : 	   <dt>string table
404 : 	   <dd>		   
405 : 	   <dt>string key
406 : 	   <dd>
407 : 	   <dt>int msgid
408 : 	   <dd>
409 : 	   <dt>int msgid2
410 : 	   <dd>		   
411 : 	 </dl>
412 : 
*/

void saacproto_DBGetEntryString_send( int fd,char* result,char* value,char* table,char* key,int msgid,int msgid2 )
{
	saacproto_CreateHeader( saacproto.work , "DBGetEntryString" );
	saacproto_strcatsafe( saacproto.work , saacproto_mkstr_string( result ) ,saacproto.workbufsize );
	saacproto_strcatsafe( saacproto.work , saacproto_mkstr_string( value ) ,saacproto.workbufsize );
	saacproto_strcatsafe( saacproto.work , saacproto_mkstr_string( table ) ,saacproto.workbufsize );
	saacproto_strcatsafe( saacproto.work , saacproto_mkstr_string( key ) ,saacproto.workbufsize );
	saacproto_strcatsafe( saacproto.work , saacproto_mkstr_int( msgid ) ,saacproto.workbufsize );
	saacproto_strcatsafe( saacproto.work , saacproto_mkstr_int( msgid2 ) ,saacproto.workbufsize );
	saacproto_Send( fd , saacproto.work );
}
/*
435 :  <LI><a name="DBUpdateEntryIntS"><font color=blue>servertoclient DBUpdateEntryInt( string result , string table, string key, int msgid, int msgid2 );</font></a><BR>
436 : 	 DBUpdateEntry及瑛绊［
437 : 	 <dl>
438 : 	   <dt>string result
439 : 	   <dd>succesful/failed
440 : 	   <dt>string table
441 : 	   <dd>  □皮伙  毛公及引引井尹允
442 : 	   <dt>string key
443 : 	   <dd>平□毛公及引引井尹允		   
444 : 	   <dt>int msgid, int msgid2
445 : 	   <dd>msgid 毛公及引引忒允［
446 : 	 </dl>
447 : 	 <BR><BR>
448 : 
*/
#ifdef _ALLDOMAN // Syu ADD 排行榜NPC
void saacproto_UpdataStele_send( int fd , char *data )
{
	saacproto_CreateHeader( saacproto.work , "UpdataStele" );
	saacproto_strcatsafe( saacproto.work , saacproto_mkstr_string( data ) ,saacproto.workbufsize );
	saacproto_Send( fd , saacproto.work );
}

void saacproto_S_UpdataStele_send( int fd , char *ocdkey , char *oname , char *ncdkey , 
					  char *nname , char *title , int level , int trns , int floor ) {
	saacproto_CreateHeader( saacproto.work , "S_UpdataStele" );
	saacproto_strcatsafe( saacproto.work , saacproto_mkstr_string( ocdkey ) ,saacproto.workbufsize );
	saacproto_strcatsafe( saacproto.work , saacproto_mkstr_string( oname ) ,saacproto.workbufsize );
	saacproto_strcatsafe( saacproto.work , saacproto_mkstr_string( ncdkey ) ,saacproto.workbufsize );
	saacproto_strcatsafe( saacproto.work , saacproto_mkstr_string( nname ) ,saacproto.workbufsize );
	saacproto_strcatsafe( saacproto.work , saacproto_mkstr_string( title ) ,saacproto.workbufsize );
	saacproto_strcatsafe( saacproto.work , saacproto_mkstr_int( level ) ,saacproto.workbufsize );
	saacproto_strcatsafe( saacproto.work , saacproto_mkstr_int( trns ) ,saacproto.workbufsize );
	saacproto_strcatsafe( saacproto.work , saacproto_mkstr_int( floor ) ,saacproto.workbufsize );
	saacproto_Send( fd , saacproto.work );

}
#endif
void saacproto_DBUpdateEntryInt_send( int fd,char* result,char* table,char* key,int msgid,int msgid2 )
{
	saacproto_CreateHeader( saacproto.work , "DBUpdateEntryInt" );
	saacproto_strcatsafe( saacproto.work , saacproto_mkstr_string( result ) ,saacproto.workbufsize );
	saacproto_strcatsafe( saacproto.work , saacproto_mkstr_string( table ) ,saacproto.workbufsize );
	saacproto_strcatsafe( saacproto.work , saacproto_mkstr_string( key ) ,saacproto.workbufsize );
	saacproto_strcatsafe( saacproto.work , saacproto_mkstr_int( msgid ) ,saacproto.workbufsize );
	saacproto_strcatsafe( saacproto.work , saacproto_mkstr_int( msgid2 ) ,saacproto.workbufsize );
	saacproto_Send( fd , saacproto.work );
}
/*
463 :  <LI><a name="DBGetEntryRankS"><font color=blue>servertoclient DBGetEntryRank( string result , int rank , int count, string table, string key, int msgid, int msgid2 );</font></a><BR>
464 : 	 赐匏毛忒允［
465 : 	 <dl>
466 : 	   <dt>string result
467 : 	   <dd>successful/failed
468 : 	   <dt>int rank
469 : 	   <dd>赐匏［1动晓及袄毛潸月［
470 : 	   <dt>int count
471 : 	   <dd>晓井日中仁勾户井［  赓及巨件玄伉反0井日反元引月［
472 : 	   <dt>string table
473 : 	   <dd>  □皮伙  毛公及引引井尹允
474 : 	   <dt>string key
475 : 	   <dd>平□毛公及引引井尹允
476 : 	   <dt>int msgid, int msgid2
477 : 	   <dd>msgid 毛公及引引忒允
478 : 	 </dl>
479 : 	 <BR><BR>
480 : 
481 : 	 
*/

void saacproto_DBGetEntryRank_send( int fd,char* result,int rank,int count,char* table,char* key,int msgid,int msgid2 )
{
	saacproto_CreateHeader( saacproto.work , "DBGetEntryRank" );
	saacproto_strcatsafe( saacproto.work , saacproto_mkstr_string( result ) ,saacproto.workbufsize );
	saacproto_strcatsafe( saacproto.work , saacproto_mkstr_int( rank ) ,saacproto.workbufsize );
	saacproto_strcatsafe( saacproto.work , saacproto_mkstr_int( count ) ,saacproto.workbufsize );
	saacproto_strcatsafe( saacproto.work , saacproto_mkstr_string( table ) ,saacproto.workbufsize );
	saacproto_strcatsafe( saacproto.work , saacproto_mkstr_string( key ) ,saacproto.workbufsize );
	saacproto_strcatsafe( saacproto.work , saacproto_mkstr_int( msgid ) ,saacproto.workbufsize );
	saacproto_strcatsafe( saacproto.work , saacproto_mkstr_int( msgid2 ) ,saacproto.workbufsize );
	saacproto_Send( fd , saacproto.work );
}
/*
494 :  <LI><a name="DBDeleteEntryIntS"><font color=blue>servertoclient DBDeleteEntryInt( string result , string table, string key, int msgid, int msgid2 );</font></a><BR>
495 : 	 绰轮及瑛绊
496 : 	 <dl>
497 : 	   <dt>string result
498 : 	   <dd>successful/failed
499 : 	   <dt>string table
500 : 	   <dd>  □皮伙  毛公及引引井尹允
501 : 	   <dt>string key
502 : 	   <dd>平□毛公及引引井尹允
503 : 	   <dt>int msgid, int msgid2
504 : 	   <dd>msgid 毛公及引引忒允
505 : 	 </dl>
506 : 	 <BR><BR>
507 : 
*/

void saacproto_DBDeleteEntryInt_send( int fd,char* result,char* table,char* key,int msgid,int msgid2 )
{
	saacproto_CreateHeader( saacproto.work , "DBDeleteEntryInt" );
	saacproto_strcatsafe( saacproto.work , saacproto_mkstr_string( result ) ,saacproto.workbufsize );
	saacproto_strcatsafe( saacproto.work , saacproto_mkstr_string( table ) ,saacproto.workbufsize );
	saacproto_strcatsafe( saacproto.work , saacproto_mkstr_string( key ) ,saacproto.workbufsize );
	saacproto_strcatsafe( saacproto.work , saacproto_mkstr_int( msgid ) ,saacproto.workbufsize );
	saacproto_strcatsafe( saacproto.work , saacproto_mkstr_int( msgid2 ) ,saacproto.workbufsize );
	saacproto_Send( fd , saacproto.work );
}
/*
520 :  <LI><a name="DBGetEntryIntS"><font color=blue>servertoclient DBGetEntryInt( string result , int value , string table, string key, int msgid, int msgid2 );</font></a><BR>
521 : 	 巨件玄伉及袄毛井尹允
522 : 	 <dl>
523 : 	   <dt>string result
524 : 	   <dd>successful/failed
525 : 	   <dt>int value
526 : 	   <dd>袄公及手及
527 : 	   <dt>string table
528 : 	   <dd>  □皮伙  毛公及引引井尹允
529 : 	   <dt>string key
530 : 	   <dd>平□毛公及引引井尹允
531 : 	   <dt>int msgid, int msgid2
532 : 	   <dd>msgid 毛公及引引忒允
533 : 	 </dl>
534 : 
535 : 
536 : 
*/

void saacproto_DBGetEntryInt_send( int fd,char* result,int value,char* table,char* key,int msgid,int msgid2 )
{
	saacproto_CreateHeader( saacproto.work , "DBGetEntryInt" );
	saacproto_strcatsafe( saacproto.work , saacproto_mkstr_string( result ) ,saacproto.workbufsize );
	saacproto_strcatsafe( saacproto.work , saacproto_mkstr_int( value ) ,saacproto.workbufsize );
	saacproto_strcatsafe( saacproto.work , saacproto_mkstr_string( table ) ,saacproto.workbufsize );
	saacproto_strcatsafe( saacproto.work , saacproto_mkstr_string( key ) ,saacproto.workbufsize );
	saacproto_strcatsafe( saacproto.work , saacproto_mkstr_int( msgid ) ,saacproto.workbufsize );
	saacproto_strcatsafe( saacproto.work , saacproto_mkstr_int( msgid2 ) ,saacproto.workbufsize );
	saacproto_Send( fd , saacproto.work );
}
/*
552 :  <LI><a name="DBGetEntryByRankS"><font color=blue>servertoclient DBGetEntryByRank( string result , string list , string table, int msgid, int msgid2 );</font></a><BR>
553 : 	 DBGetEntryByRank及瑛绊［
554 : 	 <dl>
555 : 	   <dt>string result
556 : 	   <dd>
557 : 	   <dt>string list
558 : 	   <dd>"RANK,KEY,SCORE,INFO|RANK,KEY,SCORE,INFO|...."<BR>
559 : 		   RANK反赐匏匹丐月［KEY反公及赐匏卞卅匀化中月旦戊失毛手匀化中月
560 : 		   平□［SCORE反帮醒袄匹旦戊失匹丐月［
561 : 		     ［<br>
562 : 		   "1,RINGO,123465,data1|2,HOGE,4567,data2|2,FUCK,4567,data3"<BR>
563 : 		   仇及  分午｝2匏反2谛中月仇午卞卅月［
564 : 	   <dt>string table
565 : 	   <dd>  □皮伙  
566 : 	   <dt>int msgid, int msgid2
567 : 	   <dd>
568 : 	 </dl>
569 : 
570 : 
*/

void saacproto_DBGetEntryByRank_send( int fd,char* result,char* list,char* table,int msgid,int msgid2 )
{
	saacproto_CreateHeader( saacproto.work , "DBGetEntryByRank" );
	saacproto_strcatsafe( saacproto.work , saacproto_mkstr_string( result ) ,saacproto.workbufsize );
	saacproto_strcatsafe( saacproto.work , saacproto_mkstr_string( list ) ,saacproto.workbufsize );
	saacproto_strcatsafe( saacproto.work , saacproto_mkstr_string( table ) ,saacproto.workbufsize );
	saacproto_strcatsafe( saacproto.work , saacproto_mkstr_int( msgid ) ,saacproto.workbufsize );
	saacproto_strcatsafe( saacproto.work , saacproto_mkstr_int( msgid2 ) ,saacproto.workbufsize );
	saacproto_Send( fd , saacproto.work );
}
/*
583 :  <LI><a name="DBGetEntryByCountS"><font color=blue>servertoclient DBGetEntryByCount( string result , string list , string table, int count_start, int msgid, int msgid2 );</font></a><BR>
584 : 	 DBGetEntryByCount 及瑛绊［
585 : 	 <dl>
586 : 	   <dt>string result
587 : 	   <dd>SUCCESSFUL/FAILED
588 : 	   <dt>string list
589 : 	   <dd>请  伉旦玄［1巨件玄伉丐凶曰 "INDEX,RANK,KEY,SCORE,INFO" 及4蜊及树  
590 : 		   及赚匹丐曰｝公木互蜊醒坌 "|" 匹勾卅互月［蜊醒互0及桦宁反
591 : 		   坞  侬  ［
592 : 	   <dt>int num
593 : 	   <dd>中仁勾午曰分允井［1卅日1蜊［2卅日2蜊［
594 : 	   <dt>int msgid ,msgid2
595 : 	   <dd>丢永本□斥ID.	   
596 : 	 </dL>
597 : 
*/

void saacproto_DBGetEntryByCount_send( int fd,char* result,char* list,char* table,int count_start,int msgid,int msgid2 )
{
	saacproto_CreateHeader( saacproto.work , "DBGetEntryByCount" );
	saacproto_strcatsafe( saacproto.work , saacproto_mkstr_string( result ) ,saacproto.workbufsize );
	saacproto_strcatsafe( saacproto.work , saacproto_mkstr_string( list ) ,saacproto.workbufsize );
	saacproto_strcatsafe( saacproto.work , saacproto_mkstr_string( table ) ,saacproto.workbufsize );
	saacproto_strcatsafe( saacproto.work , saacproto_mkstr_int( count_start ) ,saacproto.workbufsize );
	saacproto_strcatsafe( saacproto.work , saacproto_mkstr_int( msgid ) ,saacproto.workbufsize );
	saacproto_strcatsafe( saacproto.work , saacproto_mkstr_int( msgid2 ) ,saacproto.workbufsize );
	saacproto_Send( fd , saacproto.work );
}
/*
611 :  <LI><a name="Broadcast"><font color=blue>servertoclient Broadcast( string id, string charname, string message );</font></a><BR>
612 : 	 Broadcast及失市它件玄扔□田□井日必□丞扔□田□尺及霜耨迕
613 : 	 <dl>
614 : 	   <dt>string id
615 : 	   <dd>user ID
616 : 	   <dt>string charname
617 : 	   <dd>character name
618 : 	   <dt>string message
619 : 	   <dd>丢永本□斥
620 : 	   <dt>int flag
621 : 	   <dd>1卞允月午｝愤坌卞卞反  霜今木卅中［
622 : 	 </dl>
623 : 
*/

void saacproto_Broadcast_send( int fd,char* id,char* charname,char* message )
{
	saacproto_CreateHeader( saacproto.work , "Broadcast" );
	saacproto_strcatsafe( saacproto.work , saacproto_mkstr_string( id ) ,saacproto.workbufsize );
	saacproto_strcatsafe( saacproto.work , saacproto_mkstr_string( charname ) ,saacproto.workbufsize );
	saacproto_strcatsafe( saacproto.work , saacproto_mkstr_string( message ) ,saacproto.workbufsize );
	saacproto_Send( fd , saacproto.work );
}
/*
642 :  <LI><a name="Message"><font color=blue>servertoclient Message( string id_from, string charname_from, string id_to, string charname_to, string message , int option , int mesgid );</font></a><BR>
643 : 	 失市它件玄扔□田□井日必□丞扔□田□卞丢永本□斥毛霜耨允月［
644 : 	 <dl>
645 : 	   <dt>string id_from
646 : 	   <dd>from ID
647 : 	   <dt>string charname_from
648 : 	   <dd>from character name
649 : 	   <dt>string id_to
650 : 	   <dd>to ID
651 : 	   <dt>string charname_to
652 : 	   <dd>to character name
653 : 	   <dt>string message
654 : 	   <dd>丢永本□斥及    
655 : 	   <dt>int option
656 : 	   <dd>左皿扑亦件(晓及楮醒毛辅寰)
657 : 	   <dt>int mesgid
658 : 	   <dd>丢永本□斥ID 失市它件玄扔□田□及栋  迕
659 : 	 </dl>
660 : 
*/

void saacproto_Message_send( int fd,char* id_from,char* charname_from,char* id_to,char* charname_to,char* message,int option,int mesgid )
{
	saacproto_CreateHeader( saacproto.work , "Message" );
	saacproto_strcatsafe( saacproto.work , saacproto_mkstr_string( id_from ) ,saacproto.workbufsize );
	saacproto_strcatsafe( saacproto.work , saacproto_mkstr_string( charname_from ) ,saacproto.workbufsize );
	saacproto_strcatsafe( saacproto.work , saacproto_mkstr_string( id_to ) ,saacproto.workbufsize );
	saacproto_strcatsafe( saacproto.work , saacproto_mkstr_string( charname_to ) ,saacproto.workbufsize );
	saacproto_strcatsafe( saacproto.work , saacproto_mkstr_string( message ) ,saacproto.workbufsize );
	saacproto_strcatsafe( saacproto.work , saacproto_mkstr_int( option ) ,saacproto.workbufsize );
	saacproto_strcatsafe( saacproto.work , saacproto_mkstr_int( mesgid ) ,saacproto.workbufsize );
	saacproto_Send( fd , saacproto.work );
}

void saacproto_SetServerLogFiles( char *r , char *w )
{
	saacproto_strcpysafe( saacproto_writelogfilename , w , sizeof(saacproto_writelogfilename ));
	saacproto_strcpysafe( saacproto_readlogfilename , r , sizeof(saacproto_readlogfilename ));
}
int saacproto_InitServer(	int (*writefunc)(int,char*,int) ,	int worksiz )
{
	int i;
	if( (void*)writefunc == NULL){saacproto.write_func = saacproto_default_write_wrap;} else {saacproto.write_func = writefunc;}
	saacproto_AllocateCommonWork(worksiz);
	saacproto_stringwrapper = (char**)calloc( 1,sizeof(char*) * MAXLSRPCARGS);
	if(saacproto_stringwrapper ==NULL)return -1;
	memset( saacproto_stringwrapper , 0, sizeof(char*)*MAXLSRPCARGS);
	for(i=0;i<MAXLSRPCARGS;i++){
		saacproto_stringwrapper[i] = (char*)calloc( 1, worksiz );
		if( saacproto_stringwrapper[i] == NULL){
			for(i=0;i<MAXLSRPCARGS;i++){free( saacproto_stringwrapper[i]);return -1;}
		}
	}
	saacproto.ret_work = (char*)calloc( 1,sizeof( worksiz ));
	if( saacproto.ret_work == NULL ){ return -1; }
	return 0;
}
void saacproto_CleanupServer( void )
{
	int i;
	free( saacproto.work );
	free( saacproto.arraywork);
	free( saacproto.escapework );
	free( saacproto.val_str);
	free( saacproto.token_list );
	for(i=0;i<MAXLSRPCARGS;i++){free( saacproto_stringwrapper[i]);}
	free( saacproto_stringwrapper );
	free( saacproto.ret_work );
}

/************************************************************
 * CoolFish: Family 2001/5/18
 ***********************************************************/
// 成立家族
void saacproto_ACAddFM_send(int fd, char *result, int fmindex, int index,
	int charfdid)
{
	saacproto_CreateHeader(saacproto.work , "ACAddFM");
	saacproto_strcatsafe(saacproto.work , saacproto_mkstr_string(result) ,saacproto.workbufsize );
	saacproto_strcatsafe(saacproto.work , saacproto_mkstr_int(fmindex) ,saacproto.workbufsize );
	saacproto_strcatsafe(saacproto.work , saacproto_mkstr_int(index) ,saacproto.workbufsize );
	saacproto_strcatsafe(saacproto.work , saacproto_mkstr_int(charfdid) ,saacproto.workbufsize );
	saacproto_Send( fd , saacproto.work );
}
// 加入家族
void saacproto_ACJoinFM_send(int fd, char *result, int recv, int charfdid)
{
	saacproto_CreateHeader(saacproto.work , "ACJoinFM");
	saacproto_strcatsafe(saacproto.work , saacproto_mkstr_string(result) ,saacproto.workbufsize );
	saacproto_strcatsafe(saacproto.work , saacproto_mkstr_int(recv) ,saacproto.workbufsize );
	saacproto_strcatsafe(saacproto.work , saacproto_mkstr_int(charfdid) ,saacproto.workbufsize );
	saacproto_Send( fd , saacproto.work );
}
// 退出家族
void saacproto_ACLeaveFM_send(int fd, char *result, int resultflag, int charfdid)
{
	saacproto_CreateHeader(saacproto.work , "ACLeaveFM");
	saacproto_strcatsafe(saacproto.work , saacproto_mkstr_string(result) ,saacproto.workbufsize );
	saacproto_strcatsafe(saacproto.work , saacproto_mkstr_int(resultflag) ,saacproto.workbufsize );
	saacproto_strcatsafe(saacproto.work , saacproto_mkstr_int(charfdid) ,saacproto.workbufsize );
	saacproto_Send( fd , saacproto.work );
}
// 解散家族
void saacproto_ACDelFM_send(int fd, char *result, int charfdid)
{
	saacproto_CreateHeader(saacproto.work , "ACDelFM");
	saacproto_strcatsafe(saacproto.work , saacproto_mkstr_string(result) ,saacproto.workbufsize );
	saacproto_strcatsafe(saacproto.work , saacproto_mkstr_int(charfdid) ,saacproto.workbufsize );
	saacproto_Send( fd , saacproto.work );}
// 列出家族列表
void saacproto_ACShowFMList_send(int fd, char *result, int num, char *data)
{
	saacproto_CreateHeader(saacproto.work , "ACShowFMList");
	saacproto_strcatsafe(saacproto.work , saacproto_mkstr_string(result) ,saacproto.workbufsize );
	saacproto_strcatsafe(saacproto.work , saacproto_mkstr_int(num) ,saacproto.workbufsize );
	saacproto_strcatsafe(saacproto.work , saacproto_mkstr_string(data) ,saacproto.workbufsize );
	saacproto_Send( fd , saacproto.work );
}

// 列出成员列表
void saacproto_ACShowMemberList_send(int fd, char *result, int index, int num,
	int acceptflag, int fmjoinnum, char *data)
{
	saacproto_CreateHeader(saacproto.work , "ACShowMemberList");
	saacproto_strcatsafe(saacproto.work , saacproto_mkstr_string(result) ,saacproto.workbufsize );
	saacproto_strcatsafe(saacproto.work , saacproto_mkstr_int(index) ,saacproto.workbufsize );
	saacproto_strcatsafe(saacproto.work , saacproto_mkstr_int(num) ,saacproto.workbufsize );
	saacproto_strcatsafe(saacproto.work , saacproto_mkstr_int(acceptflag) ,saacproto.workbufsize );
	saacproto_strcatsafe(saacproto.work , saacproto_mkstr_int(fmjoinnum) ,saacproto.workbufsize );
	saacproto_strcatsafe(saacproto.work , saacproto_mkstr_string(data) ,saacproto.workbufsize );
	saacproto_Send( fd , saacproto.work );
}
// 列出家族详细资料	
void saacproto_ACFMDetail_send(int fd, char *result, char *data,
	int charfdid)
{
	saacproto_CreateHeader(saacproto.work , "ACFMDetail");
	saacproto_strcatsafe(saacproto.work , saacproto_mkstr_string(result) ,saacproto.workbufsize );
	saacproto_strcatsafe(saacproto.work , saacproto_mkstr_string(data) ,saacproto.workbufsize );
	saacproto_strcatsafe(saacproto.work , saacproto_mkstr_int(charfdid) ,saacproto.workbufsize );
	saacproto_Send( fd , saacproto.work );
}
// 族长审核成员加入家族
void saacproto_ACMemberJoinFM_send(int fd, char *result, int charfdid)
{
	saacproto_CreateHeader(saacproto.work , "ACMemberJoinFM");
	saacproto_strcatsafe(saacproto.work , saacproto_mkstr_string(result) ,saacproto.workbufsize );
	saacproto_strcatsafe(saacproto.work , saacproto_mkstr_int(charfdid) ,saacproto.workbufsize );
	saacproto_Send( fd , saacproto.work );
}
// 族长审核成员离开家族
void saacproto_ACMemberLeaveFM_send(int fd, char *result, int charfdid)
{
	saacproto_CreateHeader(saacproto.work , "ACMemberLeaveFM");
	saacproto_strcatsafe(saacproto.work , saacproto_mkstr_string(result) ,saacproto.workbufsize );
	saacproto_strcatsafe(saacproto.work , saacproto_mkstr_int(charfdid) ,saacproto.workbufsize );
	saacproto_Send( fd , saacproto.work );
}
// 加入家族之玩家login
#ifdef _PERSONAL_FAME   // Arminius: 家族个人声望
void saacproto_ACFMCharLogin_send(int fd, char *result, int index, int floor,
	int fmpopular, int joinflag, int fmsetupflag, int flag, int charindex,
	int charfame, int charfdid
	)
#else
void saacproto_ACFMCharLogin_send(int fd, char *result, int index, int floor,
	int fmpopular, int joinflag, int fmsetupflag, int flag, int charindex,
	int charfdid)
#endif
{
	saacproto_CreateHeader(saacproto.work , "ACFMCharLogin");
	saacproto_strcatsafe(saacproto.work , saacproto_mkstr_string(result) ,saacproto.workbufsize );
	saacproto_strcatsafe(saacproto.work , saacproto_mkstr_int(index) ,saacproto.workbufsize );
	saacproto_strcatsafe(saacproto.work , saacproto_mkstr_int(floor) ,saacproto.workbufsize );
	saacproto_strcatsafe(saacproto.work , saacproto_mkstr_int(fmpopular) ,saacproto.workbufsize );
	saacproto_strcatsafe(saacproto.work , saacproto_mkstr_int(joinflag) ,saacproto.workbufsize );
	saacproto_strcatsafe(saacproto.work , saacproto_mkstr_int(fmsetupflag) ,saacproto.workbufsize );
	saacproto_strcatsafe(saacproto.work , saacproto_mkstr_int(flag) ,saacproto.workbufsize );
	saacproto_strcatsafe(saacproto.work , saacproto_mkstr_int(charindex) ,saacproto.workbufsize );
#ifdef _PERSONAL_FAME   // Arminius: 家族个人声望
	saacproto_strcatsafe(saacproto.work , saacproto_mkstr_int(charfame) ,saacproto.workbufsize );
#endif
	saacproto_strcatsafe(saacproto.work , saacproto_mkstr_int(charfdid) ,saacproto.workbufsize );
	saacproto_Send( fd , saacproto.work );
}
// 加入家族之玩家logout
void saacproto_ACFMCharLogout_send(int fd, char *result, int charfdid)
{
	saacproto_CreateHeader(saacproto.work , "ACFMCharLogout");
	saacproto_strcatsafe(saacproto.work , saacproto_mkstr_string(result) ,saacproto.workbufsize );
	saacproto_strcatsafe(saacproto.work , saacproto_mkstr_int(charfdid) ,saacproto.workbufsize );
	saacproto_Send( fd , saacproto.work );
}
// 列出家族留言板
void saacproto_ACFMReadMemo_send(int fd, char *result, int index, int num,
        int dataindex, char *data)
{
	saacproto_CreateHeader(saacproto.work , "ACFMReadMemo");
	saacproto_strcatsafe(saacproto.work , saacproto_mkstr_string(result) ,saacproto.workbufsize );
	saacproto_strcatsafe(saacproto.work , saacproto_mkstr_int(index) ,saacproto.workbufsize );
	saacproto_strcatsafe(saacproto.work , saacproto_mkstr_int(num) ,saacproto.workbufsize );
	saacproto_strcatsafe(saacproto.work , saacproto_mkstr_int(dataindex) ,saacproto.workbufsize );
	saacproto_strcatsafe(saacproto.work , saacproto_mkstr_string(data) ,saacproto.workbufsize );
	saacproto_Send( fd , saacproto.work );
}
// 写入家族留言板
void saacproto_ACFMWriteMemo_send(int fd, char *result, int index)
{
	saacproto_CreateHeader(saacproto.work , "ACFMWriteMemo");
	saacproto_strcatsafe(saacproto.work , saacproto_mkstr_string(result) ,saacproto.workbufsize );
	saacproto_strcatsafe(saacproto.work , saacproto_mkstr_int(index) ,saacproto.workbufsize );
	saacproto_Send( fd , saacproto.work );
}
// 列出家族据点
void saacproto_ACFMPointList_send(int fd, char *result, char *data)
{
	saacproto_CreateHeader(saacproto.work , "ACFMPointList");
	saacproto_strcatsafe(saacproto.work , saacproto_mkstr_string(result) ,saacproto.workbufsize );
	saacproto_strcatsafe(saacproto.work , saacproto_mkstr_string(data) ,saacproto.workbufsize );
	saacproto_Send( fd , saacproto.work );
}

// 申请家族据点
void saacproto_ACSetFMPoint_send(int fd, char *result, int r, int charfdid)
{
	saacproto_CreateHeader(saacproto.work , "ACSetFMPoint");
	saacproto_strcatsafe(saacproto.work , saacproto_mkstr_string(result) ,saacproto.workbufsize );
	saacproto_strcatsafe(saacproto.work , saacproto_mkstr_int(r) ,saacproto.workbufsize );
	saacproto_strcatsafe(saacproto.work , saacproto_mkstr_int(charfdid) ,saacproto.workbufsize );
	saacproto_Send( fd , saacproto.work );
}
// 设定家族据点
void saacproto_ACFixFMPoint_send(int fd, char *result, int r)
{
	saacproto_CreateHeader(saacproto.work , "ACFixFMPoint");
	saacproto_strcatsafe(saacproto.work , saacproto_mkstr_string(result) ,saacproto.workbufsize );
	saacproto_strcatsafe(saacproto.work , saacproto_mkstr_int(r) ,saacproto.workbufsize );
	saacproto_Send( fd , saacproto.work );
}
// 族长对星系家族成员广播
void saacproto_ACFMAnnounce_send(int fd, char *result, char *fmname,
                int fmindex, int index, int kindflag, char *data, int color)
{
	saacproto_CreateHeader(saacproto.work , "ACFMAnnounce");
	saacproto_strcatsafe(saacproto.work , saacproto_mkstr_string(result) ,saacproto.workbufsize );
	saacproto_strcatsafe(saacproto.work , saacproto_mkstr_string(fmname) ,saacproto.workbufsize );
	saacproto_strcatsafe(saacproto.work , saacproto_mkstr_int(fmindex) ,saacproto.workbufsize );
	saacproto_strcatsafe(saacproto.work , saacproto_mkstr_int(index) ,saacproto.workbufsize );
	saacproto_strcatsafe(saacproto.work , saacproto_mkstr_int(kindflag) ,saacproto.workbufsize );
	saacproto_strcatsafe(saacproto.work , saacproto_mkstr_string(data) ,saacproto.workbufsize );
	saacproto_strcatsafe(saacproto.work , saacproto_mkstr_int(color) ,saacproto.workbufsize );
	saacproto_Send( fd , saacproto.work );
}                
// 列出家族排行榜
void saacproto_ACShowTopFMList_send(int fd, char *result, int kindflag,
	int num, char *data)
{
	saacproto_CreateHeader(saacproto.work , "ACShowTopFMList");
	saacproto_strcatsafe(saacproto.work , saacproto_mkstr_string(result) ,saacproto.workbufsize );
	saacproto_strcatsafe(saacproto.work , saacproto_mkstr_int(kindflag) ,saacproto.workbufsize );
	saacproto_strcatsafe(saacproto.work , saacproto_mkstr_int(num) ,saacproto.workbufsize );
	saacproto_strcatsafe(saacproto.work , saacproto_mkstr_string(data) ,saacproto.workbufsize );
	saacproto_Send( fd , saacproto.work );
}

void saacproto_ACFixFMData_send(int fd, char *result, int kindflag, char *data1,
	char *data2, int charfdid)
{
	saacproto_CreateHeader(saacproto.work , "ACFixFMData");
	saacproto_strcatsafe(saacproto.work , saacproto_mkstr_string(result) ,saacproto.workbufsize );
	saacproto_strcatsafe(saacproto.work , saacproto_mkstr_int(kindflag) ,saacproto.workbufsize );
	saacproto_strcatsafe(saacproto.work , saacproto_mkstr_string(data1) ,saacproto.workbufsize );
	saacproto_strcatsafe(saacproto.work , saacproto_mkstr_string(data2) ,saacproto.workbufsize );
	saacproto_strcatsafe(saacproto.work , saacproto_mkstr_int(charfdid) ,saacproto.workbufsize );
	saacproto_Send( fd , saacproto.work );
}
// 设定家族据点
void saacproto_ACFixFMPK_send(int fd, char *result, int award, int winindex,
	int loseindex)
{
	saacproto_CreateHeader(saacproto.work , "ACFixFMPK");
	saacproto_strcatsafe(saacproto.work , saacproto_mkstr_string(result) ,saacproto.workbufsize );
	saacproto_strcatsafe(saacproto.work , saacproto_mkstr_int(award) ,saacproto.workbufsize );
	saacproto_strcatsafe(saacproto.work , saacproto_mkstr_int(winindex) ,saacproto.workbufsize );
	saacproto_strcatsafe(saacproto.work , saacproto_mkstr_int(loseindex) ,saacproto.workbufsize );
	saacproto_Send( fd , saacproto.work );
}
// GM修正家族资料
void saacproto_ACGMFixFMData_send(int fd, char *result, char *fmname, int charfdid)
{
	saacproto_CreateHeader(saacproto.work , "ACGMFixFMData");
	saacproto_strcatsafe(saacproto.work , saacproto_mkstr_string(result) ,saacproto.workbufsize );
	saacproto_strcatsafe(saacproto.work , saacproto_mkstr_string(fmname) ,saacproto.workbufsize );
	saacproto_strcatsafe(saacproto.work , saacproto_mkstr_int(charfdid) ,saacproto.workbufsize );
	saacproto_Send( fd , saacproto.work );
}
// 取得家族资料
void saacproto_ACGetFMData_send(int fd, char *result, int kindflag, int data,
	int charfdid)
{
	saacproto_CreateHeader(saacproto.work , "ACGetFMData");
	saacproto_strcatsafe(saacproto.work , saacproto_mkstr_string(result) ,saacproto.workbufsize );
	saacproto_strcatsafe(saacproto.work , saacproto_mkstr_int(kindflag) ,saacproto.workbufsize );
	saacproto_strcatsafe(saacproto.work , saacproto_mkstr_int(data) ,saacproto.workbufsize );
	saacproto_strcatsafe(saacproto.work , saacproto_mkstr_int(charfdid) ,saacproto.workbufsize );
	saacproto_Send( fd , saacproto.work );
}
//
void saacproto_ACFMClearPK_send(int fd, char *result, char *fmname, int fmindex,
        int index)
{
	saacproto_CreateHeader(saacproto.work , "ACFMClearPK");
	saacproto_strcatsafe(saacproto.work , saacproto_mkstr_string(result) ,saacproto.workbufsize );
	saacproto_strcatsafe(saacproto.work , saacproto_mkstr_string(fmname) ,saacproto.workbufsize );
	saacproto_strcatsafe(saacproto.work , saacproto_mkstr_int(fmindex) ,saacproto.workbufsize );
	saacproto_strcatsafe(saacproto.work , saacproto_mkstr_int(index) ,saacproto.workbufsize );
	saacproto_Send( fd , saacproto.work );
}        
// 通知其他星球的踢馆人
void saacproto_ACManorPKAck_send(int fd, char *data)
{
	saacproto_CreateHeader(saacproto.work , "ACManorPKAck");
	saacproto_strcatsafe(saacproto.work , saacproto_mkstr_string(data) ,saacproto.workbufsize );
	saacproto_Send( fd , saacproto.work );
}

#ifdef _AC_SEND_FM_PK		 // WON ADD 庄园对战列表储存在AC
void saacproto_ACLoadFmPk_send(int fd, char *data)
{
	saacproto_CreateHeader(saacproto.work , "ACLoadFmPk");
	saacproto_strcatsafe(saacproto.work , saacproto_mkstr_string(data) ,saacproto.workbufsize );
	saacproto_Send( fd , saacproto.work );
}
#ifdef _ACFMPK_LIST
void saacproto_ACSendFmPk_send( int fd, int userindex, int flg)
{
	saacproto_CreateHeader(saacproto.work , "ACSendFmPk");
	saacproto_strcatsafe(saacproto.work , saacproto_mkstr_int( userindex) ,saacproto.workbufsize );
	saacproto_strcatsafe(saacproto.work , saacproto_mkstr_int( flg) ,saacproto.workbufsize );
	saacproto_Send( fd , saacproto.work );
}
#endif

#endif

#ifdef _WAEI_KICK
void saacproto_ACKick_send( int fd, int act, char* data,int id )
{
	saacproto_CreateHeader( saacproto.work , "ACKick" );
	saacproto_strcatsafe( saacproto.work , saacproto_mkstr_int( act ) ,saacproto.workbufsize );
	saacproto_strcatsafe( saacproto.work , saacproto_mkstr_string( data ) ,saacproto.workbufsize );
	saacproto_strcatsafe( saacproto.work , saacproto_mkstr_int( id ) ,saacproto.workbufsize );
	saacproto_Send( fd , saacproto.work );
}
#endif

#ifdef _SEND_EFFECT		   // WON ADD 送下雪、下雨等特效
void saacproto_SendEffect_send(int fd, char *effect)
{
	saacproto_CreateHeader(saacproto.work , "EFFECT");
	saacproto_strcatsafe( saacproto.work , saacproto_mkstr_string( effect ) ,saacproto.workbufsize );
	saacproto_Send( fd , saacproto.work );
}
#endif

void saacproto_ACCharLogin_send( int fd, int clifd, int flag )
{
	saacproto_CreateHeader( saacproto.work , "ACCharLogin" );
	saacproto_strcatsafe( saacproto.work , saacproto_mkstr_int( clifd ) ,saacproto.workbufsize );
	saacproto_strcatsafe( saacproto.work , saacproto_mkstr_int( flag ) ,saacproto.workbufsize );

	saacproto_Send( fd , saacproto.work );
}

/* end of the generated server code */
