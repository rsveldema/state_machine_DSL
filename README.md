
This project contains a state machine DSL.
A code generator generates C++ code from a state machine's description.
The generated C code is compact, and for use on embedded devices (STM32, PIC,
Kinetis, LPC, Nordic, etc. should all work using std. C++ compilers).

Antlr [http://www.antlr.org] then generates a parser from the DSL's grammar.
Hence, first install python and antlr4 using your platform's installers.
Then execute:
   pip3 install antlr4-python3-runtime


Example exerpt (more in the examples/ dir).

machine Blinky {

  event b1 "button 1 pressed";
  event b2 "button 2 pressed";
  Led led1;

  initial state bootup {
    entry {
      transition blink1;
    }
  }

  state blink1 {
    entry {
      // write C code here 
    }
    
    event b1 {
      // write more C code if b1 is triggered, i.e.:
      printf("ignoring button1 press, already in b1\n");
    }
    
    event b2 {
      // transition statement to another state so
      // it services the incoming events next.
      transition blink2;
    }
    
    exit {
      // more c code here, called as an effect of the transition statement
      led1.on();
    }
  }

  state blink2 {
    ...
  }
}