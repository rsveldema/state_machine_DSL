
// provided by programmer in support of tests/boiler.sm
// included by generated code.

#include "HashValue.h"

class PersistentStorage
{
  static const unsigned SIZE = 1;
  bool data[SIZE];
  
 public:
  PersistentStorage()
    {
      for (unsigned i=0;i<SIZE;i++)
	{
	  data[i] = false;
	}
    }
  
  bool should_restore_from_FLASH()
  {
    return data[0];
  }

  void notify_stored_in_FLASH()
  {
    ASSERT(! data[0]);
    data[0] = true;
  }

  void notify_restored_from_FLASH()
  {
    ASSERT(data[0]);
    data[0] = false;
  }

  HashValue getHash() const { return data[0]; }

  bool operator != (const PersistentStorage &s) const
  {
    for (unsigned i = 0; i < SIZE; i++)
      {
	if (data[i] != s.data[i])
	  {
	    return true;
	  }
      }
    return false;
  }
  
  bool operator < (const PersistentStorage &s) const
  {
    for (unsigned i = 0; i < SIZE; i++)
      {
	if (data[i] >= s.data[i])
	  {
	    return false;
	  }
      }
    return true;
  }
};


class Environment
{
 private:
  unsigned counter;
  static const unsigned MAX = 10;
  
 public:
  void notify_start_suspend()
  {
    counter = 0;
  }
  
  bool need_wakeup()
  {
    counter++;
    if (counter == MAX)
      {
	counter = 0;
	return true;
      }
    return false;
  }

  HashValue getHash() const { return counter; }

  bool operator != (const Environment &s) const
  {
    return counter != s.counter;
  }
  
  bool operator < (const Environment &s) const
  {
    if (counter >= s.counter)
      {
	return false;
      }
    return true;
  }
};

class SuspendToFlashStrategy
{
 private:
  unsigned counter;
  static const unsigned MAX = 10;
  
 public:
  void init()
  {
    counter = 0;
  }

  bool shouldSuspendToFLASH()
  {
    counter++;
    if (counter == MAX)
      {
	return true;
      }
    return false;
  }

  bool operator != (const SuspendToFlashStrategy &s) const
  {
    return counter != s.counter;
  }
    

  bool operator < (const SuspendToFlashStrategy &s) const
  {
    return counter < s.counter;
  }

  std::string toString() const
    {
      char buf[32];
      sprintf(buf, "suspend2flash(%d of %d)", counter, MAX);
      return buf;
    }
};


static inline
std::string convertToString(const SuspendToFlashStrategy &s)
{
  return s.toString();
}
