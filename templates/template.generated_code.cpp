#include "generated_state_machine_{{base_name}}.hpp"



{{MAIN_CODE}}


BASE_{{state_machine_name}}::EventVector BASE_{{state_machine_name}}::getEventVector()
{
  EventVector vec;
  {{EVENT_VEC}}
  return vec;
}
