#
#  $Id: makefile,v 1.2 1995/01/06 21:45:10 sev Exp $
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

$(OFILES): $(HFILES)

clean:
	rm -f $(OFILES) core min *.b $(EMOFILES)
	ci $(CFILES) $(HFILES) makefile $(EMHFILES) $(EMCFILES)

EMOFILES=	ebasic.o ebuffer.o echar.o edisplay.o\
		efile.o efileio.o einput.o eline.o emain.o emenu.o\
		erandom.o eregion.o esearch.o etcap.o eunix.o

EMCFILES=	ebasic.c ebuffer.c echar.c edisplay.c\
		efile.c efileio.c einput.c eline.c emain.c emenu.c\
		erandom.c eregion.c esearch.c etcap.c eunix.c

EMHFILES=	estruct.h edef.h ebind.h etype.h english.h

$(EMOFILES):	$(EMHFILES)

emain.o:
		$(CC) $(CFLAGS) -DCALLED=1 -c emain.c

libemacs.a:	$(EMOFILES)
		rm -f libemacs.a
		$(AR) r libemacs.a $(EMOFILES)

