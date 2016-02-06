#ifndef __NPC_FREEPETSKILLSHOP_H__
#define __NPC_FREEPETSKILLSHOP_H__

BOOL NPC_FreePetSkillShopInit( int meindex );
void NPC_FreePetSkillShopTalked( int meindex , int talkerindex , char *msg , int color );
void NPC_FreePetSkillShopWindowTalked( int meindex, int talkerindex, int seqno, int select, char *data);
//void NPC_PetSkillShopLooked( int meindex , int lookedindex);
BOOL NPC_CHECKFREEPETSKILL( int toindex, int petindex, int skillID);
#endif

