/*
 * $Id: emenu.c,v 1.1 1995/01/06 21:45:10 sev Exp $
 * 
 * ----------------------------------------------------------
 * 
 * $Log: emenu.c,v $
 * Revision 1.1  1995/01/06 21:45:10  sev
 * Initial revision
 *
 * 
 */

#include	<stdio.h>
#include	"estruct.h"
#include	"etype.h"
#include	"edef.h"

#define LEFT_KEY	(SPEC|'F')
#define	RIGHT_KEY	(SPEC|'B')
#define UP_KEY		(SPEC|'P')
#define	DOWN_KEY	(SPEC|'N')
#define ENTER		(CTRL|'M')
#define ESCAPE		META

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
  int can_go_into;
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

int playmenu(MENU *menu);
MENU *makemenu(int, MENUITEM *, int, int);
void outmenu(MENU *menu);
void closemenu(MENU *menu);
void initmenus(void);


int filemenuF(void);
int textmenuF(void);
int compilemenuF(void);
int quitF(void);
int savefileF(void);

int compileprogram(void);
int askquit(void);

MENUITEM topmenuitem[] =
{
  {"File", "Ff", filemenuF, YES},
  {"Text", "Tt", textmenuF, YES},
  {"Word", "Ww", NULLFUNC, NO},
  {"Compile", "Cc", compilemenuF, YES},
  {"Quit", "Qq", quitF, NO},
  {"", ""}
};

MENUITEM filemenuitem[] =
{
  {"Read", "Rr", NULLFUNC, NO},
  {"Save", "Ss", savefileF, NO},
  {"save As...", "Aa", NULLFUNC, NO},
  {"Load", "Ll", NULLFUNC, NO},
  {"sHell", "Hh", NULLFUNC, NO},
  {"", ""}
};

MENUITEM textmenuitem[] =
{
  {"search Forward", "Ff", forwsearch, NO},
  {"search Reverse", "Rr", NULLFUNC, NO},
  {"hunt fOrward", "Oo", NULLFUNC, NO},
  {"hunt Backward", "Bb", NULLFUNC, NO},
  {"Incremental search", "Ii", NULLFUNC, NO},
  {"", ""}
};

MENUITEM compilemenuitem[] =
{
  {"Compile", "Cc", compileprogram, NO},
  {"", ""}
};

int playmenu(MENU *menu)
{
  int c;
  int curr, oldcurr;
  int need_enter = 0;
  int i, j;

  outmenu(menu);
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
	  TTmove(menu->x1, menu->y1 + menu->pos[menu->curritem]);
	  TTputs(menu->item[menu->curritem].text);
	  break;
	case VERT:
	  TTrev(TRUE);
	  TTmove(menu->y1 + menu->curritem + 1, menu->x1 + 1);
	  TTputc(' ');
	  TTputs(menu->item[menu->curritem].text);
	  for (i = 2 + strlen(menu->item[menu->curritem].text); i < menu->width - 1;
	       i++)
	    TTputc(' ');
	  TTrev(FALSE);
	  break;
      }
      if (oldcurr != -1)
      {
	switch (menu->type)
	{
	  case HORIZ:
	    TTrev(TRUE);
	    TTmove(menu->x1, menu->y1 + menu->pos[oldcurr]);
	    TTputs(menu->item[oldcurr].text);
	    TTrev(FALSE);
	    break;
	  case VERT:
	    TTmove(menu->y1 + oldcurr + 1, menu->x1 + 1);
	    TTputc(' ');
	    TTputs(menu->item[oldcurr].text);
	    for (i = 2 + strlen(menu->item[oldcurr].text); i < menu->width - 1;
		 i++)
	      TTputc(' ');
	    break;
	}
      }
      TTmove(term.t_nrow, term.t_ncol);
      fflush(stdout);
    }

    oldcurr = menu->curritem;
    if (need_enter && menu->item[menu->curritem].func != NULLFUNC &&
			menu->item[menu->curritem].can_go_into == YES)
    {
      menu->curritem += menu->item[menu->curritem].func();
      continue;
    }

    c = getkey();
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
	  if((need_enter = menu->item[menu->curritem].func()) == LEAVE)
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
	return STAY;
	break;
      case ENTER:
	if (menu->item[menu->curritem].func != NULLFUNC)
	{
	  if((need_enter = menu->item[menu->curritem].func()) == LEAVE)
	    return LEAVE;
	  menu->curritem += need_enter;
	}
	break;
      default:
	for (i = 0; i < menu->numitem; i++)
	  for (j = 0; j < strlen(menu->item[i].hotkey); j++)
	    if (menu->item[i].hotkey[j] == c)
	    {
	      menu->curritem = i;
	      i = menu->numitem;
	      break;
	    }
    }
  }
}

MENU *makemenu(int type, MENUITEM *item, int x1, int y1)
{
  int numitem, i, maxwidth;
  MENU *menu;

  menu = (MENU *) malloc(sizeof(MENU));

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
      menu->pos = (int *) calloc(numitem, sizeof(int));
      menu->pos[0] = 2;
      for (i = 1; *item[i].text; i++)
	menu->pos[i] = menu->pos[i - 1] + strlen(item[i - 1].text) + 3;
      break;
    case VERT:			  /* vertmenu */
      maxwidth = strlen(item[0].text);
      for (i = 1; i < numitem; i++)
	if (strlen(item[i].text) > maxwidth)
	  maxwidth = strlen(item[i].text);
      menu->width = maxwidth + 4;
      menu->length = numitem + 2;
      break;
    default:
      break;
  }

  return menu;
}

void outmenu(MENU *menu)
{
  int i, j;
  int curritem = 0;

  switch (menu->type)
  {
    case HORIZ:
      TTrev(TRUE);
      TTmove(0, 0);

      for (i = 0; i < menu->width; i++)
	if (curritem >= 0 && i == menu->pos[curritem])
	{
	  if (curritem == menu->curritem)
	    TTrev(FALSE);
	  TTputs(menu->item[curritem].text);
	  if (curritem == menu->curritem)
	    TTrev(TRUE);
	  i += strlen(menu->item[curritem].text) - 1;
	  curritem++;
	  if (*menu->item[curritem].text == 0)
	    curritem = -1;
	}
	else
	  TTputc(' ');
      break;
    case VERT:
      TTmove(menu->y1, menu->x1);
      TTputc('╔');
      for (i = 1; i < menu->width - 1; i++)
	TTputc('═');
      TTputc('╗');
      for (i = 0; i < menu->numitem; i++)
      {
	TTmove(i + menu->y1 + 1, menu->x1);
	TTputc('║');
	if (i == menu->curritem)
	  TTrev(TRUE);
	TTputc(' ');
	TTputs(menu->item[i].text);
	for (j = 2 + strlen(menu->item[i].text); j < menu->width - 1; j++)
	  TTputc(' ');
	if (i == menu->curritem)
	  TTrev(FALSE);
	TTputc('║');
      }
      TTmove(menu->length + menu->y1 - 1, menu->x1);
      TTputc('╚');
      for (i = 1; i < menu->width - 1; i++)
	TTputc('═');
      TTputc('╝');
      break;
  }

  fflush(stdout);
  TTrev(FALSE);
}

void closemenu(MENU *menu)
{
  int i;

  for (i = menu->y1; i < menu->length + menu->y1; i++)
    updoneline(i, menu->x1, menu->x1 + menu->width);
  update(TRUE);
}

int mainmenu(void)		  /* main menu on top the screen */
{
  playmenu(topmenu);
  TTmove(0, 0);
  TTputs("                                        ");
  TTputs("                                        ");
  updoneline(0, 0, term.t_ncol);
  return 0;
}

int filemenuF(void)
{
  int i;
  int ret;

  ret = playmenu(filemenu);
  closemenu(filemenu);

  return ret;
}

int textmenuF(void)
{
  int ret;

  ret = playmenu(textmenu);
  closemenu(textmenu);

  return ret;
}

void initmenus(void)
{
  topmenu = makemenu(HORIZ, topmenuitem, 0, 0);
  filemenu = makemenu(VERT, filemenuitem, topmenu->pos[0], 1);
  textmenu = makemenu(VERT, textmenuitem, topmenu->pos[1], 1);
  compilemenu = makemenu(VERT, compilemenuitem, topmenu->pos[3], 1);
}

int compilemenuF(void)
{
  int ret;

  ret = playmenu(compilemenu);
  closemenu(compilemenu);

  return ret;
}

int quitF(void)
{
  quit();
  return LEAVE;
}

int savefileF(void)
{
  savefile();
  return LEAVE;
}
