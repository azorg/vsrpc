/*
 * File: "vstcpd_common.c".
 * This file is autogenerated from "vstcpd.vsidl" file
 * and can be append. You may (and must) edit this file.
 * Recomented backup copy this file to another too.
 */
//----------------------------------------------------------------------------
#include "vsrpc.h"
//----------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
//----------------------------------------------------------------------------
int vstcpd_print(vsrpc_t *rpc, char *str)
{
  printf("print: str='%s'\n", str);
  return atoi(str);
}
//----------------------------------------------------------------------------
static int buf[1024 * 1024 * 4];
void vstcpd_test(vsrpc_t *rpc, int size)
{
  int i;
  
  // read from pipe
  i = vsrpc_read(rpc, (char*) buf, size * sizeof(int));
  if (i != VSRPC_ERR_NONE) printf("\n>>> vsrpc_read() error %i\n", i);
  
  // modify data
  for (i = 0; i < size; i++)
    buf[i] *= (i + 65536);
  
  // return data to pipe
  i = vsrpc_write(rpc, (const char*) buf, size * sizeof(int));
  if (i != VSRPC_ERR_NONE) printf("\n>>> vsrpc_write() error %i\n", i);
}
//----------------------------------------------------------------------------
void cap(vsrpc_t* rpc)
{
  int chr = 0, retv;

  printf("press any keys and 'q' to exit\n");
  
  while (1)
  {
    retv = read(rpc->fd_rd, (void*) &chr, 1);
    if (retv != 1)
    {
      fprintf(stderr, "error in vstcpd_capture(): read(1) return %i\n", retv);
      break;
    }

    printf("chr: 0x%02X -> ", chr);
    if (chr >= 32 && chr <= 127 && chr != 'q')
      printf("'%c'\n", chr);
    else if (chr == 'q')
    {
      printf("'%c' => break\n", chr);
      break;
    }
    else if (chr == '\n')
      printf("'\\n'\n");
    else if (chr == '\r')
      printf("'\\r'\n");
    else
      printf("?\n");
  }
}
//----------------------------------------------------------------------------
void while_one(vsrpc_t* rpc)
{
  // loop forever
  while (1);
}
//----------------------------------------------------------------------------