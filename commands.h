/*
 * $Id: commands.h,v 1.5 1995/01/17 12:33:59 sev Exp $
 * 
 * ----------------------------------------------------------
 * 
 * $Log: commands.h,v $
 * Revision 1.5  1995/01/17 12:33:59  sev
 * Now run screen is done
 * Revision 1.4  1995/01/14  15:08:09  sev Menu works
 * right. Compiler also. Revision 1.3  1995/01/07  20:03:14  sev Maked indent
 * and some editor changes Revision 1.2  1995/01/06  21:45:10  sev It's full
 * emulator IMHO
 * 
 * Revision 1.1  1994/06/29  12:43:01  sev Initial revision
 * 
 * 
 */

struct COMMAND
{
  byte code;
  char *name;
  char *args;
  char len;
};

struct COMMARG
{
  char *name;
  char arguments;
};

#ifdef MAIN
struct COMMAND commandtable[] =
{
  {
    0x0, "nop", "", 1
  },
  {
    0x1, "lxi", "b,", 3
  },
  {
    0x2, "stax", "b", 1
  },
  {
    0x3, "inx", "b", 1
  },
  {
    0x4, "inr", "b", 1
  },
  {
    0x5, "dcr", "b", 1
  },
  {
    0x6, "mvi", "b,", 2
  },
  {
    0x7, "rlc", "", 1
  },
  {
    0x9, "dad", "b", 1
  },
  {
    0xa, "ldax", "b", 1
  },
  {
    0xb, "dcx", "b", 1
  },
  {
    0xc, "inr", "c", 1
  },
  {
    0xd, "dcr", "c", 1
  },
  {
    0xe, "mvi", "c,", 2
  },
  {
    0xf, "rrc", "", 1
  },
  {
    0x11, "lxi", "d,", 3
  },
  {
    0x12, "stax", "d", 1
  },
  {
    0x13, "inx", "d", 1
  },
  {
    0x14, "inr", "d", 1
  },
  {
    0x15, "dcr", "d", 1
  },
  {
    0x16, "mvi", "d,", 2
  },
  {
    0x17, "ral", "", 1
  },
  {
    0x19, "dad", "d", 1
  },
  {
    0x1a, "ldax", "d", 1
  },
  {
    0x1b, "dcx", "d", 1
  },
  {
    0x1c, "inr", "e", 1
  },
  {
    0x1d, "dcr", "e", 1
  },
  {
    0x1e, "mvi", "e,", 2
  },
  {
    0x1f, "rar", "", 1
  },
  {
    0x21, "lxi", "h,", 3
  },
  {
    0x22, "shld", "", 3
  },
  {
    0x23, "inx", "h", 1
  },
  {
    0x24, "inr", "h", 1
  },
  {
    0x25, "dcr", "h", 1
  },
  {
    0x26, "mvi", "h,", 2
  },
  {
    0x27, "daa", "", 1
  },
  {
    0x29, "dad", "h", 1
  },
  {
    0x2a, "lhld", "", 3
  },
  {
    0x2b, "dcx", "h", 1
  },
  {
    0x2c, "inr", "l", 1
  },
  {
    0x2d, "dcr", "l", 1
  },
  {
    0x2e, "mvi", "l,", 2
  },
  {
    0x2f, "cma", "", 1
  },
  {
    0x31, "lxi", "sp,", 3
  },
  {
    0x32, "sta", "", 3
  },
  {
    0x33, "inx", "sp", 1
  },
  {
    0x34, "inr", "m", 1
  },
  {
    0x35, "dcr", "m", 1
  },
  {
    0x36, "mvi", "m,", 2
  },
  {
    0x37, "stc", "", 1
  },
  {
    0x39, "dad", "sp", 1
  },
  {
    0x3a, "lda", "", 3
  },
  {
    0x3b, "dcx", "sp", 1
  },
  {
    0x3c, "inr", "a", 1
  },
  {
    0x3d, "dcr", "a", 1
  },
  {
    0x3e, "mvi", "a,", 2
  },
  {
    0x3f, "cmc", "", 1
  },
  {
    0x40, "mov", "b,b", 1
  },
  {
    0x41, "mov", "b,c", 1
  },
  {
    0x42, "mov", "b,d", 1
  },
  {
    0x43, "mov", "b,e", 1
  },
  {
    0x44, "mov", "b,h", 1
  },
  {
    0x45, "mov", "b,l", 1
  },
  {
    0x46, "mov", "b,m", 1
  },
  {
    0x47, "mov", "b,a", 1
  },
  {
    0x48, "mov", "c,b", 1
  },
  {
    0x49, "mov", "c,c", 1
  },
  {
    0x4a, "mov", "c,d", 1
  },
  {
    0x4b, "mov", "c,e", 1
  },
  {
    0x4c, "mov", "c,h", 1
  },
  {
    0x4d, "mov", "c,l", 1
  },
  {
    0x4e, "mov", "c,m", 1
  },
  {
    0x4f, "mov", "c,a", 1
  },
  {
    0x50, "mov", "d,b", 1
  },
  {
    0x51, "mov", "d,c", 1
  },
  {
    0x52, "mov", "d,d", 1
  },
  {
    0x53, "mov", "d,e", 1
  },
  {
    0x54, "mov", "d,h", 1
  },
  {
    0x55, "mov", "d,l", 1
  },
  {
    0x56, "mov", "d,m", 1
  },
  {
    0x57, "mov", "d,a", 1
  },
  {
    0x58, "mov", "e,b", 1
  },
  {
    0x59, "mov", "e,c", 1
  },
  {
    0x5a, "mov", "e,d", 1
  },
  {
    0x5b, "mov", "e,e", 1
  },
  {
    0x5c, "mov", "e,h", 1
  },
  {
    0x5d, "mov", "e,l", 1
  },
  {
    0x5e, "mov", "e,m", 1
  },
  {
    0x5f, "mov", "e,a", 1
  },
  {
    0x60, "mov", "h,b", 1
  },
  {
    0x61, "mov", "h,c", 1
  },
  {
    0x62, "mov", "h,d", 1
  },
  {
    0x63, "mov", "h,e", 1
  },
  {
    0x64, "mov", "h,h", 1
  },
  {
    0x65, "mov", "h,l", 1
  },
  {
    0x66, "mov", "h,m", 1
  },
  {
    0x67, "mov", "h,a", 1
  },
  {
    0x68, "mov", "l,b", 1
  },
  {
    0x69, "mov", "l,c", 1
  },
  {
    0x6a, "mov", "l,d", 1
  },
  {
    0x6b, "mov", "l,e", 1
  },
  {
    0x6c, "mov", "l,h", 1
  },
  {
    0x6d, "mov", "l,l", 1
  },
  {
    0x6e, "mov", "l,m", 1
  },
  {
    0x6f, "mov", "l,a", 1
  },
  {
    0x70, "mov", "m,b", 1
  },
  {
    0x71, "mov", "m,c", 1
  },
  {
    0x72, "mov", "m,d", 1
  },
  {
    0x73, "mov", "m,e", 1
  },
  {
    0x74, "mov", "m,h", 1
  },
  {
    0x75, "mov", "m,l", 1
  },
  {
    0x76, "hlt", "", 1
  },
  {
    0x77, "mov", "m,a", 1
  },
  {
    0x78, "mov", "a,b", 1
  },
  {
    0x79, "mov", "a,c", 1
  },
  {
    0x7a, "mov", "a,d", 1
  },
  {
    0x7b, "mov", "a,e", 1
  },
  {
    0x7c, "mov", "a,h", 1
  },
  {
    0x7d, "mov", "a,l", 1
  },
  {
    0x7e, "mov", "a,m", 1
  },
  {
    0x7f, "mov", "a,a", 1
  },
  {
    0x80, "add", "b", 1
  },
  {
    0x81, "add", "c", 1
  },
  {
    0x82, "add", "d", 1
  },
  {
    0x83, "add", "e", 1
  },
  {
    0x84, "add", "h", 1
  },
  {
    0x85, "add", "l", 1
  },
  {
    0x86, "add", "m", 1
  },
  {
    0x87, "add", "a", 1
  },
  {
    0x88, "adc", "b", 1
  },
  {
    0x89, "adc", "c", 1
  },
  {
    0x8a, "adc", "d", 1
  },
  {
    0x8b, "adc", "e", 1
  },
  {
    0x8c, "adc", "h", 1
  },
  {
    0x8d, "adc", "l", 1
  },
  {
    0x8e, "adc", "m", 1
  },
  {
    0x8f, "adc", "a", 1
  },
  {
    0x90, "sub", "b", 1
  },
  {
    0x91, "sub", "c", 1
  },
  {
    0x92, "sub", "d", 1
  },
  {
    0x93, "sub", "e", 1
  },
  {
    0x94, "sub", "h", 1
  },
  {
    0x95, "sub", "l", 1
  },
  {
    0x96, "sub", "m", 1
  },
  {
    0x97, "sub", "a", 1
  },
  {
    0x98, "sbb", "b", 1
  },
  {
    0x99, "sbb", "c", 1
  },
  {
    0x9a, "sbb", "d", 1
  },
  {
    0x9b, "sbb", "e", 1
  },
  {
    0x9c, "sbb", "h", 1
  },
  {
    0x9d, "sbb", "l", 1
  },
  {
    0x9e, "sbb", "m", 1
  },
  {
    0x9f, "sbb", "a", 1
  },
  {
    0xa0, "ana", "b", 1
  },
  {
    0xa1, "ana", "c", 1
  },
  {
    0xa2, "ana", "d", 1
  },
  {
    0xa3, "ana", "e", 1
  },
  {
    0xa4, "ana", "h", 1
  },
  {
    0xa5, "ana", "l", 1
  },
  {
    0xa6, "ana", "m", 1
  },
  {
    0xa7, "ana", "a", 1
  },
  {
    0xa8, "xra", "b", 1
  },
  {
    0xa9, "xra", "c", 1
  },
  {
    0xaa, "xra", "d", 1
  },
  {
    0xab, "xra", "e", 1
  },
  {
    0xac, "xra", "h", 1
  },
  {
    0xad, "xra", "l", 1
  },
  {
    0xae, "xra", "m", 1
  },
  {
    0xaf, "xra", "a", 1
  },
  {
    0xb0, "ora", "b", 1
  },
  {
    0xb1, "ora", "c", 1
  },
  {
    0xb2, "ora", "d", 1
  },
  {
    0xb3, "ora", "e", 1
  },
  {
    0xb4, "ora", "h", 1
  },
  {
    0xb5, "ora", "l", 1
  },
  {
    0xb6, "ora", "m", 1
  },
  {
    0xb7, "ora", "a", 1
  },
  {
    0xb8, "cmp", "b", 1
  },
  {
    0xb9, "cmp", "c", 1
  },
  {
    0xba, "cmp", "d", 1
  },
  {
    0xbb, "cmp", "e", 1
  },
  {
    0xbc, "cmp", "h", 1
  },
  {
    0xbd, "cmp", "l", 1
  },
  {
    0xbe, "cmp", "m", 1
  },
  {
    0xbf, "cmp", "a", 1
  },
  {
    0xc0, "rnz", "", 1
  },
  {
    0xc1, "pop", "b", 1
  },
  {
    0xc2, "jnz", "", 3
  },
  {
    0xc3, "jmp", "", 3
  },
  {
    0xc4, "cnz", "", 3
  },
  {
    0xc5, "push", "b", 1
  },
  {
    0xc6, "adi", "", 2
  },
  {
    0xc7, "rst", "0", 1
  },
  {
    0xc8, "rz", "", 1
  },
  {
    0xc9, "ret", "", 1
  },
  {
    0xca, "jz", "", 3
  },
  {
    0xcc, "cz", "", 3
  },
  {
    0xcd, "call", "", 3
  },
  {
    0xce, "aci", "", 2
  },
  {
    0xcf, "rst", "1", 1
  },
  {
    0xd0, "rnc", "", 1
  },
  {
    0xd1, "pop", "d", 1
  },
  {
    0xd2, "jnc", "", 3
  },
  {
    0xd3, "out", "", 2
  },
  {
    0xd4, "cnc", "", 3
  },
  {
    0xd5, "push", "d", 1
  },
  {
    0xd6, "sui", "", 2
  },
  {
    0xd7, "rst", "2", 1
  },
  {
    0xd8, "rc", "", 1
  },
  {
    0xda, "jc", "", 3
  },
  {
    0xdb, "in", "", 2
  },
  {
    0xdc, "cc", "", 3
  },
  {
    0xde, "sbi", "", 2
  },
  {
    0xdf, "rst", "3", 1
  },
  {
    0xe0, "rpo", "", 1
  },
  {
    0xe1, "pop", "h", 1
  },
  {
    0xe2, "jpo", "", 3
  },
  {
    0xe3, "xthl", "", 1
  },
  {
    0xe4, "cpo", "", 3
  },
  {
    0xe5, "push", "h", 1
  },
  {
    0xe6, "ani", "", 2
  },
  {
    0xe7, "rst", "4", 1
  },
  {
    0xe8, "rpe", "", 1
  },
  {
    0xe9, "pchl", "", 1
  },
  {
    0xea, "jpe", "", 3
  },
  {
    0xeb, "xchg", "", 1
  },
  {
    0xec, "cpe", "", 3
  },
  {
    0xee, "xri", "", 2
  },
  {
    0xef, "rst", "5", 1
  },
  {
    0xf0, "rp", "", 1
  },
  {
    0xf1, "pop", "psw", 1
  },
  {
    0xf2, "jp", "", 3
  },
  {
    0xf3, "di", "", 1
  },
  {
    0xf4, "cp", "", 3
  },
  {
    0xf5, "push", "psw", 1
  },
  {
    0xf6, "ori", "", 2
  },
  {
    0xf7, "rst", "6", 1
  },
  {
    0xf8, "rm", "", 1
  },
  {
    0xf9, "sphl", "", 1
  },
  {
    0xfa, "jm", "", 3
  },
  {
    0xfb, "ei", "", 1
  },
  {
    0xfc, "cm", "", 3
  },
  {
    0xfe, "cpi", "", 2
  },
  {
    0xff, "rst", "7", 1
  },
  {
    0, 0, 0, 0
  }
};

struct COMMARG commargtable[] =
{
  {
    "aci", BYTE
  },
  {
    "adc", REG
  },
  {
    "add", REG
  },
  {
    "adi", BYTE
  },
  {
    "ana", REG
  },
  {
    "ani", BYTE
  },
  {
    "call", WORD
  },
  {
    "cc", WORD
  },
  {
    "cma", NONE
  },
  {
    "cmc", NONE
  },
  {
    "cmp", REG
  },
  {
    "cm", WORD
  },
  {
    "cnc", WORD
  },
  {
    "cnz", WORD
  },
  {
    "cpe", WORD
  },
  {
    "cpi", BYTE
  },
  {
    "cpo", WORD
  },
  {
    "cp", WORD
  },
  {
    "cz", WORD
  },
  {
    "daa", NONE
  },
  {
    "dad", DREG
  },
  {
    "dcr", REG
  },
  {
    "dcx", DREG
  },
  {
    "di", NONE
  },
  {
    "ei", NONE
  },
  {
    "hlt", NONE
  },
  {
    "inr", REG
  },
  {
    "inx", DREG
  },
  {
    "in", BYTE
  },
  {
    "jc", WORD
  },
  {
    "jmp", WORD
  },
  {
    "jm", WORD
  },
  {
    "jnc", WORD
  },
  {
    "jnz", WORD
  },
  {
    "jpe", WORD
  },
  {
    "jpo", WORD
  },
  {
    "jp", WORD
  },
  {
    "jz", WORD
  },
  {
    "ldax", LDAX
  },
  {
    "lda", WORD
  },
  {
    "lhld", NONE
  },
  {
    "lxi", LXI
  },
  {
    "mov", MOV
  },
  {
    "mvi", MVI
  },
  {
    "nop", NONE
  },
  {
    "ora", REG
  },
  {
    "ori", BYTE
  },
  {
    "out", BYTE
  },
  {
    "pchl", NONE
  },
  {
    "pop", PUSH
  },
  {
    "push", PUSH
  },
  {
    "ral", NONE
  },
  {
    "rar", NONE
  },
  {
    "rc", NONE
  },
  {
    "ret", NONE
  },
  {
    "rlc", NONE
  },
  {
    "rm", NONE
  },
  {
    "rnc", NONE
  },
  {
    "rnz", NONE
  },
  {
    "rpe", NONE
  },
  {
    "rpo", NONE
  },
  {
    "rp", NONE
  },
  {
    "rrc", NONE
  },
  {
    "rst", RST
  },
  {
    "rz", NONE
  },
  {
    "sbb", REG
  },
  {
    "sbi", BYTE
  },
  {
    "shld", WORD
  },
  {
    "sphl", NONE
  },
  {
    "stax", LDAX
  },
  {
    "sta", WORD
  },
  {
    "stc", NONE
  },
  {
    "sub", REG
  },
  {
    "sui", BYTE
  },
  {
    "xchg", NONE
  },
  {
    "xra", REG
  },
  {
    "xri", BYTE
  },
  {
    "xthl", NONE
  },
  {
    0, 0
  }
};

#else
extern struct COMMAND commandtable[];
extern struct COMMARG commargtable[];

#endif
