#pragma once

#include <stdbool.h>

#define VAR_NULL 0
#define VAR_INT  1
#define VAR_TXT  2

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
 *
 * for VAR_NULL the target is ignored.
 * for VAR_INT target should point to an int.
 * for VAR_TEXT target should point to a char array with sufficient storage.
 */
void var_get(variable_t * var, void *target);

/**
 * Sets the value of a variable.
 * @param var    The variable of which the value should be set.
 * @param target A pointer to the variable contents.
 *
 * for VAR_NULL the target is ignored.
 * for VAR_INT target should point to an int.
 * for VAR_TEXT target is a nullterminated char *;
 */
void var_set(variable_t * var, void const * target);