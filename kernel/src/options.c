#include "options.h"
#include "io.h"
#include "standard.h"
#include <stddef.h>

// This must be initialized in .data, so HAL can register option groups as well.
optiongroup_t *firstGroup = NULL;

console_t *console = NULL;

static optiongroup_t optionsDemo = { "Example Options", NULL, NULL };

int _ival;
bool _bval;
char _tval[64];

static option_t optionInt  = { OPT_INT,  "Numeric",      &_ival, NULL };
static option_t optionBool = { OPT_BOOL, "Boolean/Flag", &_bval, NULL };
static option_t optionStr  = { OPT_TXT,  "String/Text",   _tval, NULL };

void options_init()
{
	console = console_new();
	console->flags &= ~CON_AUTOREFRESH;
	console->flags |=  CON_NOCURSOR;
	
	
	optiongroup_register(&optionsDemo);
	
	option_add(&optionsDemo, &optionInt);
	option_add(&optionsDemo, &optionBool);
	option_add(&optionsDemo, &optionStr);
	
	str_copy(_tval, "Edit this text!");
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
	switch(vkey)
	{
		case VK_NUM_PLUS:
			*((int*)option->value) += 1;
			return;
		case VK_NUM_MINUS:
			*((int*)option->value) -= 1;
			return;
	}
}

static void options_editor_bool(option_t *option, int x, int y, int len, int vkey)
{
	switch(vkey)
	{
		case VK_NUM_PLUS:
			*((bool*)option->value) = true;
			return;
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
	char *str = option->value;
	
	char *work = malloc(OPT_STR_LIMIT);
	str_copy(work, str);
	
	int cursor = str_len(str);
	
	int prevFlags = console->flags;
	console->flags &= ~CON_NOCURSOR;
	
	while(true)
	{
		for(int i = 0; i < len; i++) {
			int idx = console->width * y + x + i;
			if(i < cursor)
				console->data[idx].c = work[i];
			else
				console->data[idx].c = ' ';
			console->data[idx].attribs = CHA_HIGHLIGHT;
		}
		console->cursor.x = x + cursor;
		console->cursor.y = y;
		console_refresh();
	
		keyhit_t hit = getkey(true);
		if((hit.flags & (khfKeyPress | khfCharInput)) == 0)
			continue;
		
		if(hit.flags & khfKeyPress)
		{
			switch(hit.key)
			{
				case VK_BACKSPACE:
					if(cursor <= 0)
						continue;
					work[--cursor] = 0;
					continue;
				case VK_RETURN:
					str_copy(str, work);
					// Pass through
				case VK_ESCAPE:
					console->flags = prevFlags;
					free(work);
					return;
			}
		}
		
		if(hit.flags & khfCharInput)
		{
			// TODO: Input text here
			work[cursor++] = hit.codepoint;
			work[cursor] = 0;
		}
	}
	
	
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
