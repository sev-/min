/*
 * $Id: hardware.h,v 1.3 1995/01/14 15:08:09 sev Exp $
 * 
 * ----------------------------------------------------------
 * 
 * $Log: hardware.h,v $
 * Revision 1.3  1995/01/14 15:08:09  sev
 * Menu works right. Compiler also.
 * Revision 1.2  1995/01/07  20:03:14  sev Maked indent and
 * some editor changes Revision 1.1  1995/01/06  21:45:10  sev Initial
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

/* reg_f =  s z ac 0 p 1 */

#define NUMLABELS	100	  /* size of label list */
#define TMPSTRLEN	80	  /* size of temporary strings */


#ifdef MAIN
byte reg_f, reg_a, reg_b, reg_c, reg_d, reg_e, reg_h, reg_l;
word reg_sp, reg_pc;
byte interrupt_state;

byte memory[65536];

#else

extern byte reg_f, reg_a, reg_b, reg_c, reg_d, reg_e, reg_h, reg_l;
extern word reg_sp, reg_pc;
extern byte interrupt_state;

extern byte memory[65536];

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
