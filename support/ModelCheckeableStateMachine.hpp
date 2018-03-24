

#include "HashValue.hpp"


template<typename Event, typename StateEnum>
class ModelCheckableStateMachine : public DelayedEventsStateMachine<Event, StateEnum>
{
public:
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

 public:
  static void addAssertHook()
  {
    add_assert_hook(assertHook);
  }
  
  HashValue getHash() const {
    HashValue hashValue = this->delayed_events_stack.getHash();
    hashValue.add((uint64_t) this->state << 20);

    return hashValue;
  }
  
  std::string toString() const;
  

  bool operator == (const ModelCheckableStateMachine &other) const
  {
    fprintf(stderr, "OPERATOR ==!\n");
    return equals(other, true);
  }
  
  bool equals(const ModelCheckableStateMachine &other, bool ignore_deadline) const
  {
    return false;
  }

  
  bool operator < (const ModelCheckableStateMachine &other) const
  {
    return false;
  }
  
public:
  static ModelCheckableStateMachine *getInstance()
  {
    return (ModelCheckableStateMachine *) get_thread_local_state_machine_ptr();
  }

  static void setInstance(ModelCheckableStateMachine *instance)
  {
    set_thread_local_state_machine_ptr(instance);
  }
};

