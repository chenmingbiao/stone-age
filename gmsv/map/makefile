export CC=gcc
export RM=rm -f
export AR=ar cr
export MV=mv
export RANLIB=ranlib
export SED=sed


export SHELL=/bin/sh

INCFLAGS=-I. -I.. -I../include

CFLAGS=-O3 -w $(INCFLAGS) 
#Pentium 4 (Intel)
#CFLAGS=-march=pentium4 -O3 -pipe -fomit-frame-pointer $(INCFLAGS)
#Athlon XP (AMD)
#CFLAGS="-march=athlon-xp -O3 -pipe -fomit-frame-pointer" $(INCFLAGS)
#Athlon64��
#CFLAGS="-O2 -march=athlon64 -ftracer -fprefetch-loop-arrays -pipe" $(INCFLAGS)

PROG=libmap.a

SRC=readmap.c map_deal.c map_util.c map_warppoint.c
OBJ=$(SRC:.c=.o)

ifeq (0,$(MAKELEVEL))
CC=gcc
RM=rm -f
AR=ar cr
MV=mv
RANLIB=ranlib
SED=sed
SHELL=/bin/sh
endif

all: $(PROG)

$(PROG): $(OBJ)
	$(RM) $(PROG)
	$(AR) $(PROG) $(OBJ)
	$(RANLIB) $(PROG)
	
dos2unix:
	dos2unix $(SRC) makefile

chmod:
	chmod 777 $(SRC) makefile
	
depend:
	$(MV) makefile makefile.bak
	$(SED) -ne '1,/^# DO NOT DELETE THIS LINE/p' makefile.bak>makefile
	$(CC) $(INCFLAGS) -M $(SRC) >> makefile 

clean:
	$(RM) $(PROG)
	$(RM) $(OBJ)
	$(RM) *~

distclean:
	$(RM) $(PROG)
	$(RM) $(OBJ)
	$(RM) *~
	$(MV) makefile makefile.bak
	$(SED) -ne '1,/^# DO NOT DELETE THIS LINE/p' makefile.bak>makefile
	$(RM) makefile.bak

# DO NOT DELETE THIS LINE
readmap.o: readmap.c ../include/version.h ../include/correct_bug.h \
  ../include/common.h ../include/object.h \
  ../include/common.h ../include/readmap.h ../include/util.h \
  ../include/buf.h ../include/util.h \
  ../include/char.h ../include/char_base.h ../include/version.h \
  ../include/skill.h ../include/title.h ../include/addressbook.h \
  ../include/net.h \
  ../include/char_data.h ../include/item.h ../include/char.h \
  ../include/lssproto_serv.h ../include/lssproto_util.h \
  ../include/configfile.h
map_deal.o: map_deal.c ../include/version.h ../include/correct_bug.h \
  ../include/common.h \
  ../include/readmap.h ../include/common.h ../include/util.h \
  ../include/char.h ../include/char_base.h \
  ../include/version.h ../include/skill.h ../include/title.h \
  ../include/addressbook.h ../include/net.h \
  ../include/char_data.h \
  ../include/item.h ../include/char.h ../include/map_util.h \
  ../include/lssproto_serv.h ../include/lssproto_util.h \
  ../include/net.h ../include/anim_tbl.h
map_util.o: map_util.c ../include/version.h ../include/correct_bug.h \
  ../include/common.h \
  ../include/char.h ../include/common.h ../include/char_base.h \
  ../include/version.h ../include/skill.h ../include/util.h \
  ../include/title.h ../include/addressbook.h \
  ../include/net.h \
  ../include/char_data.h ../include/item.h \
  ../include/char.h ../include/readmap.h
map_warppoint.o: map_warppoint.c ../include/version.h \
  ../include/correct_bug.h \
  ../include/common.h \
  ../include/char.h ../include/common.h \
  ../include/char_base.h ../include/version.h ../include/skill.h \
  ../include/util.h \
  ../include/title.h ../include/addressbook.h \
  ../include/net.h \
  ../include/char_data.h ../include/item.h \
  ../include/char.h ../include/char_base.h ../include/object.h \
  ../include/buf.h ../include/util.h ../include/configfile.h \
  ../include/readmap.h ../include/map_warppoint.h
