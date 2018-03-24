
// -----------------------
//
// generated from {{FILE}}
//
// -----------------------

#ifndef __GENERATED_HEADER__H__
#define __GENERATED_HEADER__H__


#include <stdlib.h>
#include <stdio.h>

#include <tuple>

  {{ENUM_CODE}}

namespace {{base_name}}
{
  static inline const char *state_2_string(STATES s)
  {
    switch (s)
      {
      case STATES::STATE_NONE: return "NONE?";
	{{STATE2STR}}
      }
    return "unknown state enum entry?";
  }
}

static inline const char *state_2_string({{base_name}}::STATES s)
{
  return {{base_name}}::state_2_string(s);
}

#include "AbstractStateMachine.hpp"
#include "DelayedEventsStateMachine.hpp"
#include "ModelCheckeableStateMachine.hpp"

#include "support_{{base_name}}.hpp"

class BASE_{{state_machine_name}} : public AbstractStateMachine<{{base_name}}::EVENT,
                                                           {{base_name}}::STATES>
{
 public:
  static const size_t MAX_EVENTS = 16;
  typedef tiny_vector<Event, MAX_EVENTS> EventVector;
    
  EventVector getEventVector();
  
  {{MAIN_DECL}}
};

void registerTests_{{base_name}} ();

#if {{USE_FEATURE_CLASS}}
typedef {{base_class}}<BASE_{{state_machine_name}}> {{state_machine_name}};
#else
typedef BASE_{{state_machine_name}} {{state_machine_name}};
#endif

#endif
