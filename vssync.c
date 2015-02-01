/*
 * Very simple wrappers over syncronization primitives in POSIX way
 * Version: 0.8
 * File: "vssync.c"
 *
 * Copyright (c) 2008 
 *   shmigirilov@gmail.com. All rights reserved.
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

// ---------------------------------------------------------------------------
#include <stdlib.h>
#include "vssync.h"

#ifdef VSSYNC_WIN32
# include <windows.h>
#else // VSSYNC_WIN32
# include <semaphore.h>
#endif // VSSYNC_WIN32
// ---------------------------------------------------------------------------
// create semaphore object
int vssem_init(vssem_t * sem, int pshared, unsigned int value)
{
#ifdef VSSYNC_WIN32
    pshared = pshared; // unused

    sem->win_sem = CreateSemaphore(
                                  NULL,     // default security attributes
                                  value,    // initial value
                                  1000000,  // maximum value
                                  NULL      // object name
                                 );

    if (NULL == sem->win_sem)
        return -1;

    return 0;
#else // VSSYNC_WIN32
    return sem_init(&(sem->pth_sem), pshared, value);
#endif // VSSYNC_WIN32     
}
// ---------------------------------------------------------------------------
// destroy semaphore object
int vssem_destroy(vssem_t * sem)
{
#ifdef VSSYNC_WIN32
    BOOL ret;

    ret = CloseHandle(sem->win_sem);

    if (!ret)
        return -1;
    else
        return 0;
#else // VSSYNC_WIN32
    return sem_destroy(&(sem->pth_sem));
#endif // VSSYNC_WIN32     
}
// ---------------------------------------------------------------------------
// blocking decrement semaphore
int vssem_wait(vssem_t * sem)
{
#ifdef VSSYNC_WIN32
    int ret;

    // decrease count by 1
    ret = WaitForSingleObject(sem->win_sem, INFINITE);

    if (ret == WAIT_FAILED)
        return -1;

    return 0;
#else // VSSYNC_WIN32
    return sem_wait(&(sem->pth_sem));
#endif // VSSYNC_WIN32     
}
// ---------------------------------------------------------------------------
// non-blocking decrement semaphore
int vssem_trywait(vssem_t * sem)
{
#ifdef VSSYNC_WIN32
    int ret;

    // decrease count by 1
    ret = WaitForSingleObject(sem->win_sem, 0L);

    if (ret == WAIT_TIMEOUT)
        return -3;  // EAGAIN
    else if (ret == WAIT_FAILED)
        return -1;

    return 0;
#else // VSSYNC_WIN32
    return sem_trywait(&(sem->pth_sem));
#endif // VSSYNC_WIN32     
}
// ---------------------------------------------------------------------------
// increment semaphore
int vssem_post(vssem_t * sem)
{
#ifdef VSSYNC_WIN32
    BOOL ret;

    // increase count by 1
    ret = ReleaseSemaphore(sem->win_sem, 1, NULL);
    if (!ret)
        return -1;

    return 0;
#else // VSSYNC_WIN32
    return sem_post(&(sem->pth_sem));
#endif // VSSYNC_WIN32     
}
// ---------------------------------------------------------------------------
#ifdef VSSYNC_WIN32
typedef struct _SEMAINFO {
  UINT    Count;    // current semaphore count
  UINT    Limit;    // max semaphore count
} SEMAINFO, *PSEMAINFO;
// ---------------------------------------------------------------------------
int WINAPI NtQuerySemaphore(
  HANDLE Handle,
  int InfoClass,
  PSEMAINFO SemaInfo,
  int InfoSize,
  int *RetLen
);
#endif // VSSYNC_WIN32     
// ---------------------------------------------------------------------------
// get value of semaphore
int vssem_getvalue(vssem_t * sem, int *sval)
{
#ifdef VSSYNC_WIN32
  int ret;
  SEMAINFO info;
  int len;

  ret = NtQuerySemaphore(
          sem->win_sem,
          0,
          &info,
          sizeof(info),
          &len
        );

  if( ret < 0 )
  {
    *sval = -1;
    return ret;
  }
  else
  {
    *sval = info.Count;
    return 0;
  }
#else // VSSYNC_WIN32
  return sem_getvalue(&(sem->pth_sem), sval);
#endif // VSSYNC_WIN32     
}
// ---------------------------------------------------------------------------
// create mutex object
int vsmutex_init(vsmutex_t *mtx)
{
#ifdef VSSYNC_WIN32
    mtx->win_mtx = CreateMutex(
                               NULL,     // default security attributes
                               FALSE,    // Initial value
                               NULL      // object name
                              );

    if (NULL == mtx->win_mtx)
        return -1;

    return 0;
#else // VSSYNC_WIN32
    return pthread_mutex_init(&(mtx->pth_mtx), NULL);
#endif // VSSYNC_WIN32
}
// ---------------------------------------------------------------------------
// destroy mutex object
int vsmutex_destroy(vsmutex_t *mtx)
{
#ifdef VSSYNC_WIN32
    BOOL ret;

    ret = CloseHandle(mtx->win_mtx);

    if (!ret)
        return -1;
    else
        return 0;
#else // VSSYNC_WIN32
    return pthread_mutex_destroy(&(mtx->pth_mtx));
#endif // VSSYNC_WIN32
}
// ---------------------------------------------------------------------------
// lock mutex
int vsmutex_lock(vsmutex_t *mtx)
{
#ifdef VSSYNC_WIN32
    int ret;

    // wait until mutex released
    ret = WaitForSingleObject(mtx->win_mtx, INFINITE);
    if (ret == WAIT_FAILED)
        return -1;

    return 0;
#else // VSSYNC_WIN32
    return pthread_mutex_lock(&(mtx->pth_mtx));
#endif // VSSYNC_WIN32
}
// ---------------------------------------------------------------------------
// unlock mutex
int vsmutex_unlock(vsmutex_t *mtx)
{
#ifdef VSSYNC_WIN32
    BOOL ret;

    ret = ReleaseMutex(mtx->win_mtx);
    if (!ret)
        return -1;

    return 0;
#else // VSSYNC_WIN32
    return pthread_mutex_unlock(&(mtx->pth_mtx));
#endif // VSSYNC_WIN32
}
// ---------------------------------------------------------------------------
// EOF
