#include "support_statemachine.h"

unsigned get_unique_id()
{
  static unsigned id;
  return id++;
}

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

std::string time2str(const units::micros &micros)
{
  static char buf[64];

  uint64_t secs   = micros.to_secs();
  uint64_t millis = micros.to_millis() % 1000;

  sprintf(buf, "%lld.%lld sec", secs, millis);

  return buf;
}




bool should_restore_from_FLASH()
{
  return false;
}


bool need_wakeup()
{
  return false;
}


