#ifndef TINY_VECTOR__H____
#define TINY_VECTOR__H____

#include <assert.h>
#include <stdlib.h>
#include <array>

template<typename T, size_t MAX_SIZE>
class tiny_vector
{
private:
 
  T data[MAX_SIZE];
  bool valid[MAX_SIZE];

public:
  tiny_vector()
  {
    for (unsigned i=0;i<MAX_SIZE;i++)
      {
	valid[i] = false;
      }
  }

  typedef T element_t;
  typedef std::array<T, MAX_SIZE> array;
  void export_to(array &a,
		 unsigned &count) const
  {
    count = 0;
    for (unsigned i=0;i<MAX_SIZE;i++)
      {
	if (valid[i])
	  {
	    a[count] = data[i];
	    count++;
	  }
      }
  }
   

  /** NOTE: assumes its not empty.
   */
  T &getRandomElement()
  {
    unsigned ix = rand() % MAX_SIZE;
    while (1)
      {
	if (is_valid(ix))
	  {
	    return at(ix);
	  }
	ix = (ix + 1)% MAX_SIZE;
      }
  }

  bool is_valid(unsigned i)
  {
    assert(i < MAX_SIZE);
    return valid[i];
  }
  
  size_t max_size() const { return MAX_SIZE; }
  
  void erase(unsigned i)
  {
    assert(i < MAX_SIZE);
    assert(valid[i]);
    valid[i] = false;
  }
  
  bool add(const T &e)
  {
    for (unsigned i = 0; i < MAX_SIZE; i++)
      {
	if (! valid[i])
	  {
	    data[i] = e;
	    valid[i] = true;
	    return true;
	  }
      }
    assert(false);
    return false;
  }

  T &at(unsigned ix)
  {
    assert(valid[ix]);
    return data[ix];
  }
};

#endif
