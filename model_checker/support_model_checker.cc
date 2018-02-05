#include "builtins_statemachine.h"
#include "support_boiler.h"
#include "tiny_vector.hpp"
#include "HashValue.h"

/** for the model checker we use logical time units.
 * When doing normal unit tests we use physical time as there it may be more important.
 */

static uint64_t t;

// in the model checker we do not want the actual time used
units::micros currentTimeMicros()
{
  return t++;
}

void warp_speed_clock(const ZEP::Utilities::Timeout &timeout)
{
  uint64_t old = t;
  t = timeout.get().get();

  fprintf(stderr, "-----> time warp from %lld to %lld\n", (long long) old, (long long) t);
}
