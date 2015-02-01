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
#define MAX_CLIENTS 2
//----------------------------------------------------------------------------
vstcps_t serv;
//----------------------------------------------------------------------------
// on connect callback function
void on_connect(
  void **client_context, // client context
  void *server_context,  // server context
  int count)             // clients count
{
  printf("on_connect(%i)\n", count);
}
//----------------------------------------------------------------------------
// on exchange callback function
void on_exchange( 
  int fd,                // socket
  unsigned ipaddr,       // client IPv4 address
  void *client_context,  // client context
  void *server_context)  // server context
{
  char buf[256];
  int i;
  char *msg1 = "Hello, remote User!\nEnter any string:";
  char *msg2 = "Thanks and bye-bye!\n";

  printf("on_exchange(IP=%s) start\n", sl_inet_ntoa(ipaddr));
  
  sl_write(fd, (const void*) msg1, strlen(msg1));
  i = sl_read(fd, (void*) buf, 255);
  if (i > 0)
  {
    buf[i] = '\0';
    printf("Client get: %s\n", buf);
  }
  sl_write(fd, (const void*) msg2, strlen(msg2));
  
  printf("on_exchange() finish\n");
}
//----------------------------------------------------------------------------
// on disconnect callback function
void on_disconnect(void *client_context)
{
  printf("on_disconnect()\n");
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
  sl_init();

  printf("\npress ENTER to start\n");
  fgetc(stdin);
  printf("vstcps_start()\n");
  if (vstcps_start(&serv,
                   HOST, PORT, MAX_CLIENTS,
                   (void*) NULL, // server context
                   on_connect,
                   on_exchange,
                   on_disconnect,
                   16, SCHED_OTHER) != 0)
    return -1;
  
  printf("\npress ENTER to push for each client\n");
  fgetc(stdin);
  printf("vstcps_foreach()\n");
  vstcps_foreach(&serv, NULL, on_foreach);

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

