#include <tuple>
#include <functional>

#include <tracing.hpp>
#include "HashValue.hpp"



template<class BASE>
class ModelCheckableStateMachine : public DelayedEventsStateMachine<BASE>
{
public:
  static const unsigned MAX_TRACE_LEN = 32;

public:
  Trace<ModelCheckableStateMachine, MAX_TRACE_LEN, typename BASE::EVENT, typename BASE::STATES> trace;

  template<typename T>
  void addHash(HashValue &hashValue, T &a)
  {
    hashValue.add(a.getHash());
  }

  HashValue getHash() 
  {
    HashValue hashValue;    
    //auto result = std::apply(addHash, this->fields);    
    return hashValue;
  }
  
  std::string toString() const
  {
    std::string str("machine(");
    str += ")";
    return str;
  }
  

  bool operator == (const ModelCheckableStateMachine &other) const
  {
    fprintf(stderr, "OPERATOR ==!\n");
    return equals(other, true);
  }

  struct CompareEqual
  {
    bool equal = true;
  };

  struct CompareLT
  {
    bool lt = true;
  };

  template<typename T>
  static void compare_equal_func(CompareEqual &result, const T &t1, const T &t2)
  {
    if (t1 != t2)
      {
	result.equal = false;
      }
  }

  template<typename T>
  static void comparer_LT(CompareLT &result, T &t1, T &t2)
  {
    if (t1 != t2)
      {
	result.lt = false;
      }
  }

  
  template <class F, typename ARG, class Tuple, std::size_t... I>
  void apply_impl2(F f,
		   ARG& arg,
		   Tuple&& t1,
		   Tuple&& t2,
		   std::index_sequence<I ...>)
  {
    typedef decltype(std::get<I>(std::forward<Tuple>(t1))) CT;
    
    std::invoke(f<CT>, //std::forward<F>(f),
		arg,//std::forward<ARG>(arg),
		std::get<I>(std::forward<Tuple>(t1))...,
		std::get<I>(std::forward<Tuple>(t2))...
		);
  }
  
  template <class F, typename ARG, class Tuple>
  void apply2(F f,
	      ARG& arg,
	      Tuple&& t1,
	      Tuple&& t2)
  {
    apply_impl2(f, //std::forward<F>(f),
		arg, //std::forward<ARG>(arg),
		std::forward<Tuple>(t1),
		std::forward<Tuple>(t2),
		std::make_index_sequence<std::tuple_size_v<std::remove_reference_t<Tuple>>>{});
  }
  
  bool equals(const ModelCheckableStateMachine &other, bool ignore_deadline) const
  {
    CompareEqual result;
    apply2(compare_equal_func, result, this->fields, other.fields);
    return result.equal;
  }
  
  bool operator < (const ModelCheckableStateMachine &other) const
  {
    CompareLT result;
    //apply2(comparer_LT, result, this->fields, other.fields);
    return result.lt;
  }
  
public:
  static ModelCheckableStateMachine *getInstance()
  {
    return (ModelCheckableStateMachine *) get_thread_local_state_machine_ptr();
  }

  static void setInstance(ModelCheckableStateMachine *instance)
  {
    set_thread_local_state_machine_ptr(instance);
  }
    
  static void assertHook()
  {
    if (ModelCheckableStateMachine *sm = getInstance())
      {
	fprintf(stderr, "assert failed in state %s", sm->toString().c_str());
	sm->dump();
      }
    else
      {
	fprintf(stderr, "no current state machine available...\n");
      }
  }

  static void addAssertHook()
  {
    add_assert_hook(assertHook);
  }
  
};

