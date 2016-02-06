#include "version.h"
#include <signal.h>
#include <errno.h>
#include <unistd.h>

#include <stdio.h>

#include "buf.h"
#include "net.h"
#include "char.h"
#include "item.h"
#include "object.h"
#include "configfile.h"
#include "lssproto_serv.h"
#include "saacproto_cli.h"
#include "log.h"
#include "petmail.h"

/*------------------------------------------------------------
 *  卅仄
 ------------------------------------------------------------*/
static void endlsprotocol( void )
{
    lssproto_CleanupServer();
    saacproto_CleanupClient();
}

/*------------------------------------------------------------
 * 允屯化及忡绣仄卅仃木壬卅日卅中犯□正毛母件皿允月楮醒
 *  卅仄
 ------------------------------------------------------------*/
static void allDataDump( void )
{
  closeAllLogFile();
	storeCharaData();
}

/*------------------------------------------------------------
 *  卅仄
 ------------------------------------------------------------*/
void shutdownProgram( void )
{
    printf("关闭SAAC连接:%d\n",close( acfd ));
    printf("关闭绑定端口:%d\n",close( bindedfd ));
    memEnd();
}
#ifdef _GMSV_DEBUG
char *DebugMainFunction = NULL;
extern time_t initTime;
#endif
extern int player_online;
extern int player_maxonline;
char saacfunc[255]="";
void sigshutdown( int number )
{
		if( number == 0 ){
			print( "\nGMSV正常关闭\n" );
		}else{
			print( "\n=========以下是服务器出错原因=========\n");
	    print( "标准信息: %d\n" , number  );
#ifdef _GMSV_DEBUG
			print( "主 函 数: %s\n", DebugMainFunction );
#endif
			print( "在线人数: %d\n", player_online);
			print( "最高在线: %d\n", player_maxonline);
			print( "SAAC函数: %s\n", saacfunc);
#ifdef _GMSV_DEBUG
	    {
	    	time_t new_t;
	    	int dd,hh,mm,ss;
	    	char buf[128];
	    	time(&new_t);
	    	if(initTime==0){
	    		print( "运行时间: 尚未初始化完\n" );
	    	}else{
		    	new_t-=initTime;
			
					dd=(int) new_t / 86400; new_t=new_t % 86400;
		   		hh=(int) new_t / 3600;  new_t=new_t % 3600;
		      mm=(int) new_t / 60;    new_t=new_t % 60;
		      ss=(int) new_t;
		      
					if (dd>0) {
		      	snprintf( buf, sizeof( buf ) , "服务器共运行了 %d 日 %d 小时 %d 分 %d 秒。",dd,hh,mm,ss);
		      } else if (hh>0) {
		      	snprintf( buf, sizeof( buf ) , "服务器共运行了 %d 小时 %d 分 %d 秒。",hh,mm,ss);
		      } else {
		       	snprintf( buf, sizeof( buf ) , "服务器共运行了 %d 分 %d 秒。",mm,ss);
		      }
		      print( "运行时间: %s\n", buf );
	    	}
			}
#endif
			print( "=========以上是服务器出错原因=========\n");
		}
		remove( "gmsvlog.err2");
		rename( "gmsvlog.err1", "gmsvlog.err2" );
		rename( "gmsvlog.err", "gmsvlog.err1" );
		rename( "gmsvlog", "gmsvlog.err" );

    allDataDump();

    signal( SIGINT , SIG_IGN );
    signal( SIGQUIT, SIG_IGN );
    signal( SIGKILL, SIG_IGN );
    signal( SIGSEGV, SIG_IGN );
    signal( SIGPIPE, SIG_IGN );
    signal( SIGTERM, SIG_IGN );

    shutdownProgram();
    exit(number);
}



void signalset( void )
{
    // CoolFish: Test Signal 2001/10/26
    print("\n开始获取信号..\n");

		print("SIGINT:%d\n", SIGINT);
		print("SIGQUIT:%d\n", SIGQUIT);
		print("SIGKILL:%d\n", SIGKILL);
		print("SIGSEGV:%d\n", SIGSEGV);
		print("SIGPIPE:%d\n", SIGPIPE);
		print("SIGTERM:%d\n", SIGTERM);
    
    signal( SIGINT , sigshutdown );
    signal( SIGQUIT, sigshutdown );
    signal( SIGKILL, sigshutdown );
    signal( SIGSEGV, sigshutdown );
    signal( SIGPIPE, SIG_IGN );
    signal( SIGTERM, sigshutdown );
}
