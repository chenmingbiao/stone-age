#ifndef __NPC_SYSINFO_H__
#define __NPC_SYSINFO_H__

BOOL NPC_SysinfoInit( int meindex );
void NPC_SysinfoLoop( int meindex );
void NPC_SysinfoTalked( int meindex, int tindex, char *msg, int color);
#endif /*__NPC_SYSINFO_H__*/
