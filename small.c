#include <stdio.h>

#define MEMSIZE	(32768 - 1)

#define VERSION "v1.1"

typedef unsigned char byte;
typedef unsigned short word;

/* reg_f =  s z ac 0 p 1 n c */

byte reg_f, reg_a, reg_b, reg_c, reg_d, reg_e, reg_h, reg_l;
word reg_sp, reg_pc;
byte interrupt_state;

byte *memory;		/* CPU's memory */

byte outport[256];		/* out ports values */
byte inport[256];		/* in ports values */

int terminateprogram;		/* set if need stop executing */

long tacts_while_running;

#define add_tacts(t)	(tacts_while_running += 3)

#define True 0xff
#define False 0x00

#define inc(a)	(a)++
#define dec(a)	(a)--;

byte gcmd (void);		  /* docom.c */
void AddAC (byte);
void AddA (byte);
void AddHL (word);
void AndA (byte);
void Call (byte);
void CpA (byte);
void Daa (void);
void DecR (byte *);
void ExB (byte *, byte *);
int FlagP (byte);
int FlagS (byte);
byte GetMem (word);
void InB (byte);
void IncR (byte *);
void Jmp (byte);
void LfShC (void);
void LfSh (void);
void OrA (byte);
void OutB (byte);
void Pop (byte *, byte *);
void Push (byte, byte);
void PutMem (word, byte);
void ResF (char);
void Ret (byte);
void RgShC (void);
void RgSh (void);
void Rst (word);
void SetF (char);
void SubAC (byte);
void SubA (byte);
int WhatF (char);
void XorA (byte);

typedef struct COMM
{
  char name[8];
  int num;
} COMM;
COMM commands[256] = {
  { "nop",	0},	{ "lxi b, ",	2},	{ "stax b", 	0},
  { "inx b",	0},	{ "inr b",	0},	{ "dcr b",	0},
  { "mvi b, ",	1},	{ "rlc",	0},	{ "???",	0},
  { "dad b",	0},	{ "dcx b",	0},	{ "inr c",	0},
  { "ldax b",	0},	{ "dcr c",	0},	{ "mvi c, ",	1},
  { "rrc",	0},	{ "???",	0},	{ "lxi d, ",	2},
  { "stax d",	0},	{ "inx d",	0},	{ "inr d",	0},
  { "dcr d",	0},	{ "mvi d, ",	1},	{ "ral ",	0},
  { "???", 	0},	{ "dad d",	0},	{ "dcx d",	0},
  { "inr e",	0},	{ "ldax d", 	0},	{ "dcr e",	0},
  { "mvi e, ",	1},	{ "rar ",	0},	{ "???",	0},
  { "lxi h, ",	2},	{ "shld ",	2},	{ "inx h",	0},
  { "inr h",	0},	{ "dcr h",	0},	{ "mvi h, ",	1},
  { "daa",	0},	{ "???",	0},	{ "dad h",	0},
  { "lhld  ",	2},	{ "dcx h",	0},	{ "inr l",	0},
  { "dcr l",	0},	{ "mvi l, ",	1},	{ "cma",	0},
  { "???",	0},	{ "lxi sp, ",	2},	{ "sta ",	2},
  { "inx sp",	0},	{ "inr m",	0},	{ "dcr m",	0},
  { "mvi m, ",	1},	{ "stc ",	0},	{ "???",	0},
  { "dad sp",	0},	{ "lda ",	2},	{ "dcx sp",	0},
  { "inr a",	0},	{ "dcr a",	0},	{ "mvi a, ",	1},
  { "cmc",	0},	{ "mov b,b",	0},	{ "mov b,c",	0},
  { "mov b,d",	0},	{ "mov b,e",	0},	{ "mov b,h",	0},
  { "mov b,l",	0},	{ "mov b,m",	0},	{ "mov b,a",	0},
  { "mov c,b",	0},	{ "mov c,c",	0},	{ "mov c,d",	0},
  { "mov c,e",	0},	{ "mov c,h",	0},	{ "mov c,l",	0},
  { "mov c,m",	0},	{ "mov c,a",	0},	{ "mov d,b",	0},
  { "mov d,c",	0},	{ "mov d,d",	0},	{ "mov d,e",	0},
  { "mov d,h",	0},	{ "mov d,l",	0},	{ "mov d,m",	0},
  { "mov d,a",	0},	{ "mov e,b",	0},	{ "mov e,c",	0},
  { "mov e,d",	0},	{ "mov e,e",	0},	{ "mov e,h",	0},
  { "mov e,l",	0},	{ "mov e,m",	0},	{ "mov e,a",	0},
  { "mov h,b",	0},	{ "mov h,c",	0},	{ "mov h,d",	0},
  { "mov h,e",	0},	{ "mov h,h",	0},	{ "mov h,l",	0},
  { "mov h,m",	0},	{ "mov h,a",	0},	{ "mov l,b",	0},
  { "mov l,c",	0},	{ "mov l,d",	0},	{ "mov l,e",	0},
  { "mov l,h",	0},	{ "mov l,l",	0},	{ "mov l,m",	0},
  { "mov l,a",	0},	{ "mov m,b",	0},	{ "mov m,c",	0},
  { "mov m,d",	0},	{ "mov m,e",	0},	{ "mov m,h",	0},
  { "mov m,l",	0},	{ "hlt",	0},	{ "mov m,a",	0},
  { "mov a,b",	0},	{ "mov a,c",	0},	{ "mov a,d",	0},
  { "mov a,e",	0},	{ "mov a,h",	0},	{ "mov a,l",	0},
  { "mov a,m",	0},	{ "mov a,a",	0},	{ "add b",	0},
  { "add c",	0},	{ "add d",	0},	{ "add e",	0},
  { "add h",	0},	{ "add l",	0},	{ "add m",	0},
  { "add a",	0},	{ "adc b",	0},	{ "adc c",	0},
  { "adc d",	0},	{ "adc e",	0},	{ "adc h",	0},
  { "adc l",	0},	{ "adc m",	0},	{ "adc a",	0},
  { "sub b",	0},	{ "sub c",	0},	{ "sub d",	0},
  { "sub e",	0},	{ "sub h",	0},	{ "sub l",	0},
  { "sub m",	0},	{ "sub a",	0},	{ "sbb b",	0},
  { "sbb c",	0},	{ "sbb d",	0},	{ "sbb e",	0},
  { "sbb h",	0},	{ "sbb l",	0},	{ "sbb m",	0},
  { "sbb a",	0},	{ "ana b",	0},	{ "ana c",	0},
  { "ana d",	0},	{ "ana e",	0},	{ "ana h",	0},
  { "ana l",	0},	{ "ana m",	0},	{ "ana a",	0},
  { "xra b",	0},	{ "xra c",	0},	{ "xra d",	0},
  { "xra e",	0},	{ "xra h",	0},	{ "xra l",	0},
  { "xra m",	0},	{ "xra a",	0},	{ "ora b",	0},
  { "ora c",	0},	{ "ora d",	0},	{ "ora e",	0},
  { "ora h",	0},	{ "ora l",	0},	{ "ora m",	0},
  { "ora a",	0},	{ "cmp b",	0},	{ "cmp c",	0},
  { "cmp d",	0},	{ "cmp e",	0},	{ "cmp h",	0},
  { "cmp l",	0},	{ "cmp m",	0},	{ "cmp a",	0},
  { "rnz ",	0},	{ "pop b",	0},	{ "jnz ",	2},
  { "jmp ",	2},	{ "cnz ",	2},	{ "push b",	0},
  { "adi ",	1},	{ "rst 0",	0},	{ "rz ",	0},
  { "ret ",	0},	{ "jz ",	2},	{ "???",	0},
  { "cz ",	2},	{ "call ",	2},	{ "aci ",	1},
  { "rst 1",	0},	{ "rnc ",	0},	{ "pop d",	0},
  { "jnc ",	2},	{ "out ",	1},	{ "cnc ",	2},
  { "push d",	0},	{ "sui ",	1},	{ "rst 2",	0},
  { "rc ",	0},	{ "???",	0},	{ "jc ",	2},
  { "in ",	1},	{ "cc ",	2},	{ "???",	0},
  { "sbi ",	1},	{ "rst 3",	0},	{ "rpo ",	0},
  { "pop h",	0},	{ "jpo ",	2},	{ "xthl ",	0},
  { "cpo ",	2},	{ "push h",	0},	{ "ani ",	1},
  { "rst 4",	0},	{ "rpe ",	0},	{ "pchl ",	0},
  { "jpe ",	2},	{ "xchg ",	0},	{ "cpe ",	2},
  { "???",	0},	{ "xri ",	1},	{ "rst 5",	0},
  { "rp ",	0},	{ "pop psw",	0},	{ "jp ",	2},
  { "di ",	0},	{ "cp ",	2},	{ "push psw",	0},
  { "ori ",	1},	{ "rst 6",	0},	{ "rm ",	0},
  { "sphl ",	0},	{ "jm ",	2},	{ "ei ",	0},
  { "cm ",	2},	{ "???",	0},	{ "cpi ",	1},
  { "rst 7",	0}};

void do_command (void);
void do_command ()
{
  byte a, h, l, m;
  word hl, bc;

  terminateprogram = 0;
  a = gcmd ();

  switch (a)
  {
    case 0x00:			  /* nop  */
      add_tacts (5);
      break;
    case 0x01:			  /* lxi b,■■ */
      add_tacts (10);
      reg_c = gcmd ();
      reg_b = gcmd ();
      break;
    case 0x02:			  /* stax b */
      add_tacts (7);
      bc = reg_c + reg_b * 256;
      PutMem (bc, reg_a);
      break;
    case 0x03:			  /* inx b */
      add_tacts (5);
      inc (reg_c);
      if (!reg_c)
	inc (reg_b);
      break;
    case 0x04:			  /* inr b */
      add_tacts (5);
      IncR (&reg_b);
      break;
    case 0x05:			  /* dcr b */
      add_tacts (5);
      DecR (&reg_b);
      break;
    case 0x06:			  /* mvi b,■ */
      add_tacts (7);
      reg_b = gcmd ();
      break;
    case 0x07:			  /* rlc  */
      add_tacts (4);
      LfSh ();
      ResF ('h');
      ResF ('n');
      break;
    case 0x09:			  /* dad b */
      add_tacts (10);
      AddHL (reg_c + reg_b * 256);
      break;
    case 0x0C:			  /* ldax b */
      add_tacts (7);
      IncR (&reg_c);
      break;
    case 0x0a:			  /* dcx b */
      add_tacts (5);
      reg_a = GetMem (reg_c + reg_b * 256);
      break;
    case 0x0b:			  /* inr c */
      add_tacts (5);
      dec (reg_c);
      if (reg_c == 0xff)
	dec (reg_b);
      break;
    case 0x0d:			  /* dcr c */
      add_tacts (5);
      DecR (&reg_c);
      break;
    case 0x0e:			  /* mvi c,■ */
      add_tacts (7);
      reg_c = gcmd ();
      break;
    case 0x0f:			  /* rrc  */
      add_tacts (4);
      RgSh ();
      ResF ('h');
      ResF ('n');
      break;
    case 0x11:			  /* lxi d,■■ */
      add_tacts (10);
      reg_e = gcmd ();
      reg_d = gcmd ();
      break;
    case 0x12:			  /* stax d */
      add_tacts (7);
      bc = reg_e + reg_d * 256;
      PutMem (bc, reg_a);
      break;
    case 0x13:			  /* inx d */
      add_tacts (5);
      inc (reg_e);
      if (!reg_e)
	inc (reg_d);
      break;
    case 0x14:			  /* inr d */
      add_tacts (5);
      IncR (&reg_d);
      break;
    case 0x15:			  /* dcr d */
      add_tacts (5);
      DecR (&reg_d);
      break;
    case 0x16:			  /* mvi d,■ */
      add_tacts (7);
      reg_d = gcmd ();
      break;
    case 0x17:			  /* ral  */
      add_tacts (4);
      LfShC ();
      ResF ('h');
      ResF ('n');
      break;
    case 0x19:			  /* dad d */
      add_tacts (10);
      AddHL (reg_e + reg_d * 256);
      break;
    case 0x1C:			  /* ldax d */
      add_tacts (7);
      IncR (&reg_e);
      break;
    case 0x1a:			  /* dcx d */
      add_tacts (5);
      reg_a = GetMem (reg_e + reg_d * 256);
      break;
    case 0x1b:			  /* inr e */
      add_tacts (5);
      dec (reg_e);
      if (reg_e == 0xff)
	dec (reg_d);
      break;
    case 0x1d:			  /* dcr e */
      add_tacts (5);
      DecR (&reg_e);
      break;
    case 0x1e:			  /* mvi e,■ */
      add_tacts (7);
      reg_e = gcmd ();
      break;
    case 0x1f:			  /* rar  */
      add_tacts (4);
      RgShC ();
      ResF ('h');
      ResF ('n');
      break;
    case 0x21:			  /* lxi h,■■ */
      add_tacts (10);
      reg_l = gcmd ();
      reg_h = gcmd ();
      break;
    case 0x22:			  /* shld ■■ */
      add_tacts (16);
      l = gcmd ();
      h = gcmd ();
      hl = l + h * 256;
      PutMem (hl, reg_l);
      PutMem (hl + 1, reg_h);
      break;
    case 0x23:			  /* inx h */
      add_tacts (5);
      inc (reg_l);
      if (!reg_l)
	inc (reg_h);
      break;
    case 0x24:			  /* inr h */
      add_tacts (5);
      IncR (&reg_h);
      break;
    case 0x25:			  /* dcr h */
      add_tacts (5);
      DecR (&reg_h);
      break;
    case 0x26:			  /* mvi h,■ */
      add_tacts (7);
      reg_h = gcmd ();
      break;
    case 0x27:			  /* daa  */
      add_tacts (4);
      Daa ();
      break;
    case 0x29:			  /* dad h */
      add_tacts (10);
      AddHL (reg_l + reg_h * 256);
      break;
    case 0x2a:			  /* lhld ■■ */
      add_tacts (16);
      l = gcmd ();
      h = gcmd ();
      reg_l = GetMem (l + h * 256);
      reg_h = GetMem (l + h * 256 + 1);
      break;
    case 0x2b:			  /* dcx h */
      add_tacts (5);
      dec (reg_l);
      if (reg_l == 0xff)
	dec (reg_h);
      break;
    case 0x2c:			  /* inr l */
      add_tacts (5);
      IncR (&reg_l);
      break;
    case 0x2d:			  /* dcr l */
      add_tacts (5);
      DecR (&reg_l);
      break;
    case 0x2e:			  /* mvi l,■ */
      add_tacts (7);
      reg_l = gcmd ();
      break;
    case 0x2f:			  /* cma  */
      add_tacts (4);
      reg_a = reg_a ^ 0xff;
      SetF ('n');
      SetF ('h');
      break;
    case 0x31:			  /* lxi sp,■■ */
      add_tacts (10);
      l = gcmd ();
      h = gcmd ();
      reg_sp = h * 256 + l;
      break;
    case 0x32:			  /* sta ■■ */
      add_tacts (13);
      l = gcmd ();
      h = gcmd ();
      hl = l + h * 256;
      PutMem (hl, reg_a);
      break;
    case 0x33:			  /* inx sp */
      add_tacts (5);
      inc (reg_sp);
      break;
    case 0x34:			  /* inr m */
      add_tacts (10);
      m = GetMem (reg_l + reg_h * 256);
      IncR (&m);
      PutMem (reg_l + reg_h * 256, m);
      break;
    case 0x35:			  /* dcr m */
      add_tacts (10);
      m = GetMem (reg_l + reg_h * 256);
      DecR (&m);
      PutMem (reg_l + reg_h * 256, m);
      break;
    case 0x36:			  /* mvi m,■ */
      add_tacts (10);
      PutMem (reg_l + reg_h * 256, gcmd ());
      break;
    case 0x37:			  /* stc  */
      add_tacts (4);
      SetF ('c');
      ResF ('n');
      ResF ('h');
      break;
    case 0x39:			  /* dad sp */
      add_tacts (10);
      AddHL (reg_sp);
      break;
    case 0x3a:			  /* lda ■■ */
      add_tacts (13);
      l = gcmd ();
      h = gcmd ();
      reg_a = GetMem (l + h * 256);
      break;
    case 0x3b:			  /* dcx sp */
      add_tacts (5);
      dec (reg_sp);
      break;
    case 0x3c:			  /* inr a */
      add_tacts (5);
      IncR (&reg_a);
      break;
    case 0x3d:			  /* dcr a */
      add_tacts (5);
      DecR (&reg_a);
      break;
    case 0x3e:			  /* mvi a,■ */
      add_tacts (7);
      reg_a = gcmd ();
      break;
    case 0x3f:			  /* cmc  */
      add_tacts (4);
      if (WhatF ('c'))
	ResF ('c');
      else
	SetF ('c');
      ResF ('n');
      break;
    case 0x40:			  /* mov b,b */
      add_tacts (5);
      reg_b = reg_b;
      break;
    case 0x41:			  /* mov b,c */
      add_tacts (5);
      reg_b = reg_c;
      break;
    case 0x42:			  /* mov b,d */
      add_tacts (5);
      reg_b = reg_d;
      break;
    case 0x43:			  /* mov b,e */
      add_tacts (5);
      reg_b = reg_e;
      break;
    case 0x44:			  /* mov b,h */
      add_tacts (5);
      reg_b = reg_h;
      break;
    case 0x45:			  /* mov b,l */
      add_tacts (5);
      reg_b = reg_l;
      break;
    case 0x46:			  /* mov b,m */
      add_tacts (7);
      reg_b = GetMem (reg_l + reg_h * 256);
      break;
    case 0x47:			  /* mov b,a */
      add_tacts (5);
      reg_b = reg_a;
      break;
    case 0x48:			  /* mov c,b */
      add_tacts (5);
      reg_c = reg_b;
      break;
    case 0x49:			  /* mov c,c */
      add_tacts (5);
      reg_c = reg_c;
      break;
    case 0x4a:			  /* mov c,d */
      add_tacts (5);
      reg_c = reg_d;
      break;
    case 0x4b:			  /* mov c,e */
      add_tacts (5);
      reg_c = reg_e;
      break;
    case 0x4c:			  /* mov c,h */
      add_tacts (5);
      reg_c = reg_h;
      break;
    case 0x4d:			  /* mov c,l */
      add_tacts (5);
      reg_c = reg_l;
      break;
    case 0x4e:			  /* mov c,m */
      add_tacts (7);
      reg_c = GetMem (reg_l + reg_h * 256);
      break;
    case 0x4f:			  /* mov c,a */
      add_tacts (5);
      reg_c = reg_a;
      break;
    case 0x50:			  /* mov d,b */
      add_tacts (5);
      reg_d = reg_b;
      break;
    case 0x51:			  /* mov d,c */
      add_tacts (5);
      reg_d = reg_c;
      break;
    case 0x52:			  /* mov d,d */
      add_tacts (5);
      reg_d = reg_d;
      break;
    case 0x53:			  /* mov d,e */
      add_tacts (5);
      reg_d = reg_e;
      break;
    case 0x54:			  /* mov d,h */
      add_tacts (5);
      reg_d = reg_h;
      break;
    case 0x55:			  /* mov d,l */
      add_tacts (5);
      reg_d = reg_l;
      break;
    case 0x56:			  /* mov d,m */
      add_tacts (7);
      reg_d = GetMem (reg_l + reg_h * 256);
      break;
    case 0x57:			  /* mov d,a */
      add_tacts (5);
      reg_d = reg_a;
      break;
    case 0x58:			  /* mov e,b */
      add_tacts (5);
      reg_e = reg_b;
      break;
    case 0x59:			  /* mov e,c */
      add_tacts (5);
      reg_e = reg_c;
      break;
    case 0x5a:			  /* mov e,d */
      add_tacts (5);
      reg_e = reg_d;
      break;
    case 0x5b:			  /* mov e,e */
      add_tacts (5);
      reg_e = reg_e;
      break;
    case 0x5c:			  /* mov e,h */
      add_tacts (5);
      reg_e = reg_h;
      break;
    case 0x5d:			  /* mov e,l */
      add_tacts (5);
      reg_e = reg_l;
      break;
    case 0x5e:			  /* mov e,m */
      add_tacts (7);
      reg_e = GetMem (reg_l + reg_h * 256);
      break;
    case 0x5f:			  /* mov e,a */
      add_tacts (5);
      reg_e = reg_a;
      break;
    case 0x60:			  /* mov h,b */
      add_tacts (5);
      reg_h = reg_b;
      break;
    case 0x61:			  /* mov h,c */
      add_tacts (5);
      reg_h = reg_c;
      break;
    case 0x62:			  /* mov h,d */
      add_tacts (5);
      reg_h = reg_d;
      break;
    case 0x63:			  /* mov h,e */
      add_tacts (5);
      reg_h = reg_e;
      break;
    case 0x64:			  /* mov h,h */
      add_tacts (5);
      reg_h = reg_h;
      break;
    case 0x65:			  /* mov h,l */
      add_tacts (5);
      reg_h = reg_l;
      break;
    case 0x66:			  /* mov h,m */
      add_tacts (7);
      reg_h = GetMem (reg_l + reg_h * 256);
      break;
    case 0x67:			  /* mov h,a */
      add_tacts (5);
      reg_h = reg_a;
      break;
    case 0x68:			  /* mov l,b */
      add_tacts (5);
      reg_l = reg_b;
      break;
    case 0x69:			  /* mov l,c */
      add_tacts (5);
      reg_l = reg_c;
      break;
    case 0x6a:			  /* mov l,d */
      add_tacts (5);
      reg_l = reg_d;
      break;
    case 0x6b:			  /* mov l,e */
      add_tacts (5);
      reg_l = reg_e;
      break;
    case 0x6c:			  /* mov l,h */
      add_tacts (5);
      reg_l = reg_h;
      break;
    case 0x6d:			  /* mov l,l */
      add_tacts (5);
      reg_l = reg_l;
      break;
    case 0x6e:			  /* mov l,m */
      add_tacts (7);
      reg_l = GetMem (reg_l + reg_h * 256);
      break;
    case 0x6f:			  /* mov l,a */
      add_tacts (5);
      reg_l = reg_a;
      break;
    case 0x70:			  /* mov m,b */
      add_tacts (7);
      PutMem (reg_l + reg_h * 256, reg_b);
      break;
    case 0x71:			  /* mov m,c */
      add_tacts (7);
      PutMem (reg_l + reg_h * 256, reg_c);
      break;
    case 0x72:			  /* mov m,d */
      add_tacts (7);
      PutMem (reg_l + reg_h * 256, reg_d);
      break;
    case 0x73:			  /* mov m,e */
      add_tacts (7);
      PutMem (reg_l + reg_h * 256, reg_e);
      break;
    case 0x74:			  /* mov m,h */
      add_tacts (7);
      PutMem (reg_l + reg_h * 256, reg_h);
      break;
    case 0x75:			  /* mov m,l */
      add_tacts (7);
      PutMem (reg_l + reg_h * 256, reg_l);
      break;
    case 0x76:			  /* hlt  */
      add_tacts (7);
      terminateprogram = 1;
      reg_pc--;
      break;
    case 0x77:			  /* mov m,a */
      add_tacts (7);
      PutMem (reg_l + reg_h * 256, reg_a);
      break;
    case 0x78:			  /* mov a,b */
      add_tacts (5);
      reg_a = reg_b;
      break;
    case 0x79:			  /* mov a,c */
      add_tacts (5);
      reg_a = reg_c;
      break;
    case 0x7a:			  /* mov a,d */
      add_tacts (5);
      reg_a = reg_d;
      break;
    case 0x7b:			  /* mov a,e */
      add_tacts (5);
      reg_a = reg_e;
      break;
    case 0x7c:			  /* mov a,h */
      add_tacts (5);
      reg_a = reg_h;
      break;
    case 0x7d:			  /* mov a,l */
      add_tacts (5);
      reg_a = reg_l;
      break;
    case 0x7e:			  /* mov a,m */
      add_tacts (7);
      reg_a = GetMem (reg_l + reg_h * 256);
      break;
    case 0x7f:			  /* mov a,a */
      add_tacts (5);
      reg_a = reg_a;
      break;
    case 0x80:			  /* add b */
      add_tacts (4);
      AddA (reg_b);
      break;
    case 0x81:			  /* add c */
      add_tacts (4);
      AddA (reg_c);
      break;
    case 0x82:			  /* add d */
      add_tacts (4);
      AddA (reg_d);
      break;
    case 0x83:			  /* add e */
      add_tacts (4);
      AddA (reg_e);
      break;
    case 0x84:			  /* add h */
      add_tacts (4);
      AddA (reg_h);
      break;
    case 0x85:			  /* add l */
      add_tacts (4);
      AddA (reg_l);
      break;
    case 0x86:			  /* add m */
      add_tacts (7);
      AddA (GetMem (reg_l + reg_h * 256));
      break;
    case 0x87:			  /* add a */
      add_tacts (4);
      AddA (reg_a);
      break;
    case 0x88:			  /* adc b */
      add_tacts (4);
      AddAC (reg_b);
      break;
    case 0x89:			  /* adc c */
      add_tacts (4);
      AddAC (reg_c);
      break;
    case 0x8a:			  /* adc d */
      add_tacts (4);
      AddAC (reg_d);
      break;
    case 0x8b:			  /* adc e */
      add_tacts (4);
      AddAC (reg_e);
      break;
    case 0x8c:			  /* adc h */
      add_tacts (4);
      AddAC (reg_h);
      break;
    case 0x8d:			  /* adc l */
      add_tacts (4);
      AddAC (reg_l);
      break;
    case 0x8e:			  /* adc m */
      add_tacts (7);
      AddAC (GetMem (reg_l + reg_h * 256));
      break;
    case 0x8f:			  /* adc a */
      add_tacts (4);
      AddAC (reg_a);
      break;
    case 0x90:			  /* sub b */
      add_tacts (4);
      SubA (reg_b);
      break;
    case 0x91:			  /* sub c */
      add_tacts (4);
      SubA (reg_c);
      break;
    case 0x92:			  /* sub d */
      add_tacts (4);
      SubA (reg_d);
      break;
    case 0x93:			  /* sub e */
      add_tacts (4);
      SubA (reg_e);
      break;
    case 0x94:			  /* sub h */
      add_tacts (4);
      SubA (reg_h);
      break;
    case 0x95:			  /* sub l */
      add_tacts (4);
      SubA (reg_l);
      break;
    case 0x96:			  /* sub m */
      add_tacts (7);
      SubA (GetMem (reg_l + reg_h * 256));
      break;
    case 0x97:			  /* sub a */
      add_tacts (7);
      SubA (reg_a);
      break;
    case 0x98:			  /* sbb b */
      add_tacts (4);
      SubAC (reg_b);
      break;
    case 0x99:			  /* sbb c */
      add_tacts (4);
      SubAC (reg_c);
      break;
    case 0x9a:			  /* sbb d */
      add_tacts (4);
      SubAC (reg_d);
      break;
    case 0x9b:			  /* sbb e */
      add_tacts (4);
      SubAC (reg_e);
      break;
    case 0x9c:			  /* sbb h */
      add_tacts (4);
      SubAC (reg_h);
      break;
    case 0x9d:			  /* sbb l */
      add_tacts (4);
      SubAC (reg_l);
      break;
    case 0x9e:			  /* sbb m */
      add_tacts (7);
      SubAC (GetMem (reg_l + reg_h * 256));
      break;
    case 0x9f:			  /* sbb a */
      add_tacts (4);
      SubAC (reg_a);
      break;
    case 0xa0:			  /* ana b */
      add_tacts (4);
      AndA (reg_b);
      break;
    case 0xa1:			  /* ana c */
      add_tacts (4);
      AndA (reg_c);
      break;
    case 0xa2:			  /* ana d */
      add_tacts (4);
      AndA (reg_d);
      break;
    case 0xa3:			  /* ana e */
      add_tacts (4);
      AndA (reg_e);
      break;
    case 0xa4:			  /* ana h */
      add_tacts (4);
      AndA (reg_h);
      break;
    case 0xa5:			  /* ana l */
      add_tacts (4);
      AndA (reg_l);
      break;
    case 0xa6:			  /* ana m */
      add_tacts (7);
      AndA (GetMem (reg_l + reg_h * 256));
      break;
    case 0xa7:			  /* ana a */
      add_tacts (4);
      AndA (reg_a);
      break;
    case 0xa8:			  /* xra b */
      add_tacts (4);
      XorA (reg_b);
      break;
    case 0xa9:			  /* xra c */
      add_tacts (4);
      XorA (reg_c);
      break;
    case 0xaa:			  /* xra d */
      add_tacts (4);
      XorA (reg_d);
      break;
    case 0xab:			  /* xra e */
      add_tacts (4);
      XorA (reg_e);
      break;
    case 0xac:			  /* xra h */
      add_tacts (4);
      XorA (reg_h);
      break;
    case 0xad:			  /* xra l */
      add_tacts (4);
      XorA (reg_l);
      break;
    case 0xae:			  /* xra m */
      add_tacts (7);
      XorA (GetMem (reg_l + reg_h * 256));
      break;
    case 0xaf:			  /* xra a */
      add_tacts (4);
      XorA (reg_a);
      break;
    case 0xb0:			  /* ora b */
      add_tacts (4);
      OrA (reg_b);
      break;
    case 0xb1:			  /* ora c */
      add_tacts (4);
      OrA (reg_c);
      break;
    case 0xb2:			  /* ora d */
      add_tacts (4);
      OrA (reg_d);
      break;
    case 0xb3:			  /* ora e */
      add_tacts (4);
      OrA (reg_e);
      break;
    case 0xb4:			  /* ora h */
      add_tacts (4);
      OrA (reg_h);
      break;
    case 0xb5:			  /* ora l */
      add_tacts (4);
      OrA (reg_l);
      break;
    case 0xb6:			  /* ora m */
      add_tacts (7);
      OrA (GetMem (reg_l + reg_h * 256));
      break;
    case 0xb7:			  /* ora a */
      add_tacts (4);
      OrA (reg_a);
      break;
    case 0xb8:			  /* cmp b */
      add_tacts (4);
      CpA (reg_b);
      break;
    case 0xb9:			  /* cmp c */
      add_tacts (4);
      CpA (reg_c);
      break;
    case 0xba:			  /* cmp d */
      add_tacts (4);
      CpA (reg_d);
      break;
    case 0xbb:			  /* cmp e */
      add_tacts (4);
      CpA (reg_e);
      break;
    case 0xbc:			  /* cmp h */
      add_tacts (4);
      CpA (reg_h);
      break;
    case 0xbd:			  /* cmp l */
      add_tacts (4);
      CpA (reg_l);
      break;
    case 0xbe:			  /* cmp m */
      add_tacts (7);
      CpA (GetMem (reg_l + reg_h * 256));
      break;
    case 0xbf:			  /* cmp a */
      add_tacts (4);
      CpA (reg_a);
      break;
    case 0xc0:			  /* rnz  */
      if (!WhatF ('z'))
	add_tacts (11);
      else
	add_tacts (5);
      Ret (!WhatF ('z'));
      break;
    case 0xc1:			  /* pop b */
      add_tacts (10);
      Pop (&reg_b, &reg_c);
      break;
    case 0xc2:			  /* jnz ■■ */
      add_tacts (10);
      Jmp (!WhatF ('z'));
      break;
    case 0xc3:			  /* jmp ■■ */
      add_tacts (10);
      Jmp (True);
      break;
    case 0xc4:			  /* cnz ■■ */
      if (!WhatF ('z'))
	add_tacts (17);
      else
	add_tacts (11);
      Call (!WhatF ('z'));
      break;
    case 0xc5:			  /* push b */
      add_tacts (11);
      Push (reg_b, reg_c);
      break;
    case 0xc6:			  /* adi ■ */
      add_tacts (7);
      AddA (gcmd ());
      break;
    case 0xc7:			  /* rst 0 */
      add_tacts (11);
      Rst (0x00);
      break;
    case 0xc8:			  /* rz  */
    if (WhatF ('z'))
	add_tacts (11);
      else
	add_tacts (5);
      Ret (WhatF ('z'));
      break;
    case 0xc9:			  /* ret  */
      add_tacts (10);
      Ret (True);
      break;
    case 0xca:			  /* jz ■■ */
      add_tacts (11);
      Jmp (WhatF ('z'));
      break;
    case 0xcc:			  /* cz ■■ */
    if (WhatF ('z'))
	add_tacts (17);
      else
	add_tacts (11);
      Call (WhatF ('z'));
      break;
    case 0xcd:			  /* call ■■ */
      add_tacts (17);
      Call (True);
      break;
    case 0xce:			  /* aci ■ */
      add_tacts (7);
      AddAC (gcmd ());
      break;
    case 0xcf:			  /* rst 1 */
      add_tacts (11);
      Rst (0x08);
      break;
    case 0xd0:			  /* rnc  */
    if (!WhatF ('c'))
	add_tacts (11);
      else
	add_tacts (15);
      Ret (!WhatF ('c'));
      break;
    case 0xd1:			  /* pop d */
      add_tacts (10);
      Pop (&reg_d, &reg_e);
      break;
    case 0xd2:			  /* jnc ■■ */
      add_tacts (10);
      Jmp (!WhatF ('c'));
      break;
    case 0xd3:			  /* out ■ */
      add_tacts (10);
      OutB (gcmd ());
      break;
    case 0xd4:			  /* cnc ■■ */
    if (!WhatF ('c'))
	add_tacts (17);
      else
	add_tacts (11);
      Call (!WhatF ('c'));
      break;
    case 0xd5:			  /* push d */
      add_tacts (11);
      Push (reg_d, reg_e);
      break;
    case 0xd6:			  /* sui ■ */
      add_tacts (7);
      SubA (gcmd ());
      break;
    case 0xd7:			  /* rst 2 */
      add_tacts (11);
      Rst (0x10);
      break;
    case 0xd8:			  /* rc  */
    if (WhatF ('c'))
	add_tacts (11);
      else
	add_tacts (5);
      Ret (WhatF ('c'));
      break;
    case 0xda:			  /* jc ■■ */
      add_tacts (10);
      Jmp (WhatF ('c'));
      break;
    case 0xdb:			  /* in ■ */
      add_tacts (10);
      InB (gcmd ());
      break;
    case 0xdc:			  /* cc ■■ */
      if (WhatF ('c'))
	add_tacts (17);
      else
	add_tacts (11);
      Call (WhatF ('c'));
      break;
    case 0xde:			  /* sbi ■ */
      add_tacts (7);
      SubAC (gcmd ());
      break;
    case 0xdf:			  /* rst 3 */
      add_tacts (11);
      Rst (0x18);
      break;
    case 0xe0:			  /* rpo  */
    if (!WhatF ('p'))
	add_tacts (11);
      else
	add_tacts (5);
      Ret (!WhatF ('p'));
      break;
    case 0xe1:			  /* pop h */
      add_tacts (10);
      Pop (&reg_h, &reg_l);
      break;
    case 0xe2:			  /* jpo ■■ */
      add_tacts (10);
      Jmp (!WhatF ('p'));
      break;
    case 0xe3:			  /* xthl  */
      add_tacts (18);
      h = GetMem (reg_sp + 1);
      l = GetMem (reg_sp);
      ExB (&reg_h, &h);
      ExB (&reg_l, &l);
      PutMem (reg_sp + 1, h);
      PutMem (reg_sp, l);
      break;
    case 0xe4:			  /* cpo ■■ */
      if (!WhatF ('p'))
	add_tacts (17);
      else
	add_tacts (11);
      Call (!WhatF ('p'));
      break;
    case 0xe5:			  /* push h */
      add_tacts (11);
      Push (reg_h, reg_l);
      break;
    case 0xe6:			  /* ani ■ */
      add_tacts (7);
      AndA (gcmd ());
      break;
    case 0xe7:			  /* rst 4 */
      add_tacts (11);
      Rst (0x20);
      break;
    case 0xe8:			  /* rpe  */
    if (WhatF ('p'))
	add_tacts (11);
      else
	add_tacts (5);
      Ret (WhatF ('p'));
      break;
    case 0xe9:			  /* pchl  */
      add_tacts (5);
      reg_pc = reg_l + reg_h * 256;
      break;
    case 0xea:			  /* jpe ■■ */
      add_tacts (10);
      Jmp (WhatF ('p'));
      break;
    case 0xeb:			  /* xchg  */
      add_tacts (4);
      ExB (&reg_d, &reg_h);
      ExB (&reg_e, &reg_l);
      break;
    case 0xec:			  /* cpe ■■ */
      if (WhatF ('p'))
	add_tacts (17);
      else
	add_tacts (11);
      Call (WhatF ('p'));
      break;
    case 0xee:			  /* xri ■ */
      add_tacts (7);
      XorA (gcmd ());
      break;
    case 0xef:			  /* rst 5 */
      add_tacts (11);
      Rst (0x28);
      break;
    case 0xf0:			  /* rp  */
    if (!WhatF ('s'))
	add_tacts (11);
      else
	add_tacts (5);
      Ret (!WhatF ('s'));
      break;
    case 0xf1:			  /* pop psw */
      add_tacts (10);
      Pop (&reg_a, &reg_f);
      break;
    case 0xf2:			  /* jp ■■ */
      add_tacts (10);
      Jmp (!WhatF ('s'));
      break;
    case 0xf3:			  /* di  */
      add_tacts (4);
      interrupt_state = False;
      break;
    case 0xf4:			  /* cp ■■ */
      if (!WhatF ('s'))
	add_tacts (17);
      else
	add_tacts (11);
      Call (!WhatF ('s'));
      break;
    case 0xf5:			  /* push psw */
      add_tacts (11);
      Push (reg_a, reg_f);
      break;
    case 0xf6:			  /* ori ■ */
      add_tacts (7);
      OrA (gcmd ());
      break;
    case 0xf7:			  /* rst 6 */
      add_tacts (11);
      Rst (0x30);
      break;
    case 0xf8:			  /* rm  */
      if (WhatF ('s'))
	add_tacts (11);
      else
	add_tacts (5);
      Ret (WhatF ('s'));
      break;
    case 0xf9:			  /* sphl  */
      add_tacts (5);
      reg_sp = reg_l + reg_h * 256;
      break;
    case 0xfa:			  /* jm ■■ */
      add_tacts (10);
      Jmp (WhatF ('s'));
      break;
    case 0xfb:			  /* ei  */
      add_tacts (4);
      interrupt_state = True;
      break;
    case 0xfc:			  /* cm ■■ */
      if (WhatF ('s'))
	add_tacts (17);
      else
	add_tacts (11);
      Call (WhatF ('s'));
      break;
    case 0xfe:			  /* cpi ■ */
      add_tacts (7);
      CpA (gcmd ());
      break;
    case 0xff:			  /* rst 7 */
      add_tacts (11);
      Rst (0x38);
      break;
  }
}

byte gcmd (void)
{
  return GetMem(reg_pc++);
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
  return memory[a & MEMSIZE];
}

void InB (byte p)
{
  if (p < 3)
    reg_a = inport[p];
  else
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
  if (p < 3)
    outport[p] = reg_a;
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
  memory[a & MEMSIZE] = c;
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
  reg_a = (c >> 1);
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

main ()
{
  long i;

  if ((memory = (byte *)malloc (MEMSIZE)) == (byte *)NULL)
  {
    printf ("Can not allocate memory\n");
    exit (1);
  }
  for (i = 0; i < MEMSIZE; i++)
    PutMem(i, 0);
  reg_sp = 0;
  debug ();
}

debug ()
{
  int op1, op2;		/* operands */
  char comm[80];	/* full comand */
  char comtype;		/* command type */
  word i, j;
  char str[80];
  FILE *file;
  byte b;
  static word dump_addr = 0, memory_addr = 0, disassembly_addr = 0;
  int prlines;

  printf ("Small I8080 emulator  %s\n", VERSION);

  while (1)
  {
    printf ("> ");
    fflush (stdout);
    gets (comm);
    op1 = op2 = -1;
    parse (comm, &comtype, &op1, &op2);
    switch (comtype)
    {
      case 'm':		/* Enter memory */
        if (op1 == -1)
          op1 = memory_addr;
	i = op1;
	while (1)
	{
	  printf ("%04x ", i);
	  fflush (stdout);
	  if (!strcmp (gets (str), "."))
	    break;
	  sscanf (str, "%x", &b);
	  PutMem (i++, b);
	}
	memory_addr = i;
	break;
      case 'q':		/* quit */
	exit (0);
	break;
      case 'd':		/* dump */
        if (op1 == -1)
          op1 = dump_addr;
	op1 &= 0xfff0;
	if (op2 == -1)
	  op2 = op1 + 256;
	for (i = op1; i < op2; i += 16)
	{
	  printf ("%04x  ", i);
	  for (j = 0; j < 16; j++)
	  {
	    if (j == 8)
	      printf (" ");
	    printf ("%02x ", GetMem(j+i));
	  }
	  printf ("\n");
	}
	dump_addr = op2;
	break;
      case 'w':		/* write */
	sscanf (comm, "w%s", str);
	printf ("Writing to %s...", str);
	fflush (stdout);
	if((file = fopen (str, "w")) == (FILE *)NULL)
	{
	  printf ("Error while opening\n");
	  break;
	}
	if (fwrite (memory, 1, MEMSIZE, file) != MEMSIZE)
	{
	  printf ("Write error\n");
	  fclose (file);
	  break;
	}
	fclose (file);
	printf ("Done\n");
	break;
      case 'r':		/* read */
	sscanf (comm, "r%s", str);
	printf ("Reading from %s...", str);
	fflush (stdout);
	if((file = fopen (str, "r")) == (FILE *)NULL)
	{
	  printf ("Error while opening\n");
	  break;
	}
	if (fread (memory, 1, MEMSIZE, file) != MEMSIZE)
	{
	  printf ("Read error\n");
	  fclose (file);
	  break;
	}
	fclose (file);
	printf ("Done\n");
	break;
      case 'g':		/* Go */
	if (op1 == -1)
	  op1 = 0;
	reg_pc = op1;
	while (!terminateprogram)
	  do_command ();
	break;
      case 's':		/* State */
	printf ("A = %02x  B = %02x  C = %02x  D = %02x  E = %02x  H = %02x  L = % 02x\n",
		reg_a, reg_b, reg_c, reg_d, reg_e, reg_h, reg_l);
	printf ("PC = %04x  SP = %04x  I = %d\n", reg_pc, reg_sp, interrupt_state);
	break;
      case 'l':		/* disassembly */
        if (op1 == -1)
          op1 = disassembly_addr;
        i = op1;
        prlines = 0;
        while (op2 == -1 || i < op2)
	{
	  printf ("%04x  ", i);
	  switch (commands[GetMem (i)].num)
	  {
	    case 0:
	      printf ("%02x\t%s\n", GetMem (i), commands[GetMem (i)].name);
	      i += 1;
	      break;
	    case 1:
	      printf ("%02x %02x\t%s%02x\n", GetMem (i), GetMem (i+1),
				commands[GetMem (i)].name, GetMem (i+1));
	      i += 2;
	      break;
	    case 2:
	      printf ("%02x %02x %02x\t%s%02x%02x\n", GetMem (i), GetMem (i+1),
      				GetMem (i+2), commands[GetMem (i)].name,
					GetMem (i+2), GetMem (i+1));
	      i += 3;
	      break;
	  }
	  prlines++;
	  if (op2 == -1 && prlines == 22)
	    op2 = i;
	}
	disassembly_addr = i;
	break;
      case '?':		/* Help */
	printf ("Small I8080 emulator  %s\n", VERSION);
	printf ("d[[start][,end]]  - dump from start to end\n");
	printf ("m[start]          - edit memory from start\n");
	printf ("w name            - write memory to file\n");
	printf ("r name            - read memory from name\n");
	printf ("g[addr]           - execute program from addr\n");
	printf ("s                 - microprocessor state\n");
	printf ("l[[start][,end]]  - disassembly from start to end\n");
	printf ("q                 - quit\n");
        break;
      default:
	printf ("Unknown command.  Type ? for help\n");
	break;
    }
  }
}

parse (char *comm, int *comtype, int *op1, int *op2)
{
  sscanf (comm, "%c%x,%x", comtype, op1, op2);
}
