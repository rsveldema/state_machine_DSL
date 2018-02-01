//------------------
//------------------
//------------------
// NOTE: generated from main.template.cc
//------------------
//------------------
//------------------

#include "builtins_statemachine.h"
#include "support_statemachine.h"

#include "generated_state_machine_blinky.hpp"


int main(int argc, char **argv)
{
  int err1 = CU_initialize_registry();
  if (err1 != CUE_SUCCESS) {
    fprintf(stderr, "failed to initialize the CU_init library\n");
    abort();
  }
  
  registerTests_blinky();
  printf("run tests!\n");
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  CU_cleanup_registry();
  return 0;
}
