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

PROG=libnpc.a

SRC=readnpc.c npcgen.c npctemplate.c npccreate.c npcutil.c \
npc_warp.c npc_townpeople.c \
npc_msg.c npc_healer.c npc_oldman.c \
npc_storyteller.c npc_dengon.c npc_doorman.c \
npc_npcenemy.c npc_itemchange.c \
npc_action.c npc_windowman.c npc_savepoint.c \
npc_windowhealer.c npc_itemshop.c npc_sysinfo.c \
npc_duelranking.c npc_petskillshop.c npc_petshop.c \
npc_signboard.c npc_warpman.c npc_exchangeman.c \
npc_timeman.c npc_bodylan.c npc_mic.c \
npc_luckyman.c npc_bus.c npc_charm.c npc_quiz.c \
npc_poolitemshop.c npc_checkman.c npc_janken.c \
npc_transmigration.c npc_makepair.c \
npc_familyman.c npc_fmdengon.c npc_fmwarpman.c \
npc_fmhealer.c npc_bankman.c npc_fmpkman.c\
npc_airplane.c npc_scheduleman.c npc_fmpkcallman.c\
npc_manorsman.c npc_riderman.c npc_fmletter.c npc_petmaker.c \
npc_gamblemaster.c npc_gambleroulette.c npc_petfusion.c\
npc_newnpcman.c npc_gamblebank.c \
npc_petracemaster.c npc_petracepet.c npc_freepetskillshop.c \
npc_eventaction.c npc_vipshop.c npc_autopk.c npc_vippoint.c\
npc_transerman.c npc_alldoman.c\

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
readnpc.o: readnpc.c \
  ../include/version.h ../include/correct_bug.h \
  ../include/common.h \
  ../include/npctemplate.h ../include/util.h \
  ../include/common.h ../include/char_base.h \
  ../include/version.h ../include/skill.h ../include/title.h \
  ../include/addressbook.h ../include/net.h \
  ../include/npccreate.h
npcgen.o: npcgen.c ../include/version.h ../include/correct_bug.h \
  ../include/common.h \
  ../include/npccreate.h ../include/util.h \
  ../include/common.h ../include/npctemplate.h \
  ../include/char_base.h ../include/version.h ../include/skill.h \
  ../include/title.h ../include/addressbook.h ../include/net.h \
  ../include/char.h ../include/char_data.h \
  ../include/item.h ../include/char.h ../include/char_data.h \
  ../include/buf.h ../include/object.h ../include/readmap.h \
  ../include/item.h ../include/handletime.h ../include/map_deal.h
npctemplate.o: npctemplate.c ../include/version.h \
  ../include/correct_bug.h \
  ../include/common.h \
  ../include/npctemplate.h ../include/util.h \
  ../include/common.h ../include/char_base.h \
  ../include/version.h ../include/skill.h ../include/title.h \
  ../include/addressbook.h ../include/net.h \
  ../include/buf.h \
  ../include/char_data.h ../include/item.h ../include/char.h \
  ../include/char_data.h ../include/util.h ../include/handletime.h \
  ../include/item.h ../include/anim_tbl.h ../include/configfile.h
npccreate.o: npccreate.c ../include/version.h ../include/correct_bug.h \
  ../include/common.h \
  ../include/npccreate.h ../include/util.h \
  ../include/common.h ../include/npctemplate.h \
  ../include/char_base.h ../include/version.h ../include/skill.h \
  ../include/title.h ../include/addressbook.h ../include/net.h \
  ../include/util.h ../include/buf.h \
  ../include/readmap.h ../include/char_data.h ../include/item.h \
  ../include/char.h ../include/char_data.h ../include/handletime.h \
  ../include/configfile.h
npcutil.o: npcutil.c ../include/version.h ../include/correct_bug.h \
  ../include/common.h \
  ../include/char.h ../include/common.h \
  ../include/char_base.h ../include/version.h ../include/skill.h \
  ../include/util.h \
  ../include/title.h ../include/addressbook.h \
  ../include/net.h \
  ../include/char_data.h ../include/item.h \
  ../include/char.h ../include/char_base.h ../include/configfile.h \
  ../include/object.h ../include/item.h ../include/map_deal.h \
  ../include/npcutil.h ../include/npc_door.h ../include/readmap.h \
  ../include/npccreate.h ../include/enemy.h ../include/family.h \
  ../include/saacproto_cli.h ../include/saacproto_util.h \
  /usr/include/getopt.h
npc_warp.o: npc_warp.c ../include/version.h ../include/correct_bug.h \
  ../include/object.h ../include/common.h \
  ../include/char_base.h ../include/version.h \
  ../include/skill.h ../include/util.h \
  ../include/title.h ../include/addressbook.h \
  ../include/net.h \
  ../include/char.h ../include/char_base.h \
  ../include/char_data.h ../include/item.h ../include/char.h \
  ../include/map_deal.h ../include/readmap.h ../include/npccreate.h \
  ../include/npcutil.h ../include/handletime.h \
  ../include/npc_eventaction.h
npc_townpeople.o: npc_townpeople.c ../include/version.h \
  ../include/correct_bug.h ../include/object.h ../include/common.h \
  ../include/char_base.h ../include/version.h ../include/skill.h \
  ../include/util.h \
  ../include/title.h ../include/addressbook.h \
  ../include/net.h \
  ../include/char.h ../include/char_base.h \
  ../include/char_data.h ../include/item.h ../include/char.h \
  ../include/util.h ../include/handletime.h ../include/anim_tbl.h \
  ../include/npc_door.h ../include/lssproto_serv.h \
  ../include/lssproto_util.h \
  ../include/npcutil.h
npc_msg.o: npc_msg.c ../include/version.h ../include/correct_bug.h \
  ../include/char.h ../include/common.h \
  ../include/char_base.h ../include/version.h ../include/skill.h \
  ../include/util.h \
  ../include/title.h ../include/addressbook.h \
  ../include/net.h \
  ../include/char_data.h ../include/item.h \
  ../include/char.h ../include/char_base.h ../include/npcutil.h
npc_healer.o: npc_healer.c ../include/version.h ../include/correct_bug.h \
  ../include/object.h \
  ../include/common.h \
  ../include/char_base.h ../include/version.h ../include/skill.h \
  ../include/util.h \
  ../include/title.h ../include/addressbook.h \
  ../include/net.h \
  ../include/char.h ../include/char_base.h \
  ../include/char_data.h ../include/item.h ../include/char.h \
  ../include/util.h ../include/npcutil.h ../include/item.h \
  ../include/readmap.h
npc_oldman.o: npc_oldman.c ../include/version.h ../include/correct_bug.h \
  ../include/object.h ../include/common.h \
  ../include/char_base.h ../include/version.h ../include/skill.h \
  ../include/util.h \
  ../include/title.h ../include/addressbook.h \
  ../include/net.h \
  ../include/char.h ../include/char_base.h \
  ../include/char_data.h ../include/item.h ../include/char.h \
  ../include/util.h ../include/npcutil.h ../include/npc_oldman.h \
  ../include/title.h ../include/char_data.h ../include/readmap.h
npc_storyteller.o: npc_storyteller.c ../include/version.h \
  ../include/correct_bug.h ../include/char.h ../include/common.h \
  ../include/char_base.h ../include/version.h ../include/skill.h \
  ../include/util.h \
  ../include/title.h ../include/addressbook.h \
  ../include/net.h \
  ../include/char_data.h ../include/item.h \
  ../include/char.h
npc_dengon.o: npc_dengon.c ../include/version.h ../include/correct_bug.h \
  ../include/char.h ../include/common.h \
  ../include/char_base.h ../include/version.h ../include/skill.h \
  ../include/util.h \
  ../include/title.h \
  ../include/addressbook.h ../include/net.h \
  ../include/char_data.h \
  ../include/item.h ../include/char.h ../include/object.h \
  ../include/lssproto_serv.h ../include/lssproto_util.h \
  ../include/npcutil.h ../include/handletime.h
npc_doorman.o: npc_doorman.c ../include/version.h \
  ../include/correct_bug.h \
  ../include/object.h ../include/common.h \
  ../include/char_base.h ../include/version.h \
  ../include/skill.h ../include/util.h \
  ../include/title.h ../include/addressbook.h \
  ../include/net.h \
  ../include/char.h ../include/char_base.h \
  ../include/char_data.h ../include/item.h ../include/char.h \
  ../include/item.h ../include/util.h ../include/handletime.h \
  ../include/npc_doorman.h ../include/npc_door.h ../include/npcutil.h \
  ../include/configfile.h
npc_npcenemy.o: npc_npcenemy.c ../include/version.h \
  ../include/correct_bug.h \
  ../include/char.h ../include/common.h \
  ../include/char_base.h ../include/version.h \
  ../include/skill.h ../include/util.h \
  ../include/title.h ../include/addressbook.h \
  ../include/net.h \
  ../include/char_data.h ../include/item.h \
  ../include/char.h ../include/object.h ../include/char_base.h \
  ../include/npcutil.h ../include/npc_npcenemy.h ../include/battle.h \
  ../include/enemy.h ../include/readmap.h ../include/encount.h \
  ../include/lssproto_serv.h ../include/lssproto_util.h \
  ../include/configfile.h ../include/anim_tbl.h \
  ../include/handletime.h ../include/npc_eventaction.h
npc_itemchange.o: npc_itemchange.c ../include/version.h \
  ../include/correct_bug.h \
  ../include/char.h ../include/common.h \
  ../include/char_base.h ../include/version.h \
  ../include/skill.h ../include/util.h \
  ../include/title.h ../include/addressbook.h \
  ../include/net.h \
  ../include/char_data.h ../include/item.h \
  ../include/char.h ../include/object.h ../include/char_base.h \
  ../include/npcutil.h ../include/lssproto_serv.h \
  ../include/lssproto_util.h \
  ../include/saacproto_cli.h ../include/saacproto_util.h \
  ../include/readmap.h ../include/battle.h ../include/log.h \
  ../include/enemy.h ../include/handletime.h ../include/npc_eventaction.h \
  ../include/npc_itemchange.h
npc_action.o: npc_action.c ../include/version.h ../include/correct_bug.h \
  ../include/char.h ../include/common.h \
  ../include/char_base.h ../include/version.h \
  ../include/skill.h ../include/util.h \
  ../include/title.h ../include/addressbook.h \
  ../include/net.h \
  ../include/char_data.h ../include/item.h \
  ../include/char.h ../include/object.h ../include/char_base.h \
  ../include/npcutil.h ../include/npc_action.h
npc_windowman.o: npc_windowman.c ../include/version.h \
  ../include/correct_bug.h \
  ../include/char.h ../include/common.h \
  ../include/char_base.h ../include/version.h \
  ../include/skill.h ../include/util.h \
  ../include/title.h ../include/addressbook.h \
  ../include/net.h \
  ../include/char_data.h ../include/item.h \
  ../include/char.h ../include/object.h ../include/char_base.h \
  ../include/npcutil.h ../include/configfile.h ../include/lssproto_serv.h \
  ../include/lssproto_util.h \
  ../include/npc_windowman.h
npc_savepoint.o: npc_savepoint.c ../include/version.h \
  ../include/correct_bug.h \
  ../include/object.h ../include/common.h \
  ../include/char_base.h ../include/version.h \
  ../include/skill.h ../include/util.h \
  ../include/title.h ../include/addressbook.h \
  ../include/net.h \
  ../include/char.h ../include/char_base.h \
  ../include/char_data.h ../include/item.h ../include/char.h \
  ../include/util.h ../include/npcutil.h ../include/npc_oldman.h \
  ../include/title.h ../include/char_data.h ../include/readmap.h \
  ../include/lssproto_serv.h ../include/lssproto_util.h \
  ../include/log.h
npc_windowhealer.o: npc_windowhealer.c ../include/version.h \
  ../include/correct_bug.h \
  ../include/char.h ../include/common.h \
  ../include/char_base.h ../include/version.h \
  ../include/skill.h ../include/util.h \
  ../include/title.h ../include/addressbook.h \
  ../include/net.h \
  ../include/char_data.h ../include/item.h \
  ../include/char.h ../include/object.h ../include/char_base.h \
  ../include/npcutil.h ../include/lssproto_serv.h \
  ../include/lssproto_util.h \
  ../include/npc_windowhealer.h
npc_itemshop.o: npc_itemshop.c ../include/version.h \
  ../include/correct_bug.h \
  ../include/char.h ../include/common.h \
  ../include/char_base.h ../include/version.h \
  ../include/skill.h ../include/util.h \
  ../include/title.h ../include/addressbook.h \
  ../include/net.h \
  ../include/char_data.h ../include/item.h \
  ../include/char.h ../include/object.h ../include/char_base.h \
  ../include/npcutil.h ../include/lssproto_serv.h \
  ../include/lssproto_util.h \
  ../include/pet_skill.h \
  ../include/npc_eventaction.h ../include/readmap.h ../include/log.h \
  ../include/family.h
npc_sysinfo.o: npc_sysinfo.c ../include/version.h \
  ../include/correct_bug.h \
  ../include/char.h ../include/common.h \
  ../include/char_base.h ../include/version.h \
  ../include/skill.h ../include/util.h \
  ../include/title.h ../include/addressbook.h \
  ../include/net.h \
  ../include/char_data.h ../include/item.h \
  ../include/char.h ../include/char_event.h ../include/object.h \
  ../include/handletime.h ../include/magic.h ../include/npcutil.h \
  ../include/char_data.h ../include/chatmagic.h ../include/net.h \
  ../include/configfile.h ../include/npcgen.h
npc_duelranking.o: npc_duelranking.c ../include/version.h \
  ../include/correct_bug.h \
  ../include/char.h ../include/common.h \
  ../include/char_base.h ../include/version.h \
  ../include/skill.h ../include/util.h \
  ../include/title.h ../include/addressbook.h \
  ../include/net.h \
  ../include/char_data.h ../include/item.h \
  ../include/char.h ../include/object.h ../include/char_base.h \
  ../include/npcutil.h ../include/net.h ../include/lssproto_serv.h \
  ../include/lssproto_util.h \
  ../include/saacproto_cli.h ../include/saacproto_util.h \
  ../include/npc_duelranking.h ../include/handletime.h
npc_petskillshop.o: npc_petskillshop.c ../include/version.h \
  ../include/correct_bug.h \
  ../include/char.h ../include/common.h \
  ../include/char_base.h ../include/version.h \
  ../include/skill.h ../include/util.h \
  ../include/title.h ../include/addressbook.h \
  ../include/net.h \
  ../include/char_data.h ../include/item.h \
  ../include/char.h ../include/object.h ../include/char_base.h \
  ../include/npcutil.h ../include/lssproto_serv.h \
  ../include/lssproto_util.h \
  ../include/pet_skill.h \
  ../include/readmap.h ../include/battle.h
npc_petshop.o: npc_petshop.c ../include/version.h \
  ../include/correct_bug.h \
  ../include/char.h ../include/common.h \
  ../include/char_base.h ../include/version.h \
  ../include/skill.h ../include/util.h \
  ../include/title.h ../include/addressbook.h \
  ../include/net.h \
  ../include/char_data.h ../include/item.h \
  ../include/char.h ../include/object.h ../include/char_base.h \
  ../include/npcutil.h ../include/lssproto_serv.h \
  ../include/lssproto_util.h \
  ../include/pet_skill.h \
  ../include/readmap.h ../include/battle.h ../include/log.h
npc_signboard.o: npc_signboard.c ../include/version.h \
  ../include/correct_bug.h \
  ../include/char.h ../include/common.h \
  ../include/char_base.h ../include/version.h \
  ../include/skill.h ../include/util.h \
  ../include/title.h ../include/addressbook.h \
  ../include/net.h \
  ../include/char_data.h ../include/item.h \
  ../include/char.h ../include/object.h ../include/char_base.h \
  ../include/npcutil.h ../include/lssproto_serv.h \
  ../include/lssproto_util.h \
  ../include/readmap.h \
  ../include/family.h
npc_warpman.o: npc_warpman.c ../include/version.h \
  ../include/correct_bug.h \
  ../include/char.h ../include/common.h \
  ../include/char_base.h ../include/version.h \
  ../include/skill.h ../include/util.h \
  ../include/title.h ../include/addressbook.h \
  ../include/net.h \
  ../include/char_data.h ../include/item.h \
  ../include/char.h ../include/object.h ../include/char_base.h \
  ../include/npcutil.h ../include/lssproto_serv.h \
  ../include/lssproto_util.h \
  ../include/saacproto_cli.h ../include/saacproto_util.h \
  ../include/npc_warpman.h ../include/map_deal.h ../include/readmap.h \
  ../include/battle.h ../include/npc_exchangeman.h ../include/log.h \
  ../include/enemy.h ../include/handletime.h ../include/npc_eventaction.h
npc_exchangeman.o: npc_exchangeman.c ../include/version.h \
  ../include/correct_bug.h \
  ../include/char.h ../include/common.h \
  ../include/char_base.h ../include/version.h \
  ../include/skill.h ../include/util.h \
  ../include/title.h ../include/addressbook.h \
  ../include/net.h \
  ../include/char_data.h ../include/item.h \
  ../include/char.h ../include/object.h ../include/net.h \
  ../include/char_base.h ../include/npcutil.h ../include/lssproto_serv.h \
  ../include/lssproto_util.h \
  ../include/npc_exchangeman.h ../include/npc_eventaction.h \
  ../include/log.h ../include/battle.h ../include/handletime.h \
  ../include/enemy.h ../include/npc_warp.h
npc_timeman.o: npc_timeman.c ../include/version.h \
  ../include/correct_bug.h \
  ../include/object.h ../include/common.h \
  ../include/char_base.h ../include/version.h \
  ../include/skill.h ../include/util.h \
  ../include/title.h ../include/addressbook.h \
  ../include/net.h \
  ../include/char.h ../include/char_base.h \
  ../include/char_data.h ../include/item.h ../include/char.h \
  ../include/util.h ../include/npcutil.h ../include/npc_oldman.h \
  ../include/title.h ../include/char_data.h ../include/readmap.h \
  ../include/lssproto_serv.h ../include/lssproto_util.h \
  ../include/log.h ../include/handletime.h
npc_bodylan.o: npc_bodylan.c ../include/version.h \
  ../include/correct_bug.h \
  ../include/char.h ../include/common.h \
  ../include/char_base.h ../include/version.h \
  ../include/skill.h ../include/util.h \
  ../include/title.h ../include/addressbook.h \
  ../include/net.h \
  ../include/char_data.h ../include/item.h \
  ../include/char.h ../include/object.h ../include/char_base.h \
  ../include/npcutil.h ../include/lssproto_serv.h \
  ../include/lssproto_util.h \
  ../include/npc_windowhealer.h
npc_mic.o: npc_mic.c ../include/version.h ../include/correct_bug.h \
  ../include/common.h \
  ../include/char_base.h ../include/version.h \
  ../include/skill.h ../include/common.h ../include/util.h \
  ../include/title.h ../include/addressbook.h \
  ../include/net.h \
  ../include/char.h ../include/char_base.h \
  ../include/char_data.h ../include/item.h ../include/char.h \
  ../include/npcutil.h ../include/npc_mic.h ../include/saacproto_cli.h \
  ../include/saacproto_util.h \
  ../include/lssproto_serv.h ../include/lssproto_util.h \
  ../include/battle.h
npc_luckyman.o: npc_luckyman.c ../include/version.h \
  ../include/correct_bug.h \
  ../include/char.h ../include/common.h \
  ../include/char_base.h ../include/version.h \
  ../include/skill.h ../include/util.h \
  ../include/title.h ../include/addressbook.h \
  ../include/net.h \
  ../include/char_data.h ../include/item.h \
  ../include/char.h ../include/object.h ../include/char_base.h \
  ../include/npcutil.h ../include/lssproto_serv.h \
  ../include/lssproto_util.h \
  ../include/npc_luckyman.h
npc_bus.o: npc_bus.c ../include/version.h ../include/correct_bug.h \
  ../include/char.h ../include/common.h \
  ../include/char_base.h ../include/version.h \
  ../include/skill.h ../include/util.h \
  ../include/title.h ../include/addressbook.h \
  ../include/net.h \
  ../include/char_data.h ../include/item.h \
  ../include/char.h ../include/object.h ../include/char_base.h \
  ../include/npcutil.h ../include/lssproto_serv.h \
  ../include/lssproto_util.h \
  ../include/npc_bus.h \
  ../include/handletime.h
npc_charm.o: npc_charm.c ../include/version.h ../include/correct_bug.h \
  ../include/char.h ../include/common.h \
  ../include/char_base.h ../include/version.h \
  ../include/skill.h ../include/util.h \
  ../include/title.h ../include/addressbook.h \
  ../include/net.h \
  ../include/char_data.h ../include/item.h \
  ../include/char.h ../include/object.h ../include/char_base.h \
  ../include/npcutil.h ../include/lssproto_serv.h \
  ../include/lssproto_util.h \
  ../include/npc_charm.h
npc_quiz.o: npc_quiz.c ../include/version.h ../include/correct_bug.h \
  ../include/common.h \
  ../include/char_base.h ../include/version.h \
  ../include/skill.h ../include/common.h ../include/util.h \
  ../include/title.h ../include/addressbook.h \
  ../include/net.h \
  ../include/npc_quiz.h ../include/npcutil.h \
  ../include/char.h ../include/char_base.h ../include/char_data.h \
  ../include/item.h ../include/char.h ../include/lssproto_serv.h \
  ../include/lssproto_util.h \
  ../include/buf.h \
  ../include/function.h ../include/readmap.h ../include/object.h \
  ../include/log.h
npc_poolitemshop.o: npc_poolitemshop.c ../include/version.h \
  ../include/correct_bug.h ../include/char.h ../include/common.h \
  ../include/char_base.h ../include/version.h ../include/skill.h \
  ../include/util.h \
  ../include/title.h ../include/addressbook.h \
  ../include/net.h \
  ../include/char_data.h ../include/item.h \
  ../include/char.h ../include/lssproto_serv.h ../include/lssproto_util.h \
  ../include/npcutil.h ../include/npc_poolitemshop.h ../include/log.h \
  ../include/handletime.h
npc_checkman.o: npc_checkman.c ../include/version.h \
  ../include/correct_bug.h \
  ../include/char.h ../include/common.h \
  ../include/char_base.h ../include/version.h \
  ../include/skill.h ../include/util.h \
  ../include/title.h ../include/addressbook.h \
  ../include/net.h \
  ../include/char_data.h ../include/item.h \
  ../include/char.h ../include/object.h ../include/char_base.h \
  ../include/npcutil.h ../include/lssproto_serv.h \
  ../include/lssproto_util.h \
  /usr/include/getopt.h
npc_janken.o: npc_janken.c ../include/version.h ../include/correct_bug.h \
  ../include/common.h \
  ../include/char_base.h ../include/version.h \
  ../include/skill.h ../include/common.h ../include/util.h \
  ../include/title.h ../include/addressbook.h \
  ../include/net.h \
  ../include/npc_janken.h ../include/npcutil.h \
  ../include/char.h ../include/char_base.h ../include/char_data.h \
  ../include/item.h ../include/char.h ../include/lssproto_serv.h \
  ../include/lssproto_util.h \
  ../include/buf.h \
  ../include/function.h ../include/readmap.h ../include/object.h \
  ../include/log.h
npc_transmigration.o: npc_transmigration.c ../include/version.h \
  ../include/correct_bug.h \
  ../include/char.h \
  ../include/common.h \
  ../include/char_base.h ../include/version.h \
  ../include/skill.h ../include/util.h \
  ../include/title.h ../include/addressbook.h \
  ../include/net.h \
  ../include/char_data.h ../include/item.h \
  ../include/char.h ../include/object.h ../include/char_base.h \
  ../include/npcutil.h ../include/lssproto_serv.h \
  ../include/lssproto_util.h \
  ../include/enemy.h \
  ../include/log.h ../include/battle.h ../include/readmap.h \
  ../include/npc_transmigration.h ../include/npc_pettransman.h
npc_makepair.o: npc_makepair.c
npc_familyman.o: npc_familyman.c ../include/version.h \
  ../include/correct_bug.h \
  ../include/char.h ../include/common.h \
  ../include/char_base.h ../include/version.h \
  ../include/skill.h ../include/util.h \
  ../include/title.h ../include/addressbook.h \
  ../include/net.h \
  ../include/char_data.h ../include/item.h \
  ../include/char.h ../include/object.h ../include/char_base.h \
  ../include/npcutil.h ../include/configfile.h ../include/lssproto_serv.h \
  ../include/lssproto_util.h \
  ../include/saacproto_cli.h ../include/saacproto_util.h \
  ../include/npc_familyman.h ../include/family.h
npc_fmdengon.o: npc_fmdengon.c ../include/version.h \
  ../include/correct_bug.h \
  ../include/char.h ../include/common.h \
  ../include/char_base.h ../include/version.h ../include/skill.h \
  ../include/util.h \
  ../include/title.h \
  ../include/addressbook.h ../include/net.h \
  ../include/char_data.h \
  ../include/item.h ../include/char.h ../include/object.h \
  ../include/lssproto_serv.h ../include/lssproto_util.h \
  ../include/saacproto_cli.h \
  ../include/saacproto_util.h ../include/npcutil.h \
  ../include/handletime.h ../include/npc_fmdengon.h ../include/family.h
npc_fmwarpman.o: npc_fmwarpman.c ../include/version.h \
  ../include/correct_bug.h \
  ../include/char.h ../include/common.h \
  ../include/char_base.h ../include/version.h ../include/skill.h \
  ../include/util.h \
  ../include/title.h \
  ../include/addressbook.h ../include/net.h \
  ../include/char_data.h \
  ../include/item.h ../include/char.h ../include/object.h \
  ../include/char_base.h ../include/npcutil.h ../include/lssproto_serv.h \
  ../include/lssproto_util.h \
  ../include/saacproto_cli.h ../include/saacproto_util.h \
  ../include/npc_fmwarpman.h ../include/npc_scheduleman.h \
  ../include/readmap.h ../include/log.h ../include/battle.h \
  ../include/handletime.h ../include/family.h ../include/configfile.h
npc_fmhealer.o: npc_fmhealer.c ../include/version.h \
  ../include/correct_bug.h \
  ../include/object.h \
  ../include/common.h \
  ../include/char_base.h ../include/version.h ../include/skill.h \
  ../include/util.h \
  ../include/title.h ../include/addressbook.h \
  ../include/net.h \
  ../include/char.h ../include/char_base.h \
  ../include/char_data.h ../include/item.h ../include/char.h \
  ../include/util.h ../include/npcutil.h ../include/item.h \
  ../include/readmap.h
npc_bankman.o: npc_bankman.c ../include/version.h \
  ../include/correct_bug.h \
  ../include/char.h ../include/common.h \
  ../include/char_base.h ../include/version.h \
  ../include/skill.h ../include/util.h \
  ../include/title.h ../include/addressbook.h \
  ../include/net.h \
  ../include/char_data.h ../include/item.h \
  ../include/char.h ../include/object.h ../include/char_base.h \
  ../include/npcutil.h ../include/configfile.h ../include/lssproto_serv.h \
  ../include/lssproto_util.h \
  ../include/saacproto_cli.h ../include/saacproto_util.h \
  ../include/npc_bankman.h ../include/family.h
npc_fmpkman.o: npc_fmpkman.c ../include/version.h \
  ../include/correct_bug.h \
  ../include/char.h ../include/common.h \
  ../include/char_base.h ../include/version.h \
  ../include/skill.h ../include/util.h \
  ../include/title.h ../include/addressbook.h \
  ../include/net.h \
  ../include/char_data.h ../include/item.h \
  ../include/char.h ../include/object.h ../include/char_base.h \
  ../include/npcutil.h ../include/lssproto_serv.h \
  ../include/lssproto_util.h \
  ../include/npc_fmpkman.h ../include/npc_scheduleman.h \
  ../include/npc_fmwarpman.h ../include/family.h ../include/readmap.h \
  ../include/battle.h ../include/log.h
npc_airplane.o: npc_airplane.c ../include/version.h \
  ../include/correct_bug.h \
  ../include/char.h ../include/common.h \
  ../include/char_base.h ../include/version.h \
  ../include/skill.h ../include/util.h \
  ../include/title.h ../include/addressbook.h \
  ../include/net.h \
  ../include/char_data.h ../include/item.h \
  ../include/char.h ../include/object.h ../include/char_base.h \
  ../include/npcutil.h ../include/lssproto_serv.h \
  ../include/lssproto_util.h \
  ../include/npc_airplane.h ../include/handletime.h
npc_scheduleman.o: npc_scheduleman.c ../include/version.h \
  ../include/correct_bug.h \
  ../include/char.h ../include/common.h \
  ../include/char_base.h ../include/version.h ../include/skill.h \
  ../include/util.h \
  ../include/title.h \
  ../include/addressbook.h ../include/net.h \
  ../include/char_data.h \
  ../include/item.h ../include/char.h ../include/object.h \
  ../include/lssproto_serv.h ../include/lssproto_util.h \
  ../include/npcutil.h ../include/handletime.h \
  ../include/npc_scheduleman.h ../include/family.h
npc_fmpkcallman.o: npc_fmpkcallman.c ../include/version.h \
  ../include/correct_bug.h \
  ../include/char.h ../include/common.h \
  ../include/char_base.h ../include/version.h \
  ../include/skill.h ../include/util.h \
  ../include/title.h ../include/addressbook.h \
  ../include/net.h \
  ../include/char_data.h ../include/item.h \
  ../include/char.h ../include/object.h ../include/char_base.h \
  ../include/npcutil.h ../include/lssproto_serv.h \
  ../include/lssproto_util.h \
  ../include/npc_fmpkcallman.h ../include/family.h ../include/readmap.h \
  ../include/battle.h ../include/log.h ../include/npc_scheduleman.h \
  ../include/handletime.h
npc_manorsman.o: npc_manorsman.c ../include/version.h \
  ../include/correct_bug.h \
  ../include/char.h ../include/common.h \
  ../include/char_base.h ../include/version.h ../include/skill.h \
  ../include/util.h \
  ../include/title.h \
  ../include/addressbook.h ../include/net.h \
  ../include/char_data.h \
  ../include/item.h ../include/char.h ../include/object.h \
  ../include/lssproto_serv.h ../include/lssproto_util.h \
  ../include/npcutil.h ../include/handletime.h \
  ../include/npc_scheduleman.h ../include/family.h \
  ../include/npc_manorsman.h ../include/saacproto_cli.h \
  ../include/saacproto_util.h ../include/net.h ../include/configfile.h \
  ../include/log.h
npc_riderman.o: npc_riderman.c ../include/version.h \
  ../include/correct_bug.h \
  ../include/char.h ../include/common.h \
  ../include/char_base.h ../include/version.h \
  ../include/skill.h ../include/util.h \
  ../include/title.h ../include/addressbook.h \
  ../include/net.h \
  ../include/char_data.h ../include/item.h \
  ../include/char.h ../include/object.h ../include/char_base.h \
  ../include/npcutil.h ../include/configfile.h ../include/lssproto_serv.h \
  ../include/lssproto_util.h \
  ../include/saacproto_cli.h ../include/saacproto_util.h \
  ../include/npc_riderman.h ../include/family.h ../include/log.h
npc_fmletter.o: npc_fmletter.c ../include/version.h \
  ../include/correct_bug.h \
  ../include/char.h ../include/common.h \
  ../include/char_base.h ../include/version.h \
  ../include/skill.h ../include/util.h \
  ../include/title.h ../include/addressbook.h \
  ../include/net.h \
  ../include/char_data.h ../include/item.h \
  ../include/char.h ../include/object.h ../include/char_base.h \
  ../include/npcutil.h ../include/configfile.h ../include/lssproto_serv.h \
  ../include/lssproto_util.h \
  ../include/saacproto_cli.h ../include/saacproto_util.h \
  ../include/npc_fmletter.h ../include/family.h ../include/log.h
npc_petmaker.o: npc_petmaker.c ../include/version.h \
  ../include/correct_bug.h \
  ../include/object.h \
  ../include/common.h \
  ../include/char_base.h ../include/version.h ../include/skill.h \
  ../include/util.h \
  ../include/title.h ../include/addressbook.h \
  ../include/net.h \
  ../include/char.h ../include/char_base.h \
  ../include/char_data.h ../include/item.h ../include/char.h \
  ../include/util.h ../include/npcutil.h ../include/item.h \
  ../include/readmap.h ../include/enemy.h ../include/chatmagic.h \
  ../include/log.h
npc_gamblemaster.o: npc_gamblemaster.c ../include/version.h \
  ../include/correct_bug.h \
  ../include/char.h ../include/common.h \
  ../include/char_base.h ../include/version.h \
  ../include/skill.h ../include/util.h \
  ../include/title.h ../include/addressbook.h \
  ../include/net.h \
  ../include/char_data.h ../include/item.h \
  ../include/char.h ../include/object.h ../include/char_base.h \
  ../include/npcutil.h ../include/lssproto_serv.h \
  ../include/lssproto_util.h \
  ../include/handletime.h \
  ../include/readmap.h ../include/log.h ../include/npc_eventaction.h \
  ../include/npc_gamblemaster.h
npc_gambleroulette.o: npc_gambleroulette.c ../include/version.h \
  ../include/correct_bug.h \
  ../include/char.h ../include/common.h \
  ../include/char_base.h ../include/version.h \
  ../include/skill.h ../include/util.h \
  ../include/title.h ../include/addressbook.h \
  ../include/net.h \
  ../include/char_data.h ../include/item.h \
  ../include/char.h ../include/object.h ../include/char_base.h \
  ../include/npcutil.h ../include/readmap.h ../include/lssproto_serv.h \
  ../include/lssproto_util.h \
  ../include/handletime.h \
  ../include/npc_gambleroulette.h
npc_petfusion.o: npc_petfusion.c
npc_newnpcman.o: npc_newnpcman.c ../include/version.h \
  ../include/correct_bug.h \
  ../include/char.h \
  ../include/common.h \
  ../include/char_base.h ../include/version.h \
  ../include/skill.h ../include/util.h \
  ../include/title.h ../include/addressbook.h \
  ../include/net.h \
  ../include/char_data.h ../include/item.h \
  ../include/char.h ../include/object.h ../include/char_base.h \
  ../include/npcutil.h ../include/lssproto_serv.h \
  ../include/lssproto_util.h \
  ../include/enemy.h \
  ../include/log.h ../include/battle.h ../include/readmap.h \
  ../include/handletime.h ../include/npc_eventaction.h \
  ../include/npc_newnpcman.h ../include/pet.h
npc_gamblebank.o: npc_gamblebank.c ../include/version.h \
  ../include/correct_bug.h \
  ../include/char.h \
  ../include/common.h \
  ../include/char_base.h ../include/version.h \
  ../include/skill.h ../include/util.h \
  ../include/title.h ../include/addressbook.h \
  ../include/net.h \
  ../include/char_data.h ../include/item.h \
  ../include/char.h ../include/object.h ../include/char_base.h \
  ../include/npcutil.h ../include/lssproto_serv.h \
  ../include/lssproto_util.h \
  ../include/enemy.h \
  ../include/log.h ../include/battle.h ../include/readmap.h \
  ../include/handletime.h ../include/configfile.h \
  ../include/npc_gamblebank.h
npc_petracemaster.o: npc_petracemaster.c ../include/version.h \
  ../include/correct_bug.h \
  ../include/char.h ../include/common.h \
  ../include/char_base.h ../include/version.h ../include/skill.h \
  ../include/util.h \
  ../include/title.h \
  ../include/addressbook.h ../include/net.h \
  ../include/char_data.h \
  ../include/item.h ../include/char.h ../include/object.h \
  ../include/char_base.h ../include/npcutil.h ../include/lssproto_serv.h \
  ../include/lssproto_util.h \
  ../include/handletime.h \
  ../include/npc_petracemaster.h
npc_petracepet.o: npc_petracepet.c ../include/version.h \
  ../include/correct_bug.h \
  ../include/char.h ../include/common.h \
  ../include/char_base.h ../include/version.h \
  ../include/skill.h ../include/util.h \
  ../include/title.h ../include/addressbook.h \
  ../include/net.h \
  ../include/char_data.h ../include/item.h \
  ../include/char.h ../include/object.h ../include/char_base.h \
  ../include/npcutil.h ../include/lssproto_serv.h \
  ../include/lssproto_util.h \
  ../include/handletime.h \
  ../include/npc_petracepet.h
npc_freepetskillshop.o: npc_freepetskillshop.c ../include/version.h \
  ../include/correct_bug.h \
  ../include/char.h ../include/common.h \
  ../include/char_base.h ../include/version.h \
  ../include/skill.h ../include/util.h \
  ../include/title.h ../include/addressbook.h \
  ../include/net.h \
  ../include/char_data.h ../include/item.h \
  ../include/char.h ../include/object.h ../include/char_base.h \
  ../include/npcutil.h ../include/lssproto_serv.h \
  ../include/lssproto_util.h \
  ../include/pet_skill.h \
  ../include/readmap.h ../include/battle.h ../include/configfile.h \
  ../include/util.h ../include/npc_eventaction.h \
  ../include/npc_freepetskillshop.h
npc_eventaction.o: npc_eventaction.c ../include/version.h \
  ../include/correct_bug.h \
  ../include/char.h ../include/common.h \
  ../include/char_base.h ../include/version.h \
  ../include/skill.h ../include/util.h \
  ../include/title.h ../include/addressbook.h \
  ../include/net.h \
  ../include/char_data.h ../include/item.h \
  ../include/char.h ../include/object.h ../include/char_base.h \
  ../include/npcutil.h ../include/lssproto_serv.h \
  ../include/lssproto_util.h \
  ../include/saacproto_cli.h ../include/saacproto_util.h \
  ../include/map_deal.h ../include/readmap.h ../include/battle.h \
  ../include/log.h ../include/enemy.h ../include/handletime.h \
  ../include/npc_eventaction.h ../include/family.h
npc_vipshop.o: npc_vipshop.c ../include/version.h \
  ../include/correct_bug.h \
  ../include/char.h ../include/common.h \
  ../include/char_base.h ../include/version.h \
  ../include/skill.h ../include/util.h \
  ../include/title.h ../include/addressbook.h \
  ../include/net.h \
  ../include/char_data.h ../include/item.h \
  ../include/char.h ../include/object.h ../include/char_base.h \
  ../include/npcutil.h ../include/npc_vipshop.h \
  ../include/lssproto_serv.h ../include/lssproto_util.h \
  ../include/saacproto_cli.h \
  ../include/saacproto_util.h ../include/readmap.h ../include/battle.h \
  ../include/log.h ../include/enemy.h ../include/handletime.h \
  ../include/npc_eventaction.h
npc_autopk.o: npc_autopk.c ../include/version.h ../include/correct_bug.h \
  ../include/char.h ../include/common.h \
  ../include/char_base.h ../include/version.h \
  ../include/skill.h ../include/util.h \
  ../include/title.h ../include/addressbook.h \
  ../include/net.h \
  ../include/char_data.h ../include/item.h \
  ../include/char.h ../include/object.h ../include/char_base.h \
  ../include/npcutil.h ../include/npc_autopk.h ../include/lssproto_serv.h \
  ../include/lssproto_util.h \
  ../include/saacproto_cli.h ../include/saacproto_util.h \
  ../include/readmap.h ../include/battle.h ../include/log.h \
  ../include/enemy.h ../include/handletime.h ../include/npc_eventaction.h \
  ../include/npc_healer.h ../include/configfile.h
npc_vippoint.o: npc_vippoint.c ../include/version.h \
  ../include/correct_bug.h \
  ../include/char.h ../include/common.h \
  ../include/char_base.h ../include/version.h \
  ../include/skill.h ../include/util.h \
  ../include/title.h ../include/addressbook.h \
  ../include/net.h \
  ../include/char_data.h ../include/item.h \
  ../include/char.h ../include/object.h ../include/char_base.h \
  ../include/npcutil.h ../include/npc_vipshop.h \
  ../include/lssproto_serv.h ../include/lssproto_util.h \
  ../include/saacproto_cli.h \
  ../include/saacproto_util.h ../include/readmap.h ../include/battle.h \
  ../include/log.h ../include/enemy.h ../include/handletime.h \
  ../include/npc_eventaction.h
npc_transerman.o: npc_transerman.c ../include/version.h \
  ../include/correct_bug.h \
  ../include/char.h ../include/common.h \
  ../include/char_base.h ../include/version.h \
  ../include/skill.h ../include/util.h \
  ../include/title.h ../include/addressbook.h \
  ../include/net.h \
  ../include/char_data.h ../include/item.h \
  ../include/char.h ../include/object.h ../include/char_base.h \
  ../include/npcutil.h ../include/npc_transerman.h \
  ../include/lssproto_serv.h ../include/lssproto_util.h \
  ../include/saacproto_cli.h \
  ../include/saacproto_util.h ../include/readmap.h ../include/battle.h \
  ../include/log.h ../include/enemy.h ../include/handletime.h \
  ../include/npc_eventaction.h
npc_alldoman.o: npc_alldoman.c ../include/version.h \
  ../include/correct_bug.h \
  ../include/object.h \
  ../include/common.h \
  ../include/char_base.h ../include/version.h ../include/skill.h \
  ../include/util.h \
  ../include/title.h ../include/addressbook.h \
  ../include/net.h \
  ../include/char.h ../include/char_base.h \
  ../include/char_data.h ../include/item.h ../include/char.h \
  ../include/util.h ../include/npcutil.h ../include/item.h \
  ../include/readmap.h ../include/enemy.h ../include/chatmagic.h \
  ../include/log.h ../include/npc_itemchange.h ../include/lssproto_serv.h \
  ../include/lssproto_util.h \
  ../include/saacproto_cli.h ../include/saacproto_util.h \
  ../include/npc_eventaction.h ../include/npc_charm.h \
  ../include/npc_alldoman.h
