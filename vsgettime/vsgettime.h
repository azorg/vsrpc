/*
 * Very simple POSIX clock_gettime() wrapper
 * File: "vsgettime.h"
 * Windows implemtation from
 *   http://stackoverflow.com/questions/5404277/porting-clock-gettime-to-windows
 */

//----------------------------------------------------------------------------
#ifndef VSGETTIME_H
#define VSGETTIME_H
//----------------------------------------------------------------------------
#include <time.h> // time_t, struct timespec, clock_gettime()
//----------------------------------------------------------------------------
#if (defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__))
#  ifndef VSWIN32
#    define VSWIN32
#  endif
#endif
//----------------------------------------------------------------------------
#ifdef VSWIN32
#  ifndef CLOCK_REALTIME
#    define CLOCK_REALTIME 0
#  endif
struct timespec {
  time_t tv_sec;  // seconds
  long   tv_nsec; // nanoseconds
};
#endif // VSWIN32
//----------------------------------------------------------------------------
#ifdef __cplusplus
extern "C" {
#endif // __cplusplus
//----------------------------------------------------------------------------
#ifdef VSWIN32
int clock_gettime(int clk_id, struct timespec *tp);
//int clock_gettime((clockid_t clk_id, struct timespec *tp);
#endif
//----------------------------------------------------------------------------
// return real time in seconds
double vsgettime();
//----------------------------------------------------------------------------
#ifdef __cplusplus
}
#endif // __cplusplus
//----------------------------------------------------------------------------
#endif // VSGETTIME_H
//----------------------------------------------------------------------------

/*** end of "vsgettime.h" file ***/
