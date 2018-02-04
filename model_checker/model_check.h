#include <map>
#include <stack>

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
  T *data;
};


template<class T>
class Modelchecker
{
private:
  std::map<HashEntry<T>, T *> map;
  std::stack<Todo<T>> todo_stack;

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
    todo_stack.push(Todo<T>{init});
  }

private:
  bool already_seen(T *p)
  {
    HashEntry<T> entry(p);
    return map.find(entry) != map.end();
  }
  
  void process(const Todo<T> &todo)
  {        
    T *p = todo.data;
    if (! already_seen(p))
      {	
	HashEntry<T> entry(p);
	map[entry] = p;
	
	fprintf(stderr, "not already seen!\n");
	for (auto event : p->getEventVector())
	  {
	    stats.creations++;
	    T *clone = new T(*p);
	    clone->emit(event);

	    todo_stack.push(Todo<T>{clone});
	  }
      }
    else
      {
	fprintf(stderr, "already seen!\n");
	stats.duplicates++;
      }
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
	
