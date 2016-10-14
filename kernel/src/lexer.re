#include "basic/lexer.h"
#include "string.h"
#include "memory.h"
#include "debug.h"
#include <stdbool.h>

struct token lex(const char *input)
{
	int len = str_len(input);
	int marker;
	int ctxmarker;
	int index = 0;
	
	// Mark all variables as used
	(void)len;
	(void)marker;
	(void)ctxmarker;
	(void)index;
	
	if(*input == '\"')
	{
		int i;
		for(i = 1; input[i] != 0 && input[i] != '\"'; i++);
		if(input[i] == 0) {
			return (struct token){ TOKEN_INVALID, i };
		} else {
			return (struct token){ TOK_STRING, i + 1 };
		}
	}
	
#define YYCTYPE        char
#define YYPEEK()       input[index]
#define YYSKIP()       index++
#define YYBACKUP()     marker = index
#define YYBACKUPCTX()  ctxmarker = index
#define YYRESTORE()    index = marker
#define YYRESTORECTX() index = ctxmarker
#define YYLESSTHAN(n)  ((len - index) < (n))
#define YYFILL(n)      {}
	/*!re2c
		re2c:define:YYCTYPE = char;
		re2c:yyfill:enable = 0;

		bin = '0b' [01]+;
		dec = [0-9]+;
		hex = '0x' [0-9a-fA-F]+;
		ws  = [ \t];
		var = 'str'[0-9] | [A-Z] | "Ans";
		ass = "->" | "\x1A";
		id  = [A-Za-z][A-Za-z0-9]*;
		com = ",";
		str = "\"" [^"]* "\"";
		bool = 'On' | 'Off' | 'True' | 'False';

		*        { return (struct token){ TOKEN_INVALID, 0 }; }
		'0x00'   { return (struct token){ TOKEN_EMPTY, 0 }; }
		dec      { return (struct token){ TOK_INTEGER, index }; }
		":"      { return (struct token){ TOKEN_EOL, index }; }
		"+"      { return (struct token){ TOK_PLUS, index }; }
		"-"      { return (struct token){ TOK_MINUS, index }; }
		"*"      { return (struct token){ TOK_TIMES, index }; }
		"/"      { return (struct token){ TOK_DIVIDE, index }; }
		"("      { return (struct token){ TOK_BRO, index }; }
		")"      { return (struct token){ TOK_BRC, index }; }
		"!"      { return (struct token){ TOK_BANG, index }; }
		ass      { return (struct token){ TOK_ASS, index }; }
		com      { return (struct token){ TOK_COMMA, index }; }
		ws+      { return (struct token){ TOKEN_WHITESPACE, index }; }
		"\"" ([\x01-\x21\x23-\xFF])* "\""      { return (struct token){ TOK_STRING, index }; }
		bool     { return (struct token){ TOK_BOOL, index }; }
		
		' And '  { return (struct token){ TOK_AND, index }; }
		' Xor '  { return (struct token){ TOK_XOR, index }; }
		' Or '   { return (struct token){ TOK_OR, index }; }
		'Not '   { return (struct token){ TOK_NOT, index }; }
		
		'Label' ws+ [A-Za-z0-9_]+  { return (struct token){ TOKEN_LABEL, index }; }
		'Goto' ws+ [A-Za-z0-9_]+   { return (struct token){ TOKEN_GOTO, index }; }
		'If'     { return (struct token){ TOKEN_IF, index }; }
		'Stop'   { return (struct token){ TOKEN_STOP, index }; }
		'Exit'   { return (struct token){ TOKEN_EXIT, index }; }
		
		id       { return (struct token){ TOK_ID, index }; }

		"<"      { return (struct token){ TOK_LESS, index }; }
		"<="     { return (struct token){ TOK_LESSEQ, index }; }
		">"      { return (struct token){ TOK_GREATER, index }; }
		">="     { return (struct token){ TOK_GREATEREQ, index }; }
		"="      { return (struct token){ TOK_EQ, index }; }
		"!="     { return (struct token){ TOK_NEQ, index }; }
	*/
}