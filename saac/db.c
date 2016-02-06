/*
  仿件平件弘窗  勾五犯□正矛□旦及  隶［

  1999 Aug 14 Created by ringo

  犯□正矛□旦反1  及伉件弁及溥匹忡绣今木月［
  
  
 */
#define _DB_C_
#include "db.h"
#include "main.h"
#include "util.h"
#include "version.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>

/*   侬  巨件玄伉及    length */
//#define CHARVALUE_MAX 1024
#define MAXTABLE 16
// Spock 2000/10/12
#define CHARVALUE_MAX 256	// DB 字串资料的buffer大小
#define KEY_MAX 64		// DB Key字串的buffer大小
#define HASH_SIZE 65536		// Hash table 一次增加的Entry数量
#define HASH_PRIME 65521	// Hash function 使用的质数
#define DBINIT_SIZE 16384	// DB 每次配置Entry的数量
// Spock end

/* 犯□正矛□旦及伉件弁及邰豳1蜊毛丐日歹允［ */
struct dbentry
{
    int use;
//    unsigned int keyhash;       /* 腹绸平□及甩永扑亘戊□玉 */
    int ivalue;                  /* 旦戊失［玄永皿 NODE 反  -1 匹｝
                                 允屯化及旦戊失反 0 动晓匹卅中午中仃卅中*/
//    int nextind;                /* -1 分匀凶日    毛啦  允月 */
    // Spock 2000/10/12
    int prev;	// 前一个dbentry, -1表示此项为head
    int next;	// 下一个dbentry, -1表示此项为tail
    char key[KEY_MAX];
    char charvalue[CHARVALUE_MAX];
//    char key[64];               /* 腹绸平□午卅月  侬   */
//    int charvalue_index;        /*   侬  田永白央毛今允index */
    // Spock end
};

// Spock 2000/10/12
// Database hashtable
struct hashentry
{
    char key[KEY_MAX];	// 索引key值
    int use;		// 是否已被使用
    int dbind;		// 指向 dbentry 的 index
    int prev;		// 同一key值的上一个 hashentry, -1为head
    int next;		// 同一key值的下一个 hashentry, -1为tail
};
// Spock end

typedef enum
{
    DB_INT_SORTED,
    DB_STRING,
}DBTYPE;

/* 1蜊及犯□正矛□旦毛丐日歹允 */
struct table
{
    int use;		// 0:未使用 1:已使用
    DBTYPE type;                    /* DB及潘   */
    char name[32];                  /* 犯□正矛□旦及  蟆 */
    int num;                        /* 巨件玄伉及醒 */
    int toplinkindex;
    // Spock 2000/10/12
    struct hashentry *hashtable;
    int hashsize;
    int updated;	// 0:dbflush後未更新 1:已更新
    int ent_finder;	// 指向最後一次配置的 hashentry
    // Spock end
};

struct dbentry *master_buf;     /* 巨件玄伉筏盛迕 */
int dbsize = 0;                 /*   赓0匹｝1,2,4,8,16...*/
static int dbent_finder = 0;

struct table dbt[MAXTABLE];

static void dbShowAllTable(void);

// Spock 2000/10/12
int dbHash(char* s)
{
    char *p;
    unsigned int h= 0 ,g;
    for( p = s ; *p ; p ++ ){
        h = ( h<< 4 ) + (*p);
        if( (g = h & 0xf0000000) != 0){
            h = h ^ (g>>24);
            h = h ^ g;
        }
    }
    return h % HASH_PRIME;
}
// Spock end

/* Spock deleted 2000/10/12
struct charvalue
{
    int use;
    char buf[CHARVALUE_MAX];
};
struct charvalue *charvalue_buf;
int charvaluesize=0;
*/

/*
    侬  田永白央□毛傀舰允月
 */
/* Spock deleted 2000/10/12
int
reallocCharValue(void)
{
    struct charvalue *previous = charvalue_buf;
    struct charvalue *newbuf;
    int new_charvaluesize;
    if( charvaluesize == 0 ){
        new_charvaluesize = 1;
    } else {
        new_charvaluesize = charvaluesize * 2;
    }

    newbuf = ( struct charvalue *) calloc( 1, new_charvaluesize *
                                          sizeof( struct charvalue ));
    if( newbuf == NULL ){
        log( "reallocCharValue: memory shortage!! new_charvaluesize:%d\n",
             new_charvaluesize );
        return -1;
    }
    memset( newbuf, 0 , new_charvaluesize * sizeof( struct charvalue ));
    if( previous) memcpy( (char*)newbuf, (char*)previous,
            charvaluesize * sizeof( struct charvalue ));
    free( previous );
    charvaluesize = new_charvaluesize;
    charvalue_buf = newbuf;

    log( "reallocCharValue: "
         "new_charvaluesize:%d Old address:%x New address:%x\n",
         new_charvaluesize , (unsigned int )previous,
         (unsigned int)newbuf );
    return 0;
}
*/

/*
    侬  田永白央□毛1蜊歹曰丐化月［
  凶曰卅仁卅匀凶日realloc允月［
  
*/
/* Spock deleted 2000/10/12
static int charvalue_finder=0;
static int
dbAllocCharValue( void )
{
    int i;
    for(i=0;i<charvaluesize;i++){
        charvalue_finder++;
        if( charvalue_finder == charvaluesize ) charvalue_finder =0;
        if( charvalue_buf[charvalue_finder].use == 0 ){
            charvalue_buf[charvalue_finder].use =1;
            charvalue_buf[charvalue_finder].buf[0] = 0;
            return charvalue_finder;
        }

    }
    log( "dbAllocCharValue: charvalue array full. reallocating....\n" );
    if( reallocCharValue() < 0 ){
        log( "dbAllocCharValue: reallocation fail\n");
    } else {
        return dbAllocCharValue();
    }
    return -1;
}
*/

/*
  charvalue 井日/卞袄毛必永玄/本永玄允月
  int index : charvalue index
  
 */
/* Spock deleted 2000/10/12
static char *
dbGetString( int index )
{
    return charvalue_buf[index].buf;
}
static int
dbSetString( int index , char *data )
{
    int l = strlen(data);
    if( l >= (sizeof( charvalue_buf[0].buf )-1)) return -1;
    memcpy( charvalue_buf[index].buf , data, l+1 );
    return 0;
}
*/

/*
  DB及云云五今互凶日氏仁卅匀凶日歹曰丐化卅云允［中引及扔奶术及2  卞允月
  0分匀凶日1卞允月
 */
static int
reallocDB( void )
{
    struct dbentry *previous = master_buf;
    struct dbentry *newbuf;
    int new_dbsize;
/* Spock deleted 2000/10/12    
    if( dbsize == 0 ){
        new_dbsize = 1;
    } else {
        new_dbsize = dbsize * 2;
    }
*/
    // Spock+1 2000/10/12
    new_dbsize = dbsize + DBINIT_SIZE;
    
    newbuf = (struct dbentry* ) calloc( 1, new_dbsize *
                                         sizeof( struct dbentry) );
    /* 丢乒伉凶曰卅中   */
    if( newbuf == NULL ){
        log( "重新分配数据: 内存不足!!! 新数据大小: %d\n", new_dbsize );
        return -1;
    }

    /* 衙中幻丹井日蕙仄中  卞戊疋□仄化 */

    memset( newbuf , 0 , new_dbsize * sizeof( struct dbentry ) );
    /* Spock deleted 2000/10/19
    if( previous )memcpy( (char*)newbuf, (char*)previous,
            dbsize * sizeof( struct dbentry ));

    // 衙中幻丹毛荸  仄
    free( previous );
    */
    // Spock 2000/10/19
    if ( dbsize > 0 )
    {
    	memcpy( newbuf , previous , dbsize * sizeof(struct dbentry));
    	free( previous );
    }
    // Spock end

    dbent_finder = dbsize;	// 将 dbent_finder 指向未使用的 entry
    dbsize = new_dbsize;
    master_buf = newbuf;
    
    log( "重新分配数据: 新数据大小:%d 旧地址: %x 新地址:%x\n",
         new_dbsize , (unsigned int)previous, (unsigned int)newbuf );

    return 0;
}



/*
  allocate a node
 */
static int
//dbAllocNode( DBTYPE type  )
// Spock +1 2000/10/13
dbAllocNode()
{
    int i;
    for(i=0;i<dbsize;i++){
        dbent_finder ++;
        if( dbent_finder == dbsize ) {
            dbent_finder = 0;
        }
        if( master_buf[dbent_finder].use == 0 ){
            master_buf[dbent_finder].use = 1;
            /* Spock deleted 2000/10/12
            // int 匹手尥笛树  及啃卞stringbuffer毛  勾仪卞允月 kawata
            if( type == DB_STRING || type == DB_INT_SORTED){
                if( ( master_buf[dbent_finder].charvalue_index =
                      dbAllocCharValue() ) < 0 ){
                    //   侬  田永白央□互凶曰卅中冗
                    return -1;
                }
            }
            */
            return dbent_finder;
        }
    }
    log( "数据进入队列失败. 重新分配中....\n" );
    if( reallocDB() < 0 ){
        log( "重新分配失败\n" );
    } else {
        //return dbAllocNode( type );
        // Spock 2000/10/13
        master_buf[dbent_finder].use = 1;
        log( "数据探测=%d\n" , dbent_finder );
        return dbent_finder;
        // Spock end
    }
    return -1;
}
static void
dbReleaseNode( int index )
{
    // Spock 2000/10/12
    int prev = master_buf[index].prev;
    int next = master_buf[index].next;
    master_buf[index].use = 0;
    if ( prev >= 0 ) master_buf[prev].next = next;
    if ( next >= 0 ) master_buf[next].prev = prev;
    // Spock end
    /* Spock deleted 2000/10/12
	if( master_buf[index].charvalue_index >= 0 ) {
		charvalue_buf[ master_buf[index].charvalue_index].use = 0;
	}
    */
}
void
dbShowLink( int topind )
{
    int cur = topind;

    log( "开始从 %d 链接数据\n", cur );
    
    /* Spock deleted 2000/10/19
    for(;;){
        if( cur == -1 )break;
    */
    // Spock +1 2000/10/19
    while ( cur >= 0 )
    {
        if( master_buf[cur].use == 0 ){
            log( "dbShowLink: use is 0! key:%s\n", master_buf[cur].key );
            return;
        }
        // Spock +1  2000/10/12
        log( "%s %i\n", master_buf[cur].key, master_buf[cur].ivalue );
        /* Spock deleted 2000/10/12
        log( "%s %u %i\n", master_buf[cur].key ,
             master_buf[cur].keyhash, master_buf[cur].ivalue );
        */
        cur = master_buf[cur].next;
    }
}
// Spock 2000/10/13
static int
reallocHash( int dbi )
{
    struct hashentry *previous = dbt[dbi].hashtable;
    struct hashentry *newbuf;
    int new_hashsize;
 
    new_hashsize = dbt[dbi].hashsize + HASH_SIZE;
    newbuf = (struct hashentry* ) calloc( 1, new_hashsize *
                                         sizeof( struct hashentry) );
    if( newbuf == NULL ){
        log( "重新分配无用信息: 内存不足!!! 新无用信息大小: %d\n", new_hashsize );
        return -1;
    }

    memset( newbuf , 0 , new_hashsize * sizeof( struct hashentry ) );
    if( previous )
    {
    	memcpy( newbuf, previous,
            dbt[dbi].hashsize * sizeof( struct hashentry ));
        free( previous );
    }

    if ( dbt[dbi].hashsize > HASH_PRIME )
        dbt[dbi].ent_finder = dbt[dbi].hashsize;
    else
        dbt[dbi].ent_finder = HASH_PRIME;
    dbt[dbi].hashsize = new_hashsize;
    dbt[dbi].hashtable = newbuf;
    
    log( "重新分配无用信息: 新无用信息大小:%d 旧地址: %x 新地址:%x\n",
         new_hashsize , (unsigned int)previous, (unsigned int)newbuf );

    return 0;
}

static int tableGetEntry( int dbi , char *k )
{
    int hashkey = dbHash( k );
    struct hashentry *hash = dbt[dbi].hashtable;
    if ( hash[hashkey].use == 0 ) return -1;
    while ( 1 ) {
//		if ( hash[hashkey].use == 1 && strcmp( hash[hashkey].key , k ) == 0 ){
		if ( hash[hashkey].use == 1 ){
			if( strcmp( hash[hashkey].key , k ) == 0 )return hashkey;
		}
   		hashkey = hash[hashkey].next;
		if ( hashkey <= 0 ){
//			log("err not found hash[%x] -%s!\n", hashkey, k)
			return -1;
		} 
    }
}

static int tableInsertNode( int dbi , char *k , int dbind )
{
    int hashkey = dbHash( k );
    int hashnext = -1;
    int i;
    struct hashentry *hash = dbt[dbi].hashtable;
  
    if ( hash[hashkey].use == 0 )  {
    	strcpy( hash[hashkey].key , k );
   	hash[hashkey].use = 1;
    	hash[hashkey].dbind = dbind;
    	hash[hashkey].prev = -1;
    	hash[hashkey].next = -1;
    	dbt[dbi].num++;
        return hashkey;
    }else {
    	for ( i=0; i<dbt[dbi].hashsize-HASH_PRIME; i++ ){
    	    dbt[dbi].ent_finder++;
    	    if ( dbt[dbi].ent_finder >= dbt[dbi].hashsize )
    	        dbt[dbi].ent_finder = HASH_PRIME;
    	    if ( hash[dbt[dbi].ent_finder].use == 0 )
    	    {
    	        hashnext = dbt[dbi].ent_finder;
    	        break;
    	    }
    	}
    	if ( hashnext < HASH_PRIME )
    	{
            log( "tableInsertNode: hashentry array full. reallocating....\n" );
            if( reallocHash( dbi ) < 0 ){
                log( "tableInsertNode: reallocation fail\n" );
                return -1;
            }
            else
            {
            	hash = dbt[dbi].hashtable;
            	hashnext = dbt[dbi].ent_finder;
            }
        }
        strcpy( hash[hashnext].key , k );
        hash[hashnext].use = 1;
        hash[hashnext].dbind = dbind;
        hash[hashnext].prev = hashkey;
        hash[hashnext].next = hash[hashkey].next;
        if ( hash[hashkey].next >= 0 )
            hash[hash[hashkey].next].prev = hashnext;
        hash[hashkey].next = hashnext;
        dbt[dbi].num++;
        return hashnext;
    }	
}

static void
tableReleaseNode( int dbi , int ind )
{
    dbt[dbi].hashtable[ind].use = 0;
    if ( dbt[dbi].hashtable[ind].prev >= 0 ){
        dbt[dbi].hashtable[dbt[dbi].hashtable[ind].prev].next =
        dbt[dbi].hashtable[ind].next;
    }
    if ( dbt[dbi].hashtable[ind].next >= 0 ){
        dbt[dbi].hashtable[dbt[dbi].hashtable[ind].next].prev =
        dbt[dbi].hashtable[ind].prev;
    }
    dbt[dbi].num--;
}
// Spock end
/*
  伉件弁及玄永皿毛芨尹日木凶日｝平□毛  曰卞用□玉毛腹绸允月［
  心勾井日卅中桦宁反巨仿□匹反卅中及匹0

 */
/* Spock deleted 2000/10/13
static int
dbExtractNodeByKey( int topind , char *k  )
{
    int cur = topind;
    int prev = -1;
    unsigned int h = hashpjw( k );

    // 伉件弁互坞匹手心勾井日卅中分仃卅及匹0毛井尹允
    if( topind == -1 ) return 0;

    for(;;){
        if( cur == -1 )break;
        if( master_buf[cur].keyhash == h
            && strcmp( master_buf[cur].key , k ) == 0 ){
            // prev 及 戚互 cur 及戚卞卅月方丹卞允月
            if( prev == -1 ){
                // 燮  分匀凶及匹伉件弁反中元日卅中
            } else {                
                master_buf[prev].nextind = master_buf[cur].nextind;
            }
            // 公木匹愤坌互伉旦玄井日陆木月及匹荸  允月
            dbReleaseNode( cur );
            log( "find key %s deleted\n", k );
            return 0;
        }
        prev = cur;
        cur = master_buf[cur].nextind;
    }
    // not found
    log( "dbExtractNodeBykey: %s not found\n" , k );
    return 0;
}
*/
// Spock 2000/10/13
static int
dbExtractNodeByKey( int dbi , char *k  )
{
    int hashind = tableGetEntry( dbi , k );

    if ( hashind < 0 ){
    	log( "dbExtractNodeByKey: tableGetEntry fail, key:%s\n" , k );
    	return -1;
    }
    if ( dbt[dbi].hashtable[hashind].dbind < 0 ){
    	log( "dbExtractNodeByKey: invalid dbind in hash, key:%s\n" , k );
    	return -1;
    }
    dbReleaseNode( dbt[dbi].hashtable[hashind].dbind );
    tableReleaseNode( dbi , hashind );
    return 0;
}
// Spock end

/* Spock deleted 2000/10/12
static int 
dbGetEntryByKey( int topind , char *k )
{
    int cur = topind;
    unsigned int h = hashpjw( k );

    if( topind == -1 ) return 0;

    for(;;){
        if( cur == -1 )break;
        if( master_buf[cur].keyhash == h
            && strcmp( master_buf[cur].key, k ) == 0 ){
            return cur;
        }
        cur = master_buf[cur].nextind;
    }
    return -1;
}
*/

/*
  伉件弁及玄永皿毛芨尹日木凶日｝袄毛  曰卞用□玉毛腹绸仄化
  赝濠卅午仇欠卞 Insert 允月［切中今中  井日云云五中  卞卅日氏匹中月午
  移烂

 */
/* Spock deleted 2000/10/13
static int
dbInsertNodeByIValue( int topind , int ins )
{
    int cur = topind;
    int prev = -1;

    if( topind == -1 ) return -1;
    
    for(;;){
        if( cur == -1 ){
            //     引匹中匀凶及匹馨笛允月
            master_buf[prev].nextind = ins;
            master_buf[ins].nextind = -1;
            return 0;
        }
        if( master_buf[cur].ivalue < master_buf[ins].ivalue ){
            if( prev == -1 ){
                log( "top_node is badly configured\n" );
                return -1;
            }
            master_buf[prev].nextind = ins;
            master_buf[ins].nextind = cur;
            return 0;
        }
        prev = cur;
        cur = master_buf[cur].nextind;
    }
    
    return -1;
}
*/
// Spock 2000/10/13
static int
dbInsertNodeByIValue( int topind , int ins )
{
    int cur = topind;
    if ( (topind < 0)||(topind >= dbsize)||(ins < 0)||(ins >= dbsize ) )
        return -1;
    while ( master_buf[cur].next >= 0 ){
    	if ( master_buf[master_buf[cur].next].ivalue < master_buf[ins].ivalue )
    	    break;
    	cur = master_buf[cur].next;
    }
    master_buf[ins].prev = cur;
    master_buf[ins].next = master_buf[cur].next;
    if ( master_buf[cur].next >= 0 )
        master_buf[master_buf[cur].next].prev = ins;
    master_buf[cur].next = ins;
    return 0;
}
// Spock end
/* Spock deleted 2000/10/13
static int
dbAppendNode( int topind , int ins )
{
    int cur =topind;
    int prev = -1;
    if( topind == -1 ) return -1;
    for(;;){
        if( cur == -1 ){
            master_buf[prev].nextind = ins;
            master_buf[ins].nextind = -1;
            return 0;
        }
        prev = cur;
        cur = master_buf[cur].nextind;
    }
    return -1;
}
*/
// Spock 2000/10/13
static int
dbAppendNode( int topind , int ins )
{

    if ( (topind < 0)||(topind >= dbsize)||(ins < 0)||(ins >= dbsize ) )
        return -1;
    master_buf[ins].prev = topind;
    master_buf[ins].next = master_buf[topind].next;
    if ( master_buf[topind].next >= 0 )
        master_buf[master_buf[topind].next].prev = ins;
    master_buf[topind].next = ins;
    return 0;
}
// Spock end

/*
  犯□正矛□旦及  蟆毛  月［db反醒互剂卅中及匹  骚卞strcmp仄化方中
  DBTYPE :潘  ［

  云卅元卅引尹及  反｝帮醒午  侬  及    卞绣箕允月仇午互匹五月［
  
 */
static int
dbGetTableIndex( char *tname , DBTYPE type )
{
    int i;

    for(i=0;i<MAXTABLE;i++){
        if( dbt[i].use && strcmp( dbt[i].name , tname ) == 0 &&
            dbt[i].type == type ){
            return i ;
        }
    }

    /* 心勾井日卅井匀凶及匹蕙筋分 */
    for(i=0;i<MAXTABLE;i++){
        if( dbt[i].use == 0 ){
            int topind;
            dbt[i].use = 1;
            dbt[i].type = type;
            snprintf( dbt[i].name , sizeof( dbt[i].name ) , "%s", tname );
            // Spock 2000/10/16
            if ( reallocHash( i ) < 0 )
            {
            	log( "重新分配无用信息失败\n");
            	return -2;
            }
            dbt[i].ent_finder = HASH_PRIME;
            // Spock end

            //topind = dbAllocNode( type );
            // Spock +1 2000/10/16
            topind = dbAllocNode();
            if( topind < 0 ){
                log( "数据分配节点失败\n" );
                return -2;
            }
            /* Spock deleted 2000/10/16
            snprintf( master_buf[topind].key ,
                      sizeof(master_buf[topind].key), "top_node" );
            master_buf[topind].keyhash = hashpjw( master_buf[topind].key );
            master_buf[topind].nextind = -1;
            */
            
            /* 玄永皿及用□玉毛赓渝祭允月卅曰［
               侬  及桦宁手帮醒及桦宁手云卅元匹方中［
             0x7fffffff午中丹袄反｝  侬  及桦宁反今幻升啦  毛手凶卅中及分［*/
            master_buf[topind].ivalue = 0x7fffffff;    
            /* Spock deleted 2000/10/16
            if( type == DB_INT_SORTED ){
                master_buf[topind].charvalue_index = -1;
                //dbSetString( master_buf[topind].charvalue_index, "" );
            } else {
                dbSetString( master_buf[topind].charvalue_index, "" );
            }
	    */
            // Spock 2000/10/16
            master_buf[topind].prev = -1;
            master_buf[topind].next = -1;
            strcpy( master_buf[topind].key , "top_node" );
            master_buf[topind].charvalue[0] = 0;
            // Spock end
            dbt[i].toplinkindex = topind;
            return i;
        }
    }

    /*   互中匀天中分［ */
    log( "dbGetTableIndex: table full. now tables are:\n" );
    dbShowAllTable();

    return -1;
}

/*

  午曰丐尹内 strtol 匹五月袄仄井扔禾□玄仄卅中方

  腹绸仄化苇勾仃凶用□玉卞覆仄化｝
  伉件弁井日  五午匀化井日｝漆蘸反末□玄允月及卞银丹袄毛凶方曰卞
  腹绸仄化｝中中桦赭毛心勾仃凶日｝公仇卞犒仄仇戈［
  
 */
/* Spock deleted 2000/10/16
int dbUpdateEntryInt( char *table , char *key , int value, char *info )
{
    int r, dbi = dbGetTableIndex( table , DB_INT_SORTED );
    int entind;

    if( strlen(key) >= sizeof( master_buf[0].key) )return -1;
    
    if( dbi < 0 ) return -1;

    r = dbExtractNodeByKey( dbt[dbi].toplinkindex , key );
    if( r < 0 ){
        log( "dbUpdateEntryInt: dbExtractNodeByKey fail! bug!!!!\n" );
        return -1;
    }

    entind = dbAllocNode(DB_INT_SORTED);
    if( entind < 0 ) return -1;
    master_buf[entind].ivalue = value;
    snprintf( master_buf[entind].key ,
              sizeof(master_buf[entind].key), "%s", key );
    master_buf[entind].keyhash = hashpjw( master_buf[entind].key );
    master_buf[entind].nextind = -1; 
	
	// 尥笛树  毛本永玄允月
    dbSetString( master_buf[entind].charvalue_index, info );
	
	
    r = dbInsertNodeByIValue( dbt[dbi].toplinkindex , entind );
    if( r < 0 ){
        log( "dbUpdateEntryInt: dbInsertNodeByIValue failed\n" );
        return -1;
    }

    log( "dbUpdateEntryInt: successfully updated entry %s:%s:%d\n",
             table, key, value );
    return 0;    
    
}
*/
// Spock 2000/10/16
int dbUpdateEntryInt( char *table , char *key , int value, char *info )
{
    int dbi = dbGetTableIndex( table , DB_INT_SORTED );
    int dbind, hashind, newpos;
    // Spock 2000/10/23
    if ( strlen( key ) >= KEY_MAX ) {
    	log( "dbUpdateEntryInt: key is too long, key:%s\n", key );
    	return -1;
    }
    if ( strlen( info ) >= CHARVALUE_MAX ) {
    	log( "dbUpdateEntryInt: charvalue is too long, charvalue:%s\n", info );
    	return -1;
    }
    // Spock end
    if ( dbi < 0 ){
    	log( "dbUpdateEntryInt: dbGetTableIndex fail\n");
    	return -1;
    }
    hashind = tableGetEntry( dbi , key );
    if ( hashind < 0 )
    {
    	dbind = dbAllocNode();
    	if ( dbind < 0 )
    	{
    	    log( "dbUpdateEntryInt: dbAllocNode fail\n" );
    	    return -1;
    	}
    	master_buf[dbind].ivalue = value;
    	strcpy( master_buf[dbind].key , key );
    	strcpy( master_buf[dbind].charvalue , info );
    	if ( dbInsertNodeByIValue( dbt[dbi].toplinkindex , dbind ) < 0 )
    	{
    	    master_buf[dbind].use = 0;
    	    log( "dbUpdateEntryInt: dbInsertNodeByIValue fail\n" );
    	    return -1;
    	}
    	if ( tableInsertNode( dbi , key , dbind ) < 0 )
    	{
    	    dbReleaseNode( dbind );
    	    log( "dbUpdateEntryInt: tableInsertNode fail\n" );
    	    return -1;
    	}
    }
    else
    {
    	dbind = dbt[dbi].hashtable[hashind].dbind;
    	master_buf[dbind].ivalue = value;
    	strcpy( master_buf[dbind].charvalue , info );
    	newpos = dbind;
    	while ( master_buf[newpos].prev >= 0 )
    	{
      	    if ( value <= master_buf[master_buf[newpos].prev].ivalue )
    	    {
    	    	break;
    	    }
    	    newpos = master_buf[newpos].prev;
    	}
    	if ( newpos != dbind )
    	{
    	    master_buf[master_buf[dbind].prev].next = master_buf[dbind].next;
            if ( master_buf[dbind].next >= 0 )
    	        master_buf[master_buf[dbind].next].prev = master_buf[dbind].prev;
    	    master_buf[dbind].prev = master_buf[newpos].prev;
    	    master_buf[dbind].next = newpos;
    	    if ( master_buf[newpos].prev >= 0 )
    	        master_buf[master_buf[newpos].prev].next = dbind;
    	    master_buf[newpos].prev = dbind;
    	    dbt[dbi].updated = 1;
		/*
	    log( "dbUpdateEntryInt: successfully updated entry %s:%s:%d\n",
	        table, key, value );
		*/
    	    return 0;
    	}
    	while ( master_buf[newpos].next >= 0 )
    	{
    	    if ( value >= master_buf[master_buf[newpos].next].ivalue )
    	    {
    	    	break;
    	    }
	    newpos = master_buf[newpos].next;
    	}
    	if ( newpos != dbind )
    	{
    	    master_buf[master_buf[dbind].prev].next = master_buf[dbind].next;
    	    master_buf[master_buf[dbind].next].prev = master_buf[dbind].prev;
    	    master_buf[dbind].prev = newpos;
    	    master_buf[dbind].next = master_buf[newpos].next;
    	    if ( master_buf[newpos].next >= 0 )
    	        master_buf[master_buf[newpos].next].prev = dbind;
    	    master_buf[newpos].next = dbind;
    	}
    }
    dbt[dbi].updated = 1;
	/*
    log( "dbUpdateEntryInt: successfully updated entry %s:%s:%d\n",
             table, key, value );
			 */
    return 0;
}
// Spock end

int
dbDeleteEntryInt( char *table, char *key )
{
    int dbi = dbGetTableIndex( table , DB_INT_SORTED );
    int r;

    if ( strlen( key ) >= KEY_MAX ) {
    	log( "dbDeleteEntryInt: key is too long, key:%s\n", key );
    	return -1;
    }
    if( dbi < 0 ) {
    	log( "dbDeleteEntryInt: dbGetTableIndex failed for %s\n", table );
    	return -1;
    }
    //r = dbExtractNodeByKey( dbt[dbi].toplinkindex , key );
    // Spock fixed 2000/10/19
    r = dbExtractNodeByKey( dbi , key );
    if( r < 0 ){
        log( "dbDeleteEntryInt: dbExtractNodeByKey failed for %s in %s\n",
             key,table );
        return -1;
    }
    // Spock +1 2000/10/19
    dbt[dbi].updated = 1;
    log( "删除人物 %s 来至表 %s\n", key, table );
    return 0;
}

static void
dbShowAllTable(void)
{
    int i;

    for(i=0;i<MAXTABLE;i++){
        if( dbt[i].use ){
            log( "%d Name:%s Use:%d Type:%d\n",i,
                     dbt[i].name , dbt[i].num , dbt[i].type );
        }
    }
    
}

/* 犯□正毛1蜊潸曰分允［
 */
int
dbGetEntryInt( char *table, char *key, int *output )
{
    int dbi = dbGetTableIndex( table , DB_INT_SORTED );
    int entind;
    // Spock +1 2000/10/19
    int hashind;
 
    // Spock deleted 2000/10/19
    //if( strlen(key) >= sizeof( master_buf[entind].key) ) return -1;
    if( dbi <0 ) {
    	log( "dbGetEntryInt: dbGetTableIndex fail\n" );
    	return -1;
    }
    // Spock 2000/10/19
    if( strlen(key) >= KEY_MAX ) {
    	log( "dbGetEntryInt: key is too long, key:%s\n" , key );
    	return -1;
    }
    hashind = tableGetEntry( dbi , key );
    if( hashind < 0 ) return -1;
    entind = dbt[dbi].hashtable[hashind].dbind;
    //entind = dbGetEntryByKey( dbt[dbi].toplinkindex , key );
    // Spock end
    if( entind < 0 ) {
    	log( "dbGetEntryInt: Invalid dbind in hashtable of %s\n" , table );
    	return -1;
    }
    /* 心勾井匀凶及匹袄毛请  卞  木化忒允 */
    *output = master_buf[entind].ivalue;

    return 0;
}

/*
  巨仿□及桦宁反  ［0分匀凶日岳  ［

  int *rank_out : 仿件弁及请  
  int *count_out : 晓井日窒蜊  井及请  

  int 犯□正矛□旦毁迕友
  
 */

int
dbGetEntryRank( char *table, char *key , int *rank_out, int *count_out)
{
    int dbi = dbGetTableIndex( table , DB_INT_SORTED );
    // Spock deleted 2000/10/19
    //unsigned int hash = hashpjw(key);
    int cur;
    int now_score = 0x7fffffff;     /*int 匹中切壬氏匹井中袄 */
    int r = -1 , i=0;

    // Spock 2000/10/23
    //if( strlen(key) >= sizeof( master_buf[cur].key) ) return -1;
    if( strlen(key) >= KEY_MAX ) {
    	log( "dbGetEntryRank: key is too long, key:%s\n" , key );
    	return -1;
    }
    if( dbi <0 ) {
    	log( "dbGetEntryRank: dbGetTableIndex fail\n" );
    	return -1;
    }
    // Spock end

    // Spock 2000/10/23
    //cur = master_buf[dbt[dbi].toplinkindex].nextind;
    cur = master_buf[dbt[dbi].toplinkindex].next;
    //i=0;
    //for(;;){
    //    if( cur == -1 )break;
    while ( cur >= 0 )
    {
    // Spock end
        if( master_buf[cur].ivalue != now_score ){
            r=i;
            now_score = master_buf[cur].ivalue;
        }
        // Spock 2000/10/19
        //if( hash == master_buf[cur].keyhash &&
        //    strcmp( master_buf[cur].key, key )== 0 ){
        if( strcmp( master_buf[cur].key , key ) == 0 )
        {
        // Spock end
            *rank_out = r;
            *count_out = i;
            return 0;
        }
        //cur = master_buf[cur].nextind;
        // Spock fixed 2000/10/19
        cur = master_buf[cur].next;
        i++;
    }
    *count_out = i;
    *rank_out = r;
    return 0;
}

/*
  int 毁迕友
 */
int
dbGetEntryRankRange( char *table,
                     int start, int end, char *output, int outlen )
{
#define MAXHITS 1024        /* 赝癫支卅丐［匹手仇木匹蜗坌日仄中冗 ringo */
    struct hitent{          /* 仇及厌瞻  卞甲永玄仄凶支勾毛凶户化中仁 */
        int entind;
        int rank;
    };

    int r=0;
    struct hitent hits[MAXHITS];
    int dbi = dbGetTableIndex( table , DB_INT_SORTED );
    int cur;
    int hitsuse = 0,i;
    int now_score = 0x7fffffff;

    if( dbi <0 ) return -1;
    if( outlen <= 0 )return -1;
    
    cur = dbt[dbi].toplinkindex;
    // Spock 2000/10/23
    //for(;;){
    //    if( cur == -1 )break;
    while ( cur >= 0 )
    {
    // Spock end
        if( master_buf[cur].ivalue != now_score ){
            r++;
            now_score = master_buf[cur].ivalue;
        }
        if( r >= start && r <= end ){
            hits[hitsuse].entind = cur;
            hits[hitsuse].rank = r;
            hitsuse++;
            //if( hitsuse == MAXHITS )break;
            // Spock fixed 2000/10/23
            if( hitsuse >= MAXHITS ) break;
        }
        //cur = master_buf[cur].nextind;
        // Spock fixed 2000/10/19
        cur = master_buf[cur].next;
    }
    output[0] = 0;
    
    for(i=0;i<hitsuse;i++){
        char tmp[1024];
        snprintf( tmp, sizeof(tmp),
                  "%d,%s,%d,%s", hits[i].rank, master_buf[hits[i].entind].key,
                  master_buf[hits[i].entind].ivalue,
	//		            dbGetString( master_buf[i].charvalue_index ));
	// Spock fixed 2000/10/19
		  master_buf[hits[i].entind].charvalue );
        strcatsafe( output, outlen, tmp );
        if( i != ( hitsuse -1 ) ){
            strcatsafe( output, outlen, "|" );
        }
    }
    return 0;
}

int dbFlush( char *dir )
{
    int i;

    for(i=0;i<MAXTABLE;i++){
        FILE *fp;
        char filename[1024];
        int entind;
        //int j;
        if( !dbt[i].use ) continue;
        // Spock 2000/10/23
        if( dbt[i].updated == 0 )
        {
            log( "dbFlush: table %s not updated\n" , dbt[i].name );
            continue;
        }
        // Spock end

        if( dbt[i].type == DB_INT_SORTED ){
            snprintf( filename, sizeof(filename),
                      "%s/int/%s", dir, dbt[i].name );
        } else {
            snprintf( filename, sizeof( filename),
                      "%s/string/%s", dir, dbt[i].name );
        }
        
        fp = fopen( filename, "w" );
        if( fp == NULL ){
            log( "cannot open file: %s %s\n", filename, strerror( errno ));
            continue;
        }

        // Spock 2000/10/19
        //entind = master_buf[dbt[i].toplinkindex].nextind;
        entind = master_buf[dbt[i].toplinkindex].next;
        //for(j=0;;j++){
        //    if( entind == -1 )break;
        while ( entind >= 0 )
        {
        // Spock end
            if( dbt[i].type == DB_INT_SORTED ){
                fprintf( fp , "%s %d %s\n", master_buf[entind].key,
                         master_buf[entind].ivalue,
                         //makeStringFromEscaped(
                         //    dbGetString(master_buf[entind].charvalue_index)));
                         // Spock fixed 2000/10/19
                         makeStringFromEscaped(master_buf[entind].charvalue));
            } else {
                fprintf( fp , "%s %s\n", master_buf[entind].key,
                         //makeStringFromEscaped(
                         //    dbGetString(master_buf[entind].charvalue_index)));
                         // Spock fixed 2000/10/19
                         makeStringFromEscaped(master_buf[entind].charvalue));
            }
            //entind = master_buf[entind].nextind;
            // Spock fixed 2000/10/19
            entind = master_buf[entind].next;
        }
        fclose(fp);
        dbt[i].updated = 0;
    }

    return 0;
}


int dbRead( char *dir )
{
    char dirname[1024];
    DIR *d;
    struct dirent *de;
    // Spock +1 2000/10/19
    memset( dbt , 0 , MAXTABLE * sizeof(struct table) );
    {
        char tmp[1024];
        snprintf( tmp, sizeof( tmp ), "%s/int" , dir );
        if( mkdir( tmp, 0755 )==0){
            log( "创建 %s\n", tmp );
        }
        snprintf( tmp, sizeof( tmp ), "%s/string" , dir );
        if( mkdir( tmp, 0755 )==0){
            log( "创建 %s\n", tmp );
        }        
    }
        
    snprintf( dirname, sizeof( dirname ),
              "%s/int" , dir );
    d = opendir(dirname);
    if( d == NULL ){
        log( "不能打开文件 %s\n", dirname );
        return -1;
    }

    while(1){
        de = readdir( d );
        if( de == NULL )break;
        if( de->d_name[0] != '.' ){
            char filename[1024];
            FILE *fp;
            struct stat s;
            snprintf( filename, sizeof(filename),"%s/%s",dirname, de->d_name );
			log( "读取数据:%s\n..", filename);
            if( stat( filename, &s ) < 0 ){
                continue;
            }
            if( !( s.st_mode & S_IFREG ) ){
                continue;
            }
            
            fp = fopen( filename, "r" );            
            if( fp == NULL ){
                log( "不能打开文件 %s %s\n",
                         filename, strerror( errno ));
                continue;
            }
            while(1){
                char line[1024];
                char k[1024] , v[1024], info[1024];
                if( fgets( line , sizeof( line) , fp ) == NULL )break;
                chop( line);
				k[0] = '\0';
                easyGetTokenFromString( line, 1, k, sizeof(k));
				v[0] = '\0';
                easyGetTokenFromString( line, 2, v, sizeof(v));
                info[0] = '\0';
                easyGetTokenFromString( line, 3, info, sizeof(info));
                dbUpdateEntryInt( de->d_name, k, atoi(v), info);
            }
            fclose(fp);
        }
    }
    closedir(d);
    snprintf( dirname, sizeof( dirname), "%s/string" , dir );
    d = opendir( dirname );
    if( d == NULL ){
        log( "不能打开文件 %s\n", dirname );
        return -1;
    }
    while(1){
        de = readdir( d );
        if( de == NULL )break;
        if( de->d_name[0] != '.' ){
            char filename[1024];
            FILE *fp;
            struct stat s;
            snprintf( filename, sizeof( filename),"%s/%s",dirname,de->d_name );
			log( "读取数据:%s\n..", filename);

            if( stat( filename, &s ) < 0 ){
                continue;
            }
            if( !(s.st_mode & S_IFREG )){
                continue;
            }
            fp = fopen( filename, "r" );
            if( fp == NULL ){
                log( "不能打开文件 %s %s\n",
                     filename, strerror(errno ));
                continue;
            }
            while(1){
                char line[CHARVALUE_MAX+1024];     
                char k[1024];
                if( fgets( line, sizeof( line), fp ) == NULL )break;
                /* chop */
                chop(line);
				k[0] = '\0';
                easyGetTokenFromString( line, 1, k,sizeof(k));
                dbUpdateEntryString( de->d_name, k, line+strlen(k)+1);
            }
            // Nuke +1 1027: Close for safe
            fclose(fp); 
        }
    }
    closedir(d);
    return 0;
}

/* 隙烂仄凶匏  井日隙烂仄凶蜊醒潸曰分允［
 撩  仄凶日  ｝岳  仄凶日0［岳  仄化手坞及请  及午五互丐月冗［
   “num互0及午五午井｝竟癫允月巨件玄伉互卅中午五［

 int 犯□正矛□旦毁迕分冗

 */
int dbGetEntryCountRange( char *table, int count_start, int  num,
                      char *output, int outlen )
{
    int dbi = dbGetTableIndex( table , DB_INT_SORTED );
    int cur;
    int i;
    int now_score = 0x7fffffff , r;

    if( dbi < 0) return -1;
    if( outlen < 1 ) return -1;
    output[0]=0;

    //cur = master_buf[dbt[dbi].toplinkindex].nextind;
    // Spock fixed 2000/10/19
    cur = master_buf[dbt[dbi].toplinkindex].next;
    i=0;
    r=0;
    for(;;){
        if( cur == -1 ) break;

        if( master_buf[cur].ivalue != now_score ){
            r=i;
            now_score = master_buf[cur].ivalue;
        }

        if( ( i >= count_start ) &&
            ( i < (count_start + num ) ) ){
            char tmp[1024];            
            if( (i !=count_start)){
                strcatsafe( output, outlen, "|" );
            } 
              
            snprintf( tmp, sizeof( tmp),
                      "%d,%d,%s,%d,%s", i, r, master_buf[cur].key,
                      master_buf[cur].ivalue,
			            //dbGetString( master_buf[cur].charvalue_index ));
		      // Spock fixed 2000/10/19
		      master_buf[cur].charvalue);
            strcatsafe( output, outlen,tmp );
        }
        i++;
        //cur = master_buf[cur].nextind;
        // Spock fixed 2000/10/19
        cur = master_buf[cur].next;
    }
    return 0;
}


/*
    侬  犯□正矛□旦及质  
 */
/* Spock deleted 2000/10/19
int
dbUpdateEntryString( char *table, char *key, char *value )
{
    int dbi = dbGetTableIndex(table, DB_STRING);
    int r, entind;
    
    log( "dbUpdateEntryString: [%s] [%s] [%s]\n", table, key, value );
    
    if( strlen(key) >= sizeof(master_buf[0].key) )return -1;
    if( dbi < 0 )return -1;

    r = dbExtractNodeByKey( dbt[dbi].toplinkindex, key );
    if( r< 0 ){
        log( "dbUpdateEntryString dbExtractNodeByKey fail! bug!!\n" );
        return -1;
    }

    entind = dbAllocNode( DB_STRING );
    if( entind < 0 ) return -1;

    master_buf[entind].ivalue = 0;
    dbSetString( master_buf[entind].charvalue_index, value );
    snprintf( master_buf[entind].key,
              sizeof(master_buf[0].key), "%s",key );
    master_buf[entind].keyhash = hashpjw( master_buf[entind].key );
    master_buf[entind].nextind = -1;

    if(  dbAppendNode( dbt[dbi].toplinkindex, entind ) < 0 ){
        log( "dbUpdateEntryString: dbAppendNode failed\n" );
        return -1;
    }
    log( "dbUpdateEntryString: successfully updated entry %s:%s:%s\n",
         table,key,value );

    return 0;
}
*/
// Spock 2000/10/19
int dbUpdateEntryString( char *table, char *key, char *value )
{
    int dbi = dbGetTableIndex( table , DB_STRING );
    int dbind, hashind;

    if ( strlen( key ) >= KEY_MAX ) {
    	log( "dbUpdateEntryString: key is too long, key:%s\n", key );
    	return -1;
    }
    if ( strlen( value ) >= CHARVALUE_MAX ) {
    	log( "dbUpdateEntryString: charvalue is too long, charvalue:%s\n", value );
    	return -1;
    }
    if ( dbi < 0 ) {
    	log( "dbUpdateEntryString: dbGetTableIndex fail, table:%s\n", table );
    	return -1;
    }
    hashind = tableGetEntry( dbi , key );
    if ( hashind < 0 )
    {
    	dbind = dbAllocNode();
    	if ( dbind < 0 )
    	{
    	    log( "dbUpdateEntryString: dbAllocNode fail\n" );
    	    return -1;
    	}
    	strcpy( master_buf[dbind].key , key );
    	strcpy( master_buf[dbind].charvalue , value );
    	if ( dbAppendNode( dbt[dbi].toplinkindex , dbind ) < 0 )
    	{
    	    master_buf[dbind].use = 0;
    	    log( "dbUpdateEntryString: dbAppendNode fail\n" );
    	    return -1;
    	}
    	if ( tableInsertNode( dbi , key , dbind ) < 0 )
    	{
    	    dbReleaseNode( dbind );
	    log( "dbUpdateEntryString: tableInsertNode fail\n" );
	    return -1;
	}
    }
    else
    {
    	dbind = dbt[dbi].hashtable[hashind].dbind;
    	strcpy( master_buf[dbind].charvalue , value );
    }
    dbt[dbi].updated = 1;
	/*
    log( "dbUpdateEntryString: successfully updated entry %s:%s:%s\n",
         table,key,value );
		 */
    return 0;
}
// Spock end

int
dbGetEntryString( char *table, char *key, char *output, int outlen )
{
    int dbi = dbGetTableIndex( table, DB_STRING );
    int entind;
    // Spock +1 2000/10/19
    int hashind;

    // Spock 2000/10/23
    //if( strlen(key) >= sizeof( master_buf[entind].key) ) return -1;
    if ( strlen(key) >= KEY_MAX ) {
    	log( "dbGetEntryString: key is too long, key:%s\n", key );
    	return -1;
    }
    if( dbi <0 ) {
    	log( "dbGetEntryString: dbGetTableIndex fail\n" );
    	return -1;
    }
    // Spock 2000/10/19
    hashind = tableGetEntry( dbi , key );
    if ( hashind < 0 ){
		log("err hashind <0\n")
		return -1;
	}
    entind = dbt[dbi].hashtable[hashind].dbind;

    if ( entind < 0 ){
		log( "entind < 0 ");
		return -1;
	}
    snprintf( output , outlen , "%s" , master_buf[entind].charvalue );

    return 0;
}

int
dbDeleteEntryString( char *table, char *key )
{
    int dbi = dbGetTableIndex( table, DB_STRING );
    int r;

    // Spock 2000/10/23
    //if( strlen(key) >= sizeof( master_buf[entind].key) ) return -1;
    if ( strlen(key) >= KEY_MAX ) {
    	log( "dbDeleteEntryString: key is too long, key:%s\n", key );
    	return -1;
    }
    if( dbi <0 ) {
    	log( "dbDeleteEntryString: dbGetTableIndex fail\n" );
    	return -1;
    }
    // Spock end
    //r = dbExtractNodeByKey( dbt[dbi].toplinkindex, key );
    // Spock fixed 2000/10/19
    r = dbExtractNodeByKey( dbi , key );
    if( r < 0 ){
        log( "dbDeleteEntryString: dbExtractNodeByKey failed for %s in %s\n",
             key,table );
        return -1;
    }
    dbt[dbi].updated = 1;
    log( "删除人物 %s 来至表 %s\n", key, table );
    return 0;
}
