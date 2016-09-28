#include "options.h"
#include "io.h"
#include "standard.h"
#include <stddef.h>

// This must be initialized in .data, so HAL can register option groups as well.
optiongroup_t *firstGroup = NULL;

console_t *console = NULL;

void options_init()
{
	console = console_new();
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

static int options_maxwidth()
{
	int w = 0;
	for(optiongroup_t *group = firstGroup; group != NULL; group = group->next)
	{
		for(option_t *opt = group->first; opt != NULL; opt = opt->next)
		{
			w = max(w, str_len(opt->name));
		}
	}
	return w;
}

void options_showmenu()
{	
	console_t *prev = stdcon;
	
	console_set(console);
	
	int optionswidth = options_maxwidth();
	
	while(true)
	{
		// Render catalog
		cls();
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
							printf("true");
						else
							printf("false");
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
			}
			
			putc(0xC0);
			
			for(int i = 0; i < console->width - 2; i++) {
				putc(0xC4);
			}
			putc(0xD9);
		}
		
		keyhit_t hit = getkey(true);
	}
}
