#
#  $Id: makefile,v 1.3 1995/01/07 20:03:14 sev Exp $
#
# ----------------------------------------------------------
#
# 
#

CC=gcc
CFLAGS=-O

OFILES=min.o compile.o docom.o
CFILES=min.c compile.c docom.c
HFILES=commands.h hardware.h proto.h

min1: $(OFILES) libemacs.a
	$(CC) -o min1 $(CFLAGS) $(OFILES) libemacs.a -ltermcap

clean:
	rm -f $(OFILES) core min1 *.b $(EMOFILES) libemacs.a
	ci $(CFILES) $(HFILES) makefile $(EMHFILES) $(EMCFILES)

EMOFILES=	ebasic.o ebuffer.o echar.o edisplay.o\
		efile.o efileio.o einput.o eline.o emain.o emenu.o\
		erandom.o eregion.o esearch.o etcap.o eunix.o

EMCFILES=	ebasic.c ebuffer.c echar.c edisplay.c\
		efile.c efileio.c einput.c eline.c emain.c emenu.c\
		erandom.c eregion.c esearch.c etcap.c eunix.c

EMHFILES=	estruct.h ebind.h edef.h etype.h english.h

libemacs.a:	$(EMOFILES) emain.o
		rm -f libemacs.a
		$(AR) r libemacs.a $(EMOFILES) emain.o

compile.o:	hardware.h commands.h proto.h estruct.h edef.h
docom.o:	hardware.h proto.h
ebasic.o:	estruct.h etype.h edef.h english.h
ebuffer.o:	estruct.h etype.h edef.h english.h
echar.o:	estruct.h etype.h edef.h english.h
edisplay.o:	estruct.h etype.h edef.h english.h
edit_str.o:	estruct.h etype.h
efile.o:	estruct.h etype.h edef.h english.h
efileio.o:	estruct.h etype.h edef.h english.h
einput.o:	estruct.h etype.h edef.h english.h
eline.o:	estruct.h etype.h edef.h english.h
emain.o:	estruct.h etype.h edef.h english.h ebind.h
		$(CC) $(CFLAGS) -DCALLED=1 -c emain.c

emenu.o:	estruct.h etype.h edef.h
erandom.o:	estruct.h etype.h edef.h english.h
eregion.o:	estruct.h etype.h edef.h english.h
esearch.o:	estruct.h etype.h edef.h english.h
etcap.o:	estruct.h etype.h edef.h english.h
eunix.o:	estruct.h etype.h edef.h english.h
min.o:		hardware.h proto.h

