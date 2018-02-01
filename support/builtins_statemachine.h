#ifndef STATE_MACHINE_BUILTINS__H____
#define STATE_MACHINE_BUILTINS__H____

#include <new> // import placement 'new'

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#include <vector>
#include <utility>

#include "units.h"

#if USE_CUNIT

#include <CUnit/CUnit.h>

#define CREATE_TEST_SUITE(init, cleanup, name) ({		 \
      CU_pSuite suite = CU_add_suite(name, init, cleanup);	 \
      if (suite == 0) {						 \
	fprintf(stderr, "failed to add test-suite!\n");		 \
	abort();						 \
      }								 \
      suite;							 \
    })


#define REGISTER_TEST_IN_SUITE(suite, func, name) {			\
    CU_pTest err = CU_add_test(suite, name, func);			\
    if (err == 0)  {							\
      fprintf(stderr, "failed to register test in test-suite!\n");	\
      abort();								\
    }									\
  }


#define REGISTER_TEST_SUITE(suite) {		\
}

#define ASSERT(X)  CU_ASSERT(X)

#else
#define "UNHANDLED TEST SUITE"
#include <assert.h>
#define ASSERT(X) assert(x)
#endif

#define WAIT(SEC)         event_loop(SEC, self)


#define STATE_BAD_EVENT_HANDLER(name, event) {			\
    fprintf(stderr, "ERROR: bad event object passed\n");	\
    ASSERT(0);						\
  }

#define STATE_MISSING_EVENT_HANDLER(state, event) {			\
    fprintf(stderr, "ERROR: missing event hander '%s' in state '%s'\n", event, state); \
    ASSERT(0);							\
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

#endif
