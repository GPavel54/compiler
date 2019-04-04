all: compiler.o Lexer.o functions.o compiler

compiler.o: src/compiler.cpp
	g++ -Wall -c -std=c++17 -o build/compiler.o src/compiler.cpp
Lexer.o: src/Lexer.cpp
	g++ -Wall -c -std=c++17 -o build/Lexer.o src/Lexer.cpp
functions.o:
	g++ -Wall -c -std=c++17 -o build/functions.o src/functions.cpp
compiler:
	gcc -Wall -std=c++17 -o bin/compiler build/compiler.o build/Lexer.o build/functions.o
clean:
	rm build/*.o
	rm bin/compiler
