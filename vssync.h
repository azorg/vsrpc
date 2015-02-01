/*
 * Very simple wrappers over syncronization primitives in POSIX way
 * Version: 0.8
 * File: "vssync.h"
 *
 * Copyright (c) 2008 
 *   shmigirilov@gmail.com, a.grinkov@gmail.com. All rights reserved.
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

// ------------------------------------------------------------------
#ifndef VSSYNC_H
#define VSSYNC_H
// ------------------------------------------------------------------
#if (defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__))
#  define VSWIN32
#endif
// ------------------------------------------------------------------
#ifdef VSWIN32
#  include <windows.h>
#else
#  include <semaphore.h> // sem_t
#  include <pthread.h>   // pthread_mutex_t
#endif
// ------------------------------------------------------------------
#ifdef __cplusplus
extern "C" {
#endif // __cplusplus
// ==================================================================
// 'like POSIX' semaphores implementation
typedef struct
{
#ifdef VSWIN32
    HANDLE win_sem;
#else
    sem_t pth_sem; 
#endif
}
vssem_t;
// ------------------------------------------------------------------
// create semaphore object
int vssem_init(vssem_t * sem, int pshared, unsigned int value);
// ------------------------------------------------------------------
// destroy semaphore object
int vssem_destroy(vssem_t * sem);
// ------------------------------------------------------------------
// blocking decrement semaphore
int vssem_wait(vssem_t * sem);
// ------------------------------------------------------------------
// non-blocking decrement semaphore
int vssem_trywait(vssem_t * sem);
// ------------------------------------------------------------------
// increment semaphore
int vssem_post(vssem_t * sem);
// ------------------------------------------------------------------
// get value of semaphore
int vssem_getvalue(vssem_t * sem, int *sval);
// ==================================================================
// 'like POSIX' mutexes implementation
typedef struct 
{
#ifdef VSWIN32
    HANDLE win_mtx;
#else
    pthread_mutex_t pth_mtx;
#endif
}
vsmutex_t;
// ------------------------------------------------------------------
// create mutex object
int vsmutex_init(vsmutex_t *mtx);
// ------------------------------------------------------------------
// destroy mutex object
int vsmutex_destroy(vsmutex_t *mtx);
// ------------------------------------------------------------------
// lock mutex
int vsmutex_lock(vsmutex_t *mtx);
// ------------------------------------------------------------------
// unlock mutex
int vsmutex_unlock(vsmutex_t *mtx);
// ==================================================================
#ifdef __cplusplus
}
#endif // __cplusplus
// ------------------------------------------------------------------
#endif // VSSYNC_H
// ------------------------------------------------------------------

/*** end of "vssync.h" file ***/

