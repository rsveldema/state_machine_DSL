#ifndef STATE_MACHINE_BUILTINS__H____
#define STATE_MACHINE_BUILTINS__H____

#include <new> // import placement 'new'

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#include <vector>
#include <utility>
#include <string>

#include "units.hpp"

#if USE_MODELCHECK
#include <assert.h>
void model_check_assert(int line, const char *file, const char *msg);
#define ASSERT(X) {if (! (X)) { model_check_assert(__LINE__, __FILE__, #X); }}
#elif USE_CUNIT
#include "support_cunit.hpp"
#else
//#warning "UNHANDLED TEST SUITE"
#include <assert.h>
#define ASSERT(X) assert(X)
#endif

#define WAIT(SEC)         event_loop(SEC, self)


#define STATE_BAD_EVENT_HANDLER(name, event) {			\
    fprintf(stderr, "ERROR: bad event object passed\n");	\
    ASSERT(0);						\
  }

#define STATE_MISSING_EVENT_HANDLER(state, event) {			\
    fprintf(stderr, "ERROR: missing event hander '%s' in state '%s'\n", \
	    event, state);						\
    ASSERT(0);								\
  }



template<class Machine>
void event_loop(Timeout &timeout,
		Machine *self)
{
  while (! timeout.hasElapsed())
    {
      self->process_delayed_events();
    }
}


template<class Machine>
void event_loop(const std::chrono::microseconds &us,
		Machine *self)
{
  Timeout timeout(us);
  event_loop(timeout, self);
}


#include "tracing.hpp"

std::string convertToString(int32_t value);

void add_assert_hook(void (*assertHook)());
void set_thread_local_state_machine_ptr(void *ptr);
void *get_thread_local_state_machine_ptr();

#endif
