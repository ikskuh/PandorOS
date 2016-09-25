#include <stdbool.h>

#include "console.h"
#include "input.h"
#include "stdlib.h"

menu_entry_t mainmenu[] = {
	{ "Screen 0", MENU_DEFAULT },
	{ "Screen 1", MENU_DEFAULT },
	{ "Screen 2", MENU_DEFAULT },
	{ "Catalog", MENU_RIGHTALIGN },
	{ "System", MENU_RIGHTALIGN },
	{ "0", MENU_RIGHTALIGN },
};

#define MENU_SIZE (sizeof(mainmenu) / sizeof(mainmenu[0]))

void shell_start();

int ticks = 0;

void os_tick()
{
	ticks++;
	int_to_string(mainmenu[5].label, ticks, 10);
	console_refresh();
}

void os_init()
{
	// TODO: Initialize OS…
	console_init();
	console_setmenu(mainmenu, MENU_SIZE);
	
	printf("Starting the OS...\n");
	
	printf("String: %s\n", "Hello World!");
	printf("Numbers: %b %d %x\n", 14, 15, 13);
	printf("Numbers: %b %d %x\n", -14, -15, -13);

	printf("Character Map:\n");
	for(int y = -1; y < 16; y++)
	{
		for(int x = -1; x < 16; x++) {
			if(y < 0) {
				if(x < 0) {
					printf("  ");
				} else {
					printf("%x ", x);
				}
				continue;
			}
			if(x < 0) {
				printf("%x ", y);
			} else {
				char c = 16*y+x;
				if(c == '\n' || c == '\b')
					printf("  ");
				else
					printf("%c ", c);
			}
		
		}
		printf("\n");
	}
	
	shell_start();
	
	while(true);
}

void select_in_menu(int item)
{
	for(int i = 0; i < MENU_SIZE; i++)
	{
		if(item == i)
			mainmenu[i].flags |= MENU_SELECTED;
		else
			mainmenu[i].flags &= ~MENU_SELECTED;
	}
	console_refresh();
}

/**
 * Opens the system menu and returns the menu result.
 */
int menu()
{
	int selection = 0;
	while(true)
	{
		select_in_menu(selection);
		keyhit_t input = getkey(true);
		if((input.flags & khfKeyPress) == 0)
			continue;
		switch(input.key)
		{
			case VK_LEFT:
				if(selection > 0) selection -= 1;
				break;
			case VK_RIGHT:
				if(selection < (MENU_SIZE - 1)) selection += 1;
				break;
			case VK_ESCAPE:
				select_in_menu(-1);
				return -1;
			case VK_RETURN:
				select_in_menu(-1);
				return -1;
		}
	}
}

int readline(char *buffer, bool allowMenus)
{
	int cursor = 0;
	int length = 0;
	while(true)
	{
		int c = getchar();
		if(c < 0)
			continue;
		if(c == '\n')
			break;
		if(c == '\t')
		{
			if(allowMenus)
			{
				int option = menu();
			}
			continue;
		}
		switch(c)
		{
			case '\b':
				buffer[cursor--] = 0;
				putc('\b');
				break;
			default:
				buffer[cursor++] = c;
				putc(c);
				break;
		}
		if(cursor > length) {
			length = cursor;
		}
	}
	putc('\n');
	buffer[length] = 0;
	return length;
}

void shell_start()
{
	while(true)
	{
		static char line[256];
		int len = readline(line, true);
		printf("You entered %d chars:\n\"%s\"\n", len, line);
	
	}
}