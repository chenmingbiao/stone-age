#ifndef _ITEM_CHANGE_
#define _ITEM_CHANGE_

//#ifdef _CFREE_petskill
BOOL NPC_ItemchangeManInit( int meindex );
void NPC_ItemchangeManTalked( int meindex, int talkerindex, char *msg, int color );
void NPC_ItemchangeManWindowTalked( int meindex, int talkerindex, int seqno,
								int select, char *data);
void NPC_ItemchangeManLoop( int meindex);

#endif

