#include<stdio.h>

#define arraysizeof( x ) (sizeof(x)/sizeof(x[0]))


// 共通
#define WIN_WARP_X  400
#define WIN_WARP_Y  (128)
#define ROOM_SIZE_X	44
#define ROOM_SIZE_Y	12



int FloorIdTbl[] = {
	1021,
	3021
};

char *CrFileName[] = {
	"samdou",
	"jyajyadou"
};

typedef struct {
	int first;
	int last;
}ENEMYLIST;

typedef struct {
	int floor;
	int x;
	int y;
}POSITION;

ENEMYLIST EnemyTbl[] = {
	{ 655, 690 },
	{ 687, 720 }
};

POSITION SishouTbl[] = {
	{ 1008, 20, 20  },
	{ 3008, 20, 20  },
};

POSITION UketsukeTbl[] = {
	{ 1022, 1, 7  },
	{ 3022, 1, 7  },
};


int main( int argc, char **argv ){
	int i, j, first, last, xcnt = 0, ycnt = 0;
	int npccnt = 1;	// ＮＰＣの数
	int win_warpX = WIN_WARP_X, win_warpY = WIN_WARP_Y;
	int warpX = WIN_WARP_X+6, warpY = WIN_WARP_Y;
	int npcX = WIN_WARP_X, npcY = WIN_WARP_Y-4;
	int floor_id, pos;

	FILE *fp;
	char szBuffer[256];

	if( argc <= 1 ){
		printf( "0 == サムギル 1 == カルターナ\n" );
		exit( 0 );
	}
	if( sscanf( argv[1], "%d", &pos ) == 1 ){
		if( pos == 0 ){
			printf( "サムギル道場作成\n" );
		}else
		if( pos == 1 ){
			printf( "ジャジャ道場作成\n" );
		}else{
			printf( "(%s)おかしー\n", argv[1] );
			exit( 0 );
		}
	}

	// どっちのフロアか
	floor_id = FloorIdTbl[pos];

	for( xcnt = 0; xcnt < 10; xcnt ++ ){
		win_warpY = WIN_WARP_Y-2;
		npcY = WIN_WARP_Y-4;
		warpY = WIN_WARP_Y-2;
		for( ycnt = 0; ycnt < 10; ycnt ++ ){
			sprintf( szBuffer, "%s%03d.arg",  CrFileName[pos], npccnt );
			fp = fopen( szBuffer, "w" );
			if( fp == NULL )exit( 1 );
			fprintf( fp, "entype:2\n" );
			fprintf( fp, "dieact:1\n" );

			// １００人目はここ
			if( npccnt == 100 ){
				fprintf( fp, "warpfl:%d\n", SishouTbl[pos].floor );
				fprintf( fp, "warpx:%d\n", SishouTbl[pos].x );
				fprintf( fp, "warpy:%d\n", SishouTbl[pos].y );
			}else
			if( ycnt == 9 ){	// 折り返しのためこのワープ
				fprintf( fp, "warpfl:%d\n", floor_id );
				fprintf( fp, "warpx:%d\n", win_warpX-(xcnt+1)*ROOM_SIZE_X );
				fprintf( fp, "warpy:%d\n", WIN_WARP_Y );
			}else{	// 通常はこのワープ
				fprintf( fp, "warpfl:%d\n", floor_id );
				fprintf( fp, "warpx:%d\n", win_warpX-xcnt*ROOM_SIZE_X );
				fprintf( fp, "warpy:%d\n", win_warpY-(ycnt+1)*ROOM_SIZE_Y );
			}
			fprintf( fp, "gym:%d\n", npccnt );
			fprintf( fp, "startmsg:私は%d人目だ\n", npccnt );
			fprintf( fp, "enemyno:" );
			first = 564; last = 580;
			for( j = first; j <= last; j ++ ){
				fprintf( fp, "%d", j );
				if( j < last )fprintf( fp, "," );
			}
			fprintf( fp, "\n" );

			fprintf( fp, "enemypetno:" );
			// サムギルの場合
			first = EnemyTbl[pos].first; last =  EnemyTbl[pos].last;

			for( j = first; j <= last; j ++ ){
				fprintf( fp, "%d", j );
				if( j < last )fprintf( fp, "," );
			}
			fprintf( fp, "\n" );
			fclose( fp );
			npccnt ++;
		}
	}

	sprintf( szBuffer, "%s.create", CrFileName[pos] );

	fp = fopen( szBuffer, "w" );
	fprintf( fp, "NPCCREATE\n" );
	fprintf( fp, "#########   道場１００人組み手   ########\n" );

	npccnt = 1;
	for( xcnt = 0; xcnt < 10; xcnt ++ ){
		for( ycnt = 0; ycnt < 10; ycnt ++ ){
			// NPC の設定
			fprintf( fp, "{\n" );
			fprintf( fp, "floorid=%d\n", floor_id );
			fprintf( fp, "borncenter=%d,%d,1,1\n", npcX-ROOM_SIZE_X*xcnt, npcY-ROOM_SIZE_Y*ycnt );
			fprintf( fp, "movecenter=20,20,2,2\n" );
			fprintf( fp, "createnum=1\n" );
			fprintf( fp, "dir=4\n" );
			fprintf( fp, "time=60000\n" );
			fprintf( fp, "graphicname=100091\n" );
			fprintf( fp, "name=門下生%d人目\n", npccnt );
			fprintf( fp, "enemy=sb_dou|file:doujyou/%s%03d.arg\n",
				CrFileName[pos], npccnt );
			fprintf( fp, "}\n" );

			// ワープゾーンの設定
			fprintf( fp, "{\n" );
			fprintf( fp, "floorid=%d\n", floor_id );
			fprintf( fp, "borncenter=%d,%d,1,1\n", warpX-ROOM_SIZE_X*xcnt, warpY-ROOM_SIZE_Y*ycnt );
			fprintf( fp, "movecenter=20,20,2,2\n" );
			fprintf( fp, "createnum=1\n" );
			fprintf( fp, "time=60000\n" );
			fprintf( fp, "graphicname=0\n" );
			fprintf( fp, "name=ワープ\n", npccnt );
			fprintf( fp, "enemy=npcgen_warp|%d|%d|%d\n",
				UketsukeTbl[pos].floor, UketsukeTbl[pos].x, UketsukeTbl[pos].y );
			fprintf( fp, "}\n" );

			// ワープゾーンの設定２個目
			fprintf( fp, "{\n" );
			fprintf( fp, "floorid=%d\n", floor_id );
			fprintf( fp, "borncenter=%d,%d,1,1\n", warpX-ROOM_SIZE_X*xcnt, warpY-ROOM_SIZE_Y*ycnt+1 );
			fprintf( fp, "movecenter=20,20,2,2\n" );
			fprintf( fp, "createnum=1\n" );
			fprintf( fp, "time=60000\n" );
			fprintf( fp, "graphicname=0\n" );
			fprintf( fp, "name=ワープ\n", npccnt );
			fprintf( fp, "enemy=npcgen_warp|%d|%d|%d\n",
				UketsukeTbl[pos].floor, UketsukeTbl[pos].x, UketsukeTbl[pos].y );
			fprintf( fp, "}\n" );
			npccnt ++;
		}
	}

	return 0;

}

