/*
 * $Id: ebind.h,v 1.4 1995/01/17 12:33:59 sev Exp $
 * 
 * ----------------------------------------------------------
 * 
 * $Log: ebind.h,v $
 * Revision 1.4  1995/01/17 12:33:59  sev
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
  {CTRL | 'A', BINDFNC, gotobol},
  {CTRL | 'B', BINDFNC, backchar},
  {CTRL | 'C', BINDFNC, insspace},
  {CTRL | 'D', BINDFNC, forwdel},
  {CTRL | 'E', BINDFNC, gotoeol},
  {CTRL | 'F', BINDFNC, forwchar},
  {CTRL | 'G', BINDFNC, ctrlg},
  {CTRL | 'H', BINDFNC, backdel},
  {CTRL | 'I', BINDFNC, tab},
  {CTRL | 'J', BINDFNC, indent},
  {CTRL | 'K', BINDFNC, killtext},
  {CTRL | 'L', BINDFNC, refresh},
  {CTRL | 'M', BINDFNC, newline},
  {CTRL | 'N', BINDFNC, forwline},
  {CTRL | 'O', BINDFNC, mainmenu},
  {CTRL | 'P', BINDFNC, backline},
  {CTRL | 'Q', BINDFNC, quote},
  {CTRL | 'R', BINDFNC, backsearch},
  {CTRL | 'S', BINDFNC, forwsearch},
  {CTRL | 'V', BINDFNC, forwpage},
  {CTRL | 'W', BINDFNC, killregion},
  {CTRL | 'X', BINDFNC, cex},
  {CTRL | 'Y', BINDFNC, yank},
  {CTRL | 'Z', BINDFNC, backpage},
  {CTRL | '[', BINDFNC, meta},
  {CTLX | CTRL | 'C', BINDFNC, quit},
  {CTLX | CTRL | 'N', BINDFNC, mvdnwind},
  {CTLX | CTRL | 'P', BINDFNC, mvupwind},
  {CTLX | CTRL | 'R', BINDFNC, fileread},
  {CTLX | CTRL | 'W', BINDFNC, filewrite},
  {CTLX | CTRL | 'Z', BINDFNC, shrinkwind},
  {CTLX | '(', BINDFNC, ctlxlp},
  {CTLX | ')', BINDFNC, ctlxrp},
  {CTLX | '^', BINDFNC, enlargewind},
  {CTLX | '0', BINDFNC, delwind},
  {CTLX | '1', BINDFNC, onlywind},
  {CTLX | '2', BINDFNC, splitwind},
  {CTLX | 'B', BINDFNC, usebuffer},
  {CTLX | 'C', BINDFNC, spawncli},
  {CTLX | 'E', BINDFNC, ctlxe},
  {CTLX | 'K', BINDFNC, killbuffer},
  {CTLX | 'O', BINDFNC, nextwind},
  {CTLX | 'P', BINDFNC, prevwind},
  {CTLX | 'S', BINDFNC, filesave},
  {CTLX | 'W', BINDFNC, resize},
  {CTLX | 'X', BINDFNC, nextbuffer},
  {META | CTRL | 'G', BINDFNC, gotomark},
  {META | CTRL | 'L', BINDFNC, reposition},
  {META | CTRL | 'N', BINDFNC, namebuffer},
  {META | CTRL | 'R', BINDFNC, qreplace},
  {META | ' ', BINDFNC, setmark},
  {META | '>', BINDFNC, gotoeob},
  {META | '<', BINDFNC, gotobob},
  {META | 'C', BINDFNC, comp},
  {META | 'N', BINDFNC, runprogram},
  {META | 'R', BINDFNC, sreplace},
  {META | 'V', BINDFNC, backpage},
  {META | 'W', BINDFNC, copyregion},
  {META | 'Z', BINDFNC, quickexit},
  {ALTD | 'S', BINDFNC, forwhunt},
  {ALTD | 'R', BINDFNC, backhunt},
  {SPEC | '<', BINDFNC, gotobob},
  {SPEC | 'P', BINDFNC, backline},
  {SPEC | 'Z', BINDFNC, backpage},
  {SPEC | 'B', BINDFNC, backchar},
  {SPEC | 'F', BINDFNC, forwchar},
  {SPEC | '>', BINDFNC, gotoeob},
  {SPEC | 'N', BINDFNC, forwline},
  {SPEC | 'V', BINDFNC, forwpage},
  {SPEC | 'C', BINDFNC, insspace},
  {SPEC | 'D', BINDFNC, forwdel},
  {0x7F, BINDFNC, backdel},
  {0, BINDNUL, (int (*) ()) NULL}
};
