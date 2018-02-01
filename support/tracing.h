#ifndef TRACING_H___H_
#define TRACING_H___H_

#include <circular_buffer.hpp>
#include <string>

class Trace
{
 private:
  struct TraceEntry
  {
    units::micros time;
    std::string   str;
    
    TraceEntry()
    : time(0)
    {
    }
    
  TraceEntry(const std::string &s)
  : time(currentTimeMicros()),
      str(s)
    {
    }
  };

  static const unsigned SIZE = 1024;
  circular_buffer<TraceEntry, SIZE> circular_buffer;
  
 public:
  void add(const std::string &s)
  {
    circular_buffer.add(TraceEntry(s));
  }
  
  bool contains(const char *str)
  {
    for (unsigned i = circular_buffer.begin(); i != circular_buffer.end(); i = circular_buffer.next(i))
      {
	const TraceEntry &ent = circular_buffer.get(i);
	if (ent.str.find(str) != std::string::npos)
	  {
	    return true;
	  }
      }
    return false;
  }

  void dump()
  {
    fprintf(stderr, "-----------[ TRACE DUMP ]----------\n");
    for (unsigned i = circular_buffer.begin(); i != circular_buffer.end(); i = circular_buffer.next(i))
      {
	const TraceEntry &ent = circular_buffer.get(i);
	fprintf(stderr, "TRACE: %s, %s\n", time2str(ent.time).c_str(), ent.str.c_str());
      }
    fprintf(stderr, "-----------[ END TRACE DUMP ]----------\n");
  }
};

#define SM_LOG(STRING)             { fprintf(stderr, "LOG: %s\n", STRING); }
#define SM_TRACE(STRING)           { self->trace.add(STRING); fprintf(stderr, "TRACE: %s\n", STRING); }


#endif
