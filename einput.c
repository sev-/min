/*
 * $Id: einput.c,v 1.5 1995/10/14 15:46:11 sev Exp $
 * 
 * ----------------------------------------------------------
 * 
 * $Log: einput.c,v $
 * Revision 1.5  1995/10/14 15:46:11  sev
 * Program was in MSDOS and done A _LOT OF_ changes
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
 * Input:	Various input routines for MicroEMACS written by Daniel
 * Lawrence 5/9/86
 */

/*
 * Notes:
 * 
 * MicroEMACS's kernel processes two distinct forms of characters.  One of these
 * is a standard unsigned character which is used in the edited text.  The
 * other form, called an EMACS Extended Character is a 2 byte value which
 * contains both an ascii value, and flags for certain prefixes/events.
 * 
 * Bit	Usage ---	----- 0 = 7	Standard 8 bit ascii character 8
 * Control key flag 9	META prefix flag 10	^X prefix flag 11 Function
 * key flag 12	Mouse prefix 13	Shifted flag (not needed on alpha shifted
 * characters) 14	Alterate prefix (ALT key on PCs)
 * 
 * The machine dependent driver is responsible for returning a byte stream from
 * the various input devices with various prefixes/events embedded as escape
 * codes.  Zero is used as the value indicating an escape sequence is next.
 * The format of an escape sequence is as follows:
 * 
 * 0		Escape indicator <prefix byte>	upper byte of extended
 * character {<col><row>}	col, row position if the prefix byte
 * indicated a mouse event <event code>	value of event
 * 
 * Two successive zeroes are used to indicate an actual null being input.  These
 * values are then interpreted by getkey() to construct the proper extended
 * character sequences to pass to the MicroEMACS kernel.
 */

#include	<stdio.h>
#include	"estruct.h"
#include	"eproto.h"
#include	"edef.h"
#include	"english.h"

/*
 * Ask a yes or no question in the message line. Return either TRUE, FALSE,
 * or ABORT. The ABORT status is returned if the user bumps out of the
 * question with a ^G. Used any time a confirmation is required.
 */
int mlyesno (char *prompt)
{
  int c;			  /* input character */
  char buf[NPAT];		  /* prompt to user */

  for (;;)
  {
    /* build and prompt the user */
    strcpy (buf, prompt);
    strcat (buf, TEXT162);
    /* " [y/n]? " */
    mlwrite (buf);

    /* get the response */
    c = getcmd ();		  /* getcmd() lets us check for anything that
				   * might */
    /* generate a 'y' or 'Y' in case use screws up */

    if (c == ectoc (abortc))	  /* Bail out! */
      return (ABORT);

    if ((c == 'n') || (c == 'N')
	|| (c & (SPEC | ALTD | CTRL | META | CTLX | MOUS)))
      return (FALSE);		  /* ONLY 'y' or 'Y' allowed!!! */

    if (c == 'y' || c == 'Y')
      return (TRUE);

    return (FALSE);
  }
}

/*
 * ectoc:	expanded character to character collapse the CTRL and SPEC
 * flags back into an ascii code
 */
int ectoc (int c)
{
  if (c & CTRL)
    c = c & ~(CTRL | 0x40);
  if (c & SPEC)
    c = c & 255;
  return (c);
}

/*
 * tgetc:	Get a key from the terminal driver, resolve any keyboard
 * macro action
 */
int tgetc (void)
{
  int c;			  /* fetched character */

  /* if we are playing a keyboard macro back, */
  if (kbdmode == PLAY)
  {

    /* if there is some left... */
    if (kbdptr < kbdend)
      return ((int) *kbdptr++);

    /* at the end of last repitition? */
    if (--kbdrep < 1)
    {
      kbdmode = STOP;
      /* force a screen update after all is done */
      update (FALSE);
    }
    else
    {

      /* reset the macro to the begining for the next rep */
      kbdptr = &kbdm[0];
      return ((int) *kbdptr++);
    }
  }

  /* fetch a character from the terminal driver */
  c = TTgetc ();

  /* record it for $lastkey */
  lastkey = c;

  /* save it if we need to */
  if (kbdmode == RECORD)
  {
    *kbdptr++ = c;
    kbdend = kbdptr;

    /* don't overrun the buffer */
    if (kbdptr == &kbdm[NKBDM - 1])
    {
      kbdmode = STOP;
      TTbeep ();
    }
  }

  /* and finally give the char back */
  return (c);
}

/*
 * getkey: Get one keystroke. The only prefixs legal here are the SPEC and
 * CTRL prefixes.
 */
int getkey (void)
{
  int c;			  /* next input character */
  int upper;			  /* upper byte of the extended sequence */

  /* get a keystroke */
  c = tgetc ();

  /* if it exists, process an escape sequence */
  if (c == 0)
  {

    /* get the event type */
    upper = tgetc ();

    /* mouse events need us to read in the row/col */
    if (upper & (MOUS >> 8))
    {
      /* grab the x/y position of the mouse */
      xpos = tgetc ();
      ypos = tgetc ();
    }

    /* get the event code */
    c = tgetc ();

    /* if it is a function key... map it */
    c = (upper << 8) | c;

  }

  /* yank out the control prefix */
  if ((c & 255) >= 0x00 && (c & 255) <= 0x1F)
    c = CTRL | (c + '@');

  /* return the character */
  return (c);
}

/*
 * GETCMD: Get a command from the keyboard. Process all applicable prefix
 * keys
 */
int getcmd (void)
{
  int c;			  /* fetched keystroke */
  KEYTAB *key;			  /* ptr to a key entry */

  /* get initial character */
  c = getkey ();
  key = getbind (c);

  /* resolve META and CTLX prefixes */
  if (key)
  {
    if (key->k_ptr.fp == meta)
    {
      c = getkey ();
      c = upperc (c) | (c & ~255);/* Force to upper */
      c |= META;
    }
    else if (key->k_ptr.fp == cex)
    {
      c = getkey ();
      c = upperc (c) | (c & ~255);/* Force to upper */
      c |= CTLX;
    }
  }

  /* return it */
  return (c);
}

/*
 * A more generalized prompt/reply function allowing the caller to specify
 * the proper terminator. If the terminator is not a return('\r'), return
 * will echo as "<NL>"
 */
int getstring (char *prompt, char *buf, int nbuf, int eolchar)
{
  register int cpos;		  /* current character position in string */
  register int c;		  /* current input character */
  register int quotef;		  /* are we quoting the next char? */

  cpos = 0;
  quotef = FALSE;

  /* prompt the user for the input string */
  if (discmd)
    mlwrite (prompt);
  else
    movecursor (term.t_nrow, 0);

  for (;;)
  {
    /* get a character from the user */
    c = getkey ();

    /* if they hit the line terminate, wrap it up */
    if (c == eolchar && quotef == FALSE)
    {
      buf[cpos++] = 0;

      /* clear the message line */
      mlwrite ("");
      TTflush ();

      /* if we default the buffer, return FALSE */
      if (buf[0] == 0)
	return (FALSE);

      return (TRUE);
    }

    /* change from command form back to character form */
    c = ectoc (c);

    if (c == ectoc (abortc) && quotef == FALSE)
    {
      /* Abort the input? */
      ctrlg (FALSE, 0);
      TTflush ();
      return (ABORT);
    }
    else if ((c == 0x7F || c == 0x08) && quotef == FALSE)
    {
      /* rubout/erase */
      if (cpos != 0)
      {
	outstring ("\b \b");
	--ttcol;

	if (buf[--cpos] < 0x20)
	{
	  outstring ("\b \b");
	  --ttcol;
	}

	if (buf[cpos] == '\r')
	{
	  outstring ("\b\b  \b\b");
	  ttcol -= 2;
	}
	TTflush ();
      }

    }
    else if (c == 0x15 && quotef == FALSE)
    {
      /* C-U, kill */
      while (cpos != 0)
      {
	outstring ("\b \b");
	--ttcol;

	if (buf[--cpos] < 0x20)
	{
	  outstring ("\b \b");
	  --ttcol;
	}
      }
      TTflush ();

    }
    else if (c == quotec && quotef == FALSE)
    {
      quotef = TRUE;
    }
    else
    {
      quotef = FALSE;
      if (cpos < nbuf - 1)
      {
	buf[cpos++] = c;

	if ((c < ' ') && (c != '\r'))
	{
	  outstring ("^");
	  ++ttcol;
	  c ^= 0x40;
	}

	if (c != '\r')
	  mlout (c);
	else
	{			  /* put out <NL> for <ret> */
	  outstring ("<NL>");
	  ttcol += 3;
	}
	++ttcol;
	TTflush ();
      }
    }
  }
}

void outstring (char *s)	  /* output a string of input characters */
{
  while (*s)
    mlout (*s++);
}

char *gtfilename (char *prompt)	  /* prompt to user on command line */
{
  char *sp;			  /* ptr to the returned string */

  sp = complete (prompt, NULL, NFILEN);
  if (sp == NULL)
    return (NULL);

  return (sp);
}

char *complete (char *prompt, char *defval, int maxlen)
/* prompt;		prompt to user on command line */
/* defval;		default value to display to user */
/* maxlen;		maximum length of input field */
{
  register int c;		  /* current input character */
  register int ec;		  /* extended input character */
  int cpos;			  /* current column on screen output */
  static char buf[NSTRING];	  /* buffer to hold tentative name */

  /* starting at the beginning of the string buffer */
  cpos = 0;

  /* if it exists, prompt the user for a buffer name */
  if (prompt)
    mlwrite ("%s: ", prompt);

  /* build a name string from the keyboard */
  while (TRUE)
  {

    /* get the keystroke and decode it */
    ec = getkey ();
    c = ectoc (ec);

    /* if we are at the end, just match it */
    if (c == '\n' || c == '\r')
    {
      if (defval && cpos == 0)
	return (defval);
      else
      {
	buf[cpos] = 0;
	return (buf);
      }

    }
    else if (ec == abortc)
    {				  /* Bell, abort */
      ctrlg (FALSE, 0);
      TTflush ();
      return (NULL);

    }
    else if (c == 0x7F || c == 0x08)
    {				  /* rubout/erase */
      if (cpos != 0)
      {
	mlout ('\b');
	mlout (' ');
	mlout ('\b');
	--ttcol;
	--cpos;
	TTflush ();
      }

    }
    else if (c == 0x15)
    {				  /* C-U, kill */
      while (cpos != 0)
      {
	mlout ('\b');
	mlout (' ');
	mlout ('\b');
	--cpos;
	--ttcol;
      }
      TTflush ();
    }
    else
    {
      if (cpos < maxlen && c > ' ')
      {
	buf[cpos++] = c;
	mlout (c);
	++ttcol;
	TTflush ();
      }
    }
  }
}

/*
 * getcbuf:	get a completion from the user for a buffer name.
 * 
 * I was goaded into this by lots of other people's completion code.
 */

BUFFER *getcbuf (char *prompt, char *defval, int createflag)
/* char *prompt;		prompt to user on command line */
/* char *defval;		default value to display to user */
/* int createflag;		should this create a new buffer? */
{
  char *sp;			  /* ptr to the returned string */

  sp = complete (prompt, defval, NBUFN);
  if (sp == NULL)
    return (NULL);

  return (bfind (sp, createflag, 0));
}

/*
 * Write a prompt into the message line, then read back a response. Keep
 * track of the physical position of the cursor. If we are in a keyboard
 * macro throw the prompt away, and return the remembered response. This lets
 * macros run at full speed. The reply is always terminated by a carriage
 * return. Handle erase, kill, and abort keys.
 */
int mlreply (char *prompt, char *buf, int nbuf)
{
  return (getstring (prompt, buf, nbuf, ctoec ((int) '\r')));
}

/*
 * ctoec:	character to extended character pull out the CTRL and SPEC
 * prefixes (if possible)
 */
int ctoec (int c)
{
  if ((c >= 0x00 && c <= 0x1F) || c == 0x7F)
    c = CTRL | (c ^ 0x40);
  return (c);
}
