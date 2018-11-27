#ifndef MODEL_CHECKABLE_STATE_MACHINE_H__
#define MODEL_CHECKABLE_STATE_MACHINE_H__

#include <tuple>
#include <functional>

#include "../support/tracing.hpp"
#include "HashValue.hpp"

#include "DelayedEventsStateMachine.hpp"

template<class BASE>
class ModelCheckableStateMachine : public DelayedEventsStateMachine<BASE>
{
public:
  static const unsigned MAX_TRACE_LEN = 32;

public:
  Trace<ModelCheckableStateMachine, MAX_TRACE_LEN, typename BASE::EVENT, typename BASE::STATES> trace;
  
  HashValue getHash();
  
  std::string toString() const
  {
    std::string str("machine(");
    str += state_2_string(this->state);
    str += ")";
    return str;
  }
  

  bool operator == (const ModelCheckableStateMachine &other) const
  {
    return equals(other, true);
  }
  bool operator != (const ModelCheckableStateMachine &other) const
  {
    return !equals(other, true);
  }
    
  bool equals(const ModelCheckableStateMachine &other, bool ignore_deadline) const;  
  bool operator < (const ModelCheckableStateMachine &other) const;
  
public:
  static ModelCheckableStateMachine *getInstance()
  {
    return (ModelCheckableStateMachine *) get_thread_local_state_machine_ptr();
  }

  static void setInstance(ModelCheckableStateMachine *instance)
  {
    set_thread_local_state_machine_ptr(instance);
  }
    
  static void assertHook()
  {
    if (ModelCheckableStateMachine *sm = getInstance())
      {
	fprintf(stderr, "assert failed in state %s", sm->toString().c_str());
	sm->dump();
      }
    else
      {
	fprintf(stderr, "no current state machine available...\n");
      }
  }

  static void addAssertHook()
  {
    add_assert_hook(assertHook);
  }
  
  // called from do_emit
  virtual void notify_event_emit(const typename DelayedEventsStateMachine<BASE>::Event &event) override
  {
    setInstance(this);
  }
};


#endif
