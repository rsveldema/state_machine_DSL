

all: blinky

blinky: test_simple_blinky 

generated_state_machine_simple_blinky.st: simple_blinky.sm
	@echo "------------ generate code"
	python3 ../src/generate.py simple_blinky.sm -st -path:../templates
	mv *.dot dot/
	dot -Tpdf dot/simple_blinky.dot  > simple_blinky.pdf

test_simple_blinky: generated_state_machine_simple_blinky.st
	echo "done"

clean:
	rm -f *.[oads] *~ *.tokens *Listener.py *Parser.py *Lexer.py
	rm -rf __pycache__ generated_* test_* CUnit* *.dot


