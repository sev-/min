typedef struct COMM
{
  char name[8];
  int code;
} commands[256] = {
  { "nop",	0},
  { "lxi b, ",	2},
  { "stax b",	0},
  { "inx b",	0},
  { "inr b",	0},
  { "dcr b",	0},
  { "mvi b, ",	1},
  { "rlc",	0},
  { "",		0},
  { "dad b",	0},
  { "dcx b",	0},
  { "inr c",	0},
  { "ldax b",	0},
  { "dcr c",	0},
  { "mvi c, ",	1},
  { "rrc",	0},
  { "",		0},
  { "lxi d, ",	2},
  { "stax d",	0},
  { "inx d",	0},
  { "inr d",	0},
  { "dcr d",	0},
  { "mvi d, ",	1},
  { "ral ",	0},
  { "",		0},
  { "dad d",	0},
  { "dcx d",	0},
  { "inr e",	0},
  { "ldax d",	0},
  { "dcr e",	0},
  { "mvi e, ",	1},
  { "rar ",	0},
  { "",		0},
  { "lxi h, ",	2},
  { "shld ",	2},
  { "inx h",	0},
  { "inr h",	0},
  { "dcr h",	0},
  { "mvi h, ",	1},
  { "daa",	0},
  { "",		0},
  { "dad h",	0},
  { "lhld  ",	2},
  { "dcx h",	0},
  { "inr l",	0},
  { "dcr l",	0},
  { "mvi l, ",	1},
  { "cma",	0},
  { "",		0},
  { "lxi sp, ",	2},
  { "sta ",	2},
  { "inx sp",	0},
  { "inr m",	0},
  { "dcr m",	0},
  { "mvi m, ",	1},
  { "stc ",	0},
  { "",		0},
  { "dad sp",	0},
  { "lda ",	2},
  { "dcx sp",	0},
  { "inr a",	0},
  { "dcr a",	0},
  { "mvi a, ",	1},
  { "cmc",	0},
  { "mov b,b",	0},
  { "mov b,c",	0},
  { "mov b,d",	0},
  { "mov b,e",	0},
  { "mov b,h",	0},
  { "mov b,l",	0},
  { "mov b,m",	0},
  { "mov b,a",	0},
  { "mov c,b",	0},
  { "mov c,c",	0},
  { "mov c,d",	0},
  { "mov c,e",	0},
  { "mov c,h",	0},
  { "mov c,l",	0},
  { "mov c,m",	0},
  { "mov c,a",	0},
  { "mov d,b",	0},
  { "mov d,c",	0},
  { "mov d,d",	0},
  { "mov d,e",	0},
  { "mov d,h",	0},
  { "mov d,l",	0},
  { "mov d,m",	0},
  { "mov d,a",	0},
  { "mov e,b",	0},
  { "mov e,c",	0},
  { "mov e,d",	0},
  { "mov e,e",	0},
  { "mov e,h",	0},
  { "mov e,l",	0},
  { "mov e,m",	0},
  { "mov e,a",	0},
  { "mov h,b",	0},
  { "mov h,c",	0},
  { "mov h,d",	0},
  { "mov h,e",	0},
  { "mov h,h",	0},
  { "mov h,l",	0},
  { "mov h,m",	0},
  { "mov h,a",	0},
  { "mov l,b",	0},
  { "mov l,c",	0},
  { "mov l,d",	0},
  { "mov l,e",	0},
  { "mov l,h",	0},
  { "mov l,l",	0},
  { "mov l,m",	0},
  { "mov l,a",	0},
  { "mov m,b",	0},
  { "mov m,c",	0},
  { "mov m,d",	0},
  { "mov m,e",	0},
  { "mov m,h",	0},
  { "mov m,l",	0},
  { "hlt",	0},
  { "mov m,a",	0},
  { "mov a,b",	0},
  { "mov a,c",	0},
  { "mov a,d",	0},
  { "mov a,e",	0},
  { "mov a,h",	0},
  { "mov a,l",	0},
  { "mov a,m",	0},
  { "mov a,a",	0},
  { "add b",	0},
  { "add c",	0},
  { "add d",	0},
  { "add e",	0},
  { "add h",	0},
  { "add l",	0},
  { "add m",	0},
  { "add a",	0},
  { "adc b",	0},
  { "adc c",	0},
  { "adc d",	0},
  { "adc e",	0},
  { "adc h",	0},
  { "adc l",	0},
  { "adc m",	0},
  { "adc a",	0},
  { "sub b",	0},
  { "sub c",	0},
  { "sub d",	0},
  { "sub e",	0},
  { "sub h",	0},
  { "sub l",	0},
  { "sub m",	0},
  { "sub a",	0},
  { "sbb b",	0},
  { "sbb c",	0},
  { "sbb d",	0},
  { "sbb e",	0},
  { "sbb h",	0},
  { "sbb l",	0},
  { "sbb m",	0},
  { "sbb a",	0},
  { "ana b",	0},
  { "ana c",	0},
  { "ana d",	0},
  { "ana e",	0},
  { "ana h",	0},
  { "ana l",	0},
  { "ana m",	0},
  { "ana a",	0},
  { "xra b",	0},
  { "xra c",	0},
  { "xra d",	0},
  { "xra e",	0},
  { "xra h",	0},
  { "xra l",	0},
  { "xra m",	0},
  { "xra a",	0},
  { "ora b",	0},
  { "ora c",	0},
  { "ora d",	0},
  { "ora e",	0},
  { "ora h",	0},
  { "ora l",	0},
  { "ora m",	0},
  { "ora a",	0},
  { "cmp b",	0},
  { "cmp c",	0},
  { "cmp d",	0},
  { "cmp e",	0},
  { "cmp h",	0},
  { "cmp l",	0},
  { "cmp m",	0},
  { "cmp a",	0},
  { "rnz ",	0},
  { "pop b",	0},
  { "jnz ",	2},
  { "jmp ",	2},
  { "cnz ",	2},
  { "push b",	0},
  { "adi ",	1},
  { "rst 0",	0},
  { "rz ",	0},
  { "ret ",	0},
  { "jz ",	2},
  { "",		0},
  { "cz ",	2},
  { "call ",	2},
  { "aci ",	1},
  { "rst 1",	0},
  { "rnc ",	0},
  { "pop d",	0},
  { "jnc ",	2},
  { "out ",	1},
  { "cnc ",	2},
  { "push d",	0},
  { "sui ",	1},
  { "rst 2",	0},
  { "rc ",	0},
  { "",		0},
  { "jc ",	2},
  { "in ",	1},
  { "cc ",	2},
  { "",		0},
  { "sbi ",	1},
  { "rst 3",	0},
  { "rpo ",	0},
  { "pop h",	0},
  { "jpo ",	2},
  { "xthl ",	0},
  { "cpo ",	2},
  { "push h",	0},
  { "ani ",	1},
  { "rst 4",	0},
  { "rpe ",	0},
  { "pchl ",	0},
  { "jpe ",	2},
  { "xchg ",	0},
  { "cpe ",	2},
  { "",		0},
  { "xri ",	1},
  { "rst 5",	0},
  { "rp ",	0},
  { "pop psw",	0},
  { "jp ",	2},
  { "di ",	0},
  { "cp ",	2},
  { "push psw",	0},
  { "ori ",	1},
  { "rst 6",	0},
  { "rm ",	0},
  { "sphl ",	0},
  { "jm ",	2},
  { "ei ",	0},
  { "cm ",	2},
  { "",		0},
  { "cpi ",	1},
  { "rst 7",	0}};
