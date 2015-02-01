/*
 * Unit test of `vsfifo` component
 * File: "test_vsfifo.c"
 */

//----------------------------------------------------------------------------
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "vsthread.h"
#include "vsfifo.h"
//----------------------------------------------------------------------------
#define FIFO_SIZE 10
//----------------------------------------------------------------------------
vsfifo_t *fifo;
//----------------------------------------------------------------------------
void *thread()
{
  int buf;
  
  while (1)
  {
    int ret;
    
    ret = vsfifo_read(fifo, (void *)&buf, sizeof(buf));
    
    printf("vsfifo_read: ret=%i data=0x%X\n", ret, buf);    
  }
}
//----------------------------------------------------------------------------
int main()
{
  int ret;
  vsthread_t thr;

  printf("\npress ENTER to start\n");
  fgetc(stdin);

  fifo = (vsfifo_t *)malloc(sizeof(vsfifo_t));

  ret = vsfifo_init(fifo, FIFO_SIZE);
  if (ret != 0)
  {
    printf("vsfifo_init error\n");
    return 1;
  }
  
  vsfifo_clear(fifo);
  
  vsthread_create(10, SCHED_FIFO, &thr, thread, NULL);

  printf("press ENTER to write to fifo %i bytes\n", (int) sizeof(ret));
  fgetc(stdin);
  
  ret = 0xABCDEF;
  vsfifo_write(fifo, &ret, sizeof(ret));
  
  printf("press ENTER to stop\n");
  fgetc(stdin);
  
  vsfifo_release(fifo);
  
  printf("press ENTER to exit\n");
  fgetc(stdin);
    
  return 0;
}
//----------------------------------------------------------------------------

/*** end of "test_vsfifo.c" file ***/

