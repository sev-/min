/*
 * $Id: warn.c,v 1.4 1995/01/24 15:40:39 sev Exp $
 * 
 * ----------------------------------------------------------
 * 
 * $Log: warn.c,v $
 * Revision 1.4  1995/01/24 15:40:39  sev
 * Added inverse line while run; play_error; start label; Labels buffer
 *
 * Revision 1.3  1995/01/21  15:19:59  sev
 * Now Run works, Ports and regs change, list creates
 *
 * Revision 1.2  1995/01/17  12:33:59  sev
 * Now run screen is done
 * Revision 1.1  1995/01/14  15:08:09  sev Initial revision
 * 
 * 
 */

#include "hardware.h"
#include "estruct.h"
#include "edef.h"
#include "eproto.h"
#include "proto.h"

void addmessage (LINE * line, char *msg, int mode)
{
  BUFFER *cbuf;
  register LINE *lp;		  /* current line */
  register int numline;		  /* # of line before point */
  char tmp[256];

  if (line)
  {
    /* starting at the beginning of the buffer */
    lp = lforw (curbp->b_linep);

    /* start counting line */
    numline = 1;
    while (lp != line)
    {
      ++numline;
      lp = lforw (lp);
    }
  }

  switch (mode)
  {
    case 0:			  /* just a message */
      sprintf (tmp, "%s", msg);
      break;
    case 1:			  /* error */
      sprintf (tmp, "Error at line %d: %s", numline, msg);
      program_has_errors = 1;
      break;
    case 2:			  /* warning */
      sprintf (tmp, "Warning at line %d: %s", numline, msg);
      program_has_errors = 1;
      break;
  }
  cbuf = curbp;
  swbuffer (bfind (ERRORBUFFERNAME, 1, 0));
  linstr (tmp);
  forwline (1, 1);
  curbp->b_flag &= ~BFCHG;	  /* Not changed	       */
  swbuffer (cbuf);
}

void clearerrorbuffer (void)
{
  BUFFER *errbuf;

  errbuf = bfind (ERRORBUFFERNAME, 1, 0);
  errbuf->b_flag &= ~BFCHG;	  /* Not changed	       */
  bclear (errbuf);
}

int show_msg (char *msg, int mode)
{
  int center;
  int c;
  int quitflag = 0;
  int yes_no;

  center = strlen (msg)/2 + 2;
  if (mode == 0)
  {
    winopen (11, 40 - center, 13, 40 + center, "");
    TTmove (12, 42 - center);
    TTputs (msg);
    TTflush ();
    while (getkey () != ' ');
    winclose (11, 40 - center, 13, 40 + center);
    return 1;
  }
  if (mode == 1)
  {
    winopen (11, 40 - center, 14, 40 + center, "");
    TTmove (12, 42 - center);
    TTputs (msg);
    TTmove (13, 42 - 4);
    TTputs ("Yes   No");
    TTflush ();

    yes_no = !0;
    while (!quitflag)
    {
      if (yes_no)
        TTrev (TRUE);
      TTmove (13, 42 - 5);
      TTputs (" Yes ");
      if (yes_no)
        TTrev (FALSE);
      else
        TTrev (TRUE);
      TTmove (13, 42);
      TTputs (" No ");
      if (!yes_no)
        TTrev (FALSE);
      TTflush ();

      c = getkey ();
      switch (c)
      {
        case CTRL | 'G':		/* Exit */
          yes_no = 0;
          quitflag = 1;
	  break;
        case CTRL | 'M':		/* Enter */
        case CTRL | 'J':
          quitflag = 1;
  	  break;
        case CTRL | 'F':		/* Forward */
        case SPEC | 'F':
        case CTRL | 'B':		/* Backward */
        case SPEC | 'B':
          yes_no = !yes_no;
  	break;
      }
    }
    winclose (11, 40 - center, 14, 40 + center);
    update (TRUE);
    return yes_no;
  }
}
