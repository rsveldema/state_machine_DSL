
This project contains a light-weight state machine DSL.  A code
generator generates C++ code from a state machine's description.  The
generated C code is compact, and for use on embedded devices (STM32,
PIC, Arduino, Raspberry PI, Kinetis, LPC, Nordic, etc. should all work
using std. C++ compilers).

The DSL also contains facilities for writing unit tests and for
automatic model checking.

## Example ##

``` C++
machine Blinky {
  // declare events to stimulate the machine:
  event b1 "button 1 pressed";
  
  // declare C/C++ vars:
  Led led1;

  initial state bootup {
    entry {
          // C extension in our DSL:
	  transition blink1;
  } }

  state blink1 {
    entry { /* write C code here */ }
    exit  { /* more C code */ }
	
    event b1 {
      // write more C code if b1 is triggered, i.e.:
      printf("ignoring button1 press, already in b1\n");
      transition blink2;
    }   
  ...
```

From the above example we can:
     * generate a C++ class with the appropriate states as nested classes.
     * a graphviz diagram
     * generate CUnit tests
     * test that the transition statement is the last statement in the code
       (to avoid illegal accessing the prior state after a state transition).

## Unit testing ##

After your model, you can optionally specify a number of tests to run:

``` C++
testsuite buttonBoiling {
  test simple {
    transition bootup;
    wait millis(500); // wait for boot to complete

    emit button_on;
    wait secs(1); // wait for event to be processed

    emit button_start;
    wait secs(6); // wait for boiling water

    assert self->trace.contains("ENTER STATE: finish_boiling");
    self->trace.dump();
  }
}
```

## Model checking ##

A simple explicit state model checker is included. It attempts all
possible interleavings of external events sent to the machine.
For model-checking we suggest to use stubbed/mocked devices if your model
interacts with devices.

``` C++
#include "generated_state_machine_boiler.hpp"
#include "../model_checker/model_check.h"

int main(int argc, char **argv)
{
  WaterBoiler *p = new WaterBoiler();
  
  // from this state onwards we want to
  // try all possible event permutations sent:
  p->transition(p->state_union.bootup);

  // instantiate the model checker and run the test:
  Modelchecker<WaterBoiler> mc(p);
  mc.run();
  return 0;
}
```


## Installation ##

	* install graphviz
	* install python 3
	* install antlr 4
	* install cunit
	  [ http://cunit.sourceforge.net ]
	* install astyle
	  	  // used to make the generated code pretty,
		  // disable if you want.
	* pip3 install antlr4-python3-runtime
	* pip3 install pystache
	* gcc g++
	* make

Antlr [http://www.antlr.org] then generates a parser from the DSL's grammar.
Hence, first install python and antlr4 using your platform's installers.

