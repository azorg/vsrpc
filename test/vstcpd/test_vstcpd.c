/*
 * Unit test of `vstcpd` component
 * File: "vspctd.c"
 */

//----------------------------------------------------------------------------
#include <stdio.h>
#include "socklib.h"
#include "vstcpd.h"
#include "vstcpd_server.h"
//----------------------------------------------------------------------------
#define HOST "0.0.0.0"
#define PORT 7777
#define MAX_CLIENTS 2
//----------------------------------------------------------------------------
vstcpd_t serv;
//----------------------------------------------------------------------------
char **def_fn(vsrpc_t *rpc, int argc, char * const argv[])
{
  int i;
  printf("def_fn: %s", argv[0]);
  for (i = 1; i < argc; i++)
    printf(" %s", argv[i]);
  printf("\n");
  return NULL;
}
//----------------------------------------------------------------------------
int main()
{
  sl_init();

  printf("\npress ENTER to start\n");
  fgetc(stdin);
  
  printf("\nvstcpd_start()\n");

#if 1
  if (vstcpd_start(&serv, vstcpd_vsrpc_func, def_fn, VSRPC_PERM_ALL,
#else
  if (vstcpd_start(&serv, vstcpd_vsrpc_func, NULL, VSRPC_PERM_ALL,
#endif
                  HOST, PORT, MAX_CLIENTS,
                  NULL, // context
                  NULL, // on connect
                  NULL, // on disconnect
                  16, SCHED_FIFO) != 0)
    return -1;
  
  printf("press ENTER to stop\n");
  fgetc(stdin);
  
  printf("\nvstcpd_stop()\n");
  vstcpd_stop(&serv);
  
  printf("press ENTER to exit\n");
  fgetc(stdin);
    
  return 0;
}
//----------------------------------------------------------------------------

/*** end of "vstcpd_test.c" file ***/

