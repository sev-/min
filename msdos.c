/*	MSDOS.C:	Operating specific I/O and Spawning functions
			under the MS/PCDOS operating system
			for MicroEMACS 3.10
			(C)opyright 1988 by Daniel M. Lawrence
*/

#include        <stdio.h>
#include	"estruct.h"
#include	"eproto.h"

#include        "edef.h"
#include	"english.h"

#include <conio.h>
#include <dir.h>
#include <dos.h>

union REGS rg;
struct ffblk fileblock;	/* structure for directory searches */

#include	<process.h>

/*	Some global variable	*/
#define INBUFSIZ	40
static int mexist;	/* is the mouse driver installed? */
static int nbuttons;	/* number of buttons on the mouse */
static int oldbut;	/* Previous state of mouse buttons */

/*	input buffers and pointers	*/

#define	IBUFSIZE	64	/* this must be a power of 2 */

unsigned char in_buf[IBUFSIZE];	/* input character buffer */
int in_next = 0;		/* pos to retrieve next input character */
int in_last = 0;		/* pos to place most recent input character */

in_init()	/* initialize the input buffer */

{
	in_next = in_last = 0;
}

in_check()	/* is the input buffer non-empty? */

{
	if (in_next == in_last)
		return(FALSE);
	else
		return(TRUE);
}

in_put(event)

int event;	/* event to enter into the input buffer */

{
	in_buf[in_last++] = event;
	in_last &= (IBUFSIZE - 1);
}

int in_get()	/* get an event from the input buffer */

{
	register int event;	/* event to return */

	event = in_buf[in_next++];
	in_next &= (IBUFSIZE - 1);
	return(event);
}

/*
 * This function is called once to set up the terminal device streams.
 */

int ttopen()

{
#if     (HP150 == 0) & LATTICE
	/* kill the ctrl-break interupt */
	rg.h.ah = 0x33;		/* control-break check dos call */
	rg.h.al = 1;		/* set the current state */
	rg.h.dl = 0;		/* set it OFF */
	intdos(&rg, &rg);	/* go for it! */
#endif
	/* on all screens we are not sure of the initial position
	   of the cursor					*/
	ttrow = 999;
	ttcol = 999;

	mexist = 0;
}

maxlines(lines)		/* set number of vertical rows for mouse */
int lines;	/* # of vertical lines */
{
}

/*
 * This function gets called just before we go back home to the command
 * interpreter. On VMS it puts the terminal back in a reasonable state.
 * Another no-operation on CPM.
 */
int ttclose()
{
#if     (HP150 == 0) & LATTICE
	/* restore the ctrl-break interrupt */
	rg.h.ah = 0x33;		/* control-break check dos call */
	rg.h.al = 1;		/* set the current state */
	rg.h.dl = 1;		/* set it ON */
	intdos(&rg, &rg);	/* go for it! */
#endif
}

/*
 * Write a character to the display. On VMS, terminal output is buffered, and
 * we just put the characters in the big array, after checking for overflow.
 * On CPM terminal I/O unbuffered, so we just write the byte out. Ditto on
 * MS-DOS (use the very very raw console output routine).
 */
int ttputc(c)

{
	bdos(6, c, 0);
}

/*
 * Flush terminal buffer. Does real work where the terminal output is buffered
 * up. A no-operation on systems where byte at a time terminal I/O is done.
 */
int ttflush()
{
}

int doschar()	/* call the dos to get a char */

{
	register unsigned int c;	/* extended character to return */

	rg.h.ah = 7;		/* dos Direct Console Input call */
	intdos(&rg, &rg);
	if (rg.h.al == 0) {	/* function key!! */
		rg.h.ah = 7;	/* get the next character */
		intdos(&rg, &rg);
		c = extcode(rg.h.al);
		in_put(c >> 8);		/* prefix byte */
		in_put(c & 255);	/* event code byte */
		return(0);		/* extended escape sequence */
	}
	return(rg.h.al & 255);
}

/*
 * Read a character from the terminal, performing no editing and doing no echo
 * at all. Also mouse events are forced into the input stream here.
 */
int ttgetc()

{
	register int c;		/* character read */

ttc:	/* return any keystrokes waiting in the
	   type ahead buffer */
	if (in_check())
		return(in_get());

	if (typahead())
		return(doschar());

	/* with no mouse, this is a simple get char routine */
	if (mexist == FALSE)
		return(doschar());
}

#if	TYPEAH
/* typahead:	Check to see if any characters are already in the
		keyboard buffer
*/

int typahead()

{
	int flags;	/* cpu flags from dos call */

	rg.x.ax = 0x4406;	/* IOCTL input status */
	rg.x.bx = 0;		/* File handle = stdin */
	flags = intdos(&rg, &rg);
	if (flags)		/* AL = 0xFF if ready */
		return(TRUE);
	else
		return(FALSE);
}
#endif

/* return a system dependant string with the current time */

char *timeset()

{
	register char *sp;	/* temp string pointer */
	char buf[16];		/* time data buffer */
	extern char *ctime();

	time(buf);
	sp = ctime(buf);
	sp[strlen(sp)-1] = 0;
	return(sp);
}

/*	extcode:	resolve MSDOS extended character codes
			encoding the proper sequences into emacs
			printable character specifications
*/

int extcode(unsigned int c)
{
	/* function keys 1 through 9 */
	if (c >= 59 && c < 68)
		return(SPEC | c - 58 + '0');

	/* function key 10 */
	if (c == 68)
		return(SPEC | '0');

	/* shifted function keys */
	if (c >= 84 && c < 93)
		return(SPEC | SHFT | c - 83 + '0');
	if (c == 93)
		return(SPEC | SHFT | '0');

	/* control function keys */
	if (c >= 94 && c < 103)
		return(SPEC | CTRL | c - 93 + '0');
	if (c == 103)
		return(SPEC | CTRL | '0');

	/* ALTed function keys */
	if (c >= 104 && c < 113)
		return(SPEC | ALTD | c - 103 + '0');
	if (c == 113)
		return(SPEC | ALTD | '0');

	/* ALTed number keys */
	if (c >= 120 && c < 129)
		return(ALTD | c - 119 + '0');
	if (c == 130)
		return(ALTD | '0');

	/* some others as well */
	switch (c) {
		case 3:		return(0);		/* null */
		case 15:	return(SHFT | CTRL | 'I');	/* backtab */

		case 16:	return(ALTD | 'Q');
		case 17:	return(ALTD | 'W');
		case 18:	return(ALTD | 'E');
		case 19:	return(ALTD | 'R');
		case 20:	return(ALTD | 'T');
		case 21:	return(ALTD | 'Y');
		case 22:	return(ALTD | 'U');
		case 23:	return(ALTD | 'I');
		case 24:	return(ALTD | 'O');
		case 25:	return(ALTD | 'P');

		case 30:	return(ALTD | 'A');
		case 31:	return(ALTD | 'S');
		case 32:	return(ALTD | 'D');
		case 33:	return(ALTD | 'F');
		case 34:	return(ALTD | 'G');
		case 35:	return(ALTD | 'H');
		case 36:	return(ALTD | 'J');
		case 37:	return(ALTD | 'K');
		case 38:	return(ALTD | 'L');

		case 44:	return(ALTD | 'Z');
		case 45:	return(ALTD | 'X');
		case 46:	return(ALTD | 'C');
		case 47:	return(ALTD | 'V');
		case 48:	return(ALTD | 'B');
		case 49:	return(ALTD | 'N');
		case 50:	return(ALTD | 'M');

		case 71:	return(SPEC | '<');	/* HOME */
		case 72:	return(SPEC | 'P');	/* cursor up */
		case 73:	return(SPEC | 'Z');	/* page up */
		case 75:	return(SPEC | 'B');	/* cursor left */
		case 77:	return(SPEC | 'F');	/* cursor right */
		case 79:	return(SPEC | '>');	/* end */
		case 80:	return(SPEC | 'N');	/* cursor down */
		case 81:	return(SPEC | 'V');	/* page down */
		case 82:	return(SPEC | 'C');	/* insert */
		case 83:	return(SPEC | 'D');	/* delete */
		case 115:	return(SPEC | CTRL | 'B');	/* control left */
		case 116:	return(SPEC | CTRL | 'F');	/* control right */
		case 117:	return(SPEC | CTRL | '>');	/* control END */
		case 118:	return(SPEC | CTRL | 'V');	/* control page down */
		case 119:	return(SPEC | CTRL | '<');	/* control HOME */
		case 132:	return(SPEC | CTRL | 'Z');	/* control page up */
	}

	return(ALTD | c);
}

