/*
 * $Id: eunix.c,v 1.5 1995/01/27 20:52:27 sev Exp $
 * 
 * ----------------------------------------------------------
 * 
 * $Log: eunix.c,v $
 * Revision 1.5  1995/01/27 20:52:27  sev
 * Added Animate (only for Unix), Step over, Continue
 * Fixed bug with start label
 *
 * Revision 1.4  1995/01/17  12:33:59  sev
 * Now run screen is done
 * Revision 1.3  1995/01/14  15:08:09  sev Menu works right.
 * Compiler also. Revision 1.2  1995/01/07  20:03:14  sev Maked indent and
 * some editor changes Revision 1.1  1995/01/06  21:45:10  sev Initial
 * revision
 * 
 * 
 */

/*
 * UNIX.C: Operating specific I/O and Spawning functions under UNIX V7,
 * BSD4.2/3, System V, SUN OS and SCO XENIX for MicroEMACS 3.10 (C)opyright
 * 1988 by Daniel M. Lawrence
 */

#include	<stdio.h>
#include	"estruct.h"
#include	"eproto.h"
#include	"edef.h"
#include	"english.h"

#include	<sys/types.h>
#include	<signal.h>
#include	<termio.h>
#include	<fcntl.h>
#include	<sys/ndir.h>
#include	<sys/stat.h>
int kbdflgs;			  /* saved keyboard fd flags	  */
int kbdpoll;			  /* in O_NDELAY mode		     */
int kbdqp;			  /* there is a char in kbdq	 */
char kbdq;			  /* char we've already read	 */
struct termio otermio;		  /* original terminal characteristics */
struct termio ntermio;		  /* charactoristics to use inside */

#include	<signal.h>

/*
 * This function is called once to set up the terminal device streams. On
 * VMS, it translates TT until it finds the terminal, then assigns a channel
 * to it and sets it raw. On CPM it is a no-op.
 */
int ttopen (void)
{
  ioctl (0, TCGETA, &otermio);	  /* save old settings */
  ntermio.c_iflag = otermio.c_iflag & ~(INLCR | ICRNL | IGNCR);
  ntermio.c_oflag = otermio.c_oflag;
  ntermio.c_cflag = otermio.c_cflag;
  ntermio.c_lflag = otermio.c_lflag & ~(ICANON | ISIG | ECHO);
  ntermio.c_line = otermio.c_line;
  ntermio.c_cc[VMIN] = 1;
  ntermio.c_cc[VTIME] = 0;
  ioctl (0, TCSETAW, &ntermio);	  /* and activate them */
  kbdflgs = fcntl (0, F_GETFL, 0);
  kbdpoll = FALSE;

  /* on all screens we are not sure of the initial position of the cursor					 */
  ttrow = 999;
  ttcol = 999;
  return 0;
}

/*
 * This function gets called just before we go back home to the command
 * interpreter.
 */

int ttclose (void)
{
  ioctl (0, TCSETA, &otermio);	  /* restore terminal settings */
  fcntl (0, F_SETFL, kbdflgs);
  return 0;
}

int ttputs (char *s)
{
  char *p = s;

  while (*p)
    ttputc (*p++);

  return 0;
}

/*
 * Write a character to the display. On VMS, terminal output is buffered, and
 * we just put the characters in the big array, after checking for overflow.
 * On CPM terminal I/O unbuffered, so we just write the byte out. Ditto on
 * MS-DOS (use the very very raw console output routine).
 */
int ttputc (int c)
{
  fputc (c, stdout);
  return 0;
}

/*
 * Flush terminal buffer. Does real work where the terminal output is
 * buffered up. A no-operation on systems where byte at a time terminal I/O
 * is done.
 */
int ttflush (void)
{
  fflush (stdout);
  return 0;
}

/*
 * TTGETC: Read a character from the terminal, performing no editing and
 * doing no echo at all. More complex in VMS that almost anyplace else, which
 * figures. Very simple on CPM, because the system can do exactly what you
 * want.
 */
int ttgetc (void)
{
  if (kbdqp)
    kbdqp = FALSE;
  else
  {

    /*
     * we desperatly seek a character so we turn off the NO_DELAY flag and
     * simply wait for the bastard
     */
    if (fcntl (0, F_SETFL, kbdflgs) < 0 && kbdpoll)
      return (FALSE);
    kbdpoll = FALSE;		  /* no polling */
    read (0, &kbdq, 1);		  /* wait until we get a character */
  }
  return (kbdq & 255);
}

#if	TYPEAH

/*
 * typahead:	Check to see if any characters are already in the keyboard
 * buffer
 */
int typahead (void)
{
  if (!kbdqp)
  {
    /* set O_NDELAY */
    if (fcntl (0, F_SETFL, kbdflgs | O_NDELAY) < 0 && kbdpoll)
      return (FALSE);
    kbdpoll = TRUE;
    kbdqp = (1 == read (0, &kbdq, 1));
  }
  return (kbdqp);
}

#endif

/*
 * Create a subjob with a copy of the command intrepreter in it. When the
 * command interpreter exits, mark the screen as garbage so that you do a
 * full repaint. Bound to "^X C". The message at the start in VMS puts out a
 * newline. Under some (unknown) condition, you don't get one free when DCL
 * starts up.
 */
int spawncli (int f, int n)
{
  register char *cp;
  char *getenv ();

  /* don't allow this command if restricted */
  if (restflag)
    return (resterr ());

  movecursor (term.t_nrow, 0);	  /* Seek to last line.	 */
  TTflush ();
  TTclose ();			  /* stty to old settings */
  if ((cp = getenv ("SHELL")) != (char *) NULL && *cp != '\0')
    system (cp);
  else
    system ("exec /bin/sh");
  sgarbf = TRUE;
  sleep (2);
  TTopen ();
  return (TRUE);
}

void rename1 (char *old, char *new)	/* change the name of a file */
{
  link (old, new);
  unlink (old);
}

void settermnowait (int f)
{
  struct termio ts;
  static struct termio tso;

  if (f)
  {
    ioctl(0, TCGETA, &tso);
    ts = tso;
    ts.c_lflag &= 0;
    ts.c_iflag &= ~(ICRNL | IGNCR | INLCR);
    ts.c_oflag &= 0;
    ts.c_cc[VMIN] = 0;
    ts.c_cc[VTIME] = 0;
    ioctl(0, TCSETA, &ts);
  }
  else
    ioctl(0, TCSETA, &tso);
}

int kbhit(void)
{
  unsigned char xc[1];

  if( read(0,xc,1) != 1 )
	return 0;
  return 1;
}
