#include "support_statemachine.h"


units::micros currentTimeMicros()
{
  struct timeval tv;
  gettimeofday(&tv, NULL);
  units::micros ret = units::secs(tv.tv_sec) + units::micros(tv.tv_usec);

  static units::micros baseline(0);
  static bool first = true;
  if (first)
    {
      first = false;
      baseline = ret;
    }
  
  return ret - baseline;
}

void time2str(const units::micros &micros,
	      char *buf,
	      size_t buf_size)
{
  uint64_t secs   = micros.to_secs();
  uint64_t millis = micros.to_millis() % 1000;

  snprintf(buf, buf_size, "%lld.%lld sec", (long long) secs, (long long) millis);
}




bool should_restore_from_FLASH()
{
  return false;
}


bool need_wakeup()
{
  return false;
}


