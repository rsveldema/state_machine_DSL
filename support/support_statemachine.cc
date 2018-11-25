#include "support_statemachine.hpp"
#include <chrono>

/** support code for when not using the model checker.
 * The model checker uses the support file ../model_checker/support_model_checker.cc
 */

using namespace std::chrono;

static void *current_ptr;

void set_thread_local_state_machine_ptr(void *ptr)
{
  current_ptr = ptr;
}

void *get_thread_local_state_machine_ptr()
{
  return current_ptr;
}

microseconds currentTimeMicros()
{
  struct timeval tv;
  gettimeofday(&tv, NULL);
  auto ret = seconds(tv.tv_sec) + microseconds(tv.tv_usec);

  static std::chrono::microseconds baseline(0);
  static bool first = true;
  if (first)
    {
      first = false;
      baseline = ret;
    }
  
  return ret - baseline;
}

void time2str(const std::chrono::microseconds &micros,
	      char *buf,
	      size_t buf_size)
{
  uint64_t secs   = (duration_cast<seconds>(micros)).count();
  uint64_t millis = (duration_cast<milliseconds>(micros)).count() % 1000;

  snprintf(buf, buf_size, "%lld.%lld sec", (long long) secs, (long long) millis);
}



