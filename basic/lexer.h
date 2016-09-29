#pragma once

#include "grammar.h"

struct token
{
	int type;
	int length;
};

struct token lex(const char *input);