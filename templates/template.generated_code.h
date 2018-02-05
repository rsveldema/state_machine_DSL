
// -----------------------
//
// generated from {{FILE}}
//
// -----------------------

#ifndef __GENERATED_HEADER__H__
#define __GENERATED_HEADER__H__


#include "builtins_statemachine.h"
#include "support_{{base_name}}.h"
#include "tiny_vector.hpp"

#include "HashValue.h"


template<class T, size_t _max_size>
class delayed_event_queue
{
 public:
  typedef std::pair <ZEP::Utilities::Timeout, T> delayed_event_t;

 private:
  size_t count;
  bool valid[_max_size];
  delayed_event_t elts[_max_size];

 public:
  delayed_event_queue()
    {
      count = 0;
      memset(valid, 0, sizeof(valid));
    }

  HashValue getHash() const
  {
    HashValue hash(count);
    for (unsigned i = 0; i < count; i++)
      {
	hash.add(elts[i].second.getHash());
      }
    return hash;
  }

  bool operator < (const delayed_event_queue &other) const
  {
    if (count < other.count)
      {
	return true;
      }
    if (count == other.count)
      {
	for (unsigned i = 0; i < count; i++)
	  {
	    const T &e1 = get(i).second;
	    const T &e2 = other.get(i).second;
	    
	    if (e1.getHash() < e2.getHash())
	      {
		return true;
	      }
	  }
      }
    return false;
  }

  size_t max_size() const { return _max_size; }
  size_t num_elts() const { return count; }

  bool is_valid(size_t i) const
  {
    ASSERT(i < _max_size);
    return valid[i];
  }

  const delayed_event_t &get(size_t i) const
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

  bool add(const delayed_event_t &elt)
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
    Event()
      : descr(0),
	type(EVENT::EVENT_NONE),
	payload(0)
    {
    }
    
    Event(const char *_descr, EVENT _type)
      : descr (_descr),
	type (_type),
	payload(0)
    {
    }
    
    EVENT getType() const { return type; }
    int64_t getPayload() const { return payload; }
    
    HashValue getHash() const { return payload | (uint64_t) type; }
  };

  static const size_t MAX_EVENTS = 16;
  typedef tiny_vector<Event, MAX_EVENTS> EventVector;
  
  static const size_t MAX_DELAYED_EVENTS = 16;
  typedef delayed_event_queue <Event, MAX_DELAYED_EVENTS> delayed_stack_t;
  typedef delayed_stack_t::delayed_event_t delayed_event_t;
  delayed_stack_t delayed_events_stack;
  Trace trace;

  EventVector getEventVector()
  {
    EventVector vec;
    {{EVENT_VEC}}
    return vec;
  }

  HashValue getHash() const {
    HashValue hashValue = delayed_events_stack.getHash();
    hashValue.add((uint64_t)state << 20);

    {{HASH_CODE}}

    return hashValue;
  }
  
  bool operator < (const {{state_machine_name}} &other) const
  {
    {{COMPARE_FIELDS}}
    return delayed_events_stack < other.delayed_events_stack;
  }
  
  {{MAIN_CODE}}

  
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

  
  bool removeEarliestDeadlineEvent(delayed_event_t &found_de)
  {
    ZEP::Utilities::Timeout earliest_time;
    bool success = false;
    for (size_t i = 0; i < delayed_events_stack.max_size(); i++)
      {
	if (delayed_events_stack.is_valid(i))
	  {
	    const delayed_event_t &de = delayed_events_stack.get(i);
	    const ZEP::Utilities::Timeout t = de.first;
	    
	    if (t.hasElapsed())
	      {
		if (! success)
		  {
		    earliest_time = t;
		    found_de = de;
		  }
		else
		  {
		    if (earliest_time > t)
		      {
			earliest_time = t;
			found_de = de;
		      }
		  }
		
		success = true;
	      }
	  }
      }
    return success;
  }  
};

void registerTests_{{base_name}} ();
#endif
