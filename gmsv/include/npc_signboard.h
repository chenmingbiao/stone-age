#ifndef __NPC_SIGNBOARD_H__
#define __NPC_SIGNBOARD_H__


BOOL NPC_SignBoardInit( int meindex );
void NPC_SignBoardWindowTalked( int meindex, int talkerindex, int seqno, int select, char *data);
void NPC_SignBoardLooked( int meindex , int lookedindex);


#endif
 /*__NPC_WINDOWSIGNBOAD_H__*/
