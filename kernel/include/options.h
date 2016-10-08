#pragma once

#include "basic/types.h"

#define OPT_NULL 0
#define OPT_BOOL 1
#define OPT_INT  2
#define OPT_TXT  3

typedef struct {
	int min, max;
} optioncfg_int_t;

typedef struct {
	int length;
} optioncfg_txt_t;

typedef struct {
	
} optioncfg_bool_t;

typedef struct option
{
	int type;
	char const *name;
	void *value;
	void const * config;
	struct option *next;
} option_t;

typedef struct optiongroup
{
	char const *name;
	option_t *first;
	
	struct optiongroup *next;
} optiongroup_t;

/**
 * Initializes the option system.
 */
void options_init();

/**
 * Registers an optiongroup in the global registry.
 * @param group The group to register.
 * 
 * @remarks Modifies optiongroup_t::next
 */
void optiongroup_register(optiongroup_t *group);

/**
 * Adds an option to a group.
 * @param group  The group where the option should be added.
 * @param option The option to be added.
 * 
 * @remarks Modifies optiongroup_t::first, option_t::next.
 */
void option_add(optiongroup_t *group, option_t *option);

/**
 * Opens the options menu and does the input loop.
 */
void options_showmenu();

/**
 * Sets an option to a value.
 * @param option The name of the option in the format "group/option".
 * @param value  The value the option should get.
 * @remarks      Throws ERR_INVALID_OPTION if the option does not exist.
 */
void options_set(char const * option, value_t value);

/**
 * Gets an option to a value.
 * @param option The name of the option in the format "group/option".
 * @remarks      Throws ERR_INVALID_OPTION if the option does not exist.
 */
value_t options_get(char const * option);