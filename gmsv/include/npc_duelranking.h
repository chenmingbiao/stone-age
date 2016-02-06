#ifndef __NPC_DUELRANKING_H__
#define __NPC_DUELRANKING_H__

BOOL NPC_DuelrankingInit( int meindex );
void NPC_DuelrankingLooked( int meindex, int lookedindex );
void NPC_DuelrankingWindowTalked( int meindex, int talkerindex, int seqno, int select, char *data);
void NPC_Duelranking_PrintRanking( char *data, int count_start, int fdid, int msgid2);
void NPC_Duelranking_CallMyRanking( int count, int msgid, int msgid2);

#endif

