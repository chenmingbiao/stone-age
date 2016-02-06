#define _SASQL_C_

#include "version.h"

#ifdef _SASQL//新添加

#include "main.h"
#include "sasql.h"

#include <mysql/mysql.h>

MYSQL mysql;
MYSQL_RES *mysql_result;
MYSQL_ROW mysql_row;

typedef struct tagConfig
{
    char    sql_IP[16]; 

		int     sql_Port; 
		
		char    sql_Port1[16];
		
    char    sql_ID[16];
    
    char    sql_PS[16];

    char    sql_DataBase[16];  

    char    sql_Table[16];    
    
    char    sql_LOCK[16];   
    
    char    sql_NAME[16];  
    
    char    sql_PASS[16];  
}Config;

Config config;

int AutoReg;

static int readConfig( char *path )
{
    char buf[255];
    FILE *fp;

    fp = fopen( path , "r" );
    if( fp == NULL ){ return -2; }

    while( fgets( buf , sizeof( buf ) , fp )){
        char command[255];
        char param[255];
        chop(buf);
        
        easyGetTokenFromString( buf , 1 , command , sizeof( command ));
        easyGetTokenFromString( buf , 2 , param , sizeof( param ));

        if( strcmp( command , "sql_IP" ) == 0 ){
            strcmp( config.sql_IP , param ) ;
            snprintf( config.sql_IP , sizeof( config.sql_IP) , param );
            printf("\n数据库地址：  %s",config.sql_IP);
        } else if( strcmp( command , "sql_Port" ) == 0 ){
        		config.sql_Port = atoi( param );
            snprintf( config.sql_Port1 , sizeof( config.sql_Port1) , param );
				  	printf("\n数据库端口：  %d",config.sql_Port);
        } else if( strcmp( command , "sql_ID" ) == 0 ){
        		strcmp( config.sql_ID , param ) ;
            snprintf( config.sql_ID , sizeof( config.sql_ID) , param );
						printf("\n数据库用户：  %s",config.sql_ID);
        } else if( strcmp( command , "sql_PS" ) == 0 ){
        		strcmp( config.sql_PS , param ) ;
            snprintf( config.sql_PS , sizeof( config.sql_PS) , param );
						printf("\n数据库密码：  %s",config.sql_PS);
        } else if( strcmp( command , "sql_DataBase" ) == 0 ){
        		strcmp( config.sql_DataBase , param ) ;
            snprintf( config.sql_DataBase , sizeof( config.sql_DataBase) , param );
						printf("\n登陆数据库名：%s",config.sql_DataBase);
        } else if( strcmp( command , "sql_Table" ) == 0 ){
        		strcmp( config.sql_Table , param ) ;
            snprintf( config.sql_Table , sizeof( config.sql_Table) , param );
				  	printf("\n用户信息表名：  %s",config.sql_Table);
				} else if( strcmp( command , "sql_LOCK" ) == 0 ){
        		strcmp( config.sql_LOCK , param ) ;
            snprintf( config.sql_LOCK , sizeof( config.sql_LOCK) , param );
				  	printf("\n用户锁定表名：  %s",config.sql_LOCK);
				} else if( strcmp( command , "sql_NAME" ) == 0 ){
        		strcmp( config.sql_NAME , param ) ;
            snprintf( config.sql_NAME , sizeof( config.sql_NAME) , param );
				  	printf("\n账号字段名称：  %s",config.sql_NAME);
				} else if( strcmp( command , "sql_PASS" ) == 0 ){
        		strcmp( config.sql_PASS , param ) ;
            snprintf( config.sql_PASS , sizeof( config.sql_PASS) , param );
				  	printf("\n密码字段名称：  %s",config.sql_PASS);
				} else if( strcmp( command , "AutoReg" ) == 0 ){
        		AutoReg = atoi( param );
        		if(AutoReg){
        			printf("\n开放自动注册：YES");
        		}else{
        			printf("\n开放自动注册：NO");
        		}
    fclose(fp);
    return 0;
    		}
    }
}

BOOL sasql_init( void )
{
		if( mysql_init(&mysql) == NULL & readConfig("acserv.cf"))
		{
			  printf("\n数据库初始化失败！");
			  exit(1);
		    return FALSE;
		}
	
	  if( !mysql_real_connect( &mysql,
	          config.sql_IP,
	          config.sql_ID,//帐号
	          config.sql_PS,//密码
	          config.sql_DataBase,//选择的资料库
	          config.sql_Port,
	          NULL,
	          0 ) )
		{
			printf("\n数据库连接失败！\n");
			return FALSE;
		}
  
  	printf("\n数据库连接成功！\n");
  	return TRUE;
}

void sasql_close( void )
{
  mysql_close( &mysql );
}


int sasql_query(char *nm, char *pas){
	char sqlstr[256];
	sprintf(sqlstr,"select * from %s where %s=BINARY'%s'",config.sql_Table,config.sql_NAME,nm);
	
	if(!mysql_query(&mysql,sqlstr)){
	  int num_row=0;
	  mysql_result=mysql_store_result(&mysql);
	  num_row=mysql_num_rows(mysql_result);
		mysql_free_result(mysql_result);
	  if(num_row>0){
	  	mysql_row=mysql_fetch_row(mysql_result);
	  	if(strcmp(pas,mysql_row[1]) == 0){
	  		return 1;
		  }else{
		  	printf("用户%s密码错误！\n",nm);
		  	return 2;
		  }
	  }else{
	  	printf("用户%s未注册！\n",nm);
	  	return 3;
	  }
	}else{
		printf("\n数据库查找失败！\n");
		printf("重新连接数据库...");
		sasql_close();
		sasql_init();
		printf("完成\n");
		return 0;
	}
}

#ifdef _SQL_REGISTER
BOOL sasql_register(char *id, char *ps)
{
	char sqlstr[256];
//	if(AutoReg!=1)return FALSE;
	sprintf(sqlstr,"INSERT INTO %s (%s,%s,RegTime,Path) VALUES (BINARY'%s','%s',NOW(),'char/0x%x')",config.sql_Table, config.sql_NAME, config.sql_PASS, id, ps, getHash(id) & 0xff);
	if(!mysql_query(&mysql,sqlstr)){
		printf("\n新用户注册成功！\n");
		return TRUE;
	}
	printf("\n新用户注册失败！\n");
	return FALSE;
}
#endif


BOOL sasql_chehk_lock( char *idip )
{
	char sqlstr[256];
	sprintf(sqlstr,"select * from %s where %s=BINARY'%s'", config.sql_LOCK, config.sql_NAME, idip);
	if(!mysql_query(&mysql,sqlstr)){
		return TRUE;
	}
	return FALSE;
}

BOOL sasql_add_lock( char *idip )
{
	char sqlstr[256];
	sprintf(sqlstr,"INSERT INTO %s (%s) VALUES (BINARY'%s')", config.sql_LOCK, config.sql_NAME, idip);
	if(!mysql_query(&mysql,sqlstr)){
			printf("\n添加锁定%s成功！\n",idip);
			return TRUE;
	}
	return FALSE;
}

BOOL sasql_del_lock( char *idip )
{
	char sqlstr[256];
	sprintf(sqlstr,"delete from config.SQL_LOCK where %s=BINARY'%s'", config.sql_LOCK, config.sql_NAME, idip);
	if(!mysql_query(&mysql,sqlstr)){
		printf("\n解除锁定%s成功！\n",idip);
		return TRUE;
	}
	return FALSE;
}

BOOL sasql_craete_lock( void )
{

}

BOOL sasql_craete_userinfo( void )
{

}



#endif
