/*
 * $Id: eproto.h,v 1.6 1995/01/27 20:52:27 sev Exp $
 * 
 * ----------------------------------------------------------
 * 
 * $Log: eproto.h,v $
 * Revision 1.6  1995/01/27 20:52:27  sev
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
 * Revision 1.5  1994/08/15  21:27:30  sev i'm sorry, but this indent IMHO more
 * better ;-) Revision 1.4  1994/08/15  20:42:11  sev Indented Revision 1.3
 * 1994/06/27  11:18:43  sev Menu...
 * 
 * Revision 1.2  1994/06/24  17:22:21  sev Added mainmenu and ttputs proto
 * 
 * Revision 1.1  1994/06/24  14:17:12  sev Initial revision
 * 
 * 
 */

/*
 * ETYPE:		Global function type definitions for MicroEMACS 3.9
 * 
 * written by Daniel Lawrence based on code by Dave G. Conroy, Steve Wilhite and
 * George Jones
 */


/* ALL global fuction declarations */

BUFFER *bfind (char *, int, int);
BUFFER *getdefb (void);
char *bytecopy (char *, char *, int);
char *fixnull (char *);
char *int_asc (int);
char *makename (char *, char *);
int absv (int);
int eq (int, int);
int ernd (void);
int fbound (int, LINE **, int *, int);
int fexist (char *);
int getkey (void);
int nextch (LINE **, int *, int);
int readpattern (char *, char *, int);
int replaces (int, int, int);
int scanner (char *, int, int);
int tgetc (void);
LINE *lalloc (int);
int anycb (void);
int backchar (int, int);
int backdel (int, int);
int backhunt (int, int);
int backline (int, int);
int backpage (int, int);
int backsearch (int, int);
int bclear (BUFFER *);
int cex (int, int);
int cinsert (void);
int copyregion (int, int);
char *complete (char *, char *, int);
int ctlxe (int, int);
int ctlxlp (int, int);
int ctlxrp (int, int);
int ctoec (int c);
int ctrlg (int, int);
int dcline (int, char **);
int delins (int, char *, int);
int ectoc (int);
void edinit (char *);
int editloop (void);
int execkey (KEYTAB *, int, int);
int execute (int, int, int);
int expandp (char *, char *, int);
int ffclose (void);
int ffgetline (void);
int ffputline (char *, int);
int ffropen (char *);
int ffwopen (char *);
int fileread (int, int);
int filesave (int, int);
int filewrite (int, int);
int forwchar (int, int);
int forwdel (int, int);
int forwhunt (int, int);
int forwline (int, int);
int forwpage (int, int);
int forwsearch (int, int);
BUFFER *getcbuf (char *prompt, char *defval, int createflag);
int getccol (int);
int getcline (void);
int getcmd (void);
int getgoal (LINE *);
int getregion (REGION *);
int getstring (char *, char *, int, int);
int gotobob (int, int);
int gotobol (int, int);
int gotoeob (int, int);
int gotoeol (int, int);
int gotomark (int, int);
char *gtfilename (char *);
int indent (int, int);
void initchars (void);
int insspace (int, int);
void kdelete (void);
int killbuffer (int, int);
int killregion (int, int);
int killtext (int, int);
int kinsert (char);
void lchange (int);
int ldelete (long, int);
int ldelnewline (void);
void lfree (LINE *);
int linsert (int, char);
int linstr (char *);
int lnewline (void);
int lover (char *ostr);
int lowrite (char c);
int meexit (int);
int meta (int, int);
void mlerase (void);
void mlout (int);
void mlputf (int);
void mlputi (int, int);
void mlputli (long, int);
void mlputs (char *);
int mlreply (char *prompt, char *buf, int nbuf);
int mlyesno (char *);
void modeline (WINDOW *);
void movecursor (int, int);
int namebuffer (int, int);
int newline (int, int);
int nextbuffer (int, int);
int nullproc (int, int);
void outstring (char *);
int qreplace (int, int);
int quickexit (int, int);
int quit (int, int);
int quote (int, int);
int rdonly (void);
int readin (char *, int);
int reframe (WINDOW *);
int refresh (int, int);
int resterr (void);
void rvstrcpy (char *, char *);
void savematch (void);
void setjtable (char *);
int setmark (int, int);
int spawncli (int, int);
int sreplace (int, int);
int swbuffer (BUFFER *);
int tab (int, int);
int typahead (void);
void unqname (char *);
void updall (WINDOW *);
int update (int);
void upddex (void);
void updext (void);
void updgar (void);
void updone (WINDOW *);
void updoneline (int, int, int);  /* display.c */
void updpos (void);
int updupd (int);
void upmode (void);
int upscreen (int, int);
int usebuffer (int, int);
void vteeol (void);
void vtinit (void);
void vtmove (int, int);
void vtputc (int);
void vttidy (void);
int writeout (char *);
int yank (int, int);
int zotbuf (BUFFER *);
unsigned int chcase (unsigned int);
KEYTAB *getbind (int);

#if CLEAN
void clean (void);

#endif

char upperc (char);

int islower (unsigned int);
int isupper (unsigned int);
int isletter (unsigned int);

void mlwrite (char *,...);

/* some library redefinitions */

char *strcat ();
char *strcpy ();
char *malloc ();

/* emenu.c */
int mainmenu (void);
void initmenus (void);

/* edit_str.c */
int edit_string (int, int, int, int, char *, char *);

int ttopen (void);
int ttgetc (void);
int ttputc (int);
int ttflush (void);
int ttclose (void);
int tcapkopen (void);
int tcapkclose (void);
int tcapgetc (void);
int tcapmove (int, int);
int tcapeeol (void);
int tcapeeop (void);
int tcapbeep (void);
int tcaprev (int);
int tcapcres (void);
int tcapopen (void);
int tcapclose (void);
char *tgoto ();
int ttputs ();
void putpad (char *);
void vtfree (void);

/* ewindow.c */
int reposition (int, int);
int nextwind (int, int);
int prevwind (int, int);
int mvdnwind (int, int);
int mvupwind (int, int);
int onlywind (int, int);
int delwind (int, int);
int splitwind (int, int);
int enlargewind (int, int);
int shrinkwind (int, int);
int resize (int, int);
int newsize (int, int);		  /* resize the screen, re-writing the screen */
int newwidth (int, int);	  /* resize the screen, re-writing the screen */
int new_col_org (int, int);	  /* reposition the screen, re-writing the
				   * screen */
int new_row_org (int, int);	  /* reposition the screen, re-writing the
				   * screen */

#if	MEMMAP
int updateline (int row, struct VIDEO * vp1);

#else
int updateline (int, struct VIDEO *, struct VIDEO *);

#endif

/* compile.c */
int comp (int f, int n);

/* run.c */
int runprogram (int, int);
int continueprogram (int, int);

/* for eline.c  in compile.c */
void set_need_compile ();

/* for ebind.h in compile.c */
int next_window (int, int);
