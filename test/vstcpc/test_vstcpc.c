/*
 * Unit test of `vstcpc` component
 * File: "test_vstcpc.c"
 */

//----------------------------------------------------------------------------
#include <stdio.h>
#include "socklib.h"
#include "vstcpc.h"
#include "vstcpd_wrap.h"
#include "vstcpd_common.h"
//----------------------------------------------------------------------------
#define HOST "127.0.0.1"
//#define HOST "192.168.7.177"
#define PORT 7777
//----------------------------------------------------------------------------
double get_time()
{
  struct timespec tv;
  double t;
  
  clock_gettime(CLOCK_REALTIME, &tv);
  t  = ((double) tv.tv_nsec) * 1e-9;
  t += ((double) tv.tv_sec);

  return t;
}
//----------------------------------------------------------------------------
char buf[1024 * 1024 * 16];
int buf2[1024 * 1024 * 4];
//----------------------------------------------------------------------------
int main()
{
  vstcpc_t obj;
  double t1, t2;
  int i, j, k, id, size, err;

  sl_init();

  printf("\n\npress ENTER to start\n");
  fgetc(stdin);
  
  printf("vstcpc_start()\n");
  if ((i = vstcpc_start(&obj, NULL, NULL, VSRPC_PERM_DEFAULT, HOST, PORT,
                        16, SCHED_OTHER)) != 0)
  {
    fprintf(stderr, "vstcpc_start() return %i\n", i);
    return -1;
  }
  
  printf("press ENTER to ping remote host (3 times)\n");
  fgetc(stdin);
  
  // ping #1
  vsmutex_lock(&obj.mtx_rpc);
  t1 = get_time();
  err = vsrpc_remote_ping(&obj.rpc, &i);
  t2 = get_time();
  printf("t2 - t1 = %f (vsrpc_remote_ping)\n", t2 - t1);
  vsmutex_unlock(&obj.mtx_rpc);
  if (err != VSRPC_ERR_NONE)
    fprintf(stderr, "vsrpc_remote_ping() error %i\n", err);

  // ping #2
  vsmutex_lock(&obj.mtx_rpc);
  t1 = get_time();
  err = vsrpc_remote_ping(&obj.rpc, &i);
  t2 = get_time();
  printf("t2 - t1 = %f (vsrpc_remote_ping)\n", t2 - t1);
  vsmutex_unlock(&obj.mtx_rpc);
  if (err != VSRPC_ERR_NONE)
    fprintf(stderr, "vsrpc_remote_ping() error %i\n", err);
  
  // ping #3
  vsmutex_lock(&obj.mtx_rpc);
  t1 = get_time();
  err = vsrpc_remote_ping(&obj.rpc, &i);
  t2 = get_time();
  printf("t2 - t1 = %f (vsrpc_remote_ping)\n", t2 - t1);
  vsmutex_unlock(&obj.mtx_rpc);
  if (err != VSRPC_ERR_NONE)
    fprintf(stderr, "vsrpc_remote_ping() error %i\n", err);
  
  printf("\npress ENTER to run remote procedure (3 times)\n");
  fgetc(stdin);
  
  // print #1
  vsmutex_lock(&obj.mtx_rpc);
  t1 = get_time();
  i = vstcpd_print(&obj.rpc, "Hello!");
  t2 = get_time();
  printf("t2 - t1 = %f (vstcpd_print)\n", t2 - t1);
  vsmutex_unlock(&obj.mtx_rpc);
  printf(">>> vstcpd_print() return %i\n", i);
  
  // print #2
  vsmutex_lock(&obj.mtx_rpc);
  t1 = get_time();
  i = vstcpd_print(&obj.rpc, "12345");
  t2 = get_time();
  printf("t2 - t1 = %f (vstcpd_print)\n", t2 - t1);
  vsmutex_unlock(&obj.mtx_rpc);
  printf(">>> vstcpd_print() return %i\n", i);
  
  // print #3
  vsmutex_lock(&obj.mtx_rpc);
  t1 = get_time();
  i = vstcpd_print(&obj.rpc, "-12345");
  t2 = get_time();
  printf("t2 - t1 = %f (vstcpd_print)\n", t2 - t1);
  vsmutex_unlock(&obj.mtx_rpc);
  printf(">>> vstcpd_print() return %i\n", i);
  
  printf("\npress ENTER to run write/read test\n");
  fgetc(stdin);

  for (k = 0; k < 3; k++)
  {
    size = 200; //1024 * 1024;
    for (i = 0; i < size; i++)
      buf2[i] = i * i;
    
    vsmutex_lock(&obj.mtx_rpc);
    t1 = get_time();
    err = vstcpd_test_call(&obj.rpc, size);
    if (err != VSRPC_ERR_NONE)
      fprintf(stderr, "vstcpd_test_call() error %i\n", err);
    
    err = vsrpc_write(&obj.rpc, (const char*) buf2, size * sizeof(int));
    if (err != VSRPC_ERR_NONE)
      fprintf(stderr, "vsrpc_write() error %i\n", err);
    
    err = vsrpc_read(&obj.rpc, (char*) buf2, size * sizeof(int));
    if (err != VSRPC_ERR_NONE)
      fprintf(stderr, "vsrpc_read() error %i\n", err);
    
    err = vstcpd_test_wait(&obj.rpc);
    if (err != VSRPC_ERR_NONE)
      fprintf(stderr, "vstcpd_test_wait() error %i\n", err);
    t2 = get_time();
    vsmutex_unlock(&obj.mtx_rpc);
    
    j = 0;
    for (i = 0; i < size; i++)
      if (buf2[i] != i * i * (65536 + i))
        j++;
          
    if (j)
      printf("!!! errors = %i\n", j);
    
    printf("t2 - t1 = %f (write/read %i bytes)\n",
            t2 - t1, (int) (size * sizeof(int)));
  } // for

  size = 10 * 1024 * 1024;
  printf("\npress ENTER to write to server %i bytes\n", size);
  fgetc(stdin);
  
  for (i = 0; i < size; i++)
    buf[i] = (i & 0xFF) ^ 0xAC;
  vsmutex_lock(&obj.mtx_rpc);
  t1 = get_time();
  err = vsrpc_remote_malloc(&obj.rpc, size, &id); // malloc on remote machine
  t2 = get_time();
  if (err != VSRPC_ERR_NONE)
    fprintf(stderr, "vsrpc_remote_malloc() error %i\n", err);
  if (err == -VSRPC_ERR_NONE)
    printf("t2 - t1 = %f (vsrpc_remote_malloc)\n", t2 - t1);
  t1 = get_time();
  err = vsrpc_remote_write(&obj.rpc,
                           (void*) buf, // ptr - source address
                           id, 0, size, 0,
                           &i);
  t2 = get_time();
  vsmutex_unlock(&obj.mtx_rpc);
  if (err != VSRPC_ERR_NONE)
    fprintf(stderr, "vsrpc_remote_write() error %i\n", err);
  if (err == -VSRPC_ERR_NONE && i == size)
    printf("t2 - t1 = %f (vsrpc_remote_write)\n", t2 - t1);
  
  printf("\npress ENTER to read from server %i bytes\n", size);
  fgetc(stdin);
  
  vsmutex_lock(&obj.mtx_rpc);
  t1 = get_time();
  err = vsrpc_remote_read(&obj.rpc,
                          (void*) buf, // ptr - source address
                          id, 0, size, 0,
                          &i);
  t2 = get_time();
  if (err != VSRPC_ERR_NONE)
    fprintf(stderr, "vsrpc_remote_read() error %i\n", err);
  vsmutex_unlock(&obj.mtx_rpc);
  if (j == -VSRPC_ERR_NONE && i == size)
    printf("t2 - t1 = %f (vsrpc_remote_read)\n", t2 - t1);
  
  
  // check read data
  for (i = 0; i < size; i++)
  { 
    char c = (i & 0xFF) ^ 0xAC;
    if (buf[i] != c)
      fprintf(stderr, "bad data; error! (%i != %i)\n", buf[i], c);
  }

  printf("\npress ENTER to stop\n");
  fgetc(stdin);
  
  printf("vstcpc_stop()\n");
  vstcpc_stop(&obj);
  
  printf("\npress ENTER to exit\n");
  fgetc(stdin);
    
  return 0;
}
//----------------------------------------------------------------------------

/*** end of "test_vstcpc.c" file ***/
