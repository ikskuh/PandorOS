#include <stddef.h>
#include <stdbool.h>

#include "menu.h"
#include "standard.h"
#include "io.h"
#include "options.h"
#include "pmm.h"
#include "alloc.h"
#include "interpreter.h"
#include "catalog.h"

int ticks = 0;

void os_tick()
{
	ticks++;
	// int_to_string(mainmenu[5].label, ticks, 10);
	// console_refresh();
}

static void select_shell(int shellId);

static void select_shell0() { select_shell(0); }
static void select_shell1() { select_shell(1); }
static void select_shell2() { select_shell(2); }

menu_t mainmenu = {
	3,
	(menuitem_t[]) {
		{ "Catalog", MENU_DEFAULT, &catalog_open, 0, NULL },
		{ "Screen 0", MENU_DEFAULT, NULL,
			3,
			(menuitem_t[]) {
				{ "Screen 0", MENU_DEFAULT, &select_shell0, 0, NULL },
				{ "Screen 1", MENU_DEFAULT, &select_shell1, 0, NULL },
				{ "Screen 2", MENU_DEFAULT, &select_shell2, 0, NULL },
			}
		},
		{ "System", MENU_DEFAULT | MENU_RIGHTALIGN, NULL, 
			6, 
			(menuitem_t[]) {
				{ "Memory Management", MENU_DEFAULT, NULL, 0, NULL },
				{ "OS Debugger",       MENU_DEFAULT, NULL, 0, NULL },
				{ "Settings",          MENU_DEFAULT, options_showmenu, 0, NULL },
				{ "About",             MENU_DEFAULT, NULL, 0, NULL },
				{ "Reboot",            MENU_DEFAULT, NULL, 0, NULL },
				{ "Poweroff",          MENU_DEFAULT, NULL, 0, NULL },
			}
		},
	},
};

static struct {
	console_t *console;
	char input[128];
	int cursor;
} shells[3];

int currentShell = 0;
#define currshell shells[currentShell]
static char const * shell_prompt = "#> ";

static void select_shell(int shellId)
{
	if(shellId < 0 || shellId > 2)
		return;
	currentShell = shellId;
	
	mainmenu.items[1].label[7] = '0' + shellId;
	
	console_set(currshell.console);
}

static void shell_main()
{
	while(true)
	{
		int c = getchar();
		
		switch(c)
		{
			case '\t':
			{
				// menuitem_t *selection = menu_open(&mainmenu);
				menu_open(&mainmenu);
				char const * insertion = catalog_get();
				if(insertion != NULL)
				{
					while(*insertion) {
						putc(*insertion);
						currshell.input[currshell.cursor++] = *insertion++;
					}
				}
				break;
			}
			case 0x1B: // Escape
				while(--currshell.cursor >= 0)
				{
					putc('\b');
				}
				currshell.cursor = 0;
				currshell.input[0] = 0;
				
				break;
			case '\b':
				if(currshell.cursor > 0) {
					currshell.input[--currshell.cursor] = 0;
					putc('\b');
				}
				break;
			case '\n':
				printf("\n"); // end line the input
				currshell.input[currshell.cursor] = 0;
				currshell.cursor = 0;
				
				value_t result = basic_execute(currshell.input);
				
				if(basic_isnull(result) == false) {
					printf("= %v\n", result);
				}
				printf("%s", shell_prompt);
				break;
			default:
				currshell.input[currshell.cursor++] = c;
				putc(c);
				break;
		}
	}
}

void os_init()
{
	// TODO: Initialize OS…
	console_init();
	options_init();
	catalog_init();
	basic_init();
	
	var_init();
	
	for(int i = 0; i < 3; i++)
	{
		shells[i].console = console_new();
		shells[i].input[0] = 0;
		shells[i].cursor = 0;
		
		console_printf(shells[i].console, "%s", shell_prompt);
	}
	
	menu_render(&mainmenu);
	
	// menu_select(&mainmenu, -1);

	select_shell(0);
	
	shell_main();
	
	while(true);
}