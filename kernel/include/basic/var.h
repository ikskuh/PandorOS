#pragma once

#include "config.h"
#include "types.h"
#include <stdbool.h>


/**
 * Opaque handle to a variable.
 */
typedef struct variable variable_t;

/**
 * Initializes the variable system.
 */
void var_init();

/**
 * Gets a variable by its name.
 */
variable_t * var_byname(char const * name);

/**
 * Returns true if the variable is readonly.
 */
bool var_isro(variable_t *var);

/**
 * Gets the type of a variable.
 * 
 * VAR_NULL for not existent, VAR_INT for integers, VAR_TXT for strings.
 */ 
int var_type(variable_t const * var);

/**
 * Gets the value of a variable.
 * @param var    The variable of which the value should be get.
 * @param target A pointer to the variable contents.
 */
void var_get(variable_t * var, value_t * target);

/**
 * Sets the value of a variable.
 * @param var    The variable of which the value should be set.
 * @param target A pointer to the variable contents.
 */
void var_set(variable_t * var, value_t source);

/**
 * Sets the answer variable.
 */ 
void var_setans(value_t value);