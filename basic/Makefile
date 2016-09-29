

all: basic-demo

basic-demo: lexer.o
	gcc -o $@ $^

*.o: *.c
	gcc -c -o $@ $< 

lexer.c: lexer.re
	re2c --input custom -o $@ $<