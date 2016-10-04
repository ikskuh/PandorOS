#include "mainmenu.h"
#include "menu.h"
#include "catalog.h"
#include "charmap.h"
#include "options.h"
#include "shell.h"
#include "standard.h"
#include "debug.h"
#include <stddef.h>

menu_t mainmenu = {
	4,
	(menuitem_t[]) {
		{ "Catalog",  MENU_DEFAULT, &catalog_open, NULL, 0, NULL },
		{ "Char Map", MENU_DEFAULT, &charmap_open, NULL, 0, NULL },
		{ "Screen ?", MENU_DEFAULT, NULL, NULL, 0, NULL },
		{ "System",   MENU_DEFAULT | MENU_RIGHTALIGN, NULL, NULL,
			6, 
			(menuitem_t[]) {
				{ "Memory Management", MENU_DEFAULT, NULL, NULL, 0, NULL },
				{ "OS Debugger",       MENU_DEFAULT, NULL, NULL, 0, NULL },
				{ "Settings",          MENU_DEFAULT, options_showmenu, NULL, 0, NULL },
				{ "About",             MENU_DEFAULT, NULL, NULL, 0, NULL },
				{ "Reboot",            MENU_DEFAULT, NULL, NULL, 0, NULL },
				{ "Poweroff",          MENU_DEFAULT, NULL, NULL, 0, NULL },
			}
		},
	},
};

void mainmenu_open(bool fromShell)
{
	mainmenu_shellenable(fromShell);
	menu_open(&mainmenu);
	mainmenu_shellenable(fromShell);
}

void mainmenu_render()
{
	menu_render(&mainmenu);
}

void mainmenu_shellenable(bool enable)
{
	if(enable)
		mainmenu.items[MAINMENU_SHELL].flags &= ~MENU_DISABLED;
	else
		mainmenu.items[MAINMENU_SHELL].flags |=  MENU_DISABLED;
	mainmenu_render();
}

void mainmenu_initshell(shell_t * shells, int count)
{
	mainmenu.items[MAINMENU_SHELL].length = count;
	mainmenu.items[MAINMENU_SHELL].items = malloc(count * sizeof(menuitem_t));
	mem_set(
		mainmenu.items[MAINMENU_SHELL].items, 
		0, 
		count * sizeof(menuitem_t));
	for(int i = 0; i < count; i++)
	{
		str_copy(mainmenu.items[MAINMENU_SHELL].items[i].label, shells[i].name);
		mainmenu.items[MAINMENU_SHELL].items[i].callback = (menucallback_f)select_shell;
		mainmenu.items[MAINMENU_SHELL].items[i].userdata = &shells[i];
	}
}

void mainmenu_setshell(char const * name)
{
	str_copy(mainmenu.items[MAINMENU_SHELL].label, name);
}