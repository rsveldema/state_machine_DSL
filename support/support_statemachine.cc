#include "support_statemachine.hpp"

/** support code for when not using the model checker.
 * The model checker uses the support file ../model_checker/support_model_checker.cc
 */

static void *current_ptr;

void set_thread_local_state_machine_ptr(void *ptr)
{
  current_ptr = ptr;
}

void *get_thread_local_state_machine_ptr()
{
  return current_ptr;
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

void time2str(const units::micros &micros,
	      char *buf,
	      size_t buf_size)
{
  uint64_t secs   = micros.to_secs();
  uint64_t millis = micros.to_millis() % 1000;

  snprintf(buf, buf_size, "%lld.%lld sec", (long long) secs, (long long) millis);
}



