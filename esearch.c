/*
 * $Id: esearch.c,v 1.4 1995/01/17 12:33:59 sev Exp $
 * 
 * ----------------------------------------------------------
 * 
 * $Log: esearch.c,v $
 * Revision 1.4  1995/01/17 12:33:59  sev
 * Now run screen is done
 * Revision 1.3  1995/01/14  15:08:09  sev Menu works right.
 * Compiler also. Revision 1.2  1995/01/07  20:03:14  sev Maked indent and
 * some editor changes Revision 1.1  1995/01/06  21:45:10  sev Initial
 * revision
 * 
 * 
 */

#include	<stdio.h>
#include	"estruct.h"
#include	"eproto.h"
#include	"edef.h"
#include	"english.h"

static int patlenadd;
static int deltaf[HICHAR], deltab[HICHAR];
static int lastchfjump, lastchbjump;

/*
 * forwsearch -- Search forward.  Get a search string from the user, and
 * search for the string.	If found, reset the "." to be just after the
 * match string, and (perhaps) repaint the display.
 */
int forwsearch (int f, int n)
{
  register int status = TRUE;

  /*
   * If n is negative, search backwards. Otherwise proceed by asking for the
   * search string.
   */
  if (n < 0)
    return (backsearch (f, -n));

  /*
   * Ask the user for the text of a pattern.  If the response is TRUE
   * (responses other than FALSE are possible), search for the pattern for as
   * long as n is positive (n == 0 will go through once, which is just fine).
   */
  if ((status = readpattern (TEXT78, &pat[0], TRUE)) == TRUE)
  {
    /* "Search" */
    do
    {
      status = scanner (&pat[0], FORWARD, PTEND);
    } while ((--n > 0) && status);

    /* Save away the match, or complain if not there. */
    if (status != TRUE)
      mlwrite (TEXT79);
    /* "Not found" */
  }
  return (status);
}

/*
 * forwhunt -- Search forward for a previously acquired search string. If
 * found, reset the "." to be just after the match string, and (perhaps)
 * repaint the display.
 */
forwhunt (int f, int n)
{
  register int status = TRUE;

  if (n < 0)			  /* search backwards */
    return (backhunt (f, -n));

  /*
   * Make sure a pattern exists, or that we didn't switch into MAGIC mode
   * until after we entered the pattern.
   */
  if (pat[0] == '\0')
  {
    mlwrite (TEXT80);
    /* "No pattern set" */
    return FALSE;
  }

  /*
   * Search for the pattern for as long as n is positive (n == 0 will go
   * through once, which is just fine).
   */
  do
  {
    status = scanner (&pat[0], FORWARD, PTEND);
  } while ((--n > 0) && status);

  /* Save away the match, or complain if not there. */
  if (status != TRUE)
    mlwrite (TEXT79);
  /* "Not found" */

  return (status);
}

/*
 * backsearch -- Reverse search.  Get a search string from the user, and
 * search, starting at "." and proceeding toward the front of the buffer. If
 * found "." is left pointing at the first character of the pattern (the last
 * character that was matched).
 */
int backsearch (int f, int n)
{
  register int status = TRUE;

  /*
   * If n is negative, search forwards. Otherwise proceed by asking for the
   * search string.
   */
  if (n < 0)
    return (forwsearch (f, -n));

  /*
   * Ask the user for the text of a pattern.  If the response is TRUE
   * (responses other than FALSE are possible), search for the pattern for as
   * long as n is positive (n == 0 will go through once, which is just fine).
   */
  if ((status = readpattern (TEXT81, &pat[0], TRUE)) == TRUE)
  {
    /* "Reverse search" */
    do
    {
      status = scanner (&tap[0], REVERSE, PTBEG);
    } while ((--n > 0) && status);

    /* Save away the match, or complain if not there. */
    if (status != TRUE)
      mlwrite (TEXT79);
    /* "Not found" */
  }
  return (status);
}

/*
 * backhunt -- Reverse search for a previously acquired search string,
 * starting at "." and proceeding toward the front of the buffer. If found
 * "." is left pointing at the first character of the pattern (the last
 * character that was matched).
 */
int backhunt (int f, int n)
{
  register int status = TRUE;

  if (n < 0)
    return (forwhunt (f, -n));

  /*
   * Make sure a pattern exists, or that we didn't switch into MAGIC mode
   * until after we entered the pattern.
   */
  if (tap[0] == '\0')
  {
    mlwrite (TEXT80);
    /* "No pattern set" */
    return FALSE;
  }

  /*
   * Go search for it for as long as n is positive (n == 0 will go through
   * once, which is just fine).
   */
  do
  {
    status = scanner (&tap[0], REVERSE, PTBEG);
  } while ((--n > 0) && status);

  /* Save away the match, or complain if not there. */
  if (status != TRUE)
    mlwrite (TEXT79);
  /* "Not found" */

  return (status);
}


/*
 * scanner -- Search for a pattern in either direction.  If found, reset the
 * "." to be at the start or just after the match string, and (perhaps)
 * repaint the display. Fast version using simplified version of Boyer and
 * Moore Software-Practice and Experience, vol 10, 501-506 (1980)
 */
int scanner (char *patrn, int direct, int beg_or_end)
/* char *patrn;			string to scan for */
/* int direct;			which way to go. */

/*
 * int beg_or_end;		  /* put point at beginning or end of
 * pattern.
 */
{
  register int c;		  /* character at current position */
  register char *patptr;	  /* pointer into pattern */
  LINE *curline;		  /* current line during scan */
  int curoff;			  /* position within current line */
  LINE *scanline;		  /* current line during scanning */
  int scanoff;			  /* position in scanned line */
  int jump;			  /* next offset */

  /*
   * If we are going in reverse, then the 'end' is actually the beginning of
   * the pattern.  Toggle it.
   */
  beg_or_end ^= direct;

  /* Set up local pointers to global ".". */
  curline = curwp->w_dotp;
  curoff = curwp->w_doto;

  /*
   * Scan each character until we hit the head link record. Get the character
   * resolving newlines, offset by the pattern length, i.e. the last
   * character of the potential match.
   */
  jump = patlenadd;

  while (!fbound (jump, &curline, &curoff, direct))
  {

    /*
     * Save the current position in case we match the search string at this
     * point.
     */
    matchline = curline;
    matchoff = curoff;

    /* Setup scanning pointers. */
    scanline = curline;
    scanoff = curoff;
    patptr = &patrn[0];

    /* Scan through the pattern for a match. */
    while ((c = *patptr++) != '\0')
      if (!eq ((int) c, nextch (&scanline, &scanoff, direct)))
      {
	jump = (direct == FORWARD)
	     ? lastchfjump
	     : lastchbjump;
	goto fail;
      }

    /* A SUCCESSFULL MATCH!!! reset the global "." pointers */
    if (beg_or_end == PTEND)	  /* at end of string */
    {
      curwp->w_dotp = scanline;
      curwp->w_doto = scanoff;
    }
    else
      /* at beginning of string */
    {
      curwp->w_dotp = matchline;
      curwp->w_doto = matchoff;
    }

    curwp->w_flag |= WFMOVE;	  /* Flag that we have moved. */
    savematch ();
    return TRUE;

  fail:;			  /* continue to search */
  }

  return FALSE;			  /* We could not find a match */
}

/*
 * fbound -- Return information depending on whether we have hit a boundry
 * (and may therefore search no further) or if a trailing character of the
 * search string has been found.  See boundry() for search restrictions.
 */
int fbound (int jump, LINE ** pcurline, int *pcuroff, int dir)
{
  register int spare, curoff;
  register LINE *curline;

  curline = *pcurline;
  curoff = *pcuroff;

  if (dir == FORWARD)
  {
    while (jump != 0)
    {
      curoff += jump;
      spare = curoff - llength (curline);

      while (spare > 0)
      {
	curline = lforw (curline);/* skip to next line */
	curoff = spare - 1;
	spare = curoff - llength (curline);
	if (curline == curbp->b_linep)
	  return TRUE;		  /* hit end of buffer */
      }

      if (spare == 0)
	jump = deltaf[(int) '\r'];
      else
	jump = deltaf[(int) lgetc (curline, curoff)];
    }

    /* The last character matches, so back up to start of possible match. */
    curoff -= patlenadd;

    while (curoff < 0)
    {
      curline = lback (curline);  /* skip back a line */
      curoff += llength (curline) + 1;
    }

  }
  else
    /* Reverse. */
  {
    jump++;			  /* allow for offset in reverse */
    while (jump != 0)
    {
      curoff -= jump;
      while (curoff < 0)
      {
	curline = lback (curline);/* skip back a line */
	curoff += llength (curline) + 1;
	if (curline == curbp->b_linep)
	  return TRUE;		  /* hit end of buffer */
      }

      if (curoff == llength (curline))
	jump = deltab[(int) '\r'];
      else
	jump = deltab[(int) lgetc (curline, curoff)];
    }

    /* The last character matches, so back up to start of possible match. */
    curoff += matchlen;
    spare = curoff - llength (curline);
    while (spare > 0)
    {
      curline = lforw (curline);  /* skip back a line */
      curoff = spare - 1;
      spare = curoff - llength (curline);
    }
  }

  *pcurline = curline;
  *pcuroff = curoff;
  return FALSE;
}

/*
 * setjtable -- Settting up search delta forward and delta backward tables.
 * The reverse search string and string lengths are set here, for table
 * initialization and for substitution purposes.  The default for any
 * character to jump is the pattern length.
 */
void setjtable (char *apat)
{
  int i;

  rvstrcpy (tap, apat);
  patlenadd = (mlenold = matchlen = strlen (apat)) - 1;

  for (i = 0; i < HICHAR; i++)
  {
    deltaf[i] = matchlen;
    deltab[i] = matchlen;
  }

  /* Now put in the characters contained in the pattern, duplicating the CASE */
  for (i = 0; i < patlenadd; i++)
  {
    if (isletter (apat[i]))
      deltaf[(unsigned int) CHCASE ((unsigned int) apat[i])]
	   = patlenadd - i;
    deltaf[(unsigned int) apat[i]] = patlenadd - i;

    if (isletter (tap[i]))
      deltab[(unsigned int) CHCASE ((unsigned int) tap[i])]
	   = patlenadd - i;
    deltab[(unsigned int) tap[i]] = patlenadd - i;
  }

  /*
   * The last character will have the pattern length unless there are
   * duplicates of it.  Get the number to jump from the arrays delta, and
   * overwrite with zeroes in delta duplicating the CASE.
   */
  lastchfjump = patlenadd + deltaf[(unsigned int) apat[patlenadd]];
  lastchbjump = patlenadd + deltab[(unsigned int) apat[0]];

  if (isletter (apat[patlenadd]))
    deltaf[(unsigned int) CHCASE (apat[patlenadd])] = 0;
  deltaf[(int) apat[patlenadd]] = 0;

  if (isletter (apat[0]))
    deltab[(unsigned int) CHCASE (apat[0])] = 0;
  deltab[(int) apat[0]] = 0;

}

/*
 * eq -- Compare two characters.  The "bc" comes from the buffer, "pc" from
 * the pattern.  If we are not in EXACT mode, fold out the case.
 */
int eq (int bc, int pc)
{
  if ((curwp->w_bufp->b_mode & MDEXACT) == 0)
  {
    if (islower (bc))
      bc = CHCASE (bc);

    if (islower (pc))
      pc = CHCASE (pc);

  }

  return (bc == pc);
}

/*
 * readpattern -- Read a pattern.  Stash it in apat.  If it is the search
 * string, create the reverse pattern and the magic pattern, assuming we are
 * in MAGIC mode (and defined that way). Apat is not updated if the user
 * types in an empty line.  If the user typed an empty line, and there is no
 * old pattern, it is an error.  Display the old pattern, in the style of
 * Jeff Lomicka. There is some do-it-yourself control expansion.  Change to
 * using <META> to delimit the end-of-pattern to allow <NL>s in the search
 * string.
 */
int readpattern (char *prompt, char *apat, int srch)
{
  int status;
  char tpat[NPAT + 20];

  strcpy (tpat, prompt);	  /* copy prompt to output string */
  strcat (tpat, " [");		  /* build new prompt string */
  expandp (&apat[0], &tpat[strlen (tpat)], NPAT / 2);	/* add old pattern */
  strcat (tpat, "]<META>: ");

  /*
   * Read a pattern.  Either we get one, or we just get the META charater,
   * and use the previous pattern. Then, if it's the search string, make a
   * reversed pattern. *Then*, make the meta-pattern, if we are defined that
   * way.
   */
  if ((status = getstring (tpat, tpat, NPAT, sterm)) == TRUE)
  {
    strcpy (apat, tpat);

    /* If we are doing the search string set the delta tables. */
    if (srch)
      setjtable (apat);

  }
  else if (status == FALSE && apat[0] != 0)	/* Old one */
    status = TRUE;

  return (status);
}

/* savematch -- We found the pattern?  Let's save it away. */
void savematch (void)
{
  register char *ptr;		  /* pointer to last match string */
  register int j;
  LINE *curline;		  /* line of last match */
  int curoff;			  /* offset "	 "    */

  /* Free any existing match string, then attempt to allocate a new one. */
  if (patmatch != (char *) NULL)
    free (patmatch);

  ptr = patmatch = (char *) malloc (matchlen + 1);

  if (ptr != (char *) NULL)
  {
    curoff = matchoff;
    curline = matchline;

    for (j = 0; j < matchlen; j++)
      *ptr++ = nextch (&curline, &curoff, FORWARD);

    *ptr = '\0';
  }
}

/* rvstrcpy -- Reverse string copy. */
void rvstrcpy (char *rvstr, char *str)
{
  register int i;

  str += (i = strlen (str));

  while (i-- > 0)
    *rvstr++ = *--str;

  *rvstr = '\0';
}

/* sreplace -- Search and replace. */
int sreplace (int f, int n)
{
  return (replaces (FALSE, f, n));
}

/* qreplace -- search and replace with query. */
int qreplace (int f, int n)
{
  return (replaces (TRUE, f, n));
}

/*
 * replaces -- Search for a string and replace it with another string.  Query
 * might be enabled (according to kind).
 */
int replaces (int kind, int f, int n)
/* int kind;			Query enabled flag */
/* int f;			default flag */
/* int n;			# of repetitions wanted */
{
  register int status;		  /* success flag on pattern inputs */
  register int rlength;		  /* length of replacement string */
  register int numsub;		  /* number of substitutions */
  register int nummatch;	  /* number of found matches */
  int nlflag;			  /* last char of search string a <NL>? */
  int nlrepl;			  /* was a replace done on the last line? */
  char c;			  /* input char for query */
  char tpat[NPAT];		  /* temporary to hold search pattern */
  LINE *origline;		  /* original "." position */
  int origoff;			  /* and offset (for . query option) */
  LINE *lastline;		  /* position of last replace and */
  int lastoff;			  /* offset (for 'u' query option) */

  if (curbp->b_mode & MDVIEW)	  /* don't allow this command if	 */
    return (rdonly ());		  /* we are in read only mode	 */

  /* Check for negative repetitions. */
  if (f && n < 0)
    return (FALSE);

  /* Ask the user for the text of a pattern. */
  if ((status = readpattern (
		 (kind == FALSE ? TEXT84 : TEXT85), &pat[0], TRUE)) != TRUE)
    /* "Replace" */
    /* "Query replace" */
    return (status);

  /* Ask for the replacement string. */
  if ((status = readpattern (TEXT86, &rpat[0], FALSE)) == ABORT)
    /* "with" */
    return (status);

  /* Find the length of the replacement string. */
  rlength = strlen (&rpat[0]);

  /*
   * Set up flags so we can make sure not to do a recursive replace on the
   * last line.
   */
  nlflag = (pat[matchlen - 1] == '\r');
  nlrepl = FALSE;

  if (kind)
  {
    /* Build query replace question string. */
    strcpy (tpat, TEXT87);
    /* "Replace '" */
    expandp (&pat[0], &tpat[strlen (tpat)], NPAT / 3);
    strcat (tpat, TEXT88);
    /* "' with '" */
    expandp (&rpat[0], &tpat[strlen (tpat)], NPAT / 3);
    strcat (tpat, "'? ");

    /* Initialize last replaced pointers. */
    lastline = (LINE *) NULL;
    lastoff = 0;
  }

  /*
   * Save original . position, init the number of matches and substitutions,
   * and scan through the file.
   */
  origline = curwp->w_dotp;
  origoff = curwp->w_doto;
  numsub = 0;
  nummatch = 0;

  while ((f == FALSE || n > nummatch) &&
	 (nlflag == FALSE || nlrepl == FALSE))
  {

    /*
     * Search for the pattern. If we search with a regular expression,
     * matchlen is reset to the true length of the matched string.
     */
    if (!scanner (&pat[0], FORWARD, PTBEG))
      break;			  /* all done */

    ++nummatch;			  /* Increment # of matches */

    /* Check if we are on the last line. */
    nlrepl = (lforw (curwp->w_dotp) == curwp->w_bufp->b_linep);

    /* Check for query. */
    if (kind)
    {
      /* Get the query. */
    pprompt:mlwrite (&tpat[0], &pat[0], &rpat[0]);
    qprompt:
      update (TRUE);		  /* show the proposed place to change */
      c = tgetc ();		  /* and input */
      mlwrite ("");		  /* and clear it */

      /* And respond appropriately. */
      switch (c)
      {
	case 'y':		  /* yes, substitute */
	case 'Y':
	case ' ':
	  break;

	case 'n':		  /* no, onword */
	case 'N':
	  forwchar (FALSE, 1);
	  continue;

	case '!':		  /* yes/stop asking */
	  kind = FALSE;
	  break;

	case 'u':		  /* undo last and re-prompt */
	case 'U':
	  /* Restore old position. */
	  if (lastline == (LINE *) NULL)
	  {
	    /* There is nothing to undo. */
	    TTbeep ();
	    goto pprompt;
	  }
	  curwp->w_dotp = lastline;
	  curwp->w_doto = lastoff;
	  lastline = (LINE *) NULL;
	  lastoff = 0;

	  /* Delete the new string. */
	  backchar (FALSE, rlength);
	  status = delins (rlength, patmatch, FALSE);
	  if (status != TRUE)
	    return (status);

	  /*
	   * Record one less substitution, backup, save our place, and
	   * reprompt.
	   */
	  --numsub;
	  backchar (FALSE, mlenold);
	  matchline = curwp->w_dotp;
	  matchoff = curwp->w_doto;
	  goto pprompt;

	case '.':		  /* abort! and return */
	  /* restore old position */
	  curwp->w_dotp = origline;
	  curwp->w_doto = origoff;
	  curwp->w_flag |= WFMOVE;

	case BELL:		  /* abort! and stay */
	  mlwrite (TEXT89);
	  /* "Aborted!" */
	  return (FALSE);

	default:		  /* bitch and beep */
	  TTbeep ();

	case '?':		  /* help me */
	  mlwrite (TEXT90);

	  /*
	   * "(Y)es, (N)o, (!)Do rest, (U)ndo last, (^G)Abort, (.)Abort back,
	   * (?)Help: "
	   */
	  goto qprompt;

      }				  /* end of switch */
    }				  /* end of "if kind" */

    /* if this is the point origin, flag so we a can reset it */
    if (curwp->w_dotp == origline)
    {
      origline = (LINE *) NULL;
      lastline = curwp->w_dotp->l_bp;
    }

    /* Delete the sucker, and insert its replacement. */
    status = delins (matchlen, &rpat[0], TRUE);
    if (origline == (LINE *) NULL)
    {
      origline = lastline->l_fp;
      origoff = 0;
    }

    if (status != TRUE)
      return (status);

    /*
     * Save our position, since we may undo this. If we are not querying,
     * check to make sure that we didn't replace an empty string (possible in
     * MAGIC mode), because we'll infinite loop.
     */
    if (kind)
    {
      lastline = curwp->w_dotp;
      lastoff = curwp->w_doto;
    }
    else if (matchlen == 0)
    {
      mlwrite (TEXT91);
      /* "Empty string replaced, stopping." */
      return (FALSE);
    }

    numsub++;			  /* increment # of substitutions */
  }

  /* And report the results. */
  mlwrite (TEXT92, numsub);
  /* "%d substitutions" */
  return (TRUE);
}

/*
 * delins -- Delete a specified length from the current point then either
 * insert the string directly, or make use of replacement meta-array.
 */
int delins (int dlength, char *instr, int use_meta)

/*
 * int dlength; char *instr; int use_meta;
 */
{
  int status;

  /* Zap what we gotta, and insert its replacement. */
  if ((status = ldelete ((long) dlength, FALSE)) != TRUE)
    mlwrite (TEXT93);
  /* "%%ERROR while deleting" */
  else
    status = linstr (instr);

  return (status);
}

/* expandp -- Expand control key sequences for output. */
int expandp (char *srcstr, char *deststr, int maxlength)

/*
 * char *srcstr;			   string to expand char *deststr;
 * destination of expanded string int maxlength; maximum chars in destination
 */
{
  unsigned char c;		  /* current char to translate */

  /* Scan through the string. */
  while ((c = *srcstr++) != 0)
  {
    if (c == '\r')		  /* it's a newline */
    {
      *deststr++ = '<';
      *deststr++ = 'N';
      *deststr++ = 'L';
      *deststr++ = '>';
      maxlength -= 4;
    }
    else if (c < 0x20 || c == 0x7f)	/* control character */
    {
      *deststr++ = '^';
      *deststr++ = c ^ 0x40;
      maxlength -= 2;
    }
    else if (c == '%')
    {
      *deststr++ = '%';
      *deststr++ = '%';
      maxlength -= 2;
    }
    else
      /* any other character */
    {
      *deststr++ = c;
      maxlength--;
    }

    /* check for maxlength */
    if (maxlength < 4)
    {
      *deststr++ = '$';
      *deststr = '\0';
      return (FALSE);
    }
  }
  *deststr = '\0';
  return (TRUE);
}

/*
 * nextch -- retrieve the next/previous character in the buffer, and
 * advance/retreat the point. The order in which this is done is significant,
 * and depends upon the direction of the search.  Forward searches look at
 * the current character and move, reverse searches move and look at the
 * character.
 */
int nextch (LINE ** pcurline, int *pcuroff, int dir)

/*
 * LINE **pcurline; int *pcuroff; int dir;
 */
{
  register LINE *curline;
  register int curoff;
  register int c;

  curline = *pcurline;
  curoff = *pcuroff;

  if (dir == FORWARD)
  {
    if (curoff == llength (curline))	/* if at EOL */
    {
      curline = lforw (curline);  /* skip to next line */
      curoff = 0;
      c = '\r';			  /* and return a <NL> */
    }
    else
      c = lgetc (curline, curoff++);	/* get the char */
  }
  else
    /* Reverse. */
  {
    if (curoff == 0)
    {
      curline = lback (curline);
      curoff = llength (curline);
      c = '\r';
    }
    else
      c = lgetc (curline, --curoff);

  }
  *pcurline = curline;
  *pcuroff = curoff;

  return (c);
}
