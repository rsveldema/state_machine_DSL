#include "builtins_statemachine.hpp"
#include "support_boiler.hpp"
#include "tiny_vector.hpp"
#include "HashValue.hpp"

/** for the model checker we use logical time units.
 * When doing normal unit tests we use physical time as there it may be more important.
 */

static uint64_t t = 0;
static void (*assert_hook)();

static void *current_ptr;

void set_thread_local_state_machine_ptr(void *ptr)
{
  current_ptr = ptr;
}

void *get_thread_local_state_machine_ptr()
{
  return current_ptr;
}

void add_assert_hook(void (*func)())
{
  assert_hook = func;
}

void model_check_assert(int line, const char *file, const char *msg)
{
  fprintf(stderr, "%d:%s ASSERT FAILED: assert(%s)\n", line, file, msg);
  if (assert_hook)
    {
      assert_hook();
    }
  abort();
}



// in the model checker we do not want the actual time used
std::chrono::microseconds currentTimeMicros()
{
  return std::chrono::microseconds(t++);
}

void warp_speed_clock(const Timeout &timeout)
{
  uint64_t old = t;
  t = timeout.get().count();
  
  //fprintf(stderr, "-----> time warp from %lld to %lld\n", (long long) old, (long long) t);
}


std::string convertToString(int32_t value)
{
  char buf[32];
  sprintf(buf, "%d", value);
  return buf;
}
