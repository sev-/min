/*
 * $Id: ebind.h,v 1.7 1995/10/14 15:46:11 sev Exp $
 * 
 * ----------------------------------------------------------
 * 
 * $Log: ebind.h,v $
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
 * Revision 1.4  1994/08/15  21:27:30  sev i'm sorry, but this indent IMHO more
 * better ;-) Revision 1.3  1994/08/15  20:42:11  sev Indented Revision 1.2
 * 1994/06/24  17:22:21  sev Added mainmenu key
 * 
 * Revision 1.1  1994/06/24  14:17:12  sev Initial revision
 * 
 * 
 */

/*
 * EBIND:		Initial default key to function bindings for
 * MicroEMACS 3.10
 */

/*
 * Command table. This table  is *roughly* in ASCII order, left to right
 * across the characters of the command. This explains the funny location of
 * the control-X commands.
 */
KEYTAB keytab[NBINDS] =
{
  {CTRL | 'A', BINDFNC, gotobol},	/* goto begin of line */
  {CTRL | 'B', BINDFNC, backchar},	/* backward character */
  {CTRL | 'C', BINDFNC, insspace},  	/* insert space */
  {CTRL | 'D', BINDFNC, forwdel},	/* forward delete (under cursor) */
  {CTRL | 'E', BINDFNC, gotoeol},	/* goto end of line */
  {CTRL | 'F', BINDFNC, forwchar},	/* forward character */
  {CTRL | 'G', BINDFNC, ctrlg},		/* abort */
  {CTRL | 'H', BINDFNC, backdel},	/* backspace */
  {CTRL | 'I', BINDFNC, tab},		/* tabulate */
  {CTRL | 'J', BINDFNC, indent},	/* indenting */
  {CTRL | 'K', BINDFNC, killtext},	/* kill text to end of line */
  {CTRL | 'L', BINDFNC, refresh},	/* refresh screen */
  {CTRL | 'M', BINDFNC, newline},	/* open new line */
  {CTRL | 'N', BINDFNC, forwline},	/* next line */
  {CTRL | 'O', BINDFNC, mainmenu},	/* choose menu */
  {CTRL | 'P', BINDFNC, backline},	/* previous line */
  {CTRL | 'Q', BINDFNC, quote},		/* quote character */
  {CTRL | 'R', BINDFNC, backsearch},	/* backward search */
  {CTRL | 'S', BINDFNC, forwsearch},	/* forward search */
  {CTRL | 'V', BINDFNC, forwpage},	/* next page */
  {CTRL | 'W', BINDFNC, killregion},	/* kill block */
  {CTRL | 'X', BINDFNC, cex},		/* prefix key */
  {CTRL | 'Y', BINDFNC, yank},		/* copy block from memory */
  {CTRL | 'Z', BINDFNC, backpage},	/* previous page */
  {CTRL | '[', BINDFNC, meta},		/* prefix key */
  {CTLX | CTRL | 'C', BINDFNC, quit},	/* quit editor without saving */
  {CTLX | CTRL | 'N', BINDFNC, mvdnwind},	/* move window down */
  {CTLX | CTRL | 'P', BINDFNC, mvupwind},	/* move window up */
  {CTLX | CTRL | 'R', BINDFNC, fileread},	/* read file into current buffer */
  {CTLX | CTRL | 'W', BINDFNC, filewrite},	/* write file with asking name */
  {CTLX | CTRL | 'Z', BINDFNC, shrinkwind},	/* shrink window */
  {CTLX | '(', BINDFNC, ctlxlp},	/* begin keyboard macro */
  {CTLX | ')', BINDFNC, ctlxrp},	/* end keyboard macro */
  {CTLX | '^', BINDFNC, enlargewind},	/* enlarege window */
  {CTLX | '0', BINDFNC, delwind},	/* delete current window */
  {CTLX | '1', BINDFNC, onlywind},	/* left only current window */
  {CTLX | '2', BINDFNC, splitwind},	/* split current window */
  {CTLX | 'B', BINDFNC, usebuffer},	/* select buffer */
/*  {CTLX | 'C', BINDFNC, spawncli}, */
  {CTLX | 'E', BINDFNC, ctlxe},		/* execute keyboard macro */
  {CTLX | 'K', BINDFNC, killbuffer},	/* kill buffer */
  {CTLX | 'P', BINDFNC, prevwind},	/* goto previous window */
  {CTLX | 'S', BINDFNC, filesave},	/* save current file without asking name */
  {CTLX | 'W', BINDFNC, resize},	/* resize current window */
/*  {CTLX | 'X', BINDFNC, nextbuffer},	/* select next buffer */
  {META | CTRL | 'G', BINDFNC, gotomark},	/* goto mark */
  {META | CTRL | 'L', BINDFNC, reposition},	/* reposition in current window */
  {META | CTRL | 'N', BINDFNC, namebuffer},	/* change buffer name */
  {META | CTRL | 'R', BINDFNC, qreplace},	/* replase with query */
  {META | ' ', BINDFNC, setmark},	/* set begin of block */
  {META | '>', BINDFNC, gotoeob},	/* goto end of buffer */
  {META | '<', BINDFNC, gotobob},	/* goto begin of buffer */
  {META | 'C', BINDFNC, comp},		/* compile current buffer */
  {META | 'N', BINDFNC, continueprogram},	/* continue program */
  {META | 'R', BINDFNC, sreplace},	/* search with replace */
  {META | 'V', BINDFNC, backpage},	/* previous page */
  {META | 'W', BINDFNC, copyregion},	/* copy region onto memory */
  {META | 'Z', BINDFNC, quickexit},	/* exit with saving */
  {ALTD | 'S', BINDFNC, forwhunt},	/* forward search with old pattern */
  {ALTD | 'R', BINDFNC, backhunt},	/* backward search with old pattern */
  {SPEC | '7', BINDFNC, next_window},	/* choose next woindow */
  {SPEC | '<', BINDFNC, gotobob},	/* goto begin of buffer */
  {SPEC | 'P', BINDFNC, backline},	/* previous line */
  {SPEC | 'Z', BINDFNC, backpage},	/* previous page */
  {SPEC | 'B', BINDFNC, backchar},	/* backward character */
  {SPEC | 'F', BINDFNC, forwchar},	/* forward character */
  {SPEC | '>', BINDFNC, gotoeob},	/* goto end of buffer */
  {SPEC | 'N', BINDFNC, forwline},	/* next line */
  {SPEC | 'V', BINDFNC, forwpage},	/* next page */
  {SPEC | 'C', BINDFNC, insspace},	/* inset space under cursor */
  {SPEC | 'D', BINDFNC, forwdel},	/* delete character under cursor */
  {0x7F, BINDFNC, backdel},
  {0, BINDNUL, (int (*) ()) NULL}
};

