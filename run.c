/*                      `
 * $Id: run.c,v 1.7 1997/07/17 12:01:33 sev Exp $
 *
 * ----------------------------------------------------------
 *
 * $Log: run.c,v $
 * Revision 1.7  1997/07/17 12:01:33  sev
 * *** empty log message ***
 *
 * Revision 1.6  1995/10/14  15:46:11  sev
 * Program was in MSDOS and done A _LOT OF_ changes
 *
 * Revision 1.5  1995/01/27  20:52:27  sev
 * Added Animate (only for Unix), Step over, Continue
 * Fixed bug with start label
 *
 * Revision 1.4  1995/01/24  15:40:39  sev
 * Added inverse line while run; play_error; start label; Labels buffer
 *
 * Revision 1.3  1995/01/21  15:19:59  sev
 * Now Run works, Ports and regs change, list creates
 *
 * Revision 1.2  1995/01/17  12:33:59  sev
 * Now run screen is done
 * Revision 1.1  1995/01/14  15:08:09  sev Initial revision
 *
 *
 */
#include <stdio.h>
#include "hardware.h"
#include "estruct.h"
#include "proto.h"
#include "eproto.h"
#include "edef.h"

void outshape (int);
void outregs (void);
void outmem (void);
void outmemory (char *name, int addr, int y, int x, int);
void outprog (void);
void outtext (int, int, char *);
void outports (void);
void outtacts (void);
void outhelp (void);
void edit_regs (void);
void edit_ports (void);
void out_edit_ports_help (void);
void out_edit_regs_help (void);
void out_edit_memory_help (void);
int go_program (void);
void step_over (void);
void animate (void);
void animate2 (void);
void animate3 (void);
void edit_memory (void);
int a_program (int);

char *keyboard_table_QWERTY[] = { "QWERTYUIASDFGHJKZXCVBNM,",
				  "qwertyuiasdfghjkzxcvbnm,"};
char *keyboard_table_JCUKEN[] = { "JCUKENG[FYWAPROLQ^CMITX,",
				  "jcukeng[fywaprolq^cmitx,"};
char *keyboard_table_RUSSIA[] = { "ЙЦУКЕНГШФЫВАПРОЛЯЧСМИТЬБ",
				  "йцукенгшфывапролячсмитьб"};

int runprogram (int f, int n)
{
  char tmp[TMPSTRLEN];

  tacts_while_running = 0;

  if (need_compile ())
    if(!show_msg ("Program not compiled. Recompile it?", 1))
      return 1;
    else
    {
      comp (1, 1);
      if (need_compile ())
        return 1;
    }
    if (!find_start_label ())
    {
      sprintf (tmp, "Start label %s not found. Set it correct", startlabel);
      show_msg (tmp, 0);
      return 1;
    }
    return go_program ();
}

int continueprogram (int f, int n)
{
  char tmp[TMPSTRLEN];

  if (need_compile ())
    if(!show_msg ("Program not compiled. Recompile it?", 1))
      return 1;
    else
    {
      comp (1, 1);
      if (need_compile ())
        return 1;
      if (!find_start_label ())
      {
        sprintf (tmp, "Start label %s not found. Set it correct", startlabel);
        show_msg (tmp, 0);
        return 1;
      }
    }
    return go_program ();
}

int go_program (void)
{
  BUFFER *runbuf, *cbuf;
  int c, quitflag = 0;

  needmenubar = 0;

  cbuf = curbp;
  swbuffer (bfind (LISTBUFFERNAME, 1, 0));
  gotobob (1, 1);
  onlywind (1, 1);
  splitwind (TRUE, 1);

  runbuf = bfind (RUNBUFFERNAME, 1, 0);
  zotbuf (runbuf);
  runbuf = bfind (RUNBUFFERNAME, 1, 0);
  swbuffer (runbuf);
  linstr (" ");
  linstr (" ");
  modeflag = 0;
  resize (TRUE, 13);

  outshape (1);

  terminateprogram = 0;

  while (!quitflag)
  {
    c = getkey ();
    switch (c)
    {
      case CTRL | 'G':		/* Exit */
      case 'x':
      case 'X':
	quitflag = 1;
	break;
      case CTRL | 'L':		/* Refresh */
	sgarbf = TRUE;
	break;
      case 'a':			/* Animate */
      case 'A':
	animate ();
	break;
      case 'g':			/* Go */
      case 'G':
      case SPEC | '5':
	animate2 ();
	break;
      case SPEC | '8':		/* trace Into */
      case 'i':
      case 'I':
	if (terminateprogram)
	  break;
	do_command ();
	outshape (1);
	if (terminateprogram)
	  show_msg ("Program  terminated", 0);
	break;
      case SPEC | '0':		/* Step over */
      case 's':
      case 'S':
	if (terminateprogram)
	  break;
	step_over ();
	outshape (1);
	if (terminateprogram)
	  show_msg ("Program  terminated", 0);
	break;
      case SPEC | '1':		/* Restart */
      case 't':
      case 'T':
	find_start_label ();
	terminateprogram = 0;
	tacts_while_running = 0;
	outshape (1);
	break;
      case SPEC | '3':		/* Edit registers */
      case 'r':
      case 'R':
	edit_regs ();
	outshape (1);
	break;
      case 'P':			/* Edit ports */
      case 'p':
	edit_ports ();
	outshape (1);
	break;
      case 'M':			/* Edit memory */
      case 'm':
	edit_memory ();
	outshape (1);
	break;
      case 'Q':			/* Run without any output */
      case 'q':
	animate3 ();
	break;
    }
    update (TRUE);
  }
  runbuf->b_flag &= ~BFCHG;	  /* Not changed	       */
  swbuffer (cbuf);

  needmenubar = 1;

  nextwind (0, 0);
  curwp->w_markp[5] = (LINE *)NULL;
  nextwind (0, 0);

  onlywind (1, 1);
  modeflag = 1;

  return 1;
}

void askstartlabel (void)
{
  edit_string (10, 10, 70, TMPSTRLEN - 1, startlabel, " Enter label name ");
}

void askmemory (int memnum)
{
  char tmp[10], tmp2[40];

  sprintf (tmp, "%x", memoryforout[memnum]);
  sprintf (tmp2, " Memory %d address ", memnum);
  edit_string (10, 23, 58, 8, tmp, tmp2);
  memoryforout[memnum] = strtoul (tmp, NULL, 16);
}

void outshape (int numshape)
{
  switch (numshape)
  {
    case 1:
      outregs ();
      outmem ();
      outports ();
      outhelp ();
      outtacts ();
      outprog ();
      break;
    case 2:
      out_edit_ports_help ();
      outports ();
      break;
    case 3:
      out_edit_regs_help ();
      outregs ();
      break;
    case 4:
      outmem ();
      outports ();
      break;
    case 5:
      out_edit_memory_help ();
      outmem ();
      break;
    case 6:
      outregs ();
      outmem ();
      outports ();
      outtacts ();
      outprog ();
      break;
    case 7:
      outtacts ();
      break;
  }

  gotobob (1, 1);
  update (TRUE);
}

#define REGX	3
#define REGY	2

void outregs (void)
{
  char tmp[TMPSTRLEN];

  outtext (REGY, REGX - 1, "   ");
  outborder (REGY, REGX - 1, 7, 27, " Registers ");

  /* reg_f =  s z ac 0 p 1 n c */

  sprintf (tmp, "           s z ac  p   n c");
  outtext (REGY + 1, REGX, tmp);

  sprintf (tmp, "A=%02x       %d %d %d 0 %d 1 %d %d", reg_a, WhatF ('s'),
	   WhatF ('z'), WhatF ('h'), WhatF ('p'), WhatF ('n'), WhatF ('c'));
  outtext (REGY + 2, REGX, tmp);

  sprintf (tmp, "B=%02xh      C=%02xh", reg_b, reg_c);
  outtext (REGY + 3, REGX, tmp);

  sprintf (tmp, "D=%02xh      E=%02xh", reg_d, reg_e);
  outtext (REGY + 4, REGX, tmp);

  sprintf (tmp, "H=%02xh      L=%02xh", reg_h, reg_l);
  outtext (REGY + 5, REGX, tmp);

  sprintf (tmp, "SP=%04xh  PC=%04xh", reg_sp, reg_pc);
  outtext (REGY + 6, REGX, tmp);
}

#define TACTSX    40
#define TACTSY    2

void outtacts (void)
{
  char tmp[TMPSTRLEN];

  sprintf (tmp, "Tact %ld            ", tacts_while_running);
  outtext (TACTSY, TACTSX, tmp);
}

#define MEMX	40
#define MEMY	3

void outmem (void)
{
/*  outmemory ("[sp]", reg_sp, MEMY, MEMX, 1);
  outmemory ("[pc]", reg_pc, MEMY + 1, MEMX, 0); */

  outborder (MEMY + 1, MEMX - 1, 4, 36, " Memory ");
  outmemory ("[M1]", memoryforout[0], MEMY + 2, MEMX, 0);
  outmemory ("[M2]", memoryforout[1], MEMY + 3, MEMX, 0);
  outmemory ("[M3]", memoryforout[2], MEMY + 4, MEMX, 0);
}

void outmemory (char *name, int addr, int y, int x, int flag)
{
  int i, a;
  char tmp[TMPSTRLEN], tmp1[10];

  if (flag)
    a = addr;
  else
    a = addr & 0xfff8;

  sprintf (tmp, "%s: %04x ", name, a);

  for (i = 0; i < 8; i++)
  {
    if (a + i > 65535)
      a -= 65536;

    if (a + i == addr)
      strcat (tmp, ">");
    else
      strcat (tmp, " ");

    sprintf (tmp1, "%02x", GetMem (a + i));
    strcat (tmp, tmp1);
/*    if (i == 7)
      strcat (tmp, " "); */
  }
  outtext (y, x, tmp);
}

void outtext (int y, int x, char *s)
{
  int i;

  gotobob (1, 1);
  forwline (1, y);
  for (i = getcline (); i < y; i++)
    lnewline ();
  gotobob (1, 1);
  forwline (1, y);
  gotoeol (1, 1);
  if (getccol (FALSE) < x)
    linsert (x - getccol (FALSE), ' ');
  else
    backchar (1, getccol (FALSE) - x);

  lover (s);
}

void outprog (void)
{
  char tmp[10];

  nextwind (0, 0);

/*  if (getccol (FALSE) == 5)
  {
    backchar (TRUE, 1);
    lowrite (' ');
  } */
  gotobob (1, 1);

  sprintf (tmp, "\r%04x ", reg_pc);
  setjtable (tmp);
  scanner (tmp, FORWARD, PTEND);
/*  if (getccol (FALSE) == 6)
  {
    backchar (TRUE, 2);
    lowrite ('>');
  }
  curbp->b_flag &= ~BFCHG;	  /* Not changed	       */
  curwp->w_markp[5] = curwp->w_dotp;

  nextwind (0, 0);
}

#define PORTSX	10
#define PORTSY	10

void outports (void)
{
  char tmp[TMPSTRLEN], tmp1[TMPSTRLEN], tmp2[TMPSTRLEN];
  int i, j;

  outtext (PORTSY, PORTSX, "  ");
  sprintf (tmp, "InPort 0  InPort 1  InPort 2");
  outtext (PORTSY, PORTSX, tmp);
  strcpy (tmp, "");
  strcpy (tmp1, "");
  for (i = 0; i < 3; i++)
  {
    for (j = 0x80; j != 0; j /= 2)
      strcat (tmp, (inport[i] & j) ? "■" : "·");
    strcat (tmp, "  ");
    sprintf (tmp2, "(%02xh %3dd)", inport[i], inport[i]);
    strcat (tmp1, tmp2);
  }
  outtext (PORTSY+1, PORTSX, tmp);
  outtext (PORTSY+2, PORTSX, tmp1);

  sprintf (tmp, "OutPort 0 OutPort 1 OutPort 2");
  outtext (PORTSY, PORTSX+35, tmp);
  strcpy (tmp, "");
  strcpy (tmp1, "");
  for (i = 0; i < 3; i++)
  {
    for (j = 0x80; j != 0; j /= 2)
      strcat (tmp, (outport[i] & j) ? "■" : "·");
    strcat (tmp, "  ");
    sprintf (tmp2, "(%02xh %3dd)", outport[i], outport[i]);
    strcat (tmp1, tmp2);
  }
  outtext (PORTSY+1, PORTSX+35, tmp);
  outtext (PORTSY+2, PORTSX+35, tmp1);
  sprintf (tmp, "──── %s %s ", PROGNAME, VERSION);
  for (i = strlen (tmp); i < 80; i++)
    tmp[i] = '─';
  tmp[i] = 0;
  outtext (PORTSY+3, 0, tmp);
}

void edit_regs (void)
{
  int c;
  int quitflag = 0;
  static int curreg = 0;
  static int currnibble = 0;
  static byte *reglist[7] = {&reg_a, &reg_b, &reg_c, &reg_d, &reg_e,
  						&reg_h, &reg_l};

  outshape (3);

  while (!quitflag)
  {
    if (!curreg)
      TTmove (REGY + 2, REGX + 2 + currnibble);
    else
      TTmove (REGY + 3 + (curreg - 1) / 2,
			REGX + 2 + currnibble + 11 * ((curreg - 1) % 2));
    TTflush ();
    c = getkey ();
    switch (c)
    {
      case CTRL | 'G':		/* Exit */
      case 'x':
      case 'X':
	quitflag = 1;
	break;
      case CTRL | 'L':		/* Refresh */
	sgarbf = TRUE;
	update (TRUE);
	break;
      case CTRL | 'F':		/* Forward */
      case SPEC | 'F':
      case 'R':
      case 'r':
	currnibble = (currnibble + 1) % 2;
	if (!currnibble)
	  curreg = (curreg + 1) % 7;
	break;
      case CTRL | 'P':		/* Up */
      case SPEC | 'P':
      case 'U':
      case 'u':
        switch (curreg)
        {
          case 0:
	    curreg = 5;
	    break;
          case 1:
            curreg = 0;
            break;
          case 2:
            curreg = 6;
            break;
          default:
	    curreg = (curreg + 5) % 7;
	}
	break;
      case CTRL | 'N':		/* Down */
      case SPEC | 'N':
      case 'W':
      case 'w':
        if (!curreg)
          curreg++;
        else
          if (curreg == 6)
            curreg = 2;
          else
	    curreg = (curreg + 2) % 7;
	break;
      case CTRL | 'B':		/* Backward */
      case SPEC | 'B':
      case 'L':
      case 'l':
	currnibble = (currnibble + 1) % 2;
	if (currnibble)
	  curreg = (curreg + 6) % 7;
	break;
      case CTRL | 'A':		/* Home */
      case 'h':
      case 'H':
	currnibble = curreg = 0;
	break;
      case CTRL | 'E':		/* End */
      case 'n':
      case 'N':
	currnibble = 1;
	curreg = 6;
	break;
      case 'z':		/* Zero */
      case 'Z':
        reglist[curreg][0] = 0;
        outshape (3);
	break;
      case 's':		/* Set flags of reg_a */
      case 'S':
        OrA (reg_a);
        outshape (3);
	break;
      default:
        if (c <= '9' && c >= '0')
        {
          reglist[curreg][0] &= currnibble ? 0xf0 : 0x0f;
          reglist[curreg][0] |= currnibble ? c - '0' : (c - '0') << 4;
          if (!currnibble)
            currnibble++;
	  outshape (3);
	  break;
        }
        if (c <= 'f' && c >= 'a')
        {
          reglist[curreg][0] &= currnibble ? 0xf0 : 0x0f;
          reglist[curreg][0] |= currnibble ? c - 'a' + 10 : (c - 'a' + 10) << 4;
          if (!currnibble)
            currnibble++;
	  outshape (3);
	  break;
        }
        if (c <= 'F' && c >= 'A')
        {
          reglist[curreg][0] &= currnibble ? 0xf0 : 0x0f;
          reglist[curreg][0] |= currnibble ? c - 'A' + 10 : (c - 'A' + 10) << 4;
          if (!currnibble)
            currnibble++;
	  outshape (3);
	  break;
        }
    }
  }
}

void edit_ports (void)
{
  int c;
  int quitflag = 0;
  static int currbit = 0;
  static int currport = 0;

  outshape (2);

  while (!quitflag)
  {
    TTmove (PORTSY + 1, PORTSX + 10 * currport + currbit);
    TTflush ();
    c = getkey ();
    switch (c)
    {
      case CTRL | 'G':		/* Exit */
	quitflag = 1;
	break;
      case CTRL | 'L':		/* Refresh */
	sgarbf = TRUE;
	update (TRUE);
	break;
      case CTRL | 'F':		/* Forward */
      case SPEC | 'F':
	currbit = (currbit + 1) % 8;
	if (!currbit)
	  currport = (currport + 1) % 3;
	break;
      case CTRL | 'B':		/* Backward */
      case SPEC | 'B':
	currbit = (currbit + 7) % 8;
	if (currbit == 7)
	  currport = (currport + 2) % 3;
	break;
      case CTRL | 'A':		/* Home */
	currbit = currport = 0;
	break;
      case CTRL | 'E':		/* End */
	currbit = 7;
	currport = 2;
	break;
      case SPEC | '2':	/* Invert */
        inport[currport] ^= 0xff;
        outshape (2);
	break;
      case ' ':		/* Change */
        inport[currport] ^= 0x80 >> currbit;
        outshape (2);
	break;
      case SPEC | '1': /* Zero */
        inport[currport] = 0;
        outshape (2);
	break;
      default:
	if (mapped_portkey (c) != -1)
	{
	  inport[mapped_portkey (c) / 8] ^= 0x80 >> (mapped_portkey (c) % 8);
	  outshape (2);
	}
    }
  }
}

#define HELPX	1
#define HELPY	0

void outhelp (void)
{
  outtext (HELPY, HELPX,
  "F8 - Into  F10 - Step  F1 - resTart  P - edit Ports  ^G - eXit  F3 - edit Regs");
  outtext (HELPY+1, HELPX,
  "F5 - Go  A - Animate  Q - go without any output  M - edit Memory              ");
}

void out_edit_ports_help (void)
{
  outtext (HELPY, HELPX,
  "Space - Change  F1 - Zero  F2 - Invert all  ^G - eXit                         ");
  outtext (HELPY+1, HELPX,
  " press keyboard rows to change bits                                           ");
}

void out_edit_regs_help (void)
{
  outtext (HELPY, HELPX,
  "Z - Zero  ^G - eXit  S - Set flags of A  hdigit - change  H - Home  N - eNd   ");
  outtext (HELPY+1, HELPX,
  "                                                                              ");
}

void out_edit_memory_help (void)
{
  outtext (HELPY, HELPX,
  "Z - Zero  ^G - eXit  hdigit - change  H - Home  N - eNd                        ");
  outtext (HELPY+1, HELPX,
  "  Warning: You can't change the program                                        ");
}

void out_animate_help (void)
{
  switch (keyboard_type)
  {
    case QWERTY:
  outtext (HELPY, HELPX,
  "press keys   qwertyui  to change port 0,  asdfghjkl  to change port 1, etc     ");
    break;
    case JCUKEN:
  outtext (HELPY, HELPX,
  "press keys   jcukeng[  to change port 0,  fywaprold  to change port 1, etc     ");
  break;
  }
  outtext (HELPY+1, HELPX,
  " other keys to stop executing                                                  ");
}

void set_need_compile(void)
{
  curbp->b_flag |= BFNCOM;
}

void reset_need_compile(void)
{
  curbp->b_flag &= ~BFNCOM;
}

int need_compile(void)
{
  return curbp->b_flag & BFNCOM;
}

int find_start_label (void)
{
  BUFFER *cbuf;
  char tmp[TMPSTRLEN];

  sprintf (tmp, "%s ", startlabel);
  cbuf = curbp;
  swbuffer (bfind (LABELBUFFERNAME, 1, 0));
  gotobob (1, 1);
  setjtable (tmp);
  scanner (tmp, FORWARD, PTEND);

  if(strncmp (curwp->w_dotp->l_text, tmp, strlen (tmp)))
  {
    swbuffer (cbuf);
    return 0;
  }
  sscanf (curwp->w_dotp->l_text, "%*s %04x", &reg_pc);

  swbuffer (cbuf);
  return 1;
}

void step_over (void)
{
  char tmp[TMPSTRLEN];
  const char *throw[] =
	{"call", "cc", "cm", "cnc", "cnz", "cpe", "cpo", "cp", "cz", "rst", ""};
  int i, nextaddr;

  nextwind (0, 0);

  gotobob (1, 1);

  sprintf (tmp, "\r%04x ", reg_pc);
  setjtable (tmp);
  scanner (tmp, FORWARD, PTEND);

  getcommand_fromline (tmp, curwp->w_dotp->l_text, llength (curwp->w_dotp));

  nextwind (0, 0);

  for (i = 0; throw[i][0] && strcmp (throw[i], tmp); i++);
  if (throw[i][0])	/* call command */
  {
    if (!strcmp (tmp, "rst"))
      nextaddr = reg_pc+1;
    else
      nextaddr = reg_pc+3;

    while (reg_pc != nextaddr && !terminateprogram)
      do_command ();
  }
  else
    do_command ();
}

void animate (void)
{
  int c;
#if UNIX
  extern void settermnowait (int);
  extern int kbhit (void);

  settermnowait (1);
#endif

  out_animate_help ();
  while (1)
  {
    while (!terminateprogram && !kbhit ())
    {
      do_command ();
      outshape (6);
    }
    c = getkey ();
    if (mapped_portkey (c) != -1)
      inport[mapped_portkey (c) / 8] ^= 0x80 >> (mapped_portkey (c) % 8);
    else
      break;
  }

#if UNIX
  settermnowait (0);
#else
  while (kbhit ())
    getch ();
#endif

  outshape (1);
  if (terminateprogram)
    show_msg ("Program  terminated", 0);
}

void animate2 (void)	/* Go */
{
  int c;
  long ou = 0;
#if UNIX
  extern void settermnowait (int);
  extern int kbhit (void);

  settermnowait (1);
#endif

  out_animate_help ();
  while (1)
  {
    while (!terminateprogram && !kbhit ())
    {
      do_command ();
      outshape (4);

      if (tacts_while_running - ou > 100)
      {
	ou = tacts_while_running;
	outshape (7);
      }
    }
    c = getkey ();
    if (mapped_portkey (c) != -1)
      inport[mapped_portkey (c) / 8] ^= 0x80 >> (mapped_portkey (c) % 8);
    else
      break;
  }

#if UNIX
  settermnowait (0);
#else
  while (kbhit ())
    getch ();
#endif

  outshape (1);
  if (terminateprogram)
    show_msg ("Program  terminated", 0);
}

void animate3 (void)	/* Run without any output */
{
  long ou = 0;
#if UNIX
  extern void settermnowait (int);
  extern int kbhit (void);

  settermnowait (1);
#endif

  while (!terminateprogram && !kbhit ())
  {
    do_command ();

    if (tacts_while_running - ou > 3000)
    {
      ou = tacts_while_running;
      outshape (7);
    }
  }

#if UNIX
  settermnowait (0);
#else
  while (kbhit ())
    getch ();
#endif

  outshape (1);
  if (terminateprogram)
    show_msg ("Program  terminated", 0);
}

int outborder (int y1, int x1, int h, int w, char *title)
{
  int i, c;
  char tmp[80];

  c = (w - strlen (title)) / 2;

  *tmp = 0;
  strcat (tmp, "╔");
  for (i = 0; i < c; i++)
    strcat (tmp, "═");
  strcat (tmp, title);
  i += strlen (title) + 1;
  for (; i < w; i++)
    strcat (tmp, "═");
  strcat (tmp, "╗");
  outtext (y1, x1, tmp);

  *tmp = 0;
  strcat (tmp, "║");
  for (i = 1; i < w; i++)
    strcat (tmp, " ");
  strcat (tmp, "║");
  for (i = 1; i < h; i++)
    outtext (i + y1, x1, tmp);

  *tmp = 0;
  strcat (tmp, "╚");
  for (i = 1; i < w; i++)
    strcat (tmp, "═");
  strcat (tmp, "╝");
  outtext (y1+h, x1, tmp);
}

void edit_memory (void)
{
  int c;
  int quitflag = 0;
  static int curcol = 0, currow = 0;
  static int currnibble = 0;
  word tmpaddr;

  outshape (5);

  while (!quitflag)
  {
    TTmove (MEMY + 2 + currow, MEMX + 12 + currnibble + curcol * 3);
    TTflush ();
    c = getkey ();
    switch (c)
    {
      case CTRL | 'G':		/* Exit */
      case 'x':
      case 'X':
	quitflag = 1;
	break;
      case CTRL | 'L':		/* Refresh */
	sgarbf = TRUE;
	update (TRUE);
	break;
      case CTRL | 'F':		/* Forward */
      case SPEC | 'F':
      case 'R':
      case 'r':
	currnibble = (currnibble + 1) % 2;
	if (!currnibble)
	  curcol = (curcol + 1) % 7;
	break;
      case CTRL | 'P':		/* Up */
      case SPEC | 'P':
      case 'U':
      case 'u':
	currow = (currow + 2) % 3;
	break;
      case CTRL | 'N':		/* Down */
      case SPEC | 'N':
      case 'W':
      case 'w':
	currow = (currow + 1) % 3;
	break;
      case CTRL | 'B':		/* Backward */
      case SPEC | 'B':
      case 'L':
      case 'l':
	currnibble = (currnibble + 1) % 2;
	if (currnibble)
	  curcol = (curcol + 7) % 8;
	break;
      case CTRL | 'A':		/* Home */
      case 'h':
      case 'H':
	currnibble = currow = curcol = 0;
	break;
      case CTRL | 'E':		/* End */
      case 'n':
      case 'N':
	currnibble = 1;
	curcol = 7;
	currow = 2;
	break;
      case 'z':		/* Zero */
      case 'Z':
	if (a_program ((memoryforout[currow] & 0xfff8) + curcol))
	  break;
	PutMem ((memoryforout[currow] & 0xfff8) + curcol, 0);
	outshape (5);
	break;
      default:
	if (a_program ((memoryforout[currow] & 0xfff8) + curcol))
	  break;
        tmpaddr = (memoryforout[currow] & 0xfff8) + curcol;
	if (c <= '9' && c >= '0')
        {
	  PutMem (tmpaddr, GetMem (tmpaddr) & (currnibble ? 0xf0 : 0x0f));
	  PutMem (tmpaddr, GetMem (tmpaddr) | (currnibble ?
						 c - '0' : (c - '0') << 4));
          if (!currnibble)
            currnibble++;
	  outshape (5);
	  break;
        }
        if (c <= 'f' && c >= 'a')
        {
	  PutMem (tmpaddr, GetMem (tmpaddr) & (currnibble ? 0xf0 : 0x0f));
	  PutMem (tmpaddr, GetMem (tmpaddr) | (currnibble ?
					c - 'a' + 10 : (c - 'a' + 10) << 4));
          if (!currnibble)
            currnibble++;
	  outshape (5);
	  break;
        }
        if (c <= 'F' && c >= 'A')
        {
	  PutMem (tmpaddr, GetMem (tmpaddr) & (currnibble ? 0xf0 : 0x0f));
	  PutMem (tmpaddr, GetMem (tmpaddr) | (currnibble ?
					c - 'A' + 10 : (c - 'A' + 10) << 4));
          if (!currnibble)
            currnibble++;
	  outshape (5);
	  break;
        }
    }
  }
}

int a_program (int address)
{
  return (address >= first_addr_of_program && address <= last_addr_of_program);
}

int mapped_portkey (int c)
{
  int i, j;

  for (i = 0; i < 2; i++)
    for (j = 0; j < 3*8; j++)
    {
      switch (keyboard_type)
      {
	case QWERTY:
	  if (keyboard_table_QWERTY[i][j] == c)
	    return j;
	  break;
	case JCUKEN:
	  if (keyboard_table_JCUKEN[i][j] == c)
	    return j;
	  break;
      }
      if (keyboard_table_RUSSIA[i][j] == c)
	    return j;
    }

  return -1;
}
