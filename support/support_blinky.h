#ifndef SUPP_BLINKY___H___
#define SUPP_BLINKY___H___


#include <assert.h>


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
