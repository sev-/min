/*
 * $Id: warn.c,v 1.1 1995/01/14 15:08:09 sev Exp $
 * 
 * ----------------------------------------------------------
 * 
 * $Log: warn.c,v $
 * Revision 1.1  1995/01/14 15:08:09  sev
 * Initial revision
 *
 * 
 */

#include "estruct.h"
#include "edef.h"
#include "eproto.h"

void addmessage (LINE * line, char *msg, int mode)
{
  BUFFER *cbuf;
  register LINE *lp;		  /* current line */
  register int numline;		  /* # of line before point */
  char tmp[256];

  switch (mode)
  {
    case 0:			  /* just a message */
      sprintf (tmp, "%s", msg);
      break;
    case 1:			  /* error */
      /* starting at the beginning of the buffer */
      lp = lforw (curbp->b_linep);

      /* start counting line */
      numline = 1;
      while (lp != line)
      {
	++numline;
	lp = lforw (lp);
      }
      sprintf (tmp, "Error at line %d: %s", numline, msg);
      break;
  }
  cbuf = curbp;
  swbuffer (bfind ("Error", 1, 0));
  linstr (tmp);
  forwline (1, 1);
  curbp->b_flag &= ~BFCHG;	  /* Not changed	       */
  swbuffer (cbuf);
}

void clearerrorbuffer (void)
{
  BUFFER *errbuf;

  errbuf = bfind ("Error", 1, 0);
  errbuf->b_flag &= ~BFCHG;	  /* Not changed	       */
  errbuf->b_mode &= ~MDVIEW;
  bclear (errbuf);
}
