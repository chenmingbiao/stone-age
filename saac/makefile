export CC=gcc
export RM=rm -f
export AR=ar cr
export MV=mv
export RANLIB=ranlib
export SED=sed

export SHELL=/bin/sh

#
# Edit by ringo 19980606
# 19990723
#
#
#

INCFLAGS=-I. -I./include

CFLAGS=-O3 -w $(INCFLAGS)
#Pentium 4 (Intel)
#CFLAGS=-march=pentium4 -O3 -pipe -fomit-frame-pointer $(INCFLAGS)
#Athlon XP (AMD)
#CFLAGS="-march=athlon-xp -O3 -pipe -fomit-frame-pointer" $(INCFLAGS)
#Athlon64：
#CFLAGS="-O2 -march=athlon64 -ftracer -fprefetch-loop-arrays -pipe" $(INCFLAGS)

#加入mysql资料库功能
MYSQL= -lz -L/usr/lib64/mysql -lmysqlclient

TOP=../..
PROG = ./saacjt.exe
SHELL = /bin/sh
PERL=/usr/bin/perl

RPCGEN=${TOP}/bin/lsgen.perl

RPCHTML = ${TOP}/doc/saacproto.html
#RPCSRC = saacproto_serv.c saacproto_util.csaacproto_sup.c 
RPCSRC = saacproto_serv.c saacproto_util.c 
RPCINCLUDE = saacproto_serv.h saacproto_util.h mac.h
RPCNOUSE = saacproto_cli.c saacproto_cli.h

#加入mysql资料库功能
SRC = main.c recv.c util.c char.c db.c mail.c acfamily.c lock.c sasql.c $(RPCSRC)
#SRC = main.c recv.c util.c char.c db.c mail.c acfamily.c lock.c $(RPCSRC)

OBJ=$(SRC:.c=.o)

${PROG} : $(RPCSRC) $(OBJ)
	$(CC) $(CFLAGS) -o $(PROG) $(OBJ) $(MYSQL)

dos2unix:
	dos2unix $(SRC) ./include/*.h makefile

chmod:
	chmod 777 $(SRC) ./include/*.h makefile

depend: $(RPCSRC) $(ANIMDATAFILE)
	$(MV) makefile makefile.bak
	$(SED) -ne '1,/^# DO NOT DELETE THIS LINE/p' makefile.bak>makefile
	$(CC) $(INCFLAGS) -M $(SRC) >> makefile 
	
clean:
	$(RM) $(PROG)
	$(RM) $(OBJ)
	$(RM) *~

distclean:
	$(RM) $(CLIRPCSRC)
	$(RM) $(CLIRPCINCLUDE)
	$(RM) $(SERVRPCSRC)
	$(RM) $(SERVRPCINCLUDE)
	$(RM) $(ANIMDATAFILE)
	$(RM) include/*~

	$(RM) $(PROG)
	$(RM) $(OBJ)
	$(RM) *~

	$(MV) Makefile Makefile.bak
	$(SED) -ne '1,/^# DO NOT DELETE THIS LINE/p' Makefile.bak>Makefile
	$(RM) Makefile.bak

rpc : ${RPCHTML} ${RPCGEN}
	$(PERL) ${RPCGEN} ${RPCHTML} -h . -o .
	rm -f ${RPCNOUSE}

# DO NOT DELETE THIS LINE
main.o: main.c ./include/main.h \
  ./include/version.h \
  ./include/util.h ./include/mail.h ./include/db.h \
  ./include/saacproto_util.h ./include/saacproto_serv.h \
  ./include/saacproto_util.h ./include/acfamily.h ./include/version.h \
  ./include/recv.h ./include/acfamily.h ./include/char.h \
  ./include/sasql.h \
  ./include/lock.h
recv.o: recv.c ./include/version.h ./include/main.h \
  ./include/version.h \
  ./include/saacproto_util.h ./include/saacproto_serv.h \
  ./include/saacproto_util.h ./include/recv.h ./include/acfamily.h \
  ./include/char.h ./include/db.h ./include/mail.h ./include/lock.h \
  ./include/util.h ./include/sasql.h ./include/acfamily.h
util.o: util.c \
  ./include/main.h ./include/version.h \
  ./include/util.h
char.o: char.c \
  ./include/main.h \
  ./include/version.h ./include/char.h ./include/saacproto_serv.h \
  ./include/saacproto_util.h ./include/util.h ./include/acfamily.h \
  ./include/version.h ./include/lock.h ./include/recv.h \
  ./include/acfamily.h
db.o: db.c ./include/db.h ./include/main.h \
  ./include/version.h \
  ./include/util.h ./include/version.h 
mail.o: mail.c ./include/main.h \
  ./include/version.h \
  ./include/mail.h ./include/util.h ./include/saacproto_serv.h \
  ./include/saacproto_util.h 
acfamily.o: acfamily.c ./include/version.h ./include/main.h \
  ./include/version.h \
  ./include/acfamily.h ./include/util.h ./include/saacproto_serv.h \
  ./include/saacproto_util.h 
lock.o: lock.c \
  ./include/lock.h \
  ./include/version.h ./include/main.h \
  ./include/char.h
sasql.o: sasql.c ./include/version.h ./include/main.h \
  ./include/version.h 
saacproto_serv.o: saacproto_serv.c ./include/version.h \
  ./include/saacproto_serv.h ./include/version.h \
  ./include/saacproto_util.h \
  ./include/main.h 
saacproto_util.o: saacproto_util.c ./include/version.h \
  ./include/main.h \
  ./include/version.h \
  ./include/saacproto_util.h
