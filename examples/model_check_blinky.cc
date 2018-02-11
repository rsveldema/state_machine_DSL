#include "generated_state_machine_blinky.hpp" 

#include "../model_checker/model_check.h"	

int main(int argc, char **argv)
{
  Blinky::addAssertHook();
  
  Blinky *p = new Blinky();
  
  p->initial_transition(p->state_union.bootup);
  Modelchecker<Blinky> mc(p);
  mc.run();
  return 0;
}
