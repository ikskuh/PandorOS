#pragma once

#include "gc.h"

// value_t type enumeration
#define TYPE_NULL  0
#define TYPE_NUM   1
#define TYPE_TEXT  2
#define TYPE_PTR   3
#define TYPE_OBJ   4

#define TYPE_NUM_LEN 16
#define TYPE_STR_LEN 4095

#define PTR_RAW  0
#define PTR_FILE 1

/**
 * The numeric type of the interpreter.
 */
typedef double number_t;

/**
 * The string type of the interpreter.
 */
typedef char const * string_t;

/**
 * The pointer type of the interpreter.
 */
typedef struct pointer
{
	/**
	 * Any of PTR_* macros.
	 */
	int type;
	
	/**
	 * The reference of the pointer.
	 */
	void * ref;
} pointer_t;

/**
 * A typed interpreter value.
 */
typedef struct value {
	int type;  // Uses the VAR_* types.
	union {
		number_t number;
		string_t string;
		pointer_t pointer;
		gcref_t object;
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