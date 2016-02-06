#include "version.h"
#include <stdio.h>
#include <stdlib.h>

#include "common.h"
#include "object.h"
#include "char_base.h"
#include "char.h"
#include "configfile.h"
#include "handletime.h"
#include "pet_event.h"
#include "npcutil.h"
#include "log.h"
#include "lssproto_serv.h"
// Arminius 8.14 pet talk
#include <string.h>
#include "npc_exchangeman.h"
#include "npc_eventaction.h"

#ifdef _PET_TALK
//BOOL PetTalk_CheckFree( int meindex, int  toindex, char *buf);
BOOL PetTalk_CheckFree( int meindex, int talker, char *buf);

BOOL PetTalk_BSCheck(int meindex,int talker,char* buf);
BOOL PetTalk_FreeIfCheck(int meindex,int talker,char* buf,int kosuu,int flg, int temp);
BOOL PetTalk_WarpManReduce(int meindex,int talker,char *buf);
BOOL PetTalk_BigSmallLastCheck(int point1,int mypoint,int flg);
BOOL PetTalk_CheckTrans(int meindex,int talker,int trans,int flg);
BOOL PetTalk_LevelCheck(int meindex,int talker,int level,int flg);
BOOL PetTalk_CheckMyPet( int meindex, int talker, int petLv, int flg, int petid);
BOOL PetTalk_ItemCheck(int meindex,int talker,int itemNo,int flg);
BOOL PetTalk_CheckMyType( int meindex, int toindex, int kosuu, int flg, int Type );
BOOL PetTalk_CheckPetEvent( int meindex, int toindex, char *buf);
void PetTalk_RequestMain(int meindex,int talker,char *buf);
BOOL PetTalk_AddItem(int meindex, int talker, char *buf);
BOOL PetTalk_DelItem(int meindex,int talker,char *buf);
BOOL PetTalk_RunEvent( int meindex, int talker, char *buf);
BOOL PetTalk_CheckMyFloor( int meindex, int talker, char *buf, int flg);
#ifdef _PET_TALKBBI
BOOL PET_CheckPlayerBBI( int meindex, int charindex, int BBI, int flg);
#endif
#ifdef _PET_TALKPRO
#else
char *Pet_TalkGetFunStr( char *temp , char *buf, int len)
{
	char filename[56];
	char pathfile[128];
	char talkfun[ 10240];
	char buf1[256],buf2[256],buf3[256];
	FILE *petarg;
	char	*cStr=NULL; 
	int talkNo=1,mark=1;
	char	line[4096];
	BOOL find=FALSE;
	talkfun[0] ='\0';

	while( getStringFromIndexWithDelim( pettalktext,"&",talkNo, buf1, sizeof( buf1) ) != FALSE){
		talkNo++;
		if( NPC_Util_GetStrFromStrWithDelim( buf1, "PETTEMPNO", buf2, sizeof( buf2)) == NULL  )
			continue;
		mark=2;
		strcpy( filename,"\0");
		while( getStringFromIndexWithDelim( buf2,",", mark,buf3,sizeof( buf3)) != FALSE )	{
			mark ++;
			if( !strcmp( buf3, temp))	{
				print("\n buf2=%s",buf2);
				if( getStringFromIndexWithDelim( buf2,",", 1,filename,sizeof( filename)) == FALSE )
					return NULL;
				find = TRUE;
				break;
			}
		}
		if( find == TRUE )
			break;
	}
	if( !strcmp( filename, "\0") )
		return NULL;

	sprintf( pathfile, "%s/pettalk/%s", getNpcdir(), filename);
	petarg = fopen( pathfile , "r" );
	if( petarg != NULL ) {
		while( fgets( line, sizeof( line), petarg ) ) {
			if( strlen( talkfun) != 0 ) {
				if( talkfun[strlen( talkfun) -1] != '|' ) {
					strcatsafe( talkfun, sizeof( talkfun), "|");
				}
			}
			chompex( line);
			strcatsafe( talkfun, sizeof( talkfun ),  line);
		}
		fclose( petarg);
	}else	{
		return NULL;
	}
	talkNo = 1;
	while( getStringFromIndexWithDelim( talkfun,"}",talkNo, buf, len) != FALSE)	{
		talkNo++;
		if( NPC_Util_GetStrFromStrWithDelim( buf, "PETTEMPNO", buf2, sizeof( buf2)) == NULL  )
			continue;
		if( !strcmp( temp, buf2) )		{
			cStr = buf;
			break;
		}
	}
	return( cStr);
}
#endif
void PET_Talkfunc( int meindex, int talkerindex, char *msg, int color)
{
  char buf2[1024], buf3[256];

  char TalkType[][16]={"TALKRUN","EVENTRUN","BOTH"};
  char AllTalk[PETTALK_MAXID][1024];
  int Type=0,j,i;
  int talkNo=0;
  BOOL FREEs=FALSE;
  int buttontype = 0;
  int windowtype = 0;
#ifdef _PET_TALKPRO
  int tPage=-1;
#else
  int petid=-1;
  char tempNo[32], buf1[10240];
#endif
  int fd = getfdFromCharaIndex( talkerindex);
#define RAND(x,y)   ((x-1)+1+ (int)( (double)(y-(x-1))*rand()/(RAND_MAX+1.0)) )
  windowtype = WINDOW_MESSAGETYPE_MESSAGE;
  buttontype = WINDOW_BUTTONTYPE_OK;
  if( NPC_Util_isFaceToFace( meindex ,talkerindex , 2) == FALSE) 	{
		if( NPC_Util_isFaceToChara( talkerindex, meindex, 1) == FALSE)
			return;
  }
  for( i = 0 ; i < 5 ; i++ )	{
  	strcpy( AllTalk[i], "\0" );
  }   

#ifdef _PET_TALKPRO
  for( i=0;i<PETTALK_MAXID;i++)	{
	  if( pettalktext[i].ID >=0 && pettalktext[i].ID == CHAR_getInt(meindex, CHAR_PETID) )	{
		  if( strcmp( pettalktext[i].DATA, "\0") && strlen( pettalktext[i].DATA) > 0 )	{
			  //snprintf( buf1, sizeof(buf1), pettalktext[i].DATA);
			  tPage=i;
			break;
		  }
	  }
  }
  if( i == PETTALK_MAXID )
	  return;
#else
  sprintf(tempNo,"%d", CHAR_getInt(meindex, CHAR_PETID));
  petid = CHAR_getInt(meindex, CHAR_PETID);
  if( Pet_TalkGetFunStr( tempNo , buf1 , sizeof( buf1) ) == NULL )	{
  	return;
  }
#endif
  if( CHAR_getInt( meindex, CHAR_LV ) >= CHAR_getInt( meindex, CHAR_LIMITLEVEL)  )	{
	Type = 1;
  }
	//设定为非主人不得与PET互动
  if( strcmp( CHAR_getChar( meindex, CHAR_OWNERCDKEY), CHAR_getChar( talkerindex, CHAR_CDKEY) ) ||
      strcmp( CHAR_getChar( meindex, CHAR_OWNERCHARANAME), CHAR_getChar( talkerindex, CHAR_NAME) )){
#ifdef _PET_TALKPRO
	if( NPC_Util_GetStrFromStrWithDelim( pettalktext[tPage].DATA, "NoPlayerMsg", buf3, sizeof( buf3)) != NULL )	{
#else
  	if( NPC_Util_GetStrFromStrWithDelim( buf1, "NoPlayerMsg", buf3, sizeof( buf3)) != NULL )	{
#endif
  	}else	{
  		sprintf(buf3,"陌生人？我不认识你呀！你是坏人！");
  	}
  	CHAR_talkToCli( talkerindex, meindex, buf3, color);
  	return;	
  }
  j=0;

#ifdef _PET_TALKPRO
	while( getStringFromIndexWithDelim( pettalktext[tPage].DATA,"OVER",talkNo, buf2, sizeof( buf2) ) != FALSE){
#else
	while( getStringFromIndexWithDelim( buf1,"OVER",talkNo, buf2, sizeof( buf2) ) != FALSE){
#endif
		talkNo++;
		if( ( strstr( buf2, TalkType[Type]) == NULL ) && ( strstr( buf2, TalkType[2]) == NULL ) )continue;
		if( NPC_Util_GetStrFromStrWithDelim( buf2, "FLOOR", buf3, sizeof( buf3)) != NULL )	{//判断房间号
			if( PetTalk_CheckMyFloor( meindex, talkerindex, buf3, 0) == FALSE )continue;
		}
		if( NPC_Util_GetStrFromStrWithDelim( buf2, "PET", buf3, sizeof( buf3)) != NULL )	{//判断宠物FREE条件
			if( PetTalk_CheckFree( meindex, meindex, buf3) != TRUE ) continue;
		}
		if( NPC_Util_GetStrFromStrWithDelim( buf2, "FREE", buf3, sizeof( buf3) ) == NULL)continue;
		if( NPC_ActionPassCheck( meindex, talkerindex, buf3) == FALSE )	continue;//判断玩家FREE条件
//		if( ActionNpc_CheckFree( meindex, talkerindex, buf2, 0) == FALSE ) continue; 
		if( PetTalk_CheckPetEvent( meindex, talkerindex, buf2) == FALSE )continue;
		FREEs = TRUE;  //条件成立
		strcpy( AllTalk[j++], buf2 );
		if( j > PETTALK_MAXID-1 ) break;
	}
	talkNo = 0;
	if( FREEs == FALSE)	{	//如果全部条件都不成立
		j=0;
#ifdef _PET_TALKPRO
		while( getStringFromIndexWithDelim( pettalktext[tPage].DATA,"OVER",talkNo, buf2, sizeof( buf2) ) != FALSE ){
#else
		while( getStringFromIndexWithDelim( buf1,"OVER",talkNo, buf2, sizeof( buf2) ) != FALSE ){
#endif
			talkNo ++;
			if( strstr( buf2, "OTHER") == NULL ) continue;
			if( NPC_Util_GetStrFromStrWithDelim( buf2, "TalkMsg", buf3, sizeof( buf3)) == NULL) continue;
			while( getStringFromIndexWithDelim( buf3,",",j+1, AllTalk[j], sizeof( AllTalk[j]) ) != FALSE ){
				j++;
			}
			break;
		}
	}
  if( j > 0 )	{
  	strcpy( buf2, AllTalk[ RAND( 0, (j-1) ) ] );
	if( PetTalk_RunEvent( meindex, talkerindex, buf2) == FALSE )	{
		sprintf( buf3,"．．．．！");
		CHAR_talkToCli( talkerindex, meindex, buf3, color);
		return;
	}
	if( NPC_Util_GetStrFromStrWithDelim( buf2, "TalkMsg", buf3, sizeof( buf3)) == NULL)     {
		//sprintf( buf3,"主人，我们去逛逛吧！");
		strcpy( buf3, buf2);
	}

	lssproto_WN_send( fd, windowtype, buttontype, 0,
		CHAR_getWorkInt( meindex, CHAR_WORKOBJINDEX), buf3 );         
  }
  
}

BOOL PetTalk_CheckMyFloor( int meindex, int talker, char *buf, int flg)	{
	char buf1[16];
	int Myfloor=-1;
	Myfloor = CHAR_getInt( talker, CHAR_FLOOR);
		if( strstr( buf, "!") != NULL )	{
			getStringFromIndexWithDelim( buf,"!", 2, buf1, sizeof( buf1) );
			if( Myfloor == atoi( buf1) )	{
				return FALSE;
			}
		}else	{
			if( Myfloor != atoi( buf) )	{
				return FALSE;
			}
		}
	return TRUE;
}

BOOL PetTalk_RunEvent( int meindex, int talker, char *buf)
{
	char buf1[256];
	int LimitLevel = -1;

	if( Action_RunDoEventAction( meindex, talker, buf) == FALSE )
		return FALSE;

	if( NPC_Util_GetStrFromStrWithDelim( buf, "RandItem", buf1, sizeof( buf1)) != NULL ){
		if( RAND( 0, 10) > 9 )  {
			PetTalk_AddItem( meindex, talker, buf1);
		}else	{
			return FALSE;
		}
	}
	if( NPC_Util_GetStrFromStrWithDelim( buf, "LimitLevel", buf1, sizeof( buf1)) != NULL ){
		LimitLevel = atoi( buf1);
		CHAR_setInt( meindex, CHAR_LIMITLEVEL, LimitLevel);
	}
	return TRUE;
	
}

BOOL PetTalk_DelItem(int meindex,int talker,char *buf)
{

	int i = 1, j = 1,k = 1;
	char buff3[128];
	char buf2[32];
	int itemindex;

	while( getStringFromIndexWithDelim(buf , "," , k, buff3, sizeof(buff3)) !=FALSE )	{
		k++;
		if(strstr(buff3,"*")!=NULL){
			int itemno;
			int kosuu;
			int id;
			int cnt=0;
				
			getStringFromIndexWithDelim(buff3,"*",1,buf2,sizeof(buf2));
			itemno = atoi(buf2);
			getStringFromIndexWithDelim(buff3,"*",2,buf2,sizeof(buf2));
			kosuu = atoi(buf2);
	
			for( i =0 ; i < CHAR_MAXITEMHAVE ; i++ ){
				itemindex=CHAR_getItemIndex( talker , i );
				if( ITEM_CHECKINDEX(itemindex) ){
					id=ITEM_getInt(itemindex ,ITEM_ID );
					if(itemno==id){
						cnt++;
						
						LogItem(
							CHAR_getChar( talker, CHAR_NAME ), /* 平乓仿   */
							CHAR_getChar( talker, CHAR_CDKEY ),
#ifdef _add_item_log_name  // WON ADD 在item的log中增加item名称
							itemindex,
#else
							ITEM_getInt( itemindex, ITEM_ID),  /* 失奶  丞  寞 */
#endif
							"WarpManDelItem(NPC收道具後传至某点)",
							CHAR_getInt( talker, CHAR_FLOOR),
							CHAR_getInt( talker, CHAR_X ),
 							CHAR_getInt( talker, CHAR_Y ),
							ITEM_getChar( itemindex, ITEM_UNIQUECODE),
						ITEM_getChar( itemindex, ITEM_NAME),
						ITEM_getInt( itemindex, ITEM_ID)

						);

						CHAR_DelItem( talker, i);
						if(cnt == kosuu){
							break;
						}
					}
				}
			}		
		}else{
			/*--蓟氏分  寞及失奶  丞毛蓟请---*/
			for( j = 0 ;  j < CHAR_MAXITEMHAVE ; j++){
				itemindex = CHAR_getItemIndex( talker ,j);

				if( ITEM_CHECKINDEX(itemindex) ){
					if( atoi( buff3) == ITEM_getInt(itemindex,ITEM_ID)){
						LogItem(
							CHAR_getChar( talker, CHAR_NAME ), /* 平乓仿   */
							CHAR_getChar( talker, CHAR_CDKEY ),
#ifdef _add_item_log_name  // WON ADD 在item的log中增加item名称
							itemindex,
#else
							ITEM_getInt( itemindex, ITEM_ID),  /* 失奶  丞  寞 */
#endif
							"WarpManDelItem(NPC收道具後传至某点)",
							CHAR_getInt( talker,CHAR_FLOOR),
							CHAR_getInt( talker,CHAR_X ),
							CHAR_getInt( talker,CHAR_Y ),
							ITEM_getChar( itemindex, ITEM_UNIQUECODE),
						ITEM_getChar( itemindex, ITEM_NAME),
						ITEM_getInt( itemindex, ITEM_ID)
						);
						CHAR_DelItem( talker, j);
					}
				}
			}
		}
	}

	return TRUE;
}

BOOL PetTalk_AddItem(int meindex, int talker, char *buf)
{
	int itemID,k=1,itemindex=-1;
	int spaceNum=5,i;
	char buff3[256], msgbuf[64], token[256];
	int ret;
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                
  while( getStringFromIndexWithDelim(buf , "," , k, buff3, sizeof(buff3)) !=FALSE ){
	k++;
		for( i = spaceNum ; i < CHAR_MAXITEMHAVE ; i++ ){
			itemindex=CHAR_getItemIndex( talker , i );
			if( itemindex == -1 )	{
				spaceNum = i+1;
				break;
			}
		}                                                                                                                                                                                                                                                                                                                                      
		if( i == CHAR_MAXITEMHAVE )	{
			snprintf( msgbuf,sizeof( msgbuf), "主人，你的物品栏已经满了！！");
			CHAR_talkToCli( talker, meindex, msgbuf,  CHAR_COLORWHITE);
			return FALSE;
		}
  }
  k=1;
  while( getStringFromIndexWithDelim(buf , "," , k, buff3, sizeof(buff3)) !=FALSE ){
	k++;
        itemID = atoi( buff3);
        if( itemID  )
		itemindex = ITEM_makeItemAndRegist( itemID);
		if(itemindex == -1)
			continue;
		ret = CHAR_addItemSpecificItemIndex( talker, itemindex);
		if( ret < 0 || ret >= CHAR_MAXITEMHAVE ) {
			ITEM_endExistItemsOne( itemindex);
			print ("\n ret error!!");
			return FALSE;
		}
		sprintf( token,"拿到%s",ITEM_getChar( itemindex, ITEM_NAME));
		CHAR_talkToCli( talker, -1,token,CHAR_COLORWHITE);
                                
		CHAR_sendItemDataOne( talker, ret);
		                                                                                                
  }
	return TRUE;	                                                                                                                                                                      
}

BOOL PetTalk_CheckPetEvent( int meindex, int toindex, char *buf)
{
	char buf1[256],buf2[256];
	int k = 0;

	if( NPC_Util_GetStrFromStrWithDelim( buf, "EndSetFlg", buf1,sizeof( buf1) ) != NULL )	{
		k=1;
		while( getStringFromIndexWithDelim(buf1 , "," ,k, buf2, sizeof(buf2) ) != FALSE ){
			k++;
			NPC_EventSetFlg( toindex, atoi( buf2));	
		}
	}
	if( NPC_Util_GetStrFromStrWithDelim( buf, "NowSetFlg", buf1, sizeof( buf1)) != NULL) {
		k=1 ;
		while(getStringFromIndexWithDelim(buf1 , "," , k, buf2, sizeof(buf2))!= FALSE ){
			k++;
			NPC_NowEventSetFlg( toindex, atoi( buf2));
		}
	}
	return TRUE;
}

void PetTalk_RequestMain(int meindex,int talker,char *buf)
{
	int shiftbit;
	char buf2[128];
	if( NPC_Util_GetStrFromStrWithDelim( buf,"EventNo", buf2,sizeof( buf2) ) == NULL )	{
		print("\n pet_event.c err:NOT FIND [EventNo] !!");
		return;
	}
	shiftbit = atoi( buf2);
	if( NPC_NowEventCheckFlg( talker, shiftbit) != TRUE )	{
		NPC_NowEventSetFlg( talker, shiftbit);
	}
}

BOOL PetTalk_CheckFree( int meindex, int talker, char *buf)
{
	char buff2[256];
	char buff3[128];
	int i=1,j=1;
	int loop=0;
	while( getStringFromIndexWithDelim(buf,",",i,buff2,sizeof(buff2)) !=FALSE )	{	
		i++;
		if(strstr(buff2,"&")!=NULL){
			j=1;
			loop=0;
			while( getStringFromIndexWithDelim(buff2,"&",j,buff3,sizeof(buff3)) !=FALSE )	{
				j++;
				if(PetTalk_BSCheck(meindex,talker,buff3)==FALSE)	{
					loop=1;
					break;
				}
			}
			if(loop==0) {
				CHAR_setWorkInt( talker, CHAR_WORKWARPCHECK, TRUE );
				return TRUE;
			}
		}else{
			if( PetTalk_BSCheck( meindex, talker, buff2) == TRUE ){
				CHAR_setWorkInt( talker, CHAR_WORKWARPCHECK, TRUE );
				return TRUE;
			}
		}
	}
	CHAR_setWorkInt( talker, CHAR_WORKWARPCHECK, FALSE );
	return FALSE;
}

BOOL PetTalk_BSCheck(int meindex,int talker,char* buf)
{
	char buff2[128];
	int kosuu,temp=-1,flg=0;
	char buff1[128],buff3[128];
	if(strstr( buf, "-") != NULL)	{
		//buff3为抓宠物ID
		getStringFromIndexWithDelim( buf, "-", 2, buff3, sizeof(buff3));
		temp = atoi( buff3);
		getStringFromIndexWithDelim( buf, "-", 1, buff1, sizeof(buff1));
		strcpy( buf, buff1);
        }                         
	if(strstr( buf, "<") != NULL){
		getStringFromIndexWithDelim( buf, "<", 2, buff2, sizeof(buff2));
		kosuu = atoi( buff2);
		getStringFromIndexWithDelim( buf, "<", 1, buff2, sizeof(buff2));

		if(PetTalk_FreeIfCheck( meindex, talker, buff2, kosuu, 1, temp)==TRUE){
			return TRUE;
		}
	}else if(strstr( buf, ">") != NULL){
		getStringFromIndexWithDelim( buf, ">", 2, buff2, sizeof(buff2));
		kosuu = atoi(buff2);
		getStringFromIndexWithDelim( buf, ">" ,1, buff2, sizeof(buff2));

		if(PetTalk_FreeIfCheck( meindex, talker, buff2, kosuu, 2, temp)==TRUE){
			return TRUE;
		}
	}else if(strstr( buf, "!" ) != NULL){
		getStringFromIndexWithDelim( buf, "!=", 2, buff2, sizeof(buff2));
		kosuu = atoi( buff2);
		getStringFromIndexWithDelim( buf, "!=", 1, buff2, sizeof(buff2));
		if(PetTalk_FreeIfCheck( meindex, talker, buff2, kosuu, 0, temp) == TRUE){
			return FALSE;
		}else{
			return TRUE;
		}

	}else if(strstr( buf, "=") != NULL){
		getStringFromIndexWithDelim( buf, "=", 2, buff2, sizeof(buff2));
		kosuu = atoi( buff2);
		getStringFromIndexWithDelim( buf, "=", 1, buff2, sizeof(buff2));

		if( strstr( buf, "PET"))	{
			flg = 3;
		}
		if(strstr( buf, "*") != NULL){
			if( PetTalk_WarpManReduce( meindex, talker, buf)==TRUE){
				return TRUE;
			}
		}else if(PetTalk_FreeIfCheck( meindex, talker, buff2, kosuu, flg, temp)==TRUE){
			return TRUE;
		}
	}
	return FALSE;
}

BOOL PetTalk_FreeIfCheck(int meindex,int talker,char* buf,int kosuu,int flg, int temp)
{
	int Type = -1;
	if(strcmp(buf,"LV")==0){
		if(PetTalk_LevelCheck(meindex,talker,kosuu,flg)==TRUE){
				return TRUE;
		}
	}
	
	if( strcmp( buf, "TRANS") == 0 )	{
		if( PetTalk_CheckTrans( meindex, talker, kosuu, flg) == TRUE )
			return TRUE;
	}
	if( strcmp( buf, "PET") == 0 )	{
		if( temp > 0 )	{
			if( PetTalk_CheckMyPet( meindex, talker, kosuu, flg, temp) == TRUE )
				return TRUE;
		}else
			return FALSE;
	}	
	if(strcmp( buf, "ITEM")==0){
		if(PetTalk_ItemCheck(meindex,talker,kosuu,flg)==TRUE){
				return TRUE;
		}
	}
	if(strcmp( buf, "ENDEV")==0){
		if(NPC_EventCheckFlg( talker, kosuu)==TRUE){
				return TRUE;
		}
	}
	if(strcmp( buf, "NOWEV")==0){
		if(NPC_NowEventCheckFlg( talker, kosuu)==TRUE){
				return TRUE;
		}
	}
	if(strcmp( buf, "HP" ) == 0 )	{
		Type = 0;
		if( PetTalk_CheckMyType( meindex, talker, kosuu, flg, Type ) == TRUE )	{
			return TRUE;
		}
	}
#ifdef _PET_TALKBBI
    if(strcmp( buf, "BBI" ) == 0 )	{
		if( PET_CheckPlayerBBI( meindex, talker, kosuu, flg)== TRUE)	{
			return TRUE;
		}
	}
#endif
	return FALSE;
}
BOOL PetTalk_CheckMyType( int meindex, int toindex, int kosuu, int flg, int Type )	{
	int MyType=0,MyMaxType=0;
	switch( Type )	{
	case 0:	//HP
		MyType = CHAR_getInt( toindex, CHAR_HP);
		MyMaxType = CHAR_getWorkInt( toindex, CHAR_WORKMAXHP);
		MyMaxType = (MyMaxType * kosuu )/ 100;
		if( PetTalk_BigSmallLastCheck( MyMaxType, MyType , flg ) == TRUE )	{
			return TRUE;
		}
	break;
	}
	return FALSE;
}

BOOL PetTalk_WarpManReduce(int meindex,int talker,char *buf)
{
	char buf2[512];
	char buf3[256];
	int id=0;
	int i;
	int itemindex;
	int itemno;
	int kosuu;
	int cnt=0;
	
	getStringFromIndexWithDelim(buf,"=",2,buf2,sizeof(buf2));
	getStringFromIndexWithDelim(buf2,"*",1,buf3,sizeof(buf3));
	itemno = atoi(buf3);
	getStringFromIndexWithDelim(buf2,"*",2,buf3,sizeof(buf3));
	kosuu = atoi(buf3);
	for( i=0 ; i < CHAR_MAXITEMHAVE;i++ ){
		itemindex=CHAR_getItemIndex( talker , i );
		if( ITEM_CHECKINDEX(itemindex) ){
			id=ITEM_getInt(itemindex ,ITEM_ID );
			if(itemno==id){
				cnt++;
				if(cnt==kosuu){
					return TRUE;
				}
			}
		}
	}
	return FALSE;
}

BOOL PetTalk_LevelCheck(int meindex,int talker,int level,int flg)
{
	int mylevel;
	mylevel=CHAR_getInt(talker,CHAR_LV);
	if(PetTalk_BigSmallLastCheck(level,mylevel,flg)==TRUE)	{
		return TRUE;
	}
	return FALSE;
}

BOOL PetTalk_CheckTrans(int meindex,int talker,int trans,int flg)
{
	int myTrans;
	myTrans=CHAR_getInt(talker, CHAR_TRANSMIGRATION);
	if( PetTalk_BigSmallLastCheck( trans, myTrans, flg ) == TRUE )	{
             return TRUE;
        }                   
	return FALSE;
}

BOOL PetTalk_CheckMyPet( int meindex, int talker, int petLv, int flg, int petid)
{
	int petsel,petindex=-1;	

	for(petsel=0 ; petsel < CHAR_MAXPETHAVE ; petsel++)     {
		petindex = CHAR_getCharPet( talker, petsel);
		if( petindex == -1  )
			continue;
	        if(CHAR_getInt( petindex, CHAR_PETID) !=  petid )
			continue;
		break;
	}
	if( petsel  == CHAR_MAXPETHAVE )	{
		return FALSE;
	}else	{	//找到条件宠
		if( PetTalk_BigSmallLastCheck( petLv, CHAR_getInt( petindex, CHAR_LV), flg ) == TRUE  )
			return TRUE;
	}
	return FALSE;                                             
}

BOOL PetTalk_ItemCheck(int meindex,int talker,int itemNo,int flg)
{
	int i;
	int itemindex=-1;
	int id;
	for( i=0;i<CHAR_MAXITEMHAVE;i++ ){
		itemindex = CHAR_getItemIndex( talker , i );
		if( ITEM_CHECKINDEX( itemindex) )	{
			id=ITEM_getInt(itemindex ,ITEM_ID );
			if( PetTalk_BigSmallLastCheck(itemNo,id,flg) == TRUE )
				return TRUE;
		}
	}
	return FALSE;
}

BOOL PetTalk_BigSmallLastCheck(int point1,int mypoint,int flg)
{
	if(flg==0){
		if(point1==mypoint) {
			return TRUE;
		}	
	}else if(flg==1){
		if(mypoint < point1) {
			return TRUE;
		}
	}else if(flg==2){
		if(mypoint > point1) {
			return TRUE;
		}
	}
	return FALSE;
}
#else
void PET_Talkfunc(  int meindex, int talkerindex, char *msg, int color)
{
	print("\n PET_Talkfunc(...) return; ");
	return;
}
#endif

void PET_CleanFreePetAll()
{
	int objindex;
	int objmaxnum = OBJECT_getNum();

	for( objindex=0; objindex<objmaxnum; objindex++)	{
		if( CHECKOBJECT( objindex ) == FALSE ) continue;
		PET_CleanPetdeletetime( objindex);
	}
}

int PET_CleanPetdeletetime( int objmeindex)
{
	int		pindex;

	if( OBJECT_getType(objmeindex) != OBJTYPE_CHARA ) return 0;
	pindex = OBJECT_getIndex( objmeindex);
	if( CHAR_getInt( pindex, CHAR_WHICHTYPE) != CHAR_TYPEPET) return 0;
	if( CHAR_getInt( pindex, CHAR_MAILMODE) != CHAR_PETMAIL_NONE) {
	}else if( CHAR_getWorkInt( pindex, CHAR_WORKPETFOLLOWMODE) == CHAR_PETFOLLOW_NOW ){
	}else {
#ifdef _PET_LOSTPET
//		CHAR_CharSaveLostPet( pindex, 0);
#endif
		LogPet(
			"系统",
			"Watchfunc",
			CHAR_getChar( pindex, CHAR_NAME),
			CHAR_getInt( pindex, CHAR_LV),
			"timeout_lost(系统清除-地上自由宠)",
			CHAR_getInt( pindex, CHAR_FLOOR),
			CHAR_getInt( pindex,CHAR_X ),
			CHAR_getInt( pindex,CHAR_Y ),
			CHAR_getChar( pindex, CHAR_UNIQUECODE)   // shan 2001/12/14
		);
		CHAR_CharaDelete(pindex);
		return 1;
	}
	return 0;
}

void PET_CHECKFreePetIsIt( int petindex)
{
	int petputtime, defmaxtime;
	defmaxtime = getPetdeletetime();
	petputtime = CHAR_getInt( petindex, CHAR_PUTPETTIME);

	if( NowTime.tv_sec < ( petputtime + getPetdeletetime() ) ) return;

#ifdef _PET_LOSTPET
//	CHAR_CharSaveLostPet( petindex, 0);
#endif

	LogPet(
			"系统",
			"Watchfunc",
			CHAR_getChar( petindex, CHAR_NAME),
			CHAR_getInt( petindex, CHAR_LV),
			"timeout_lost(系统清除-地上自由宠)",
			CHAR_getInt( petindex, CHAR_FLOOR),
			CHAR_getInt( petindex,CHAR_X ),
			CHAR_getInt( petindex,CHAR_Y ),
			CHAR_getChar( petindex, CHAR_UNIQUECODE)   // shan 2001/12/14
		);

	CHAR_CharaDelete(petindex);
}

#define PETFOLLOW_TIME 60*60
void PET_Watchfunc( int objmeindex, int objmoveindex, CHAR_ACTION act, int x, int y, int dir,
                           int* opt, int optlen )
{
	int		pindex;
	int		petindex;
	int		petputtime;
	
	if( OBJECT_getType(objmoveindex) != OBJTYPE_CHARA ) return;
	pindex = OBJECT_getIndex( objmoveindex);
	if( CHAR_getInt( pindex, CHAR_WHICHTYPE ) != CHAR_TYPEPLAYER ) return;
	petindex = OBJECT_getIndex( objmeindex);
	petputtime = CHAR_getInt( petindex, CHAR_PUTPETTIME);

		if( CHAR_getInt( petindex, CHAR_MAILMODE) != CHAR_PETMAIL_NONE) {
			//宠邮不处理
		}else if( CHAR_getWorkInt( petindex, CHAR_WORKPETFOLLOWMODE) == CHAR_PETFOLLOW_NOW ){
			if( NowTime.tv_sec >= (petputtime + 60*60) ) {
				int	ownerindex = CHAR_getWorkInt( petindex, CHAR_WORKPLAYERINDEX);
				if( CHAR_CHECKINDEX( ownerindex) ){
					if( CHAR_pickupFollowPet( ownerindex, petindex ) )	{
						return;
					}
					CHAR_talkToCli( ownerindex, -1, "溜宠太久，宠物走失了！！", CHAR_COLORYELLOW );
				}
#ifdef _PET_LOSTPET
				CHAR_CharSaveLostPet( petindex, 1);
				LogPet(
					"系统",
					"Watchfunc",
					CHAR_getChar( petindex, CHAR_NAME),
					CHAR_getInt( petindex, CHAR_LV),
					"timeout_lost(系统扣留-溜宠自由宠)",
					CHAR_getInt( petindex, CHAR_FLOOR),
					CHAR_getInt( petindex,CHAR_X ),
					CHAR_getInt( petindex,CHAR_Y ),
					CHAR_getChar( petindex, CHAR_UNIQUECODE)   // shan 2001/12/14
				);
				print("系统扣留-溜宠自由宠:%s\n", CHAR_getUseName( petindex));
				CHAR_CharaDelete( petindex);
#else
				CHAR_setInt( petindex, CHAR_PUTPETTIME, NowTime.tv_sec);
				CHAR_setWorkInt( petindex, CHAR_WORKPETFOLLOWMODE, CHAR_PETFOLLOW_NONE);
				LogPet(
					CHAR_getChar( pindex, CHAR_NAME ),
					CHAR_getChar( pindex, CHAR_CDKEY ),
					CHAR_getChar( petindex, CHAR_NAME),
					CHAR_getInt( petindex, CHAR_LV),
					"timeout_lost(溜宠太久，宠物走失))",
					CHAR_getInt( pindex,CHAR_FLOOR),
					CHAR_getInt( pindex,CHAR_X ),
	 				CHAR_getInt( pindex,CHAR_Y ),
					CHAR_getChar( petindex, CHAR_UNIQUECODE)   // shan 2001/12/14

				);
#endif
				return;
			}

		}else{
			PET_CHECKFreePetIsIt( petindex);
		}
	if( CHAR_getInt( petindex, CHAR_MAILMODE) == CHAR_PETMAIL_IDLE3 ||
		CHAR_getInt( petindex, CHAR_MAILMODE) == CHAR_PETMAIL_IDLE2){
		if( act == CHAR_ACTATTACK) {
			if( NPC_Util_isFaceToChara( pindex,petindex,1 ) == TRUE ) {
				int action[2] = { CHAR_ACTDAMAGE, CHAR_ACTGUARD};
				CHAR_sendWatchEvent( objmeindex, action[RAND(0,1)], NULL,0,FALSE);
				CHAR_setWorkInt( petindex, CHAR_WORKACTION, act);
			}
		}
	}else if( CHAR_getInt( petindex, CHAR_MAILMODE) != CHAR_PETMAIL_NONE) {
		;
	}else if( CHAR_getWorkInt( petindex, CHAR_WORKFIXAI) >= 100 ) {
		int	workpindex = CHAR_getWorkInt( petindex, CHAR_WORKPLAYERINDEX);
		if( act == CHAR_ACTWALK && CHAR_CHECKINDEX( workpindex) && workpindex == pindex ){
			dir = NPC_Util_GetDirCharToChar( petindex, pindex, 0);
			if( dir != -1 ) {
				if( CHAR_getInt( petindex, CHAR_DIR) != dir ) {
					CHAR_setInt( petindex, CHAR_DIR, dir);
					CHAR_sendWatchEvent( CHAR_getWorkInt( petindex, CHAR_WORKOBJINDEX),
											CHAR_ACTTURN,NULL,0,TRUE);
				}
			}
		}else if( NPC_Util_isFaceToFace( petindex, pindex, 2 ) == TRUE ) {
			switch( act) {
			  case CHAR_ACTATTACK:
			  case CHAR_ACTDAMAGE:
			  case CHAR_ACTDOWN:
			  case CHAR_ACTSTAND:
			  case CHAR_ACTACTIONWALK:
			  case CHAR_ACTGUARD:
              case CHAR_ACTACTIONSTAND:
				CHAR_sendWatchEvent( objmeindex, act, NULL,0,FALSE);
				CHAR_setWorkInt( petindex, CHAR_WORKACTION, act);
				break;
			  default:
			  	break;
			}
		}
	}else {

	}
	return;
}

#ifdef _PET_TALKBBI
BOOL PET_CheckPlayerBBI( int meindex, int charindex, int BBI, int flg)
{
	int MyBBI;
	if( !CHAR_CHECKINDEX( charindex))
		return FALSE;
	if( BBI < 0 )
		return FALSE;
	MyBBI = CHAR_getInt( charindex, CHAR_BASEIMAGENUMBER);

	if(flg==0){
		if(BBI==MyBBI)
			return TRUE;
	}else if(flg==1){
		if(BBI < MyBBI)
			return TRUE;
	}else if(flg==2){
		if(BBI > MyBBI)
			return TRUE;
	}

	return FALSE;
}
#endif
