#pragma once

#define TOKEN_WHILE      -15
#define TOKEN_IF         -14
#define TOKEN_THEN       -13
#define TOKEN_ELSE       -12
#define TOKEN_END        -11
#define TOKEN_FOR        -10

#define TOKEN_EOF        -4
#define TOKEN_EOL        -3
#define TOKEN_INVALID    -2
#define TOKEN_WHITESPACE -1
#define TOKEN_EMPTY       0

#include "grammar.h"

struct token
{
	int type;
	int length;
};

struct token lex(const char *input);