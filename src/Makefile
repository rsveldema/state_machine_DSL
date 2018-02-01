
all: generate_parser 

generate_parser:
	@echo "------------ generate parser"
	rm -f *.dot
	antlr4  -Dlanguage=Python3 dsl.antlr4

install:
	sudo pip3 install antlr4-python3-runtime


clean:
	rm -f *.[oads] *~ *.tokens *Listener.py *Parser.py *Lexer.py
	rm -rf __pycache__ generated_* test_* CUnit* *.dot


