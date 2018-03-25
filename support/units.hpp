#ifndef UNITS__H____H_
#define UNITS__H____H_

#include <string>

namespace units
{
  class micros
  {
  private:
    // in micro-seconds:
    uint64_t value;
    
  public:
    micros(uint64_t _t)
      : value(_t)
    {
    }

    std::string toString() const
    {
      char buf[32];
      sprintf(buf, "%d.%d", (int)to_secs(), (int) to_millis() % 1000);
      return buf;
    }

    std::string toMicroString() const
      {	
	char buf[32];
	sprintf(buf, "%ld", (long) value);
	return buf;
      }

    uint64_t get() const { return value; }

    uint64_t to_secs() const
    {
      return value / (1000UL * 1000UL);
    }
    
    uint64_t to_millis() const
    {
      return value / 1000UL;
    }

    bool operator > (const micros &t) const
    {
      return value > t.value;
    }

    bool operator >= (const micros &t) const
    {
      return value >= t.value;
    }

    bool operator < (const micros &t) const
    {
      return value < t.value;
    }

    bool operator != (const micros &t) const
    {
      return value != t.value;
    }

    bool operator == (const micros &t) const
    {
      return value == t.value;
    }

    const micros operator + (const micros &t) const
    {
      return micros(value + t.value);
    }
    
    const micros operator - (const micros &t) const
    {
      return micros(value - t.value);
    }
  };
  

  class millis : public micros
  {
  public:
    millis(uint64_t c)
      : micros(c * 1000UL)
    {
    }
  };
  
  class secs : public millis
  {
  public:
    secs(uint64_t c)
      : millis(c * 1000UL)
      {
      }
  };
}

units::micros currentTimeMicros();
void time2str(const units::micros &micros,
	      char *buffer,
	      size_t buffer_size);


class Timeout
{
private:
  units::micros deadline;
  
public:
  Timeout()
    : deadline(0)
  {      
  }
      
  Timeout(const Timeout &t)
    : deadline(t.deadline)
  {
  }
  
  Timeout(const units::micros &time)
    : deadline(time + currentTimeMicros())
  {
  }
      
  Timeout &operator = (const Timeout &t)
  {
    this->deadline = t.deadline;
    return *this;
  }


  std::string toString() const
  {
    return "timeout:" + deadline.toString();
  }

      
  bool hasElapsed() const
  {
    const units::micros &now = currentTimeMicros();
    return now > deadline;
  }
      
  units::micros get() const { return deadline; }
      
  bool operator < (const Timeout &t2) const
  {
    return get() < t2.get();
  }

  bool operator >= (const Timeout &t2) const
  {
    return get() >= t2.get();
  }

  bool operator != (const Timeout &t2) const
  {
    return get() != t2.get();
  }

  bool operator == (const Timeout &t2) const
  {
    return get() == t2.get();
  }

  bool operator > (const Timeout &t2) const
  {
    return get() > t2.get();
  }
};

 


#endif
