/*
 * $Id: min.c,v 1.4 1995/01/14 15:08:09 sev Exp $
 * 
 * ----------------------------------------------------------
 * 
 * $Log: min.c,v $
 * Revision 1.4  1995/01/14 15:08:09  sev
 * Menu works right. Compiler also.
 * Revision 1.3  1995/01/07  20:03:14  sev Maked indent and some
 * editor changes Revision 1.2  1995/01/06  21:45:10  sev I began to do real
 * work
 * 
 * Revision 1.1  1994/06/29  12:43:01  sev Initial revision
 * 
 * 
 */

#define MAIN

#include <stdio.h>
#include "hardware.h"
#include "estruct.h"
#include "proto.h"
#include "commands.h"

int emacs (int, char **);

main (int argc, char **argv)
{
  emacs (argc, argv);
}
