#ifndef __NPC_TRANSMIGRATION_H__
#define __NPC_TRANSMIGRATION_H__

void NPC_TransmigrationTalked( int meindex , int talkerindex , char *msg ,
                     int color );
BOOL NPC_TransmigrationInit( int meindex );
void NPC_TransmigrationWindowTalked( int meindex, int talkerindex, int seqno, int select, char *data);

#endif 

/*__NPC_TRANSMIGRATION_H__*/
