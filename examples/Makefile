
CXXFLAGS=-c -std=c++11 -g2 -I$(HOME)/CUnitHome/include -DUSE_CUNIT=1 -I. -I../support -MMD

# comment line to conserve memory.
CXXFLAGS += -DSTATE_MACHINE_SUPPORT_TRACES

CXXFLAGS_MC=$(CXXFLAGS) -DUSE_MODELCHECK #-fsanitize=address
LDFLAGS_MC=$(LDFLAGS) #-fsanitize=address

LDFLAGS=-L$(HOME)/CUnitHome/lib/ -lcunit

all: test_boiler test_power_manager test_blinky model_check_boiler model_check_blinky model_check_power_manager

model_check_boiler: generated_test_boiler.cc
	g++ $(CXXFLAGS_MC) model_check_boiler.cc ../model_checker/support_model_checker.cc 
	g++ model_check_boiler.o support_model_checker.o -o model_check_boiler $(LDFLAGS_MC)


model_check_blinky: generated_test_blinky.cc
	g++ $(CXXFLAGS_MC) model_check_blinky.cc ../model_checker/support_model_checker.cc 
	g++ model_check_blinky.o support_model_checker.o -o model_check_blinky $(LDFLAGS_MC)


model_check_power_manager: generated_test_power_manager.cc
	g++ $(CXXFLAGS_MC) model_check_power_manager.cc ../model_checker/support_model_checker.cc 
	g++ model_check_power_manager.o support_model_checker.o -o model_check_power_manager $(LDFLAGS_MC)

generated_test_power_manager.cc: power_manager.sm
	@echo "------------ generate code"
	cpp power_manager.sm > /tmp/power_manager.sm
	python3 ../src/generate.py /tmp/power_manager.sm -path:../templates
	astyle generated_state_machine_power_manager.hpp
	astyle generated_test_power_manager.cc

generated_test_blinky.cc: blinky.sm
	@echo "------------ generate code"
	cpp blinky.sm > /tmp/blinky.sm
	python3 ../src/generate.py /tmp/blinky.sm -path:../templates
	astyle generated_state_machine_blinky.hpp
	astyle generated_test_blinky.cc

generated_test_boiler.cc: boiler.sm
	@echo "------------ generate code"
	cpp boiler.sm > /tmp/boiler.sm
	python3 ../src/generate.py /tmp/boiler.sm -path:../templates
	astyle generated_state_machine_boiler.hpp
	astyle generated_test_boiler.cc

-include generated_test_blinky.d generated_test_power_manager.d	main_boiler.d support_statemachine.d generated_test_boiler.d	main_blinky.d main_power_manager.d

test_power_manager: generated_test_power_manager.o main_power_manager.o support_statemachine.o
	g++ -o test_power_manager main_power_manager.o generated_test_power_manager.o support_statemachine.o $(LDFLAGS)

test_boiler: generated_test_boiler.o main_boiler.o support_statemachine.o
	g++ -o test_boiler main_boiler.o generated_test_boiler.o support_statemachine.o $(LDFLAGS)


test_blinky: generated_test_blinky.o main_blinky.o support_statemachine.o
	g++ -o test_blinky main_blinky.o generated_test_blinky.o support_statemachine.o $(LDFLAGS)


clean:
	rm -f *.[oads] *~ *.tokens *Listener.py *Parser.py *Lexer.py
	rm -rf __pycache__ generated_* test_* CUnit* *.dot


