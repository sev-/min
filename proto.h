/*
 * $Id: proto.h,v 1.5 1995/01/21 15:19:59 sev Exp $
 * 
 * ----------------------------------------------------------
 * 
 * $Log: proto.h,v $
 * Revision 1.5  1995/01/21 15:19:59  sev
 * Now Run works, Ports and regs change, list creates
 *
 * Revision 1.4  1995/01/17  12:33:59  sev
 * Now run screen is done
 * Revision 1.3  1995/01/14  15:08:09  sev Menu works right.
 * Compiler also. Revision 1.2  1995/01/07  20:03:14  sev Maked indent and
 * some editor changes Revision 1.1  1995/01/06  21:45:10  sev Initial
 * revision
 * 
 * 
 */

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

/* warn.c */
void addmessage (LINE *, char *, int);
void clearerrorbuffer (void);
void show_msg (char *);

/* compile.c */
void compileprogram (int);
int comp (int f, int n);

/* run.c */
int runprogram (int, int);
void askstartlabel (void);

/* edit_str.c */
void winopen (int y1, int x1, int y2, int x2, char *title);
void winclose (int y1, int x1, int y2, int x2);

