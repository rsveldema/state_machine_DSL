#ifndef TINY_VECTOR__H____
#define TINY_VECTOR__H____

#include <assert.h>

template<typename T, size_t MAX_SIZE>
class tiny_vector
{
 private:
  T data[MAX_SIZE];
  unsigned count;
  
 public:
  tiny_vector()
    : count(0)
    {
    }

  void push_back(const T &e)
  {
    assert(count < MAX_SIZE);
    data[count] = e;
    count++;
  }

  struct iterator
  {
    unsigned ix;
    tiny_vector *self;
    
    iterator operator++()
    {
      unsigned last = ix;
      ix++;
      return {last, self};
    }

    bool operator != (const iterator &it) const
    {
      return ix != it.ix;
    }
    
    T operator *()
    {
      assert(self != 0);
      assert(ix < self->count);
      return self->data[ix];
    }    
  };

  iterator begin() { return {0, this}; }

  iterator end() { return {count, this}; }
};

#endif
