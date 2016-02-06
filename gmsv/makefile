export CC=gcc
export RM=rm -f
export AR=ar cr
export MV=mv
export RANLIB=ranlib
export SED=sed


export SHELL=/bin/sh

# USE_MITO 反穴伙民旦伊永玉IO毛银丹方丹卞允月＄1¤7indy匹戊件由奶伙允月午五叄1¤7
# pthread 反骚日卅中及匹｝仇木毛反内仄化pthread 手反内允仇午

INCFLAGS=-I. -I./include

CFLAGS=-O3 -w $(INCFLAGS)
#Pentium 4 (Intel)
#CFLAGS=-march=pentium4 -O3 -pipe -fomit-frame-pointer $(INCFLAGS)
#Athlon XP (AMD)
#CFLAGS="-march=athlon-xp -O3 -pipe -fomit-frame-pointer" $(INCFLAGS)
#Athlon64：
#CFLAGS="-O2 -march=athlon64 -ftracer -fprefetch-loop-arrays -pipe" $(INCFLAGS)

PROG=./gmsvjt.exe

#TOP=../..
TOP=..

PERL=/usr/bin/env perl

SIMPLEEX=$(TOP)/bin/simpleex.perl

#RPC
RPCGEN=~/bin/lsgen.perl
CLIRPCHTML=~/doc/saacproto.html
CLIRPCSRC=saacproto_cli.c saacproto_util.c
CLIRPCINCLUDE=include/saacproto_cli.h include/saacproto_util.h
CLIRPCNOUSE=saacproto_serv.c include/saacproto_serv.h
CLIRPCOPTION=-h include -o . -euc 

SERVRPCHTML=~/doc/lssproto.html
SERVRPCSRC=lssproto_serv.c lssproto_util.c
SERVRPCINCLUDE=include/lssproto_serv.h include/lssproto_util.h
SERVRPCNOUSE=lssproto_cli.c include/lssproto_cli.h
SERVRPCOPTION= -h include -o . -euc -e

RPCSRC=$(CLIRPCSRC) $(SERVRPCSRC)

#PETSKILL
PETSKILLGEN=~/bin/makedefs.perl
PETSKILLTXT=data/petskill.txt
PETSKILLOPTION= -name 8 -value 4 -o

#CHAR_ANIM
ANIMDATAGEN=~/bin/convertStringtographicnumber.perl
ANIMDATATXT=include/anim_tbl.h
ANIMDATAFILE=char/ls2data.h


SUBDIR=char npc map item battle magic
LIBS=char/libchar.a npc/libnpc.a map/libmap.a\
item/libitem.a \
magic/libmagic.a \
battle/libbattle.a \

SRC=main.c init.c configfile.c util.c net.c buf.c msignal.c \
callfromac.c callfromcli.c object.c handletime.c function.c log.c autil.c \
$(CLIRPCSRC) $(SERVRPCSRC)

OBJ=$(SRC:.c=.o)


LDFLAGS=-lm -lpthread
#LDFLAGS=-lm


ifeq ($(shell uname),Linux)
	LDFLAGS+=
endif
ifeq ($(shell uname),SunOS)
	LDFLAGS+=-lnsl -lsocket
endif
ifeq ($(shell uname),FreeBSD)
	LDFLAGS+=
endif

all: $(PROG)

$(PROG):  $(RPCSRC) $(ANIMDATAFILE) $(LIBS) $(OBJ) 
	$(CC) $(CFLAGS) -o $(PROG) $(OBJ) $(LIBS) $(LDFLAGS)

dos2unix:
	@for dir in $(SUBDIR) ; do \
		$(MAKE) -C $$dir dos2unix;\
	done
	dos2unix ./include/*.h $(SRC) makefile
	
chmod:
	@for dir in $(SUBDIR) ; do \
		$(MAKE) -C $$dir chmod;\
	done
	chmod 777 ./include/*.h $(SRC) makefile
	
depend: $(RPCSRC) $(ANIMDATAFILE)
	@for dir in $(SUBDIR) ; do \
		$(MAKE) -C $$dir depend;\
	done
	$(MV) makefile makefile.bak
	$(SED) -ne '1,/^# DO NOT DELETE THIS LINE/p' makefile.bak>makefile
	$(CC) $(INCFLAGS) -M $(SRC) >> makefile 

clean:
	@for dir in $(SUBDIR) ; do \
		$(MAKE) -C $$dir clean;\
	done
	$(RM) $(PROG)
	$(RM) $(OBJ)
	$(RM) *~

distclean:
	@for dir in $(SUBDIR) ; do \
		$(MAKE) -C $$dir distclean;\
	done
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

data:	$(RPCSRC) $(ANIMDATAFILE)

$(CLIRPCSRC): #$(RPCGEN) $(CLIRPCHTML) $(CLIRPCGEN)
#	$(PERL) $(RPCGEN)  $(CLIRPCOPTION) $(CLIRPCHTML)
#	$(RM) $(CLIRPCNOUSE)

$(SERVRPCSRC): #$(RPCGEN) $(SERVRPCHTML) $(SERVRPCGEN)
#	$(PERL) $(RPCGEN) $(SERVRPCOPTION) $(SERVRPCHTML)
#	$(RM) $(SERVRPCNOUSE)

$(ANIMDATAFILE): #$(ANIMDATAGEN) $(ANIMDATATXT)
#	$(PERL) $(ANIMDATAGEN) $(ANIMDATATXT) > $(ANIMDATAFILE)

$(LIBS):
	@target=`echo $@ | $(SED) s/\\\\/.\\*//`; \
	$(MAKE) -C $$target;


jpex :
	$(PERL) $(SIMPLEEX) *.c */*.c | nkf -s > jp_literal.sjis

# DO NOT DELETE THIS LINE
main.o: main.c ./include/version.h ./include/correct_bug.h \
  ./include/handletime.h ./include/char.h ./include/char_base.h \
  ./include/version.h ./include/skill.h ./include/util.h \
  ./include/net.h ./include/char_data.h ./include/item.h ./include/char.h \
  ./include/map_warppoint.h ./include/npcgen.h ./include/log.h \
  ./include/configfile.h ./include/battle.h ./include/petmail.h \
  ./include/autil.h ./include/family.h ./include/trade.h
init.o: init.c ./include/version.h ./include/correct_bug.h \
  ./include/configfile.h ./include/common.h  \
  ./include/buf.h ./include/object.h ./include/char.h \
  ./include/char_base.h ./include/version.h ./include/skill.h \
  ./include/util.h ./include/title.h ./include/addressbook.h \
  ./include/net.h ./include/char_data.h ./include/item.h ./include/char.h \
  ./include/char_data.h ./include/item.h ./include/readmap.h \
  ./include/function.h ./include/saacproto_cli.h \
  ./include/saacproto_util.h ./include/lssproto_serv.h \
  ./include/lssproto_util.h ./include/readnpc.h ./include/log.h \
  ./include/handletime.h ./include/title.h ./include/encount.h \
  ./include/enemy.h ./include/battle.h ./include/magic_base.h \
  ./include/pet_skill.h ./include/item_gen.h ./include/petmail.h \
  ./include/npc_quiz.h ./include/init.h
configfile.o: configfile.c ./include/version.h ./include/correct_bug.h \
  ./include/configfile.h ./include/net.h  \
  ./include/char.h ./include/char_base.h ./include/version.h \
  ./include/skill.h ./include/util.h ./include/title.h \
  ./include/addressbook.h ./include/net.h ./include/char_data.h \
  ./include/item.h ./include/char.h ./include/char_data.h \
  ./include/lssproto_serv.h ./include/lssproto_util.h
util.o: util.c ./include/version.h ./include/correct_bug.h \
  ./include/common.h ./include/util.h ./include/common.h \
  ./include/configfile.h
net.o: net.c ./include/version.h ./include/correct_bug.h \
  ./include/common.h  \
  ./include/buf.h ./include/common.h ./include/msignal.h \
  ./include/configfile.h ./include/util.h ./include/saacproto_cli.h \
  ./include/version.h ./include/saacproto_util.h  \
  ./include/lssproto_serv.h ./include/lssproto_util.h ./include/char.h \
  ./include/char_base.h ./include/skill.h ./include/util.h \
  ./include/title.h ./include/addressbook.h ./include/net.h \
  ./include/char_data.h ./include/item.h ./include/char.h \
  ./include/handletime.h ./include/log.h ./include/object.h \
  ./include/item_event.h ./include/enemy.h ./include/battle.h \
  ./include/pet_event.h ./include/char_talk.h ./include/petmail.h \
  ./include/npc_autopk.h
buf.o: buf.c ./include/version.h ./include/correct_bug.h \
  ./include/common.h  \
  ./include/handletime.h  
msignal.o: msignal.c ./include/version.h ./include/correct_bug.h \
  ./include/net.h \
  ./include/char_base.h ./include/version.h ./include/skill.h \
  ./include/util.h   \
  ./include/net.h ./include/char_data.h ./include/item.h ./include/char.h \
  ./include/item.h ./include/object.h ./include/configfile.h \
  ./include/lssproto_serv.h ./include/lssproto_util.h \
  ./include/saacproto_cli.h ./include/saacproto_util.h ./include/log.h \
  ./include/petmail.h
callfromac.o: callfromac.c \
  ./include/common.h  \
  ./include/saacproto_cli.h ./include/version.h \
  ./include/saacproto_util.h \
  ./include/lssproto_serv.h ./include/lssproto_util.h ./include/net.h \
  ./include/common.h  \
  ./include/skill.h ./include/util.h ./include/title.h \
  ./include/addressbook.h ./include/net.h ./include/char_data.h \
  ./include/item.h ./include/char.h ./include/object.h \
  ./include/readmap.h ./include/msignal.h ./include/map_deal.h \
  ./include/npc_duelranking.h ./include/npc_warpman.h \
  ./include/handletime.h ./include/npc_scheduleman.h \
  ./include/configfile.h ./include/npc_eventaction.h ./include/family.h \
  ./include/log.h ./include/buf.h ./include/npc_alldoman.h
callfromcli.o: callfromcli.c ./include/version.h ./include/correct_bug.h \
  ./include/lssproto_serv.h ./include/lssproto_util.h \
  ./include/saacproto_cli.h ./include/version.h \
  ./include/saacproto_util.h ./include/net.h \
  ./include/char_base.h ./include/skill.h ./include/util.h \
  ./include/title.h ./include/addressbook.h ./include/net.h \
  ./include/char_data.h ./include/item.h ./include/char.h \
  ./include/object.h ./include/readmap.h ./include/addressbook.h \
  ./include/handletime.h ./include/configfile.h ./include/event.h \
  ./include/pet.h ./include/battle.h ./include/battle_command.h \
  ./include/magic.h ./include/petmail.h ./include/item_gen.h \
  ./include/pet_skill.h ./include/log.h ./include/map_deal.h \
  ./include/trade.h ./include/family.h ./include/item_event.h
object.o: object.c ./include/version.h ./include/correct_bug.h \
  ./include/char_base.h ./include/version.h ./include/skill.h \
  ./include/util.h ./include/title.h ./include/addressbook.h \
  ./include/net.h \
  ./include/char_data.h ./include/item.h ./include/char.h \
  ./include/item.h ./include/buf.h ./include/readmap.h ./include/pet.h
handletime.o: handletime.c ./include/version.h ./include/correct_bug.h \
  ./include/handletime.h ./include/common.h
function.o: function.c ./include/version.h ./include/correct_bug.h \
  ./include/char_base.h ./include/version.h ./include/skill.h \
  ./include/util.h ./include/title.h ./include/addressbook.h \
  ./include/net.h \
  ./include/item.h ./include/char.h ./include/item_event.h \
  ./include/magic.h ./include/pet_event.h ./include/npc_townpeople.h \
  ./include/npc_Dengon.h ./include/npc_door.h ./include/npc_healer.h \
  ./include/npc_oldman.h ./include/npc_warp.h ./include/npc_storyteller.h \
  ./include/npc_msg.h ./include/npc_npcenemy.h ./include/npc_action.h \
  ./include/npc_windowman.h ./include/npc_savepoint.h \
  ./include/npc_windowhealer.h ./include/npc_itemshop.h \
  ./include/npc_sysinfo.h ./include/npc_duelranking.h \
  ./include/npc_petskillshop.h ./include/npc_petshop.h \
  ./include/npc_signboard.h ./include/npc_warpman.h \
  ./include/npc_exchangeman.h ./include/petmail.h ./include/npc_timeman.h \
  ./include/npc_bodylan.h ./include/npc_mic.h ./include/npc_luckyman.h \
  ./include/npc_bus.h ./include/npc_charm.h ./include/npc_poolitemshop.h \
  ./include/npc_quiz.h ./include/npc_checkman.h ./include/npc_janken.h \
  ./include/npc_transmigration.h ./include/battle_event.h \
  ./include/enemy.h ./include/npc_familyman.h ./include/npc_bankman.h \
  ./include/npc_fmdengon.h ./include/npc_fmhealer.h \
  ./include/npc_petmaker.h ./include/npc_fmwarpman.h \
  ./include/npc_fmpkman.h ./include/npc_fmpkcallman.h \
  ./include/npc_airplane.h ./include/npc_scheduleman.h \
  ./include/npc_manorsman.h ./include/npc_riderman.h \
  ./include/npc_fmletter.h ./include/npc_gamblebank.h \
  ./include/npc_newnpcman.h ./include/npc_gambleroulette.h \
  ./include/npc_gamblemaster.h ./include/npc_transerman.h \
  ./include/npc_autopk.h ./include/npc_freepetskillshop.h \
  ./include/npc_petracemaster.h ./include/npc_petracepet.h \
  ./include/npc_itemchange.h ./include/npc_alldoman.h
log.o: log.c ./include/version.h ./include/correct_bug.h \
  ./include/common.h  \
  ./include/util.h \
  ./include/log.h ./include/handletime.h ./include/net.h \
  ./include/skill.h ./include/util.h ./include/title.h \
  ./include/addressbook.h ./include/net.h
autil.o: autil.c ./include/version.h ./include/correct_bug.h \
  ./include/version.h ./include/common.h \
  ./include/skill.h ./include/util.h  \
  ./include/title.h ./include/addressbook.h ./include/net.h \
  ./include/char.h ./include/lssproto_util.h  
saacproto_cli.o: saacproto_cli.c ./include/version.h \
  ./include/correct_bug.h ./include/saacproto_cli.h ./include/version.h \
  ./include/saacproto_util.h  \
  ./include/common.h  \
  ./include/net.h ./include/common.h  
saacproto_util.o: saacproto_util.c ./include/version.h \
  ./include/correct_bug.h \
  ./include/saacproto_util.h
lssproto_serv.o: lssproto_serv.c ./include/version.h \
  ./include/correct_bug.h ./include/autil.h ./include/version.h \
  ./include/common.h \
  ./include/lssproto_serv.h ./include/lssproto_util.h \
  ./include/char_base.h ./include/skill.h ./include/util.h \
  ./include/net.h \
  ./include/char.h ./include/net.h
lssproto_util.o: lssproto_util.c ./include/version.h \
  ./include/correct_bug.h  \
  ./include/lssproto_util.h ./include/common.h 
