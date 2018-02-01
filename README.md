
This project contains a light-weight state machine DSL.
A code generator generates C++ code from a state machine's description.
The generated C code is compact, and for use on embedded devices (STM32, PIC,
Kinetis, LPC, Nordic, etc. should all work using std. C++ compilers).

Example exerpt (more in the examples/ dir):

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

From the above example we:
     - generate a C++ class with the appropriate states,
     - a graphviz 


INSTALLATION:
	- install graphviz
	- install python 3
	- install antlr 4
	- install astyle
	  	  // used to make the generated code pretty,
		  // disable if you want
	- pip3 install antlr4-python3-runtime
	- pip3 install pystache
	- gcc g++
	- make

Antlr [http://www.antlr.org] then generates a parser from the DSL's grammar.
Hence, first install python and antlr4 using your platform's installers.

