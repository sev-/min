/*
 * $Id: warn.c,v 1.3 1995/01/21 15:19:59 sev Exp $
 * 
 * ----------------------------------------------------------
 * 
 * $Log: warn.c,v $
 * Revision 1.3  1995/01/21 15:19:59  sev
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
      break;
    case 2:			  /* warning */
      sprintf (tmp, "Warning at line %d: %s", numline, msg);
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
  errbuf->b_mode &= ~MDVIEW;
  bclear (errbuf);
}

void show_msg (char *msg)
{
  int c;

  c = strlen (msg)/2 + 2;
  winopen (11, 40 - c, 13, 40 + c, "");
  TTmove (12, 42 - c);
  TTputs (msg);
  TTflush ();
  while (getkey () != ' ');
  winclose (11, 40 - c, 13, 40 + c);
}
