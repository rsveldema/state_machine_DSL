#include <map>
#include <stack>
#include <string>
#include <algorithm>
#include <vector>
#include <memory>
#include "../support/stats.h"

void warp_speed_clock(const ZEP::Utilities::Timeout &t);


template<class T>
class HashEntry
{
private:
  T *data;
  HashValue hash;

public:
  HashEntry(T *_data)
    : data(_data),
      hash(data->getHash())
  {
    //fprintf(stderr, "computed hash: %lx\n", (long) hash.get());
  }

  void set(T *a)
  {
    data = a;
  }

  bool operator < (const HashEntry &e) const
  {
    assert((int)data->state != 0);
    assert((int)e.data->state != 0);
    
    if (hash == e.hash)
      {
	return (*data < *e.data);
      }
    return hash < e.hash;
  }
};


template<class T>
class Todo
{
 private:
  T* data;

 public:
  Todo(T *d)
    : data(d)
    {
    }
  T *get() const { return data; }
};


template<class T>
class Modelchecker
{
private:
  const bool use_statistical_analysis;
  const unsigned max_step_count_per_machine;
  std::map<HashEntry<T>, T*> hashmap;
  std::stack<Todo<T>> todo_stack;

  struct Statictics : public CheckerStatistics {
    uint64_t creations = 0;
    uint64_t hash_duplicates = 0;
    uint64_t hash_checks = 0;
        
    void dump()
    {
      fprintf(stderr, "------------------------------------\n");
      fprintf(stderr, "------ MODEL CHECKING REPORT -------\n");
      fprintf(stderr, "    STEPS: %ld  (max sequence len: %ld)\n",
	      (long)steps,
	      (long)max_step_count);
      fprintf(stderr, "    created machines to handle non-deterministic events:   %ld\n", (long)creations);
      fprintf(stderr, "    hash duplicate detected machines: %ld\n", (long)hash_duplicates);
      fprintf(stderr, "    hash checks machines: %ld\n", (long)hash_checks);

      double effectiveness = 100 * ((double)hash_duplicates / (double) hash_checks);
      fprintf(stderr, "        (hash effectiveness %4.0f %%)\n", effectiveness);

      fprintf(stderr, " ----------  %ld WARNINGS ----------\n", (long)warnings);
      for (std::string msg : logged_warnings)
	{
	  fprintf(stderr, "\tWARN: %s\n", msg.c_str());
	}
      fprintf(stderr, "------------------------------------\n");
    }
    
  } stats;

 public:
 Modelchecker(T* init,
	      bool _use_statistical_analysis,
	      unsigned _max_step_count_per_machine)
   : use_statistical_analysis(_use_statistical_analysis),
    max_step_count_per_machine(_max_step_count_per_machine)
    {
      //fprintf(stderr, "------------------  init %p\n", init);

      todo_stack.push(Todo<T>(init));
    }
  
private:
  bool already_seen_or_add(T* &p)
  {
    stats.hash_checks++;
    
    HashEntry<T> entry(p);
    if (hashmap.find(entry) == hashmap.end())
      {
	T *clone = new T(*p);
	entry.set(clone);
	
	hashmap[entry] = new T(*p);
	return false;
      }
    else
      {
	stats.hash_duplicates++;
	return true;
      }
  }


  bool step(T* &p)
  {
    typename T::delayed_event_t found_de;
    if (p->removeEarliestDeadlineEvent(found_de))
      {	
	warp_speed_clock(found_de.first);
	
	p->do_emit(found_de.second);
	return true;
      }
    else
      {
	// no pending events, can only inject new events
	return false;
      }
  }

  void send_event(T *p, typename T::Event &event)
  {
    stats.creations++;
    T* clone = new T(*p);
    //fprintf(stderr, "------------------  new %p\n", clone);    
    clone->do_emit(event);    
    todo_stack.push(Todo<T>{clone});
  }
  

  void send_events(T *p)
  {
    if (use_statistical_analysis)
      {
	send_event(p, p->getEventVector().getRandomElement());
      }
    else
      {
	for (auto event : p->getEventVector())
	  {
	    send_event(p, event);
	  }
      }
  }

  /** process one element from the to-do stack
   */
  void process(Todo<T> &todo)
  {        
    T* p = todo.get();
    
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
	
	// if we've already seen this state somehow, then
	// there is no need to continue searching for states following
	// this one.
	if (already_seen_or_add(p))
	  {
	    break;
	  }

	// send possible external events to the machine.
	send_events(p);

	// see if we can execute a step
	T temp(*p);
	if (! step(p))
	  {
	    fprintf(stderr, "XXXXXX  no pending events\n");
	    break;
	  }
	
	// There are pending events for this instance.
	// Now lets process one of the events and see
	// if that changes the MC state of the machine.
	// If no change is made, we can ignore this and any following MC states.
	stats.steps++;
	if (temp.equals(*p, false))
	  {
	    fprintf(stderr, "XXXXXX  pending event and machine unequal after step\n");
	    break;
	  }
	else if (temp.equals(*p, true))
	  {
	    std::string msg("the only difference after state comparison is a deadline change due to a re-submitted command.\n");
	    msg += "\t\tBEFORE: " + temp.toString() + "\n";
	    msg += "\t\tAFTER:  " + p->toString() + "\n";
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
    
    //fprintf(stderr, "------------------  delete %p\n", p);
    delete p;
  }
  
public:
  void run()
  {
    while (! todo_stack.empty())
      {
	Todo<T> t = todo_stack.top();
	todo_stack.pop();
	
	process(t);
      }

    stats.dump();
  }
};
	
