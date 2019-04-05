all: compiler.o Lexer.o functions.o compiler

compiler.o: src/compiler.cpp
	g++ -Wall -c -o build/compiler.o src/compiler.cpp -g
Lexer.o: src/Lexer.cpp
	g++ -Wall -c -o build/Lexer.o src/Lexer.cpp -g
functions.o: src/functions.cpp
	g++ -Wall -c -o build/functions.o src/functions.cpp -g
compiler:
	g++ -Wall -o bin/compiler build/compiler.o build/Lexer.o build/functions.o
clean:
	rm build/*.o
	rm bin/compiler
