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

PROG=libbattle.a

SRC=battle.c battle_event.c battle_command.c battle_ai.c battle_magic.c pet_skill.c battle_item.c
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
battle.o: battle.c ../include/version.h ../include/correct_bug.h \
  ../include/buf.h ../include/common.h \
  ../include/object.h ../include/char.h ../include/char_base.h \
  ../include/version.h ../include/skill.h ../include/util.h \
  ../include/title.h ../include/addressbook.h \
  ../include/net.h \
  ../include/char_data.h ../include/item.h \
  ../include/char.h ../include/char_base.h ../include/char_data.h \
  ../include/battle.h ../include/battle_event.h \
  ../include/battle_command.h ../include/battle_ai.h \
  ../include/configfile.h ../include/lssproto_serv.h \
  ../include/lssproto_util.h \
  ../include/encount.h \
  ../include/enemy.h ../include/handletime.h ../include/readmap.h \
  ../include/pet_skill.h ../include/npcutil.h ../include/magic.h \
  ../include/npc_npcenemy.h ../include/log.h ../include/pet_skillinfo.h \
  ../include/anim_tbl.h ../include/common.h ../include/battle_magic.h \
  ../include/item.h ../include/correct_bug.h ../include/npc_autopk.h
battle_event.o: battle_event.c ../include/version.h \
  ../include/correct_bug.h \
  ../include/char.h \
  ../include/common.h \
  ../include/char_base.h ../include/version.h \
  ../include/skill.h ../include/util.h \
  ../include/title.h ../include/addressbook.h \
  ../include/net.h \
  ../include/char_data.h ../include/item.h \
  ../include/char.h ../include/char_event.h ../include/battle.h \
  ../include/battle_event.h ../include/configfile.h ../include/pet.h \
  ../include/log.h ../include/lssproto_serv.h ../include/lssproto_util.h \
  ../include/anim_tbl.h ../include/battle_magic.h \
  ../include/util.h ../include/enemy.h ../include/pet_skill.h \
  ../include/char_base.h ../include/item_event.h
battle_command.o: battle_command.c ../include/version.h \
  ../include/correct_bug.h \
  ../include/char.h \
  ../include/common.h \
  ../include/char_base.h ../include/version.h \
  ../include/skill.h ../include/util.h \
  ../include/title.h ../include/addressbook.h ../include/net.h \
  ../include/char_data.h ../include/item.h \
  ../include/char.h ../include/char_base.h ../include/battle.h \
  ../include/pet_skill.h ../include/battle_event.h \
  ../include/battle_command.h ../include/lssproto_serv.h \
  ../include/lssproto_util.h \
  ../include/magic.h \
  ../include/magic_base.h ../include/handletime.h
battle_ai.o: battle_ai.c ../include/version.h ../include/correct_bug.h \
  ../include/common.h \
  ../include/char.h ../include/common.h \
  ../include/char_base.h ../include/version.h ../include/skill.h \
  ../include/util.h \
  ../include/title.h ../include/addressbook.h \
  ../include/net.h \
  ../include/char_data.h ../include/item.h \
  ../include/char.h ../include/char_base.h ../include/battle.h \
  ../include/npcutil.h ../include/pet_skill.h
battle_magic.o: battle_magic.c ../include/version.h \
  ../include/correct_bug.h \
  ../include/char.h ../include/common.h \
  ../include/char_base.h ../include/version.h \
  ../include/skill.h ../include/util.h \
  ../include/title.h ../include/addressbook.h \
  ../include/net.h \
  ../include/char_data.h ../include/item.h \
  ../include/char.h ../include/char_base.h ../include/battle.h \
  ../include/battle_event.h ../include/magic_base.h \
  ../include/battle_magic.h ../include/item_event.h ../include/anim_tbl.h \
  ../include/common.h ../include/lssproto_serv.h \
  ../include/lssproto_util.h \
  /usr/include/getopt.h
pet_skill.o: pet_skill.c ../include/version.h ../include/correct_bug.h \
  ../include/common.h \
  ../include/buf.h ../include/common.h \
  ../include/configfile.h ../include/char_base.h ../include/version.h \
  ../include/skill.h ../include/util.h \
  ../include/title.h ../include/addressbook.h \
  ../include/net.h \
  ../include/char_data.h ../include/item.h \
  ../include/char.h ../include/char_base.h ../include/char_data.h \
  ../include/pet_skill.h ../include/battle.h ../include/battle_event.h \
  ../include/battle_magic.h ../include/item.h ../include/item_gen.h \
  ../include/char_talk.h
battle_item.o: battle_item.c ../include/version.h \
  ../include/correct_bug.h \
  ../include/char.h ../include/common.h \
  ../include/char_base.h ../include/version.h \
  ../include/skill.h ../include/util.h \
  ../include/title.h ../include/addressbook.h \
  ../include/net.h \
  ../include/char_data.h ../include/item.h \
  ../include/char.h ../include/char_base.h ../include/battle.h \
  ../include/battle_event.h ../include/battle_item.h \
  ../include/battle_magic.h ../include/item_event.h ../include/log.h \
  ../include/anim_tbl.h ../include/npcutil.h ../include/magic_base.h \
  ../include/lssproto_serv.h ../include/lssproto_util.h \
  /usr/include/getopt.h
