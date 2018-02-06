#include "generated_state_machine_boiler.hpp" 

#include "../model_checker/model_check.h"	

int main(int argc, char **argv)
{
  WaterBoiler *p = new WaterBoiler();
  
  p->initial_transition(p->state_union.bootup);
  Modelchecker<WaterBoiler> mc(p);
  mc.run();
  return 0;
}
