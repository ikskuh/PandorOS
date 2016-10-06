#pragma once

#include "basic/types.h"
#include "basic/var.h"
#include "file.h"

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#define BASIC_FUNCTION 1
#define BASIC_ORDER    2

/**
 * Initializes the basic interpreter.
 */
void basic_init();

/**
 * Initializes the basic stdlib.
 */
void stdlib_init();

/**
 * A function that can be called from basic.
 * @param argc Number of arguments
 * @param argv An array with the argument values.
 * @return     The result of the function.
 */
typedef value_t (*basfunc_f)(int argc, value_t *argv);

/**
 * Executes a line of basic code.
 * 
 * @param input The basic code that should be executed.
 * @return      The evaluation result.
 */
value_t basic_execute(char const *input);

value_t basic_execute2(uint8_t const * tokens, int length);

/**
 * Registers a basic function.
 * @param name     The case sensitive name of the function
 * @param function The function itself.
 * @param type     The type of the function (BASIC_ORDER or BASIC_FUNCTION)
 */
void basic_register(char const *name, basfunc_f function, int type);

/**
 *
 */
void basic_compile(file_t * in, file_t * outfile);

/**
 * Returns true if a value is null.
 */
bool basic_isnull(value_t value);

/**
 * Extracts the number from a value.
 */
number_t basic_getnum(value_t value);

/**
 * Extracts the string from a value.
 */
string_t basic_getstr(value_t value);

/**
 * Makes a numeric value.
 */
value_t basic_mknum(number_t num);

/**
 * Makes a string value.
 */
value_t basic_mkstr(string_t num);

/**
 * Makes a null value.
 */
value_t basic_mknull();

/**
 * Throws an error.
 */
void basic_error(error_t reason);

/**
 * Returns the last error that occured.
 * @returns The last error or ERR_SUCCESS on success.
 */
error_t basic_lasterror();

/**
 * Returns the human-readable name of the error code
 */
char const * basic_err_to_string(error_t err);

/** 
 * Resets execution-time memory.
 */
void basic_memreset();

/**
 * Allocates execution-time memory.
 */
void *basic_alloc(size_t size);