#include "var.h"
#include "string.h"
#include "pmm.h"

#include <stddef.h>
#include <stdbool.h>

typedef struct variable
{
	int type;
	bool ro;
	union {
		int num;
		char * text;
	};
} variable_t;

variable_t variable_ans = { VAR_NULL, true, {} };

variable_t variables_int[26];
variable_t variables_str[10];

void var_init()
{
	for(int i = 0; i < 26; i++)
	{
		variables_int[i].type = VAR_INT;
		variables_int[i].ro   = false;
		variables_int[i].num  = 0;
	}
	
	for(int i = 0; i < 10; i++)
	{
		variables_str[i].type = VAR_TXT;
		variables_str[i].ro   = false;
		variables_str[i].text = pmm_getptr(pmm_alloc());
		str_copy(variables_str[i].text, "");
	}
}


variable_t * var_byname(char const * name)
{
	if(str_eq(name, "ANS")) {
		return &variable_ans;
	}
	if(str_startswith(name, "STR"))
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
		return NULL;
	}
}

int var_type(variable_t const * var)
{
	if(var != NULL)
		return var->type;
	else
		return VAR_NULL;
}


void var_get(variable_t * var, void *target)
{
	if(var == NULL || var->type == VAR_NULL)
		return;
	switch(var->type)
	{
		case VAR_INT:
			*((int*)target) = var->num;
			break;
		case VAR_TXT:
			str_copy(target, var->text);
			break;
	}
}

void var_set(variable_t * var, void const * target)
{
	if(var == NULL || var->type == VAR_NULL)
		return;
	if(var->ro)
		return;
	switch(var->type)
	{
		case VAR_INT:
			var->num = *((int const*)target);
			break;
		case VAR_TXT:
			str_copy(var->text, target);
			break;
	}
}