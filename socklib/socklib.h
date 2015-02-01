/*
 * Simple wrappers to work with UNIX-like TCP/IP sockets
 * Version: 0.11
 * File: "socklib.h"
 *
 * Copyright (c) 2005, 2006, 2007, 2008, 2009
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
 * Last update: 2009.08.21
 */

#ifndef SOCKLIB_H
#define SOCKLIB_H
// ---------------------------------------------------------------------------
#if (defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__))
#  define SL_WIN32
#endif
// ---------------------------------------------------------------------------
// use poll() nor select() in sl_select()
//#define SL_USE_POLL
// ---------------------------------------------------------------------------

#ifdef SL_WIN32
#  include <windows.h>
#  include <winsock.h>
#else // SL_WIN32
#  include <errno.h>      // errno
#  include <sys/time.h>   // select()
#  include <sys/types.h>  // socket(), setsockopt(), bind()
#  include <sys/socket.h> // socket(), setsockopt(), bind(), listen(),
                          // inet_aton(), accept()
#  include <unistd.h>     // gethostname(), read(), write(), {select()}
#  include <netdb.h>      // gethostbyname()
#  include <arpa/inet.h>  // inet_aton()
#  include <netinet/in.h> // inet_aton()
#endif // SL_WIN32

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus
// ---------------------------------------------------------------------------
// missing const
#ifdef SL_WIN32
# define SHUT_RDWR 2
#endif // SL_WIN32
// ---------------------------------------------------------------------------
// error codes
#define SL_SUCCESS               0
#define SL_ERROR_INIT           -1
#define SL_ERROR_SOCKET         -2
#define SL_ERROR_ADDR           -3
#define SL_ERROR_BIND           -4
#define SL_ERROR_LISTEN         -5
#define SL_ERROR_RESOLVE        -6
#define SL_ERROR_CONNECT        -7
#define SL_ERROR_ACCEPT         -8
#define SL_ERROR_POOL           -9
#define SL_ERROR_SELECT         -10
#define SL_ERROR_READ           -11
#define SL_ERROR_WRITE          -12
#define SL_ERROR_DISCONNECT     -13
#define SL_ERROR_ALREADY_INIT   -14
#define SL_ERROR_NOTINIT        -15
#define SL_TIMEOUT              -16

#define SL_NUM_ERRORS           17
// ---------------------------------------------------------------------------
// initialize network subsystem
int sl_init(void);

// finalize network subsystem
void sl_term(void);

// get extra error code (useful for win32 WSA functions)
int sl_get_last_error();

// make server TCP socket
int sl_make_server_socket_ex(const char *host_ip, int port, int backlog);

// make server TCP socket (simple)
int sl_make_server_socket(int port); // host_ip="0.0.0.0" backlog=1

// make client TCP socket
int sl_connect_to_server(const char *host, int port);

// close socket
int sl_disconnect(int fd);

// accept wrapper (return file descriptor or -1 on error)
int sl_accept(int server_socket, unsigned *ipaddr);

// select wraper for non block read (return 0:false, 1:true, -1:error)
int sl_select(int fd, int msec);

// fuse select wraper (always return 1)
int sl_select_fuse(int fd, int msec);

// read wraper
int sl_read(int fd, void *buf, int size);

// read `size` bytes from stream `fd` to `buf` at once
int sl_read_all(int fd, void *buf, int size);

// read `size` bytes from stream `fd` to `buf` at once with use timeout
int sl_read_all_to(int fd, void *buf, int size, int ms);

// write `size` bytes to stream `fd` from `buf` at once
int sl_write(int fd, const void *buf, int size);

// make server UDP socket
int sl_udp_make_server_socket(int port);

// make client UDP socket
int sl_udp_make_client_socket();

// read datagram from UDP socket (blocked)
int sl_udp_read(int fd, void *buf, int size, unsigned *ipaddr);

// read datagram from UDP socket (timeout)
int sl_udp_read_to(int fd, void *buf, int size, unsigned *ipaddr, int ms);

// send datagram to peer via UDP to ip numeric
int sl_udp_sendto(int fd, unsigned ipaddr, int port, const void *buf, int size);

// send datagram to peer via UDP to ip char*
int sl_udp_sendto_addr(int fd, const char *host, int port, const void *buf, int size);

// convert dotted ipaddr to numeric
unsigned sl_inet_aton(const char *s);

// convert numeric ipaddr to dotted
const char *sl_inet_ntoa(unsigned ipaddr);

// ntohl etc wrappers
unsigned sl_htonl(unsigned hostlong);
unsigned short sl_htons(unsigned short hostshort);
unsigned sl_ntohl(unsigned netlong);
unsigned short sl_ntohs(unsigned short netshort);

// returns socklib error string
char *sl_error_str(int err);
// ---------------------------------------------------------------------------
#ifdef __cplusplus
}
#endif // __cplusplus
// ---------------------------------------------------------------------------
#endif // SOCKLIB_H

/*** end of "socklib.h" file ***/
