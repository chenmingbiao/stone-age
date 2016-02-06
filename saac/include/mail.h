#ifndef _MAIL_H_
#define _MAIL_H_

void receiveMail( char *id_from, char *charname_from,
                  char *id_to, char *charname_to, char *message, int option ,
                  int use_msgid, unsigned int msgid );
void receiveMailAck( char *id, char *charname, int a , int mesgid );
void flushMail( int fd, char *id, char *charname );
void expireMail();
int readMail( char *dir );
#endif


