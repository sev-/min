/*
 *  $Id: docom.c,v 1.2 1994/07/04 19:24:31 sev Exp $
 *
 * ---------------------------------------------------------- 
 *
 * $Log: docom.c,v $
 * Revision 1.2  1994/07/04 19:24:31  sev
 * Some commands added. I need table!!!
 *
 * Revision 1.1  1994/06/29  12:43:01  sev
 * Initial revision
 *
 *
 */

#include "regs.h"
 
do_command(command)
unsigned char command;
{
  int tmpreg;

  switch(command)
  {
	case 0x0:	/* nop  */
		break;
	case 0x1:	/* lxi b,■■ */
		reg_c = memory[reg_pc+1];
		reg_b = memory[reg_pc+2];
		reg_pc += 2;
		break;
	case 0x2:	/* stax b */
		memory[reg_bc] = reg_a;
		break;
	case 0x3:	/* inx b */
		tmpreg = reg_bc + 1;
		reg_b = tmpreg & 0xff00;
		reg_c = tmpreg & 0x00ff;
		break;
	case 0x4:	/* inr b */
		reg_b++;
		break;
	case 0x5:	/* dcr b */
		reg_b--;
		break;
	case 0x6:	/* mvi b,■ */
		reg_b = memory[reg_pc+1];
		reg_pc++;
		break;
	case 0x7:	/* rlc  */
		break;
	case 0x9:	/* dad b */
		tmpreg = reg_bc+reg_hl;
		Flag_c = (tmpreg > 0xffff) ? 0xff : 0x00;
		reg_h = tmpreg & 0xff00;
		reg_l = tmpreg & 0x00ff;
		break;
	case 0xa:	/* ldax b */
		reg_a = memory[reg_bc];
		break;
	case 0xb:	/* dcx b */
		tmpreg = reg_bc-1;
		reg_b = tmpreg & 0xff00;
		reg_c = tmpreg & 0x00ff;
		break;
	case 0xc:	/* inr c */
		reg_c++;
		break;
	case 0xd:	/* dcr c */
		break;
	case 0xe:	/* mvi c,■ */
		break;
	case 0xf:	/* rrc  */
		break;
	case 0x11:	/* lxi d,■■ */
		break;
	case 0x12:	/* stax d */
		break;
	case 0x13:	/* inx d */
		break;
	case 0x14:	/* inr d */
		break;
	case 0x15:	/* dcr d */
		break;
	case 0x16:	/* mvi d,■ */
		break;
	case 0x17:	/* ral  */
		break;
	case 0x19:	/* dad d */
		break;
	case 0x1a:	/* ldax d */
		break;
	case 0x1b:	/* dcx d */
		break;
	case 0x1c:	/* inr e */
		break;
	case 0x1d:	/* dcr e */
		break;
	case 0x1e:	/* mvi e,■ */
		break;
	case 0x1f:	/* rar  */
		break;
	case 0x20:	/* rim'  */
		break;
	case 0x21:	/* lxi h,■■ */
		break;
	case 0x22:	/* shld ■■ */
		break;
	case 0x23:	/* inx h */
		break;
	case 0x24:	/* inr h */
		break;
	case 0x25:	/* dcr h */
		break;
	case 0x26:	/* mvi h,■ */
		break;
	case 0x27:	/* daa  */
		break;
	case 0x29:	/* dad h */
		break;
	case 0x2a:	/* lhld ■■ */
		break;
	case 0x2b:	/* dcx h */
		break;
	case 0x2c:	/* inr l */
		break;
	case 0x2d:	/* dcr l */
		break;
	case 0x2e:	/* mvi l,■ */
		break;
	case 0x2f:	/* cma  */
		break;
	case 0x30:	/* sim'  */
		break;
	case 0x31:	/* lxi sp,■■ */
		break;
	case 0x32:	/* sta ■■ */
		break;
	case 0x33:	/* inx sp */
		break;
	case 0x34:	/* inr m */
		break;
	case 0x35:	/* dcr m */
		break;
	case 0x36:	/* mvi m,■ */
		break;
	case 0x37:	/* stc  */
		break;
	case 0x39:	/* dad sp */
		break;
	case 0x3a:	/* lda ■■ */
		break;
	case 0x3b:	/* dcx sp */
		break;
	case 0x3c:	/* inr a */
		break;
	case 0x3d:	/* dcr a */
		break;
	case 0x3e:	/* mvi a,■ */
		break;
	case 0x3f:	/* cmc  */
		break;
	case 0x40:	/* mov b,b */
		break;
	case 0x41:	/* mov b,c */
		break;
	case 0x42:	/* mov b,d */
		break;
	case 0x43:	/* mov b,e */
		break;
	case 0x44:	/* mov b,h */
		break;
	case 0x45:	/* mov b,l */
		break;
	case 0x46:	/* mov b,m */
		break;
	case 0x47:	/* mov b,a */
		break;
	case 0x48:	/* mov c,b */
		break;
	case 0x49:	/* mov c,c */
		break;
	case 0x4a:	/* mov c,d */
		break;
	case 0x4b:	/* mov c,e */
		break;
	case 0x4c:	/* mov c,h */
		break;
	case 0x4d:	/* mov c,l */
		break;
	case 0x4e:	/* mov c,m */
		break;
	case 0x4f:	/* mov c,a */
		break;
	case 0x50:	/* mov d,b */
		break;
	case 0x51:	/* mov d,c */
		break;
	case 0x52:	/* mov d,d */
		break;
	case 0x53:	/* mov d,e */
		break;
	case 0x54:	/* mov d,h */
		break;
	case 0x55:	/* mov d,l */
		break;
	case 0x56:	/* mov d,m */
		break;
	case 0x57:	/* mov d,a */
		break;
	case 0x58:	/* mov e,b */
		break;
	case 0x59:	/* mov e,c */
		break;
	case 0x5a:	/* mov e,d */
		break;
	case 0x5b:	/* mov e,e */
		break;
	case 0x5c:	/* mov e,h */
		break;
	case 0x5d:	/* mov e,l */
		break;
	case 0x5e:	/* mov e,m */
		break;
	case 0x5f:	/* mov e,a */
		break;
	case 0x60:	/* mov h,b */
		break;
	case 0x61:	/* mov h,c */
		break;
	case 0x62:	/* mov h,d */
		break;
	case 0x63:	/* mov h,e */
		break;
	case 0x64:	/* mov h,h */
		break;
	case 0x65:	/* mov h,l */
		break;
	case 0x66:	/* mov h,m */
		break;
	case 0x67:	/* mov h,a */
		break;
	case 0x68:	/* mov l,b */
		break;
	case 0x69:	/* mov l,c */
		break;
	case 0x6a:	/* mov l,d */
		break;
	case 0x6b:	/* mov l,e */
		break;
	case 0x6c:	/* mov l,h */
		break;
	case 0x6d:	/* mov l,l */
		break;
	case 0x6e:	/* mov l,m */
		break;
	case 0x6f:	/* mov l,a */
		break;
	case 0x70:	/* mov m,b */
		break;
	case 0x71:	/* mov m,c */
		break;
	case 0x72:	/* mov m,d */
		break;
	case 0x73:	/* mov m,e */
		break;
	case 0x74:	/* mov m,h */
		break;
	case 0x75:	/* mov m,l */
		break;
	case 0x76:	/* hlt  */
		break;
	case 0x77:	/* mov m,a */
		break;
	case 0x78:	/* mov a,b */
		break;
	case 0x79:	/* mov a,c */
		break;
	case 0x7a:	/* mov a,d */
		break;
	case 0x7b:	/* mov a,e */
		break;
	case 0x7c:	/* mov a,h */
		break;
	case 0x7d:	/* mov a,l */
		break;
	case 0x7e:	/* mov a,m */
		break;
	case 0x7f:	/* mov a,a */
		break;
	case 0x80:	/* add b */
		break;
	case 0x81:	/* add c */
		break;
	case 0x82:	/* add d */
		break;
	case 0x83:	/* add e */
		break;
	case 0x84:	/* add h */
		break;
	case 0x85:	/* add l */
		break;
	case 0x86:	/* add m */
		break;
	case 0x87:	/* add a */
		break;
	case 0x88:	/* adc b */
		break;
	case 0x89:	/* adc c */
		break;
	case 0x8a:	/* adc d */
		break;
	case 0x8b:	/* adc e */
		break;
	case 0x8c:	/* adc h */
		break;
	case 0x8d:	/* adc l */
		break;
	case 0x8e:	/* adc m */
		break;
	case 0x8f:	/* adc a */
		break;
	case 0x90:	/* sub b */
		break;
	case 0x91:	/* sub c */
		break;
	case 0x92:	/* sub d */
		break;
	case 0x93:	/* sub e */
		break;
	case 0x94:	/* sub h */
		break;
	case 0x95:	/* sub l */
		break;
	case 0x96:	/* sub m */
		break;
	case 0x97:	/* sub a */
		break;
	case 0x98:	/* sbb b */
		break;
	case 0x99:	/* sbb c */
		break;
	case 0x9a:	/* sbb d */
		break;
	case 0x9b:	/* sbb e */
		break;
	case 0x9c:	/* sbb h */
		break;
	case 0x9d:	/* sbb l */
		break;
	case 0x9e:	/* sbb m */
		break;
	case 0x9f:	/* sbb a */
		break;
	case 0xa0:	/* ana b */
		break;
	case 0xa1:	/* ana c */
		break;
	case 0xa2:	/* ana d */
		break;
	case 0xa3:	/* ana e */
		break;
	case 0xa4:	/* ana h */
		break;
	case 0xa5:	/* ana l */
		break;
	case 0xa6:	/* ana m */
		break;
	case 0xa7:	/* ana a */
		break;
	case 0xa8:	/* xra b */
		break;
	case 0xa9:	/* xra c */
		break;
	case 0xaa:	/* xra d */
		break;
	case 0xab:	/* xra e */
		break;
	case 0xac:	/* xra h */
		break;
	case 0xad:	/* xra l */
		break;
	case 0xae:	/* xra m */
		break;
	case 0xaf:	/* xra a */
		break;
	case 0xb0:	/* ora b */
		break;
	case 0xb1:	/* ora c */
		break;
	case 0xb2:	/* ora d */
		break;
	case 0xb3:	/* ora e */
		break;
	case 0xb4:	/* ora h */
		break;
	case 0xb5:	/* ora l */
		break;
	case 0xb6:	/* ora m */
		break;
	case 0xb7:	/* ora a */
		break;
	case 0xb8:	/* cmp b */
		break;
	case 0xb9:	/* cmp c */
		break;
	case 0xba:	/* cmp d */
		break;
	case 0xbb:	/* cmp e */
		break;
	case 0xbc:	/* cmp h */
		break;
	case 0xbd:	/* cmp l */
		break;
	case 0xbe:	/* cmp m */
		break;
	case 0xbf:	/* cmp a */
		break;
	case 0xc0:	/* rnz  */
		break;
	case 0xc1:	/* pop b */
		break;
	case 0xc2:	/* jnz ■■ */
		break;
	case 0xc3:	/* jmp ■■ */
		break;
	case 0xc4:	/* cnz ■■ */
		break;
	case 0xc5:	/* push b */
		break;
	case 0xc6:	/* adi ■ */
		break;
	case 0xc7:	/* rst 0 */
		break;
	case 0xc8:	/* rz  */
		break;
	case 0xc9:	/* ret  */
		break;
	case 0xca:	/* jz ■■ */
		break;
	case 0xcc:	/* cz ■■ */
		break;
	case 0xcd:	/* call ■■ */
		break;
	case 0xce:	/* aci ■ */
		break;
	case 0xcf:	/* rst 1 */
		break;
	case 0xd0:	/* rnc  */
		break;
	case 0xd1:	/* pop d */
		break;
	case 0xd2:	/* jnc ■■ */
		break;
	case 0xd3:	/* out ■ */
		break;
	case 0xd4:	/* cnc ■■ */
		break;
	case 0xd5:	/* push d */
		break;
	case 0xd6:	/* sui ■ */
		break;
	case 0xd7:	/* rst 2 */
		break;
	case 0xd8:	/* rc  */
		break;
	case 0xda:	/* jc ■■ */
		break;
	case 0xdb:	/* in ■ */
		break;
	case 0xdc:	/* cc ■■ */
		break;
	case 0xde:	/* sbi ■ */
		break;
	case 0xdf:	/* rst 3 */
		break;
	case 0xe0:	/* rpo  */
		break;
	case 0xe1:	/* pop h */
		break;
	case 0xe2:	/* jpo ■■ */
		break;
	case 0xe3:	/* xthl  */
		break;
	case 0xe4:	/* cpo ■■ */
		break;
	case 0xe5:	/* push h */
		break;
	case 0xe6:	/* ani ■ */
		break;
	case 0xe7:	/* rst 4 */
		break;
	case 0xe8:	/* rpe  */
		break;
	case 0xe9:	/* pchl  */
		break;
	case 0xea:	/* jpe ■■ */
		break;
	case 0xeb:	/* xchg  */
		break;
	case 0xec:	/* cpe ■■ */
		break;
	case 0xee:	/* xri ■ */
		break;
	case 0xef:	/* rst 5 */
		break;
	case 0xf0:	/* rp  */
		break;
	case 0xf1:	/* pop psw */
		break;
	case 0xf2:	/* jp ■■ */
		break;
	case 0xf3:	/* di  */
		break;
	case 0xf4:	/* cp ■■ */
		break;
	case 0xf5:	/* push psw */
		break;
	case 0xf6:	/* ori ■ */
		break;
	case 0xf7:	/* rst 6 */
		break;
	case 0xf8:	/* rm  */
		break;
	case 0xf9:	/* sphl  */
		break;
	case 0xfa:	/* jm ■■ */
		break;
	case 0xfb:	/* ei  */
		break;
	case 0xfc:	/* cm ■■ */
		break;
	case 0xfe:	/* cpi ■ */
		break;
	case 0xff:	/* rst 7 */
		break;
  }
}
