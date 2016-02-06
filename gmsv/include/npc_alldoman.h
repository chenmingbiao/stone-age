#ifndef __NPC_ALLDOMAN_H__
#define __NPC_ALLDOMAN_H__

BOOL NPC_AlldomanInit( int meindex );
void NPC_AlldomanTalked( int meindex , int talker , char *msg ,int color );
void NPC_AlldomanWindowTalked(int meindex, int talkerindex, int seqno, int select,char *data);
void NPC_AlldomanWriteStele( char *token ) ; 
void NPC_Alldoman_S_WriteStele( char *ocdkey , char *oname , char *ncdkey , 
			  char *nname , char *title	, int level , int trns , int floor ) ;
#endif
