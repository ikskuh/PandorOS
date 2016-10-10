#include "interpreter.h"
#include "string.h"
#include "pmm.h"
#include "hal.h"
#include "debug.h"

#include <stddef.h>
#include <stdbool.h>

typedef struct variable
{
	int type;
	bool ro;
	union {
		number_t num;
		char * text;
	};
} variable_t;

variable_t variable_ans = { TYPE_NULL, true, {} };
variable_t variable_null = { TYPE_NULL, true, {} };

variable_t variables_int[26];
variable_t variables_str[10];

static char *ansstring;

void var_init()
{
	for(int i = 0; i < 26; i++)
	{
		variables_int[i].type = TYPE_NUM;
		variables_int[i].ro   = false;
		variables_int[i].num  = 0;
	}
	
	for(int i = 0; i < 10; i++)
	{
		variables_str[i].type = TYPE_TEXT;
		variables_str[i].ro   = false;
		variables_str[i].text = pmm_getptr(pmm_alloc());
		str_copy(variables_str[i].text, "");
	}
	
	ansstring = pmm_getptr(pmm_alloc());
}

variable_t * var_byname(char const * name)
{
	debug("var-by-name: '%s'\n", name);
	if(str_eq(name, "Ans")) {
		return &variable_ans;
	}
	if(str_startswith(name, "Str"))
	{
		if(name[3] >= '0' && name[3] <= '9') {
			return &variables_str[name[3] - '0'];
		} else {
			return NULL;
		}
	}
	char n = name[0];
	if(n >= 'A' && n <= 'Z') {
		return &variables_int[n - 'A'];
	// } else if(n >= 'a' && n <= 'z') {
	// 	return &variables_int[n - 'a'];
	} else {
		debug("\tnot found!\n");
		return &variable_null;
	}
}

int var_type(variable_t const * var)
{
	if(var != NULL)
		return var->type;
	else
		return TYPE_NULL;
}


void var_get(variable_t * var, value_t * target)
{
	if(var == NULL || var->type == TYPE_NULL || var == &variable_null) {
		basic_error(ERR_INVALID_VAR);
		return;
	}
	target->type = var->type;
	switch(var->type)
	{
		case TYPE_NUM:
			target->number = var->num;
			break;
		case TYPE_TEXT:
			target->string = var->text;
			break;
	}
}

void var_set(variable_t * var, value_t source)
{
	if(var == NULL || var->type == TYPE_NULL) {
		basic_error(ERR_INVALID_VAR);
		return;
	}
	if(var->ro) {
		basic_error(ERR_INVALID_VAR);
		return;
	}
	if(var->type != source.type)
		basic_error(ERR_INVALID_TYPE);
	hal_debug("Setting varibale to '%v'.\n", source);
	switch(var->type)
	{
		case TYPE_NUM:
			var->num = source.number;
			break;
		case TYPE_TEXT:
			str_copy(var->text, source.string);
			break;
	}
}

void var_setans(value_t value)
{
	variable_ans.type = value.type;
	variable_ans.ro = false;
	if(value.type == TYPE_TEXT) {
		// We have a shared string ptr that will change on numeric stuff...
		variable_ans.text = ansstring;
	}
	var_set(&variable_ans, value);
	variable_ans.ro = true;
}

void var_getans(value_t * target)
{
	var_get(&variable_ans, target);
}