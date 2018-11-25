#ifndef UNITS__H____H_
#define UNITS__H____H_

#include <string>
#include <chrono>


std::chrono::microseconds currentTimeMicros();

void time2str(const std::chrono::microseconds &micros,
	      char *buffer,
	      size_t buffer_size);


class Timeout
{
private:
	std::chrono::microseconds deadline;
  
public:
  Timeout()
    : deadline(0)
  {      
  }
      
  Timeout(const Timeout &t)
    : deadline(t.deadline)
  {
  }
  
  Timeout(const std::chrono::microseconds &time)
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
	char buf[64];
	snprintf(buf, sizeof(buf), "timeout %ld micros", (unsigned long) deadline.count());
	return buf;
  }

      
  bool hasElapsed() const
  {
	auto  now = currentTimeMicros();
    return now > deadline;
  }
      
  std::chrono::microseconds get() const { return deadline; }
      
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
