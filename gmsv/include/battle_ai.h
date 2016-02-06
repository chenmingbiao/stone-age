#ifndef __BATTLE_AI_H__
#define __BATTLE_AI_H__

/*   Ô»éø */
int BATTLE_ai_all( int battleindex, int side, int turn);
int BATTLE_ai_one( int charaindex, int battleindex, int side, int turn);
#ifdef _ENEMY_ATTACK_AI
int GetSubdueAtt(int index);
#endif
#endif
