/*
 *  $Id: debug_gettoken.c,v 1.1 1995/01/16 11:01:32 sev Exp $
 *
 * ---------------------------------------------------------- 
 *
 * $Log: debug_gettoken.c,v $
 * Revision 1.1  1995/01/16 11:01:32  sev
 * Initial revision
 *
 *
 */
 
#define MAIN

#include <stdio.h>
#include <ctype.h>
#include "hardware.h"
#include "commands.h"

enum
{
  TCOMMENT, TCOMMAND, TLABEL, TERROR, TWORD, TBYTE, TREG
};


char *gettoken (char *, int *, int);
char *getdigit (char *, int *, int);
int tokentype (char *);
int isconstword (char *);
int isconstbyte (char *);
int iscommand (char *);

main ()
{
  char tmp[80];
  char tmp1[80];
  char *s, *s1;
  int i;

  while (1)
  {
    i = 0;
    printf ("Expression: ");
    fflush (stdout);
    gets (tmp);
    while (s = gettoken (tmp, &i, strlen (tmp)))
    {
      switch (tokentype (s))
      {
	case TCOMMENT:
	  strcpy (tmp1, "TCOMMENT");
	  break;
	case TCOMMAND:
	  strcpy (tmp1, "TCOMMAND");
	  break;
	case TLABEL:
	  strcpy (tmp1, "TLABEL");
	  break;
	case TERROR:
	  strcpy (tmp1, "TERROR");
	  break;
	case TWORD:
	  strcpy (tmp1, "TWORD");
	  break;
	case TBYTE:
	  strcpy (tmp1, "TBYTE");
	  break;
	case TREG:
	  strcpy (tmp1, "TREG");
	  break;
      }
      printf ("%s ---> %s\n", s, tmp1);
    }
  }
}

char *gettoken (char *str, int *lpos, int maxlen)
{
  char tmp[TMPSTRLEN];
  char *s;
  char *s1 = tmp;

  s = &str[*lpos];
  while (*lpos < maxlen && (*s == ' ' || *s == '\t'))
  {
    s++;
    *lpos = *lpos + 1;
  }
  if (*lpos < maxlen && isalpha (*s))
  {
    while (*lpos < maxlen && isalnum (*s))
    {
      *s1++ = *s++;
      *lpos = *lpos + 1;
    }
    *s1 = '\0';
    if (*s == ':')		  /* label */
    {
      *s1++ = ':';
      *s1 = '\0';
      *lpos = *lpos + 1;
      s++;
      return tmp;
    }
    return tmp;
  }				  /* isalpha */

  if (*lpos < maxlen && isdigit (*s))
  {
    strcpy (tmp, getdigit (str, lpos, maxlen));
    return tmp;
  }

  if (*lpos >= maxlen)
    return (char *) 0;

  tmp[0] = *s++;
  *lpos = *lpos + 1;
  tmp[1] = '\0';

  return tmp;
}

char *getdigit (char *str, int *lpos, int maxlen)
{
  char tmp[80];
  char *s1 = tmp;
  char base = 10;
  int result = 0;
  int digit;
  char *s;

  s = &str[*lpos];
  if (*lpos < maxlen && *s == '0')
  {
    *lpos = *lpos + 1;
    s++;
    if (*s == 'x')
    {
      base = 16;
      *lpos = *lpos + 1;
      s++;
    }
  }

  while (*lpos < maxlen && isxdigit (*s))
  {
    *s1++ = *s++;
    *lpos = *lpos + 1;
  }
  *s1 = '\0';

  switch (*s)
  {
    case 'h':
    case 'H':
      *lpos = *lpos + 1;
      s++;
      base = 16;
      break;
    case 'o':
    case 'O':
      *lpos = *lpos + 1;
      s++;
      base = 8;
      break;
  }
  if (base != 16 && (s1[-1] == 'b' || s1[-1] == 'B'))
  {
    base = 2;
    s1[-1] = '\0';
  }

  for (s1 = tmp; *s1; s1++)
  {
    switch (base)
    {
      case 2:
	if (*s1 > '1')
	  base = 100;
	break;
      case 8:
	if (*s1 > '7')
	  base = 100;
	break;
      case 10:
	if (*s1 > '9')
	  base = 100;
	break;
    }
    if (base == 100)
      break;

    if (*s1 > '9')
      if (*s1 > 'F')
	digit = *s1 - 'a' + 10;
      else
	digit = *s1 - 'A' + 10;
    else
      digit = *s1 - '0';
    result = result * base + digit;
  }
  if (base != 100)
    sprintf (tmp, "#%x", result);
  else
    strcpy (tmp, "#error");

  return tmp;
}

int tokentype (char *token)
{
  if (token[0] == '#')
    return strlen (token) > 3 ? TWORD : TBYTE;
  if (token[0] == ';')
    return TCOMMENT;
  if (token[strlen (token) - 1] == ':')
    return TLABEL;
  if (isconstword (token))
    return TWORD;
  if (isconstbyte (token))
    return TBYTE;
  if (iscommand (token))
    return TCOMMAND;
  if (!token[1] && strchr ("abcdehlABCDEHL", token[0]))
    return TREG;
  return TERROR;
}

int isconstbyte (char *token)
{
  return 0;
}

int isconstword (char *token)
{
  return 0;
}

int iscommand (char *token)
{
  int i;
  char tmp[80];

  strcpy (tmp, token);
  for (i = 0; i < strlen (tmp); i++)
    tmp[i] = tolower (tmp[i]);

  for (i = 0; commargtable[i].name; i++)
    if (!strcmp (tmp, commargtable[i].name))
      return 1;
  return 0;
}
