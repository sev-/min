/*
 *  $Id: min.c,v 1.1 1994/06/29 12:43:01 sev Exp $
 *
 * ---------------------------------------------------------- 
 *
 * $Log: min.c,v $
 * Revision 1.1  1994/06/29 12:43:01  sev
 * Initial revision
 *
 *
 */

#include <stdio.h>
#include "commands.h"

main()
{
  int i;

  for(i = 0; i < NUMCOMMANDS; i++)
    printf("\tcase 0x%x:\t/* %s %s%s */\n\t\tbreak;\n",
	command[i].code, command[i].name, command[i].oper,
	 command[i].len == 1 ? "" : command[i].len == 2 ? "■" : "■■");
}
