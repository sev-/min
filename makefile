#
#  $Id: makefile,v 1.1 1994/06/29 12:43:01 sev Exp $
#
# ----------------------------------------------------------
#
# 
#

CC=rcc
CFLAGS=

OFILES=min.o docom.o
CFILES=min.c docom.c
HFILES=commands.h regs.h

min: $(OFILES)
	$(CC) -o min $(CFLAGS) $(OFILES)

$(OFILES): $(HFILES)

clean:
	rm -f $(OFILES) core min *.b
	ci $(CFILES) $(HFILES) makefile
