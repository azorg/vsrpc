/*
 * Unit test of `vstcps` component
 * File: "test_vstcps.c"
 */

//----------------------------------------------------------------------------
#include <stdio.h>
#include <string.h>

#include "vstcps.h"
#include "socklib.h"
//----------------------------------------------------------------------------
#define HOST "0.0.0.0"
#define PORT 7777
#define PORT_NUM 3
#define MAX_CLIENTS 2
//----------------------------------------------------------------------------
vstcps_t serv;
//----------------------------------------------------------------------------
// on accept callback function
int on_accept(
  int fd,                // socket
  unsigned ipaddr,       // client IPv4 address
  void **client_context, // client context
  void *server_context,  // server context
  int count)             // clients count
{
  static int reject = 1; // FIXME (bad code)
  
  printf(">>> on_accept(fd=%i, IP=%s, count=%i)",
         fd, sl_inet_ntoa(ipaddr), count);
  
  reject = reject ? 0 : 1;
  
  if (reject)
    printf(" => REJECT (try again)\n");
  else
    printf(" => ACCEPT\n");

  return reject;
}
//----------------------------------------------------------------------------
// on connect callback function
void on_connect( 
  int fd,                // socket
  unsigned ipaddr,       // client IPv4 address
  void *client_context,  // client context
  void *server_context)  // server context
{
  char buf[256];
  int i;
  char *msg1 = "Hello, remote User!\nEnter any string:";
  char *msg2 = "Thanks and bye-bye!\n";

  printf(">>> on_connect(fd=%i, IP=%s) start\n", fd, sl_inet_ntoa(ipaddr));
  
  sl_write(fd, (const void*) msg1, strlen(msg1));
  i = sl_read(fd, (void*) buf, 255);
  if (i > 0)
  {
    buf[i] = '\0';
    printf(">>> client get: %s\n", buf);
  }
  sl_write(fd, (const void*) msg2, strlen(msg2));
  
  printf(">>> on_connect() finish\n");
}
//----------------------------------------------------------------------------
// on disconnect callback function
void on_disconnect(void *client_context)
{
  printf(">>> on_disconnect()\n");
}
//----------------------------------------------------------------------------
// on foreach callback function
void on_foreach(
  int fd,                // socket
  unsigned ipaddr,       // client IPv4 address
  void *client_context,  // client context
  void *server_context,  // server context
  void *foreach_context, // optional context
  int client_index,      // client index (< client_count)
  int client_count)      // client count
{
  char msg[512];
  printf("<<< send broadcast message for each clients (index=%i count=%i)\n",
          client_index, client_count);
  sprintf(msg, "\n>>> broadcast message from server (index=%i count=%i)\n",
          client_index, client_count);
  sl_write(fd, msg, strlen(msg));
}
//----------------------------------------------------------------------------
int main()
{
  int port;
  sl_init();

  printf("\npress ENTER to start\n");
  fgetc(stdin);
  printf("vstcps_start()\n");
  
  for (port = PORT; port < (PORT + PORT_NUM); port++)
  {
    if (vstcps_start(&serv,
                     HOST, port, MAX_CLIENTS,
                     (void*) NULL, // server context
                     on_accept,
                     on_connect,
                     on_disconnect,
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
  printf("\npress ENTER to push for each client\n");
  fgetc(stdin);

  printf("vstcps_foreach()\n");
  vstcps_foreach(&serv, NULL, on_foreach);
#endif

  printf("\npress ENTER to stop\n");
  fgetc(stdin);

  printf("vstcps_stop()\n");
  vstcps_stop(&serv);
  
  printf("\npress ENTER to exit\n");
  fgetc(stdin);
  return 0;
}
//----------------------------------------------------------------------------

/*** end of "test_vstcps.c" file ***/

