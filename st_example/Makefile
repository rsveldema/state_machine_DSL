

all: sorter

sorter: generated_state_machine_sorter.st

blinky: generated_state_machine_simple_blinky.st

generated_state_machine_sorter.st: ../../RDA-sorter/sorter.sm
	@echo "------------ generate code"
	python ../src/generate.py ../../RDA-sorter/sorter.sm -st -path:../templates
	mv *.dot dot/
	dot -Tpdf dot/sorter.dot  > ../../RDA-sorter/sorter.pdf

generated_state_machine_simple_blinky.st: simple_blinky.sm
	@echo "------------ generate code"
	python ../src/generate.py simple_blinky.sm -st -path:../templates
	mv *.dot dot/
	dot -Tpdf dot/simple_blinky.dot  > simple_blinky.pdf


install:
	cp generated_event_enum.st /c/Users/rove/Source/Repos/RDA-sorter/RDA-sorter/SortingPLC/DUTs/EventType.TcDUT
	cp generated_states_enum.st /c/Users/rove/Source/Repos/RDA-sorter/RDA-sorter/SortingPLC/DUTs/StateMachineStateNames.TcDUT
	cp generated_main.st /c/Users/rove/Source/Repos/RDA-sorter/RDA-sorter/SortingPLC/POUs/MAIN.TcPOU

clean:
	rm -f *.[oads] *~ *.tokens *Listener.py *Parser.py *Lexer.py
	rm -rf __pycache__ generated_* test_* CUnit* *.dot


