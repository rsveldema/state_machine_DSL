
// provided by programmer in support of tests/boiler.sm
// included by generated code.

#include "peripherals.hpp"

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
  bool operator >= (const Stopwatch &e) const { return getHash() >= e.getHash(); }
  bool operator != (const Stopwatch &e) const { return state != e.state; }
};

class Boiler
{
 private:
  Timeout t;
  bool enabled = false;
  
 public:
  Boiler() : t(std::chrono::seconds(0))
    {
    }

  HashValue getHash() const { return enabled; }
  bool operator >= (const Boiler &e) const { return getHash() >= e.getHash(); }
  bool operator != (const Boiler &e) const { return enabled != e.enabled; }
  
  void on()   { assert(! enabled); enabled = true;  t = Timeout(4s); }
  void off()  { assert(enabled);   enabled = false; t = Timeout(0s); }
  bool done() { return enabled && t.hasElapsed(); }
};

class Pump
{
  int state = 1;
  
 public:
  void on()  { state = 2; }
  void off() { state = 3; }

  HashValue getHash() const { return state; }
  bool operator >= (const Pump &e) const { return getHash() >= e.getHash(); }
  bool operator != (const Pump &e) const { return state != e.state; }
};


