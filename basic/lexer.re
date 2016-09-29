#include <stdio.h>
#include <string.h>
#include <stdbool.h>

struct token
{
	int type;
	int length;
};

static struct token lex(const char *input)
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
		dec      { return (struct token){ 1, index }; }
		hex      { return (struct token){ 2, index }; }
		[+\-*\/] { return (struct token){ 3, index }; }
		ws+      { return (struct token){ 4, index }; }
		var      { return (struct token){ 5, index }; }
		ass      { return (struct token){ 6, index }; }
	*/
}

int main(int argc, char **argv)
{
    for (int i = 1; i < argc; ++i)
		{
			char *input = argv[i];
			printf("%s\n", input);
			while(true)
			{
				struct token token = lex(input);
				printf ("  (%d, %d): '", token.type, token.length);
				fwrite(input, token.length, 1, stdout);
				printf("'\n");
				if(token.type == 0)
					break;
				input += token.length;
				if(*input == 0)
					break;
			}
    }
    return 0;
}