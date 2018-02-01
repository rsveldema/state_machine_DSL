
// -----------------------
//
// generated from {{FILE}}
//
// -----------------------

#ifndef __GENERATED_HEADER__H__
#define __GENERATED_HEADER__H__


#include "builtins_statemachine.h"
#include "support_{{base_name}}.h"



template<class T, size_t _max_size>
class event_vector
{
 private:
  size_t count;
  bool valid[_max_size];
  T elts[_max_size];

 public:
  event_vector()
    {
      count = 0;
      memset(valid, 0, sizeof(valid));
    }

  size_t max_size() const { return _max_size; }
  size_t num_elts() const { return count; }

  bool is_valid(size_t i) const
  {
    ASSERT(i < _max_size);
    return valid[i];
  }

  const T &get(size_t i) const
  {
    ASSERT(i < _max_size);
    ASSERT(valid[i]);
    return elts[i];
  }

  void erase(size_t i)
  {    
    ASSERT(i < _max_size);
    ASSERT(valid[i]);
    ASSERT(count > 0);
        
    valid[i] = false;
    count--;
  }

  bool add(const T &elt)
  {
    for (size_t i = 0; i < _max_size; i++)
      {
	if (! valid[i])
	  {
	    count++;
	    valid[i] = true;
	    elts[i] = elt;
	    return true;
	  }	   
      }
    return false;
  }
};

class {{state_machine_name}}
{
public:
  {{ENUM_CODE}}
  
  class Event
  {
  private:
    const char *descr;
    EVENT type;
    int64_t payload;
    
  public:
    Event() { type = EVENT::EVENT_NONE; }
    Event(const char *_descr, EVENT _type):descr (_descr), type (_type)
    {
    }
    
    EVENT getType() const { return type; }
    int64_t getPayload() const { return payload; }
  };
  
  static const size_t MAX_DELAYED_EVENTS = 16;
  typedef std::pair <ZEP::Utilities::Timeout, Event> delayed_event_t;
  typedef event_vector <delayed_event_t, MAX_DELAYED_EVENTS> delayed_stack_t;
  delayed_stack_t delayed_events_stack;
  Trace trace;

  
  {{MAIN_CODE}}
  
  bool emit (const Event &event, const ZEP::Utilities::Timeout & timeout)
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
	    SM_LOG("dropped event\n");
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

  void emit(const Event &event)
  {
    process_delayed_events();
    do_emit(event);
  }
  
};

void registerTests_{{base_name}} ();
#endif
