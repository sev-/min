/*
 * The routines in this file provide support for the IBM-PC and other
 * compatible terminals. It goes directly to the graphics RAM to do
 * screen output. It compiles into nothing if not an IBM-PC driver
 * Supported monitor cards include CGA, MONO and EGA.
 */

#define	termdef	1			/* don't define "term" external */

#include        <stdio.h>
#include	<dos.h>
#include	<conio.h>
#include	"estruct.h"
#include	"eproto.h"
#include        "edef.h"
#include	"english.h"

#define NROW	50			/* Max Screen size.		*/
#define NCOL    80                      /* Edit if you want to.         */
#define	MARGIN	8			/* size of minimim margin and	*/
#define	SCRSIZ	64			/* scroll size for extended lines */
#define	NPAUSE	200			/* # times thru update to pause */
#define BEL     0x07                    /* BEL character.               */
#define ESC     0x1B                    /* ESC character.               */
#define	SPACE	32			/* space character		*/

#define	SCADC	0xb8000000L		/* CGA address of screen RAM	*/
#define	SCADM	0xb0000000L		/* MONO address of screen RAM	*/
#define SCADE	0xb8000000L		/* EGA/VGA address of screen RAM*/

#define MONOCRSR 0x0B0D			/* monochrome cursor		*/
#define CGACRSR 0x0607			/* CGA cursor			*/
#define EGACRSR 0x0709			/* EGA/VGA cursor		*/

#define	CDCGA	0			/* color graphics card		*/
#define	CDMONO	1			/* monochrome text card		*/
#define	CDEGA	2			/* EGA color adapter		*/
#define	CDVGA	3			/* VGA color adapter		*/
#define	CDSENSE	9			/* detect the card type		*/

#define NDRIVE	4			/* number of screen drivers	*/

int dtype = -1;				/* current display type		*/
char drvname[][8] = {			/* screen resolution names	*/
	"CGA", "MONO", "EGA", "VGA"
};
long scadd;				/* address of screen ram	*/
int *scptr[NROW];			/* pointer to screen lines	*/
unsigned int sline[NCOL];		/* screen line image		*/
int egaexist = FALSE;			/* is an EGA card available?	*/
int vgaexist = FALSE;			/* is video graphics array available? */
extern union REGS rg;			/* cpu register for use of DOS calls */

int ibmmove();
int ibmeeol();
int ibmputc();
int ibmeeop();
int ibmrev();
int ibmcres();
int spal();
int ibmbeep();
int ibmopen();
int ibmclose();
int ibmkopen();
int ibmkclose();
int scinit();
int getboard();
int egaopen();
int egaclose();
int fnclabel();
int ibmputs();

/*
 * Standard terminal interface dispatch table. Most of the fields pointo
 * "termio" code.
 */
TERM    term    = {
	NROW-1,
        NROW-1,
        NCOL,
        NCOL,
	MARGIN,
	SCRSIZ,
	NPAUSE,
        ibmopen,
        ibmclose,
	ibmkopen,
	ibmkclose,
        ttgetc,
	ibmputc,
        ttflush,
        ibmmove,
        ibmeeol,
        ibmeeop,
        ibmbeep,
	ibmrev,
	ibmcres,
	ibmputs
};

int ibmmove(row, col)
{
  gotoxy (col+1, row+1);
}

int ibmeeol()	/* erase to the end of the line */
{
	clreol();
}

int ibmputc(int ch) /* put a character at the current position in the
		   current colors */
{
  putch(ch);
}
int ibmputs (char *s)
{
    cputs(s);
}

int ibmeeop()
{
	int attr;		/* attribute to fill screen with */

	rg.h.ah = 6;		/* scroll page up function code */
	rg.h.al = 0;		/* # lines to scroll (clear it) */
	rg.x.cx = 0;		/* upper left corner of scroll */
	rg.x.dx = (term.t_nrow << 8) | (term.t_ncol - 1);
				/* lower right corner of scroll */
	attr = 0;
	rg.h.bh = attr;
	int86(0x10, &rg, &rg);
}

int ibmrev(int state)	/* change reverse video state */
{
  if (state)
  {
    textcolor(BLACK);
    textbackground(WHITE);
  }
  else
  {
    textcolor(WHITE);
    textbackground(BLACK);
  }
}

int ibmcres(char *res) /* change screen resolution */
{
	int i;		/* index */

	for (i = 0; i < NDRIVE; i++)
		if (strcmp(res, drvname[i]) == 0) {
			scinit(i);
			return(TRUE);
		}
	return(FALSE);
}

int spal()	/* reset the pallette registers */
{
	/* nothin here now..... */
}

int ibmbeep()
{
	bdos(6, BEL, 0);
}

int ibmopen()
{
	scinit(CDSENSE);
	revexist = TRUE;
        ttopen();
}

int ibmclose()
{
	/* if we had the EGA open... close it */
	if (dtype == CDEGA || dtype == CDVGA)
		egaclose();

	ttclose();
}

int ibmkopen()	/* open the keyboard */
{
	rg.x.ax = 0x3301;
	rg.h.dl = 0x00;
	intdos(&rg, &rg);
}

int ibmkclose() /* close the keyboard */
{
}

int scinit(int type) /* initialize the screen head pointers */
{
	union {
		long laddr;	/* long form of address */
		int *paddr;	/* pointer form of address */
	} addr;
	int i;

	window (1, 1, 80, 25);
	/* if asked...find out what display is connected */
	if (type == CDSENSE)
		type = getboard();

	/* if we have nothing to do....don't do it */
	if (dtype == type)
		return(TRUE);

	/* if we try to switch to EGA and there is none, don't */
	if (type == CDEGA && !egaexist)
		return(FALSE);

	/* if we try to switch to VGA and there is none, don't */
	if (type == CDVGA && !vgaexist )
		return(FALSE);

	/* if we had the EGA or VGA open... close it */
	if (dtype == CDEGA || dtype == CDVGA)
		egaclose();

	/* and set up the various parameters as needed */
	switch (type) {
		case CDMONO:	/* Monochrome adapter */
				scadd = SCADM;
				newsize(TRUE, 25);
				maxlines(25);
				break;

		case CDCGA:	/* Color graphics adapter */
				scadd = SCADC;
				newsize(TRUE, 25);
				maxlines(25);
				break;

		case CDEGA:	/* Enhanced graphics adapter */
				scadd = SCADE;
				egaopen();
				newsize(TRUE, 43);
				maxlines(43);
				break;

		case CDVGA:	/* video graphics array - acts as EGA but more lines */
				scadd = SCADE;
				egaopen();
				newsize(TRUE, 50);
				maxlines(50);
				break;
	}

	/* reset the $sres environment variable */
	strcpy(sres, drvname[type]);
	dtype = type;

	/* initialize the screen pointer array */
	for (i = 0; i < NROW; i++) {
		addr.laddr = scadd + (long)(NCOL * i * 2);
		scptr[i] = addr.paddr;
	}
	return(TRUE);
}

/* getboard:	Determine which type of display board is attached.
		Current known types include:

		CDMONO	Monochrome graphics adapter
		CDCGA	Color Graphics Adapter
		CDEGA	Extended graphics Adapter
		CDVGA	Vidio Graphics Array

		if MONO	set to MONO
		   CGA	set to CGA	EGAexist = FALSE VGAexist = FALSE
		   EGA	set to CGA	EGAexist = TRUE  VGAexist = FALSE
		   VGA	set to CGA	EGAexist = TRUE  VGAexist = TRUE
*/
int getboard()
{
	int type;	/* board type to return */

	type = CDCGA;
	int86(0x11, &rg, &rg);
	if ((((rg.x.ax >> 4) & 3) == 3))
		type = CDMONO;

	/* test if EGA present */
	rg.x.ax = 0x1200;
	rg.x.bx = 0xff10;
	int86(0x10,&rg, &rg);		/* If EGA, bh=0-1 and bl=0-3 */
	egaexist = !(rg.x.bx & 0xfefc);	/* Yes, it's EGA */
	if (egaexist) {
		/* Adapter says it's an EGA. We'll get the same response
		   from a VGA, so try to tell the two apart */
		rg.x.ax = 0x1a00;	/* read display combination */
		int86(0x10,&rg,&rg);
		if (rg.h.al == 0x1a && (rg.h.bl == 7 || rg.h.bl == 8)) {
			/* Function is supported and it's a PS/2 50,60,80 with
			   analog display, so it's VGA (I hope!) */
			vgaexist = TRUE;
		} else {
			/* Either BIOS function not supported or something
			   other then VGA so set it to be EGA */
			vgaexist = FALSE;
		}
	}
	return(type);
}

int egaopen()	/* init the computer to work with the EGA or VGA */
{
	/* put the beast into EGA 43/VGA 50 line mode */
	rg.x.ax = 3;
	int86(16, &rg, &rg);

	rg.h.ah = 17;		/* set char. generator function code */
	rg.h.al = 18;		/*  to 8 by 8 double dot ROM         */
	rg.h.bl = 0;		/* block 0                           */
	int86(16, &rg, &rg);

	rg.h.ah = 18;		/* alternate select function code    */
	rg.h.al = 0;		/* clear AL for no good reason       */
	rg.h.bl = 32;		/* alt. print screen routine         */
	int86(16, &rg, &rg);

	rg.h.ah = 1;		/* set cursor size function code */
	rg.x.cx = 0x0607;	/* turn cursor on code */
	int86(0x10, &rg, &rg);

	outp(0x3d4, 10);	/* video bios bug patch */
	outp(0x3d5, 6);
}

int egaclose()
{
	/* put the beast into 80 column mode */
	rg.x.ax = 3;
	int86(16, &rg, &rg);
}

int scwrite(row, outstr, forg, bacg)	/* write a line out*/
int row;	/* row of screen to place outstr on */
char *outstr;	/* string to write out (must be term.t_ncol long) */
int forg;	/* forground color of string to write */
int bacg;	/* background color */
{
	unsigned int attr;	/* attribute byte mask to place in RAM */
	unsigned int *lnptr;	/* pointer to the destination line */
	int i;

	/* build the attribute byte and setup the screen pointer */
	attr = (((bacg & 15) << 4) | (forg & 15)) << 8;
	lnptr = &sline[0];
	for (i=0; i<term.t_ncol; i++)
		*lnptr++ = (outstr[i] & 255) | attr;

	if (dtype == CDCGA) {
		/* wait for vertical retrace to be off */
		while ((inp(0x3da) & 8))
			;
	
		/* and to be back on */
		while ((inp(0x3da) & 8) == 0)
			;
	}

	/* and send the string out */
	movmem(&sline[0], scptr[row],term.t_ncol*2);
}

