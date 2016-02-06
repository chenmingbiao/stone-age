#ifndef __NPC_SAVEPOINT_H__
#define __NPC_SAVEPOINT_H__


void NPC_SavePointTalked( int meindex , int talkerindex , char *msg ,
                     int color );

BOOL NPC_SavePointInit( int meindex );
void NPC_SavePointWindowTalked( int meindex, int talkerindex, int seqno, int select, char *data);


#endif 

/*__NPC_SAVEPOINT_H__*/
