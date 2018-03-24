#include "generated_state_machine_simple_blinky.hpp" 

#include "../model_checker/model_check.h"
#include "../model_checker/random_path_checker.h"	

#include <sys/time.h>

void Usage()
{
  fprintf(stderr, "Usage: [-rpath|-seed <number|-exhaustive|-maxseqlen <count>]\n");
  exit(1);
}

uint64_t get_current_time_micros()
{
  timeval tv;
  gettimeofday(&tv, NULL);
  return tv.tv_sec + (tv.tv_usec * 1000 * 1000LL);
}

int main(int argc, char **argv)
{
  bool random_path = true;
  bool use_statistical_analysis = true;
  int max_step_count_per_machine = 10;
  unsigned seed = get_current_time_micros();

  for (int i = 1; i < argc; i++)
    {
      if (strcmp(argv[i], "-rpath") == 0)
	{
	  random_path = true;
	}
      else if (strcmp(argv[i], "-seed") == 0)
	{
	  random_path = true;
	  i++;
	  seed = atoi(argv[i]);
	}
      else if (strcmp(argv[i], "-exhaustive") == 0)
	{
	  use_statistical_analysis = false;
	}
      else if (strcmp(argv[i], "-maxseqlen") == 0)
	{
	  i++;
	  max_step_count_per_machine = atoi(argv[i]);
	}
      else
	{
	  fprintf(stderr, "unrecognized parameter '%s'\n", argv[i]);
	  Usage();
	}
    }

  fprintf(stderr, "exhaustive search: %s\n",
	  use_statistical_analysis ? "disabled" : "enabled");
  fprintf(stderr, "max sequence length per machine: %d\n",
	  max_step_count_per_machine);
  fprintf(stderr, "seed = %u\n", seed);
  srand(seed);
  
  Blinky::addAssertHook();
  
  Blinky *p = new Blinky();
  
  p->initial_transition(p->state_union.bootup);

  if (random_path)
    {
      RandomPathChecker<Blinky> rc(p, max_step_count_per_machine);
      rc.run();
    }
  else
    {
      Modelchecker<Blinky> mc(p, use_statistical_analysis, max_step_count_per_machine);
      mc.run();
    }
  return 0;
}
