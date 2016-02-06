#include "version.h"
#include "autil.h"
#include "lssproto_serv.h"
#include "common.h"	// for StoneAge
#include "log.h"	// for StoneAge
#include "char.h"
#include <stdio.h>
#include <time.h>
#include <net.h>


void lssproto_SetServerLogFiles( char *r , char *w )
{
    lssproto_strcpysafe( lssproto_writelogfilename , w , sizeof(lssproto_writelogfilename ));
    lssproto_strcpysafe( lssproto_readlogfilename , r , sizeof(lssproto_readlogfilename ));
}
int lssproto_InitServer(	int (*writefunc)(int,char*,int) ,	int worksiz )
{
    int i;
    if( (void*)writefunc == NULL) {
        lssproto.write_func = lssproto_default_write_wrap;
    } else {
        lssproto.write_func = writefunc;
    }
    lssproto_AllocateCommonWork(worksiz);
    lssproto_stringwrapper = (char**)calloc( 1,sizeof(char*) * MAXLSRPCARGS);
    if(lssproto_stringwrapper ==NULL)
        return -1;
    memset( lssproto_stringwrapper , 0, sizeof(char*)*MAXLSRPCARGS);
    for(i=0;i<MAXLSRPCARGS;i++) {
        lssproto_stringwrapper[i] = (char*)calloc( 1, worksiz );
        if( lssproto_stringwrapper[i] == NULL) {
            for(i=0;i<MAXLSRPCARGS;i++) {
                free( lssproto_stringwrapper[i]);
                return -1;
            }
        }
    }
    lssproto.ret_work = (char*)calloc( 1,sizeof( worksiz ));
    if( lssproto.ret_work == NULL ) {
        return -1;
    }
    return 0;
}

void lssproto_CleanupServer( void )
{
    int i;
    free( lssproto.work );
    free( lssproto.arraywork);
    free( lssproto.escapework );
    free( lssproto.val_str);
    free( lssproto.token_list );
    for(i=0;i<MAXLSRPCARGS;i++) {
        free( lssproto_stringwrapper[i]);
    }
    free( lssproto_stringwrapper );
    free( lssproto.ret_work );
}

#define DME() print("<DME(%d)%d:%d>",fd,__LINE__,func)

int lssproto_ServerDispatchMessage(int fd, char *encoded)
{
	int func,fieldcount;
	char raw[1024 * 64];

	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, _RUNNING_KEY);

	util_DecodeMessage(raw,encoded);
#ifdef _DEBUG_RET	
	print("\nraw=%s\n",raw);
#endif
	if( !util_SplitMessage(raw,SEPARATOR) ){
		print("\nDME1:package=%s\n",raw);
		DME(); return -1;
	}

	if (!util_GetFunctionFromSlice(&func, &fieldcount)) {
		logHack(fd,HACK_GETFUNCFAIL);
		DME(); return -1;
	}
#ifdef _DEBUG_RET	
	printf("\n客户端接口=%d\n",func);
#endif
	if (func==LSSPROTO_W_RECV) {
		int checksum=0, checksumrecv;
		int x;
		int y;
		char direction[1024 * 64];

		checksum += util_deint(2, &x);
		checksum += util_deint(3, &y);
		checksum += util_destring(4, direction);
		util_deint(5, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			logHack(fd, HACK_CHECKSUMERROR);
			DME(); return -1;
		}
#ifdef _DEBUG_RET_CLI
		printf("[接收]LSSPROTO_W_RECV-x:%d,y:%d,direction:%s\n", x, y, direction);
#endif
		lssproto_W_recv(fd, x, y, direction);
		util_DiscardMessage();
		return 0;
	}

	if (func==LSSPROTO_W2_RECV) {
		int checksum=0, checksumrecv;
		int x;
		int y;
		char direction[1024 * 64];

		checksum += util_deint(2, &x);
		checksum += util_deint(3, &y);
		checksum += util_destring(4, direction);
		util_deint(5, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			logHack(fd, HACK_CHECKSUMERROR);
			DME(); return -1;
		}
#ifdef _DEBUG_RET_CLI
		printf("[接收]LSSPROTO_W2_RECV-x:%d,y:%d,direction:%s\n", x, y, direction);
#endif
		lssproto_W2_recv(fd, x, y, direction);
		util_DiscardMessage();
		return 0;
	}

	if (func==LSSPROTO_EV_RECV) {
		int checksum=0, checksumrecv;
		int event;
		int seqno;
		int x;
		int y;
		int dir;

		checksum += util_deint(2, &event);
		checksum += util_deint(3, &seqno);
		checksum += util_deint(4, &x);
		checksum += util_deint(5, &y);
		checksum += util_deint(6, &dir);
		util_deint(7, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			logHack(fd, HACK_CHECKSUMERROR);
			DME(); return -1;
		}
#ifdef _DEBUG_RET_CLI
		printf("[接收]LSSPROTO_EV_RECV-event:%d,seqno:%d,x:%d,y:%d,dir:%d\n", event, seqno, x, y, dir);
#endif
		lssproto_EV_recv(fd, event, seqno, x, y, dir);
		util_DiscardMessage();
		return 0;
	}

	if (func==LSSPROTO_DU_RECV) {
		int checksum=0, checksumrecv;
		int x;
		int y;

		checksum += util_deint(2, &x);
		checksum += util_deint(3, &y);
		util_deint(4, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			logHack(fd, HACK_CHECKSUMERROR);
			DME(); return -1;
		}
#ifdef _DEBUG_RET_CLI
		printf("[接收]LSSPROTO_DU_RECV-x:%d,y:%d\n", x, y);
#endif
		lssproto_DU_recv(fd, x, y);
		util_DiscardMessage();
		return 0;
	}

	if (func==LSSPROTO_EO_RECV) {
		int checksum=0, checksumrecv;
		int dummy;

		checksum += util_deint(2, &dummy);
		util_deint(3, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			logHack(fd, HACK_CHECKSUMERROR);
			DME(); return -1;
		}
#ifdef _DEBUG_RET_CLI
		printf("[接收]LSSPROTO_EO_RECV-dummy:%d\n", dummy);
#endif
		lssproto_EO_recv(fd, dummy);
		util_DiscardMessage();
		return 0;
	}
     
	if (func==LSSPROTO_BU_RECV) {
		int checksum=0, checksumrecv;
		int dummy;

		checksum += util_deint(2, &dummy);
		util_deint(3, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			logHack(fd, HACK_CHECKSUMERROR);
			DME(); return -1;
		}
#ifdef _DEBUG_RET_CLI
		printf("[接收]LSSPROTO_BU_RECV-dummy:%d\n", dummy);
#endif
		lssproto_BU_recv(fd, dummy);
		util_DiscardMessage();
		return 0;
	}

	if (func==LSSPROTO_JB_RECV) {
		int checksum=0, checksumrecv;
		int x;
		int y;

		checksum += util_deint(2, &x);
		checksum += util_deint(3, &y);
		util_deint(4, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			logHack(fd, HACK_CHECKSUMERROR);
			DME(); return -1;
		}
#ifdef _DEBUG_RET_CLI
		printf("[接收]LSSPROTO_JB_RECV-x:%d,y:%d\n", x, y);
#endif
		lssproto_JB_recv(fd, x, y);
		util_DiscardMessage();
		return 0;
	}

	if (func==LSSPROTO_LB_RECV) {
		int checksum=0, checksumrecv;
		int x;
		int y;

		checksum += util_deint(2, &x);
		checksum += util_deint(3, &y);
		util_deint(4, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			logHack(fd, HACK_CHECKSUMERROR);
			DME(); return -1;
		}
#ifdef _DEBUG_RET_CLI
		printf("[接收]LSSPROTO_LB_RECV-x:%d,y:%d\n", x, y);
#endif
		lssproto_LB_recv(fd, x, y);
		util_DiscardMessage();
		return 0;
	}

	if (func==LSSPROTO_B_RECV) {
		int checksum=0, checksumrecv;
		char command[1024 * 64];

		checksum += util_destring(2, command);
		util_deint(3, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			logHack(fd, HACK_CHECKSUMERROR);
			DME(); return -1;
		}		
#ifdef _DEBUG_RET_CLI
		printf("[接收]LSSPROTO_B_RECV-command:%d\n", command);
#endif
		lssproto_B_recv(fd, command);
		util_DiscardMessage();
		return 0;
	}

	if (func==LSSPROTO_SKD_RECV) {
		int checksum=0, checksumrecv;
		int dir;
		int index;

		checksum += util_deint(2, &dir);
		checksum += util_deint(3, &index);
		util_deint(4, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			logHack(fd, HACK_CHECKSUMERROR);
			DME(); return -1;
		}
#ifdef _DEBUG_RET_CLI
		printf("[接收]LSSPROTO_SKD_RECV-dir:%d,index:%d\n", dir, index);
#endif
		lssproto_SKD_recv(fd, dir, index);
		util_DiscardMessage();
		return 0;
	}

	if (func==LSSPROTO_ID_RECV) {
		int checksum=0, checksumrecv;
		int x;
		int y;
		int haveitemindex;
		int toindex;

		checksum += util_deint(2, &x);
		checksum += util_deint(3, &y);
		checksum += util_deint(4, &haveitemindex);
		checksum += util_deint(5, &toindex);
		util_deint(6, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			logHack(fd, HACK_CHECKSUMERROR);
			DME(); return -1;
		}
#ifdef _DEBUG_RET_CLI
		printf("[接收]LSSPROTO_ID_RECV-x:%d,y:%d,haveitemindex:%d,toindex:%d\n", x, y, haveitemindex, toindex);
#endif
		lssproto_ID_recv(fd, x, y, haveitemindex, toindex);
		util_DiscardMessage();
		return 0;
	}

	if (func==LSSPROTO_PI_RECV) {
		int checksum=0, checksumrecv;
		int x;
		int y;
		int dir;

		checksum += util_deint(2, &x);
		checksum += util_deint(3, &y);
		checksum += util_deint(4, &dir);
		util_deint(5, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			logHack(fd, HACK_CHECKSUMERROR);
			DME(); return -1;
		}
#ifdef _DEBUG_RET_CLI
		printf("[接收]LSSPROTO_PI_RECV-x:%d,y:%d,dir:%d\n", x, y, dir);
#endif
		lssproto_PI_recv(fd, x, y, dir);
		util_DiscardMessage();
		return 0;
	}

	if (func==LSSPROTO_DI_RECV) {
		int checksum=0, checksumrecv;
		int x;
		int y;
		int itemindex;

		checksum += util_deint(2, &x);
		checksum += util_deint(3, &y);
		checksum += util_deint(4, &itemindex);
		util_deint(5, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			logHack(fd, HACK_CHECKSUMERROR);
			DME(); return -1;
		}
#ifdef _DEBUG_RET_CLI
		printf("[接收]LSSPROTO_DI_RECV-x:%d,y:%d,itemindex:%d\n", x, y, itemindex);
#endif
		lssproto_DI_recv(fd, x, y, itemindex);
		util_DiscardMessage();
		return 0;
	}

	if (func==LSSPROTO_DG_RECV) {
		int checksum=0, checksumrecv;
		int x;
		int y;
		int amount;

		checksum += util_deint(2, &x);
		checksum += util_deint(3, &y);
		checksum += util_deint(4, &amount);
		util_deint(5, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			logHack(fd, HACK_CHECKSUMERROR);
			DME(); return -1;
		}
#ifdef _DEBUG_RET_CLI
		printf("[接收]LSSPROTO_DG_RECV-x:%d,y:%d,amount:%d\n", x, y, amount);
#endif
		lssproto_DG_recv(fd, x, y, amount);
		util_DiscardMessage();
		return 0;
	}

	if (func==LSSPROTO_DP_RECV) {
		int checksum=0, checksumrecv;
		int x;
		int y;
		int petindex;

		checksum += util_deint(2, &x);
		checksum += util_deint(3, &y);
		checksum += util_deint(4, &petindex);
		util_deint(5, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			logHack(fd, HACK_CHECKSUMERROR);
			DME(); return -1;
		}
#ifdef _DEBUG_RET_CLI
		printf("[接收]LSSPROTO_DP_RECV-x:%d,y:%d,petindex:%d\n", x, y, petindex);
#endif
		lssproto_DP_recv(fd, x, y, petindex);
		util_DiscardMessage();
		return 0;
	}

	if (func==LSSPROTO_MI_RECV) {
		int checksum=0, checksumrecv;
		int fromindex;
		int toindex;

		checksum += util_deint(2, &fromindex);
		checksum += util_deint(3, &toindex);
		util_deint(4, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			logHack(fd, HACK_CHECKSUMERROR);
			DME(); return -1;
		}
#ifdef _DEBUG_RET_CLI
		printf("[接收]LSSPROTO_MI_RECV-fromindex:%d,toindex:%d\n", fromindex, toindex);
#endif
		lssproto_MI_recv(fd, fromindex, toindex);
		util_DiscardMessage();
		return 0;
	}

	if (func==LSSPROTO_MSG_RECV) {
		int checksum=0, checksumrecv;
		int index;
		char message[1024 * 64];
		int color;

		checksum += util_deint(2, &index);
		checksum += util_destring(3, message);
		checksum += util_deint(4, &color);
		util_deint(5, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			logHack(fd, HACK_CHECKSUMERROR);
			DME(); return -1;
		}
#ifdef _DEBUG_RET_CLI
		printf("[接收]LSSPROTO_MSG_RECV-index:%d,message:%s,color:%d\n", index, message, color);
#endif
		lssproto_MSG_recv(fd, index, message, color);
		util_DiscardMessage();
		return 0;
	}

	if (func==LSSPROTO_PMSG_RECV) {
		int checksum=0, checksumrecv;
		int index;
		int petindex;
		int itemindex;
		char message[1024 * 64];
		int color;

		checksum += util_deint(2, &index);
		checksum += util_deint(3, &petindex);
		checksum += util_deint(4, &itemindex);
		checksum += util_destring(5, message);
		checksum += util_deint(6, &color);
		util_deint(7, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			logHack(fd, HACK_CHECKSUMERROR);
			DME(); return -1;
		}
#ifdef _DEBUG_RET_CLI
		printf("[接收]LSSPROTO_PMSG_RECV-index:%d,petindex:%d,itemindex:%d,message:%s,color:%d\n", index, petindex, itemindex, message, color);
#endif
		lssproto_PMSG_recv(fd, index, petindex, itemindex, message, color);
		util_DiscardMessage();
		return 0;
	}

	if (func==LSSPROTO_AB_RECV) {
		int checksum=0, checksumrecv;
		util_deint(2, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			logHack(fd, HACK_CHECKSUMERROR);
			DME(); return -1;
		}
#ifdef _DEBUG_RET_CLI
		printf("[接收]LSSPROTO_AB_RECV\n");
#endif
		lssproto_AB_recv(fd);
		util_DiscardMessage();
		return 0;
	}

	if (func==LSSPROTO_DAB_RECV) {
		int checksum=0, checksumrecv;
		int index;

		checksum += util_deint(2, &index);
		util_deint(3, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			logHack(fd, HACK_CHECKSUMERROR);
			DME(); return -1;
		}
#ifdef _DEBUG_RET_CLI
		printf("[接收]LSSPROTO_DAB_RECV-index:%d\n", index);
#endif
		lssproto_DAB_recv(fd, index);
		util_DiscardMessage();
		return 0;
	}

	if (func==LSSPROTO_AAB_RECV) {
		int checksum=0, checksumrecv;
		int x;
		int y;

		checksum += util_deint(2, &x);
		checksum += util_deint(3, &y);
		util_deint(4, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			logHack(fd, HACK_CHECKSUMERROR);
			DME(); return -1;
		}
#ifdef _DEBUG_RET_CLI
		printf("[接收]LSSPROTO_AAB_RECV-x:%d,y:%d\n", x, y);
#endif
		lssproto_AAB_recv(fd, x, y);
		util_DiscardMessage();
		return 0;
	}

	if (func==LSSPROTO_L_RECV) {
		int checksum=0, checksumrecv;
		int dir;

		checksum += util_deint(2, &dir);
		util_deint(3, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			logHack(fd, HACK_CHECKSUMERROR);
			DME(); return -1;
		}
#ifdef _DEBUG_RET_CLI
		printf("[接收]LSSPROTO_L_RECV-dir:%d\n", dir);
#endif
		lssproto_L_recv(fd, dir);
		util_DiscardMessage();
		return 0;
	}

	if (func==LSSPROTO_TK_RECV) {
		int checksum=0, checksumrecv;
		int x;
		int y;
		char message[1024 * 64];
		int color;
		int area;

		checksum += util_deint(2, &x);
		checksum += util_deint(3, &y);
		checksum += util_destring(4, message);
		checksum += util_deint(5, &color);
		checksum += util_deint(6, &area);
		util_deint(7, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			logHack(fd, HACK_CHECKSUMERROR);
			DME(); return -1;
		}
#ifdef _DEBUG_RET_CLI
		printf("[接收]LSSPROTO_TK_RECV-x:%d,y:%d,message:%s,color:%d,area:%d\n", x, y, message, color, area);
#endif
		lssproto_TK_recv(fd, x, y, message, color, area);
		util_DiscardMessage();
		return 0;
	}

	if (func==LSSPROTO_M_RECV) {
		int checksum=0, checksumrecv;
		int fl;
		int x1;
		int y1;
		int x2;
		int y2;

		checksum += util_deint(2, &fl);
		checksum += util_deint(3, &x1);
		checksum += util_deint(4, &y1);
		checksum += util_deint(5, &x2);
		checksum += util_deint(6, &y2);
		util_deint(7, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			logHack(fd, HACK_CHECKSUMERROR);
			DME(); return -1;
		}
#ifdef _DEBUG_RET_CLI
		printf("[接收]LSSPROTO_M_RECV-fl:%d,x1:%d,y1:%d,x2:%d,y2:%d\n", fl, x1, y1, x2, y2);
#endif
		lssproto_M_recv(fd, fl, x1, y1, x2, y2);
		util_DiscardMessage();
		return 0;
	}

	if (func==LSSPROTO_C_RECV) {
		int checksum=0, checksumrecv;
		int index;

		checksum += util_deint(2, &index);
		util_deint(3, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			logHack(fd, HACK_CHECKSUMERROR);
			DME(); return -1;
		}
#ifdef _DEBUG_RET_CLI
		printf("[接收]LSSPROTO_C_RECV-index:%d\n", index);
#endif
		lssproto_C_recv(fd, index);
		util_DiscardMessage();
		return 0;
	}

	if (func==LSSPROTO_S_RECV) {
		int checksum=0, checksumrecv;
		char category[1024 * 64];

		checksum += util_destring(2, category);
		util_deint(3, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			logHack(fd, HACK_CHECKSUMERROR);
			DME(); return -1;
		}
#ifdef _DEBUG_RET_CLI
		printf("[接收]LSSPROTO_S_RECV-category:%s\n", category);
#endif
		lssproto_S_recv(fd, category);
		util_DiscardMessage();
		return 0;
	}

	if (func==LSSPROTO_FS_RECV) {
		int checksum=0, checksumrecv;
		int flg;

		checksum += util_deint(2, &flg);
		util_deint(3, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			logHack(fd, HACK_CHECKSUMERROR);
			DME(); return -1;
		}
#ifdef _DEBUG_RET_CLI
		printf("[接收]LSSPROTO_FS_RECV-flg:%d\n", flg);
#endif
		lssproto_FS_recv(fd, flg);
		util_DiscardMessage();
		return 0;
	}

	if (func==LSSPROTO_HL_RECV) {
		int checksum=0, checksumrecv;
		int flg;

		checksum += util_deint(2, &flg);
		util_deint(3, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			logHack(fd, HACK_CHECKSUMERROR);
			DME(); return -1;
		}
#ifdef _DEBUG_RET_CLI
		printf("[接收]LSSPROTO_HL_RECV-flg:%d\n", flg);
#endif
		lssproto_HL_recv(fd, flg);
		util_DiscardMessage();
		return 0;
	}

	if (func==LSSPROTO_PR_RECV) {
		int checksum=0, checksumrecv;
		int x;
		int y;
		int request;

		checksum += util_deint(2, &x);
		checksum += util_deint(3, &y);
		checksum += util_deint(4, &request);
		util_deint(5, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			logHack(fd, HACK_CHECKSUMERROR);
			DME(); return -1;
		}
#ifdef _DEBUG_RET_CLI
		printf("[接收]LSSPROTO_PR_RECV-x:%d,y:%d,request:%d\n", x, y, request);
#endif
		lssproto_PR_recv(fd, x, y, request);
		util_DiscardMessage();
		return 0;
	}

	if (func==LSSPROTO_KS_RECV) {
		int checksum=0, checksumrecv;
		int petarray;
		checksum += util_deint(2, &petarray);
		util_deint(3, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			logHack(fd, HACK_CHECKSUMERROR);
			DME(); return -1;
		}
#ifdef _DEBUG_RET_CLI
		printf("[接收]LSSPROTO_KS_RECV-petarray:%d\n", petarray);
#endif
		lssproto_KS_recv(fd, petarray);
		util_DiscardMessage();
		return 0;
	}

#ifdef _STANDBYPET
	if (func==LSSPROTO_SPET_RECV) {
		int checksum=0, checksumrecv;
		int standbypet;
		checksum += util_deint(2, &standbypet);
		util_deint(3, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			logHack(fd, HACK_CHECKSUMERROR);
			DME(); return -1;
		}
#ifdef _DEBUG_RET_CLI
		printf("[接收]LSSPROTO_SPET_RECV-standbypet:%d\n", standbypet);
#endif
		lssproto_SPET_recv(fd, standbypet);
		util_DiscardMessage();
		return 0;
	}
#endif

	if (func==LSSPROTO_AC_RECV) {
		int checksum=0, checksumrecv;
		int x;
		int y;
		int actionno;

		checksum += util_deint(2, &x);
		checksum += util_deint(3, &y);
		checksum += util_deint(4, &actionno);
		util_deint(5, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			logHack(fd, HACK_CHECKSUMERROR);
			DME(); return -1;
		}
#ifdef _DEBUG_RET_CLI
		printf("[接收]LSSPROTO_AC_RECV-x:%d,y:%d,actionno:%d\n", x, y, actionno);
#endif
		lssproto_AC_recv(fd, x, y, actionno);
		util_DiscardMessage();
		return 0;
	}

	if (func==LSSPROTO_MU_RECV) {
		int checksum=0, checksumrecv;
		int x;
		int y;
		int array;
		int toindex;

		checksum += util_deint(2, &x);
		checksum += util_deint(3, &y);
		checksum += util_deint(4, &array);
		checksum += util_deint(5, &toindex);
		util_deint(6, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			logHack(fd, HACK_CHECKSUMERROR);
			DME(); return -1;
		}
#ifdef _DEBUG_RET_CLI
		printf("[接收]LSSPROTO_MU_RECV-x:%d,y:%d,array:%d,toindex:%d\n", x, y, array, toindex);
#endif
		lssproto_MU_recv(fd, x, y, array, toindex);
		util_DiscardMessage();
		return 0;
	}

	if (func==LSSPROTO_PS_RECV) {
		int checksum=0, checksumrecv;
		int havepetindex;
		int havepetskill;
		int toindex;
		char data[1024 * 64];
		
		checksum += util_deint(2, &havepetindex);
		checksum += util_deint(3, &havepetskill);
		checksum += util_deint(4, &toindex);
		checksum += util_destring(5, data);
		util_deint(6, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			logHack(fd, HACK_CHECKSUMERROR);

			DME(); return -1;
		}
#ifdef _DEBUG_RET_CLI
		printf("[接收]LSSPROTO_PS_RECV-havepetindex:%d,havepetskill:%d,toindex:%d,data:%s\n", havepetindex, havepetskill, toindex, data);
#endif
		lssproto_PS_recv(fd, havepetindex, havepetskill, toindex, data);
		
		util_DiscardMessage();
		return 0;
	}

	if (func==LSSPROTO_ST_RECV) {
		int checksum=0, checksumrecv;
		int titleindex;

		checksum += util_deint(2, &titleindex);
		util_deint(3, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			logHack(fd, HACK_CHECKSUMERROR);
			DME(); return -1;
		}
#ifdef _DEBUG_RET_CLI
		printf("[接收]LSSPROTO_ST_RECV-titleindex:%d\n", titleindex);
#endif
		lssproto_ST_recv(fd, titleindex);
		util_DiscardMessage();
		return 0;
	}

	if (func==LSSPROTO_DT_RECV) {
		int checksum=0, checksumrecv;
		int titleindex;

		checksum += util_deint(2, &titleindex);
		util_deint(3, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			logHack(fd, HACK_CHECKSUMERROR);
			DME(); return -1;
		}
#ifdef _DEBUG_RET_CLI
		printf("[接收]LSSPROTO_DT_RECV-titleindex:%d\n", titleindex);
#endif
		lssproto_DT_recv(fd, titleindex);
		util_DiscardMessage();
		return 0;
	}

	if (func==LSSPROTO_FT_RECV) {
		int checksum=0, checksumrecv;
		char data[1024 * 64];

		checksum += util_destring(2, data);
		util_deint(3, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			logHack(fd, HACK_CHECKSUMERROR);
			DME(); return -1;
		}
#ifdef _DEBUG_RET_CLI
		printf("[接收]LSSPROTO_FT_RECV-data:%s\n", data);
#endif
		lssproto_FT_recv(fd, data);
		util_DiscardMessage();
		return 0;
	}

	if (func==LSSPROTO_SKUP_RECV) {
		int checksum=0, checksumrecv;
		int skillid;

		checksum += util_deint(2, &skillid);
		util_deint(3, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			logHack(fd, HACK_CHECKSUMERROR);
			DME(); return -1;
		}
#ifdef _DEBUG_RET_CLI
		printf("[接收]LSSPROTO_SKUP_RECV-skillid:%d\n", skillid);
#endif
		lssproto_SKUP_recv(fd, skillid);
		util_DiscardMessage();
		return 0;
	}

	if (func==LSSPROTO_KN_RECV) {
		int checksum=0, checksumrecv;
		int havepetindex;
		char data[1024 * 64];

		checksum += util_deint(2, &havepetindex);
		checksum += util_destring(3, data);
		util_deint(4, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			logHack(fd, HACK_CHECKSUMERROR);
			DME(); return -1;
		}
#ifdef _DEBUG_RET_CLI
		printf("[接收]LSSPROTO_KN_RECV-havepetindex:%d,data:%s\n", havepetindex, data);
#endif
		lssproto_KN_recv(fd, havepetindex, data);
		util_DiscardMessage();
		return 0;
	}

	if (func==LSSPROTO_WN_RECV) {
		int checksum=0, checksumrecv;
		int x;
		int y;
		int seqno;
		int objindex;
		int select;
		char data[1024 * 64];

		checksum += util_deint(2, &x);
		checksum += util_deint(3, &y);
		checksum += util_deint(4, &seqno);
		checksum += util_deint(5, &objindex);
		checksum += util_deint(6, &select);
		checksum += util_destring(7, data);
		
		util_deint(8, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			logHack(fd, HACK_CHECKSUMERROR);
			DME(); return -1;
		}
#ifdef _DEBUG_RET_CLI
		printf("[接收]LSSPROTO_WN_RECV-x:%d,y:%d,seqno:%d,objindex:%d,select:%d,data:%s\n", x, y, seqno, objindex, select, data);
#endif
		lssproto_WN_recv(fd, x, y, seqno, objindex, select, data);
		util_DiscardMessage();
		return 0;
	}

	if (func==LSSPROTO_SP_RECV) {
		int checksum=0, checksumrecv;
		int x;
		int y;
		int dir;

		checksum += util_deint(2, &x);
		checksum += util_deint(3, &y);
		checksum += util_deint(4, &dir);
		util_deint(5, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			logHack(fd, HACK_CHECKSUMERROR);
			DME(); return -1;
		}
#ifdef _DEBUG_RET_CLI
		printf("[接收]LSSPROTO_SP_RECV-x:%d,y:%d,dir:%d\n", x, y, dir);
#endif
		lssproto_SP_recv(fd, x, y, dir);
		util_DiscardMessage();
		return 0;
	}

	if (func==LSSPROTO_CLIENTLOGIN_RECV) {
		int checksum=0, checksumrecv;
		char cdkey[CDKEYLEN];
		char passwd[PASSWDLEN];
		
		strcpy(PersonalKey, _DEFAULT_PKEY);

		checksum += util_destring(2, cdkey);
		checksum += util_destring(3, passwd);
		
		util_deint(4, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			
			logHack(fd, HACK_CHECKSUMERROR);
			DME(); return -1;
		}
#ifdef _DEBUG_RET_CLI
		printf("[接收]LSSPROTO_CLIENTLOGIN_RECV-cdkey:%s,passwd:%s\n", cdkey, passwd);
#endif
		lssproto_ClientLogin_recv(fd, cdkey, passwd);
		util_DiscardMessage();
		return 0;
	}

	if (func==LSSPROTO_CREATENEWCHAR_RECV) {
		int checksum=0, checksumrecv;
		int dataplacenum;
		char charname[CHARNAMELEN];;
		int imgno;
		int faceimgno;
		int vital;
		int str;
		int tgh;
		int dex;
		int earth;
		int water;
		int fire;
		int wind;
		int hometown;

		checksum += util_deint(2, &dataplacenum);
		checksum += util_destring(3, charname);
		checksum += util_deint(4, &imgno);
		checksum += util_deint(5, &faceimgno);
		checksum += util_deint(6, &vital);
		checksum += util_deint(7, &str);
		checksum += util_deint(8, &tgh);
		checksum += util_deint(9, &dex);
		checksum += util_deint(10, &earth);
		checksum += util_deint(11, &water);
		checksum += util_deint(12, &fire);
		checksum += util_deint(13, &wind);
		checksum += util_deint(14, &hometown);
		util_deint(15, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			logHack(fd, HACK_CHECKSUMERROR);
			DME(); return -1;
		}
#ifdef _DEBUG_RET_CLI
		printf("[接收]LSSPROTO_CREATENEWCHAR_RECV-dataplacenum:%d,charname:%s,imgno:%d,faceimgno:%d,vital:%d,str:%d,"
						"tgh:%d,dex:%d,earth:%d,water:%d,fire:%d,wind:%d,hometown:%d\n",
						dataplacenum, charname, imgno, faceimgno, vital, str, tgh, dex, earth, water, fire, wind, hometown);
#endif
		lssproto_CreateNewChar_recv(fd, dataplacenum, charname, imgno, faceimgno, vital, str, tgh, dex, earth, water, fire, wind, hometown);
		util_DiscardMessage();
		return 0;
	}

	if (func==LSSPROTO_CHARDELETE_RECV) {
		int checksum=0, checksumrecv;
		char charname[CHARNAMELEN];;

		checksum += util_destring(2, charname);
		util_deint(3, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			logHack(fd, HACK_CHECKSUMERROR);
			DME(); return -1;
		}
#ifdef _DEBUG_RET_CLI
		printf("[接收]LSSPROTO_CHARDELETE_RECV-charname:%s\n", charname);
#endif
		lssproto_CharDelete_recv(fd, charname);
		util_DiscardMessage();
		return 0;
	}

	if (func==LSSPROTO_CHARLOGIN_RECV) {
		int checksum=0, checksumrecv;
		char charname[CHARNAMELEN];;

		checksum += util_destring(2, charname);
		util_deint(3, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			logHack(fd, HACK_CHECKSUMERROR);
			DME(); return -1;
		}
#ifdef _DEBUG_RET_CLI
		printf("[接收]LSSPROTO_CHARLOGIN_RECV-charname:%s\n", charname);
#endif
		lssproto_CharLogin_recv(fd, charname);
		util_DiscardMessage();
		return 0;
	}

	if (func==LSSPROTO_CHARLIST_RECV) {
		int checksum=0, checksumrecv;
		util_deint(2, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			logHack(fd, HACK_CHECKSUMERROR);
			DME(); return -1;
		}
#ifdef _DEBUG_RET_CLI
		printf("[接收]LSSPROTO_CHARLIST_RECV\n");
#endif
		lssproto_CharList_recv( fd);

		util_DiscardMessage();
		return 0;
	}

	if (func==LSSPROTO_CHARLOGOUT_RECV) {
		int checksum=0, checksumrecv;
		int Flg=1;
		util_deint(2, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			logHack(fd, HACK_CHECKSUMERROR);
			DME(); return -1;
		}
#ifdef _DEBUG_RET_CLI
		printf("[接收]LSSPROTO_CHARLOGOUT_RECV-Flg:%d\n", Flg);
#endif
		lssproto_CharLogout_recv(fd, Flg);
		util_DiscardMessage();
		return 0;
	}

	if (func==LSSPROTO_PROCGET_RECV) {
		int checksum=0, checksumrecv;
		
		strcpy( PersonalKey, _DEFAULT_PKEY);

		util_deint(2, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			logHack(fd, HACK_CHECKSUMERROR);
			DME(); return -1;
		}
#ifdef _DEBUG_RET_CLI
		printf("[接收]LSSPROTO_PROCGET_RECV\n");
#endif
		lssproto_ProcGet_recv(fd);
		util_DiscardMessage();
		return 0;
	}

	if (func==LSSPROTO_PLAYERNUMGET_RECV) {
		int checksum=0, checksumrecv;
		util_deint(2, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			logHack(fd, HACK_CHECKSUMERROR);
			DME(); return -1;
		}
#ifdef _DEBUG_RET_CLI
		printf("[接收]LSSPROTO_PLAYERNUMGET_RECV\n");
#endif
		lssproto_PlayerNumGet_recv(fd);
		util_DiscardMessage();
		return 0;
	}

	if (func==LSSPROTO_ECHO_RECV) {
		int checksum=0, checksumrecv;
		char test[1024 * 64];

		checksum += util_destring(2, test);
		util_deint(3, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			logHack(fd, HACK_CHECKSUMERROR);
			DME(); return -1;
		}
#ifdef _DEBUG_RET_CLI
		printf("[接收]LSSPROTO_ECHO_RECV-fd:%d,test:%s\n", fd, test);
#endif
		lssproto_Echo_recv(fd, test);
		util_DiscardMessage();
		return 0;
	}

	if (func==LSSPROTO_SHUTDOWN_RECV) {
		int checksum=0, checksumrecv;
		char passwd[1024 * 64];
		int min;

		checksum += util_destring(2, passwd);
		checksum += util_deint(3, &min);
		util_deint(4, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			logHack(fd, HACK_CHECKSUMERROR);
			DME(); return -1;
		}
#ifdef _DEBUG_RET_CLI
		printf("[接收]LSSPROTO_SHUTDOWN_RECV-passwd:%s,min:%d\n", passwd,min);
#endif
		lssproto_Shutdown_recv(fd, passwd, min);
		util_DiscardMessage();
		return 0;
	}

	if (func==LSSPROTO_TD_RECV) {
		int checksum=0, checksumrecv;
		char message[1024 * 64];

		checksum += util_destring(2, message);
		util_deint(3, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			logHack(fd, HACK_CHECKSUMERROR);
			DME(); return -1;
		}
#ifdef _DEBUG_RET_CLI
		printf("[接收]LSSPROTO_TD_RECV-message:%s\n", message);
#endif
		lssproto_TD_recv(fd, message);
		util_DiscardMessage();
		return 0;
	}

	if (func==LSSPROTO_FM_RECV) {
		int checksum=0, checksumrecv;
		char message[1024 * 64];

		checksum += util_destring(2, message);
		util_deint(3, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			logHack(fd, HACK_CHECKSUMERROR);
			DME(); return -1;
		}
#ifdef _DEBUG_RET_CLI
		printf("[接收]LSSPROTO_FM_RECV-message:%s\n", message);
#endif
		lssproto_FM_recv(fd, message);
		util_DiscardMessage();
		return 0;
	}
    
	if (func==LSSPROTO_PETST_RECV) {
		int checksum=0, checksumrecv;
		int nPet;
		int sPet;		

		checksum += util_deint(2, &nPet);
		checksum += util_deint(3, &sPet);		
		util_deint(4, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			logHack(fd, HACK_CHECKSUMERROR);
			DME(); return -1;
		}	
#ifdef _DEBUG_RET_CLI
		printf("[接收]LSSPROTO_PETST_RECV-nPet:%d,sPet:%d\n", nPet, sPet);
#endif
		lssproto_PETST_recv(fd, nPet, sPet);		
		util_DiscardMessage();
		return 0;
	}

#ifdef _CHECK_GAMESPEED
	if (func==LSSPROTO_CS_RECV) {
	    char buffer[2];
	    buffer[0] = '\0';
#ifdef _DEBUG_RET_CLI
		printf("[接收]LSSPROTO_CS_RECV\n");
#endif
		lssproto_CS_recv( fd );
		util_DiscardMessage();
		return 0;
	}
#endif

#ifdef _MIND_ICON
	if(func==LSSPROTO_MA_RECV){
		int checksum = 0, checksumrecv;
		int nMind;
		int x, y;
		
		checksum += util_deint( 2, &nMind);
		checksum += util_deint( 3, &x);
		checksum += util_deint( 4, &y);
		util_deint( 5, &checksumrecv);
		if(checksum!=checksumrecv){
			util_DiscardMessage();
			logHack( fd, HACK_CHECKSUMERROR);
			DME(); return -1;
		}		
#ifdef _DEBUG_RET_CLI
		printf("[接收]LSSPROTO_MA_RECV-x:%d,y:%d,nMind:%d\n", x, y, nMind);
#endif
		lssproto_MA_recv(fd, x, y, nMind);
		util_DiscardMessage();
		return 0;
	}
#endif

#ifdef _OUTOFBATTLESKILL			// (不可开) Syu ADD 非战斗时技能Protocol
		if(func==LSSPROTO_BATTLESKILL_RECV){
				int checksum = 0, checksumrecv;
				int iNum;
				
				checksum += util_deint( 2, &iNum);
				util_deint( 3, &checksumrecv);
				if(checksum!=checksumrecv){
					util_DiscardMessage();
					logHack( fd, HACK_CHECKSUMERROR);
					DME(); return -1;
				}		
#ifdef _DEBUG_RET_CLI
		printf("[接收]LSSPROTO_BATTLESKILL_RECV-iNum:%d\n", iNum);
#endif
				lssproto_BATTLESKILL_recv(fd, iNum);
				util_DiscardMessage();
				return 0;
		}
#endif

	printf("\n无法找到客户端接口=%d\n",func);

	util_DiscardMessage();
	logHack(fd,HACK_NOTDISPATCHED);
	DME(); return -1;
}

/*
   servertoclient XYD( int x, int y, int dir );
*/
void lssproto_XYD_send(int fd,int x,int y,int dir)
{
#ifdef _DEBUG_SEND_CLI
	printf("[发送]LSSPROTO_XYD_SEND-x:%d,y:%d,dir:%d\n",x,y,dir);
#endif
	char buffer[1024 * 64];
	int checksum=0;

	strcpy(buffer,"");

	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, _RUNNING_KEY);

	checksum += util_mkint(buffer, x);
	checksum += util_mkint(buffer, y);
	checksum += util_mkint(buffer, dir);
	util_mkint(buffer, checksum);
	util_SendMesg(fd, LSSPROTO_XYD_SEND, buffer);
}

void lssproto_EV_send(int fd,int seqno,int result)
{
#ifdef _DEBUG_SEND_CLI
	printf("[发送]LSSPROTO_EV_SEND-seqno:%d,result:%d\n",seqno,result);
#endif
	char buffer[1024 * 64];
	int checksum=0;

	strcpy(buffer,"");

	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, _RUNNING_KEY);

	checksum += util_mkint(buffer, seqno);
	checksum += util_mkint(buffer, result);
	util_mkint(buffer, checksum);
	util_SendMesg(fd, LSSPROTO_EV_SEND, buffer);
}

void lssproto_EN_send(int fd,int result,int field)
{
#ifdef _DEBUG_SEND_CLI
	printf("[发送]LSSPROTO_EN_SEND-result:%d,field:%d\n",result,field);
#endif
	char buffer[1024 * 64];
	int checksum=0;

	//print(" EN_send ");

	strcpy(buffer,"");

	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, _RUNNING_KEY);

	checksum += util_mkint(buffer, result);
	checksum += util_mkint(buffer, field);
	util_mkint(buffer, checksum);
	util_SendMesg(fd, LSSPROTO_EN_SEND, buffer);
}


void lssproto_RS_send(int fd,char* data)
{
#ifdef _DEBUG_SEND_CLI
	printf("[发送]LSSPROTO_RS_SEND-data:%s\n",data);
#endif
	char buffer[1024 * 64];
	int checksum=0;

	strcpy(buffer,"");

	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, _RUNNING_KEY);

	checksum += util_mkstring(buffer, data);
	util_mkint(buffer, checksum);
	util_SendMesg(fd, LSSPROTO_RS_SEND, buffer);
}


void lssproto_RD_send(int fd,char* data)
{
#ifdef _DEBUG_SEND_CLI
	printf("[发送]LSSPROTO_RD_SEND-data:%s\n",data);
#endif
	char buffer[1024 * 64];
	int checksum=0;

	strcpy(buffer,"");

	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, _RUNNING_KEY);

	checksum += util_mkstring(buffer, data);
	util_mkint(buffer, checksum);
	util_SendMesg(fd, LSSPROTO_RD_SEND, buffer);
}

void lssproto_B_send(int fd,char* command)
{
#ifdef _DEBUG_SEND_CLI
	printf("[发送]LSSPROTO_B_SEND-command:%s\n",command);
#endif
	char buffer[1024 * 64];
	int checksum=0;

	strcpy(buffer,"");

	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, _RUNNING_KEY);

	checksum += util_mkstring(buffer, command);
	util_mkint(buffer, checksum);
	util_SendMesg(fd, LSSPROTO_B_SEND, buffer);
}

void lssproto_I_send(int fd,char* data)
{
#ifdef _DEBUG_SEND_CLI
	printf("[发送]LSSPROTO_I_SEND-data:%s\n",data);
#endif
	char buffer[1024 * 64];
	int checksum=0;

	strcpy(buffer,"");

	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, _RUNNING_KEY);

	checksum += util_mkstring(buffer, data);
	util_mkint(buffer, checksum);
	util_SendMesg(fd, LSSPROTO_I_SEND, buffer);
}

void lssproto_SI_send(int fd,int fromindex,int toindex)
{
#ifdef _DEBUG_SEND_CLI
	printf("[发送]LSSPROTO_SI_SEND-fromindex:%d,toindex:%d\n",fromindex,toindex);
#endif
	char buffer[1024 * 64];
	int checksum=0;

	strcpy(buffer,"");

	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, _RUNNING_KEY);
	checksum += util_mkint(buffer, fromindex);
	checksum += util_mkint(buffer, toindex);
	util_mkint(buffer, checksum);
	util_SendMesg(fd, LSSPROTO_SI_SEND, buffer);
}

void lssproto_MSG_send(int fd,int aindex,char* text,int color)
{
#ifdef _DEBUG_SEND_CLI
	printf("[发送]lssproto_MSG_send-aindex:%d,text:%s,color:%d\n",aindex,text,color);
#endif
	char buffer[1024 * 64];
	int checksum=0;

	strcpy(buffer,"");

	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, _RUNNING_KEY);

	checksum += util_mkint(buffer, aindex);
	checksum += util_mkstring(buffer, text);
	checksum += util_mkint(buffer, color);
	util_mkint(buffer, checksum);
	util_SendMesg(fd, LSSPROTO_MSG_SEND, buffer);
}


void lssproto_PME_send(int fd,int objindex,int graphicsno,int x,int y,int dir,int flg,int no,char* cdata)
{
#ifdef _DEBUG_SEND_CLI
	printf("[发送]LSSPROTO_PME_SEND-objindex:%d,graphicsno:%d,x:%d,y:%d,dir:%d,flg:%d,no:%d,cdata:%s\n",objindex,graphicsno,x,y,dir,flg,no,cdata);
#endif
	char buffer[1024 * 64];
	int checksum=0;

	strcpy(buffer,"");

	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, _RUNNING_KEY);

	checksum += util_mkint(buffer, objindex);
	checksum += util_mkint(buffer, graphicsno);
	checksum += util_mkint(buffer, x);
	checksum += util_mkint(buffer, y);
	checksum += util_mkint(buffer, dir);
	checksum += util_mkint(buffer, flg);
	checksum += util_mkint(buffer, no);
	checksum += util_mkstring(buffer, cdata);
	util_mkint(buffer, checksum);
	util_SendMesg(fd, LSSPROTO_PME_SEND, buffer);
}


void lssproto_AB_send(int fd,char* data)
{
#ifdef _DEBUG_SEND_CLI
	printf("[发送]LSSPROTO_AB_SEND-data:%s\n",data);
#endif
	char buffer[1024 * 64];
	int checksum=0;

	strcpy(buffer,"");

	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, _RUNNING_KEY);

	checksum += util_mkstring(buffer, data);
	util_mkint(buffer, checksum);
	util_SendMesg(fd, LSSPROTO_AB_SEND, buffer);
}


void lssproto_ABI_send(int fd,int num,char* data)
{
#ifdef _DEBUG_SEND_CLI
	printf("[发送]LSSPROTO_ABI_SEND-num:%d,data:%s\n",num,data);
#endif
	char buffer[1024 * 64];
	int checksum=0;

	strcpy(buffer,"");

	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, _RUNNING_KEY);

	checksum += util_mkint(buffer, num);
	checksum += util_mkstring(buffer, data);
	util_mkint(buffer, checksum);
	util_SendMesg(fd, LSSPROTO_ABI_SEND, buffer);
}

void lssproto_TK_send(int fd,int index,char* message,int color)
{
#ifdef _DEBUG_SEND_CLI
	printf("[发送]LSSPROTO_TK_SEND-index:%d,message:%s,color:%d\n",index,message,color);
#endif
	char buffer[1024 * 64];
	int checksum=0;

	strcpy(buffer,"");

	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, _RUNNING_KEY);

	checksum += util_mkint(buffer, index);
	checksum += util_mkstring(buffer, message);
	checksum += util_mkint(buffer, color);
	util_mkint(buffer, checksum);
	util_SendMesg(fd, LSSPROTO_TK_SEND, buffer);
}

void lssproto_MC_send(int fd,int fl,int x1,int y1,int x2,int y2,int tilesum,int objsum,int eventsum,char* data)
{
#ifdef _DEBUG_SEND_CLI
	printf("[发送]LSSPROTO_MC_SEND-fl:%d,x1:%d,y1:%d,x2:%d,y2:%d,tilesum:%d,objsum:%d,eventsum:%d,data:%s\n",fl,x1,y1,x2,y2,tilesum,objsum,eventsum,data);
#endif
	char buffer[1024 * 64];
	int checksum=0;

	strcpy(buffer,"");

	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, _RUNNING_KEY);

	checksum += util_mkint(buffer, fl);
	checksum += util_mkint(buffer, x1);
	checksum += util_mkint(buffer, y1);
	checksum += util_mkint(buffer, x2);
	checksum += util_mkint(buffer, y2);
	checksum += util_mkint(buffer, tilesum);
	checksum += util_mkint(buffer, objsum);
	checksum += util_mkint(buffer, eventsum);
	checksum += util_mkstring(buffer, data);
	util_mkint(buffer, checksum);
	util_SendMesg(fd, LSSPROTO_MC_SEND, buffer);
}


void lssproto_M_send(int fd,int fl,int x1,int y1,int x2,int y2,char* data)
{
#ifdef _DEBUG_SEND_CLI
	printf("[发送]LSSPROTO_M_SEND-fl:%d,x1:%d,y1:%d,x2:%d,y2:%d,data:%s\n",fl,x1,y1,x2,y2,data);
#endif
	char buffer[1024 * 64];
	int checksum=0;

	strcpy(buffer,"");

	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, _RUNNING_KEY);

	checksum += util_mkint(buffer, fl);
	checksum += util_mkint(buffer, x1);
	checksum += util_mkint(buffer, y1);
	checksum += util_mkint(buffer, x2);
	checksum += util_mkint(buffer, y2);
	checksum += util_mkstring(buffer, data);
	util_mkint(buffer, checksum);
	util_SendMesg(fd, LSSPROTO_M_SEND, buffer);
}

void lssproto_C_send(int fd,char* data)
{
#ifdef _DEBUG_SEND_CLI
	printf("[发送]LSSPROTO_C_SEND-data:%s\n",data);
#endif
	char buffer[1024 * 64];
	int checksum=0;

	strcpy(buffer,"");

	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, _RUNNING_KEY);

	checksum += util_mkstring(buffer, data);
	util_mkint(buffer, checksum);
	util_SendMesg(fd, LSSPROTO_C_SEND, buffer);
}

void lssproto_CA_send(int fd,char* data)
{
#ifdef _DEBUG_SEND_CLI
	printf("[发送]LSSPROTO_CA_SEND-data:%s\n",data);
#endif
	char buffer[1024 * 64];
	int checksum=0;

	strcpy(buffer,"");

	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, _RUNNING_KEY);

	checksum += util_mkstring(buffer, data);
	util_mkint(buffer, checksum);
	util_SendMesg(fd, LSSPROTO_CA_SEND, buffer);
}

void lssproto_CD_send(int fd,char* data)
{
#ifdef _DEBUG_SEND_CLI
	printf("[发送]LSSPROTO_CD_SEND-data:%s\n",data);
#endif
	char buffer[1024 * 64];
	int checksum=0;

	strcpy(buffer,"");

	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, _RUNNING_KEY);

	checksum += util_mkstring(buffer, data);
	util_mkint(buffer, checksum);
	util_SendMesg(fd, LSSPROTO_CD_SEND, buffer);
}

void lssproto_R_send(int fd,char* data)
{
#ifdef _DEBUG_SEND_CLI
	printf("[发送]LSSPROTO_R_SEND-data:%s\n",data);
#endif
	char buffer[1024 * 64];
	int checksum=0;

	strcpy(buffer,"");

	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, _RUNNING_KEY);

	checksum += util_mkstring(buffer, data);
	util_mkint(buffer, checksum);
	util_SendMesg(fd, LSSPROTO_R_SEND, buffer);
}

void lssproto_S_send(int fd,char* data)
{
#ifdef _DEBUG_SEND_CLI
	printf("[发送]LSSPROTO_S_SEND-data:%s\n",data);
#endif
	char buffer[1024 * 64];
	int checksum=0;

	strcpy(buffer,"");

	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, _RUNNING_KEY);

	checksum += util_mkstring(buffer, data);
	util_mkint(buffer, checksum);
	util_SendMesg(fd, LSSPROTO_S_SEND, buffer);
}

void lssproto_D_send(int fd,int category,int dx,int dy,char* data)
{
#ifdef _DEBUG_SEND_CLI
	printf("[发送]LSSPROTO_D_SEND-category:%d,dx:%d,dy:%d,data:%s\n",category,dx,dy,data);
#endif
	char buffer[1024 * 64];
	int checksum=0;

	strcpy(buffer,"");

	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, _RUNNING_KEY);

	checksum += util_mkint(buffer, category);
	checksum += util_mkint(buffer, dx);
	checksum += util_mkint(buffer, dy);
	checksum += util_mkstring(buffer, data);
	util_mkint(buffer, checksum);
	util_SendMesg(fd, LSSPROTO_D_SEND, buffer);
}

void lssproto_FS_send(int fd,int flg)
{
#ifdef _DEBUG_SEND_CLI
	printf("[发送]LSSPROTO_FS_SEND-flg:%d\n",flg);
#endif
	char buffer[1024 * 64];
	int checksum=0;

	strcpy(buffer,"");

	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, _RUNNING_KEY);

	checksum += util_mkint(buffer, flg);
	util_mkint(buffer, checksum);
	util_SendMesg(fd, LSSPROTO_FS_SEND, buffer);
}

void lssproto_HL_send(int fd,int flg)
{
#ifdef _DEBUG_SEND_CLI
	printf("[发送]LSSPROTO_HL_SEND-flg:%d\n",flg);
#endif
	char buffer[1024 * 64];
	int checksum=0;

	strcpy(buffer,"");

	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, _RUNNING_KEY);

	checksum += util_mkint(buffer, flg);
	util_mkint(buffer, checksum);
	util_SendMesg(fd, LSSPROTO_HL_SEND, buffer);
}

void lssproto_PR_send(int fd,int request,int result)
{
#ifdef _DEBUG_SEND_CLI
	printf("[发送]LSSPROTO_PR_SEND-request:%d,result:%d\n",request,result);
#endif
	char buffer[1024 * 64];
	int checksum=0;

	strcpy(buffer,"");

	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, _RUNNING_KEY);

	checksum += util_mkint(buffer, request);
	checksum += util_mkint(buffer, result);
	util_mkint(buffer, checksum);
	util_SendMesg(fd, LSSPROTO_PR_SEND, buffer);
}

#ifdef _PETS_SELECTCON
void lssproto_PETS_send(int fd,int petarray,int result)
{
#ifdef _DEBUG_SEND_CLI
	printf("[发送]LSSPROTO_PETST_SEND-petarray:%d,result:%d\n",petarray,result);
#endif
	char buffer[1024 * 64];
	int checksum=0;

	strcpy(buffer,"");

	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, _RUNNING_KEY);

	checksum += util_mkint(buffer, petarray);
	checksum += util_mkint(buffer, result);
	util_mkint(buffer, checksum);
	util_SendMesg(fd, LSSPROTO_PETST_SEND, buffer);
}
#endif

void lssproto_KS_send(int fd,int petarray,int result)
{
#ifdef _DEBUG_SEND_CLI
	printf("[发送]LSSPROTO_KS_SEND-petarray:%d,result:%d\n",petarray,result);
#endif
	char buffer[1024 * 64];
	int checksum=0;

	strcpy(buffer,"");

	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, _RUNNING_KEY);

	checksum += util_mkint(buffer, petarray);
	checksum += util_mkint(buffer, result);
	util_mkint(buffer, checksum);
	util_SendMesg(fd, LSSPROTO_KS_SEND, buffer);
}

void lssproto_SPET_send(int fd, int standbypet, int result)
{
#ifdef _DEBUG_SEND_CLI
	printf("[发送]LSSPROTO_SPET_SEND-standbypet:%d,result:%d\n",standbypet,result);
#endif
	char buffer[1024 * 64];
	int checksum=0;

	strcpy(buffer,"");

	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, _RUNNING_KEY);

	checksum += util_mkint(buffer, standbypet);
	checksum += util_mkint(buffer, result);
	util_mkint(buffer, checksum);
	util_SendMesg(fd, LSSPROTO_SPET_SEND, buffer);
}

void lssproto_PS_send(int fd,int result,int havepetindex,int havepetskill,int toindex)
{
#ifdef _DEBUG_SEND_CLI
	printf("[发送]LSSPROTO_PS_SEND-result:%d,havepetindex:%d,havepetskill:%d,toindex:%d\n",result,havepetindex,havepetskill,toindex);
#endif
	char buffer[1024 * 64];
	int checksum=0;

	strcpy(buffer,"");

	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, _RUNNING_KEY);

	checksum += util_mkint(buffer, result);
	checksum += util_mkint(buffer, havepetindex);
	checksum += util_mkint(buffer, havepetskill);
	checksum += util_mkint(buffer, toindex);
	util_mkint(buffer, checksum);
	util_SendMesg(fd, LSSPROTO_PS_SEND, buffer);
}

void lssproto_SKUP_send(int fd,int point)
{
#ifdef _DEBUG_SEND_CLI
	printf("[发送]LSSPROTO_SKUP_SEND-point:%d\n",point);
#endif
	char buffer[1024 * 64];
	int checksum=0;

	strcpy(buffer,"");

	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, _RUNNING_KEY);

	checksum += util_mkint(buffer, point);
	util_mkint(buffer, checksum);
	util_SendMesg(fd, LSSPROTO_SKUP_SEND, buffer);
}
void lssproto_WN_send(int fd,int windowtype,int buttontype,int seqno,int objindex,char* data)
{
#ifdef _DEBUG_SEND_CLI
	printf("[发送]LSSPROTO_WN_SEND-windowtype:%d,buttontype:%d,seqno:%d,objindex:%s\n",windowtype,buttontype,seqno,data);
#endif
	char buffer[1024 * 64];
	int checksum=0;

	strcpy(buffer,"");

	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, _RUNNING_KEY);

	checksum += util_mkint(buffer, windowtype);
	checksum += util_mkint(buffer, buttontype);
	checksum += util_mkint(buffer, seqno);
	checksum += util_mkint(buffer, objindex);
	checksum += util_mkstring(buffer, data);
	util_mkint(buffer, checksum);
	util_SendMesg(fd, LSSPROTO_WN_SEND, buffer);
	
}

void lssproto_EF_send(int fd,int effect,int level,char* option)
{
#ifdef _DEBUG_SEND_CLI
	printf("[发送]LSSPROTO_EF_SEND-effect:%d,level:%d,char:%s\n",effect,level,option);
#endif
	char buffer[1024 * 64];
	int checksum=0;

	strcpy(buffer,"");

	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, _RUNNING_KEY);

	checksum += util_mkint(buffer, effect);
	checksum += util_mkint(buffer, level);
	checksum += util_mkstring(buffer, option);
	util_mkint(buffer, checksum);
	util_SendMesg(fd, LSSPROTO_EF_SEND, buffer);
}
void lssproto_SE_send(int fd,int x,int y,int senumber,int sw)
{
#ifdef _DEBUG_SEND_CLI
	printf("[发送]LSSPROTO_SE_SEND-x:%d,y:%d,senumber:%d,sw:%d\n",x,y,senumber,sw);
#endif
	char buffer[1024 * 64];
	int checksum=0;

	strcpy(buffer,"");

	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, _RUNNING_KEY);

	checksum += util_mkint(buffer, x);
	checksum += util_mkint(buffer, y);
	checksum += util_mkint(buffer, senumber);
	checksum += util_mkint(buffer, sw);
	util_mkint(buffer, checksum);
	util_SendMesg(fd, LSSPROTO_SE_SEND, buffer);
}

void lssproto_ClientLogin_send(int fd,char* result)
{
#ifdef _DEBUG_SEND_CLI
	printf("[发送]LSSPROTO_CLIENTLOGIN_SEND-result:%s\n",result);
#endif
	char buffer[1024 * 64];
	int checksum=0;

	strcpy(buffer,"");

	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, _RUNNING_KEY);
	checksum += util_mkstring(buffer, result);
	util_mkint(buffer, checksum);
	
	util_SendMesg(fd, LSSPROTO_CLIENTLOGIN_SEND, buffer);
	
}

void lssproto_CreateNewChar_send(int fd,char* result,char* data)
{
#ifdef _DEBUG_SEND_CLI
	printf("[发送]LSSPROTO_CREATENEWCHAR_SEND-result:%s,data:%s\n",result,data);
#endif
	char buffer[1024 * 64];
	int checksum=0;

	strcpy(buffer,"");

	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, _RUNNING_KEY);

	checksum += util_mkstring(buffer, result);
	checksum += util_mkstring(buffer, data);
	util_mkint(buffer, checksum);
	util_SendMesg(fd, LSSPROTO_CREATENEWCHAR_SEND, buffer);
}

void lssproto_CharDelete_send(int fd,char* result,char* data)
{
#ifdef _DEBUG_SEND_CLI
	printf("[发送]LSSPROTO_CHARDELETE_SEND-result:%s,data:%s\n",result,data);
#endif
	char buffer[1024 * 64];
	int checksum=0;

	strcpy(buffer,"");

	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, _RUNNING_KEY);

	checksum += util_mkstring(buffer, result);
	checksum += util_mkstring(buffer, data);
	util_mkint(buffer, checksum);
	util_SendMesg(fd, LSSPROTO_CHARDELETE_SEND, buffer);
}

void lssproto_CharLogin_send(int fd,char* result,char* data)
{
#ifdef _DEBUG_SEND_CLI
	printf("[发送]LSSPROTO_CHARLOGIN_SEND-result:%s,data:%s\n",result,data);
#endif
	char buffer[1024 * 64];
	int checksum=0;

	strcpy(buffer,"");

	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, _RUNNING_KEY);

	checksum += util_mkstring(buffer, result);
	checksum += util_mkstring(buffer, data);
	util_mkint(buffer, checksum);
	util_SendMesg(fd, LSSPROTO_CHARLOGIN_SEND, buffer);
}

void lssproto_CharList_send(int fd,char* result,char* data)
{
#ifdef _DEBUG_SEND_CLI
	printf("[发送]LSSPROTO_CHARLIST_SEND-result:%s,data:%s\n",result,data);
#endif
	char buffer[1024 * 64];
	int checksum=0;

	strcpy(buffer,"");

	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, _RUNNING_KEY);

	checksum += util_mkstring(buffer, result);
	checksum += util_mkstring(buffer, data);
	util_mkint(buffer, checksum);
	util_SendMesg(fd, LSSPROTO_CHARLIST_SEND, buffer);
}

void lssproto_CharLogout_send(int fd,char* result,char* data)
{
#ifdef _DEBUG_SEND_CLI
	printf("[发送]LSSPROTO_CHARLOGOUT_SEND-result:%s,data:%s\n",result,data);
#endif
	char buffer[1024 * 64];
	int checksum=0;

	strcpy(buffer,"");

	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, _RUNNING_KEY);

	checksum += util_mkstring(buffer, result);
	checksum += util_mkstring(buffer, data);
	util_mkint(buffer, checksum);
	util_SendMesg(fd, LSSPROTO_CHARLOGOUT_SEND, buffer);
}

void lssproto_ProcGet_send(int fd,char* data)
{
#ifdef _DEBUG_SEND_CLI
	printf("[发送]LSSPROTO_PROCGET_SEND-data:%s\n",data);
#endif
	char buffer[1024 * 64];
	int checksum=0;

	strcpy(buffer,"");
	
	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, _RUNNING_KEY);

	checksum += util_mkstring(buffer, data);
	
	util_mkint(buffer, checksum);
	util_SendMesg(fd, LSSPROTO_PROCGET_SEND, buffer);
}

void lssproto_PlayerNumGet_send(int fd,int logincount,int player)
{
#ifdef _DEBUG_SEND_CLI
	printf("[发送]LSSPROTO_PLAYERNUMGET_SEND-logincount:%d,player:%d\n",logincount,player);
#endif
	char buffer[1024 * 64];
	int checksum=0;

	strcpy(buffer,"");

	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, _RUNNING_KEY);

	checksum += util_mkint(buffer, logincount);
	checksum += util_mkint(buffer, player);
	util_mkint(buffer, checksum);
	util_SendMesg(fd, LSSPROTO_PLAYERNUMGET_SEND, buffer);
}

void lssproto_Echo_send(int fd,char* test)
{
#ifdef _DEBUG_SEND_CLI
	printf("[发送]LSSPROTO_ECHO_SEND-test:%s\n",test);
#endif
	char buffer[1024 * 64];
	int checksum=0;

	strcpy(buffer,"");

	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, _RUNNING_KEY);

	checksum += util_mkstring(buffer, test);
	util_mkint(buffer, checksum);
	util_SendMesg(fd, LSSPROTO_ECHO_SEND, buffer);
}

// CoolFish: Trade 2001/4/18

void lssproto_TD_send(int fd, int index, char* message)
{
#ifdef _DEBUG_SEND_CLI
	printf("[发送]LSSPROTO_TD_SEND-message:%s\n",message);
#endif
	char buffer[1024 * 64];
	int checksum=0;

	strcpy(buffer,"");

	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, _RUNNING_KEY);

	checksum += util_mkstring(buffer, message);
	util_mkint(buffer, checksum);
	util_SendMesg(fd, LSSPROTO_TD_SEND, buffer);
}

void lssproto_NU_send(int fd, int nu)
{
#ifdef _DEBUG_SEND_CLI
	printf("[发送]LSSPROTO_NU_SEND-nu:%d\n",nu);
#endif
	char buffer[1024 * 64];
	int checksum=0;

	strcpy(buffer,"");

	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, _RUNNING_KEY);

	checksum += util_mkint(buffer, nu);
	util_mkint(buffer, checksum);
	util_SendMesg(fd, LSSPROTO_NU_SEND, buffer);
}


void lssproto_FM_send(int fd, char* message)
{
#ifdef _DEBUG_SEND_CLI
	printf("[发送]LSSPROTO_FM_SEND-message:%s\n",message);
#endif
	char buffer[1024 * 64];
	int checksum=0;

	strcpy(buffer,"");

	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, _RUNNING_KEY);

	checksum += util_mkstring(buffer, message);
	util_mkint(buffer, checksum);
	util_SendMesg(fd, LSSPROTO_FM_SEND, buffer);
}


void lssproto_WO_send(int fd,int effect)
{
#ifdef _DEBUG_SEND_CLI
	printf("[发送]LSSPROTO_WO_SEND-effect:%d\n",effect);
#endif
	char buffer[1024 * 64];
	int checksum=0;

	strcpy(buffer,"");

	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, _RUNNING_KEY);

	checksum += util_mkint(buffer, effect);
	util_mkint(buffer, checksum);
	util_SendMesg(fd, LSSPROTO_WO_SEND, buffer);
}
#ifdef _ITEM_CRACKER
void lssproto_IC_send(int fd, int x, int y)
{
#ifdef _DEBUG_SEND_CLI
	printf("[发送]lssproto_IC_send-x:%d,y:%d\n",x,y);
#endif
	char buffer[1024 * 64];
	int checksum=0;

	strcpy(buffer,"");

	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, _RUNNING_KEY);

	checksum += util_mkint(buffer, x);
	checksum += util_mkint(buffer, y);
	util_mkint(buffer, checksum);
	util_SendMesg(fd, LSSPROTO_IC_SEND, buffer);
}
#endif
#ifdef _MAGIC_NOCAST             // 精灵:沉默
void lssproto_NC_send(int fd,int flg)
{
#ifdef _DEBUG_SEND_CLI
	printf("[发送]LSSPROTO_NC_SEND-flg:%d\n",flg);
#endif
	char buffer[1024 * 64];
	int checksum=0;

	strcpy(buffer,"");

	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, _RUNNING_KEY);

	checksum += util_mkint(buffer, flg);
	util_mkint(buffer, checksum);
	util_SendMesg(fd, LSSPROTO_NC_SEND, buffer);
}
#endif

