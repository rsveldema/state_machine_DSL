#include "units.hpp"
#include <assert.h>

template<typename EVENT_ENUM, typename STATE_ENUM>
class AbstractStateMachine
{
public:
  typedef EVENT_ENUM EVENT;
  typedef STATE_ENUM STATES;

  STATES state;
  
  class Event
  {
  private:
    typedef EVENT EventEnum_t;
    EVENT type;
    int64_t payload;
    
  public:
    Event()
      : type(EVENT::EVENT_NONE),
	payload(0)
    {
    }
    
    Event(EVENT _type)
      : type (_type),
	payload(0)
    {
    }

    const char *toString() const {
      return eventToString(type);      
    }
    
    EVENT getType() const { return type; }
    int64_t getPayload() const { return payload; }
    
    bool operator != (const Event &ev) const
    {
      return type != ev.type || payload != ev.payload;
    }
  };

  virtual void dump() {}
  virtual void do_emit(const Event &event) = 0;
  virtual bool emit(const Event &event, const Timeout &timeout)
  {
    assert(false); // unimplemented in base class.
  }
  virtual void emit(const Event &event)
  {
    do_emit(event);
  }
};

