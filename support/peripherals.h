#ifndef PERIPHERALS__H___
#define PERIPHERALS__H___


#include <assert.h>
class Stopwatch
{
 public:
  void init() {}
  void start() {}
  void stop() {}
};

class Boiler
{
 private:
  ZEP::Utilities::Timeout t;
  bool enabled = false;
  
 public:
 Boiler() : t(units::secs(0))
    {
    }
  
  void on()   { assert(! enabled); enabled = true; t = ZEP::Utilities::Timeout(units::secs(4)); }
  void off()  { assert(enabled);   enabled = false; t = ZEP::Utilities::Timeout(0); }
  bool done() { return enabled && t.hasElapsed(); }
};

class Pump
{
 public:
  void on() {}
  void off() {}
};





#endif
