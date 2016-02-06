#ifndef _MAKEPAIR_MAN_H
#define _MAKEPAIR_MAN_H

void NPC_MakePairManTalked( int meindex, int talkerindex, char *msg, int color );
BOOL NPC_MakePairManInit( int meindex );
void NPC_MakePairManWindowTalked( int meindex, int talkerindex, int seqno, int select, char *data);
void NPC_MakePairManLoop( int meindex);

typedef struct _tagPairMenu{
	char cdKey[256];
	char name[256];
	int pindex;
	int forindex;
	int code;
	int use;
}PairMenu;



#endif


