#pragma once

#include "var.h"

/**
 * A typed interpreter value.
 */
typedef int value_t;

/**
 * Initializes the basic interpreter.
 */
void basic_init();

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

/**
 * Registers a basic function.
 * @param name     The case sensitive name of the function
 * @param function The function itself.
 */
void basic_register(char const *name, basfunc_f function);