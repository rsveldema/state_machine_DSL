#ifndef SUPP_BLINKY___H___
#define SUPP_BLINKY___H___


#include <assert.h>
#include "HashValue.hpp"

class Led
{
 private:
  unsigned id;
  bool state = false;
  
 public:
  Led()
    {
      id = -1;
    }

  HashValue getHash() const { return id | state << 12; }
  bool operator >= (const Led &e) const { return getHash() >= e.getHash(); }
  bool operator != (const Led &e) const { return getHash() != e.getHash(); }
  
  void init(int id) { this->id = id; }
  
  void on()
  {
    state = true;
    printf("%d: ON\n", id);
  }

  void off()
  {
    state = false;
    printf("%d: OFF\n", id);
  }

  void toggle()
  {
    state = ! state;
    printf("%d: toggle: %d\n", id, state);
  }

  bool isOn() const
  {
    return state;
  }
};

static inline void init(Led &led, int id) { led.init(id); } 


#endif
