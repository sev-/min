/*
 * $Id: echar.c,v 1.1 1995/01/06 21:45:10 sev Exp $
 * 
 * ----------------------------------------------------------
 * 
 * $Log: echar.c,v $
 * Revision 1.1  1995/01/06 21:45:10  sev
 * Initial revision
 *
 * 
 */

/*
 * CHAR.C: Character handling functions for MicroEMACS 3.10 (C)opyright 1988
 * by Daniel Lawrence
 * 
 * ALL THE CODE HERE IS FOR VARIOUS FORMS OF ASCII AND WILL HAVE TO BE MODIFIED
 * FOR EBCDIC
 */

#include	<stdio.h>
#include	"estruct.h"
#include	"etype.h"
#include	"edef.h"
#include	"english.h"

/*
 * isletter() Is the character a letter?  We presume a letter must be either
 * in the upper or lower case tables (even if it gets translated to itself).
 */

int isletter(ch)

register unsigned int ch;

{
  return (isupper(ch) || islower(ch));
}

/*
 * islower() Is the character a lower case letter?  This looks in the lower
 * to uppercase translation table.
 */

int islower(ch)
register unsigned int ch;
{
  return (lowcase[ch] != 0);
}

/*
 * isupper() Is the character a upper case letter?  This looks in the upper
 * to lowercase translation table.
 */

int isupper(ch)
register unsigned int ch;
{
  return (upcase[ch] != 0);
}

/*
 * chcase()
 * 
 * Change the case of the current character. First check lower and then upper.
 * If it is not a letter, it gets returned unchanged.
 */

unsigned int chcase(ch)
register unsigned int ch;
{
  /* translate lowercase */
  if (islower(ch))
    return (lowcase[ch]);

  /* translate uppercase */
  if (isupper(ch))
    return (upcase[ch]);

  /* let the rest pass */
  return (ch);
}

char upperc(ch)			  /* return the upper case equivalant of a
				   * character */

char ch;			  /* character to get uppercase euivalant of */

{
  if (islower(ch))
    return (lowcase[ch]);
  else
    return (ch);
}

initchars()			  /* initialize the character upper/lower
				   * case tables */

{
  register int index;		  /* index into tables */

  /* all of both tables to zero */
  for (index = 0; index < HICHAR; index++)
  {
    lowcase[index] = 0;
    upcase[index] = 0;
  }

  /* lower to upper */
  for (index = 'a'; index <= 'z'; index++)
    lowcase[index] = index - DIFCASE;

  /* upper to lower */
  for (index = 'A'; index <= 'Z'; index++)
    upcase[index] = index + DIFCASE;
}
