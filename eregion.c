/*
 * $Id: eregion.c,v 1.4 1995/01/17 12:33:59 sev Exp $
 * 
 * ----------------------------------------------------------
 * 
 * $Log: eregion.c,v $
 * Revision 1.4  1995/01/17 12:33:59  sev
 * Now run screen is done
 * Revision 1.3  1995/01/14  15:08:09  sev Menu works right.
 * Compiler also. Revision 1.2  1995/01/07  20:03:14  sev Maked indent and
 * some editor changes Revision 1.1  1995/01/06  21:45:10  sev Initial
 * revision
 * 
 * 
 */

/*
 * The routines in this file deal with the region, that magic space between
 * "." and mark. Some functions are commands. Some functions are just for
 * internal use.
 */
#include	<stdio.h>
#include	"estruct.h"
#include	"eproto.h"
#include	"edef.h"
#include	"english.h"

/*
 * Kill the region. Ask "getregion" to figure out the bounds of the region.
 * Move "." to the start, and kill the characters. Bound to "C-W".
 */
int killregion (int f, int n)
{
  register int s;
  REGION region;

  if (curbp->b_mode & MDVIEW)	  /* don't allow this command if	 */
    return (rdonly ());		  /* we are in read only mode	 */
  if ((s = getregion (&region)) != TRUE)
    return (s);
  if ((lastflag & CFKILL) == 0)	  /* This is a kill type	 */
    kdelete ();			  /* command, so do magic */
  thisflag |= CFKILL;		  /* kill buffer stuff.	 */
  curwp->w_dotp = region.r_linep;
  curwp->w_doto = region.r_offset;
  return (ldelete (region.r_size, TRUE));
}

/*
 * Copy all of the characters in the region to the kill buffer. Don't move
 * dot at all. This is a bit like a kill region followed by a yank. Bound to
 * "M-W".
 */
int copyregion (int f, int n)
{
  register LINE *linep;
  register int loffs;
  register int s;
  REGION region;

  if ((s = getregion (&region)) != TRUE)
    return (s);
  if ((lastflag & CFKILL) == 0)	  /* Kill type command.	 */
    kdelete ();
  thisflag |= CFKILL;
  linep = region.r_linep;	  /* Current line.	 */
  loffs = region.r_offset;	  /* Current offset.	 */
  while (region.r_size--)
  {
    if (loffs == llength (linep))
    {				  /* End of line. 	 */
      if ((s = kinsert ('\r')) != TRUE)
	return (s);
      linep = lforw (linep);
      loffs = 0;
    }
    else
    {				  /* Middle of line.	 */
      if ((s = kinsert (lgetc (linep, loffs))) != TRUE)
	return (s);
      ++loffs;
    }
  }
  mlwrite (TEXT70);
  /* "[region copied]" */
  return (TRUE);
}

/*
 * This routine figures out the bounds of the region in the current window,
 * and fills in the fields of the "REGION" structure pointed to by "rp".
 * Because the dot and mark are usually very close together, we scan outward
 * from dot looking for mark. This should save time. Return a standard code.
 * Callers of this routine should be prepared to get an "ABORT" status; we
 * might make this have the confirm thing later.
 */
int getregion (REGION * rp)
{
  register LINE *flp;
  register LINE *blp;
  long fsize;
  long bsize;

  if (curwp->w_markp[0] == (LINE *) NULL)
  {
    mlwrite (TEXT76);
    /* "No mark set in this window" */
    return (FALSE);
  }
  if (curwp->w_dotp == curwp->w_markp[0])
  {
    rp->r_linep = curwp->w_dotp;
    if (curwp->w_doto < curwp->w_marko[0])
    {
      rp->r_offset = curwp->w_doto;
      rp->r_size = (long) (curwp->w_marko[0] - curwp->w_doto);
    }
    else
    {
      rp->r_offset = curwp->w_marko[0];
      rp->r_size = (long) (curwp->w_doto - curwp->w_marko[0]);
    }
    return (TRUE);
  }
  blp = curwp->w_dotp;
  bsize = (long) curwp->w_doto;
  flp = curwp->w_dotp;
  fsize = (long) (llength (flp) - curwp->w_doto + 1);
  while (flp != curbp->b_linep || lback (blp) != curbp->b_linep)
  {
    if (flp != curbp->b_linep)
    {
      flp = lforw (flp);
      if (flp == curwp->w_markp[0])
      {
	rp->r_linep = curwp->w_dotp;
	rp->r_offset = curwp->w_doto;
	rp->r_size = fsize + curwp->w_marko[0];
	return (TRUE);
      }
      fsize += llength (flp) + 1;
    }
    if (lback (blp) != curbp->b_linep)
    {
      blp = lback (blp);
      bsize += llength (blp) + 1;
      if (blp == curwp->w_markp[0])
      {
	rp->r_linep = blp;
	rp->r_offset = curwp->w_marko[0];
	rp->r_size = bsize - curwp->w_marko[0];
	return (TRUE);
      }
    }
  }
  mlwrite (TEXT77);
  /* "Bug: lost mark" */
  return (FALSE);
}
