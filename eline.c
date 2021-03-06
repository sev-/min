/*
 * $Id: eline.c,v 1.6 1995/10/14 15:46:11 sev Exp $
 * 
 * ----------------------------------------------------------
 * 
 * $Log: eline.c,v $
 * Revision 1.6  1995/10/14 15:46:11  sev
 * Program was in MSDOS and done A _LOT OF_ changes
 *
 * Revision 1.5  1995/01/24  15:40:39  sev
 * Added inverse line while run; play_error; start label; Labels buffer
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
 * The functions in this file are a general set of line management utilities.
 * They are the only routines that touch the text. They also touch the buffer
 * and window structures, to make sure that the necessary updating gets done.
 * There are routines in this file that handle the kill buffer too. It isn't
 * here for any good reason.
 * 
 * Note that this code only updates the dot and mark values in the window list.
 * Since all the code acts on the current window, the buffer that we are
 * editing must be being displayed, which means that "b_nwnd" is non zero,
 * which means that the dot and mark values in the buffer headers are
 * nonsense.
 */

#include	<stdio.h>
#include	"estruct.h"
#include	"eproto.h"
#include	"edef.h"
#include	"english.h"

#define BSIZE(a)	(a + NBLOCK - 1) & (~(NBLOCK - 1))

/*
 * This routine allocates a block of memory large enough to hold a LINE
 * containing "used" characters. Return a pointer to the new block, or NULL
 * if there isn't any memory left. Print a message in the message line if no
 * space.
 */
LINE *lalloc (int used)
{
  register LINE *lp;

  if ((lp = (LINE *) malloc (sizeof (LINE) + used)) == (LINE *) NULL)
  {
    mlwrite (TEXT99);
    /* "[OUT OF MEMORY]" */
    return ((LINE *) NULL);
  }
  lp->l_size = used;
  lp->l_used = used;
  return (lp);
}

/*
 * Delete line "lp". Fix all of the links that might point at it (they are
 * moved to offset 0 of the next line. Unlink the line from whatever buffer
 * it might be in. Release the memory. The buffers are updated too; the magic
 * conditions described in the above comments don't hold here.
 */
void lfree (LINE * lp)
{
  register BUFFER *bp;
  register WINDOW *wp;
  register int cmark;		  /* current mark */

  wp = wheadp;
  while (wp != (WINDOW *) NULL)
  {
    if (wp->w_linep == lp)
      wp->w_linep = lp->l_fp;
    if (wp->w_dotp == lp)
    {
      wp->w_dotp = lp->l_fp;
      wp->w_doto = 0;
    }
    for (cmark = 0; cmark < NMARKS; cmark++)
    {
      if (wp->w_markp[cmark] == lp)
      {
	wp->w_markp[cmark] = lp->l_fp;
	wp->w_marko[cmark] = 0;
      }
    }
    wp = wp->w_wndp;
  }
  bp = bheadp;
  while (bp != (BUFFER *) NULL)
  {
    if (bp->b_nwnd == 0)
    {
      if (bp->b_dotp == lp)
      {
	bp->b_dotp = lp->l_fp;
	bp->b_doto = 0;
      }
      for (cmark = 0; cmark < NMARKS; cmark++)
      {
	if (bp->b_markp[cmark] == lp)
	{
	  bp->b_markp[cmark] = lp->l_fp;
	  bp->b_marko[cmark] = 0;
	}
      }
    }
    bp = bp->b_bufp;
  }
  lp->l_bp->l_fp = lp->l_fp;
  lp->l_fp->l_bp = lp->l_bp;
  free ((char *) lp);
}

/*
 * This routine gets called when a character is changed in place in the
 * current buffer. It updates all of the required flags in the buffer and
 * window system. The flag used is passed as an argument; if the buffer is
 * being displayed in more than 1 window we change EDIT t HARD. Set MODE if
 * the mode line needs to be updated (the "*" has to be set).
 */
void lchange (int flag)
{
  register WINDOW *wp;

  if (curbp->b_nwnd != 1)	  /* Ensure hard. 	 */
    flag = WFHARD;
  set_need_compile ();	/* added by Sev */
  if ((curbp->b_flag & BFCHG) == 0)
  {				  /* First change, so     */
    flag |= WFMODE;		  /* update mode lines.	 */
    curbp->b_flag |= BFCHG;
  }

  /* make sure all the needed windows get this flag */
  wp = wheadp;
  while (wp != (WINDOW *) NULL)
  {
    if (wp->w_bufp == curbp)
      wp->w_flag |= flag;
    wp = wp->w_wndp;
  }
}

int insspace (int f, int n)	  /* insert spaces forward into text */
{
  linsert (n, ' ');
  backchar (f, n);
}

/* linstr -- Insert a string at the current point */
int linstr (char *instr)
{
  register int status = TRUE;

  if (instr != (char *) NULL)
    while (*instr && status == TRUE)
    {
      status = ((*instr == '\r') ? lnewline () : linsert (1, *instr));

      /* Insertion error? */
      if (status != TRUE)
      {
	mlwrite (TEXT168);
	/* "%%Can not insert string" */
	break;
      }
      instr++;
    }
  return (status);
}

/*
 * Insert "n" copies of the character "c" at the current location of dot. In
 * the easy case all that happens is the text is stored in the line. In the
 * hard case, the line has to be reallocated. When the window list is
 * updated, take special care; I screwed it up once. You always update dot in
 * the current window. You update mark, and a dot in another window, if it is
 * greater than the place where you did the insert. Return TRUE if all is
 * well, and FALSE on errors.
 */
int linsert (int n, char c)
{
  register char *cp1;
  register char *cp2;
  register LINE *lp1;
  register LINE *lp2;
  register LINE *lp3;
  register int doto;
  register int i;
  register WINDOW *wp;
  int cmark;			  /* current mark */

  if (curbp->b_mode & MDVIEW)	  /* don't allow this command if	 */
    return (rdonly ());		  /* we are in read only mode	 */
  lchange (WFEDIT);
  lp1 = curwp->w_dotp;		  /* Current line	       */
  if (lp1 == curbp->b_linep)
  {				  /* At the end: special	 */
    if (curwp->w_doto != 0)
    {
      mlwrite (TEXT170);
      /* "bug: linsert" */
      return (FALSE);
    }
    if ((lp2 = lalloc (BSIZE (n))) == (LINE *) NULL)	/* Allocate new line    */
      return (FALSE);
    lp2->l_used = n;
    lp3 = lp1->l_bp;		  /* Previous line	      */
    lp3->l_fp = lp2;		  /* Link in		 */
    lp2->l_fp = lp1;
    lp1->l_bp = lp2;
    lp2->l_bp = lp3;
    for (i = 0; i < n; ++i)
      lp2->l_text[i] = c;
    curwp->w_dotp = lp2;
    curwp->w_doto = n;
    return (TRUE);
  }
  doto = curwp->w_doto;		  /* Save for later.      */
  if (lp1->l_used + n > lp1->l_size)
  {				  /* Hard: reallocate	 */
    if ((lp2 = lalloc (BSIZE (lp1->l_used + n))) == (LINE *) NULL)
      return (FALSE);
    lp2->l_used = lp1->l_used + n;
    cp1 = &lp1->l_text[0];
    cp2 = &lp2->l_text[0];
    while (cp1 != &lp1->l_text[doto])
      *cp2++ = *cp1++;
    cp2 += n;
    while (cp1 != &lp1->l_text[lp1->l_used])
      *cp2++ = *cp1++;
    lp1->l_bp->l_fp = lp2;
    lp2->l_fp = lp1->l_fp;
    lp1->l_fp->l_bp = lp2;
    lp2->l_bp = lp1->l_bp;
    free ((char *) lp1);
  }
  else
  {				  /* Easy: in place	     */
    lp2 = lp1;			  /* Pretend new line	 */
    lp2->l_used += n;
    cp2 = &lp1->l_text[lp1->l_used];
    cp1 = cp2 - n;
    while (cp1 != &lp1->l_text[doto])
      *--cp2 = *--cp1;
  }
  for (i = 0; i < n; ++i)	  /* Add the characters   */
    lp2->l_text[doto + i] = c;
  wp = wheadp;			  /* Update windows	 */
  while (wp != (WINDOW *) NULL)
  {
    if (wp->w_linep == lp1)
      wp->w_linep = lp2;
    if (wp->w_dotp == lp1)
    {
      wp->w_dotp = lp2;
      if (wp == curwp || wp->w_doto > doto)
	wp->w_doto += n;
    }
    for (cmark = 0; cmark < NMARKS; cmark++)
    {
      if (wp->w_markp[cmark] == lp1)
      {
	wp->w_markp[cmark] = lp2;
	if (wp->w_marko[cmark] > doto)
	  wp->w_marko[cmark] += n;
      }
    }
    wp = wp->w_wndp;
  }
  return (TRUE);
}

/*
 * Insert a newline into the buffer at the current location of dot in the
 * current window. The funny ass-backwards way it does things is not a botch;
 * it just makes the last line in the file not a special case. Return TRUE if
 * everything works out and FALSE on error (memory allocation failure). The
 * update of dot and mark is a bit easier then in the above case, because the
 * split forces more updating.
 */
int lnewline (void)
{
  register char *cp1;
  register char *cp2;
  register LINE *lp1;
  register LINE *lp2;
  register int doto;
  register WINDOW *wp;
  int cmark;			  /* current mark */

  if (curbp->b_mode & MDVIEW)	  /* don't allow this command if	 */
    return (rdonly ());		  /* we are in read only mode	 */
  lchange (WFHARD);
  lp1 = curwp->w_dotp;		  /* Get the address and  */
  doto = curwp->w_doto;		  /* offset of "."	 */
  if ((lp2 = lalloc (doto)) == (LINE *) NULL)	/* New first half line	 */
    return (FALSE);
  cp1 = &lp1->l_text[0];	  /* Shuffle text around  */
  cp2 = &lp2->l_text[0];
  while (cp1 != &lp1->l_text[doto])
    *cp2++ = *cp1++;
  cp2 = &lp1->l_text[0];
  while (cp1 != &lp1->l_text[lp1->l_used])
    *cp2++ = *cp1++;
  lp1->l_used -= doto;
  lp2->l_bp = lp1->l_bp;
  lp1->l_bp = lp2;
  lp2->l_bp->l_fp = lp2;
  lp2->l_fp = lp1;
  wp = wheadp;			  /* Windows		  */
  while (wp != (WINDOW *) NULL)
  {
    if (wp->w_linep == lp1)
      wp->w_linep = lp2;
    if (wp->w_dotp == lp1)
    {
      if (wp->w_doto < doto)
	wp->w_dotp = lp2;
      else
	wp->w_doto -= doto;
    }
    for (cmark = 0; cmark < NMARKS; cmark++)
    {
      if (wp->w_markp[cmark] == lp1)
      {
	if (wp->w_marko[cmark] < doto)
	  wp->w_markp[cmark] = lp2;
	else
	  wp->w_marko[cmark] -= doto;
      }
    }
    wp = wp->w_wndp;
  }
  return (TRUE);
}

/*
 * This function deletes "n" bytes, starting at dot. It understands how to
 * deal with end of lines, etc. It returns TRUE if all of the characters were
 * deleted, and FALSE if they were not (because dot ran into the end of the
 * buffer. The "kflag" is TRUE if the text should be put in the kill buffer.
 */
int ldelete (long n, int kflag)
/* long n;			# of chars to delete */
/* int kflag;			put killed text in kill buffer flag */
{
  register char *cp1;
  register char *cp2;
  register LINE *dotp;
  register int doto;
  register int chunk;
  register WINDOW *wp;
  int cmark;			  /* current mark */

  if (curbp->b_mode & MDVIEW)	  /* don't allow this command if	 */
    return (rdonly ());		  /* we are in read only mode	 */
  while (n != 0)
  {
    dotp = curwp->w_dotp;
    doto = curwp->w_doto;
    if (dotp == curbp->b_linep)	  /* Hit end of buffer.   */
      return (FALSE);
    chunk = dotp->l_used - doto;  /* Size of chunk.       */
    if (chunk > n)
      chunk = n;
    if (chunk == 0)
    {				  /* End of line, merge.  */
      lchange (WFHARD);
      if (ldelnewline () == FALSE
	  || (kflag != FALSE && kinsert ('\r') == FALSE))
	return (FALSE);
      --n;
      continue;
    }
    lchange (WFEDIT);
    cp1 = &dotp->l_text[doto];	  /* Scrunch text.	      */
    cp2 = cp1 + chunk;
    if (kflag != FALSE)
    {				  /* Kill?		      */
      while (cp1 != cp2)
      {
	if (kinsert (*cp1) == FALSE)
	  return (FALSE);
	++cp1;
      }
      cp1 = &dotp->l_text[doto];
    }
    while (cp2 != &dotp->l_text[dotp->l_used])
      *cp1++ = *cp2++;
    dotp->l_used -= chunk;
    wp = wheadp;		  /* Fix windows 	 */
    while (wp != (WINDOW *) NULL)
    {
      if (wp->w_dotp == dotp && wp->w_doto >= doto)
      {
	wp->w_doto -= chunk;
	if (wp->w_doto < doto)
	  wp->w_doto = doto;
      }
      for (cmark = 0; cmark < NMARKS; cmark++)
      {
	if (wp->w_markp[cmark] == dotp && wp->w_marko[cmark] >= doto)
	{
	  wp->w_marko[cmark] -= chunk;
	  if (wp->w_marko[cmark] < doto)
	    wp->w_marko[cmark] = doto;
	}
      }
      wp = wp->w_wndp;
    }
    n -= chunk;
  }
  return (TRUE);
}

/*
 * Delete a newline. Join the current line with the next line. If the next
 * line is the magic header line always return TRUE; merging the last line
 * with the header line can be thought of as always being a successful
 * operation, even if nothing is done, and this makes the kill buffer work
 * "right". Easy cases can be done by shuffling data around. Hard cases
 * require that lines be moved about in memory. Return FALSE on error and
 * TRUE if all looks ok. Called by "ldelete" only.
 */
int ldelnewline (void)
{
  register char *cp1;
  register char *cp2;
  register LINE *lp1;
  register LINE *lp2;
  register LINE *lp3;
  register WINDOW *wp;
  int cmark;			  /* current mark */

  if (curbp->b_mode & MDVIEW)	  /* don't allow this command if	 */
    return (rdonly ());		  /* we are in read only mode	 */
  lp1 = curwp->w_dotp;
  lp2 = lp1->l_fp;
  if (lp2 == curbp->b_linep)
  {				  /* At the buffer end.   */
    if (lp1->l_used == 0)	  /* Blank line.		 */
      lfree (lp1);
    return (TRUE);
  }
  if (lp2->l_used <= lp1->l_size - lp1->l_used)
  {
    cp1 = &lp1->l_text[lp1->l_used];
    cp2 = &lp2->l_text[0];
    while (cp2 != &lp2->l_text[lp2->l_used])
      *cp1++ = *cp2++;
    wp = wheadp;
    while (wp != (WINDOW *) NULL)
    {
      if (wp->w_linep == lp2)
	wp->w_linep = lp1;
      if (wp->w_dotp == lp2)
      {
	wp->w_dotp = lp1;
	wp->w_doto += lp1->l_used;
      }
      for (cmark = 0; cmark < NMARKS; cmark++)
      {
	if (wp->w_markp[cmark] == lp2)
	{
	  wp->w_markp[cmark] = lp1;
	  wp->w_marko[cmark] += lp1->l_used;
	}
      }
      wp = wp->w_wndp;
    }
    lp1->l_used += lp2->l_used;
    lp1->l_fp = lp2->l_fp;
    lp2->l_fp->l_bp = lp1;
    free ((char *) lp2);
    return (TRUE);
  }
  if ((lp3 = lalloc (lp1->l_used + lp2->l_used)) == (LINE *) NULL)
    return (FALSE);
  cp1 = &lp1->l_text[0];
  cp2 = &lp3->l_text[0];
  while (cp1 != &lp1->l_text[lp1->l_used])
    *cp2++ = *cp1++;
  cp1 = &lp2->l_text[0];
  while (cp1 != &lp2->l_text[lp2->l_used])
    *cp2++ = *cp1++;
  lp1->l_bp->l_fp = lp3;
  lp3->l_fp = lp2->l_fp;
  lp2->l_fp->l_bp = lp3;
  lp3->l_bp = lp1->l_bp;
  wp = wheadp;
  while (wp != (WINDOW *) NULL)
  {
    if (wp->w_linep == lp1 || wp->w_linep == lp2)
      wp->w_linep = lp3;
    if (wp->w_dotp == lp1)
      wp->w_dotp = lp3;
    else if (wp->w_dotp == lp2)
    {
      wp->w_dotp = lp3;
      wp->w_doto += lp1->l_used;
    }
    for (cmark = 0; cmark < NMARKS; cmark++)
    {
      if (wp->w_markp[cmark] == lp1)
	wp->w_markp[cmark] = lp3;
      else if (wp->w_markp[cmark] == lp2)
      {
	wp->w_markp[cmark] = lp3;
	wp->w_marko[cmark] += lp1->l_used;
      }
    }
    wp = wp->w_wndp;
  }
  free ((char *) lp1);
  free ((char *) lp2);
  return (TRUE);
}

/*
 * Delete all of the text saved in the kill buffer. Called by commands when a
 * new kill context is being created. The kill buffer array is released, just
 * in case the buffer has grown to immense size. No errors.
 */
void kdelete (void)
{
  KILL *kp;			  /* ptr to scan kill buffer chunk list */

  if (kbufh != (KILL *) NULL)
  {

    /* first, delete all the chunks */
    kbufp = kbufh;
    while (kbufp != (KILL *) NULL)
    {
      kp = kbufp->d_next;
      free (kbufp);
      kbufp = kp;
    }

    /* and reset all the kill buffer pointers */
    kbufh = kbufp = (KILL *) NULL;
    kused = KBLOCK;
  }
}

/*
 * Insert a character to the kill buffer, allocating new chunks as needed.
 * Return TRUE if all is well, and FALSE on errors.
 */
int kinsert (char c)
{
  KILL *nchunk;			  /* ptr to newly malloced chunk */

  /* check to see if we need a new chunk */
  if (kused >= KBLOCK)
  {
    if ((nchunk = (KILL *) malloc (sizeof (KILL))) == (KILL *) NULL)
      return (FALSE);
    if (kbufh == (KILL *) NULL)	  /* set head ptr if first time */
      kbufh = nchunk;
    if (kbufp != (KILL *) NULL)	  /* point the current to this new one */
      kbufp->d_next = nchunk;
    kbufp = nchunk;
    kbufp->d_next = (KILL *) NULL;
    kused = 0;
  }

  /* and now insert the character */
  kbufp->d_chunk[kused++] = c;
  return (TRUE);
}

/*
 * Yank text back from the kill buffer. This is really easy. All of the work
 * is done by the standard insert routines. All you do is run the loop, and
 * check for errors. Bound to "C-Y".
 */
int yank (int f, int n)
{
  register int c;
  register int i;
  register char *sp;		  /* pointer into string to insert */
  KILL *kp;			  /* pointer into kill buffer */

  if (curbp->b_mode & MDVIEW)	  /* don't allow this command if	 */
    return (rdonly ());		  /* we are in read only mode	 */
  if (n < 0)
    return (FALSE);
  /* make sure there is something to yank */
  if (kbufh == (KILL *) NULL)
    return (TRUE);		  /* not an error, just nothing */

  /* for each time.... */
  while (n--)
  {
    kp = kbufh;
    while (kp != (KILL *) NULL)
    {
      if (kp->d_next == (KILL *) NULL)
	i = kused;
      else
	i = KBLOCK;
      sp = kp->d_chunk;
      while (i--)
      {
	if ((c = *sp++) == '\r')
	{
	  if (lnewline () == FALSE)
	    return (FALSE);
	}
	else
	{
	  if (linsert (1, c) == FALSE)
	    return (FALSE);
	}
      }
      kp = kp->d_next;
    }
  }
  return (TRUE);
}

/*
 * lover -- Overwrite a string at the current point
 */
int lover (char *ostr)
{
  register int status = TRUE;

  if (ostr != NULL)
    while (*ostr && status == TRUE)
    {
      status = ((*ostr == '\r') ? lnewline () : lowrite (*ostr));

      /* Insertion error? */
      if (status != TRUE)
      {
	mlwrite (TEXT172);
	/* "%%Out of memory while overwriting" */
	break;
      }
      ostr++;
    }
  return (status);
}

/*
 * Overwrite a character into the current line at the current position
 * 
 */
int lowrite (char c)
{
  if (curwp->w_doto < curwp->w_dotp->l_used &&
      (lgetc (curwp->w_dotp, curwp->w_doto) != '\t' ||
       (curwp->w_doto) % 8 == 7))
    ldelete (1L, FALSE);
  return (linsert (1, c));
}
