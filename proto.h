/*
 *  $Id: proto.h,v 1.1 1995/01/06 21:45:10 sev Exp $
 *
 * ---------------------------------------------------------- 
 *
 * $Log: proto.h,v $
 * Revision 1.1  1995/01/06 21:45:10  sev
 * Initial revision
 *
 *
 */

#define True 0xff
#define False 0x00
 
#define inc(a)	(a)++
#define dec(a)	(a)--;
 
byte gcmd(void);	/* docom.c */
void AddAC(byte);
void AddA(byte);
void AddHL(word);
void AndA(byte);
void Call(byte);
void CpA(byte);
void Daa(void);
void DecR(byte *);
void ExB(byte *,byte *);
int  FlagP(byte);
int  FlagS(byte);
byte GetMem(word);
void InB(byte);
void IncR(byte *);
void Jmp(byte);
void LfShC(void);
void LfSh(void);
void OrA(byte);
void OutB(byte);
void Pop(byte *,byte *);
void Push(byte, byte);
void PutMem(word, byte);
void ResF(char);
void Ret(byte);
void RgShC(void);
void RgSh(void);
void Rst(word);
void SetF(char);
void SubAC(byte);
void SubA(byte);
int  WhatF(char);
void XorA(byte);

int compileprogram(void);	/* compile.c */

