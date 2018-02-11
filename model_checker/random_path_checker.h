#include <map>
#include <stack>
#include <string>
#include <algorithm>
#include <vector>
#include <memory>
#include "../support/stats.h"

template<class T>
class RandomPathChecker
{
 private:
  const unsigned max_step_count_per_machine;

  class Statistics : public CheckerStatistics 
  {
  public:
    void dump()
    {
      fprintf(stderr, "------------------------------------\n");
      fprintf(stderr, "------ RANDOM PATH CHECKING REPORT -------\n");
      fprintf(stderr, "    STEPS: %ld  (max sequence len: %ld)\n",
	      (long)steps,
	      (long)max_step_count);
    }
  };

  Statistics stats;
  T* machine;
  
 public:
 RandomPathChecker(T* _machine,
		   unsigned _max_step_count_per_machine)
   : machine(_machine),
    max_step_count_per_machine(_max_step_count_per_machine)
    {      
    }
  
private:
  bool step()
  {
    typename T::delayed_event_t found_de;
    if (machine->removeEarliestDeadlineEvent(found_de))
      {	
	warp_speed_clock(found_de.first);
	
	machine->do_emit(found_de.second);
	return true;
      }
    else
      {
	// no pending events, can only inject new events
	return false;
      }
  }

  void send_event()
  {
    //fprintf(stderr, "------------------  new %p\n", clone);    
    machine->do_emit(machine->getEventVector().getRandomElement());
  }
  
  /** process one element from the to-do stack
   */
  void process()
  {            
    volatile unsigned step_count = 0;
    while (1)
      {
	step_count++;
	stats.record_max_step_count(step_count);
	if (step_count > max_step_count_per_machine)
	  {
	    fprintf(stderr, "max step count reached for this machine, assuming all okay...\n");
	    break;
	  }
	
	// send possible external events to the machine.
	send_event();

	// see if we can execute a step
	T temp(*machine);
	bool have_pending_events = false;
	if (! step())
	  {
	    have_pending_events = true;
	    //fprintf(stderr, "XXXXXX  no pending events: %s\n", machine->toString().c_str());
	    //break;
	  }
	
	// There are pending events for this instance.
	// Now lets process one of the events and see
	// if that changes the MC state of the machine.
	// If no change is made, we can ignore this and any following MC states.
	stats.steps++;
	if (machine->equals(temp, false))
	  {
	    if (have_pending_events)
	      {
		//fprintf(stderr, "XXXXXX  pending event and machine unequal after step\n");
		//break;
	      }
	  }
	else if (machine->equals(temp, true))
	  {
	    std::string msg("the only difference after state comparison is a deadline change due to a re-submitted command.\n");
	    msg += "\t\tBEFORE: " + temp.toString() + "\n";
	    msg += "\t\tAFTER:  " + machine->toString() + "\n";
	    stats.warn(msg);
	    break;
	  }
	else
	  {
	    /*
	    fprintf(stderr,
		    "XXXXXXX state machine is different after stepping?\n");
	    */
	  }
      }    
  }
  
public:
  void run()
  {
    process();

#if STATE_MACHINE_SUPPORT_TRACES
    machine->trace.dump();
#endif
    
    stats.dump();
  }
};
	
