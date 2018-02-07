#ifndef TRACING_H___H_
#define TRACING_H___H_

#include <circular_buffer.hpp>
#include <string.h>


enum class TraceEntryType {
  NONE,
    EVENT,
    ENTER,
    LEAVE
};


template<typename event_t, typename state_t>
class TraceEntry
{
 public:
  units::micros time;
  TraceEntryType type;

  state_t state;
  event_t event;
  
 public:
 TraceEntry()
   : time(0),
     type(TraceEntryType::NONE)
    {
    }
  
 TraceEntry(state_t s, event_t e)
   : time(currentTimeMicros()),
     type(TraceEntryType::EVENT),
     state(s),
     event(e)
  {
  }

 TraceEntry(TraceEntryType t, state_t s)
   : time(currentTimeMicros()),
     type(t),
     state(s)
  {
  }
  
  bool has(const event_t &e) const {
    return event == e;
  }
  
  bool has(TraceEntryType t, const state_t &s) const {
    return type == t && state == s;
  }

  std::string toString()
  {
    switch (type)
      {
      case TraceEntryType::NONE: break;
      case TraceEntryType::EVENT: return "event<" + state_2_string(state) + ", " + event_2_string(event) + ">";
      case TraceEntryType::ENTER: return "enter<" + state_2_string(state) + ">";
      case TraceEntryType::LEAVE: return "leave<" + state_2_string(state) + ">";
      }    
  }
};


template<unsigned SIZE, typename event_t, typename state_t>
class Trace
{
 private:
  typedef TraceEntry<event_t, state_t> entry_t;
  
  circular_buffer<entry_t, SIZE> buffer;
  
public:
  void add(event_t e)
  {
    buffer.add(entry_t(e));
  }
  
  
  void add(TraceEntryType t, state_t e)
  {
    buffer.add(entry_t(t, e));
  }
  
  bool contains(TraceEntryType t, state_t e)
  {
    for (unsigned i = buffer.begin();
	 i != buffer.end();
	 i = buffer.next(i))
      {
	auto &ent = buffer.get(i);
	if (ent.has(t, e))
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
	auto &ent = buffer.get(i);
	fprintf(stderr, "LOG: %s\n", ent.toString().c_str());
      }
    fprintf(stderr, "-----------[ END TRACE DUMP ]----------\n");
  }
};


#if STATE_MACHINE_SUPPORT_TRACES
#define SM_TRACE_TRANSITION(TYPE, STATE)      { self->trace.add(TraceEntryType::TYPE, STATE); }
#define SM_TRACE_EVENT(EVENT)
#else
#define SM_TRACE_TRANSITION(TYPE, STATE)
#define SM_TRACE_EVENT(EVENT)
#endif


#endif
