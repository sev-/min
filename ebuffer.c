/*
 * $Id: ebuffer.c,v 1.4 1995/01/17 12:33:59 sev Exp $
 * 
 * ----------------------------------------------------------
 * 
 * $Log: ebuffer.c,v $
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
 * BUFFER.C:	buffer mgmt. routines MicroEMACS 3.10
 * 
 * Buffer management. Some of the functions are internal, and some are actually
 * attached to user keys. Like everyone else, they set hints for the display
 * system.
 */
#include	<stdio.h>
#include	"estruct.h"
#include	"eproto.h"
#include	"edef.h"
#include	"english.h"

nextbuffer (int f, int n)	  /* switch to the next buffer in the buffer
				   * list */
{
  register BUFFER *bp;		  /* current eligable buffer */
  register int status;

  /* make sure the arg is legit */
  if (f == FALSE)
    n = 1;
  if (n < 1)
    return (FALSE);

  /* cycle thru buffers until n runs out */
  while (n-- > 0)
  {
    bp = getdefb ();
    if (bp == (BUFFER *) NULL)
      return (FALSE);
    status = swbuffer (bp);
    if (status != TRUE)
      return (status);
  }
  return (status);
}

int swbuffer (BUFFER * bp)	  /* make buffer BP current */
{
  register WINDOW *wp;
  register int cmark;		  /* current mark */

  if (--curbp->b_nwnd == 0)
  {				  /* Last use.		 */
    curbp->b_dotp = curwp->w_dotp;
    curbp->b_doto = curwp->w_doto;
    for (cmark = 0; cmark < NMARKS; cmark++)
    {
      curbp->b_markp[cmark] = curwp->w_markp[cmark];
      curbp->b_marko[cmark] = curwp->w_marko[cmark];
    }
    curbp->b_fcol = curwp->w_fcol;
  }
  curbp = bp;			  /* Switch.	       */
  if (curbp->b_active != TRUE)
  {				  /* buffer not active yet */
    /* read it in and activate it */
    readin (curbp->b_fname, TRUE);
    curbp->b_dotp = lforw (curbp->b_linep);
    curbp->b_doto = 0;
    curbp->b_active = TRUE;
  }
  curwp->w_bufp = bp;
  curwp->w_linep = bp->b_linep;	  /* For macros, ignored. */
  curwp->w_flag |= WFMODE | WFFORCE | WFHARD;	/* Quite nasty. 	 */
  if (bp->b_nwnd++ == 0)
  {				  /* First use.		 */
    curwp->w_dotp = bp->b_dotp;
    curwp->w_doto = bp->b_doto;
    for (cmark = 0; cmark < NMARKS; cmark++)
    {
      curwp->w_markp[cmark] = bp->b_markp[cmark];
      curwp->w_marko[cmark] = bp->b_marko[cmark];
    }
    curwp->w_fcol = bp->b_fcol;
  }
  else
  {
    wp = wheadp;		  /* Look for old */
    while (wp != (WINDOW *) NULL)
    {
      if (wp != curwp && wp->w_bufp == bp)
      {
	curwp->w_dotp = wp->w_dotp;
	curwp->w_doto = wp->w_doto;
	for (cmark = 0; cmark < NMARKS; cmark++)
	{
	  curwp->w_markp[cmark] = wp->w_markp[cmark];
	  curwp->w_marko[cmark] = wp->w_marko[cmark];
	}
	curwp->w_fcol = wp->w_fcol;
	break;
      }
      wp = wp->w_wndp;
    }
  }

  return (TRUE);
}

BUFFER *getdefb (void)		  /* get the default buffer for a use or kill */
{
  BUFFER *bp;			  /* default buffer */

  /* Find the next buffer, which will be the default */
  bp = curbp->b_bufp;

  /* cycle through the buffers to find an eligable one */
  while (bp == (BUFFER *) NULL || bp->b_flag & BFINVS)
  {
    if (bp == (BUFFER *) NULL)
      bp = bheadp;
    else
      bp = bp->b_bufp;

    /* don't get caught in an infinite loop! */
    if (bp == curbp)
    {
      bp = (BUFFER *) NULL;
      break;
    }
  }
  return (bp);
}

int zotbuf (BUFFER * bp)	  /* kill the buffer pointed to by bp */
{
  register BUFFER *bp1;
  register BUFFER *bp2;
  register int s;

  if (bp->b_nwnd != 0)
  {				  /* Error if on screen.	 */
    mlwrite (TEXT28);
    /* "Buffer is being displayed" */
    return (FALSE);
  }
  if ((s = bclear (bp)) != TRUE)  /* Blow text away.      */
    return (s);
  free ((char *) bp->b_linep);	  /* Release header line. */
  bp1 = (BUFFER *) NULL;	  /* Find the header.	 */
  bp2 = bheadp;
  while (bp2 != bp)
  {
    bp1 = bp2;
    bp2 = bp2->b_bufp;
  }
  bp2 = bp2->b_bufp;		  /* Next one in chain.   */
  if (bp1 == (BUFFER *) NULL)	  /* Unlink it.		 */
    bheadp = bp2;
  else
    bp1->b_bufp = bp2;
  free ((char *) bp);		  /* Release buffer block */
  return (TRUE);
}

/*
 * Look through the list of buffers. Return TRUE if there are any changed
 * buffers. Buffers that hold magic internal stuff are not considered; who
 * cares if the list of buffer names is hacked. Return FALSE if no buffers
 * have been changed.
 */
int anycb (void)
{
  register BUFFER *bp;

  bp = bheadp;
  while (bp != (BUFFER *) NULL)
  {
    if ((bp->b_flag & BFINVS) == 0 && (bp->b_flag & BFCHG) != 0)
      return (TRUE);
    bp = bp->b_bufp;
  }
  return (FALSE);
}

/*
 * Find a buffer, by name. Return a pointer to the BUFFER structure
 * associated with it. If the buffer is not found and the "cflag" is TRUE,
 * create it. The "bflag" is the settings for the flags in in buffer.
 */
BUFFER *bfind (char *bname, int cflag, int bflag)
/* register char *bname;	   name of buffer to find */
/* int cflag;			   create it if not found? */
/* int bflag;			   bit settings for a new buffer */
{
  register BUFFER *bp;
  register BUFFER *sb;		  /* buffer to insert after */
  register LINE *lp;
  int cmark;			  /* current mark */

  bp = bheadp;
  while (bp != (BUFFER *) NULL)
  {
    if (strcmp (bname, bp->b_bname) == 0)
      return (bp);
    bp = bp->b_bufp;
  }
  if (cflag != FALSE)
  {
    if ((bp = (BUFFER *) malloc (sizeof (BUFFER))) == (BUFFER *) NULL)
      return ((BUFFER *) NULL);
    if ((lp = lalloc (0)) == (LINE *) NULL)
    {
      free ((char *) bp);
      return ((BUFFER *) NULL);
    }
    /* find the place in the list to insert this buffer */
    if (bheadp == (BUFFER *) NULL || strcmp (bheadp->b_bname, bname) > 0)
    {
      /* insert at the beginning */
      bp->b_bufp = bheadp;
      bheadp = bp;
    }
    else
    {
      sb = bheadp;
      while (sb->b_bufp != (BUFFER *) NULL)
      {
	if (strcmp (sb->b_bufp->b_bname, bname) > 0)
	  break;
	sb = sb->b_bufp;
      }

      /* and insert it */
      bp->b_bufp = sb->b_bufp;
      sb->b_bufp = bp;
    }

    /* and set up the other buffer fields */
    bp->b_topline = (LINE *) NULL;
    bp->b_botline = (LINE *) NULL;
    bp->b_active = TRUE;
    bp->b_dotp = lp;
    bp->b_doto = 0;
    for (cmark = 0; cmark < NMARKS; cmark++)
    {
      bp->b_markp[cmark] = (LINE *) NULL;
      bp->b_marko[cmark] = 0;
    }
    bp->b_fcol = 0;
    bp->b_flag = bflag;
    bp->b_mode = gmode;
    bp->b_nwnd = 0;
    bp->b_linep = lp;
    strcpy (bp->b_fname, "");
    strcpy (bp->b_bname, bname);
    lp->l_fp = lp;
    lp->l_bp = lp;
  }
  return (bp);
}

/*
 * This routine blows away all of the text in a buffer. If the buffer is
 * marked as changed then we ask if it is ok to blow it away; this is to save
 * the user the grief of losing text. The window chain is nearly always wrong
 * if this gets called; the caller must arrange for the updates that are
 * required. Return TRUE if everything looks good.
 */
int bclear (BUFFER * bp)
{
  register LINE *lp;
  register int s;
  int cmark;			  /* current mark */

  if ((bp->b_flag & BFINVS) == 0  /* Not scratch buffer.	 */
      && (bp->b_flag & BFCHG) != 0/* Something changed	 */
      && (s = mlyesno (TEXT32)) != TRUE)
    /* "Discard changes" */
    return (s);
  bp->b_flag &= ~BFCHG;		  /* Not changed	       */
  while ((lp = lforw (bp->b_linep)) != bp->b_linep)
    lfree (lp);
  bp->b_dotp = bp->b_linep;	  /* Fix "."	       */
  bp->b_doto = 0;
  for (cmark = 0; cmark < NMARKS; cmark++)
  {
    bp->b_markp[cmark] = (LINE *) NULL;	/* Invalidate "mark"    */
    bp->b_marko[cmark] = 0;
  }
  bp->b_fcol = 0;
  return (TRUE);
}

/*
 * Attach a buffer to a window. The values of dot and mark come from the
 * buffer if the use count is 0. Otherwise, they come from some other window.
 */
int usebuffer (int f, int n)
{
  register BUFFER *bp;		  /* temporary buffer pointer */

  /* get the buffer name to switch to */
  bp = getdefb ();
  bp = getcbuf (TEXT24, bp ? bp->b_bname : mainbuf, TRUE);
  /* "Use buffer" */
  if (!bp)
    return (ABORT);

  /* make it invisable if there is an argument */
  if (f == TRUE)
    bp->b_flag |= BFINVS;

  /* switch to it in any case */
  return (swbuffer (bp));
}

/*
 * Dispose of a buffer, by name. Ask for the name. Look it up (don't get too
 * upset if it isn't there at all!). Get quite upset if the buffer is being
 * displayed. Clear the buffer (ask if the buffer has been changed). Then
 * free the header line and the buffer header. Bound to "C-X K".
 */
int killbuffer (int f, int n)
{
  register BUFFER *bp;		  /* ptr to buffer to dump */

  /* get the buffer name to kill */
  bp = getdefb ();
  bp = getcbuf (TEXT26, bp ? bp->b_bname : mainbuf, TRUE);
  /* "Kill buffer" */
  if (bp == NULL)
    return (ABORT);

  return (zotbuf (bp));
}

int namebuffer (int f, int n)	  /* Rename the current buffer	 */
{
  register BUFFER *bp;		  /* pointer to scan through all buffers */
  char bufn[NBUFN];		  /* buffer to hold buffer name */

  /* prompt for and get the new buffer name */
ask:if (mlreply (TEXT29, bufn, NBUFN) != TRUE)
    /* "Change buffer name to: " */
    return (FALSE);

  /* and check for duplicates */
  bp = bheadp;
  while (bp != NULL)
  {
    if (bp != curbp)
    {
      /* if the names the same */
      if (strcmp (bufn, bp->b_bname) == 0)
	goto ask;		  /* try again */
    }
    bp = bp->b_bufp;		  /* onward */
  }

  strcpy (curbp->b_bname, bufn);  /* copy buffer name to structure */
  upmode ();			  /* make all mode lines replot */
  mlerase ();
  return (TRUE);
}
