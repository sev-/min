/*
 * $Id: run.c,v 1.4 1995/01/24 15:40:39 sev Exp $
 * 
 * ----------------------------------------------------------
 * 
 * $Log: run.c,v $
 * Revision 1.4  1995/01/24 15:40:39  sev
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
void outhelp (void);
void edit_regs (void);
void edit_ports (void);
void out_edit_ports_help (void);
void out_edit_regs_help (void);
int find_start_label (void);

int runprogram (int f, int n)
{
  BUFFER *runbuf, *cbuf;
  int c, quitflag = 0;
  char tmp[TMPSTRLEN];

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

  cbuf = curbp;
  swbuffer (bfind (LISTBUFFERNAME, 1, 0));
  gotobob (1, 1);
  onlywind (1, 1);
  splitwind (TRUE, 1);

  runbuf = bfind (RUNBUFFERNAME, 1, 0);
  bclear (runbuf);
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
      case SPEC | '8':		/* Step */
      case 's':
      case 'S':
	if (terminateprogram)
	  break;
	do_command ();
	outshape (1);
	if (terminateprogram)
	  show_msg ("Program  terminated", 0);
	break;
      case SPEC | '1':		/* Restart */
      case 't':
      case 'T':
	reg_pc = 0;
	terminateprogram = 0;
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
    }
    update (TRUE);
  }
  runbuf->b_flag &= ~BFCHG;	  /* Not changed	       */
  swbuffer (cbuf);

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

void outshape (int numshape)
{
  switch (numshape)
  {
    case 1:
      outregs ();
      outmem ();
      outports ();
      outhelp ();
      gotobob (1, 1);
      outprog ();
      break;
    case 2:
      out_edit_ports_help ();
      outports ();
      gotobob (1, 1);
      break;
    case 3:
      out_edit_regs_help ();
      outregs ();
      gotobob (1, 1);
      break;
  }

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

  sprintf (tmp, "%s: %04x ", name, a);

  for (i = 0; i < 16; i++)
  {
    if (a + i > 65535)
      a -= 65536;

    if (a + i == addr)
      strcat (tmp, ">");
    else
      strcat (tmp, " ");

    sprintf (tmp1, "%02x", memory[a + i]);
    strcat (tmp, tmp1);
    if (i == 7)
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
  char tmp[10];

  nextwind (0, 0);

/*  if (getccol (FALSE) == 5)
  {
    backchar (TRUE, 1);
    lowrite (' ');
  } */
  gotobob (1, 1);

  sprintf (tmp, "%04x  ", reg_pc);
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
}

#define HELPX	40
#define HELPY	0

void outhelp (void)
{
  outtext (HELPY, HELPX,   "F8 - Step    F3 - edit Regs           ");
  outtext (HELPY+1, HELPX, "F1 - resTart  P - edit Ports          ");
  outtext (HELPY+2, HELPX, "^G - eXit                             ");
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
      case 'g':
      case 'G':
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
      case 'x':
      case 'X':
	quitflag = 1;
	break;
      case CTRL | 'F':		/* Forward */
      case SPEC | 'F':
      case 'R':
      case 'r':
	currbit = (currbit + 1) % 8;
	if (!currbit)
	  currport = (currport + 1) % 3;
	break;
      case CTRL | 'B':		/* Backward */
      case SPEC | 'B':
      case 'L':
      case 'l':
	currbit = (currbit + 7) % 8;
	if (currbit == 7)
	  currport = (currport + 2) % 3;
	break;
      case CTRL | 'A':		/* Home */
      case 'b':
      case 'B':
	currbit = currport = 0;
	break;
      case CTRL | 'E':		/* End */
      case 'E':
      case 'e':
	currbit = 7;
	currport = 2;
	break;
      case 'i':		/* Invert */
      case 'I':
        inport[currport] ^= 0xff;
        outshape (2);
	break;
      case ' ':		/* Change */
        inport[currport] ^= 0x80 >> currbit;
        outshape (2);
	break;
      case 'z':		/* Zero */
      case 'Z':
        inport[currport] = 0;
        outshape (2);
	break;
    }
  }
}  

void out_edit_ports_help (void)
{
  outtext (HELPY, HELPX,   "^A - Begin      ^E - End  Left, Right ");
  outtext (HELPY+1, HELPX, "Space - Change   Z - Zero             ");
  outtext (HELPY+2, HELPX, "I - Invert all  ^G - eXit             ");
}

void out_edit_regs_help (void)
{
  outtext (HELPY, HELPX,   "^A - beGin      ^E - eNd     Z - Zero ");
  outtext (HELPY+1, HELPX, "^G - eXit        S - Set flags of A   ");
  outtext (HELPY+2, HELPX, "hdigit - change  Left, Right, Up, Down");
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
  sscanf (curwp->w_dotp->l_text, "%*s %x", &reg_pc);

  swbuffer (cbuf);
  return 1;
}
