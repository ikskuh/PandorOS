#pragma once

#define TOKEN_EXIT       -9
#define TOKEN_STOP       -8
#define TOKEN_LABEL      -7
#define TOKEN_GOTO       -6
#define TOKEN_IF         -5
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