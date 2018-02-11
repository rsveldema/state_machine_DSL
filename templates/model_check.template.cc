#include "generated_state_machine_{{base_name}}.hpp" 

#include "../model_checker/model_check.h"	

int main(int argc, char **argv)
{
  {{state_machine_name}}::addAssertHook();
  
  {{state_machine_name}} *p = new {{state_machine_name}}();
  
  p->initial_transition(p->state_union.bootup);
  Modelchecker<{{state_machine_name}}> mc(p);
  mc.run();
  return 0;
}
