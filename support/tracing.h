#ifndef TRACING_H___H_
#define TRACING_H___H_

#include <circular_buffer.hpp>
#include <string.h>

class TraceEntry
{
 public:
  static const unsigned STR_SIZE = 32;
  units::micros time;
  char string[STR_SIZE];
  
 public:
 TraceEntry()
   : time(0)
    {
      strcpy(string, "");
    }
  
 TraceEntry(const char *s)
   : time(currentTimeMicros())
  {
    strncpy(string, s, sizeof(string));
  }
  
  bool contains(const char *s) const
  {
    return strstr(string, s) != NULL;
  }
};


class Trace
{
 private: 

  static const unsigned SIZE = 1024;
  circular_buffer<TraceEntry, SIZE> buffer;
  
 public:
  void add(const char *s)
  {
    buffer.add(TraceEntry(s));
  }
  
  bool contains(const char *str)
  {
    for (unsigned i = buffer.begin();
	 i != buffer.end();
	 i = buffer.next(i))
      {
	const TraceEntry &ent = buffer.get(i);
	if (ent.contains(str))
	  {
	    return true;
	  }
      }
    return false;
  }

  void dump()
  {
    fprintf(stderr, "-----------[ TRACE DUMP ]----------\n");
    for (unsigned i = buffer.begin();
	 i != buffer.end();
	 i = buffer.next(i))
      {
	const TraceEntry &ent = buffer.get(i);

	char buf[16];
	time2str(ent.time, buf, sizeof(buf));
	fprintf(stderr, "TRACE: %s, %s\n",
		buf,
		ent.string);
      }
    fprintf(stderr, "-----------[ END TRACE DUMP ]----------\n");
  }
};

#define SM_LOG(STRING)             { fprintf(stderr, "LOG: %s\n", STRING); }
#define SM_TRACE(STRING)           { self->trace.add(STRING); fprintf(stderr, "TRACE: %s\n", STRING); }


#endif
