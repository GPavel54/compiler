all: compiler.o Lexer.o Parser.o functions.o compiler

compiler.o: src/compiler.cpp
	mkdir -p build bin
	g++ -Wall --std=c++11 -c -o build/compiler.o src/compiler.cpp -g
Lexer.o: src/Lexer.cpp
	g++ -Wall --std=c++11 -c -o build/Lexer.o src/Lexer.cpp -g
Parser.o: src/Parser.cpp
	g++ -Wall --std=c++11 -c -o build/Parser.o src/Parser.cpp -g
functions.o: src/functions.cpp
	g++ -Wall --std=c++11 -c -o build/functions.o src/functions.cpp -g
compiler:
	g++ -Wall --std=c++11 -o bin/compiler build/compiler.o build/Lexer.o build/Parser.o build/functions.o -g
clean:
	rm build/*.o
	rm bin/compiler
