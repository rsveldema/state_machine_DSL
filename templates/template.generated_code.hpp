
// -----------------------
//
// generated from {{FILE}}
//
// -----------------------

#ifndef __GENERATED_HEADER__H__
#define __GENERATED_HEADER__H__

#if SUPPORT_DELAYED_EVENTS
#include "builtins_statemachine.h"
#include "support_{{base_name}}.h"
#endif

#if SUPPORT_MODEL_CHECKING
#include "HashValue.h"
#endif

#if SUPPORT_DELAYED_EVENTS
#include "event_queue.h"
#endif


class {{state_machine_name}}
{
public:
  {{ENUM_CODE}}
  
  class Event
  {
  private:
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

#if SUPPORT_MODEL_CHECKING
    HashValue getHash() const { return payload | (uint64_t) type; }
#endif
    
    bool operator != (const Event &ev) const
    {
      return type != ev.type || payload != ev.payload;
    }
  };

 private:

#if SUPPORT_DELAYED_EVENTS
  static const size_t MAX_EVENTS = 16;
  typedef tiny_vector<Event, MAX_EVENTS> EventVector;  
  static const size_t MAX_DELAYED_EVENTS = 16;
  typedef delayed_event_queue <Event, MAX_DELAYED_EVENTS> delayed_stack_t;
  delayed_stack_t delayed_events_stack;
 public:
  typedef delayed_stack_t::delayed_event_t delayed_event_t;

#endif

#if STATE_MACHINE_SUPPORT_TRACES
  static const unsigned MAX_TRACE_LEN = 32;
 public:
  Trace<{{state_machine_name}}, MAX_TRACE_LEN, EVENT, STATES> trace;
#endif

  
#if SUPPORT_MODEL_CHECKING
 public:
  static {{state_machine_name}} *getInstance()
  {
    return ({{state_machine_name}} *) get_thread_local_state_machine_ptr();
  }

  static void setInstance({{state_machine_name}} *instance)
  {
    set_thread_local_state_machine_ptr(instance);
  }
#endif
  
  static const char *state_2_string(STATES s)
  {
    switch (s)
      {
      case STATES::STATE_NONE: return "NONE?";
	{{STATE2STR}}
      }
    return "unknown state enum entry?";
  }

 private:
#if SUPPORT_MODEL_CHECKING
  static void assertHook()
  {
    if ({{state_machine_name}} *sm = getInstance())
      {
	fprintf(stderr, "assert failed in state %s", sm->toString().c_str());
#if STATE_MACHINE_SUPPORT_TRACES
	sm->trace.dump();
#endif
      }
    else
      {
	fprintf(stderr, "no current state machine available...\n");
      }
  }

 public:
  static void addAssertHook()
  {
    add_assert_hook(assertHook);
  }
  
  HashValue getHash() const {
    HashValue hashValue = delayed_events_stack.getHash();
    hashValue.add((uint64_t)state << 20);

    {{HASH_CODE}}

    return hashValue;
  }
  
  std::string toString() const
    {
      std::string ret("{{state_machine_name}}<");
      ret += delayed_events_stack.toString();
      ret += ">, ";

      switch (state)
	{
	  case STATES::STATE_NONE:
	  {
	    ASSERT(0);
	  }

	  {{STATES_TO_STRING}}
	}
      
      return ret;
    }
  

  bool operator == (const {{state_machine_name}} &other) const
  {
    fprintf(stderr, "OPERATOR ==!\n");
    return equals(other, true);
  }
  
  bool equals(const {{state_machine_name}} &other, bool ignore_deadline) const
  {
    {{EQUAL_FIELDS}}

    if (delayed_events_stack.equals(other.delayed_events_stack, ignore_deadline))
      {
	switch (state)
	  {
	  case STATES::STATE_NONE:
	  {
	    ASSERT(0);
	  }
	  
	  {{EQUAL_STATES}}
	  }
      }
    return false;
  }

  
  bool operator < (const {{state_machine_name}} &other) const
  {
    {{COMPARE_FIELDS}}
    
    if (delayed_events_stack < other.delayed_events_stack)
      {
	return true;
      }
    switch (state)
      {
	case STATES::STATE_NONE:
	  {
	    ASSERT(0);
	  }
	  
	{{COMPARE_STATES}}
      }
    return false;
  }
#endif
  

#if SUPPORT_DELAYED_EVENTS
  bool hasPendingEvents() const
  {
    return ! delayed_events_stack.empty();
  }
  
  EventVector getEventVector()
  {
    EventVector vec;
    {{EVENT_VEC}}
    return vec;
  }
#endif


 public:
  {{MAIN_DECLS}}

#if SUPPORT_DELAYED_EVENTS  
  bool emit (const Event &event, const ZEP::Utilities::Timeout &timeout)
  {
    if (timeout.hasElapsed ())
      {
	emit(event);
      }
    else
      {
	bool success = delayed_events_stack.add(delayed_event_t(timeout, event));
	if (! success)
	  {
	    return false;
	  }
      }
    return true;
  }


  bool process_delayed_events()
  {
    bool success = false;
    for (size_t i = 0; i < delayed_events_stack.max_size(); i++)
      {
	if (delayed_events_stack.is_valid(i))
	  {
	    const delayed_event_t &de = delayed_events_stack.get(i);
	    const ZEP::Utilities::Timeout t = de.first;
	    
	    if (t.hasElapsed())
	      {
		delayed_events_stack.erase(i);
	    		
		auto event      = de.second;
		do_emit(event);
		success = true;
	      }
	  }
      }
    return success;
  }
  
  bool removeEarliestDeadlineEvent(delayed_event_t &found_de)
  {
    ZEP::Utilities::Timeout earliest_time;
    int earliest_index = -1;
    bool success = false;
    for (size_t i = 0; i < delayed_events_stack.max_size(); i++)
      {
	if (delayed_events_stack.is_valid(i))
	  {
	    const delayed_event_t &de = delayed_events_stack.get(i);
	    const ZEP::Utilities::Timeout t = de.first;
	    
	    if (! success)
	      {
		success = true;
		
		earliest_time = t;
		found_de = de;
		earliest_index = i;
	      }
	    else
	      {
		if (earliest_time > t)
		  {
		    earliest_time = t;
		    found_de = de;
		    earliest_index = i;
		  }
	      }
	  }
      }
    if (success)
      {
	//fprintf(stderr, "---- erase: %d\n", earliest_index);
	assert(earliest_index >= 0);
	delayed_events_stack.erase(earliest_index);
      }
    else
      {
	//fprintf(stderr, "---- failed to retrieve earliest event\n");
      }
    return success;
  }  
#endif

  void emit(const Event &event)
  {
#if SUPPORT_DELAYED_EVENTS
    process_delayed_events();
#endif
    do_emit(event);
  }

};

void registerTests_{{base_name}} ();
#endif
