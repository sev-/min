/*
 * $Id: compile.c,v 1.7 1995/01/27 20:52:27 sev Exp $
 * 
 * ----------------------------------------------------------
 * 
 * $Log: compile.c,v $
 * Revision 1.7  1995/01/27 20:52:27  sev
 * Added Animate (only for Unix), Step over, Continue
 * Fixed bug with start label
 *
 * Revision 1.6  1995/01/24  15:40:39  sev
 * Added inverse line while run; play_error; start label; Labels buffer
 *
 * Revision 1.5  1995/01/21  15:19:59  sev
 * Now Run works, Ports and regs change, list creates
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
#include <stdio.h>
#include <ctype.h>
#include "estruct.h"
#include "edef.h"
#include "hardware.h"
#include "commands.h"
#include "proto.h"

enum
{
  TCOMMENT, TCOMMAND, TLABEL, TERROR, TWORD, TBYTE, TREG, TORG
};

BUFFER *bfind (char *, int, int);
int gettoken (char *, char *, int *, int);
void getdigit (char *, char *, int *, int);
int tokentype (char *);
int isconstword (char *);
int iscommand (char *);
int commandargs (char *);
int addlabel (char *, int);
int findlabel (char *);
byte commandcode (char *);
int parseword (char *, int);
byte parsebyte (char *);
void clearlabeltable (void);
int strcasecmp (char *s1, char *s2);
void tomemory (char *, word, byte);
void clearlistbuffer (void);
void addlistline (char *liststr, char *, int);
void clearlabelbuffer (void);
void addlabelline (char *label, int addr);

int comp (int f, int n)
{
  char tmp[TMPSTRLEN];

  reset_need_compile ();
  program_has_errors = 0;
  clearerrorbuffer ();
  clearlabeltable ();
  clearlistbuffer ();
  clearlabelbuffer ();
  compileprogram (1);

  gotobob (1, 1);
  onlywind (1, 1);
  splitwind (TRUE, 1);

  nextwind (0, 0);
  resize (TRUE, 5);
  swbuffer (bfind ("Error", 1, 0));
  gotobob (1, 1);

  if (program_has_errors)
  {
    set_need_compile ();
    play_errors();
  }
  else
    reset_need_compile ();

  nextwind (0, 0);
  return 1;
}

void compileprogram (int pass)
{
  LINE *curline;
  int lpos;
  char tmp[TMPSTRLEN];
  char tmp1[TMPSTRLEN];
  word curraddr = 0;
  char *s;
  char liststr[TMPSTRLEN];

  sprintf (tmp1, "PASS %d", pass);
  addmessage ((LINE *) 0, tmp1, 0);

  curline = lforw (curbp->b_linep);

  while (curline != curbp->b_linep)
  {
    lpos = 0;
    s = curline->l_text;
    sprintf (liststr, "%04x  ", curraddr);

    while (gettoken (tmp, s, &lpos, llength (curline)))
    {
      switch (tokentype (tmp))
      {
	case TORG:
	  gettoken (tmp, s, &lpos, llength (curline));
	  if (tokentype (tmp) != TWORD && tokentype (tmp) != TBYTE)
	    if (pass == 1)
	    {
	      adderror (curline, "directive: number expected");
	      break;
	    }
	  if (parseword (tmp, 0) == -1)
	  {
	    sprintf (tmp1, "undefined label (%s)", tmp);
	    adderror (curline, "directive: number expected");
	  }
	  curraddr = parseword (tmp, 3);
	  sprintf (liststr, "%04x.    ", curraddr);
	  break;
	case TLABEL:
	  if (pass == 1)
	    if (addlabel (tmp, curraddr))
	      adderror (curline, "label redefiniton");
	  break;
	case TCOMMAND:
	  strcpy (tmp1, tmp);
	  switch (commandargs (tmp))
	  {
	    case NONE:
	      if (pass == 1)
		curraddr++;
	      else
		tomemory (liststr, curraddr++, commandcode (tmp1));
	      break;
	    case BYTE:
	      gettoken (tmp, s, &lpos, llength (curline));
	      if (tokentype (tmp) == TWORD)
	      {
		if( pass == 1)
		  addwarning (curline, "word stripped");
	      }
	      else
 		if (tokentype (tmp) != TBYTE)
		{
		  if (pass == 1)
		    adderror (curline, "byte expected");
		  curraddr += 2;
		  break;
	        }
	      if (pass == 1)
		curraddr += 2;
	      else
	      {
		if (parseword (tmp, 0) == -1)
		{
		  sprintf (tmp1, "undefined label (%s)", tmp);
		  adderror (curline, tmp1);
		}
		tomemory (liststr, curraddr++, commandcode (tmp1));
		tomemory (liststr, curraddr++, parsebyte (tmp));
	      }
	      break;

	    case WORD:
	      gettoken (tmp, s, &lpos, llength (curline));
	      if (tokentype (tmp) != TWORD && tokentype (tmp) != TREG &&
		  tokentype (tmp) != TBYTE)
	      {
		if (pass == 1)
		  adderror (curline, "word expected");
		curraddr += 3;
		break;
	      }
	      if (pass == 1)
		curraddr += 3;
	      else
	      {
		tomemory (liststr, curraddr++, commandcode (tmp1));
		if (parseword (tmp, 0) == -1)
		{
		  sprintf (tmp1, "undefined label (%s)", tmp);
		  adderror (curline, tmp1);
		}
		tomemory (liststr, curraddr++, (byte) parseword (tmp, 0));	/* low byte */
		tomemory (liststr, curraddr++, (byte) parseword (tmp, 1));	/* high byte */
	      }
	      break;

	    case REG:		  /* a b c d e h l m */
	      gettoken (tmp, s, &lpos, llength (curline));
	      if (strcmp (tmp, "m") && tokentype (tmp) != TREG)
	      {
		if (pass == 1)
		  adderror (curline, "register expected");
		curraddr++;
		break;
	      }
	      if (pass == 1)
		curraddr++;
	      else
	      {
		strcat (tmp1, " ");
		strcat (tmp1, tmp);
		tomemory (liststr, curraddr++, commandcode (tmp1));
	      }
	      break;

	    case LDAX:		  /* b d */
	      gettoken (tmp, s, &lpos, llength (curline));
	      if (strcmp (tmp, "b") && strcmp (tmp, "d"))
	      {
		if (pass == 1)
		  adderror (curline, "register b or d expected");
		curraddr++;
		break;
	      }
	      if (pass == 1)
		curraddr++;
	      else
	      {
		strcat (tmp1, " ");
		strcat (tmp1, tmp);
		tomemory (liststr, curraddr++, commandcode (tmp1));
	      }
	      break;

	    case DREG:		  /* b d h sp */
	      gettoken (tmp, s, &lpos, llength (curline));
	      if (strcmp (tmp, "b") && strcmp (tmp, "d") && strcmp (tmp, "h")
		  && strcmp (tmp, "sp"))
	      {
		if (pass == 1)
		  adderror (curline, "register b, d, h or sp expected");
		curraddr++;
		break;
	      }
	      if (pass == 1)
		curraddr++;
	      else
	      {
		strcat (tmp1, " ");
		strcat (tmp1, tmp);
		tomemory (liststr, curraddr++, commandcode (tmp1));
	      }
	      break;

	    case MOV:		  /* reg, reg */
	      gettoken (tmp, s, &lpos, llength (curline));
	      if (strcmp (tmp, "m") && tokentype (tmp) != TREG)
		if (pass == 1)
		  adderror (curline, "first parameter must be a register");

	      strcat (tmp1, " ");
	      strcat (tmp1, tmp);

	      gettoken (tmp, s, &lpos, llength (curline));
	      if (strcmp (tmp, ","))
		if (pass == 1)
		  adderror (curline, "comma expected");

	      strcat (tmp1, ",");

	      gettoken (tmp, s, &lpos, llength (curline));
	      if (strcmp (tmp, "m") && tokentype (tmp) != TREG)
	      {
		if (pass == 1)
		  adderror (curline, "second parameter must be a register");
		curraddr++;
		break;
	      }
	      strcat (tmp1, tmp);
	      if (pass == 1)
		curraddr++;
	      else
		tomemory (liststr, curraddr++, commandcode (tmp1));
	      break;

	    case LXI:		  /* [b d h sp], word */
	      gettoken (tmp, s, &lpos, llength (curline));
	      if (strcmp (tmp, "b") && strcmp (tmp, "d") && strcmp (tmp, "h")
		  && strcmp (tmp, "sp"))
		if (pass == 1)
		  adderror (curline, "register b, d, h or sp expected");

	      strcat (tmp1, " ");
	      strcat (tmp1, tmp);

	      gettoken (tmp, s, &lpos, llength (curline));
	      if (strcmp (tmp, ","))
		if (pass == 1)
		  adderror (curline, "comma expected");

	      strcat (tmp1, ",");

	      gettoken (tmp, s, &lpos, llength (curline));
	      if (tokentype (tmp) != TWORD && tokentype (tmp) != TREG
				&& tokentype (tmp) != TBYTE)
	      {
		if (pass == 1)
		  adderror (curline, "word expected");
		curraddr += 3;
		break;
	      }
	      if (pass == 1)
		curraddr += 3;
	      else
	      {
		tomemory (liststr, curraddr++, commandcode (tmp1));
		if (parseword (tmp, 0) == -1)
		{
		  sprintf (tmp1, "undefined label (%s)", tmp);
		  adderror (curline, tmp1);
		}
		tomemory (liststr, curraddr++, (byte) parseword (tmp, 0));	/* low byte */
		tomemory (liststr, curraddr++, (byte) parseword (tmp, 1));	/* high byte */
	      }
	      break;

	    case MVI:		  /* [a b c d e h l m], byte */
	      gettoken (tmp, s, &lpos, llength (curline));
	      if (strcmp (tmp, "m") && tokentype (tmp) != TREG)
		if (pass == 1)
		  adderror (curline, "first parameter must be a register");

	      strcat (tmp1, " ");
	      strcat (tmp1, tmp);

	      gettoken (tmp, s, &lpos, llength (curline));
	      if (strcmp (tmp, ","))
		if (pass == 1)
		  adderror (curline, "comma expected");

	      strcat (tmp1, ",");

	      gettoken (tmp, s, &lpos, llength (curline));
	      if (tokentype (tmp) == TWORD)
	      {
		if( pass == 1)
		  addwarning (curline, "word stripped");
	      }
	      else
	        if (tokentype (tmp) != TBYTE)
	        {
		  if (pass == 1)
		    adderror (curline, "byte expected");
		  curraddr += 2;
		  break;
	        }
	      if (pass == 1)
		curraddr += 2;
	      else
	      {
		if (parseword (tmp, 0) == -1)
		{
		  sprintf (tmp1, "undefined label (%s)", tmp);
		  adderror (curline, tmp1);
		}
		tomemory (liststr, curraddr++, commandcode (tmp1));
		tomemory (liststr, curraddr++, parsebyte (tmp));
	      }
	      break;

	    case RST:		  /* 0 1 2 3 4 5 6 7 */
	      gettoken (tmp, s, &lpos, llength (curline));
	      if (parsebyte (tmp) > 7)
	      {
		if (pass == 1)
		  adderror (curline, "parameter must be 0-7");
		curraddr++;
		break;
	      }
	      strcat (tmp1, " ");
	      strcat (tmp1, &tmp[1]);
	      if (pass == 1)
		curraddr++;
	      else
		tomemory (liststr, curraddr++, commandcode (tmp1));
	      break;

	    case PUSH:		  /* b d h psw */
	      gettoken (tmp, s, &lpos, llength (curline));
	      if (strcmp (tmp, "b") && strcmp (tmp, "d") && strcmp (tmp, "h")
		  && strcmp (tmp, "psw"))
	      {
		if (pass == 1)
		  adderror (curline, "register b, d, h or psw expected");
		curraddr++;
		break;
	      }
	      if (pass == 1)
		curraddr++;
	      else
	      {
		strcat (tmp1, " ");
		strcat (tmp1, tmp);
		tomemory (liststr, curraddr++, commandcode (tmp1));
	      }
	      break;
	    default:
	      if (pass == 1)
		adderror (curline, "INTERNAL ERROR");
	      break;
	  }			  /* switch commandargs */
	  break;
	case TCOMMENT:
	  lpos = llength (curline);
	  break;
	default:
	  sprintf (tmp1, "%s unexpected", tmp);
	  if (pass == 1)
	    adderror (curline, tmp1);
	  break;
      }				  /* switch tokentype */
    }				  /* while gettoken */
    if (pass == 2)
      if (strlen (liststr) < 9)
        addlistline (" ", curline->l_text, llength (curline));
      else
        addlistline (liststr, curline->l_text, llength (curline));
    curline = lforw (curline);
  }				  /* while curline */

  if (pass == 1)
    compileprogram (2);
  else
  {
    sprintf (tmp1, "Last address %04xh", curraddr);
    addmessage ((LINE *) 0, tmp1, 0);
  }
}

int gettoken (char *tmp, char *str, int *lpos, int maxlen)
{
  char *s;
  char *s1 = tmp;

  s = &str[*lpos];
  while (*lpos < maxlen && (*s == ' ' || *s == '\t'))
  {
    s++;
    *lpos = *lpos + 1;
  }
  if ((*lpos < maxlen) && isalpha (*s))
  {
    while (*lpos < maxlen && (isalnum (*s) || *s == '_'))
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
    }
    return 1;
  }				  /* isalpha */

  if (*lpos < maxlen && isdigit (*s))
  {
    getdigit (tmp, str, lpos, maxlen);
    return 1;
  }

  if (*lpos >= maxlen)
    return 0;

  tmp[0] = *s++;
  *lpos = *lpos + 1;
  tmp[1] = '\0';

  return 1;
}

void getdigit (char *tmp, char *str, int *lpos, int maxlen)
{
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
}

int tokentype (char *token)
{
  if (token[0] == '#')
    return strlen (token) > 3 ? TWORD : TBYTE;
  if (token[0] == ';')
    return TCOMMENT;
  if (token[strlen (token) - 1] == ':')
    return TLABEL;
  if (!strcasecmp (token, "org"))
    return TORG;
  if (iscommand (token))
    return TCOMMAND;
  if (!token[1] && strchr ("abcdehlABCDEHL", token[0]))
    return TREG;
  if (isconstword (token))
    return TWORD;
  return TERROR;
}

int isconstword (char *token)
{
  char *s;

  for (s = token; *s; s++)
    if (!isalnum (*s) && *s != '_')
      return 0;
  if (s == token)
    return 0;
  return 1;
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

int commandargs (char *token)
{
  int i;
  char tmp[TMPSTRLEN];
  char *s;

  strcpy (tmp, token);
  for (s = tmp; *s = tolower (*s); s++);

  for (i = 0; commargtable[i].name; i++)
    if (!strcmp (tmp, commargtable[i].name))
      return commargtable[i].arguments;
  return 0;
}

int numlabels = 0;

struct LABEL
{
  char *name;
  int addr;
} labeltable[NUMLABELS];

int addlabel (char *label, int addr)
{
  int i;
  char tmp[TMPSTRLEN];

  strcpy (tmp, label);
  tmp[strlen (tmp) - 1] = '\0';

  for (i = 0; i < numlabels; i++)
    if (!strcmp (tmp, labeltable[i].name))
      return 1;

  /* to delete starnge bug with malloc (4) there is vvvvvvvvvvvvvvvvvv */
  labeltable[numlabels].name = (char *) malloc (strlen (tmp) < 16 ? 16
					: strlen (tmp));
  strcpy (labeltable[numlabels].name, tmp);
  labeltable[numlabels].addr = addr;
  numlabels++;

  addlabelline (tmp, addr);

  return 0;
}

int findlabel (char *label)
{
  int i;

  for (i = 0; i < numlabels; i++)
    if (!strcmp (label, labeltable[i].name))
      return labeltable[i].addr;

  return -1;
}

byte commandcode (char *comm)
{
  char comm1[5], args[10];
  char *s, *s1;
  int i;
  char tmp[TMPSTRLEN];

  strcpy (tmp, comm);
  for (s = tmp; *s = tolower (*s); s++);

  for (s = tmp, s1 = comm1; *s != ' ' && (*s1 = *s); s++, s1++);

  *s1 = '\0';
  if (*s)
    s++;
  strcpy (args, s);

  for (i = 0; commandtable[i].name; i++)
    if (!strcmp (comm1, commandtable[i].name) &&
	!strcmp (args, commandtable[i].args))
      return commandtable[i].code;

  return (byte) 0xff;
}

int parseword (char *token, int numbyte)
{
  int result = 0, digit;
  char *s;

  if (token[0] == '#')
  {
    for (s = &token[1]; *s; s++)
    {
      if (*s > '9')
	digit = *s - 'a' + 10;
      else
	digit = *s - '0';
      result = result * 16 + digit;
    }
  }
  else
    result = findlabel (token);

  if (result == -1)
    return -1;

  if (numbyte == 1)		  /* high byte */
    return (result & 0xff00) >> 8;
  if (numbyte == 0)		  /* low byte */
    return result & 0xff;
  return result;		  /* all result */
}

byte parsebyte (char *token)
{
  return (byte) parseword (token, 0);
}

void clearlabeltable (void)
{
  int i;

  for (i = 0; i < numlabels; i++)
    free (labeltable[i].name);

  numlabels = 0;
}

int strcasecmp (char *s1, char *s2)
{
  char tmp1[TMPSTRLEN];
  char *ptr1, *ptr2;

  for (ptr1 = tmp1, ptr2 = s1; *ptr1 = tolower (*ptr2); ptr1++, ptr2++);

  return strcmp (tmp1, s2);
}

void tomemory (char *list, word a, byte b)
{
  char tmp[10];

  sprintf (tmp, "%02x ", b);
  strcat (list, tmp);

  PutMem (a, b);
}

void clearlistbuffer (void)
{
  BUFFER *listbuf;

  listbuf = bfind (LISTBUFFERNAME, 1, 0);
  listbuf->b_flag &= ~BFCHG;	  /* Not changed	       */
  bclear (listbuf);
}

void addlistline (char *liststr, char *s, int len)
{
  BUFFER *cbuf;
  char tmp[TMPSTRLEN];

  strcpy (tmp, liststr);
  while (strlen (tmp) < LISTTABSIZE)
    strcat (tmp, " ");
  strncat (tmp, s, len);

  cbuf = curbp;
  swbuffer (bfind (LISTBUFFERNAME, 1, 0));
  linstr (tmp);
  forwline (1, 1);
  curbp->b_flag &= ~BFCHG;	  /* Not changed	       */
  swbuffer (cbuf);
}

void play_errors (void)
{
  int c;
  int quitflag = 0;
  LINE *linep = (LINE *)NULL;
  int line_to_go;

  gotobob (1, 1);
  while (curwp->w_dotp != curbp->b_linep &&
	strncmp (curwp->w_dotp->l_text, "Error", 5) &&
		strncmp (curwp->w_dotp->l_text, "Warning", 7))
    forwline (1, 1);

  while (!quitflag)
  {
    if (linep != curwp->w_dotp)
    {
      line_to_go = 0;
      if (!strncmp (curwp->w_dotp->l_text, "Error", 5))
        sscanf (curwp->w_dotp->l_text, "Error at line %d", &line_to_go);
      if (!strncmp (curwp->w_dotp->l_text, "Warning", 7))
        sscanf (curwp->w_dotp->l_text, "Warning at line %d", &line_to_go);

      nextwind (0, 0);
      gotobob (1, 1);
      forwline (1, line_to_go-1);
      curwp->w_markp[5] = curwp->w_dotp;
      nextwind (0, 0);
      update (TRUE);
    }

    linep = curwp->w_dotp;
    c = getkey ();
    switch (c)
    {
      case CTRL | 'G':		/* Exit */
      case SPEC | '7':
	quitflag = 1;
	break;
      case CTRL | 'N':		/* Down */
      case SPEC | 'N':
        forwline (1, 1);
	while (curwp->w_dotp != curbp->b_linep &&
		strncmp (curwp->w_dotp->l_text, "Error", 5) &&
			strncmp (curwp->w_dotp->l_text, "Warning", 7))
	  forwline (1, 1);
	if (curwp->w_dotp == curbp->b_linep)
	{
          backline (1, 1);
	  while (curwp->w_dotp != curbp->b_linep->l_bp &&
		strncmp (curwp->w_dotp->l_text, "Error", 5) &&
			strncmp (curwp->w_dotp->l_text, "Warning", 7))
	    backline (1, 1);
	}
	break;
      case CTRL | 'P':		/* Up */
      case SPEC | 'P':
        backline (1, 1);
	while (curwp->w_dotp != curbp->b_linep->l_bp &&
		strncmp (curwp->w_dotp->l_text, "Error", 5) &&
			strncmp (curwp->w_dotp->l_text, "Warning", 7))
	  backline (1, 1);
	break;
      case SPEC | '<':		/* Home */
      case META | '<':
	gotobob (1, 1);
	while (curwp->w_dotp != curbp->b_linep &&
		strncmp (curwp->w_dotp->l_text, "Error", 5) &&
			strncmp (curwp->w_dotp->l_text, "Warning", 7))
	  forwline (1, 1);
	break;
      case META | '>':		/* End */
      case SPEC | '>':
	gotoeob ();
	backline (1, 1);
	while (curwp->w_dotp != curbp->b_linep->l_bp &&
		strncmp (curwp->w_dotp->l_text, "Error", 5) &&
			strncmp (curwp->w_dotp->l_text, "Warning", 7))
	  backline (1, 1);
	break;
    }
  }
  nextwind (0, 0);
  curwp->w_markp[5] = (LINE *)NULL;
  nextwind (0, 0);
}  

int next_window (int f, int n)
{
  nextwind (0, 0);
  if (!strcmp (curbp->b_bname, ERRORBUFFERNAME))
  {
    play_errors ();
    nextwind (0, 0);
  }
  return 1;
}

void clearlabelbuffer (void)
{
  BUFFER *labelbuf;

  labelbuf = bfind (LABELBUFFERNAME, 1, 0);
  labelbuf->b_flag &= ~BFCHG;	  /* Not changed	       */
  bclear (labelbuf);
}

void addlabelline (char *label, int addr)
{
  BUFFER *cbuf;
  char tmp[TMPSTRLEN];

  sprintf (tmp, "%s %04x", label, addr);

  cbuf = curbp;
  swbuffer (bfind (LABELBUFFERNAME, 1, 0));
  linstr (tmp);
  forwline (1, 1);
  curbp->b_flag &= ~BFCHG;	  /* Not changed	       */
  swbuffer (cbuf);
}

void getcommand_fromline (char *command, char *line, int lsize)
{
  int l = 0;

  while (gettoken (command, line, &l, lsize)
		&& tokentype (command) != TCOMMAND);
}
