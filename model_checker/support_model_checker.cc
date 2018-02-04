#include "builtins_statemachine.h"
#include "support_boiler.h"
#include "tiny_vector.hpp"
#include "HashValue.h"


// in the model checker we do not want the actual time used
units::micros currentTimeMicros()
{
  static uint64_t t;
  return t++;
}
