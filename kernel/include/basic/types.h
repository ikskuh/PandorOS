#pragma once

// value_t type enumeration
#define TYPE_NULL  0
#define TYPE_NUM   1
#define TYPE_TEXT  2

#define TYPE_NUM_LEN 16
#define TYPE_STR_LEN 4095

/**
 * The numeric type of the interpreter.
 */
typedef double number_t;

/**
 * The string type of the interpreter.
 */
typedef char const * string_t;

/**
 * A typed interpreter value.
 */
typedef struct value {
	int type;  // Uses the VAR_* types.
	union {
		number_t number;
		string_t string;
	};
} value_t;

/**
 * The error enumeration
 */
typedef enum error
{
#define ERR(i,n) ERR_##n = i,
#include "errors.lst"
#undef ERR
} error_t;