/*
 *  $Id: hardware.h,v 1.1 1995/01/06 21:45:10 sev Exp $
 *
 * ---------------------------------------------------------- 
 *
 * $Log: hardware.h,v $
 * Revision 1.1  1995/01/06 21:45:10  sev
 * Initial revision
 *
 * Revision 1.3  1994/07/04  20:19:39  sev
 * Added getword definition
 *
 * Revision 1.2  1994/07/04  19:24:31  sev
 * Added regs. Changed flags
 *
 * Revision 1.1  1994/06/29  12:43:01  sev
 * Initial revision
 *
 *
 */

typedef unsigned char byte;
typedef unsigned short word;

/* reg_f =  s z ac 0 p 1 */


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
