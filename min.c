/*
 * $Id: min.c,v 1.6 1995/01/24 15:40:39 sev Exp $
 * 
 * ----------------------------------------------------------
 * 
 * $Log: min.c,v $
 * Revision 1.6  1995/01/24 15:40:39  sev
 * Added inverse line while run; play_error; start label; Labels buffer
 *
 * Revision 1.5  1995/01/17  12:33:59  sev
 * Now run screen is done
 * Revision 1.4  1995/01/14  15:08:09  sev Menu works right.
 * Compiler also. Revision 1.3  1995/01/07  20:03:14  sev Maked indent and
 * some editor changes Revision 1.2  1995/01/06  21:45:10  sev I began to do
 * real work
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
  memoryforout[0] = 10;
  memoryforout[1] = 17;
  memoryforout[2] = 20;
  inport[0] = 22;
  inport[1] = 0x55;
  inport[2] = 0xaa;
  outport[0] = 0;
  outport[1] = 0x55;
  outport[2] = 0xff;
  reg_sp = 0xffff;
  emacs (argc, argv);
}
