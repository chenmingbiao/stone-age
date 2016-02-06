#include "version.h"
#include <stdio.h>
#include <ctype.h>
#include <time.h>
//???
#include "common.h"
#include "handletime.h"
#include "object.h"
#include "char_base.h"
#include "char.h"
#include "char_event.h"
#include "lssproto_serv.h"
#include "readmap.h"
#include "map_deal.h"
#include "npccreate.h"
#include "encount.h"
#include "npcutil.h"
#include "battle.h"
#include "net.h"
#include "configfile.h"
#include "npc_npcenemy.h"

static void CHAR_sendCharaAtWalk( int charaindex, int of, int ox, int oy, int xflg, int yflg );
static void CHAR_sendCDCharaAtWalk( int charaindex, int of, int ox, int oy,	int xflg, int yflg );

static CHAR_WALKRET CHAR_walk_turn( int index, int dir )
{
    int     fx,fy,ff;
    OBJECT  object;

    if(!CHAR_CHECKINDEX( index ))return CHAR_WALKSYSTEMERROR;
    CHAR_setInt(index,CHAR_DIR,dir);
    fx = CHAR_getInt(index,CHAR_X);
    fy = CHAR_getInt(index,CHAR_Y);
    ff = CHAR_getInt(index,CHAR_FLOOR);
    CHAR_sendWatchEvent( CHAR_getWorkInt(index, CHAR_WORKOBJINDEX),
                         CHAR_ACTTURN,NULL,0,FALSE);
	CHAR_setWorkInt( index, CHAR_WORKACTION, CHAR_ACTTURN);
    for( object = MAP_getTopObj(ff,fx,fy) ; object ;
         object = NEXT_OBJECT(object ) ){
        typedef void (*POSTOFUNC)(int,int);
        POSTOFUNC   pfunc=NULL;
        int objindex = GET_OBJINDEX(object);

        switch( OBJECT_getType(objindex)  ){
        case OBJTYPE_CHARA:
            pfunc = (POSTOFUNC)CHAR_getFunctionPointer(
                OBJECT_getIndex(objindex),
                CHAR_POSTOVERFUNC);
            break;
        case OBJTYPE_ITEM:
            pfunc = (POSTOFUNC)ITEM_getFunctionPointer(
                OBJECT_getIndex(objindex),
                ITEM_POSTOVERFUNC);
            break;
        case OBJTYPE_GOLD:
            break;
        default:
            break;
        }
        if( pfunc )pfunc( OBJECT_getIndex(objindex),index );
    }
    return CHAR_WALKSUCCESSED;
}


/*------------------------------------------------------------
 * n ÐÚ ÐÚÖÐÐ×mapÃ«ËªÔÂ
 * Â¦ÐÑ
 *  index       int     Æ½ÅÒ·Â¼°ÄÌ¼þ·¸ÓÀÛÍµ©
 *  fl          int     °×·òÊ§ID
 *  ox          int     ó¡¼°ÇÁÚØ¼°x
 *  oy          int     ó¡¼°ÇÁÚØ¼°y
 *  fx          int     ÐÚÖÐÐ×  ¼°x
 *  fy          int     ÐÚÖÐÐ×  ¼°y
 * ß¯Ô»°À
 *  Ø¦ØÆ
 ------------------------------------------------------------*/
static void CHAR_sendMapAtWalk( int index, int fl ,int ox ,int oy,
								int fx, int fy )
{
    /*
     *  òØ»þ¼°°ÀÎç£ýó¡ÒýÆ¥¼°°À¼°êûÃ«Î­ÔÂ
     *  ³ðµ¤ÔÊÔÂÎç£ý·¥¡õÃóNPCÊÖ¿ÃÇ©·Ö£Û
     */
    int     vx,vy;
    const int     seesiz = MAP_CHAR_DEFAULTSEESIZ;

    if( !CHAR_CHECKINDEX( index ) )return;

    vx = fx - ox;
    vy = fy - oy;

    if( ABS(vx) >= seesiz/2 ||       /*  êû»¥  ÎåÖÐ  */
        ABS(vy) >= seesiz/2 ){       /*  êû»¥  ÎåÖÐ  */
        return;

    }else{
        RECT    send,get;
        char*   mapdata;
        int oldlux = ox - (int)(seesiz/2);
        int oldluy = oy - (int)(seesiz/2);
        int absx = ABS(vx);
        int absy = ABS(vy);
        /*
         * ·òÎç¹´Äô»§±åµ©Õý¡õÐþÃ«äúÔÂ¼°Îç£ývx , vy ¾®ÈÕ1·òÈÊ¼°»¥ºÌÄÌ¼þÐþ
         * ó¡Îç¾®½ñØ¦ÈÕØ¦ÖÐ·½µ¤±å£ý
         * clipRect ·´ w,h 0 Ã«ÐÄÎç»§»¯ÖÐÔÂ
         */

        /*  == 0 ¼°ÁÝ·´ËªÈÕØ¦ÈÊ»¯·½ÖÐ   */
        if( vx != 0 ){
            if( vx > 0 ){
                send.x     = oldlux + seesiz;
                send.width = absx;
            }else if( vx < 0 ){
                send.x     = oldlux - absx;
                send.width = absx;
            }

            if( vy >= 0 ){
                send.y      = oldluy;
                send.height = seesiz + absy;
            }else{
                send.y      = oldluy - absy;
                send.height = seesiz + absy;
            }
{
            int     tilesum, objsum,eventsum;
            mapdata = MAP_getChecksumFromRECT(fl,&send,&get,&tilesum, &objsum,&eventsum );
            if( mapdata != NULL
                && !( get.width == 0 && get.height == 0 ) ){
                int fd = getfdFromCharaIndex( index );
                if( fd !=  -1 )
                    lssproto_MC_send( fd,fl,get.x,get.y,
                                     get.x+get.width,
                                     get.y+get.height,
                                     tilesum,
                                     objsum,
                                     eventsum,
                                     mapdata );
            }
}
        }

        /*  == 0 ¼°ÁÝ·´ËªÈÕØ¦ÈÊ»¯·½ÖÐ   */
        if( vy != 0 ){
            if( vy > 0 ){
                send.y      = oldluy + seesiz;
                send.height = absy;
            }else{
                send.y      = oldluy - absy;
                send.height = absy;
            }
            if( vx >= 0 ){
                send.x     = oldlux;
                send.width = seesiz + absx;
            }else if( vx < 0 ){
                send.x     = oldlux - absx;
                send.width = seesiz + absx;
            }
{
            int     tilesum, objsum,eventsum;
            mapdata = MAP_getChecksumFromRECT(fl,&send,&get, &tilesum, &objsum,&eventsum);
            if( mapdata != NULL
                && !( get.width == 0 && get.height == 0 ) ){
                int fd = getfdFromCharaIndex( index );
                if( fd !=  -1 )
                    lssproto_MC_send( fd,fl,get.x,get.y,
                                     get.x+get.width,
                                     get.y+get.height,
                                     tilesum,
                                     objsum,
                                     eventsum,
                                     mapdata );

            }
}
        }
    }

}
static CHAR_WALKRET CHAR_walk_move( int charaindex, int dir );
/*------------------------------------------------------------
 * à¢  ÔÊÔÂ£Û
 * Â¦ÐÑ
 *  index       int     Æ½ÅÒ·Â¼°ÄÌ¼þ·¸ÓÀÛÍµ©
 *  dir         int       éù
 * ß¯Ô»°À
 *  CHAR_WALKRET
 ------------------------------------------------------------*/
static CHAR_WALKRET CHAR_walk_move( int charaindex, int dir )
{
    int     i;
    int     fx,fy,ff;
    int     ox,oy,of;
    int     objbuf[128];
    int     objbufindex=0;
    int     notover=FALSE;
    int     retvalue=CHAR_WALKSUCCESSED;

    if( !CHAR_CHECKINDEX( charaindex )) return CHAR_WALKSYSTEMERROR;
    ox = CHAR_getInt(charaindex,CHAR_X);
    oy = CHAR_getInt(charaindex,CHAR_Y);
    of = CHAR_getInt(charaindex,CHAR_FLOOR);
    CHAR_getCoordinationDir( dir,ox,oy,1,&fx,&fy);
    ff = of;

    if( CHAR_getInt( charaindex,CHAR_WHICHTYPE ) == CHAR_TYPEENEMY ){
        RECT    walkr;
        POINT   nextp;
        int npccreateindex;
        npccreateindex = CHAR_getInt( charaindex,CHAR_NPCCREATEINDEX );
        if( CHAR_isInvincibleArea( ff,fx,fy)) {
            CHAR_setInt(charaindex,CHAR_DIR,dir);
            retvalue = CHAR_WALKEXTEND;
            goto CHAR_AFTERWALK;
        }
        if( NPC_isBoundarySet( npccreateindex ) &&
            NPC_createGetRECT( npccreateindex,&walkr ) == TRUE )
        {
            nextp.x = fx;
            nextp.y = fy;
            if( PointInRect( &walkr,&nextp ) == FALSE ){
                CHAR_setInt(charaindex,CHAR_DIR,dir);
                retvalue = CHAR_WALKEXTEND;
                goto CHAR_AFTERWALK;
            }
        }else{
            ;
        }
    }


    if( CHAR_getFlg( charaindex, CHAR_ISBIG ) == 1 ){
        static POINT offset[5]={
            { 0,-1},
            {-1, 0},
            { 0, 0},
            { 1, 0},
            { 0, 1},
        };
        int     k;
        for( k=0 ; k<5; k++ ){
            if( !MAP_walkAble( charaindex, ff, fx+offset[k].x,
                               fy+offset[k].y )){
                CHAR_setInt(charaindex,CHAR_DIR,dir);
                retvalue = CHAR_WALK1357;
                goto CHAR_AFTERWALK;
            }
        }

    }else{
        if( CHAR_getDX(dir)*CHAR_getDY(dir) == 0 ){
            if( !MAP_walkAble( charaindex,ff, fx, fy ) ){
                CHAR_setInt(charaindex,CHAR_DIR,dir);
                retvalue = CHAR_WALK1357;
                goto CHAR_AFTERWALK;
            }
        }else{
            int     xflg,yflg;
            if( !MAP_walkAble( charaindex,ff, fx, fy ) ){
                CHAR_setInt(charaindex,CHAR_DIR,dir);
                retvalue = CHAR_WALK1357;
                goto CHAR_AFTERWALK;
            }

            xflg = MAP_walkAble( charaindex,of, ox+CHAR_getDX(dir), oy );
            yflg = MAP_walkAble( charaindex,of, ox, oy+CHAR_getDY(dir) );

            if( !xflg || !yflg ) {
                CHAR_setInt(charaindex,CHAR_DIR,dir);
                retvalue = CHAR_WALK1357;
                goto CHAR_AFTERWALK;
            }
        }
    }
    objbufindex = CHAR_getSameCoordinateObjects( objbuf, arraysizeof( objbuf ), ff,fx,fy );
    for( i = 0 ; i < objbufindex ; i ++ ){
        int     objindex= objbuf[i];
        switch( OBJECT_getType(objindex) ){
        case OBJTYPE_CHARA:
            if( !CHAR_getFlg( OBJECT_getIndex(objindex),CHAR_ISOVERED) ){
            	if(!CHAR_CHECKINDEX(OBJECT_getIndex(objindex))){
	            	printf("×Ô¶¯É¾³ýÒ»¸öÎÊÌâ¶ÔÏó£¡");
	        			endObjectOne(objindex);
	        			break;
	        		}
              notover= TRUE;
            }
            break;
        case OBJTYPE_ITEM:
            if( !ITEM_getInt( OBJECT_getIndex(objindex), ITEM_ISOVERED ) )
                notover= TRUE;
            break;
        case OBJTYPE_GOLD:
            break;
        default:
            break;
        }
    }
    if( notover == TRUE ){
        CHAR_setInt(charaindex,CHAR_DIR,dir);
        retvalue = CHAR_WALKHITOBJECT;
    }else{
        for( i = 0 ; i < objbufindex ; i ++ ){
            typedef void (*PREOFUNC)(int,int);
            PREOFUNC   pfunc=NULL;
            int     objindex=objbuf[i];
            switch( OBJECT_getType(objindex) ){
            case OBJTYPE_CHARA:
                pfunc = (PREOFUNC)CHAR_getFunctionPointer(
                    OBJECT_getIndex(objindex),
                    CHAR_PREOVERFUNC);
                break;
            case OBJTYPE_ITEM:
                pfunc = (PREOFUNC)ITEM_getFunctionPointer(
                    OBJECT_getIndex(objindex),
                    ITEM_PREOVERFUNC);
                break;
            case OBJTYPE_GOLD:
                break;
            default:
                break;
            }
            if( pfunc )pfunc( OBJECT_getIndex(objindex),charaindex );
        }

        CHAR_setInt(charaindex,CHAR_X,fx);
        CHAR_setInt(charaindex,CHAR_Y,fy);
        CHAR_setInt(charaindex,CHAR_FLOOR,ff);
        CHAR_setInt(charaindex,CHAR_DIR,dir);

        {
            int objindex;
            int ox,oy,of;
            objindex = CHAR_getWorkInt( charaindex,CHAR_WORKOBJINDEX );
            of = OBJECT_setFloor(objindex,
                                 CHAR_getInt(charaindex,CHAR_FLOOR));
            ox = OBJECT_setX(objindex,CHAR_getInt(charaindex,CHAR_X));
            oy = OBJECT_setY(objindex,CHAR_getInt(charaindex,CHAR_Y));
            if( !MAP_objmove( objindex, of,ox,oy,ff,fx,fy ) ){
                /*  ³ðÊÏØ¦ÊÏÉýµ¤ØÆ·½µ¤ÊÖØ¦ÖÐ    */
                fprint( "ERROR MAP_OBJMOVE objindex=%d(%s)\n",objindex,
                	CHAR_getUseName( charaindex ) );
            }
        }

        CHAR_setInt(charaindex,CHAR_WALKCOUNT,
                    CHAR_getInt(charaindex,CHAR_WALKCOUNT) + 1 );


        for( i = 0 ; i < objbufindex ; i ++ ){
            typedef void (*POSTOFUNC)(int,int);
            POSTOFUNC   pfunc=NULL;
            int     objindex=objbuf[i];

            switch( OBJECT_getType(objindex) ){
            case OBJTYPE_CHARA:
                pfunc = (POSTOFUNC)CHAR_getFunctionPointer(
                    OBJECT_getIndex(objindex),
                    CHAR_POSTOVERFUNC);
                break;
            case OBJTYPE_ITEM:
                pfunc = (POSTOFUNC)ITEM_getFunctionPointer( OBJECT_getIndex(objindex), ITEM_POSTOVERFUNC);
                break;
            case OBJTYPE_GOLD:
                break;
            default:
                break;
            }
            if( pfunc )pfunc( OBJECT_getIndex(objindex),charaindex );
        }
        objbufindex = CHAR_getSameCoordinateObjects(objbuf, arraysizeof(objbuf), of, ox, oy );
        for( i=0 ;i<objbufindex;i++){
            typedef void (*OFFFUNC)(int,int);
            OFFFUNC   ofunc=NULL;
            int     objindex = objbuf[i];

            switch( OBJECT_getType( objindex ) ){
            case OBJTYPE_CHARA:
                ofunc = (OFFFUNC)CHAR_getFunctionPointer( OBJECT_getIndex(objindex), CHAR_OFFFUNC);
                break;
            case OBJTYPE_ITEM:
                /*    ±åÖÏÊÖØÆØ¦ÖÐ  */
                break;
            case OBJTYPE_GOLD:
                /*    ±åÖÏÊÖØÆØ¦ÖÐ  */
                break;
            default:
                break;
            }
            if( ofunc )ofunc( OBJECT_getIndex(objindex), charaindex );

        }
    }
CHAR_AFTERWALK:
    if( retvalue == CHAR_WALK1357 || retvalue == CHAR_WALKHITOBJECT ){
	    {
	        int     opt[2] = { ox, oy};
	        CHAR_sendWatchEvent( CHAR_getWorkInt(charaindex,
	                                             CHAR_WORKOBJINDEX),
	                             CHAR_ACTWALK,opt,2,TRUE );
	    }
        CHAR_setWorkChar( charaindex, CHAR_WORKWALKARRAY, "");
		if( CHAR_getInt( charaindex, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER) {
		    CHAR_sendWatchEvent( CHAR_getWorkInt(charaindex, CHAR_WORKOBJINDEX),
    		                     CHAR_ACTWARP,NULL,0,TRUE);
		}
    }else if( CHAR_getInt( charaindex, CHAR_WHICHTYPE ) == CHAR_TYPEPLAYER ){

		BOOL	flg = FALSE;
		int		par;
		int		count;
		CHAR_setWorkInt( charaindex, CHAR_WORKACTION, -1 );
		if( CHAR_getWorkInt( charaindex, CHAR_WORKBATTLEMODE ) == BATTLE_CHARMODE_NONE ){
			CHAR_sendCharaAtWalk( charaindex, of,ox,oy,CHAR_getDX(dir), CHAR_getDY(dir));
		}

	    {
	        int     opt[2] = { ox, oy};
	        CHAR_sendWatchEvent( CHAR_getWorkInt(charaindex,
	                                             CHAR_WORKOBJINDEX),
	                             CHAR_ACTWALK,opt,2,TRUE );
	    }
		if( CHAR_getWorkInt( charaindex, CHAR_WORKPARTYMODE) == CHAR_PARTY_CLIENT ) {
			CHAR_sendMapAtWalk( charaindex, of,
								ox,oy,
								CHAR_getInt( charaindex, CHAR_X),
								CHAR_getInt( charaindex, CHAR_Y));
		}
		count = CHAR_getWorkInt( charaindex, CHAR_WORK_TOHELOS_COUNT);
		if( count > 0 ) {
			CHAR_setWorkInt( charaindex, CHAR_WORK_TOHELOS_COUNT, count -1);
			if( count -1 == 0 ) {
				CHAR_talkToCli( charaindex, -1, "µÀ¾ßµÄÐ§Á¦ÒÑµ½¡£", CHAR_COLORWHITE);
			}
		}
		par = ENCOUNT_getEncountPercentMin( charaindex, of,ox,oy);
		if( par != -1 ) {
			if( CHAR_getWorkInt( charaindex, CHAR_WORKENCOUNTPROBABILITY_MIN) != par ){
				flg = TRUE;
				CHAR_setWorkInt( charaindex, CHAR_WORKENCOUNTPROBABILITY_MIN, par);
			}
		}

		par = ENCOUNT_getEncountPercentMax( charaindex, of,ox,oy);
		if( par != -1 ) {
			if( CHAR_getWorkInt( charaindex, CHAR_WORKENCOUNTPROBABILITY_MAX) != par ){
				flg = TRUE;
				CHAR_setWorkInt( charaindex, CHAR_WORKENCOUNTPROBABILITY_MAX, par);
			}
		}
		// Arminius 7.12 login announce
		/*
		{
		  int enfd = getfdFromCharaIndex( charaindex );
		  if (CONNECT_get_announced(enfd)==0) {
		    // Robin 0720
		    //AnnounceToPlayer(charaindex);
		    AnnounceToPlayerWN( enfd );
		    CONNECT_set_announced(enfd,1);
		  }
		}
		*/
		// Arminius 6.22 check Encounter
		// Nuke 0622: Provide No Enemy function
		{
		  int enfd = getfdFromCharaIndex( charaindex );
		  int eqen = getEqNoenemy( enfd );	// Arminius 7.2: Ra's amulet
		  int noen = getNoenemy(enfd);

          // Arminius 7.31 cursed stone
      if (getStayEncount(enfd)>0) {
			  clearStayEncount(enfd);
		  }
		  //print("\n enfd=%d,eqen=%d,noen=%d", enfd, eqen, noen);

		  // Arminius 7.2 Ra's amulet
		  if (eqen>=200) {
		    noen=1;
		  } else if (eqen>=120) {
		    if ((ff==100)||(ff==200)||(ff==300)||(ff==400)||(ff==500)) noen=1;
		  } else if (eqen>=80) {
		    if ((ff==100)||(ff==200)||(ff==300)||(ff==400)) noen=1;
		  } else if (eqen>=40) {
		    if ((ff==100)||(ff==200)) noen=1;
		  }


		  //print("\n noen=%d", noen);
		  if (noen==0) {
				int maxep = CHAR_getWorkInt(charaindex, CHAR_WORKENCOUNTPROBABILITY_MAX);
		    int minep = CHAR_getWorkInt(charaindex, CHAR_WORKENCOUNTPROBABILITY_MIN);
		    int cep = CONNECT_get_CEP(enfd);

		    if (cep<minep) cep=minep;
		    if (cep>maxep) cep=maxep;
		    if (CHAR_getWorkInt(charaindex,CHAR_WORKBATTLEMODE)==BATTLE_CHARMODE_NONE) {
				int entflag=1;
				{
					int objindex,index;
					OBJECT obj;
					for ( obj = MAP_getTopObj( ff, fx, fy); obj; obj = NEXT_OBJECT( obj)) {
						objindex = GET_OBJINDEX( obj);
						if (OBJECT_getType( objindex) == OBJTYPE_CHARA) {
							int etype;
							index = OBJECT_getIndex( objindex);
							if (!CHAR_CHECKINDEX( index)) continue;
							if( CHAR_getInt( index, CHAR_WHICHTYPE) == CHAR_TYPENPCENEMY &&
								CHAR_getWorkInt( index, CHAR_WORKEVENTTYPE) == CHAR_EVENT_ENEMY )	{
								CHAR_setInt( charaindex, CHAR_X, ox);
								CHAR_setInt( charaindex, CHAR_Y, oy);
								lssproto_XYD_send( getfdFromCharaIndex(charaindex),
								CHAR_getInt( charaindex, CHAR_X ),
								CHAR_getInt( charaindex, CHAR_Y ),
								CHAR_getInt( charaindex, CHAR_DIR ) );
								break;
							}
							etype = CHAR_getWorkInt( index, CHAR_WORKEVENTTYPE);
							if (etype!=CHAR_EVENT_NONE) {
								if (etype==CHAR_EVENT_WARP) entflag=0;
							}
						}
					}
				}
#ifdef _ENEMY_ACTION
				if( rand()%(120*getEnemyAction()) < cep ){
#else
		      if (rand()%120<cep){	// Arminius 6.28 lower encounter prob.
#endif
					if (entflag) {
#ifdef _Item_MoonAct
						if( getEqRandenemy( enfd) > 0 )	{
							int Rnum=0;
							int RandEnemy = getEqRandenemy( enfd);
							Rnum = RAND(0,100);
							if( Rnum > RandEnemy )	{
#endif
								//encounter!!
								cep = minep;
								lssproto_EN_recv(enfd,
								CHAR_getInt(charaindex,CHAR_X),
								CHAR_getInt(charaindex,CHAR_Y));
#ifdef _Item_MoonAct
							}
						}else{
								cep = minep;
								lssproto_EN_recv(enfd,
								CHAR_getInt(charaindex,CHAR_X),
								CHAR_getInt(charaindex,CHAR_Y));
						}
#endif
					}
		      } else {
		        if (cep<maxep) cep++;
		      }
		    }
		    CONNECT_set_CEP(enfd, cep);
		  }
		}
		if( CHAR_getWorkInt( charaindex, CHAR_WORKPARTYMODE )!= CHAR_PARTY_CLIENT ){
			CHAR_setFlg( charaindex, CHAR_ISWARP, 0);
		}
    }
    else if( CHAR_getInt( charaindex, CHAR_WHICHTYPE ) == CHAR_TYPEPET ||
    		CHAR_getInt( charaindex, CHAR_WHICHTYPE) == CHAR_TYPEBUS	
#ifdef _GAMBLE_ROULETTE	
			|| CHAR_getInt( charaindex, CHAR_WHICHTYPE) == CHAR_GAMBLEROULETTE 
#endif
#ifdef _PETRACE
			|| CHAR_getInt(charaindex, CHAR_WHICHTYPE) == CHAR_PETRACEPET
#endif
	)	{
		CHAR_setWorkInt( charaindex, CHAR_WORKACTION, -1 );
		if( CHAR_getWorkInt( charaindex, CHAR_WORKBATTLEMODE ) == BATTLE_CHARMODE_NONE ){
			CHAR_sendCharaAtWalk( charaindex, of,ox,oy,CHAR_getDX(dir), CHAR_getDY(dir));
		}
	    {
	        int     opt[2] = { ox, oy};
	        CHAR_sendWatchEvent( CHAR_getWorkInt(charaindex,
	                                             CHAR_WORKOBJINDEX),
	                             CHAR_ACTWALK,opt,2,TRUE );
	    }
    }
    if( retvalue == CHAR_WALKSUCCESSED ) {
		if( CHAR_getWorkInt( charaindex, CHAR_WORKBATTLEMODE )	== BATTLE_CHARMODE_NONE ){
			CHAR_sendCDCharaAtWalk( charaindex, of,ox,oy,
									CHAR_getDX(dir), CHAR_getDY(dir));
		}
	}
    return retvalue;
}

/*------------------------------------------------------------
 * Æ½ÅÒ·Â»¥ÐÚÈÊ£Û
 * Â¦ÐÑ
 *  index   int     CharÆ¥¼°index
 *  dir     int       éù
 *  mode    int     0Ø¦ÈÕÐÚÈÊ£Û1Ø¦ÈÕ  éù±î¾§£Û
 * ß¯Ô»°À
 *  CHAR_WALKRET
 ------------------------------------------------------------*/
CHAR_WALKRET CHAR_walk( int index, int dir, int mode)
{
    CHAR_WALKRET ret;
    BOOL (*prewalk)(int , int* , int* );
    void (*postwalk)(int);
    if( !CHAR_CHECKINDEX( index ) )return CHAR_WALKSYSTEMERROR;

    if( CHAR_getFlg(index,CHAR_ISDIE) ) return CHAR_WALKDIE;

    VALIDATEDIR(dir);
    prewalk = (BOOL(*)(int,int*,int*))
        CHAR_getFunctionPointer(index, CHAR_WALKPREFUNC);

    if( prewalk != NULL )
        if( prewalk(index,&dir,&mode) == FALSE )	{
            return CHAR_WALKPREWALK;
		}

    if( mode == 0 )
        ret = CHAR_walk_move( index, dir );
    else
        ret = CHAR_walk_turn( index, dir );
    postwalk = (void(*)(int))
        CHAR_getFunctionPointer(index,CHAR_WALKPOSTFUNC);

    if( postwalk != NULL )
        postwalk(index);

    return ret;
}


/*------------------------------------------------------------
 * ÐÚ¾®ÁùÔÂó¡ÉÍ  £Û
 * Â¦ÐÑ
 *  index       int         Æ½ÅÒ·ÂÄÌ¼þ·¸ÓÀÛÍµ©
 * ß¯Ô»°À
 *  Ø¦ØÆ
 ------------------------------------------------------------*/
void CHAR_walkcall( int index )
{
    char    tmp[128];
    int     dir,mode;
	int		ret;
	POINT	start,end;

    if( !CHAR_CHECKINDEX( index ) ) return;

    strcpysafe( tmp ,sizeof(tmp),
                CHAR_getWorkChar(index,CHAR_WORKWALKARRAY));

    CHAR_ctodirmode( tmp[0], &dir ,&mode );
	/* ÍÊØ¦ÈÕÒÇó¡±åèëô÷Ã«·¤Ê¢ØÆ»¯ÔÆÈÊ
	 * ÍÊ¼°  ÈÊó¡¼°èëô÷Ã«    ±åºÒ»¥à¢  ÔÊÔÂ¿Ð
	 */
	if( CHAR_getInt( index, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER ) {
		if( CHAR_getWorkInt( index, CHAR_WORKPARTYMODE ) == CHAR_PARTY_LEADER ) {
			end.x = CHAR_getInt( index, CHAR_X);
			end.y = CHAR_getInt( index, CHAR_Y);
		}
	}

	/* ÐÚÈÊ */
    ret = CHAR_walk( index , dir ,mode );

	if( mode == 0 && ret == CHAR_WALKSUCCESSED ) {
		/* ·ßÛÐ»¥ÍÊØ¦ÈÕõ´ÃÞÃ«ÐÚ¾®ÁùÔÂ */
		if( CHAR_getInt( index, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER ) {
		

			if( CHAR_getWorkInt( index, CHAR_WORKPARTYMODE) == CHAR_PARTY_LEADER ) {
				int	i;
				for( i = 1; i < CHAR_PARTYMAX; i ++ ) {
					int toindex = CHAR_getWorkInt( index, i + CHAR_WORKPARTYINDEX1);
					if( CHAR_CHECKINDEX(toindex) ) {
						int		parent_dir;
						/* ºÒ¼°ÞË  Îç£ÝÍÊ¼°ÐÚÎåó¡¼°ÞË  ¾®ÈÕ  éùÃ«·Æ»§ÔÂ */
						/* ÐÚÈÊ */
						start.x = CHAR_getInt( toindex, CHAR_X);
						start.y = CHAR_getInt( toindex, CHAR_Y);
						parent_dir = NPC_Util_getDirFromTwoPoint( &start,&end );
						/* ºë·Â·¸Å«Ëüµ©×óÃóÆËÒà¼þÐÚÎåÃ«  òØÔÊÔÂ¿Ð±å£Ý
						 * ÆÝ¼°ºÒ·´ó¡¼°ºÒ¼°  Ã«Ü°µ¤·½µ¤±åÔÊÔÂ
						 */
						end = start;
						if( parent_dir != -1 ) {
							CHAR_walk( toindex, parent_dir, 0);
							
#if 1
			// Robin 0702 petFollow
			if( CHAR_getWorkInt( toindex, CHAR_WORKPETFOLLOW) != -1 ) {
					int petindex = CHAR_getWorkInt( toindex, CHAR_WORKPETFOLLOW);
					if( CHAR_CHECKINDEX(petindex) ) {
						int		parent_dir;
						/* ºÒ¼°ÞË  Îç£ÝÍÊ¼°ÐÚÎåó¡¼°ÞË  ¾®ÈÕ  éùÃ«·Æ»§ÔÂ */
						/* ÐÚÈÊ */
						end.x = CHAR_getInt( toindex, CHAR_X);
						end.y = CHAR_getInt( toindex, CHAR_Y);						
						start.x = CHAR_getInt( petindex, CHAR_X);
						start.y = CHAR_getInt( petindex, CHAR_Y);

						if( end.x > start.x )    end.x --;
						else if( end.x < start.x )	end.x ++;
						if( end.y > start.y )    end.y --;
						else if( end.y < start.y )	end.y ++;						
						
						parent_dir = NPC_Util_getDirFromTwoPoint( &start,&end );
						if( parent_dir != -1 ) {
							if( CHAR_walk( petindex, parent_dir, 0) == CHAR_WALKSUCCESSED )
							{
								CHAR_setWorkInt( petindex, CHAR_WORKPETFOLLOWCOUNT,
									1 + CHAR_getWorkInt( petindex, CHAR_WORKPETFOLLOWCOUNT) );
								if( CHAR_getWorkInt( petindex, CHAR_WORKPETFOLLOWCOUNT) > 1000 ){
									CHAR_setWorkInt( petindex, CHAR_WORKPETFOLLOWCOUNT, 0);
									CHAR_pickupFollowPet( toindex, petindex );
								}
								
							}
						}
					}else {
						CHAR_setWorkInt( toindex, CHAR_WORKPETFOLLOW, -1);
					}
				
			}			

#endif							
							
						}
					}
					else {
						/* âÙÓå¼ÀØÆØ¦ÔÆÔÊ */
						CHAR_setWorkInt( index, i+CHAR_WORKPARTYINDEX1, -1);
					}
				}
			}
#if 1
			// Robin 0702 petFollow
			if( CHAR_getWorkInt( index, CHAR_WORKPETFOLLOW) != -1 ) {
					int petindex = CHAR_getWorkInt( index, CHAR_WORKPETFOLLOW);
					if( CHAR_CHECKINDEX(petindex) ) {
						int		parent_dir;
						/* ºÒ¼°ÞË  Îç£ÝÍÊ¼°ÐÚÎåó¡¼°ÞË  ¾®ÈÕ  éùÃ«·Æ»§ÔÂ */
						/* ÐÚÈÊ */
						end.x = CHAR_getInt( index, CHAR_X);
						end.y = CHAR_getInt( index, CHAR_Y);						
						start.x = CHAR_getInt( petindex, CHAR_X);
						start.y = CHAR_getInt( petindex, CHAR_Y);

						if( end.x > start.x )    end.x --;
						else if( end.x < start.x )	end.x ++;
						if( end.y > start.y )    end.y --;
						else if( end.y < start.y )	end.y ++;						
						
						parent_dir = NPC_Util_getDirFromTwoPoint( &start,&end );

						//if( (end.x == start.x) && (end.x == start.y) )
						//	parent_dir = -1;						

						/* ºë·Â·¸Å«Ëüµ©×óÃóÆËÒà¼þÐÚÎåÃ«  òØÔÊÔÂ¿Ð±å£Ý
						 * ÆÝ¼°ºÒ·´ó¡¼°ºÒ¼°  Ã«Ü°µ¤·½µ¤±åÔÊÔÂ
						 */
						//print(" pdir:%d sx:%d sy:%d ex:%d ey:%d ", 
						//	parent_dir, start.x, start.y, end.x, end.y ); 
						//end = start;

						if( parent_dir != -1 ) {
							if( CHAR_walk( petindex, parent_dir, 0) == CHAR_WALKSUCCESSED )
							{
								CHAR_setWorkInt( petindex, CHAR_WORKPETFOLLOWCOUNT,
									1 + CHAR_getWorkInt( petindex, CHAR_WORKPETFOLLOWCOUNT) );
								if( CHAR_getWorkInt( petindex, CHAR_WORKPETFOLLOWCOUNT) > 1000 )
								{
									/*
									CHAR_setInt( petindex, CHAR_VARIABLEAI,
										200 + CHAR_getInt( petindex, CHAR_VARIABLEAI) );
									*/
									CHAR_setWorkInt( petindex, CHAR_WORKPETFOLLOWCOUNT, 0);
									CHAR_pickupFollowPet( index, petindex );
								}
							}
						}
					}
					else {
						/* âÙÓå¼ÀØÆØ¦ÔÆÔÊ */
						CHAR_setWorkInt( index, CHAR_WORKPETFOLLOW, -1);
					}
				
			}

#endif

		}
    }

    /* 99/03/09 By Kawata */
    /* walk¼°çø°í£ýWALKARRAY»¥ÛÍØøÊ§½ñÄ¾»¯ÖÐÔÂèëÄþ»¥Ø¤ÔÂ¼°Æ¥
     * äúÔ»  ØÆ»¯ÎìñâÔÂ
     */
    strcpysafe( tmp ,sizeof(tmp),
                CHAR_getWorkChar(index,CHAR_WORKWALKARRAY));
    if( strlen( tmp) > 0 ) {
        CHAR_setWorkChar(index,CHAR_WORKWALKARRAY,&tmp[1]);
    }

    /*  ÁÝÃÞÃ«ñ×»§ÔÂ    */
    CHAR_setWorkInt( index, CHAR_WORKWALKSTARTSEC , NowTime.tv_sec );
    CHAR_setWorkInt( index, CHAR_WORKWALKSTARTMSEC , NowTime.tv_usec );
}



/*------------------------------------------------------------
 * ÐÚÎåï§»§Ã«è¶  ÔÊÔÂ£Û
 * Â¦ÐÑ
 *  index           int         Æ½ÅÒ·Â    Æ¥¼°ÄÌ¼þ·¸ÓÀÛÍµ©
 *  dir             char*         éù  Ù¯  
 *  id              int         ID
 * ß¯Ô»°À
 *  Ø¦ØÆ
 ------------------------------------------------------------*/
void CHAR_walk_start(int index, int x, int y, char* dir, BOOL mapsendmode )
{

    if( !CHAR_CHECKINDEX( index ) )return;
    
    // Nuke 08/27: Stop wap
    /*
    {
       int ox, oy;
       const int     seesiz = MAP_CHAR_DEFAULTSEESIZ;
       ox = CHAR_getInt(index,CHAR_X);
       oy = CHAR_getInt(index,CHAR_Y);
       if ( ABS(x - ox) > seesiz || ABS(y - oy) > seesiz ) {
          CHAR_talkToCli(index, -1, "Òò×ù±ê´íÎó¶ø¶ÏÏß¡£", CHAR_COLORYELLOW);
          CONNECT_setCloseRequest_debug(getfdFromCharaIndex(index) , 1);
          return;
       }
    }
    */
                                                                                                       
    

    if( CHAR_getFlg( index,CHAR_ISDIE ) )return;
    CHAR_setWorkChar( index,CHAR_WORKWALKARRAY, dir );
    CHAR_setWorkInt( index, CHAR_WORKWALKSTARTSEC , NowTime.tv_sec );
    CHAR_setWorkInt( index, CHAR_WORKWALKSTARTMSEC , NowTime.tv_usec );

    CHAR_setMyPosition( index, x, y, FALSE);
	if( mapsendmode ) {
		int ox,oy;
		char	buf[64];
		int	len;
		int	i;
		strcpysafe( buf, sizeof(buf),
				CHAR_getWorkChar( index, CHAR_WORKWALKARRAY));

		len = strlen( buf );
	    ox = CHAR_getInt(index,CHAR_X);
	    oy = CHAR_getInt(index,CHAR_Y);

		for( i = 0; i < len; i ++ ) {
		    int	dir,mode;
			int	fx,fy;
		    CHAR_ctodirmode( buf[i], &dir ,&mode );
			if( mode == 0 ) {
			    CHAR_getCoordinationDir( dir,ox,oy,1,&fx,&fy);
				ox = fx;
				oy = fy;
			}
		}
		
		CHAR_sendMapAtWalk(index,
							CHAR_getInt( index, CHAR_FLOOR),
							CHAR_getInt( index, CHAR_X),
							CHAR_getInt( index, CHAR_Y),
							ox,oy);
	}

    if( strlen( dir ) >= 1 ) CHAR_walkcall( index );
}
/*------------------------------------------------------------
 * ÐÚÎå¼°  âÙ
 * Â¦ÐÑ
 *  index           int         Æ½ÅÒ·Â    Æ¥¼°ÄÌ¼þ·¸ÓÀÛÍµ©
 *  dir             char*         éù  Ù¯  
 *  id              int         ID
 * ß¯Ô»°À
 *  Ø¦ØÆ
 ------------------------------------------------------------*/
void CHAR_walk_init( int fd, int x, int y, char *direction, BOOL mapsendmode)
{
    if( strlen( direction) > 32) {
        print( "walkarray length over\n");
        return;
    }
    // Nuke +1 : For acceleration avoidance
    if (checkWalkTime(fd) < 0) {
        CONNECT_setCloseRequest(fd,1);
        print("Walk drop ");
        return;
    }

	if( CHAR_getWorkInt( CONNECT_getCharaindex(fd), CHAR_WORKBATTLEMODE) != BATTLE_CHARMODE_NONE){
		return ;
	}
	if( CHAR_getWorkInt( CONNECT_getCharaindex(fd), CHAR_WORKPARTYMODE ) != CHAR_PARTY_CLIENT ){
    	CHAR_walk_start( CONNECT_getCharaindex(fd), x, y, direction, mapsendmode );
	}else {
		int		dir, mode;
	    CHAR_ctodirmode( direction[0], &dir ,&mode );
		if( mode == 0 ) {
			//print( "Íæ¼Ò²¢·ÇÁì¶ÓÒ²·Çµ¥ÈËÍæ¼Ò¡£\n");
		}else {
	    	CHAR_walk_start( CONNECT_getCharaindex(fd),
                             x,y,direction, FALSE );
		}
	}
}

void CHAR_sendCharaAtWalk( int charaindex, int of,int ox,int  oy,int xflg, int yflg )
{
	int 	fd = -1;
	int     i,j,fl, x, y;
	char    introduction[512];
	char    c_msg[1024*96];
	int     strpos = 0;
	int     seesiz = CHAR_DEFAULTSEESIZ/2;
	char	cabuf[128];
	char	myintroduction[512];
	int		whichtype;

	whichtype = CHAR_getInt( charaindex, CHAR_WHICHTYPE);
	if( whichtype == CHAR_TYPEPLAYER) {
		fd = getfdFromCharaIndex( charaindex );
		if( fd == -1 )return;
	}
	if( xflg > 1 || xflg < -1 ) {
		print( "%s:%d: arg err[xflg]\n",__FILE__, __LINE__);
		return;
	}
	if( yflg > 1 || yflg < -1 ) {
		print( "%s:%d: arg err[yflg]\n",__FILE__, __LINE__);
		return;
	}
	fl = CHAR_getInt( charaindex, CHAR_FLOOR);
	x = CHAR_getInt( charaindex, CHAR_X);
	y = CHAR_getInt( charaindex, CHAR_Y);

	if( of !=  fl || ABS(x - ox) > seesiz/2 || ABS(y - oy) > seesiz/2 )
		return;

	c_msg[0] = '\0';
	if( !CHAR_makeObjectCString( CHAR_getWorkInt( charaindex, CHAR_WORKOBJINDEX), myintroduction, sizeof( myintroduction))){
		myintroduction[0] = '\0';
	}
	if( xflg != 0) {
		i = x + seesiz * xflg;
		for( j = y-seesiz ; j <= y+seesiz ;j ++ ) {
			OBJECT  object;
			for( object = MAP_getTopObj(fl,i,j) ; object ; object = NEXT_OBJECT(object ) ){
				int introlen;
				int objindex = GET_OBJINDEX(object);
				int	c_index = OBJECT_getIndex( objindex);

				if( OBJECT_getType(objindex) == OBJTYPE_NOUSE ) continue;
				if( OBJECT_getType(objindex) == OBJTYPE_CHARA && !CHAR_getFlg(OBJECT_getIndex(objindex), CHAR_ISVISIBLE) ){
					continue;
				}
				if( strlen( myintroduction) != 0 ) {
					if( OBJECT_getType( objindex) == OBJTYPE_CHARA ) {
						if( CHAR_getInt( c_index, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER ) {
							int		tofd = getfdFromCharaIndex( c_index );
							CDsend( tofd);
							lssproto_C_send( tofd, myintroduction);
						}
					}
				}
				if( whichtype == CHAR_TYPEPLAYER) {
					/* Make C*/
					if( CHAR_makeObjectCString( objindex, introduction,	sizeof( introduction))){
						introlen = strlen( introduction);
						introduction[introlen] = ',';
						introduction[introlen+1] = '\0';
						strcpysafe( &c_msg[strpos], sizeof( c_msg) - strpos, introduction);
						strpos += strlen( introduction);
					}
					if( OBJECT_getType(objindex) == OBJTYPE_CHARA ) {
						if( CHAR_getInt( c_index, CHAR_WHICHTYPE ) == CHAR_TYPEPLAYER &&
							CHAR_getWorkInt( c_index, CHAR_WORKPARTYMODE ) == CHAR_PARTY_LEADER ){
							if( CHAR_makeCAOPT1String( objindex, cabuf, sizeof( cabuf),	CHAR_ACTLEADER,1 )){
								CONNECT_appendCAbuf( fd,cabuf,strlen(cabuf));
							}
						}
						if( CHAR_getInt( c_index, CHAR_WHICHTYPE ) == CHAR_TYPEPLAYER &&
							CHAR_getWorkInt( charaindex, CHAR_WORKPARTYMODE ) == CHAR_PARTY_LEADER &&
							CHAR_getWorkInt( c_index, CHAR_WORKBATTLEMODE) == BATTLE_CHARMODE_NONE){
							int		tofd = getfdFromCharaIndex( c_index );
							if( tofd != -1 ) {
								if( CHAR_makeCAOPT1String( CHAR_getWorkInt( charaindex, CHAR_WORKOBJINDEX),
															cabuf, sizeof( cabuf), CHAR_ACTLEADER,1 )){
									CONNECT_appendCAbuf( tofd,cabuf,strlen(cabuf));
								}
							}
						}
						if( CHAR_getInt( c_index, CHAR_WHICHTYPE ) == CHAR_TYPEPLAYER &&
							CHAR_getWorkInt( c_index, CHAR_WORKBATTLEMODE )	!= BATTLE_CHARMODE_NONE ){
							if( CHAR_getWorkInt( c_index, CHAR_WORKBATTLEWATCH) == TRUE ) {
								if( CHAR_makeCAOPT1String( objindex, cabuf, sizeof( cabuf),
															CHAR_ACTBATTLEWATCH, 1)){
									CONNECT_appendCAbuf( fd, cabuf, strlen( cabuf));
								}
							}else {
								int	battleno = CHAR_getWorkInt( c_index, CHAR_WORKBATTLEINDEX);
								int	sideno   = CHAR_getWorkInt( c_index,CHAR_WORKBATTLESIDE);
								int	helpno;
								if( !BATTLE_CHECKINDEX( battleno) )	{
									//print(" %s[%d]: BATTLE_CHECKINDEX( %d) err!!\n", __FILE__, __LINE__, battleno);
								}else	{
									helpno = (BattleArray[ battleno].Side[ sideno].flg & BSIDE_FLG_HELP_OK) ? TRUE:FALSE;
									/* HelpNo = 1 ·´ÒÆ */
									if( CHAR_makeCAOPT3String( objindex, cabuf, sizeof( cabuf),
																CHAR_ACTBATTLE,	battleno,sideno,helpno )){
										CONNECT_appendCAbuf( fd,cabuf,strlen(cabuf));
									}
								}
							}
						}
						if( ( CHAR_getInt( c_index, CHAR_WHICHTYPE ) == CHAR_TYPEPLAYER ||
							  CHAR_getInt( c_index, CHAR_WHICHTYPE ) == CHAR_TYPEPET )  &&
							CHAR_getWorkInt( c_index, CHAR_WORKACTION ) != -1 ) {
							if( CHAR_makeCADefaultString( objindex, cabuf, sizeof( cabuf),
											CHAR_getWorkInt( c_index, CHAR_WORKACTION))){
								CONNECT_appendCAbuf( fd,cabuf,strlen(cabuf));
							}
						}
#ifdef _ADD_ACTION
						else if( CHAR_getWorkInt( c_index, CHAR_WORKACTION ) > 0 ) {
							if( CHAR_makeCADefaultString( objindex, cabuf, sizeof( cabuf),
											CHAR_getWorkInt( c_index, CHAR_WORKACTION))){
								CONNECT_appendCAbuf( fd,cabuf,strlen(cabuf));
							}
						}
#endif
					}
				}
			}
		}
	}
	if( yflg != 0) {
		j = y + seesiz * yflg;
		for( i = x-seesiz ; i <= x+seesiz  ;i ++ ) {
			OBJECT  object;
			for( object = MAP_getTopObj(fl,i,j) ; object ; object = NEXT_OBJECT(object ) ){
				int introlen;
				int objindex = GET_OBJINDEX(object);
				int	c_index = OBJECT_getIndex( objindex);
				if( OBJECT_getType(objindex) == OBJTYPE_NOUSE ) continue;
				if( OBJECT_getType(objindex) == OBJTYPE_CHARA &&
					!CHAR_getFlg(OBJECT_getIndex(objindex),	CHAR_ISVISIBLE) ){
					if(!CHAR_CHECKINDEX(OBJECT_getIndex(objindex))){
	            printf("×Ô¶¯É¾³ýÒ»¸öÎÊÌâ¶ÔÏó£¡");
	        		endObjectOne(objindex);
	        }
					continue;
				}
				if( strlen( myintroduction) != 0 ) {
					if( OBJECT_getType( objindex) == OBJTYPE_CHARA ) {
						if( CHAR_getInt( c_index, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER ) {
							int		tofd = getfdFromCharaIndex( c_index );
							CDsend( tofd);
							lssproto_C_send( tofd, myintroduction);
						}
					}
				}
				if( whichtype == CHAR_TYPEPLAYER ) {
					/* Make C*/
					if( CHAR_makeObjectCString( objindex, introduction,	sizeof( introduction))){
						introlen = strlen( introduction);
						introduction[introlen] = ',';
						introduction[introlen+1] = '\0';
						strcpysafe( &c_msg[strpos], sizeof( c_msg) - strpos, introduction);
						strpos += strlen( introduction);
					}
					if( OBJECT_getType(objindex) == OBJTYPE_CHARA ) {
						if( CHAR_getInt( c_index, CHAR_WHICHTYPE ) == CHAR_TYPEPLAYER &&
							CHAR_getWorkInt( c_index, CHAR_WORKPARTYMODE ) == CHAR_PARTY_LEADER ){
							if( CHAR_makeCAOPT1String( objindex, cabuf, sizeof( cabuf),
														CHAR_ACTLEADER,1 )){
								CONNECT_appendCAbuf( fd,cabuf,strlen(cabuf));
							}
						}
						if( CHAR_getInt( c_index, CHAR_WHICHTYPE ) == CHAR_TYPEPLAYER &&
							CHAR_getWorkInt( charaindex, CHAR_WORKPARTYMODE ) == CHAR_PARTY_LEADER &&
							CHAR_getWorkInt( c_index, CHAR_WORKBATTLEMODE)== BATTLE_CHARMODE_NONE){
							int		tofd = getfdFromCharaIndex( c_index );
							if( tofd != -1 ) {
								if( CHAR_makeCAOPT1String( CHAR_getWorkInt( charaindex, CHAR_WORKOBJINDEX),
															cabuf, sizeof( cabuf), CHAR_ACTLEADER,1 )){
									CONNECT_appendCAbuf( tofd,cabuf,strlen(cabuf));
								}
							}
						}
						if( CHAR_getInt( c_index, CHAR_WHICHTYPE ) == CHAR_TYPEPLAYER &&
							CHAR_getWorkInt( c_index, CHAR_WORKBATTLEMODE )	!= BATTLE_CHARMODE_NONE ){
							if( CHAR_getWorkInt( c_index, CHAR_WORKBATTLEWATCH) == TRUE ) {
								if( CHAR_makeCAOPT1String( objindex, cabuf,	sizeof( cabuf),
															CHAR_ACTBATTLEWATCH,1)){
									CONNECT_appendCAbuf( fd, cabuf, strlen( cabuf));
								}
							}else {
								int	battleno = CHAR_getWorkInt( c_index, CHAR_WORKBATTLEINDEX);
								int	sideno   = CHAR_getWorkInt( c_index, CHAR_WORKBATTLESIDE);
								int	helpno = (BattleArray[CHAR_getWorkInt( c_index,
										CHAR_WORKBATTLEINDEX)].Side[
										CHAR_getWorkInt( c_index,
										CHAR_WORKBATTLESIDE)].flg & BSIDE_FLG_HELP_OK) ? TRUE:FALSE;

								if( CHAR_makeCAOPT3String( objindex, cabuf, sizeof( cabuf),
															CHAR_ACTBATTLE,	battleno,sideno,helpno	)){
									CONNECT_appendCAbuf( fd,cabuf,strlen(cabuf));
								}
							}
						}
						if( ( CHAR_getInt( c_index, CHAR_WHICHTYPE ) == CHAR_TYPEPLAYER ||
							  CHAR_getInt( c_index, CHAR_WHICHTYPE ) == CHAR_TYPEPET )  &&
							CHAR_getWorkInt( c_index, CHAR_WORKACTION ) != -1 )	{
							if( CHAR_makeCADefaultString( objindex, cabuf, sizeof( cabuf),
											CHAR_getWorkInt( c_index, CHAR_WORKACTION))){
								CONNECT_appendCAbuf( fd,cabuf,strlen(cabuf));
							}
						}
#ifdef _ADD_ACTION
						else if( CHAR_getWorkInt( c_index, CHAR_WORKACTION ) > 0 ) {
							if( CHAR_makeCADefaultString( objindex, cabuf, sizeof( cabuf),
											CHAR_getWorkInt( c_index, CHAR_WORKACTION))){
								CONNECT_appendCAbuf( fd,cabuf,strlen(cabuf));
							}
						}
#endif
					}
				}
			}
		}
	}
	dchop( c_msg, "," );
	if( strlen( c_msg) != 0 ) {
		CDflush( charaindex);
		lssproto_C_send( fd, c_msg);
	}
}

static void CHAR_sendCDCharaAtWalk( int charaindex, int of, int ox, int oy,	 int xflg, int yflg ){
	int     i,j,fl, x, y;
	int     seesiz = CHAR_DEFAULTSEESIZ/2;
	int     fd, tofd;
	if( xflg > 1 || xflg < -1 ) {
		print( "%s:%d: arg err[xflg]\n",__FILE__, __LINE__);
		return;
	}
	if( yflg > 1 || yflg < -1 ) {
		print( "%s:%d: arg err[yflg]\n",__FILE__, __LINE__);
		return;
	}
	fl = CHAR_getInt( charaindex, CHAR_FLOOR);
	x = CHAR_getInt( charaindex, CHAR_X);
	y = CHAR_getInt( charaindex, CHAR_Y);

	if( of !=  fl ||  /*  °×·òÊ§»¥à¨µ¤    */
		ABS(x - ox) > seesiz/2 ||       /*  êû»¥  ÎåÖÐ  */
		ABS(y - oy) > seesiz/2 )        /*  êû»¥  ÎåÖÐ  */
		return;

	fd = getfdFromCharaIndex( charaindex );
	if( xflg != 0) {
		i = x + (seesiz * xflg + xflg)*-1;
		for( j = y-seesiz +(yflg*-1) ; j <= y+seesiz +(yflg*-1) ;j ++ ) {
			OBJECT  object;
			for( object = MAP_getTopObj(fl,i,j) ;
				 object ;
				 object = NEXT_OBJECT(object ) )
			{
				int objindex = GET_OBJINDEX(object);
				char    buf[64];
				if( OBJECT_getType(objindex) == OBJTYPE_NOUSE ) continue;
				if( OBJECT_getType(objindex) == OBJTYPE_CHARA &&
					!CHAR_getFlg(OBJECT_getIndex(objindex),	CHAR_ISVISIBLE) ){
					if(!CHAR_CHECKINDEX(OBJECT_getIndex(objindex))){
	            printf("×Ô¶¯É¾³ýÒ»¸öÎÊÌâ¶ÔÏó£¡");
	        		endObjectOne(objindex);
	        }
					continue;
				}
				if( fd != -1) {
					cnv10to62( objindex, buf, sizeof( buf));
					CONNECT_appendCDbuf( fd, buf, strlen( buf));
				}
				if( OBJECT_getType( objindex) == OBJTYPE_CHARA) {
					int tocharaindex = OBJECT_getIndex( objindex);
					if( CHAR_getInt( tocharaindex, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER )	{
						if( CHAR_getWorkInt( tocharaindex, CHAR_WORKBATTLEMODE)
							== BATTLE_CHARMODE_NONE ) {
							tofd = getfdFromCharaIndex( tocharaindex );
							if( tofd != -1 ) {
								cnv10to62( CHAR_getWorkInt( charaindex, CHAR_WORKOBJINDEX),
											buf, sizeof( buf));
								CONNECT_appendCDbuf( tofd, buf, strlen( buf));
								CAsend( tofd);
								CDsend( tofd);
							}
						}
					}
				}
			}
		}
	}
	if( yflg != 0) {
		j = y + (seesiz * yflg + yflg)*-1;
		for( i = x-seesiz + (xflg*-1) ; i <= x+seesiz + (xflg*-1) ;i ++ ) {
			OBJECT  object;
			for( object = MAP_getTopObj(fl,i,j) ; object ; object = NEXT_OBJECT(object ) ){
				int objindex = GET_OBJINDEX(object);
				char    buf[64];
				if( OBJECT_getType(objindex) == OBJTYPE_NOUSE ) continue;
				if( OBJECT_getType(objindex) == OBJTYPE_CHARA &&
					!CHAR_getFlg(OBJECT_getIndex(objindex),	CHAR_ISVISIBLE) ){
					continue;
				}
				if( fd != -1 ) {
					cnv10to62( objindex, buf, sizeof( buf));
					CONNECT_appendCDbuf( fd, buf, strlen( buf));
				}
				if( OBJECT_getType( objindex) == OBJTYPE_CHARA) {
					int tocharaindex = OBJECT_getIndex( objindex);
					if( CHAR_getInt( tocharaindex, CHAR_WHICHTYPE)	== CHAR_TYPEPLAYER ){
						if( CHAR_getWorkInt( tocharaindex, CHAR_WORKBATTLEMODE)	== BATTLE_CHARMODE_NONE ){
							tofd = getfdFromCharaIndex( tocharaindex );
							if( tofd != -1 ) {
								cnv10to62( CHAR_getWorkInt( charaindex, CHAR_WORKOBJINDEX),	buf, sizeof( buf));
								CONNECT_appendCDbuf( tofd, buf, strlen( buf));
								CAsend( tofd);
								CDsend( tofd);
							}
						}
					}
				}
			}
		}
	}
	CDsend( fd);
}

void CHAR_ctodirmode(char moji , int* dir , int* mode)
{
	*mode = 1;
	if( 'a' <= moji && moji <= 'h' )*mode = 0;

	*dir = tolower(moji) - 'a';
}
