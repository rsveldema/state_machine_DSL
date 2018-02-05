#include <map>
#include <stack>

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
    fprintf(stderr, "computed hash: %lx\n", (long) hash.get());
  }
  
  bool operator < (const HashEntry &e) const
  {
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
  T data;
};


template<class T>
class Modelchecker
{
private:
  std::map<HashEntry<T>, T *> map;
  std::stack<Todo<T*>> todo_stack;

  struct Statictics {
    uint64_t creations = 0;
    uint64_t duplicates = 0;

    void dump()
    {
      fprintf(stderr, "------------------------------------\n");
      fprintf(stderr, "------ MODEL CHECKING REPORT -------\n");
      fprintf(stderr, "    CREATED machines:   %ld\n", (long)creations);
      fprintf(stderr, "    DUPLICATE machines: %ld", (long)duplicates);

      double effectiveness = 100 * ((double)duplicates / (double) creations);
      fprintf(stderr, "        (hash effectiveness %4.0f %%)\n", effectiveness);
      fprintf(stderr, "------------------------------------\n");
    }
  } stats;

public:
  Modelchecker(T *init)
  {
    todo_stack.push(Todo<T*>{init});
  }

private:
  bool already_seen(T *p)
  {
    HashEntry<T> entry(p);
    return map.find(entry) != map.end();
  }


  bool step(T *p)
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

  bool send_events(T *p)
  {
    if (! already_seen(p))
      {	
	HashEntry<T> entry(p);
	map[entry] = p;
	
	fprintf(stderr, "not already seen!\n");
	for (auto event : p->getEventVector())
	  {
	    stats.creations++;
	    T *clone = new T(*p);
	    clone->do_emit(event);
	    
	    todo_stack.push(Todo<T*>{clone});
	  }
	return true;
      }
    else
      {
	fprintf(stderr, "already seen!\n");
	stats.duplicates++;
	return false;
      }
  }

  void process(const Todo<T*> &todo)
  {        
    T *p = todo.data;

    bool done = false;
    while (!done)
      {
	if (! send_events(p))
	  {
	    fprintf(stderr, "failed to send events, machine already seen\n");
	  }
	
	if (! step(p))
	  {
	    fprintf(stderr, "no pending events\n");
	    done = true;
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
	Todo<T*> t = todo_stack.top();
	todo_stack.pop();
	
	process(t);
      }

    stats.dump();
  }
};
	
