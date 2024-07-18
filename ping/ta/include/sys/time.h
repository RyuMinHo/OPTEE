#ifndef LWIP_SYS__TIME_H
#define LWIP_SYS__TIME_H

#include <stdlib.h> /* time_t */
#include <stdint.h>

typedef int64_t time_t;

struct timeval {
  time_t    tv_sec;         /* seconds */
  long    tv_usec;        /* and microseconds */
};
int gettimeofday(struct timeval* tp, void* tzp);

#endif
