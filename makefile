#
#  $Id: makefile,v 1.9 1995/10/14 15:46:11 sev Exp $
#
# ----------------------------------------------------------
#
# 
#

CC=gcc
CFLAGS=-g

OFILES=min.o compile.o docom.o run.o warn.o
CFILES=min.c compile.c docom.c run.c warn.c
HFILES=commands.h hardware.h proto.h

EMOFILES=	ebasic.o ebuffer.o echar.o edisplay.o edit_str.o\
		efile.o efileio.o einput.o eline.o emain.o emenu.o\
		erandom.o eregion.o esearch.o etcap.o eunix.o ewindow.o

EMCFILES=	ebasic.c ebuffer.c echar.c edisplay.c edit_str.c\
		efile.c efileio.c einput.c eline.c emain.c emenu.c\
		erandom.c eregion.c esearch.c etcap.c eunix.c ewindow.c

EMHFILES=	estruct.h ebind.h edef.h eproto.h english.h


min1: $(EMOFILES) $(OFILES)
	$(CC) -o min1 $(CFLAGS) $(OFILES) $(EMOFILES) -ltermcap

emacs1: $(EMOFILES)
	$(CC) $(CFLAGS) -DCALLED=0 -c emain.c
	$(CC) -o emacs1 $(CFLAGS) emain.o $(EMOFILES) -ltermcap

clean:
	rm -f $(OFILES) core min1 *.b $(EMOFILES) small.o
	ci $(CFILES) $(HFILES) makefile $(EMHFILES) $(EMCFILES) small.c
	co -l makefile

small: small.o
	$(CC) -o small small.o

$(CFILES) $(EMCFILES) $(HFILES) $(EMHFILES) small.c:
	co -l $@,v

compile.o : compile.c estruct.h edef.h hardware.h commands.h proto.h 
docom.o : docom.c hardware.h estruct.h proto.h 
ebasic.o : ebasic.c estruct.h eproto.h edef.h english.h 
ebuffer.o : ebuffer.c estruct.h eproto.h edef.h english.h 
echar.o : echar.c estruct.h eproto.h edef.h english.h 
edisplay.o : edisplay.c estruct.h eproto.h edef.h english.h 
edit_str.o : edit_str.c estruct.h eproto.h edef.h 
efile.o : efile.c estruct.h eproto.h edef.h english.h 
efileio.o : efileio.c estruct.h eproto.h edef.h english.h 
einput.o : einput.c estruct.h eproto.h edef.h english.h 
eline.o : eline.c estruct.h eproto.h edef.h english.h 
emain.o : emain.c estruct.h eproto.h edef.h english.h ebind.h 
		$(CC) $(CFLAGS) -DCALLED=1 -c emain.c

emenu.o : emenu.c estruct.h eproto.h edef.h 
erandom.o : erandom.c estruct.h eproto.h edef.h english.h 
eregion.o : eregion.c estruct.h eproto.h edef.h english.h 
esearch.o : esearch.c estruct.h eproto.h edef.h english.h 
etcap.o : etcap.c estruct.h eproto.h edef.h english.h 
eunix.o : eunix.c estruct.h eproto.h edef.h english.h 
ewindow.o : ewindow.c estruct.h eproto.h edef.h english.h 
min.o : min.c hardware.h estruct.h proto.h commands.h 
run.o : run.c hardware.h estruct.h proto.h eproto.h edef.h 
warn.o : warn.c hardware.h estruct.h edef.h eproto.h proto.h 

