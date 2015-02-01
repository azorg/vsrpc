/*
 * Very Simple FIFO (Very Simple Remote Procedure Call (VSRPC) project)
 * Version: 0.8 (development)
 * File: "vsfifo.h"
 *
 * Copyright (c) 2005, 2006, 2007, 2008
 *   a.grinkov@gmail.com. All rights reserved.
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
 * Last update: 2008.03.25
 */

#ifndef VSFIFO_H
#define VSFIFO_H
//----------------------------------------------------------------------------
#include "vssync.h" // vsset_t
//----------------------------------------------------------------------------
// add receive/send functions from/to pipe
#define VSFIFO_PIPE

// add functions to work with UNIX file descriptors
#define VSFIFO_UNIX

typedef struct vsfifo_ vsfifo_t;
struct vsfifo_ {
  char *data;       // pointer to FIFO buffer
  char *in;         // pointer for next input data
  char *out;        // pointer to next output data
  int count;        // data counter
  int size;         // FIFO size
  vssem_t read_sem; // block read wait semaphore
};
//----------------------------------------------------------------------------
#ifdef __cplusplus
extern "C" {
#endif // __plusplus
//----------------------------------------------------------------------------
// create new FIFO (constructor)
// on success return 0, else -1
int vsfifo_init(vsfifo_t *fifo, int size);

// print fifo status
void vsfifo_show(vsfifo_t *fifo);

// desrtroy FIFO (destructor)
void vsfifo_release(vsfifo_t *fifo);

// clear FIFO
void vsfifo_clear(vsfifo_t *fifo);

// return FIFO counter
int vsfifo_count(vsfifo_t *fifo);

// return FIFO free bytes
int vsfifo_free(vsfifo_t *fifo);

// write to FIFO from memory buffer
int vsfifo_write(vsfifo_t *fifo, const void *buf, int count);

// read from FIFO to memory buffer non-block
int vsfifo_read_nb(vsfifo_t *fifo, void *buf, int count);

// read (and not eject!) from FIFO to memory buffer non-block
int vsfifo_read_back(vsfifo_t *fifo, void *buf, int count);

// read from FIFO to memory buffer
int vsfifo_read(vsfifo_t *fifo, void *buf, int count);

// read from FIFO nowhere (to /dev/null)
int vsfifo_to_nowhere(vsfifo_t *fifo, int count);

#ifdef VSFIFO_PIPE
// write to FIFO from pipe
int vsfifo_from_pipe(
  vsfifo_t *fifo,
  int (*read_fn)(int fd, void *buf, int size), int fd,
  int count);

// read from FIFO to pipe
int vsfifo_to_pipe(
  vsfifo_t *fifo,
  int (*write_fn)(int fd, const void *buf, int size), int fd,
  int count);
#endif // VSFIFO_PIPE

#ifdef VSFIFO_UNIX
// read `size` bytes from stream `fd` to `buf` at once
int vsfifo_read_fd(int fd, void *buf, int size);

// write `size` bytes to stream `fd` from `buf` at once
int vsfifo_write_fd(int fd, const void *buf, int size);

// write to FIFO from UNIX pipe by fd
int vsfifo_from_unix_pipe(vsfifo_t *fifo, int fd, int count);

// read from FIFO to UNIX pipe by fd
int vsfifo_to_unix_pipe(vsfifo_t *fifo, int fd, int count);
#endif // VSFIFO_UNIX
//----------------------------------------------------------------------------
#ifdef __cplusplus
}
#endif // __plusplus
//----------------------------------------------------------------------------
#endif // VSFIFO_H

/*** end of "vsfifo.h" file ***/

