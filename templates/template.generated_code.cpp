#include "generated_state_machine_{{base_name}}.hpp"

namespace {{base_name}}
{

  {{MAIN_CODE}}


  BASE_{{state_machine_name}}::EventVector BASE_{{state_machine_name}}::getEventVector()
    {
       EventVector vec;
       {{EVENT_VEC}}
       return vec;
    }
}
