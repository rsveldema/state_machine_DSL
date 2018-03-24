

#include <tracing.hpp>

template<typename Event, typename StateEnum>
class StateTracingStateMachine : public AbstractStateMachine<Event, StateEnum>
{
  static const unsigned MAX_TRACE_LEN = 32;

 public:
  Trace<StateTracingStateMachine, MAX_TRACE_LEN, typename Event::EventEnum_t, StateEnum> trace;
};


