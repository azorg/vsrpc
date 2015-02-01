/*
 * Very Simple TCP Client based on VSRPC and POSIX threads
 * Version: 0.8 (development)
 * File: "vstcpc.h"
 *
 * Copyright (c) 2005, 2006, 2007, 2008
 *   a.grinkov@gmail.com, All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the names of the copyright holders may be used to endorse 
 *       or promote products derived from this software without specific prior 
 *       written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY  ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDER BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. 
 *
 * Last update: 2008.09.24
 */

//----------------------------------------------------------------------------
#include "socklib.h"
#include "vstcpc.h"
#include <stdlib.h> // malloc(), free()
//----------------------------------------------------------------------------
// listen server thread
static void *vstcpc_thread(void *arg)
{
  vstcpc_t *vstcpc = (vstcpc_t*) arg;
  int retv;
  
  while (1)
  {
    // wait request from client side
    while ((retv = sl_select(vstcpc->fd, VSTCPC_SELECT_TIMEOUT)) == 0);
    
    vsmutex_lock(&vstcpc->mtx_rpc);
    if (retv < 0)
    { // error of connection
      VSTCPC_DBG("thread 'vstcpc_thread' finish; disconnected");
      break;
    }
    
    // allow run 1 procedure on client
    retv = vsrpc_run(&vstcpc->rpc); // allow run 1 procedure
    
    // check VSRPC return value
    if (retv == VSRPC_ERR_EXIT || retv == VSRPC_ERR_EOP)
      break; // server say "goodbye" or close connection
    
    if (retv != VSRPC_ERR_NONE &&
        retv != VSRPC_ERR_EMPTY &&
        retv != VSRPC_ERR_RET)
    { // close connection
      VSTCPC_DBG("Ooops; disconnected by VSRPC error (retv=%i)", retv);
      break; // VSRPC error
    }
  
    vsmutex_unlock(&vstcpc->mtx_rpc);
  } // while (1)
  
#ifdef VSWIN32
  closesocket(vstcpc->fd);
#else // VSWIN32
  shutdown(vstcpc->fd, SHUT_RDWR);
  close(vstcpc->fd); // close socket
#endif // VSWIN32
  vstcpc->active = 0;
  vsmutex_unlock(&vstcpc->mtx_rpc);
  
  return NULL;
}
//----------------------------------------------------------------------------
// start client (connect to server)
int vstcpc_start(
  vstcpc_t *vstcpc,            // pointer to VSTCPC object
  vsrpc_func_t *rpc_functions, // table of VSRPC functions or NULL
  char** (*rpc_def_func)(      // pointer to default function or NULL
    vsrpc_t *rpc,                // VSRPC structure
    int argc,                    // number of arguments
    char * const argv[]),        // arguments (strings)
  int rpc_permissions,         // VSRPC permissions
  const char* host,            // server address
  int port,                    // server TCP port
  int priority, int sched)     // POSIX threads attributes
{
  int retv;
  vstcpc->active = 0;
  
  // connect to server (make client socket)
  retv = sl_connect_to_server(host, port);
  VSTCPC_DBG("sl_connect_to_server() finish (retv=%i)", retv);

  if (retv < 0)
  {
    VSTCPC_DBG("Ooops; can't connect to TCP/IP server %s:%i", host, port);
    return -1;
  }
  
  // init VSRPC structure
  vstcpc->fd = retv;
  retv = vsrpc_init(
    &vstcpc->rpc,
    rpc_functions,         // user functions or NULL
    rpc_def_func,          // default function or NULL
    rpc_permissions,       // permissions
    (void*) vstcpc,        // context
    retv, retv,            // read and write file descriptor (socket)
    sl_read,               // read() function
    sl_write,              // write() function
    sl_select,             // select() function
    (void (*)(int)) NULL); // flush() function
  
  if (retv != VSRPC_ERR_NONE)
  {
    VSTCPC_DBG("Ooops; can't init VSRPC (retv=%i)", retv);
    return -2;
  }
  
  // create semaphore
  if (vsmutex_init(&vstcpc->mtx_rpc))
  {
    VSTCPC_DBG("Ooops; can't init mutex");
    return -3;
  }
  
#ifdef VSTHREAD_POOL
  if (vsthread_pool_init(&vstcpc->pool, 1, priority, sched) != 0) //!!!
  {
    VSTCPC_DBG("Ooops; can't create pool of threads");
    return -4;
  }
#endif // VSTHREAD_POOL
  
  // create thread
  retv = vsthread_create(
#ifdef VSTHREAD_POOL
    &vstcpc->pool,
#else
    priority, sched,
#endif // !VSTHREAD_POOL
    &vstcpc->thread, vstcpc_thread, (void*) vstcpc);
  if (retv != 0)
  {
    VSTCPC_DBG("Ooops; can't create thread");
    return -5;
  }

  vstcpc->active = 1;
  return 0; // success
}
//----------------------------------------------------------------------------
// stop client (disconnect)
void vstcpc_stop(
  vstcpc_t *vstcpc)  // pointer to VSTCPC object
{
  // finish VSRPC connection and wait thread
  vsmutex_lock(&vstcpc->mtx_rpc);
  vsrpc_remote_exit(&vstcpc->rpc, 0); // say to server "exit 0"
  vsmutex_unlock(&vstcpc->mtx_rpc);
  vsthread_join(
#ifdef VSTHREAD_POOL
    &vstcpc->pool,
#endif // VSTHREAD_POOL
    vstcpc->thread, NULL);
  
#ifdef VSTHREAD_POOL
  // destroy pool of threads
  vsthread_pool_destroy(&vstcpc->pool);
#endif // VSTHREAD_POOL
  
  // destroy VSRPC
  vsrpc_release(&vstcpc->rpc);

  // destroy semaphore
  vsmutex_destroy(&vstcpc->mtx_rpc);
}
//----------------------------------------------------------------------------

/*** end of "vstcpc.c" file ***/

