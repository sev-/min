/*
 * $Id: run.c,v 1.2 1995/01/17 12:33:59 sev Exp $
 * 
 * ----------------------------------------------------------
 * 
 * $Log: run.c,v $
 * Revision 1.2  1995/01/17 12:33:59  sev
 * Now run screen is done
 * Revision 1.1  1995/01/14  15:08:09  sev Initial revision
 * 
 * 
 */
#include <stdio.h>
#include "hardware.h"
#include "commands.h"
#include "estruct.h"
#include "proto.h"
#include "eproto.h"
#include "edef.h"

void outshape (void);
void outregs (void);
void outmem (void);
void outmemory (char *name, int addr, int y, int x, int);
void outprog (void);
void outtext (int, int, char *);
void outports (void);

int runprogram (int f, int n)
{
  BUFFER *runbuf, *cbuf;
  int c, quitflag = 0;

  onlywind (1, 1);
  splitwind (TRUE, 1);

  cbuf = curbp;
  runbuf = bfind ("Run", 1, 0);
  runbuf->b_mode &= ~MDVIEW;
  bclear (runbuf);
  swbuffer (runbuf);
  linstr (" ");
  linstr (" ");
  modeflag = 0;
  resize (TRUE, 13);

  outshape ();

  while (!quitflag)
  {
    c = getkey ();
    switch (c)
    {
      case CTRL | 'G':
	quitflag = 1;
	break;
    }
  }
  runbuf->b_flag &= ~BFCHG;	  /* Not changed	       */
  runbuf->b_mode |= MDVIEW;
  swbuffer (cbuf);
  onlywind (1, 1);
  modeflag = 1;
  return 1;
}

void askstartlabel (void)
{
  edit_string (10, 10, 70, TMPSTRLEN - 1, startlabel, " Enter label name ");
}

void outshape (void)
{
  outregs ();
  outmem ();
  outports ();
  gotobob (1, 1);
  outprog ();

  TTmove (0, 0);
  update (TRUE);
}

#define REGX	0
#define REGY	0

void outregs (void)
{
  char tmp[TMPSTRLEN];

  outtext (REGY, REGX, "    Registers");

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

#define MEMX	20
#define MEMY	4

void outmem (void)
{
  char tmp[TMPSTRLEN];

  outmemory ("[sp]", reg_sp, MEMY, MEMX, 1);
  outmemory ("[pc]", reg_pc, MEMY + 1, MEMX, 0);
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
    a = addr & 0xfff0;

  sprintf (tmp, "%s:  %04x  ", name, a);

  for (i = 0; i < 16; i++)
  {
    if (a + i > 65535)
      a -= 65536;
    sprintf (tmp1, "%02x", memory[a + i]);
    strcat (tmp, tmp1);
    if (i != 15)
      strcat (tmp, " ");
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
}
