/*
 * $Id: hardware.h,v 1.8 1995/10/14 15:46:11 sev Exp $
 * 
 * ----------------------------------------------------------
 * 
 * $Log: hardware.h,v $
 * Revision 1.8  1995/10/14 15:46:11  sev
 * Program was in MSDOS and done A _LOT OF_ changes
 *
 * Revision 1.7  1995/01/27  20:52:27  sev
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
 * Revision 1.3  1995/01/14  15:08:09  sev Menu works
 * right. Compiler also. Revision 1.2  1995/01/07  20:03:14  sev Maked indent
 * and some editor changes Revision 1.1  1995/01/06  21:45:10  sev Initial
 * revision
 * 
 * Revision 1.3  1994/07/04  20:19:39  sev Added getword definition
 * 
 * Revision 1.2  1994/07/04  19:24:31  sev Added regs. Changed flags
 * 
 * Revision 1.1  1994/06/29  12:43:01  sev Initial revision
 * 
 * 
 */

typedef unsigned char byte;
typedef unsigned short word;

/* reg_f =  s z ac 0 p 1 n c */

#define NUMLABELS	100	  /* size of label list */
#define TMPSTRLEN	256	  /* size of temporary strings */


#ifdef MAIN
byte reg_f, reg_a, reg_b, reg_c, reg_d, reg_e, reg_h, reg_l;
word reg_sp, reg_pc;
byte interrupt_state;

byte *memory;		/* CPU's memory */
char startlabel[TMPSTRLEN] = "start";	/* name of start program label */

word memoryforout[3];		/* hooks to browse memory */
byte outport[3];		/* out ports values */
byte inport[3];			/* in ports values */

int terminateprogram;		/* set if need stop executing */
int program_has_errors;		/* set if program has errors/warnings */

word last_addr_of_program;
word first_addr_of_program;

int keyboard_type;	/* type of keyboard while edit ports */

long tacts_while_running;
#else
extern byte reg_f, reg_a, reg_b, reg_c, reg_d, reg_e, reg_h, reg_l;
extern word reg_sp, reg_pc;
extern byte interrupt_state;

extern byte *memory;
extern char startlabel[TMPSTRLEN];

extern word memoryforout[3];
extern byte outport[3];
extern byte inport[3];

extern int terminateprogram;
extern int program_has_errors;

extern word last_addr_of_program;
extern word first_addr_of_program;

extern int keyboard_type;

extern long tacts_while_running;
#endif

/* type command parameters */
enum
{
  NONE, BYTE, WORD,
  REG,				  /* a b c d e h l m */
  LDAX,				  /* b d */
  DREG,				  /* b d h sp */
  MOV,				  /* reg, reg */
  LXI,				  /* [b d h sp], word */
  MVI,				  /* [a b c d e h l m], byte */
  RST,				  /* 0 1 2 3 4 5 6 7 */
  PUSH				  /* b d h psw */
};

#define adderror(a,b)	addmessage((a),(b),1)
#define addwarning(a,b)	addmessage((a),(b),2)

#define LISTBUFFERNAME	"List"
#define RUNBUFFERNAME	"Run"
#define ERRORBUFFERNAME	"Message"
#define LABELBUFFERNAME	"Labels"

#define LISTTABSIZE	24	/* size of address/command fields in list */

#define QWERTY	0
#define JCUKEN	1

#define add_tacts(t)	(tacts_while_running += t)

#define MEMSIZE (32768 - 1)
