all: compiler.o Lexer.o Parser.o CodeGen.o functions.o compiler

compiler.o: src/compiler.cpp
	mkdir -p build bin assembler
	g++ -Wall --std=c++14 -c -o build/compiler.o src/compiler.cpp -g
Lexer.o: src/Lexer.cpp
	g++ -Wall --std=c++14 -c -o build/Lexer.o src/Lexer.cpp -g
Parser.o: src/Parser.cpp
	g++ -Wall --std=c++14 -c -o build/Parser.o src/Parser.cpp -g
CodeGen.o: src/CodeGen.cpp
	g++ -Wall --std=c++14 -c -o build/CodeGen.o src/CodeGen.cpp -g
functions.o: src/functions.cpp
	g++ -Wall --std=c++14 -c -o build/functions.o src/functions.cpp -g
compiler:
	g++ -Wall --std=c++14 -o bin/compiler build/compiler.o build/Lexer.o build/Parser.o build/CodeGen.o build/functions.o -g
clean:
	rm build/*.o
	rm bin/compiler
build_tests:
	gcc -Wall -o run_all tests/main.c tests/alltests.c
ctests:
	rm branches*
	rm counting
	rm run_all
	rm string
	rm while
	rm ifarr
	rm assembler/*.o
