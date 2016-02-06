#ifndef __ITEM_GEN_H__
#define __ITEM_GEN_H__


int ITEM_initItemIngCache( void );
int ITEM_initItemAtom( char *fn );
int ITEM_initRandTable( void);
int ITEM_mergeItem( int charaindex, ITEM_Item *items, int num , int money, int petid, int searchtable, int petindex, int alchemist);
int ITEM_canDigest( ITEM_Item *t );
int ITEM_mergeItem_merge( int charaindex,int petid, char *data, int petindex, int alchemist);

#ifdef _ITEM_INSLAY
int PETSKILL_ITEM_inslay( int charindex, int inslayindex, int itemindex);
#endif

#ifdef _PETSKILL_FIXITEM
int PETSKILL_ITEM_FixItem( int charindex, int fixindex, int *itemindex);
#endif

#endif


