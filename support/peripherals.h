#ifndef PERIPHERALS__H___
#define PERIPHERALS__H___


#include <assert.h>
#include "HashValue.h"
#include <string>

class Stopwatch
{
  int state = 1;

 public: 
  void init()  { state = 2; }
  void start() { state = 3; }
  void stop()  { state = 4; }

  Stopwatch()
    {
    }

  Stopwatch(const Stopwatch &w)
    : state(w.state)
    {
    }

  HashValue getHash() const { return state; }
  bool operator < (const Stopwatch &e) const { return getHash() < e.getHash(); }
  bool operator != (const Stopwatch &e) const { return state != e.state; }
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

  HashValue getHash() const { return enabled; }
  bool operator < (const Boiler &e) const { return getHash() < e.getHash(); }
  bool operator != (const Boiler &e) const { return enabled != e.enabled; }
  
  void on()   { assert(! enabled); enabled = true;  t = ZEP::Utilities::Timeout(units::secs(4)); }
  void off()  { assert(enabled);   enabled = false; t = ZEP::Utilities::Timeout(0); }
  bool done() { return enabled && t.hasElapsed(); }
};

class Pump
{
  int state = 1;
  
 public:
  void on()  { state = 2; }
  void off() { state = 3; }

  HashValue getHash() const { return state; }
  bool operator < (const Pump &e) const { return getHash() < e.getHash(); }
  bool operator != (const Pump &e) const { return state != e.state; }
};





#endif
