#include "basic/lexer.h"
#include "string.h"
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
		dec = [0-9]+;
		hex = '0x' [0-9a-fA-F]+;
		ws  = [ \t];
		var = 'str'[0-9] | [A-Z] | "Ans";
		ass = "->" | "\x1A";
		fun = [A-Za-z][A-Za-z0-9]* "(";
		com = ",";
		str = "\"" [^"]* "\"";
		ord = [A-Za-z][A-Za-z0-9]* " "*;

		*        { return (struct token){ 0, 0 }; }
		dec      { return (struct token){ TOK_INTEGER, index }; }
		hex      { return (struct token){ 2, index }; }
		"+"      { return (struct token){ TOK_PLUS, index }; }
		"-"      { return (struct token){ TOK_MINUS, index }; }
		"*"      { return (struct token){ TOK_TIMES, index }; }
		"/"      { return (struct token){ TOK_DIVIDE, index }; }
		"("      { return (struct token){ TOK_BRO, index }; }
		")"      { return (struct token){ TOK_BRC, index }; }
		ass      { return (struct token){ TOK_ASS, index }; }
		var      { return (struct token){ TOK_VAR, index }; }
		fun      { return (struct token){ TOK_FUN, index }; }
		com      { return (struct token){ TOK_COMMA, index }; }
		ws+      { return (struct token){ -1, index }; }
		str      { return (struct token){ TOK_STRING, index }; }
		ord      { return (struct token){ TOK_ORDER, index }; }
		
		" And "  { return (struct token){ TOK_AND, index }; }
		" Xor "  { return (struct token){ TOK_XOR, index }; }
		" Or "   { return (struct token){ TOK_OR, index }; }
		"Not "   { return (struct token){ TOK_NOT, index }; }
	*/
}