#include <time.h>
#include <stdio.h>
#include <kernel/kernel.h>
#include <hellbender/threadlocal.h>


struct tm *localtime(const time_t *rawtime) {
  struct tm _tm;
  //uint32_t days_since_epoch = (uint32_t)(*rawtime / 86400);
  int second_of_day = (int)(*rawtime % 86400);
  _tm.tm_hour = second_of_day / 3600;
  int second_of_hour = second_of_day % 3600;
  _tm.tm_min = second_of_hour / 60;
  _tm.tm_sec = second_of_hour % 60;
  // TODO: populate remaining fields

  struct tm *retval = &THREADLOCAL->tm;
  *retval = _tm;
  return retval;
}

