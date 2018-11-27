#ifndef CIRCULAR_BUFFER__H___
#define CIRCULAR_BUFFER__H___


template<typename T, unsigned SIZE>
class circular_buffer
{
private:  
  unsigned rp, wp;
  T buffer[SIZE];
  
public:
  circular_buffer()
  {
    rp = wp = 0;
  }

  
  void add(const T e)
  {
    buffer[wp] = e;
    wp = next(wp);
  }

  unsigned begin() const { return rp; }
  unsigned end() const { return wp; }
  
  unsigned next(unsigned i)
  {
    ASSERT(i < SIZE);
    return (i + 1) % SIZE;
  }

  const T &get(unsigned i)
  {
    ASSERT(i < SIZE);
    return buffer[i];
  }  
};


#endif
