/*
 * Unit test of `vstcpd` component
 * File: "vspctd.c"
 */

//----------------------------------------------------------------------------
#include <stdio.h>
#include "socklib.h"
#include "vstcpd.h"
#include "rpc_server.h"
//----------------------------------------------------------------------------
#define HOST "0.0.0.0"
#define PORT 7777
#define PORT_NUM 3
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
// on foreach exit function
void foreach_exit(
  int fd,                // socket
  unsigned ipaddr,       // client IPv4 address
  void *client_context,  // client context
  void *server_context,  // server context
  void *foreach_context, // optional context
  int client_index,      // client index (< client_count)
  int client_count)      // client count
{
  int retv = *((int*) foreach_context);
  vstcpd_client_t *pc = (vstcpd_client_t*) client_context;

  printf(">>> call vsrpc_remote_exit(%i) to client %i of %i",
         retv, client_index, client_count);

  retv = vsrpc_remote_exit(&pc->rpc, retv);

  printf(", return %i\n", retv);
}
//----------------------------------------------------------------------------
int main()
{
  int port, exit_code;
  sl_init();

  printf("\npress ENTER to start\n");
  fgetc(stdin);

  printf("\nvstcpd_start()\n");

  for (port = PORT; port < (PORT + PORT_NUM); port++)
  {
#if 1
    if (vstcpd_start(&serv, rpc_vsrpc_func, def_fn, VSRPC_PERM_ALL,
#else
    if (vstcpd_start(&serv, rpc_vsrpc_func, NULL, VSRPC_PERM_ALL,
#endif
                     HOST, port, MAX_CLIENTS,
                     NULL, // context
                     NULL, // on connect
                     NULL, // on disconnect
                     16, SCHED_FIFO) == 0)
      break;
  }

  if (port == (PORT + PORT_NUM))
  {
    printf("can't create server socket; exit\n");
    return -1;
  }
  else
    printf(">>> TCP server start on %i port\n", port);

#if 1
  printf("press ENTER to say exit to all clients\n");
  fgetc(stdin);
  exit_code = 12345;
  vstcps_foreach(&serv.tcps, &exit_code, foreach_exit);
#endif

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

