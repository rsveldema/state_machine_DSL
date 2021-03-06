#define USE_MODELCHECK 1

#include "builtins_statemachine.hpp"
#include "ModelCheckableStateMachine.hpp"

namespace tuple_comparers
{
  template<std::size_t I=0, typename... Tp>
  inline typename std::enable_if<I == sizeof...(Tp), bool>::type
  compare_fields_tuple(const std::tuple<Tp...>& t1,
		const std::tuple<Tp...>& t2)
  {
    return true;
  }

  template<std::size_t I=0, typename... Tp>
  inline typename std::enable_if<I < sizeof...(Tp), bool>::type
  compare_fields_tuple(const std::tuple<Tp...>& t1,
		const std::tuple<Tp...>& t2)
  {
    if (std::get<I>(t1) != std::get<I>(t2))
      {
	return false;
      }
    
    return compare_fields_tuple<I + 1, Tp...>(t1, t2);
  }


  template<std::size_t I=0, typename... Tp>
  inline typename std::enable_if<I == sizeof...(Tp), bool>::type
  equal_states(const std::tuple<Tp...>& t1,
	       const std::tuple<Tp...>& t2)
  {
    return true;
  }

    
  template<std::size_t I=0, typename... Tp>
  inline typename std::enable_if<I < sizeof...(Tp), bool>::type
  equal_states(const std::tuple<Tp...>& t1,
	       const std::tuple<Tp...>& t2)
  {
    auto p1 = std::get<I>(t1);
    auto p2 = std::get<I>(t1);
    if (p1)
      {
	assert(p2 != NULL);
	// we have both state pointers!
	return compare_fields_tuple(p1->fields,
			     p2->fields);
      }
    else
      {
	assert(p2 == NULL);
      }
    return equal_states<I+1, Tp...>(t1, t2);
  }
    
    
  // =======================================================
    
  template<std::size_t I=0, typename... Tp>
  inline typename std::enable_if<I == sizeof...(Tp), bool>::type
  compare_LT(const std::tuple<Tp...>& t1,
	     const std::tuple<Tp...>& t2)
  {
    return true;
  }

  template<std::size_t I=0, typename... Tp>
  inline typename std::enable_if<I < sizeof...(Tp), bool>::type
  compare_LT(const std::tuple<Tp...>& t1,
	     const std::tuple<Tp...>& t2)
  {
    if (std::get<I>(t1) >= std::get<I>(t2))
      {
	return false;
      }
    
    return compare_LT<I + 1, Tp...>(t1, t2);
  }
    
  template<std::size_t I=0, typename... Tp>
  inline typename std::enable_if<I == sizeof...(Tp), bool>::type
  states_compare_LT(const std::tuple<Tp...>& t1,
		    const std::tuple<Tp...>& t2)
  {
    return true;
  }

  template<std::size_t I=0, typename... Tp>
  inline typename std::enable_if<I < sizeof...(Tp), bool>::type
  states_compare_LT(const std::tuple<Tp...>& t1,
		    const std::tuple<Tp...>& t2)
  {
    auto p1 = std::get<I>(t1);
    auto p2 = std::get<I>(t2);
    
    if (p1)
      {
	assert(p2);
	return compare_LT(p1->fields,
			  p2->fields);
      }
    else
      {
	assert(p2 == NULL);
      }
    
    return states_compare_LT<I + 1, Tp...>(t1, t2);
  }

  // =============================================================

  void addHash(HashValue &hashValue, const int32_t t)
  {
    hashValue.add(t);
  }
  void addHash(HashValue &hashValue, const int64_t t)
  {
    hashValue.add(t);
  }
  void addHash(HashValue &hashValue, const double t)
  {
    hashValue.add(t);
  }

    template<typename T>
    void addHash(HashValue &hashValue, T &t)
    {
      hashValue.add(t.getHash());
    }

  
    
  template<std::size_t I=0, typename... Tp>
  inline typename std::enable_if<I == sizeof...(Tp), void>::type
  hasher(HashValue &hashValue,
	 const std::tuple<Tp...>& t1)
  {
  }

  template<std::size_t I=0, typename... Tp>
  inline typename std::enable_if<I < sizeof...(Tp), void>::type
  hasher(HashValue &hashValue,
	 const std::tuple<Tp...>& t1)	 
  {
    addHash(hashValue, std::get<I>(t1));
    hasher<I + 1, Tp...>(hashValue, t1);
  }
    
  template<std::size_t I=0, typename... Tp>
  inline typename std::enable_if<I == sizeof...(Tp), void>::type
  state_hasher(HashValue &hashValue,
	       const std::tuple<Tp...>& t1)
  {
  }

  template<std::size_t I=0, typename... Tp>
  inline typename std::enable_if<I < sizeof...(Tp), void>::type
  state_hasher(HashValue &hashValue,
	 const std::tuple<Tp...>& t1)	 
  {
    if (auto p1 = std::get<I>(t1))
      {
	hasher(hashValue, p1->fields);
	return;
      }    
    state_hasher<I + 1, Tp...>(hashValue, t1);
  }

}

template<class BASE>
HashValue ModelCheckableStateMachine<BASE>::getHash() 
{
  HashValue hashValue;
  hashValue.add((int)this->state);
  tuple_comparers::state_hasher(hashValue, this->getStateVector());
  tuple_comparers::hasher(hashValue, this->fields);
  return hashValue;
}

template<typename T>
int comparer(T &a,
	     T &b)
{
}


template<class BASE>
bool ModelCheckableStateMachine<BASE>::equals(const ModelCheckableStateMachine<BASE> &other,
					      bool ignore_deadline) const
{
  if (this->state != other.state)
    {
      return false;
    }

  if (! tuple_comparers::equal_states(this->getStateVector(),
				      other.getStateVector()))
    {
      return false;
    }
  
  if (! tuple_comparers::compare_fields_tuple(this->fields, other.fields))
    {
      return false;
    }


  typedef typename DelayedEventsStateMachine<BASE>::delayed_stack_t stack_t;
  unsigned count1 = 0;
  typename stack_t::array events1;
  
  unsigned count2 = 0;
  typename stack_t::array events2;
    
  this->delayed_events_stack.export_to(events1, count1);
  other.delayed_events_stack.export_to(events2, count2);

  if (count1 != count2)
    {
      return false;
    }
    
  std::sort(events1.begin(), events1.begin() + count1);
  std::sort(events2.begin(), events2.begin() + count2);

  for (unsigned i=0;i<count1;i++)
    {
      if (events1[i] != events2[i])
	{
	  return false;
	}
    }
  return true;
}


template<class BASE>
bool ModelCheckableStateMachine<BASE>::operator < (const ModelCheckableStateMachine &other) const
{
  if (this->state >= other.state)
    {
      return false;
    }

  if (this->state == other.state)
    {
      if (! tuple_comparers::states_compare_LT(this->getStateVector(),
					       other.getStateVector()))
	{
	  return false;
	}
      
      if (tuple_comparers::compare_LT(this->fields, other.fields))
	{
	  return true;
	}

      typedef typename DelayedEventsStateMachine<BASE>::delayed_stack_t stack_t;
      unsigned count1 = 0;
      typename stack_t::array events1;
  
      unsigned count2 = 0;
      typename stack_t::array events2;
    
      this->delayed_events_stack.export_to(events1, count1);
      other.delayed_events_stack.export_to(events2, count2);

      if (count1 < count2)
	{
	  return true;
	}
      else if (count1 == count2)
	{
	  std::sort(events1.begin(), events1.begin() + count1);
	  std::sort(events2.begin(), events2.begin() + count2);
	  
	  for (unsigned i=0;i<count1;i++)
	    {
	      if (events1[i] >= events2[i])
		{
		  return false;
		}
	    }
	}
    }
    
  assert(this->state < other.state);  
  return true;
}
