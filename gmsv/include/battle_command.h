#ifndef __BATTLE_COMMAND_H__
#define __BATTLE_COMMAND_H__

void BattleCommandDispach(
	int fd,
	char *command
);

void BattleEncountOut(
	int charaindex
);

BOOL BATTLE_CommandSend( int charaindex, char *pszCommand );

BOOL BATTLE_MakeCharaString(
	int battleindex,
	char *pszCommand,	// Æ½ÅÒ·ÂÛÍÕı¡õÊ÷  Çë  ÛÆ
	int	size 			// ÈÓÄÌÊõ
);

void BATTLE_CharSendAll( int battleindex );

BOOL	BATTLE_PetDefaultCommand( int petindex );

void BATTLE_ActSettingSend( int battleindex );

#endif
