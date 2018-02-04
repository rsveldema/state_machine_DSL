#include <map>
#include <stack>

units::micros currentTimeMicros()
{
  static uint64_t t;
  return t++;
}

namespace ZEP
{
  namespace Utilities
  {
  }
}


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
	    T *clone = new T(*p);
	    clone->emit(event);

	    todo_stack.push(Todo<T>{clone});
	  }
      }
    else
      {
	fprintf(stderr, "already seen!\n");
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
  }
};
	
