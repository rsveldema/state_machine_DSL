
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
#include <string>
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

  bool empty() const
  {
    return size() == 0;
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

  bool equals(const delayed_event_queue &other, bool ignore_deadline) const
  {
    if (count != other.count)
      {
	return false;
      }

    for (unsigned i = 0; i < _max_size; i++)
      {
	if (valid[i] != other.valid[i])
	  {
	    return false;
	  }
	
	if (valid[i])
	  {
	    if (! ignore_deadline)
	      {
		if (elts[i].first != other.elts[i].first)
		  {
		    return false;
		  }
	      }
	    if (elts[i].second != other.elts[i].second)
	      {
		return false;
	      }
	  }
      }
    return true;
  }

  std::string toString() const
    {
      std::string ret("delayed<");
      for (unsigned i=0;i<count;i++)
	{
	  if (valid[i])
	    {
	      ret += "(";
	      ret += elts[i].first.toString();
	      ret += ", ";
	      ret += elts[i].second.toString();
	      ret += ")";
	    }
	}
      ret += ">";
      return ret;
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
	    if (! valid[i] && other.valid[i])
	      {
		return true;
	      }
	    else if (valid[i] && ! other.valid[i])
	      {
		return false;
	      }
	    else if (valid[i] && other.valid[i])
	      {
		const T &e1 = get(i).second;
		const T &e2 = other.get(i).second;
		
		if (e1.getHash() < e2.getHash())
		  {
		    return true;
		  }
	      }
	  }
      }
    return false;
  }

  size_t max_size() const { return _max_size; }
  size_t size() const     { return count; }
  
  bool is_valid(size_t i) const
  {
    ASSERT(i < _max_size);
    ASSERT(count >= 0);
    return valid[i];
  }

  const delayed_event_t &get(size_t i) const
  {
    ASSERT(i < _max_size);
    ASSERT(valid[i]);
    ASSERT(count > 0);
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
    
    HashValue getHash() const { return payload | (uint64_t) type; }

    bool operator != (const Event &ev) const
    {
      return type != ev.type || payload != ev.payload;
    }
  };

  static const size_t MAX_EVENTS = 16;
  typedef tiny_vector<Event, MAX_EVENTS> EventVector;
  
  static const size_t MAX_DELAYED_EVENTS = 16;
  typedef delayed_event_queue <Event, MAX_DELAYED_EVENTS> delayed_stack_t;
  typedef delayed_stack_t::delayed_event_t delayed_event_t;
  delayed_stack_t delayed_events_stack;

#if STATE_MACHINE_SUPPORT_TRACES
  Trace trace;
#endif

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

  HashValue getHash() const {
    HashValue hashValue = delayed_events_stack.getHash();
    hashValue.add((uint64_t)state << 20);

    {{HASH_CODE}}

    return hashValue;
  }

  std::string eventString() const
    {
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
  
  bool equals (const {{state_machine_name}} &other, bool ignore_deadline) const
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
};

void registerTests_{{base_name}} ();
#endif
