#
#  $Id: makefile,v 1.5 1995/01/14 15:08:09 sev Exp $
#
# ----------------------------------------------------------
#
# 
#

CC=gcc
CFLAGS=-O

OFILES=min.o compile.o docom.o run.o warn.o
CFILES=min.c compile.c docom.c run.c warn.c
HFILES=commands.h hardware.h proto.h

min1: $(OFILES) libemacs.a
	$(CC) -o min1 $(CFLAGS) $(OFILES) libemacs.a -ltermcap

emacs1: libemacs.a
	$(CC) $(CFLAGS) -DCALLED=0 -c emain.c
	$(CC) -o emacs1 $(CFLAGS) emain.o libemacs.a -ltermcap

clean:
	rm -f $(OFILES) core min1 *.b $(EMOFILES) libemacs.a
	ci $(CFILES) $(HFILES) makefile $(EMHFILES) $(EMCFILES)

EMOFILES=	ebasic.o ebuffer.o echar.o edisplay.o\
		efile.o efileio.o einput.o eline.o emain.o emenu.o\
		erandom.o eregion.o esearch.o etcap.o eunix.o ewindow.o

EMCFILES=	ebasic.c ebuffer.c echar.c edisplay.c edit_str.c\
		efile.c efileio.c einput.c eline.c emain.c emenu.c\
		erandom.c eregion.c esearch.c etcap.c eunix.c ewindow.c

EMHFILES=	estruct.h ebind.h edef.h eproto.h english.h

libemacs.a:	$(EMOFILES) emain.o
		rm -f libemacs.a
		$(AR) r libemacs.a $(EMOFILES) emain.o

compile.o:	estruct.h edef.h hardware.h commands.h proto.h
docom.o:	hardware.h estruct.h proto.h
ebasic.o:	estruct.h eproto.h edef.h english.h
ebuffer.o:	estruct.h eproto.h edef.h english.h
echar.o:	estruct.h eproto.h edef.h english.h
edisplay.o:	estruct.h eproto.h edef.h english.h
edit_str.o:	estruct.h eproto.h
efile.o:	estruct.h eproto.h edef.h english.h
efileio.o:	estruct.h eproto.h edef.h english.h
einput.o:	estruct.h eproto.h edef.h english.h
eline.o:	estruct.h eproto.h edef.h english.h
emain.o:	estruct.h eproto.h edef.h english.h ebind.h
		$(CC) $(CFLAGS) -DCALLED=1 -c emain.c

emenu.o:	estruct.h eproto.h edef.h
erandom.o:	estruct.h eproto.h edef.h english.h
eregion.o:	estruct.h eproto.h edef.h english.h
esearch.o:	estruct.h eproto.h edef.h english.h
etcap.o:	estruct.h eproto.h edef.h english.h
eunix.o:	estruct.h eproto.h edef.h english.h
ewindow.o:	estruct.h eproto.h edef.h english.h
min.o:		hardware.h estruct.h proto.h commands.h
run.o:		hardware.h commands.h
warn.o:		estruct.h edef.h eproto.h
