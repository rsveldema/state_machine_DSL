#ifndef DELAYED_EVENTS_STATE_MACHINE_H_____
#define DELAYED_EVENTS_STATE_MACHINE_H_____

#include "builtins_statemachine.hpp"
#include "tiny_vector.hpp"
#include "units.hpp"

template<typename Event>
class event_with_timeout_t
{
public:
  Timeout timeout;
  Event event;
  
  event_with_timeout_t() {}
  
  event_with_timeout_t(const Timeout &_timeout,
		  const Event &_event)
    : timeout(_timeout),
      event(_event)
  {
  }
  
  bool operator < (const event_with_timeout_t &other) const
  {
    if (timeout >= other.timeout)
      {
	return false;
      }
    if (event.getType() >= other.event.getType())
      {
	return false;
      }
    return true;
  }
  
  bool operator >= (const event_with_timeout_t &other) const
  {
    if (timeout < other.timeout)
      {
	return false;
      }
    if (event.getType() < other.event.getType())
      {
	return false;
      }
    return true;
  }
  
  bool operator != (const event_with_timeout_t &other) const
  {
    if (timeout != other.timeout)
      {
	return true;
      }
    if (event.getType() != other.event.getType())
      {
	return true;
      }
    return false;
  }
};



template<class BASE>
class DelayedEventsStateMachine : public BASE
{
 public:
  typedef typename BASE::Event Event;
  typedef ::event_with_timeout_t<Event> delayed_event_t;

  static const size_t MAX_DELAYED_EVENTS = 16;
  typedef tiny_vector <delayed_event_t, MAX_DELAYED_EVENTS> delayed_stack_t;
  delayed_stack_t delayed_events_stack;

public:
  virtual void emit(const Event &event) override
  {
    this->process_delayed_events();
    this->do_emit(event);
  }
  
  bool hasPendingEvents() const
  {
    return ! delayed_events_stack.empty();
  }
  
  virtual bool emit(const Event &event, const Timeout &timeout) override
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


  virtual bool process_delayed_events() override
  {
    bool success = false;
    for (size_t i = 0; i < delayed_events_stack.max_size(); i++)
      {
	if (delayed_events_stack.is_valid(i))
	  {
	    const delayed_event_t &de = delayed_events_stack.at(i);
	    const Timeout t = de.timeout;
	    
	    if (t.hasElapsed())
	      {
		delayed_events_stack.erase(i);
	    		
		auto event      = de.event;
		this->do_emit(event);
		success = true;
	      }
	  }
      }
    return success;
  }
  
  bool removeEarliestDeadlineEvent(delayed_event_t &found_de)
  {
    Timeout earliest_time;
    int earliest_index = -1;
    bool success = false;
    for (size_t i = 0; i < delayed_events_stack.max_size(); i++)
      {
	if (delayed_events_stack.is_valid(i))
	  {
	    const delayed_event_t &de = delayed_events_stack.at(i);
	    const Timeout t = de.timeout;
	    
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

#endif
