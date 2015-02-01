/*
 * Very Simple Thread Implementation (VSRPC project)
 * Version: 0.8 (development)
 * File: "vsthread.h"
 *
 * Copyright (c) 2005, 2006, 2007, 2008,
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
 * Last update: 2008.09.16
 */

#ifndef VSTHREAD_H
#define VSTHREAD_H
//----------------------------------------------------------------------------
#include "vssync.h"
//----------------------------------------------------------------------------
// version for Linux
//#define VSTHREAD_LINUX

// version for Linux with "Real Time threads"
//#define VSTHREAD_LINUX_RT

// version for eCos
//#define VSTHREAD_ECOS

// version for OS2000 (WxWorks clone)
//#define VSTHREAD_OS2000

// include debuging output
//#define VSTHREAD_DEBUG
//----------------------------------------------------------------------------
#ifdef VSTHREAD_ECOS

// create threads pool
#  define VSTHREAD_POOL

// threads stack size [bytes] (if use pool)
// may be not defined
#  ifndef VSTHREAD_STACKSIZE
#    define VSTHREAD_STACKSIZE 8192
#  endif

#endif // VSTHREAD_ECOS
//----------------------------------------------------------------------------
#if (defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__))
#  ifndef VSWIN32
#    define VSWIN32
#  endif
#  undef VSTHREAD_POOL
#endif
//----------------------------------------------------------------------------
#ifdef VSTHREAD_DEBUG
#  include <stdio.h>  // fprintf(), vfprintf()
#  include <string.h> // strerror()
#  if defined(__GNUC__)
#    define VSTHREAD_DBG(fmt, arg...) fprintf(stderr, "VSTHREAD: " fmt "\n", ## arg)
#  elif defined(VSWIN32)
#    define VSTHREAD_DBG(fmt, ...) fprintf(stderr, "VSTHREAD: " fmt "\n", __VA_ARGS__)
#  elif defined(__BORLANDC__)
#    include <stdarg.h> // va_list, va_start(), va_end()
void VSTHREAD_DBG(const char *fmt, ...)
{
  va_list ap;
  va_start(ap, fmt);
  fprintf(stderr, "VSTHREAD: ");
  vfprintf(stderr, fmt, ap);
  va_end(ap);
}
#  else
#    warning "unknown compiler"
#    define VSTHREAD_DBG(fmt, arg...) fprintf(stderr, "VSTHREAD: " fmt "\n", ## arg)
#  endif
#else
#  define VSTHREAD_DBG(fmt, ...) // debug output off
#endif // VSTHREADS_DEBUG
//----------------------------------------------------------------------------
#ifdef VSWIN32
// SCHED_* constants unused under Windows
# ifndef SCHED_OTHER
#   define SCHED_OTHER    1
# endif
# ifndef SCHED_RR
#   define SCHED_RR       2
# endif
# ifndef SCHED_FIFO
#   define SCHED_FIFO     3
# endif
# ifndef SCHED_DEADLINE
#   define SCHED_DEADLINE 4
# endif
#endif // VSWIN32
//----------------------------------------------------------------------------
//
// Structures
//

// Very Simple Thread handle
typedef struct {
#ifdef VSWIN32
  HANDLE handle;    // handle of WIN32 thread
  DWORD  id;        // ID of thread
#else // VSWIN32
  pthread_t pthread; // handle of POSIX thread
#endif
} vsthread_t;

#ifdef VSTHREAD_POOL
typedef struct {
  pthread_t pthread;    // handle of POSIX thread
  vssem_t sem_start;      // semaphore to start thread
  vssem_t sem_free;       // semaphore set if thread free
  vssem_t sem_finish;     // semaphore set if thread finish
  vssem_t sem_kill;       // semaphore to kill thread
  void *(*func)(void*); // thread function
  void *arg;            // argument for thread function
  void *ret;            // return value of thread function
#ifdef VSTHREAD_STACKSIZE
  char stack[VSTHREAD_STACKSIZE];
#endif // VSTHREAD_STACKSIZE
} vsthread_record_t;

typedef struct {
  vssem_t sem_list;          // semaphore for modify pool list
  int size;                // size of list
  vsthread_record_t *list; // pool list
  int priority; int sched; // POSIX threads attributes
} vsthread_pool_t;
#endif // VSTHREAD_POOL

//----------------------------------------------------------------------------
//
// Functions
//

#ifdef __cplusplus
extern "C" {
#endif // __plusplus

#ifdef VSTHREAD_POOL

// create treads pool (start all threads before)
int vsthread_pool_init(vsthread_pool_t *pool, int poolsize,
                       int priority, int sched);

// destroy pool of all startted thread
void vsthread_pool_destroy(vsthread_pool_t *pool);

// create thread (wakeup thread in pool)
int vsthread_create(
  vsthread_pool_t *pool,
  vsthread_t *thread, void *(*func)(void *), void *arg);

// cancel thread in pool
int vsthread_cancel(vsthread_pool_t *pool, vsthread_t thread);

// join to thread in pool
int vsthread_join(
  vsthread_pool_t *pool,
  vsthread_t thread, void **thread_return);

#else // !VSTHREAD_POOL

// create POSIX thread
int vsthread_create(
  int priority, int sched,
  vsthread_t *thread, void *(*func)(void *), void *arg);

// cancel POSIX thread
int vsthread_cancel(vsthread_t thread);

// join to POSIX thread
int vsthread_join(vsthread_t thread, void **thread_return);

#endif // VSTHREAD_POOL

//----------------------------------------------------------------------------
#ifdef __cplusplus
}
#endif // __plusplus
//----------------------------------------------------------------------------
#endif // VSTHREAD_H

/*** end of "vsthread.h" file ***/

