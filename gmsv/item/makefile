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

PROG=libitem.a

SRC=item.c item_event.c item_gen.c
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
	$(MV) makefile Makefile.bak
	$(SED) -ne '1,/^# DO NOT DELETE THIS LINE/p' makefile.bak>makefile
	$(RM) makefile.bak

# DO NOT DELETE THIS LINE
item.o: item.c ../include/version.h ../include/correct_bug.h \
  ../include/common.h \
  ../include/char.h ../include/common.h ../include/char_base.h \
  ../include/version.h ../include/skill.h ../include/util.h \
  ../include/title.h ../include/addressbook.h ../include/net.h \
  ../include/char_data.h ../include/item.h \
  ../include/char.h ../include/char_data.h ../include/battle.h \
  ../include/item.h ../include/buf.h ../include/function.h \
  ../include/magic_base.h
item_event.o: item_event.c ../include/version.h ../include/correct_bug.h \
  ../include/common.h \
  ../include/char.h ../include/common.h \
  ../include/char_base.h ../include/version.h ../include/skill.h \
  ../include/util.h \
  ../include/title.h ../include/addressbook.h \
  ../include/net.h \
  ../include/char_data.h ../include/item.h \
  ../include/char.h ../include/char_base.h ../include/char_data.h \
  ../include/configfile.h ../include/item.h ../include/item_event.h \
  ../include/object.h ../include/anim_tbl.h ../include/magic.h \
  ../include/handletime.h ../include/log.h ../include/util.h \
  ../include/encount.h ../include/battle.h ../include/battle_item.h \
  ../include/lssproto_serv.h ../include/lssproto_util.h \
  ../include/net.h ../include/pet.h \
  ../include/npcutil.h ../include/enemy.h ../include/battle_magic.h \
  ../include/pet_skill.h ../include/npc_freepetskillshop.h
item_gen.o: item_gen.c ../include/version.h ../include/correct_bug.h \
  ../include/configfile.h ../include/common.h \
  ../include/util.h \
  ../include/buf.h \
  ../include/char.h ../include/char_base.h ../include/version.h \
  ../include/skill.h ../include/util.h ../include/title.h \
  ../include/addressbook.h ../include/net.h \
  ../include/char_data.h \
  ../include/item.h ../include/char.h ../include/item.h \
  ../include/item_gen.h ../include/enemy.h ../include/log.h \
  ../include/saacproto_cli.h ../include/saacproto_util.h \
  ../include/family.h ../include/magic_base.h
