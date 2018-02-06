#include <map>
#include <stack>
#include <string>
#include <algorithm>
#include <vector>
#include <memory>

void warp_speed_clock(const ZEP::Utilities::Timeout &t);



template<typename T, typename K>
bool contains(const T &l, const K &k)
{
  return std::find(l.begin(), l.end(), k) != l.end();
}


template<class T>
class HashEntry
{
private:
  const T * const data;
  HashValue hash;

public:
  HashEntry(const T * const &_data)
    : data(_data),
      hash(data->getHash())
  {
    //fprintf(stderr, "computed hash: %lx\n", (long) hash.get());
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
public:
  T* data;
};


template<class T>
class Modelchecker
{
private:
  std::map<HashEntry<T>, T*> hashmap;
  std::stack<Todo<T>> todo_stack;

  struct Statictics {
    uint64_t creations = 0;
    uint64_t duplicates = 0;
    uint64_t steps = 0;
    
    uint64_t warnings = 0;
    std::vector<std::string> logged_warnings;
    
    void dump()
    {
      fprintf(stderr, "------------------------------------\n");
      fprintf(stderr, "------ MODEL CHECKING REPORT -------\n");
      fprintf(stderr, "    STEPS: %ld\n", (long)steps);
      fprintf(stderr, "    CREATED machines:   %ld\n", (long)creations);
      fprintf(stderr, "    DUPLICATE machines: %ld", (long)duplicates);

      double effectiveness = 100 * ((double)duplicates / (double) creations);
      fprintf(stderr, "        (hash effectiveness %4.0f %%)\n", effectiveness);

      fprintf(stderr, "    WARNINGS: %ld", (long)warnings);
      for (std::string msg : logged_warnings)
	{
	  fprintf(stderr, "\tWARN: %s\n", msg.c_str());
	}
      fprintf(stderr, "------------------------------------\n");
    }


    void warn(const std::string &msg)
    {
      fprintf(stderr, "WARNING: %s\n", msg.c_str());

      if (! contains(logged_warnings, msg))
	{
	  logged_warnings.push_back(msg);
	}
    }
    
  } stats;

 public:
  Modelchecker(T* init)
    {
      todo_stack.push(Todo<T>{init});
    }
  
private:
  bool already_seen_or_add(T* &p)
  {
    HashEntry<T> entry(p);
    if (hashmap.find(entry) == hashmap.end())
      {
	hashmap[entry] = new T(*p);
	return false;
      }
    else
      {
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

  bool send_events(T* &p)
  {
    if (! already_seen_or_add(p))
      {
	//fprintf(stderr, "not already seen!\n");
	for (auto event : p->getEventVector())
	  {
	    stats.creations++;
	    T* clone(new T(*p));
	    clone->do_emit(event);
	    
	    todo_stack.push(Todo<T>{clone});
	  }
	return true;
      }
    else
      {
	//fprintf(stderr, "already seen!\n");
	stats.duplicates++;
	return false;
      }
  }

  void process(Todo<T> &todo)
  {        
    T* p = todo.data;

    bool done = false;
    while (!done)
      {
	if (! send_events(p))
	  {
	    fprintf(stderr, "failed to send events, machine already seen\n");
	  }

	T temp(*p);
	if (! step(p))
	  {
	    //fprintf(stderr, "XXXXXX  no pending events\n");
	    done = true;
	  }
	else
	  {
	    stats.steps++;
	    if (temp.equals(*p, false))
	      {
		done = true;
	      }
	    else if (temp.equals(*p, true))
	      {
		std::string msg("the only difference after state comparison is a deadline change due to a re-submitted command.\n");
		msg += "\t\tBEFORE: " + temp.toString() + "\n";
		msg += "\t\tAFTER:  " + p->toString() + "\n";

		
		stats.warn(msg);
		done = true;
	      }
	    else
	      {
		//fprintf(stderr, "XXXXXXX state machine is different after stepping?\n");
	      }
	  }
      }
    fprintf(stderr, "------------------   trying next one!\n");
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
	
