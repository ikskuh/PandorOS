#pragma once

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