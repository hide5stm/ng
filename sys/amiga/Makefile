# $Id: Makefile,v 1.6 2003/02/22 08:09:47 amura Exp $
# Makefile for MicroEMACS. using Aztec small model
#
# Modified for Ng 1.3 AmigaDOS version by H.Ohkubo Dec.20,1992

SYS = amiga
S = sys/amiga

# If defined USE_ARP, set LIBS -la -lc.
#LIBS = -lc
LIBS = -la -lc
#LIBS = +l rexxglue.o rexxvars.o +l -lc

# Aztec C 5.0 has no alloca().
ALLOCA = alloca.o

# Now, compile-time options (PREFS) are defined in config.h.
# XOBJ is used to denote any extra object files needed for these
# options.  MENUOBJ and MOUSEOBJ denote the extra object files
# needed for the menu, mouse and Browser.

MOUSEOBJ = ttymouse.o
MENUOBJ  = ttymenu.o
OFFSETOBJ = offset.o
ICONIFYOBJ = iconify.o ttyicon.o
#REXXOBJ = rexx.o
REXXOBJ =
CLIPOBJ = clipboard.o

#PREFS =
XOBJ  = $(MOUSEOBJ) $(MENUOBJ) $(ICONIFYOBJ) $(OFFSETOBJ) $(CLIPOBJ) $(REXXOBJ)

CFLAGS = -I$(S) -Isys/default -DAMIGA -DMANX -wos -su

# Objects which only depend on the "standard" includes
OBJS	= basic.o dir.o dired.o shell.o version.o window.o \
	  kinsoku.o jump.o autosave.o

# Those with unique requirements
IND	= buffer.o complt.o display.o cmode.o echo.o extend.o file.o \
	  help.o kbd.o keymap.o line.o macro.o main.o match.o modes.o \
	  paragraph.o random.o region.o regex.o re_search.o search.o \
	  skg.o kanji.o undo.o word.o

# System dependent objects
OOBJS = cinfo.o spawn.o ttyio.o tty.o ttykbd.o fileio.o sleep.o \
 sysinit.o console.o dirio.o

OBJ = $(OBJS) $(IND) $(OOBJS) $(XOBJ) $(ALLOCA) zz_pointer.o

OSRCS = cinfo.c fileio.c spawn.c ttyio.c tty.c ttykbd.c fileio.c sleep.c \
   sysinit.c malloc.c console.c dirio.c
SRCS = basic.c dir.c dired.c file.c line.c match.c paragraph.c \
   random.c region.c search.c version.c window.c word.c \
   buffer.c display.c echo.c extend.c help.c kbd.c keymap.c \
   macro.c main.c modes.c regex.c re_search.c \
   cmode.c shell.c kinsoku.c complt.c kanji.c skg.c jump.c undo.c autosave.c

OINCS = $(S)/ttydef.h $(S)/sysdef.h sys/default/chrdef.h
INCS = def.h config.h extern.h
REINCS = regex_e.h regex_j.h regex_j.c regex_e.h kanji_.h kanji_.c

#PROG = mg
PROG = ng

#
# The editor
# Building in Ram: saves a LOT of time in pass 2 of the linker.
#
$(PROG):		$(OBJ)
		ln -o ram:$(PROG) -w $(OBJ) $(LIBS)
		copy ram:$(PROG) $(PROG)
		delete ram:$(PROG)

$(OBJ):		$(INCS) $(OINCS)

buffer.o: 	$(INCS) $(OINCS) kbd.h undo.h

cmode.o file.o line.o paragraph.o random.o region.o undo.o word.o: \
		$(INCS) $(OINCS) undo.h

complt.o:	$(INCS) $(OINCS) kbd.h complt.h

display.o modes.o: \
		$(INCS) $(OINCS) kbd.h

echo.o:		$(INCS) $(OINCS) key.h macro.h complt.h

extend.o help.o: \
		$(INCS) $(OINCS) kbd.h macro.h key.h

kanji.o:	$(INCS) $(OINCS) kinit.h

kbd.o:		$(INCS) $(OINCS) kbd.h macro.h key.h undo.h

macro.o: 	$(INCS) $(OINCS) macro.h key.h

main.o search.o: \
		$(INCS) $(OINCS) macro.h

match.o:	$(INCS) $(OINCS) key.h

re_search.o:	$(INCS) $(OINCS) $(REINCS) macro.h

regex.o:	$(INCS) $(OINCS) $(REINCS)

skg.o: 		$(INCS) $(OINCS) macro.h key.h undo.h

$(OOBJS):	$(INCS) $(OINCS)

clean:
	Delete $(OBJ)


keymap.o:	keymap.c $(S)/amiga_maps.c kbd.h
		cc $(CFLAGS) keymap.c

fileio.o:	$(S)/fileio.c $(S)/zz_pointer.h kbd.h
		cc $(CFLAGS) -o fileio.o $(S)/fileio.c

spawn.o:	$(S)/spawn.c
		cc $(CFLAGS) -o spawn.o $(S)/spawn.c

sleep.o:	$(S)/sleep.c
		cc $(CFLAGS) -o sleep.o $(S)/sleep.c

sysinit.o:	$(S)/sysinit.c $(S)/zz_pointer.h
		cc $(CFLAGS) -o sysinit.o $(S)/sysinit.c

#malloc.o:	$(S)/malloc.c
#		cc $(CFLAGS) -o malloc.o $(S)/malloc.c

offset.o:	$(S)/offset.c
		cc $(CFLAGS) -o offset.o $(S)/offset.c

iconify.o:	$(S)/iconify/iconify.c $(S)/iconify.h
		cc $(CFLAGS) -o iconify.o $(S)/iconify/iconify.c

tty.o:		$(S)/tty.c
		cc $(CFLAGS) -o tty.o $(S)/tty.c

ttyicon.o:	$(S)/ttyicon.c $(S)/iconify.h
		cc $(CFLAGS) -I$(S)/iconify -o ttyicon.o $(S)/ttyicon.c

ttyio.o:	$(S)/ttyio.c
		cc $(CFLAGS) -o ttyio.o $(S)/ttyio.c

ttykbd.o:	$(S)/ttykbd.c kbd.h
		cc $(CFLAGS) -o ttykbd.o $(S)/ttykbd.c

ttymenu.o:	$(S)/ttymenu.c macro.h $(S)/menumap.h
		cc $(CFLAGS) -o ttymenu.o $(S)/ttymenu.c

ttymouse.o:	$(S)/ttymouse.c macro.h
		cc $(CFLAGS) -o ttymouse.o $(S)/ttymouse.c

#menustack.o:	$(S)/menustack.c
#		cc $(CFLAGS) -o menustack.o $(S)/menustack.c

console.o:	$(S)/console.c
		cc $(CFLAGS) -o console.o $(S)/console.c

dirio.o:	$(S)/dirio.c
		cc $(CFLAGS) -o dirio.o $(S)/dirio.c

#alloca.o:	$(S)/alloca.asm
#		as -o alloca.o $(S)/alloca.asm
alloca.o:	sys/default/alloca.c
		cc $(CFLAGS) -o $@ sys/default/$*.c

cinfo.o:	sys/default/cinfo.c
		cc $(CFLAGS) -o cinfo.o sys/default/cinfo.c

#rexx.o:		$(S)/rexx.c
#		cc $(CFLAGS) -o rexx.o $(S)/rexx.c

zz_pointer.o:	$(S)/zz_pointer.c $(S)/zz_pointer.h
		cc $(CFLAGS) -o zz_pointer.o $(S)/zz_pointer.c

clipboard.o: $(S)/clipboard.c
		cc $(CFLAGS) -o clipboard.o $(S)/clipboard.c
