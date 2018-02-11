#include "generated_state_machine_power_manager.hpp" 

#include "../model_checker/model_check.h"	

int main(int argc, char **argv)
{
  PowerManager::addAssertHook();
  
  PowerManager *p = new PowerManager();
  
  p->initial_transition(p->state_union.bootup);
  Modelchecker<PowerManager> mc(p);
  mc.run();
  return 0;
}
