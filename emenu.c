/*
 * $Id: emenu.c,v 1.7 1995/10/14 15:46:11 sev Exp $
 *
 * ----------------------------------------------------------
 * 
 * $Log: emenu.c,v $
 * Revision 1.7  1995/10/14 15:46:11  sev
 * Program was in MSDOS and done A _LOT OF_ changes
 *
 * Revision 1.6  1995/01/27  20:52:27  sev
 * Added Animate (only for Unix), Step over, Continue
 * Fixed bug with start label
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

#include	<stdio.h>
#include	"estruct.h"
#include	"eproto.h"
#include	"edef.h"

#include	"hardware.h"

#define LEFT_KEY	(SPEC|'F')
#define	RIGHT_KEY	(SPEC|'B')
#define UP_KEY		(SPEC|'P')
#define	DOWN_KEY	(SPEC|'N')
#define ENTER		(CTRL|'M')
#define ESCAPE		META|CTRL|'['

#define NULLFUNC	((int (*)(void))NULL)

#define HORIZ	0
#define VERT	1

#define NEXT	1
#define PREV	-1
#define STAY	0
#define LEAVE	3

#define YES	1
#define NO	0

typedef struct MENUITEM
{
  char *text;
  char hotkey[5];
  int (*func) (void);
  int can_go_into;  	/* stay or not */
} MENUITEM;

typedef struct MENU
{
  int type;			  /* menu type (0 - horiz), (1 - vert) */
  MENUITEM *item;		  /* menu lines */
  int *pos;			  /* items positions */
  int x1, y1;			  /* menu corner */
  int width, length;
  int numitem;
  int curritem;
} MENU;

MENU *topmenu;
MENU *filemenu;
MENU *textmenu;
MENU *compilemenu;
MENU *runmenu;
MENU *keyboardmenu;

int playmenu (MENU * menu);
MENU *makemenu (int, MENUITEM *, int, int);
void outmenu (MENU * menu);
void closemenu (MENU * menu);
void initmenus (void);

/* compile.c */
void compileprogram (int);
int comp (int f, int n);

/* run.c */
void askstartlabel (void);
void askmemory (int);

int filemenuF (void);
int textmenuF (void);
int compilemenuF (void);
int quitF (void);
int savefileF (void);
int readfileF (void);
int runF (void);
int continueF (void);
int compileprogramF (void);
int saveasF (void);
int forwsearchF (void);
int backsearchF (void);
int runmenuF (void);
int startlabelF (void);
int memory0F (void);
int memory1F (void);
int memory2F (void);
int chkeyboardF (void);
int keyboard_JCUKEN_F (void);
int keyboard_QWERTY_F (void);

MENUITEM topmenuitem[] =
{
  {"File", "Ff", filemenuF, YES},
  {"Text", "Tt", textmenuF, YES},
  {"Compile", "Cc", compilemenuF, YES},
  {"Run", "Rr", runmenuF, YES},
  {"", ""}
};

MENUITEM filemenuitem[] =
{
  {"Read", "Rr", readfileF, NO},
  {"Save", "Ss", savefileF, NO},
  {"save As...", "Aa", saveasF, NO},
  {"Quit", "Qq", quitF, NO},
  {"", ""}
};

MENUITEM textmenuitem[] =
{
  {"search Forward", "Ff", forwsearchF, NO},
  {"search Reverse", "Rr", backsearchF, NO},
  {"", ""}
};

MENUITEM compilemenuitem[] =
{
  {"Compile", "Cc", compileprogramF, NO},
  {"", ""}
};

MENUITEM runmenuitem[] =
{
  {"Run", "Rr", runF, NO},
  {"Continue", "Cc", continueF, NO},
  {"Start label", "Ss", startlabelF, NO},
  {"memory0", "0", memory0F, NO},
  {"memory1", "1", memory1F, NO},
  {"memory2", "2", memory2F, NO},
  {"change Keyboard", "kK", chkeyboardF, YES},
  {"", ""}
};

MENUITEM keyboardmenuitem[] =
{
  {"JCUKEN", "Jj", keyboard_JCUKEN_F, NO},
  {"QWERTY", "Qq", keyboard_QWERTY_F, NO},
  {"", ""}
};

int playmenu (MENU * menu)
{
  int c;
  int oldcurr;
  int need_enter = 0;
  int i, j;

  outmenu (menu);
  oldcurr = menu->curritem;
  while (1)
  {
    if (menu->curritem < 0)
      menu->curritem = menu->numitem - 1;
    menu->curritem %= menu->numitem;

    if (oldcurr != menu->curritem || oldcurr == -1)
    {
      switch (menu->type)
      {
	case HORIZ:
	  TTmove (menu->x1, menu->y1 + menu->pos[menu->curritem]);
	  TTputs (menu->item[menu->curritem].text);
	  break;
	case VERT:
	  TTrev (TRUE);
	  TTmove (menu->y1 + menu->curritem + 1, menu->x1 + 1);
	  TTputc (' ');
	  TTputs (menu->item[menu->curritem].text);
	  for (i = 2 + strlen (menu->item[menu->curritem].text);
	       i < menu->width - 1; i++)
	    TTputc (' ');
	  TTrev (FALSE);
	  break;
      }
      if (oldcurr != -1)
      {
	switch (menu->type)
	{
	  case HORIZ:
	    TTrev (TRUE);
	    TTmove (menu->x1, menu->y1 + menu->pos[oldcurr]);
	    TTputs (menu->item[oldcurr].text);
	    TTrev (FALSE);
	    break;
	  case VERT:
	    TTmove (menu->y1 + oldcurr + 1, menu->x1 + 1);
	    TTputc (' ');
	    TTputs (menu->item[oldcurr].text);
	    for (i = 2 + strlen (menu->item[oldcurr].text); i < menu->width - 1;
		 i++)
	      TTputc (' ');
	    break;
	}
      }
      TTmove (0, 0);
      TTflush ();
    }

    oldcurr = menu->curritem;
    if (need_enter && menu->item[menu->curritem].func != NULLFUNC &&
	menu->item[menu->curritem].can_go_into == YES)
    {
      if ((need_enter = menu->item[menu->curritem].func ()) == LEAVE)
	return LEAVE;
      menu->curritem += need_enter;
      continue;
    }

    c = getkey ();
    switch (c)
    {
      case LEFT_KEY:
	if (menu->type == VERT)
	  return NEXT;
	menu->curritem++;
	break;
      case DOWN_KEY:
	if (menu->type == HORIZ)
	{
	  if ((need_enter = menu->item[menu->curritem].func ()) == LEAVE)
	    return LEAVE;
	  menu->curritem += need_enter;
	}
	menu->curritem++;
	break;
      case RIGHT_KEY:
	if (menu->type == VERT)
	  return PREV;
	menu->curritem--;
	break;
      case UP_KEY:
	if (menu->type == HORIZ)
	  break;
	menu->curritem--;
	break;
      case ESCAPE:
      case CTRL | 'G':
	return LEAVE;
	break;
      case ENTER:
	if (menu->item[menu->curritem].func != NULLFUNC)
	{
	  if ((need_enter = menu->item[menu->curritem].func ()) == LEAVE)
	    return LEAVE;
	  menu->curritem += need_enter;
	}
	break;
      default:
	for (i = 0; i < menu->numitem; i++)
	  for (j = 0; j < strlen (menu->item[i].hotkey); j++)
	    if (menu->item[i].hotkey[j] == c)
	    {
	      menu->curritem = i;
	      i = menu->numitem;
	      break;
	    }
    }
  }
}

MENU *makemenu (int type, MENUITEM * item, int x1, int y1)
{
  int numitem, i, maxwidth;
  MENU *menu;

  menu = (MENU *) malloc (sizeof (MENU));

  menu->type = type;
  menu->item = item;
  menu->x1 = x1;
  menu->y1 = y1;
  for (numitem = 0; *menu->item[numitem].text; numitem++);
  menu->numitem = numitem;
  menu->curritem = 0;

  switch (type)
  {
    case HORIZ:		  /* horizmenu */
      menu->width = term.t_ncol - x1;
      menu->length = 1;
      menu->pos = (int *) calloc (numitem, sizeof (int));
      menu->pos[0] = 2;
      for (i = 1; *item[i].text; i++)
	menu->pos[i] = menu->pos[i - 1] + strlen (item[i - 1].text) + 3;
      break;
    case VERT:			  /* vertmenu */
      maxwidth = strlen (item[0].text);
      for (i = 1; i < numitem; i++)
	if (strlen (item[i].text) > maxwidth)
	  maxwidth = strlen (item[i].text);
      menu->width = maxwidth + 4;
      menu->length = numitem + 2;
      break;
    default:
      break;
  }

  return menu;
}

void outmenu (MENU * menu)
{
  int i, j;
  int curritem = 0;

  switch (menu->type)
  {
    case HORIZ:
      TTrev (TRUE);
      TTmove (0, 0);

      for (i = 0; i < menu->width; i++)
	if (curritem >= 0 && i == menu->pos[curritem])
	{
	  if (curritem == menu->curritem)
	    TTrev (FALSE);
	  TTputs (menu->item[curritem].text);
	  if (curritem == menu->curritem)
	    TTrev (TRUE);
	  i += strlen (menu->item[curritem].text) - 1;
	  curritem++;
	  if (*menu->item[curritem].text == 0)
	    curritem = -1;
	}
	else
	  TTputc (' ');
      break;
    case VERT:
      TTmove (menu->y1, menu->x1);
      TTputc ('╔');
      for (i = 1; i < menu->width - 1; i++)
	TTputc ('═');
      TTputc ('╗');
      for (i = 0; i < menu->numitem; i++)
      {
	TTmove (i + menu->y1 + 1, menu->x1);
	TTputc ('║');
	if (i == menu->curritem)
	  TTrev (TRUE);
	TTputc (' ');
	TTputs (menu->item[i].text);
	for (j = 2 + strlen (menu->item[i].text); j < menu->width - 1; j++)
	  TTputc (' ');
	if (i == menu->curritem)
	  TTrev (FALSE);
	TTputc ('║');
      }
      TTmove (menu->length + menu->y1 - 1, menu->x1);
      TTputc ('╚');
      for (i = 1; i < menu->width - 1; i++)
	TTputc ('═');
      TTputc ('╝');
      break;
  }

  TTmove (0, 0);
  TTrev (FALSE);
  TTflush ();

  /* Now playing strange games :-) */
  if (menu->type == VERT)
    for (i = menu->y1; i < menu->length + menu->y1; i++)
      updoneline (i, menu->x1, menu->x1 + menu->width);
}

void closemenu (MENU * menu)
{
  int i;

  for (i = menu->y1; i < menu->length + menu->y1; i++)
    updoneline (i, menu->x1, menu->x1 + menu->width);

  TTmove (0, 0);
  update (TRUE);
}

int mainmenu (void)		  /* main menu on top the screen */
{
  playmenu (topmenu);
  updoneline (0, 0, term.t_ncol);
  TTmove (0, 0);
  return 0;
}

int filemenuF (void)
{
  int ret;

  ret = playmenu (filemenu);
  closemenu (filemenu);

  return ret;
}

int textmenuF (void)
{
  int ret;

  ret = playmenu (textmenu);
  closemenu (textmenu);

  return ret;
}

void initmenus (void)
{
  topmenu = makemenu (HORIZ, topmenuitem, 0, 0);
  filemenu = makemenu (VERT, filemenuitem, topmenu->pos[0], 1);
  textmenu = makemenu (VERT, textmenuitem, topmenu->pos[1], 1);
  compilemenu = makemenu (VERT, compilemenuitem, topmenu->pos[2], 1);
  runmenu = makemenu (VERT, runmenuitem, topmenu->pos[3], 1);
  keyboardmenu = makemenu (VERT, keyboardmenuitem, topmenu->pos[3]+13, 8);
}

int compilemenuF (void)
{
  int ret;

  ret = playmenu (compilemenu);
  closemenu (compilemenu);

  return ret;
}

int quitF (void)
{
  quit (1, 1);
  return LEAVE;
}

int savefileF (void)
{
  filesave (1, 1);
  return LEAVE;
}

int readfileF (void)
{
  fileread (1, 1);
  return LEAVE;
}

int runF (void)
{
  runprogram (1, 1);
  return LEAVE;
}

int continueF (void)
{
  continueprogram (1, 1);
  return LEAVE;
}

int compileprogramF (void)
{
  comp (1, 1);
  return LEAVE;
}

int saveasF (void)
{
  filewrite (1, 1);
  return LEAVE;
}

int forwsearchF (void)
{
  forwsearch (1, 1);
  return LEAVE;
}

int backsearchF (void)
{
  backsearch (1, 1);
  return LEAVE;
}

int runmenuF (void)
{
  int ret;

  ret = playmenu (runmenu);
  closemenu (runmenu);

  return ret;
}

int startlabelF (void)
{
  askstartlabel ();
  return LEAVE;
}

int memory0F (void)
{
  askmemory (0);
  return LEAVE;
}

int memory1F (void)
{
  askmemory (1);
  return LEAVE;
}

int memory2F (void)
{
  askmemory (2);
  return LEAVE;
}

int chkeyboardF (void)		  /* change keyboard menu from run */
{
  int ret;

  ret = playmenu (keyboardmenu);
  closemenu (keyboardmenu);

  return ret;
}

int keyboard_JCUKEN_F (void)
{
  keyboard_type = JCUKEN;
  return LEAVE;
}
int keyboard_QWERTY_F (void)
{
  keyboard_type = QWERTY;
  return LEAVE;
}

