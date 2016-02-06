#ifndef __NPC_PETSKILLSHOP_H__
#define __NPC_PETSKILLSHOP_H__

void NPC_PetSkillShopTalked( int meindex , int talkerindex , char *msg ,
                     int color );
BOOL NPC_PetSkillShopInit( int meindex );
void NPC_PetSkillShopWindowTalked( int meindex, int talkerindex, int seqno, int select, char *data);
void NPC_PetSkillShopLooked( int meindex , int lookedindex);

void NPC_PetSkillMakeStr(int meindex,int toindex,int select);
		
#endif
 /*__NPC_WINDOWPETSKILLSHOP_H__*/
