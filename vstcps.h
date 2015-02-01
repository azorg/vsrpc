/*
 * Very Simple TCP/IP Server based on threads (without VSRPC)
 * Version: 0.8 (development)
 * File: "vstcps.h"
 *
 * Copyright (c) 2005, 2006, 2007, 2008
 *   a.grinkov@gmail.com, shmigirilov@gmail.com. All rights reserved.
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

#ifndef VSTCPS_H
#define VSTCPS_H
//----------------------------------------------------------------------------
#include "vssync.h"
#include "vsthread.h"
//----------------------------------------------------------------------------
// include debuging output
//#define VSTCPS_DEBUG
//----------------------------------------------------------------------------
#if (defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__))
#  ifndef VSWIN32
#    define VSWIN32
#  endif
#endif
//----------------------------------------------------------------------------
#ifdef VSTCPS_DEBUG
#  include <stdio.h> // fprintf()
#  ifdef VSWIN32
#    define VSTCPS_DBG(fmt, ...) fprintf(stderr, "VSTCPS: " fmt "\n", __VA_ARGS__)
#  elif defined(__BORLANDC__)
#    include <stdarg.h> // va_list, va_start(), va_end()
#    include <stdio.h>  // vfprintf()
void VSTCPS_DBG(const char *fmt, ...)
{
  va_list ap;
  va_start(ap, fmt);
  fprintf(stderr, "VSTCPS: ");
  vfprintf(stderr, fmt, ap);
  va_end(ap);
}
#  else
#    define VSTCPS_DBG(fmt, arg...) fprintf(stderr, "VSTCPS: " fmt "\n", ## arg)
#  endif
#else
#  define VSTCPS_DBG(fmt, ...)
#endif
//----------------------------------------------------------------------------
// error codes of vstcps_start()
#define VSTCPS_ERR_NONE    0 // no error all success
#define VSTCPS_ERR_SOCKET -1 // can't create TCP server socket
#define VSTCPS_ERR_MUTEX  -2 // can't init mutex
#define VSTCPS_ERR_SEM    -3 // can't init semaphore
#define VSTCPS_ERR_POOL   -4 // can't create pool of threads
#define VSTCPS_ERR_THREAD -5 // can't create thread
//----------------------------------------------------------------------------
typedef struct vstcps_ vstcps_t;
typedef struct vstcps_client_ vstcps_client_t;
//----------------------------------------------------------------------------
// main server structure
struct vstcps_ {
  int max_clients;        // maximum clients may connected
  int sock;               // server socket ID
  int count;              // counter of connected clients
  vsthread_t thread;      // VSTCPD listen port thread
  vssem_t sem_zero;       // signal of zero clients counter
  vsmutex_t mtx_list;     // mutex for modify clients list
  vstcps_client_t *first; // structure of first connected client
  vstcps_client_t *last;  // structure of last connected client
  void *context;          // pointer to optional server context or NULL
  
  void (*on_connect)(     // on connect callback function
    void **client_context,    // client context
    void *server_context,     // server context
    int count);               // clients count
  
  void (*on_exchange)(    // on exchange callback function
    int fd,                   // socket
    unsigned ipaddr,          // client IPv4 address
    void *client_context,     // client context
    void *server_context);    // server context
  
  void (*on_disconnect)(  // on disconnect callback function
    void *client_context);    // client context

#ifdef VSTHREAD_POOL
  vsthread_pool_t pool;
#else
  int priority; int sched; // POSIX threads attributes
#endif // !VSTHREAD_POOL
};
//----------------------------------------------------------------------------
// structure for each connected client
struct vstcps_client_ {
  vstcps_t *server;      // pointer to VSTCPS (server) structure
  int fd;                // file descriptor
  void *context;         // client context
  unsigned ipaddr;       // client IPv4 address
  vsthread_t thread;     // connection thread
  vstcps_client_t *prev; // previous structure or NULL for first
  vstcps_client_t *next; // next structure or NULL for last
};
//----------------------------------------------------------------------------
#ifdef __cplusplus
extern "C" {
#endif // __plusplus
//----------------------------------------------------------------------------
// start server
// (return error code)
int vstcps_start(
  vstcps_t *server,       // pointer to VSTCPS object
  const char *host,       // server listen address
  int port,               // server listen TCP port
  int max_clients,        // max clients
  void *server_context,   // pointer to optional server context or NULL
  
  void (*on_connect)(     // on connect callback function
    void **client_context,    // client context
    void *server_context,     // server context
    int count),               // clients count
  
  void (*on_exchange)(    // on exchange callback function
    int fd,                   // socket
    unsigned ipaddr,          // client IPv4 address
    void *client_context,     // client context
    void *server_context),    // server context
  
  void (*on_disconnect)(  // on disconnect callback function
    void *client_context),    // client context
  
  int priority, int sched // POSIX threads attributes
);
//----------------------------------------------------------------------------
// stop server
void vstcps_stop(vstcps_t *server);
//----------------------------------------------------------------------------
// exchange for each client socket
// (return number of connected clients)
int vstcps_foreach(
  vstcps_t *server,       // pointer to VSTCPS object
  void *foreach_context,  // pointer to optional context or NULL
  
  void (*on_foreach)(     // on foreach callback function
    int fd,                  // socket
    unsigned ipaddr,         // client IPv4 address
    void *client_context,    // client context
    void *server_context,    // server context
    void *foreach_context,   // optional context
    int client_index,        // client index (< client_count)
    int client_count)        // client count
);
//----------------------------------------------------------------------------
#ifdef __cplusplus
}
#endif // __plusplus
//----------------------------------------------------------------------------
#endif // VSTCPS_H

/*** end of "vstcps.h" file ***/

