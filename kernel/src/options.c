#include "options.h"
#include "io.h"
#include "standard.h"
#include <stddef.h>

// This must be initialized in .data, so HAL can register option groups as well.
optiongroup_t *firstGroup = NULL;

console_t *console = NULL;
;
/*
static optiongroup_t optionsDemo = { "Example Options", NULL, NULL };

int _ival;
bool _bval;
char _tval[16];

static option_t optionInt  = {
	OPT_INT,  "Numeric",      &_ival, 
	(optioncfg_int_t[]) { { -10, 10 } },
	NULL
};

static option_t optionBool = { OPT_BOOL, "Boolean/Flag", &_bval, NULL, NULL };

static option_t optionStr  = {
	OPT_TXT,  "String/Text",   _tval, 
	(optioncfg_txt_t[]) { { 15 } },
	NULL
};
*/

void options_init()
{
	console = console_new();
	console->flags &= ~CON_AUTOREFRESH;
	console->flags |=  CON_NOCURSOR;
	
	/*
	optiongroup_register(&optionsDemo);
	
	option_add(&optionsDemo, &optionInt);
	option_add(&optionsDemo, &optionBool);
	option_add(&optionsDemo, &optionStr);
	
	str_copy(_tval, "Edit this text!");
	*/
}

void optiongroup_register(optiongroup_t *group)
{
	optiongroup_t *it = firstGroup;
	if(it == NULL) {
		firstGroup = group;
	} else {
		while(it->next) it = it->next;
		it->next = group;
	}
	group->next = NULL;
}

void option_add(optiongroup_t *group, option_t *option)
{
	option_t *it = group->first;
	if(it == NULL) {
		group->first = option;
	} else {
		while(it->next) it = it->next;
		it->next = option;
	}
	option->next = NULL;
}

static void options_bounds(int *width, int *count)
{
	*width = 0;
	*count = 0;
	for(optiongroup_t *group = firstGroup; group != NULL; group = group->next)
	{
		for(option_t *opt = group->first; opt != NULL; opt = opt->next)
		{
			*width = max(*width, str_len(opt->name));
			(*count)++;
		}
	}
}

static option_t * option_get(int index)
{
	int count = 0;
	for(optiongroup_t *group = firstGroup; group != NULL; group = group->next)
	{
		for(option_t *opt = group->first; opt != NULL; opt = opt->next)
		{
			if(count == index) return opt;
			count++;
		}
	}
	return NULL;
}

static void options_editor_int(option_t *option, int x, int y, int len, int vkey)
{
	optioncfg_int_t const *cfg = option->config;
	int *i = option->value;
	switch(vkey)
	{
		case VK_RIGHT:
		case VK_NUM_PLUS:
			*i += 1;
			break;
		case VK_LEFT:
		case VK_NUM_MINUS:
			*i -= 1;
			break;
	}
	if(vkey == VK_RETURN)
	{
		char buffer[64];
		str_printf(buffer, "%d", *i);
		if(input_textfield(buffer, 63, x, y, len, TEXTFIELD_DEFAULT) != VK_ESCAPE)
		{
			*i = str_to_int(buffer, 10);
		}
	}
	if(cfg != NULL) {
		*i = clamp(*i,  cfg->min, cfg->max);
	}
}

static void options_editor_bool(option_t *option, int x, int y, int len, int vkey)
{
	// optioncfg_bool_t const *cfg = option->config;
	(void)x;
	(void)y;
	(void)len;
	switch(vkey)
	{
		case VK_RIGHT:
		case VK_NUM_PLUS:
			*((bool*)option->value) = true;
			return;
		case VK_LEFT:
		case VK_NUM_MINUS:
			*((bool*)option->value) = false;
			return;
		case VK_SPACE:
		case VK_RETURN:
			*((bool*)option->value) ^= true;
			return;
	}
}

static void options_editor_txt(option_t *option, int x, int y, int len, int vkey)
{
	optioncfg_txt_t const *cfg = option->config;
	if(vkey != VK_RETURN)
		return;
	int limit = 4096; // TODO: Replace with meaningful value.
	if(cfg != NULL)
		limit = cfg->length;
	input_textfield(option->value, limit, x, y, len, TEXTFIELD_DEFAULT);
}

/**
 * Edits the given option.
 * @param x    The x-coordinate of the editor.
 * @param y    The y-coordinate of the editor.
 * @param len  The length of the editor.
 * @param vkey The key that invoked the editor
 */
void options_editor(option_t *option, int x, int y, int len, int vkey)
{
	switch(option->type)
	{
		case OPT_NULL:
			return;
		case OPT_INT:
			options_editor_int(option, x, y, len, vkey);
			return;
		case OPT_BOOL:
			options_editor_bool(option, x, y, len, vkey);
			return;
		case OPT_TXT:
			options_editor_txt(option, x, y, len, vkey);
			return;
	}
}

void options_showmenu()
{
	console_t *prev = stdcon;
	
	console_set(console);
	
	int optionswidth;
	int optionscount;
	options_bounds(&optionswidth, &optionscount);
	
	int cursor = 0;
	
	while(true)
	{
		// Render catalog
		cls();
		
		int index = 0;
		int cursor_y = -1;
		for(optiongroup_t *group = firstGroup; group != NULL; group = group->next)
		{
			putc(0xDA);
			putc(0xC4);
			putc(0xC4);
			printf("[%s]", group->name);
			
			int len = console->width - str_len(group->name) - 6;
			for(int i = 0; i < len; i++) {
				putc(0xC4);
			}
			putc(0xBF);
			
			for(option_t *opt = group->first; opt != NULL; opt = opt->next)
			{
				putc(0xB3);
				putc(' ');
				printf("%s", opt->name);
				
				if(index == cursor) {
					cursor_y = console->cursor.y;
					for(int i = 0; i < optionswidth; i++) {
						int idx = console->cursor.y * console->width + 2 + i;
						console->data[idx].attribs = CHA_HIGHLIGHT;
					}
				}
				
				console->cursor.x = optionswidth + 3;
				putc('=');
				putc(' ');
				
				switch(opt->type)
				{
					case OPT_NULL:
						printf("NULL");
						break;
					case OPT_BOOL:
						if(*((bool*)opt->value))
							printf("on");
						else
							printf("off");
						break;
					case OPT_INT:
						printf("%d", *((int*)opt->value));
						break;
					case OPT_TXT:
						printf("%s", opt->value);
						break;
				}
				
				console->cursor.x = console->width - 1;
				putc(0xB3);
				
				index++;
			}
			
			putc(0xC0);
			
			for(int i = 0; i < console->width - 2; i++) {
				putc(0xC4);
			}
			putc(0xD9);
		}
		
		console_refresh();
		
		keyhit_t hit = getkey(true);
		if((hit.flags & khfKeyPress) == 0)
			continue;
			
		option_t *opt = option_get(cursor);
		
		switch(hit.key)
		{
			case VK_UP:
				if(cursor > 0) cursor--;
				break;
			case VK_DOWN:
				if((++cursor) >= optionscount) cursor--;
				break;
			case VK_LEFT:
			case VK_RIGHT:
			case VK_NUM_PLUS:
			case VK_NUM_MINUS:
			case VK_RETURN:
			case VK_SPACE:
				if(opt == NULL)
					break;
				options_editor(
					opt,
					optionswidth + 5, 
					cursor_y,
					console->width - optionswidth - 7,
					hit.key);
				break;
			case VK_ESCAPE:
				console_set(prev);
				return;
		}
	}
}

#include "interpreter.h"
#include "debug.h"

static option_t *find(char const * option)
{
	int len = str_len(option);
	char buffer[128];
	str_copy(buffer, option);
	
	int i;
	for(i = 0; i < len; i++)
	{
		if(buffer[i] == '/')
			break;
	}
	if(i >= len)
		return NULL;
	buffer[i] = 0;

	optiongroup_t *group;
	for(group = firstGroup; group != NULL; group = group->next)
	{
		if(str_eq(group->name, buffer))
			break;
	}
	if(group == NULL)
		return NULL;
	for(option_t *opt = group->first; opt != NULL; opt = opt->next)
	{
		if(str_eq(opt->name, &buffer[i+1]))
			return opt;
	}
	return NULL;
}

void options_set(char const * optname, value_t value)
{
	option_t * option = find(optname);
	if(option == NULL)
		basic_error(ERR_INVALID_OPTION);
	
	switch(option->type)
	{
		case OPT_BOOL:
		{
			bool state = basic_getnum(value);
			*((bool*)option->value) = state;
			break;
		}
		case OPT_INT:
		{
			int num = basic_getnum(value);
			
			optioncfg_int_t const *cfg = option->config;
			int *i = option->value;
			*i = num;
			if(cfg != NULL) {
				*i = clamp(*i,  cfg->min, cfg->max);
			}
			
			break;
		}
		case OPT_TXT:
		{
			string_t text = basic_getstr(value);
			optioncfg_txt_t const *cfg = option->config;
			
			int limit = 4096; // TODO: Replace with meaningful value.
			if(cfg != NULL)
				limit = cfg->length;
			
			int len = str_len(text);
			if(len > limit)
				basic_error(ERR_INVALID_VALUE);
			
			str_copy(option->value, text);
			
			break;
		}
		default:
			return;
	}
}

value_t options_get(char const * optname)
{
	option_t * option = find(optname);
	if(option == NULL)
		basic_error(ERR_INVALID_OPTION);
	
	switch(option->type)
	{
		case OPT_BOOL:
		{
			bool *b = option->value;
			return basic_mknum(*b);
		}
		case OPT_INT:
		{
			int *i = option->value;
			return basic_mknum(*i);
		}
		case OPT_TXT:
		{
			return basic_mkstr(option->value);
		}
		default:
			return basic_mknull();
	}
}