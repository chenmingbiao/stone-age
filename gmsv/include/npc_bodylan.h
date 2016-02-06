#ifndef __NPC_BODYLAN_H__
#define __NPC_BODYLAN_H__

void NPC_BodyLanTalked( int meindex , int talkerindex , char *msg ,
                     int color );
BOOL NPC_BodyLanInit( int meindex );
void NPC_BodyLanWatch( int meindex , int lookedindex);
void NPC_BodyLanWindowTalked(
	int meindex,
	int talkerindex,
	int seqno,
	int select,
	char *data
);

#endif 
