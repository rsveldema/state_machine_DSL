
CXXFLAGS=-c -std=c++17 -g2 -I$(HOME)/CUnitHome/include -DUSE_CUNIT=1 -I. -I../support -MMD 

# comment line to conserve memory.
CXXFLAGS += -DSTATE_MACHINE_SUPPORT_TRACES

LDFLAGS_MC=$(LDFLAGS) #-fsanitize=address

LDFLAGS=-L$(HOME)/CUnitHome/lib/ -lcunit


all: power boiler blinky

power: test_power_manager model_check_power_manager
boiler: test_boiler model_check_boiler
blinky: test_simple_blinky test_blinky model_check_blinky


model_check_boiler: generated_test_boiler.o  model_check_boiler.o generated_state_machine_boiler.o ../model_checker/support_model_checker.o
	g++ $^ -o model_check_boiler $(LDFLAGS_MC)


model_check_blinky: generated_test_blinky.o  model_check_blinky.o generated_state_machine_blinky.o  ../model_checker/support_model_checker.o 
	g++ $^ -o model_check_blinky $(LDFLAGS_MC)

model_check_power_manager: generated_test_power_manager.o model_check_power_manager.o generated_state_machine_power_manager.o ../model_checker/support_model_checker.o
	g++ $^ -o model_check_power_manager $(LDFLAGS_MC)

generated_state_machine_power_manager.cpp generated_test_power_manager.cc: power_manager.sm
	@echo "------------ generate code"
	cpp power_manager.sm > /tmp/power_manager.sm
	python3 ../src/generate.py /tmp/power_manager.sm -path:../templates
	astyle generated_state_machine_power_manager.hpp
	astyle generated_test_power_manager.cc

generated_state_machine_blinky.cpp generated_test_blinky.cc: blinky.sm
	@echo "------------ generate code"
	cpp blinky.sm > /tmp/blinky.sm
	python3 ../src/generate.py /tmp/blinky.sm -path:../templates
	astyle generated_state_machine_blinky.hpp
	astyle generated_state_machine_blinky.cpp
	astyle generated_test_blinky.cc

generated_state_machine_simple_blinky.cpp generated_test_simple_blinky.cc: simple_blinky.sm
	@echo "------------ generate code"
	python3 ../src/generate.py simple_blinky.sm -path:../templates
	astyle generated_state_machine_simple_blinky.hpp
	astyle generated_state_machine_simple_blinky.cpp
	astyle generated_test_simple_blinky.cc

generated_state_machine_boiler.cpp generated_test_boiler.cc: boiler.sm
	@echo "------------ generate code"
	cpp boiler.sm > /tmp/boiler.sm
	python3 ../src/generate.py /tmp/boiler.sm -path:../templates
	astyle generated_state_machine_boiler.hpp
	astyle generated_test_boiler.cc

-include generated_test_blinky.d generated_test_power_manager.d	main_boiler.d support_statemachine.d generated_test_boiler.d	main_blinky.d main_power_manager.d

test_power_manager: generated_test_power_manager.o main_power_manager.o support_statemachine.o generated_state_machine_power_manager.o
	g++ -o test_power_manager $^ $(LDFLAGS)

test_boiler: generated_test_boiler.o main_boiler.o support_statemachine.o generated_state_machine_boiler.o
	g++ -o test_boiler $^ $(LDFLAGS)

test_blinky: generated_test_blinky.o main_blinky.o support_statemachine.o generated_state_machine_blinky.o
	g++ -o test_blinky $^ $(LDFLAGS)

test_simple_blinky: generated_test_simple_blinky.o main_simple_blinky.o support_statemachine.o  generated_state_machine_simple_blinky.o
	g++ -o test_simple_blinky $^ $(LDFLAGS)


clean:
	rm -f *.[oads] *~ *.tokens *Listener.py *Parser.py *Lexer.py
	rm -rf __pycache__ generated_* test_* CUnit* *.dot


