#ifndef __NPC_WINDOWMAN_H__
#define __NPC_WINDOWMAN_H__

void NPC_WindowmanTalked( int meindex , int talkerindex , char *msg ,
                     int color );

BOOL NPC_WindowmanInit( int meindex );
void NPC_WindowmanLooked( int meindex, int lookedindex );
void NPC_WindowmanWindowTalked( int meindex, int talkerindex, int seqno, int select, char *data);

void NPC_Windowman_selectWindow( int meindex, int toindex, int num);
BOOL NPC_Windowman_readData( int meindex, int windowno, BOOL chkflg);
int NPC_Windowman_restoreButtontype( char *data );

#endif 
/*__NPC_WINDOWMAN_H__*/
