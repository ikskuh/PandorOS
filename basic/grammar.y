%token_type {int}  
%token_prefix TOK_

%left PLUS MINUS.   
%left DIVIDE TIMES.  
   
%include {
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "lexer.h"
}  
   
%syntax_error {  
  printf("Syntax error!\n");
}   
   
program ::= expr(A).   { printf("Result=%d\n", A); }  

expr(A) ::= expr(B) MINUS  expr(C). { A = B - C; }  
expr(A) ::= expr(B) PLUS   expr(C). { A = B + C; }  
expr(A) ::= expr(B) TIMES  expr(C). { A = B * C; }  
expr(A) ::= expr(B) DIVIDE expr(C). {
	if(C != 0){
		A = B / C;
	}else{
		printf("divide by zero\n");
	}
}  /* end of DIVIDE */

expr(A) ::= BRO expr(B) BRC. { A = B; }

expr(A) ::= INTEGER(B). { A = B; } 

% code {

void *mwrap(size_t size)
{
	printf("Allocating %d bytes...\n", size);
	return malloc(size);
}

int main(int argc, char **argv)
{
	for (int i = 1; i < argc; ++i)
	{
		char *input = argv[i];
		printf("%s\n", input);
		
		
		void* pParser = ParseAlloc (mwrap);
		while(true)
		{
			struct token token = lex(input);
			if(token.type >= 0)
			{
				printf ("  (%d, %d): '", token.type, token.length);
				fwrite(input, token.length, 1, stdout);
				printf("'\n");
				static char buffer[64];
				memset(buffer, 0, sizeof(buffer));
				memcpy(buffer, input, token.length);
				int value = strtol(buffer, NULL, 10);
				
				Parse(pParser, token.type, value);
				
				if(token.type == 0)
					break;
			}
			input += token.length;
			if(*input == 0)
				break;
		}
		
		Parse(pParser, 0, 0);
		
		ParseFree(pParser, free );
	}
	
	return 0;
}

}