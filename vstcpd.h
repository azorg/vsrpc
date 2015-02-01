/*
 * Very Simple TCP/IP Daemon based on VSRPC and VSTCPS
 * Version: 0.8 (development)
 * File: "vstcpd.h"
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
 * Last update: 2008.09.22
 */

#ifndef VSTCPD_H
#define VSTCPD_H
//----------------------------------------------------------------------------
#include "vsrpc.h"
#include "vstcps.h"
//----------------------------------------------------------------------------
// select default timeout [ms]
#define VSTCPD_SELECT_TIMEOUT (-1)
//----------------------------------------------------------------------------
// include debuging output
//#define VSTCPD_DEBUG
//----------------------------------------------------------------------------
#if (defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__))
#  ifndef VSWIN32
#    define VSWIN32
#  endif
#endif
//----------------------------------------------------------------------------
#ifdef VSTCPD_DEBUG
#  include <stdio.h> // fprintf()
#  ifdef VSWIN32
#    define VSTCPD_DBG(fmt, ...) fprintf(stderr, "VSTCPD: " fmt "\n", __VA_ARGS__)
#  elif defined(__BORLANDC__)
#    include <stdarg.h> // va_list, va_start(), va_end()
#    include <stdio.h>  // vfprintf()
void VSTCPD_DBG(const char *fmt, ...)
{
  va_list ap;
  va_start(ap, fmt);
  fprintf(stderr, "VSTCPD: ");
  vfprintf(stderr, fmt, ap);
  va_end(ap);
}
#  else
#    define VSTCPD_DBG(fmt, arg...) fprintf(stderr, "VSTCPD: " fmt "\n", ## arg)
#  endif
#else
#  define VSTCPD_DBG(fmt, ...)
#endif
//----------------------------------------------------------------------------
typedef struct vstcpd_ vstcpd_t;
typedef struct vstcpd_client_ vstcpd_client_t;
//----------------------------------------------------------------------------
// main server structure
struct vstcpd_ {
  vstcps_t tcps;          // TCP server structure
  vsrpc_func_t *func;     // VSRPC user-defined table or NULL
  char** (*def_func)(     // pointer to default function or NULL
    vsrpc_t *rpc,             // VSRPC structure
    int argc,                 // number of arguments
    char * const argv[]);     // arguments (strings)
  int perm;               // VSRPC permissions
  void *context;          // pointer to optional server data or NULL

  void (*on_connect)(     // on connect callback function
    void **client_context,    // client context
    void *server_context,     // server context
    int count);               // clients count
  
  void (*on_disconnect)(  // on disconnect callback function
    void *client_context);    // client context
};
//----------------------------------------------------------------------------
// structure for each client
struct vstcpd_client_ {
  int fd;            // socket
  vsmutex_t mtx_fd;  // mutex for using fd
  unsigned ipaddr;   // client IPv4 address
  void *context;     // client context
  vstcpd_t *server;  // pointer to server structure
  vsrpc_t rpc;       // VSRPC structure
};
//----------------------------------------------------------------------------
#ifdef __cplusplus
extern "C" {
#endif // __plusplus
//----------------------------------------------------------------------------
// start server
// (return error code of vstcps_start())
int vstcpd_start(
  vstcpd_t *server,            // pointer to VSTCPD object
  vsrpc_func_t *rpc_functions, // table of VSRPC functions or NULL
  char** (*rpc_def_func)(      // pointer to default function or NULL
    vsrpc_t *rpc,                  // VSRPC structure
    int argc,                      // number of arguments
    char * const argv[]),          // arguments (strings)
  int rpc_permissions,         // VSRPC permissions
  const char* host,            // server listen address
  int port,                    // server listen TCP port
  int max_clients,             // max clients
  void *server_context,        // pointer to optional server data or NULL

  void (*on_connect)(          // start callback function or NULL
    void **client_context,         // client context
    void *server_context,          // server context
    int count),                    // clients count
  
  void (*on_disconnect)(       // terminate callback function or NULL
    void *client_context),         // client context

  int priority, int sched      // POSIX threads attributes
);
//----------------------------------------------------------------------------
// stop server
void vstcpd_stop(
  vstcpd_t *server // pointer to VSTCPD object
);
//----------------------------------------------------------------------------
// broadcast procedures call on all clients
int vstcpd_broadcast(
  vstcpd_t *server,   // pointer to VSTCPD object
  void *exclude,      // exclude this client (by client context)
  char * const argv[] // function name and arguments
);
//----------------------------------------------------------------------------
// broadcast procedures call on all clients with "friendly" interface
// list: s-string, i-integer, f-float, d-double
int vstcpd_broadcast_ex(
  vstcpd_t *server, // pointer to VSTCPD object
  void *exclude,    // exclude this client (by client context)
  const char *list, // list of argumets type (begin with 's' - function name)
  ...               // function name and arguments
);
//----------------------------------------------------------------------------
#ifdef __cplusplus
}
#endif // __plusplus
//----------------------------------------------------------------------------
#endif // VSTCPD_H

/*** end of "vstcpd.h" file ***/

