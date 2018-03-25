
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

namespace {{base_name}}
{
  {{ENUM_CODE}}

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

#include "DelayedEventsStateMachine.hpp"
#include "ModelCheckableStateMachine.hpp"
#include "support_{{base_name}}.hpp"


namespace {{base_name}}
{
  class BASE_{{state_machine_name}}
  {
  public:
    typedef {{base_name}}::EVENT  EVENT;
    typedef {{base_name}}::STATES STATES;
    
    STATES state;
    
    class Event
    {
    private:
      typedef EVENT EventEnum_t;
      EVENT type;
      int64_t payload;
      
    public:
      Event()
	: type(EVENT::EVENT_NONE),
	  payload(0)
      {
      }
      
      Event(EVENT _type)
	: type (_type),
	  payload(0)
      {
      }
      
      const char *toString() const {
	return eventToString(type);      
      }
      
      EVENT getType() const { return type; }
      int64_t getPayload() const { return payload; }
    
      bool operator != (const Event &ev) const
      {
	return type != ev.type || payload != ev.payload;
      }
    };
    
    
    static const size_t MAX_EVENTS = 16;
    typedef tiny_vector<Event, MAX_EVENTS> EventVector;
    
    EventVector getEventVector();
    
    {{MAIN_DECL}}
    
    template<class Machine, class State>
      static void initialize_state(Machine *machine, State *statePtr, STATES stateEnumValue)
    {
      new (statePtr) State();
      machine->state = stateEnumValue;
    }
    
    virtual void dump() {}
    virtual bool process_delayed_events()
    {
      assert(false); // unimplemented in base class.
    }
    virtual bool emit(const Event &event, const Timeout &timeout)
    {
      assert(false); // unimplemented in base class.
    }
    virtual void emit(const Event &event)
    {
      do_emit(event);
    }
  };
  
  
#if {{USE_FEATURE_CLASS}}
  typedef {{base_class}}<BASE_{{state_machine_name}}> {{state_machine_name}};
#else
  typedef BASE_{{state_machine_name}} {{state_machine_name}};
#endif
  
}

void registerTests_{{base_name}} ();

#endif
