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

#ifndef VSTCPC_H
#define VSTCPC_H
//----------------------------------------------------------------------------
#include "vsrpc.h"
#include "vsthread.h"
#include "vssync.h"
//----------------------------------------------------------------------------
// select default timeout [ms]
#define VSTCPC_SELECT_TIMEOUT (-1)

// include debuging output
//#define VSTCPC_DEBUG
//----------------------------------------------------------------------------
#ifdef VSTCPC_DEBUG
#  include <stdio.h>  // fprintf(), vfprintf()
#  include <string.h> // strerror()
#  if defined(__GNUC__)
#    define VSTCPC_DBG(fmt, arg...) fprintf(stderr, "VSTCPC: " fmt "\n", ## arg)
#  elif defined(VSWIN32)
#    define VSTCPC_DBG(fmt, ...) fprintf(stderr, "VSTCPC: " fmt "\n", __VA_ARGS__)
#  elif defined(__BORLANDC__)
#    include <stdarg.h> // va_list, va_start(), va_end()
void VSTCPC_DBG(const char *fmt, ...)
{
  va_list ap;
  va_start(ap, fmt);
  fprintf(stderr, "VSTCPC: ");
  vfprintf(stderr, fmt, ap);
  va_end(ap);
}
#  else
#    warning "unknown compiler"
#    define VSTCPC_DBG(fmt, arg...) fprintf(stderr, "VSTCPC: " fmt "\n", ## arg)
#  endif
#else
#  define VSTCPC_DBG(fmt, ...) // debug output off
#endif // VSTCPC_DEBUG
//----------------------------------------------------------------------------
// main server structure
typedef struct {
  int fd;            // file descriptor (client socket)
  vsrpc_t rpc;       // VSRPC object
  vsmutex_t mtx_rpc; // mutex for work with VSRPC object
  vsthread_t thread; // VSTCPC listen server thread
  int active;        // flag: 1-connected, 0-disconnected
#ifdef VSTHREAD_POOL
  vsthread_pool_t pool;
#endif // VSTHREAD_POOL
} vstcpc_t;
//----------------------------------------------------------------------------
#ifdef __cplusplus
extern "C" {
#endif // __plusplus
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
  int priority, int sched      // POSIX threads attributes
);
//----------------------------------------------------------------------------
// stop client (disconnect)
void vstcpc_stop(
  vstcpc_t *vstcpc // pointer to VSTCPC object
);
//----------------------------------------------------------------------------
#ifdef __cplusplus
}
#endif // __plusplus
//----------------------------------------------------------------------------
#endif // VSTCPC_H

/*** end of "vstcpc.h" file ***/

