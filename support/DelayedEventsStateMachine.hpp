


#include "builtins_statemachine.hpp"
#include "tiny_vector.hpp"
#include "units.hpp"

template<typename Event, typename StateEnum>
class DelayedEventsStateMachine : public AbstractStateMachine<Event, StateEnum>
{
 public:
  struct delayed_event_t
  {
    const ZEP::Utilities::Timeout timeout;
    Event event;
  };
  

  static const size_t MAX_DELAYED_EVENTS = 16;
  typedef tiny_vector <delayed_event_t, MAX_DELAYED_EVENTS> delayed_stack_t;
  delayed_stack_t delayed_events_stack;

public:
  virtual void emit(const Event &event) override
  {
    process_delayed_events();
    do_emit(event);
  }
  
  bool hasPendingEvents() const
  {
    return ! delayed_events_stack.empty();
  }
  
  bool emit(const Event &event, const ZEP::Utilities::Timeout &timeout)
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
};
