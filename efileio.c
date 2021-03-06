/*
 * $Id: efileio.c,v 1.5 1995/10/14 15:46:11 sev Exp $
 * 
 * ----------------------------------------------------------
 * 
 * $Log: efileio.c,v $
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
 * FILEIO.C:	Low level file i/o routines MicroEMACS 3.10
 * 
 * The routines in this file read and write ASCII files from the disk. All of
 * the knowledge about files are here.
 */

#include	<stdio.h>
#include	"estruct.h"
#include	"eproto.h"
#include	"edef.h"
#include	"english.h"

FILE *ffp;			  /* File pointer, all functions. */
static int eofflag;		  /* end-of-file flag */

/* Open a file for reading. */
int ffropen (char *fn)
{
  if ((ffp = fopen (fn, "r")) == (FILE *) NULL)
    return (FIOFNF);
  eofflag = FALSE;
  return (FIOSUC);
}

/*
 * Open a file for writing. Return TRUE if all is well, and FALSE on error
 * (cannot create).
 */
int ffwopen (char *fn)
{
  if ((ffp = fopen (fn, "w")) == (FILE *) NULL)
  {
    mlwrite (TEXT155);
    /* "Cannot open file for writing" */
    return (FIOERR);
  }
  return (FIOSUC);
}

/* Close a file. Should look at the status in all systems. */
int ffclose (void)
{
  /* free this since we do not need it anymore */
  if (fline)
  {
    free (fline);
    fline = (char *) NULL;
  }

  if (fclose (ffp) != FALSE)
  {
    mlwrite (TEXT156);
    /* "Error closing file" */
    return (FIOERR);
  }
  return (FIOSUC);
}

/*
 * Write a line to the already opened file. The "buf" points to the buffer,
 * and the "nbuf" is its length, less the free newline. Return the status.
 * Check only at the newline.
 */
int ffputline (char *buf, int nbuf)
{
  register int i;

  for (i = 0; i < nbuf; ++i)
    putc (buf[i], ffp);

  putc ('\n', ffp);

  if (ferror (ffp))
  {
    mlwrite (TEXT157);
    /* "Write I/O error" */
    return (FIOERR);
  }

  return (FIOSUC);
}

/*
 * Read a line from a file, and store the bytes in the supplied buffer. The
 * "nbuf" is the length of the buffer. Complain about long lines and lines at
 * the end of the file that don't have a newline present. Check for I/O
 * errors too. Return status.
 */
int ffgetline (void)
{
  register int c;		  /* current character read */
  register int i;		  /* current index into fline */
  register char *tmpline;	  /* temp storage for expanding line */

  /* if we are at the end...return it */
  if (eofflag)
    return (FIOEOF);

  /* dump fline if it ended up too big */
  if (flen > NSTRING && fline != (char *) NULL)
  {
    free (fline);
    fline = (char *) NULL;
  }

  /* if we don't have an fline, allocate one */
  if (fline == (char *) NULL)
    if ((fline = malloc (flen = NSTRING)) == (char *) NULL)
      return (FIOMEM);

  /* read the line in */
  i = 0;
  while ((c = getc (ffp)) != EOF && c != '\n')
  {
    fline[i++] = c;
    /* if it's longer, get more room */
    if (i >= flen)
    {
      if ((tmpline = malloc (flen + NSTRING)) == (char *) NULL)
	return (FIOMEM);
      bytecopy (tmpline, fline, flen);
      flen += NSTRING;
      free (fline);
      fline = tmpline;
    }
  }

  /* test for any errors that may have occured */
  if (c == EOF)
  {
    if (ferror (ffp))
    {
      mlwrite (TEXT158);
      /* "File read error" */
      return (FIOERR);
    }

    if (i != 0)
      eofflag = TRUE;
    else
      return (FIOEOF);
  }

  /* terminate and decrypt the string */
  fline[i] = 0;
  return (FIOSUC);
}

int fexist (char *fname)	  /* does <fname> exist on disk? */
{
  FILE *fp;

  /* try to open the file for reading */
  fp = fopen (fname, "r");

  /* if it fails, just return false! */
  if (fp == (FILE *) NULL)
    return (FALSE);

  /* otherwise, close it and report true */
  fclose (fp);
  return (TRUE);
}
