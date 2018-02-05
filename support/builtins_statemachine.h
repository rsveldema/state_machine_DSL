#ifndef STATE_MACHINE_BUILTINS__H____
#define STATE_MACHINE_BUILTINS__H____

#include <new> // import placement 'new'

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#include <vector>
#include <utility>
#include <string>

#include "units.h"

#if USE_MODELCHECK
#include <assert.h>
#define ASSERT(X) assert(X)
#elif USE_CUNIT
#include "support_cunit.h"
#else
#warning "UNHANDLED TEST SUITE"
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
void event_loop(ZEP::Utilities::Timeout &timeout,
		Machine *self)
{
  while (! timeout.hasElapsed())
    {
      self->process_delayed_events();
    }
}


template<class Machine>
void event_loop(const units::micros &us,
		Machine *self)
{
  ZEP::Utilities::Timeout timeout(us);
  event_loop(timeout, self);
}


#include "tracing.h"

std::string convertToString(int32_t value);

#endif
