/*
 * $Id: docom.c,v 1.8 1995/01/21 15:19:59 sev Exp $
 * 
 * ----------------------------------------------------------
 * 
 * $Log: docom.c,v $
 * Revision 1.8  1995/01/21 15:19:59  sev
 * Now Run works, Ports and regs change, list creates
 *
 * Revision 1.7  1995/01/17  12:33:59  sev
 * Now run screen is done
 * Revision 1.6  1995/01/14  15:08:09  sev Menu works right.
 * Compiler also. Revision 1.5  1995/01/07  20:03:14  sev Maked indent and
 * some editor changes Revision 1.4  1995/01/06  21:45:10  sev It's full
 * emulator IMHO
 * 
 * Revision 1.3  1994/07/04  20:19:39  sev Added all non-arithmetic command
 * (without pchl and xthl)
 * 
 * Revision 1.2  1994/07/04  19:24:31  sev Some commands added. I need table!!!
 * 
 * Revision 1.1  1994/06/29  12:43:01  sev Initial revision
 * 
 * 
 */

#include "hardware.h"
#include "estruct.h"
#include "proto.h"

void do_command ()
{
  byte a, h, l, m;
  word hl, bc;

  terminateprogram = 0;
  a = gcmd ();

  switch (a)
  {
    case 0x00:			  /* nop  */
      break;
    case 0x01:			  /* lxi b,■■ */
      reg_c = gcmd ();
      reg_b = gcmd ();
      break;
    case 0x02:			  /* stax b */
      bc = reg_c + reg_b * 256;
      PutMem (bc, reg_a);
      break;
    case 0x03:			  /* inx b */
      inc (reg_c);
      if (!reg_c)
	inc (reg_b);
      break;
    case 0x04:			  /* inr b */
      IncR (&reg_b);
      break;
    case 0x05:			  /* dcr b */
      DecR (&reg_b);
      break;
    case 0x06:			  /* mvi b,■ */
      reg_b = gcmd ();
      break;
    case 0x07:			  /* rlc  */
      LfSh ();
      ResF ('h');
      ResF ('n');
      break;
    case 0x09:			  /* dad b */
      AddHL (reg_c + reg_b * 256);
      break;
    case 0x0C:			  /* ldax b */
      IncR (&reg_c);
      break;
    case 0x0a:			  /* dcx b */
      reg_a = GetMem (reg_c + reg_b * 256);
      break;
    case 0x0b:			  /* inr c */
      dec (reg_c);
      if (reg_c == 0xff)
	dec (reg_b);
      break;
    case 0x0d:			  /* dcr c */
      DecR (&reg_c);
      break;
    case 0x0e:			  /* mvi c,■ */
      reg_c = gcmd ();
      break;
    case 0x0f:			  /* rrc  */
      RgSh ();
      ResF ('h');
      ResF ('n');
      break;
    case 0x11:			  /* lxi d,■■ */
      reg_e = gcmd ();
      reg_d = gcmd ();
      break;
    case 0x12:			  /* stax d */
      bc = reg_e + reg_d * 256;
      PutMem (bc, reg_a);
      break;
    case 0x13:			  /* inx d */
      inc (reg_e);
      if (!reg_e)
	inc (reg_d);
      break;
    case 0x14:			  /* inr d */
      IncR (&reg_d);
      break;
    case 0x15:			  /* dcr d */
      DecR (&reg_d);
      break;
    case 0x16:			  /* mvi d,■ */
      reg_d = gcmd ();
      break;
    case 0x17:			  /* ral  */
      LfShC ();
      ResF ('h');
      ResF ('n');
      break;
    case 0x19:			  /* dad d */
      AddHL (reg_e + reg_d * 256);
      break;
    case 0x1C:			  /* ldax d */
      IncR (&reg_e);
      break;
    case 0x1a:			  /* dcx d */
      reg_a = GetMem (reg_e + reg_d * 256);
      break;
    case 0x1b:			  /* inr e */
      dec (reg_e);
      if (reg_e == 0xff)
	dec (reg_d);
      break;
    case 0x1d:			  /* dcr e */
      DecR (&reg_e);
      break;
    case 0x1e:			  /* mvi e,■ */
      reg_e = gcmd ();
      break;
    case 0x1f:			  /* rar  */
      RgShC ();
      ResF ('h');
      ResF ('n');
      break;
    case 0x21:			  /* lxi h,■■ */
      reg_l = gcmd ();
      reg_h = gcmd ();
      break;
    case 0x22:			  /* shld ■■ */
      l = gcmd ();
      h = gcmd ();
      hl = l + h * 256;
      PutMem (hl, reg_l);
      PutMem (hl + 1, reg_h);
      break;
    case 0x23:			  /* inx h */
      inc (reg_l);
      if (!reg_l)
	inc (reg_h);
      break;
    case 0x24:			  /* inr h */
      IncR (&reg_h);
      break;
    case 0x25:			  /* dcr h */
      DecR (&reg_h);
      break;
    case 0x26:			  /* mvi h,■ */
      reg_h = gcmd ();
      break;
    case 0x27:			  /* daa  */
      Daa ();
      break;
    case 0x29:			  /* dad h */
      AddHL (reg_l + reg_h * 256);
      break;
    case 0x2a:			  /* lhld ■■ */
      l = gcmd ();
      h = gcmd ();
      reg_l = GetMem (l + h * 256);
      reg_h = GetMem (l + h * 256 + 1);
      break;
    case 0x2b:			  /* dcx h */
      dec (reg_l);
      if (reg_l == 0xff)
	dec (reg_h);
      break;
    case 0x2c:			  /* inr l */
      IncR (&reg_l);
      break;
    case 0x2d:			  /* dcr l */
      DecR (&reg_l);
      break;
    case 0x2e:			  /* mvi l,■ */
      reg_l = gcmd ();
      break;
    case 0x2f:			  /* cma  */
      reg_a = reg_a ^ 0xff;
      SetF ('n');
      SetF ('h');
      break;
    case 0x31:			  /* lxi sp,■■ */
      l = gcmd ();
      h = gcmd ();
      reg_sp = h * 256 + l;
      break;
    case 0x32:			  /* sta ■■ */
      l = gcmd ();
      h = gcmd ();
      hl = l + h * 256;
      PutMem (hl, reg_a);
      break;
    case 0x33:			  /* inx sp */
      inc (reg_sp);
      break;
    case 0x34:			  /* inr m */
      m = GetMem (reg_l + reg_h * 256);
      IncR (&m);
      PutMem (reg_l + reg_h * 256, m);
      break;
    case 0x35:			  /* dcr m */
      m = GetMem (reg_l + reg_h * 256);
      DecR (&m);
      PutMem (reg_l + reg_h * 256, m);
      break;
    case 0x36:			  /* mvi m,■ */
      PutMem (reg_l + reg_h * 256, gcmd ());
      break;
    case 0x37:			  /* stc  */
      SetF ('c');
      ResF ('n');
      ResF ('h');
      break;
    case 0x39:			  /* dad sp */
      AddHL (reg_sp);
      break;
    case 0x3a:			  /* lda ■■ */
      l = gcmd ();
      h = gcmd ();
      reg_a = GetMem (l + h * 256);
      break;
    case 0x3b:			  /* dcx sp */
      dec (reg_sp);
      break;
    case 0x3c:			  /* inr a */
      IncR (&reg_a);
      break;
    case 0x3d:			  /* dcr a */
      DecR (&reg_a);
      break;
    case 0x3e:			  /* mvi a,■ */
      reg_a = gcmd ();
      break;
    case 0x3f:			  /* cmc  */
      if (WhatF ('c'))
	ResF ('c');
      else
	SetF ('c');
      ResF ('n');
      break;
    case 0x40:			  /* mov b,b */
      reg_b = reg_b;
      break;
    case 0x41:			  /* mov b,c */
      reg_b = reg_c;
      break;
    case 0x42:			  /* mov b,d */
      reg_b = reg_d;
      break;
    case 0x43:			  /* mov b,e */
      reg_b = reg_e;
      break;
    case 0x44:			  /* mov b,h */
      reg_b = reg_h;
      break;
    case 0x45:			  /* mov b,l */
      reg_b = reg_l;
      break;
    case 0x46:			  /* mov b,m */
      reg_b = GetMem (reg_l + reg_h * 256);
      break;
    case 0x47:			  /* mov b,a */
      reg_b = reg_a;
      break;
    case 0x48:			  /* mov c,b */
      reg_c = reg_b;
      break;
    case 0x49:			  /* mov c,c */
      reg_c = reg_c;
      break;
    case 0x4a:			  /* mov c,d */
      reg_c = reg_d;
      break;
    case 0x4b:			  /* mov c,e */
      reg_c = reg_e;
      break;
    case 0x4c:			  /* mov c,h */
      reg_c = reg_h;
      break;
    case 0x4d:			  /* mov c,l */
      reg_c = reg_l;
      break;
    case 0x4e:			  /* mov c,m */
      reg_c = GetMem (reg_l + reg_h * 256);
      break;
    case 0x4f:			  /* mov c,a */
      reg_c = reg_a;
      break;
    case 0x50:			  /* mov d,b */
      reg_d = reg_b;
      break;
    case 0x51:			  /* mov d,c */
      reg_d = reg_c;
      break;
    case 0x52:			  /* mov d,d */
      reg_d = reg_d;
      break;
    case 0x53:			  /* mov d,e */
      reg_d = reg_e;
      break;
    case 0x54:			  /* mov d,h */
      reg_d = reg_h;
      break;
    case 0x55:			  /* mov d,l */
      reg_d = reg_l;
      break;
    case 0x56:			  /* mov d,m */
      reg_d = GetMem (reg_l + reg_h * 256);
      break;
    case 0x57:			  /* mov d,a */
      reg_d = reg_a;
      break;
    case 0x58:			  /* mov e,b */
      reg_e = reg_b;
      break;
    case 0x59:			  /* mov e,c */
      reg_e = reg_c;
      break;
    case 0x5a:			  /* mov e,d */
      reg_e = reg_d;
      break;
    case 0x5b:			  /* mov e,e */
      reg_e = reg_e;
      break;
    case 0x5c:			  /* mov e,h */
      reg_e = reg_h;
      break;
    case 0x5d:			  /* mov e,l */
      reg_e = reg_l;
      break;
    case 0x5e:			  /* mov e,m */
      reg_e = GetMem (reg_l + reg_h * 256);
      break;
    case 0x5f:			  /* mov e,a */
      reg_e = reg_a;
      break;
    case 0x60:			  /* mov h,b */
      reg_h = reg_b;
      break;
    case 0x61:			  /* mov h,c */
      reg_h = reg_c;
      break;
    case 0x62:			  /* mov h,d */
      reg_h = reg_d;
      break;
    case 0x63:			  /* mov h,e */
      reg_h = reg_e;
      break;
    case 0x64:			  /* mov h,h */
      reg_h = reg_h;
      break;
    case 0x65:			  /* mov h,l */
      reg_h = reg_l;
      break;
    case 0x66:			  /* mov h,m */
      reg_h = GetMem (reg_l + reg_h * 256);
      break;
    case 0x67:			  /* mov h,a */
      reg_h = reg_a;
      break;
    case 0x68:			  /* mov l,b */
      reg_l = reg_b;
      break;
    case 0x69:			  /* mov l,c */
      reg_l = reg_c;
      break;
    case 0x6a:			  /* mov l,d */
      reg_l = reg_d;
      break;
    case 0x6b:			  /* mov l,e */
      reg_l = reg_e;
      break;
    case 0x6c:			  /* mov l,h */
      reg_l = reg_h;
      break;
    case 0x6d:			  /* mov l,l */
      reg_l = reg_l;
      break;
    case 0x6e:			  /* mov l,m */
      reg_l = GetMem (reg_l + reg_h * 256);
      break;
    case 0x6f:			  /* mov l,a */
      reg_l = reg_a;
      break;
    case 0x70:			  /* mov m,b */
      PutMem (reg_l + reg_h * 256, reg_b);
      break;
    case 0x71:			  /* mov m,c */
      PutMem (reg_l + reg_h * 256, reg_c);
      break;
    case 0x72:			  /* mov m,d */
      PutMem (reg_l + reg_h * 256, reg_d);
      break;
    case 0x73:			  /* mov m,e */
      PutMem (reg_l + reg_h * 256, reg_e);
      break;
    case 0x74:			  /* mov m,h */
      PutMem (reg_l + reg_h * 256, reg_h);
      break;
    case 0x75:			  /* mov m,l */
      PutMem (reg_l + reg_h * 256, reg_l);
      break;
    case 0x76:			  /* hlt  */
      terminateprogram = 1;
      break;
    case 0x77:			  /* mov m,a */
      PutMem (reg_l + reg_h * 256, reg_a);
      break;
    case 0x78:			  /* mov a,b */
      reg_a = reg_b;
      break;
    case 0x79:			  /* mov a,c */
      reg_a = reg_c;
      break;
    case 0x7a:			  /* mov a,d */
      reg_a = reg_d;
      break;
    case 0x7b:			  /* mov a,e */
      reg_a = reg_e;
      break;
    case 0x7c:			  /* mov a,h */
      reg_a = reg_h;
      break;
    case 0x7d:			  /* mov a,l */
      reg_a = reg_l;
      break;
    case 0x7e:			  /* mov a,m */
      reg_a = GetMem (reg_l + reg_h * 256);
      break;
    case 0x7f:			  /* mov a,a */
      reg_a = reg_a;
      break;
    case 0x80:			  /* add b */
      AddA (reg_b);
      break;
    case 0x81:			  /* add c */
      AddA (reg_c);
      break;
    case 0x82:			  /* add d */
      AddA (reg_d);
      break;
    case 0x83:			  /* add e */
      AddA (reg_e);
      break;
    case 0x84:			  /* add h */
      AddA (reg_h);
      break;
    case 0x85:			  /* add l */
      AddA (reg_l);
      break;
    case 0x86:			  /* add m */
      AddA (GetMem (reg_l + reg_h * 256));
      break;
    case 0x87:			  /* add a */
      AddA (reg_a);
      break;
    case 0x88:			  /* adc b */
      AddAC (reg_b);
      break;
    case 0x89:			  /* adc c */
      AddAC (reg_c);
      break;
    case 0x8a:			  /* adc d */
      AddAC (reg_d);
      break;
    case 0x8b:			  /* adc e */
      AddAC (reg_e);
      break;
    case 0x8c:			  /* adc h */
      AddAC (reg_h);
      break;
    case 0x8d:			  /* adc l */
      AddAC (reg_l);
      break;
    case 0x8e:			  /* adc m */
      AddAC (GetMem (reg_l + reg_h * 256));
      break;
    case 0x8f:			  /* adc a */
      AddAC (reg_a);
      break;
    case 0x90:			  /* sub b */
      SubA (reg_b);
      break;
    case 0x91:			  /* sub c */
      SubA (reg_c);
      break;
    case 0x92:			  /* sub d */
      SubA (reg_d);
      break;
    case 0x93:			  /* sub e */
      SubA (reg_e);
      break;
    case 0x94:			  /* sub h */
      SubA (reg_h);
      break;
    case 0x95:			  /* sub l */
      SubA (reg_l);
      break;
    case 0x96:			  /* sub m */
      SubA (GetMem (reg_l + reg_h * 256));
      break;
    case 0x97:			  /* sub a */
      SubA (reg_a);
      break;
    case 0x98:			  /* sbb b */
      SubAC (reg_b);
      break;
    case 0x99:			  /* sbb c */
      SubAC (reg_c);
      break;
    case 0x9a:			  /* sbb d */
      SubAC (reg_d);
      break;
    case 0x9b:			  /* sbb e */
      SubAC (reg_e);
      break;
    case 0x9c:			  /* sbb h */
      SubAC (reg_h);
      break;
    case 0x9d:			  /* sbb l */
      SubAC (reg_l);
      break;
    case 0x9e:			  /* sbb m */
      SubAC (GetMem (reg_l + reg_h * 256));
      break;
    case 0x9f:			  /* sbb a */
      SubAC (reg_a);
      break;
    case 0xa0:			  /* ana b */
      AndA (reg_b);
      break;
    case 0xa1:			  /* ana c */
      AndA (reg_c);
      break;
    case 0xa2:			  /* ana d */
      AndA (reg_d);
      break;
    case 0xa3:			  /* ana e */
      AndA (reg_e);
      break;
    case 0xa4:			  /* ana h */
      AndA (reg_h);
      break;
    case 0xa5:			  /* ana l */
      AndA (reg_l);
      break;
    case 0xa6:			  /* ana m */
      AndA (GetMem (reg_l + reg_h * 256));
      break;
    case 0xa7:			  /* ana a */
      AndA (reg_a);
      break;
    case 0xa8:			  /* xra b */
      XorA (reg_b);
      break;
    case 0xa9:			  /* xra c */
      XorA (reg_c);
      break;
    case 0xaa:			  /* xra d */
      XorA (reg_d);
      break;
    case 0xab:			  /* xra e */
      XorA (reg_e);
      break;
    case 0xac:			  /* xra h */
      XorA (reg_h);
      break;
    case 0xad:			  /* xra l */
      XorA (reg_l);
      break;
    case 0xae:			  /* xra m */
      XorA (GetMem (reg_l + reg_h * 256));
      break;
    case 0xaf:			  /* xra a */
      XorA (reg_a);
      break;
    case 0xb0:			  /* ora b */
      OrA (reg_b);
      break;
    case 0xb1:			  /* ora c */
      OrA (reg_c);
      break;
    case 0xb2:			  /* ora d */
      OrA (reg_d);
      break;
    case 0xb3:			  /* ora e */
      OrA (reg_e);
      break;
    case 0xb4:			  /* ora h */
      OrA (reg_h);
      break;
    case 0xb5:			  /* ora l */
      OrA (reg_l);
      break;
    case 0xb6:			  /* ora m */
      OrA (GetMem (reg_l + reg_h * 256));
      break;
    case 0xb7:			  /* ora a */
      OrA (reg_a);
      break;
    case 0xb8:			  /* cmp b */
      CpA (reg_b);
      break;
    case 0xb9:			  /* cmp c */
      CpA (reg_c);
      break;
    case 0xba:			  /* cmp d */
      CpA (reg_d);
      break;
    case 0xbb:			  /* cmp e */
      CpA (reg_e);
      break;
    case 0xbc:			  /* cmp h */
      CpA (reg_h);
      break;
    case 0xbd:			  /* cmp l */
      CpA (reg_l);
      break;
    case 0xbe:			  /* cmp m */
      CpA (GetMem (reg_l + reg_h * 256));
      break;
    case 0xbf:			  /* cmp a */
      CpA (reg_a);
      break;
    case 0xc0:			  /* rnz  */
      Ret (!WhatF ('z'));
      break;
    case 0xc1:			  /* pop b */
      Pop (&reg_b, &reg_c);
      break;
    case 0xc2:			  /* jnz ■■ */
      Jmp (!WhatF ('z'));
      break;
    case 0xc3:			  /* jmp ■■ */
      Jmp (True);
      break;
    case 0xc4:			  /* cnz ■■ */
      Call (!WhatF ('z'));
      break;
    case 0xc5:			  /* push b */
      Push (reg_b, reg_c);
      break;
    case 0xc6:			  /* adi ■ */
      AddA (gcmd ());
      break;
    case 0xc7:			  /* rst 0 */
      Rst (0x00);
      break;
    case 0xc8:			  /* rz  */
      Ret (WhatF ('z'));
      break;
    case 0xc9:			  /* ret  */
      Ret (True);
      break;
    case 0xca:			  /* jz ■■ */
      Jmp (WhatF ('z'));
      break;
    case 0xcc:			  /* cz ■■ */
      Call (WhatF ('z'));
      break;
    case 0xcd:			  /* call ■■ */
      Call (True);
      break;
    case 0xce:			  /* aci ■ */
      AddAC (gcmd ());
      break;
    case 0xcf:			  /* rst 1 */
      Rst (0x08);
      break;
    case 0xd0:			  /* rnc  */
      Ret (!WhatF ('c'));
      break;
    case 0xd1:			  /* pop d */
      Pop (&reg_d, &reg_e);
      break;
    case 0xd2:			  /* jnc ■■ */
      Jmp (!WhatF ('c'));
      break;
    case 0xd3:			  /* out ■ */
      OutB (gcmd ());
      break;
    case 0xd4:			  /* cnc ■■ */
      Call (!WhatF ('c'));
      break;
    case 0xd5:			  /* push d */
      Push (reg_d, reg_e);
      break;
    case 0xd6:			  /* sui ■ */
      SubA (gcmd ());
      break;
    case 0xd7:			  /* rst 2 */
      Rst (0x10);
      break;
    case 0xd8:			  /* rc  */
      Ret (WhatF ('c'));
      break;
    case 0xda:			  /* jc ■■ */
      Jmp (WhatF ('c'));
      break;
    case 0xdb:			  /* in ■ */
      InB (gcmd ());
      break;
    case 0xdc:			  /* cc ■■ */
      Call (WhatF ('c'));
      break;
    case 0xde:			  /* sbi ■ */
      SubAC (gcmd ());
      break;
    case 0xdf:			  /* rst 3 */
      Rst (0x18);
      break;
    case 0xe0:			  /* rpo  */
      Ret (!WhatF ('p'));
      break;
    case 0xe1:			  /* pop h */
      Pop (&reg_h, &reg_l);
      break;
    case 0xe2:			  /* jpo ■■ */
      Jmp (!WhatF ('p'));
      break;
    case 0xe3:			  /* xthl  */
      h = GetMem (reg_sp + 1);
      l = GetMem (reg_sp);
      ExB (&reg_h, &h);
      ExB (&reg_l, &l);
      PutMem (reg_sp + 1, h);
      PutMem (reg_sp, l);
      break;
    case 0xe4:			  /* cpo ■■ */
      Call (!WhatF ('p'));
      break;
    case 0xe5:			  /* push h */
      Push (reg_h, reg_l);
      break;
    case 0xe6:			  /* ani ■ */
      AndA (gcmd ());
      break;
    case 0xe7:			  /* rst 4 */
      Rst (0x20);
      break;
    case 0xe8:			  /* rpe  */
      Ret (WhatF ('p'));
      break;
    case 0xe9:			  /* pchl  */
      reg_pc = reg_l + reg_h * 256;
      break;
    case 0xea:			  /* jpe ■■ */
      Jmp (WhatF ('p'));
      break;
    case 0xeb:			  /* xchg  */
      ExB (&reg_d, &reg_h);
      ExB (&reg_e, &reg_l);
      break;
    case 0xec:			  /* cpe ■■ */
      Call (WhatF ('p'));
      break;
    case 0xee:			  /* xri ■ */
      XorA (gcmd ());
      break;
    case 0xef:			  /* rst 5 */
      Rst (0x28);
      break;
    case 0xf0:			  /* rp  */
      Ret (!WhatF ('s'));
      break;
    case 0xf1:			  /* pop psw */
      Pop (&reg_a, &reg_f);
      break;
    case 0xf2:			  /* jp ■■ */
      Jmp (!WhatF ('s'));
      break;
    case 0xf3:			  /* di  */
      interrupt_state = False;
      break;
    case 0xf4:			  /* cp ■■ */
      Call (!WhatF ('s'));
      break;
    case 0xf5:			  /* push psw */
      Push (reg_a, reg_f);
      break;
    case 0xf6:			  /* ori ■ */
      OrA (gcmd ());
      break;
    case 0xf7:			  /* rst 6 */
      Rst (0x30);
      break;
    case 0xf8:			  /* rm  */
      Ret (WhatF ('s'));
      break;
    case 0xf9:			  /* sphl  */
      reg_sp = reg_l + reg_h * 256;
      break;
    case 0xfa:			  /* jm ■■ */
      Jmp (WhatF ('s'));
      break;
    case 0xfb:			  /* ei  */
      interrupt_state = True;
      break;
    case 0xfc:			  /* cm ■■ */
      Call (WhatF ('s'));
      break;
    case 0xfe:			  /* cpi ■ */
      CpA (gcmd ());
      break;
    case 0xff:			  /* rst 7 */
      Rst (0x38);
      break;
  }
}

byte gcmd (void)
{
  return memory[reg_pc++];
}

void AddAC (byte r)
{
  byte a;

  a = reg_a + r;

  if (WhatF ('c'))
    inc (a);

  if (a < r)
  {
    SetF ('c');
    SetF ('v');
  }
  else
  {
    ResF ('c');
    ResF ('v');
  }

  if (FlagS (a))
    SetF ('s');

  if (!a)
    SetF ('z');
  else
    ResF ('z');

  ResF ('n');

  if (((r & 0x0f) + (reg_a & 0x0f)) > 0x0f)
    SetF ('h');
  else
    ResF ('h');

  reg_a = a;
}

void AddA (byte r)
{
  ResF ('c');
  AddAC (r);
}

void AddHL (word pare)
{
  word hl1, hl;

  hl1 = reg_l + reg_h * 256;
  hl = hl1 + pare;
  reg_l = (hl & 0xff);
  reg_h = ((hl >> 8) & 0xff);
  ResF ('n');

  if (hl1 > hl)
    SetF ('c');
  else
    ResF ('c');
}

void AndA (byte r)
{
  byte a;

  a = (reg_a & r);
  ResF ('n');
  ResF ('c');
  SetF ('h');

  if (FlagP (a))
    SetF ('p');
  else
    ResF ('p');

  if (FlagS (a))
    SetF ('s');
  else
    ResF ('s');

  if (!a)
    SetF ('z');
  else
    ResF ('z');

  reg_a = a;
}

void Call (byte f)
{
  byte h, l;

  l = gcmd ();
  h = gcmd ();

  if (f)
  {
    PutMem (reg_sp - 1, (reg_pc >> 8) & 0xff);
    PutMem (reg_sp - 2, (reg_pc & 0xff));
    reg_sp = reg_sp - 2;
    reg_pc = l + h * 256;
  }
}

void CpA (byte r)
{
  byte a;

  a = reg_a;
  SubA (r);
  ResF ('n');
  reg_a = a;
}

void Daa (void)
{
  byte b, c;

  c = reg_a;
  if (WhatF ('n'))
  {
    if (WhatF ('h'))
      c -= 0x06;
    else if ((c & 0x0f) > 0x09)
      c -= 0x06;

    if ((c & 0xf0) != (reg_a & 0xf0))
      SetF ('h');
    else
      ResF ('h');

    if ((c & 0xf0) > (reg_a & 0xf0))
      SetF ('c');
    else
      ResF ('c');

    b = c;
    if (WhatF ('c'))
      c -= 0x60;
    else if ((c & 0xf0) > 0x90)
      c -= 0x60;

    if ((c & 0xf0) > (b & 0xf0))
      SetF ('c');
  }
  else
  {
    if (WhatF ('h'))
      c += 0x06;
    else if ((c & 0x0f) > 0x09)
      c += 0x06;

    if ((c & 0xf0) != (reg_a & 0xf0))
      SetF ('h');
    else
      ResF ('h');

    if ((c & 0xf0) < (reg_a & 0xf0))
      SetF ('c');
    else
      ResF ('c');

    b = c;
    if (WhatF ('c'))
      c += 0x60;
    else if ((c & 0xf0) > 0x90)
      c += 0x60;

    if ((c & 0xf0) < (b & 0xf0))
      SetF ('c');
  }

  if (!c)
    SetF ('z');
  else
    ResF ('z');

  if (FlagP (c))
    SetF ('p');
  else
    ResF ('p');

  if (FlagS (c))
    SetF ('s');
  else
    ResF ('s');

  reg_a = c;
}

void DecR (byte * r)
{
  byte a;

  a = reg_a;
  reg_a = *r;
  SubA (0x01);
  *r = reg_a;
  reg_a = a;
}

void ExB (byte * a, byte * b)
{
  byte c;

  c = *a;
  *a = *b;
  *b = c;
}

int FlagP (byte a)
{
  byte b, c, i;

  b = 1;
  c = a;
  for (i = 0; i < 8; i++)
  {
    if ((c & 1) != 0)
      b = !b;
    c = c >> 1;
  }
  return b;
}

int FlagS (byte a)
{
  return (a & 0x80);
}

byte GetMem (word a)
{
  return memory[a];
}

void InB (byte p)
{
  reg_a = p;
}

void IncR (byte * r)
{
  byte a;

  a = reg_a;
  reg_a = *r;
  AddA (0x01);
  *r = reg_a;
  reg_a = a;
}

void Jmp (byte f)
{
  byte h, l;

  l = gcmd ();
  h = gcmd ();
  if (f)
    reg_pc = l + h * 256;
}

void LfShC (void)
{
  byte c;

  c = reg_a;
  reg_a = (c << 1);
  if (WhatF ('c'))
    reg_a |= 0x01;

  if ((c & 0x80))
    SetF ('c');
  else
    ResF ('c');
}

void LfSh (void)
{
  byte c;

  c = reg_a;
  reg_a = (c << 1) | (c >> 7);

  if ((c & 0x80))
    SetF ('c');
  else
    ResF ('c');
}

void OrA (byte r)
{
  byte a;

  a = (reg_a | r);
  ResF ('n');
  ResF ('c');
  SetF ('h');

  if (FlagP (a))
    SetF ('p');
  else
    ResF ('p');

  if (FlagS (a))
    SetF ('s');
  else
    ResF ('s');

  if (!a)
    SetF ('z');
  else
    ResF ('z');

  reg_a = a;
}

void OutB (byte p)
{
  p++;
}

void Pop (byte * h, byte * l)
{
  *h = GetMem (reg_sp + 1);
  *l = GetMem (reg_sp);
  reg_sp += 2;
}

void Push (byte h, byte l)
{
  PutMem (reg_sp - 1, h);
  PutMem (reg_sp - 2, l);
  reg_sp -= 2;
}

void PutMem (word a, byte c)
{
  memory[a] = c;
}

void ResF (char a)
{
  switch (a)
  {
      case 'c':reg_f &= 0x01 ^ 0xff;
      break;
    case 'n':
      reg_f &= 0x02 ^ 0xff;
      break;
    case 'p':
      reg_f &= 0x04 ^ 0xff;
      break;
    case 'h':
      reg_f &= 0x10 ^ 0xff;
      break;
    case 'z':
      reg_f &= 0x40 ^ 0xff;
      break;
    case 's':
      reg_f &= 0x80 ^ 0xff;
      break;
  }
}

void Ret (byte f)
{
  byte h, l;

  if (f)
  {
    l = GetMem (reg_sp);
    h = GetMem (reg_sp + 1);
    reg_sp += 2;
    reg_pc = l + h * 256;
  }
}

void RgShC (void)
{
  byte c;

  c = reg_a;
  reg_a = (c << 1);
  if (WhatF ('c'))
    reg_a |= 0x80;

  if (c & 0x01)
    SetF ('c');
  else
    ResF ('c');
}

void RgSh (void)
{
  byte c;

  c = reg_a;
  reg_a = (c >> 1) | (c << 7);

  if (c & 0x01)
    SetF ('c');
  else
    ResF ('c');
}

void Rst (word a)
{
  PutMem (reg_sp - 1, (reg_pc >> 8) & 0xff);
  PutMem (reg_sp - 2, reg_pc & 0xff);
  reg_sp -= 2;
  reg_pc = a;
}

void SetF (char a)
{
  switch (a)
  {
      case 'c':reg_f |= 0x01;
      break;
    case 'n':
      reg_f |= 0x02;
      break;
    case 'p':
      reg_f |= 0x04;
      break;
    case 'h':
      reg_f |= 0x10;
      break;
    case 'z':
      reg_f |= 0x40;
      break;
    case 's':
      reg_f |= 0x80;
      break;
  }
}

void SubAC (byte r)
{
  byte a;

  a = reg_a - r;
  if (WhatF ('c'))
    dec (a);

  if (a > r)
  {
    SetF ('c');
    SetF ('v');
  }
  else
  {
    ResF ('c');
    ResF ('v');
  }

  if (FlagS (a))
    SetF ('s');

  if (!a)
    SetF ('z');
  else
    ResF ('z');

  SetF ('n');
  if ((reg_a & 0x0f) < (r & 0x0f))
    SetF ('h');
  else
    ResF ('h');

  reg_a = a;
}

void SubA (byte r)
{
  ResF ('c');
  SubAC (r);
}

int WhatF (char a)
{
  switch (a)
  {
      case 'c':
      return (reg_f & 0x01) ? 1 : 0;
    case 'n':
      return (reg_f & 0x02) ? 1 : 0;
    case 'p':
      return (reg_f & 0x04) ? 1 : 0;
    case 'h':
      return (reg_f & 0x10) ? 1 : 0;
    case 'z':
      return (reg_f & 0x40) ? 1 : 0;
    case 's':
      return (reg_f & 0x80) ? 1 : 0;
  }
  return reg_f;
}

void XorA (byte r)
{
  byte a;

  a = (reg_a ^ r);
  ResF ('n');
  ResF ('c');
  SetF ('h');

  if (FlagP (a))
    SetF ('p');
  else
    ResF ('p');

  if (FlagS (a))
    SetF ('s');
  else
    ResF ('s');

  if (!a)
    SetF ('z');
  else
    ResF ('z');

  reg_a = a;
}
