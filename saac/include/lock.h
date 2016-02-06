#ifndef _LOCK_H_
#define _LOCK_H_

#include "version.h"

typedef struct tagLockNode {
  int use;
  char cdkey[16];
  char server[128];
  int process;
  struct tagLockNode *next,*prev;
} LockNode;

extern LockNode **userlock;

void Lock_Init(void);
int InsertMemLock(int entry, char *cdkey, char *passwd, char *server, int process, char *deadline);
int DeleteMemLock(int entry, char *cdkey, int *process);
void DeleteMemLockServer(char *sname);
int isMemLocked(int entry, char *cdkey);
int GetMemLockState(int entry, char *cdkey, char *result);
int GetMemLockServer(int entry, char *cdkey, char *result);

int LockNode_getGname( int entries, char *id, char *gname);

int ChangeMemDeadline(int entry, char *cdkey, char *deadline);

#endif

