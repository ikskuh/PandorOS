

all: basic-demo

basic-demo: grammar.o lexer.o
	gcc -o $@ $^

grammar.o: grammar.c
	gcc -c -o $@ $< 
	
lexer.o: lexer.c grammar.h
	gcc -c -o $@ $< 

lexer.c: lexer.re
	re2c --input custom -o $@ $<

grammar.c: grammar.y
	lemon $^