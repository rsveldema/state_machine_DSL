#define USE_MODELCHECK 1

#include "builtins_statemachine.hpp"
#include "ModelCheckableStateMachine.hpp"

namespace tuple_comparers
{
  template<std::size_t I=0, typename... Tp>
  inline typename std::enable_if<I == sizeof...(Tp), bool>::type
  compare_equal(const std::tuple<Tp...>& t1,
		const std::tuple<Tp...>& t2)
  {
    return true;
  }

  template<std::size_t I=0, typename... Tp>
  inline typename std::enable_if<I < sizeof...(Tp), bool>::type
  compare_equal(const std::tuple<Tp...>& t1,
		const std::tuple<Tp...>& t2)
  {
    if (std::get<I>(t1) != std::get<I>(t2))
      {
	return false;
      }
    
    return compare_equal<I + 1, Tp...>(t1, t2);
  }
    
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
}

template<class BASE>
HashValue ModelCheckableStateMachine<BASE>::getHash() 
{
  HashValue hashValue;
  tuple_comparers::hasher(hashValue, this->fields);
  return hashValue;
}


template<class BASE>
bool ModelCheckableStateMachine<BASE>::equals(ModelCheckableStateMachine<BASE> &other,
					      bool ignore_deadline)
{
  return tuple_comparers::compare_equal(this->fields, other.fields);
}


template<class BASE>
bool ModelCheckableStateMachine<BASE>::operator < (const ModelCheckableStateMachine &other) const
{
  return tuple_comparers::compare_LT(this->fields, other.fields);
}
