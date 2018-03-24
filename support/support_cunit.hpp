#ifndef SUPPORT_CUNIT_H_H_H_H___
#define SUPPORT_CUNIT_H_H_H_H___

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

#endif
