/*
 * Very Simple TCP/IP Server based on threads (without VSRPC)
 * Version: 0.8 (development)
 * File: "vstcps.c"
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

//----------------------------------------------------------------------------
#include "socklib.h"
#include "vstcps.h"
#include <stdarg.h>  // va_list, va_start(), va_end()
#include <stdlib.h>  // malloc(), free()
#include <string.h>  // strlen()
//----------------------------------------------------------------------------
// listen 1 client (1 thread per 1 client)
static void *vstcps_exchange_thread(void *arg)
{
  vstcps_client_t *client = (vstcps_client_t*) arg;
  vstcps_t *server = client->server;

  VSTCPS_DBG("vstcps_exchange_thread() start");
  
  if (server->on_exchange != NULL)
    server->on_exchange(client->fd, client->ipaddr, 
                        client->context, server->context);
  
  VSTCPS_DBG("server->on_exchanged() finish");

  // disconnect
  vsmutex_lock(&server->mtx_list);
  if (--server->count == 0)
  {
    vssem_post(&server->sem_zero);
    server->first = server->last = NULL;
  }
  else
  {
    if (client->prev != NULL)
      client->prev->next = client->next;
    else
      server->first = client->next;

    if (client->next != NULL)
      client->next->prev = client->prev;
    else
      server->last = client->prev;
  }

  if (server->on_disconnect != NULL)
    server->on_disconnect(client->context);
  
  sl_disconnect(client->fd);

  VSTCPS_DBG("client disconnected: IP=%s, count=%d",
             sl_inet_ntoa(client->ipaddr), server->count);
  
  free((void*) client); // free memory
  
  vsmutex_unlock(&server->mtx_list);
  
  VSTCPS_DBG("vstcps_exchange_thread() finish");
  return NULL;
}
//----------------------------------------------------------------------------
// server listen TCP/IP port thread
static void *vstcps_listen_port_thread(void *arg)
{
  vstcps_t *server = (vstcps_t*) arg;
  vstcps_client_t *client;
  int fd, retv;
  unsigned ipaddr;

  VSTCPS_DBG("vstcps_listen_port_thread() start");

  while (1)
  {
    // wait for client connection
    fd = sl_accept(server->sock, &ipaddr);
    vsmutex_lock(&server->mtx_list);
    if (fd < 0)
    {
      VSTCPS_DBG("Ooops; sl_accept() return %i < 0", fd);
      break;
    }

    // check connections number
    if (server->count >= server->max_clients)
    { // too many connections - disconnect
      vsmutex_unlock(&server->mtx_list);
      sl_disconnect(fd);
      continue;
    }
    
    // new client connected
    // allocate memory for connected client
    client = (vstcps_client_t*) malloc(sizeof(vstcps_client_t));
    if (client == NULL)
    {
      VSTCPS_DBG("Ooops; malloc() return NULL");
      break;
    }

    // store in client structure
    client->server = server; // pointer to server structure
    client->ipaddr = ipaddr; // client IPv4 address
    client->fd = fd;         // file descriptor (socket)
    client->context = NULL;  // client context

    // add client structure to the end of list
    client->prev = server->last;
    client->next = NULL;
    if (server->count == 0)
    {
      vssem_wait(&server->sem_zero);
      server->first = client;
    }
    else
      server->last->next = client;
    server->last = client;
    server->count++;

    VSTCPS_DBG("client connected: IP=%s, count=%d",
               sl_inet_ntoa(ipaddr),
               server->count);

    if (server->on_connect != NULL)
      server->on_connect(&client->context, server->context, server->count);
    
    // create thread and start on_exchange()
    retv = vsthread_create(
#ifdef VSTHREAD_POOL
      &server->pool,
#else
      server->priority, server->sched,
#endif // !VSTHREAD_POOL
      &client->thread, vstcps_exchange_thread, (void*) client);
    if (retv != 0)
    {
      VSTCPS_DBG("Ooops; can't create thread");
      break;
    }

    vsmutex_unlock(&server->mtx_list);
  } // while (1)
  
  vsmutex_unlock(&server->mtx_list);

  VSTCPS_DBG("vstcpd_listen_port_thread() finish");
  return NULL;
}
//----------------------------------------------------------------------------
// start server
// (return error code)
int vstcps_start(
  vstcps_t *server,        // pointer to VSTCPS object
  const char *host,        // server listen address
  int port,                // server listen TCP port
  int max_clients,         // max clients
  void *server_context,    // pointer to optional server context or NULL
  
  void (*on_connect)(      // on connect callback function
    void **client_context,     // client context
    void *server_context,      // server context
    int count),                // clients count

  void (*on_exchange)(     // on exchange callback function
    int fd,                    // socket
    unsigned ipaddr,           // client IPv4 address
    void *client_context,      // client context
    void *server_context),     // server context
  
  void (*on_disconnect)(   // on disconnect callback function
    void *client_context),     // client context
  
  int priority, int sched) // POSIX threads attributes
{
  int retv;

  // set counter of clients to zero
  // and init server structure
  server->count = 0;
  server->first = server->last = NULL;
  server->max_clients = max_clients;
  server->context = server_context;
  server->on_connect    = on_connect;
  server->on_exchange   = on_exchange;
  server->on_disconnect = on_disconnect;

  // make server socket
  retv = sl_make_server_socket_ex(host, port, max_clients);
  if (retv < 0)
  {
    VSTCPS_DBG("Ooops; can't create server socket %s:%i (%s)",
               host, port, sl_error_str(retv));
    return VSTCPS_ERR_SOCKET;
  }
  server->sock = retv;

  // create mutex
  if (vsmutex_init(&server->mtx_list) < 0)
  {
    VSTCPS_DBG("Ooops; can't init mutex");
    return VSTCPS_ERR_MUTEX;
  }

  // create semaphore
  if (vssem_init(&server->sem_zero, 0, 1) < 0)
  {
    VSTCPS_DBG("Ooops; can't init semaphore");
    return VSTCPS_ERR_SEM;
  }

#ifdef VSTHREAD_POOL
  if (vsthread_pool_init(&server->pool, max_clients + 1,
      priority, sched) != 0)
  {
    VSTCPS_DBG("Ooops; can't create pool of threads");
    return VSTCPS_ERR_POOL;
  }
#endif // VSTHREAD_POOL

  // create server listen port thread
  retv = vsthread_create(
#ifdef VSTHREAD_POOL
    &server->pool,
#else
    server->priority = priority, server->sched = sched,
#endif // !VSTHREAD_POOL
    &server->thread, vstcps_listen_port_thread, (void*) server);
  if (retv != 0)
  {
    VSTCPS_DBG("Ooops; can't create thread");
    return VSTCPS_ERR_THREAD;
  }

  VSTCPS_DBG("TCP server %s:%i started by 'vstcps_start' function",
             host, port);
  return 0; // success
}
//----------------------------------------------------------------------------
// stop server
void vstcps_stop(vstcps_t *server)
{
  vstcps_client_t *client;
  vsmutex_lock(&server->mtx_list);

  // close server socket and wait server thread
  sl_disconnect(server->sock);

  // close all clients sockets
  client = server->first;
  while (client != NULL)
  {
    sl_disconnect(client->fd);
    client = client->next;
  }

#if 0 // FIXME
  vsthread_join(
#ifdef VSTHREAD_POOL
    &server->pool,
#endif // VSTHREAD_POOL
    server->thread, NULL);
#endif

  vsmutex_unlock(&server->mtx_list);

  // wait last client thread
  vssem_wait(&server->sem_zero);

#ifdef VSTHREAD_POOL
  // destroy pool of threads
  vsthread_pool_destroy(&server->pool);
#endif // VSTHREAD_POOL

  // destroy semaphores
  vssem_destroy(&server->sem_zero);
  vsmutex_destroy(&server->mtx_list);
}
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
    int client_count))       // client count
{
  int i = 0;
  vstcps_client_t *client;
  vsmutex_lock(&server->mtx_list);
  client = server->first;
  while (client != NULL)
  {
    on_foreach(client->fd, client->ipaddr, 
               client->context, server->context, foreach_context,
               i, server->count);
    i++;
    client = client->next;
  }
  vsmutex_unlock(&server->mtx_list);
  return i;
}
//----------------------------------------------------------------------------

/*** end of "vstcps.c" file ***/

