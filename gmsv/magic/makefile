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
#Athlon64£º
#CFLAGS="-O2 -march=athlon64 -ftracer -fprefetch-loop-arrays -pipe" $(INCFLAGS)

PROG=libmagic.a

SRC=magic.c magic_base.c magic_field.c
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
magic.o: magic.c ../include/version.h ../include/correct_bug.h \
  ../include/common.h \
  ../include/char_base.h ../include/version.h \
  ../include/skill.h ../include/common.h ../include/util.h \
  ../include/title.h ../include/addressbook.h \
  ../include/net.h \
  ../include/char_data.h ../include/item.h \
  ../include/char.h ../include/char_base.h ../include/char_data.h \
  ../include/item.h ../include/battle.h ../include/magic_base.h \
  ../include/magic.h ../include/magic_field.h ../include/family.h \
  ../include/handletime.h ../include/battle_magic.h \
  ../include/battle_event.h ../include/lssproto_serv.h \
  ../include/lssproto_util.h \
  ../include/pet_skill.h
magic_base.o: magic_base.c ../include/version.h ../include/correct_bug.h \
  ../include/common.h \
  ../include/buf.h ../include/common.h \
  ../include/configfile.h ../include/magic_base.h ../include/util.h \
  ../include/magic.h
magic_field.o: magic_field.c ../include/version.h \
  ../include/correct_bug.h \
  ../include/common.h \
  ../include/char_base.h ../include/version.h \
  ../include/skill.h ../include/common.h ../include/util.h \
  ../include/title.h ../include/addressbook.h \
  ../include/net.h \
  ../include/char.h ../include/char_base.h \
  ../include/char_data.h ../include/item.h ../include/char.h \
  ../include/magic_base.h ../include/magic.h ../include/magic_field.h
