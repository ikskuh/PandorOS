#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "lexer.h"

struct token lex(const char *input)
{
	int len = strlen(input);
	int marker;
	int ctxmarker;
	int index = 0;
#define YYCTYPE        char
#define YYPEEK()       input[index]
#define YYSKIP()       index++
#define YYBACKUP()     marker = index
#define YYBACKUPCTX()  ctxmarker = index
#define YYRESTORE()    index = marker
#define YYRESTORECTX() index = ctxmarker
#define YYLESSTHAN(n)  len - index < n
#define YYFILL(n)      {}
	/*!re2c
		re2c:define:YYCTYPE = char;
		re2c:yyfill:enable = 0;

		end = "\x00";
		bin = '0b' [01]+;
		oct = "0" [0-7]*;
		dec = [1-9][0-9]*;
		hex = '0x' [0-9a-fA-F]+;
		ws  = [ \t];
		var = 'str'[0-9] | [A-Z];
		ass = "->";

		*        { return (struct token){ 0, 0 }; }
		dec      { return (struct token){ TOK_INTEGER, index }; }
		hex      { return (struct token){ 2, index }; }
		"+"      { return (struct token){ TOK_PLUS, index }; }
		"-"      { return (struct token){ TOK_MINUS, index }; }
		"*"      { return (struct token){ TOK_TIMES, index }; }
		"/"      { return (struct token){ TOK_DIVIDE, index }; }
		"("      { return (struct token){ TOK_BRO, index }; }
		")"      { return (struct token){ TOK_BRC, index }; }
		ws+      { return (struct token){ -1, index }; }
		
	*/
}