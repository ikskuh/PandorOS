#include <stdbool.h>

#include "console.h"

menu_entry_t mainmenu[] = {
	{ "Screen 0", MENU_SELECTED },
	{ "Screen 1", MENU_DEFAULT },
	{ "Screen 2", MENU_DEFAULT },
	{ "Warn", MENU_RED },
	{ "Warn!", MENU_RED | MENU_SELECTED },
	{ "16:40", MENU_RIGHTALIGN },
	{ "Right 0", MENU_RIGHTALIGN },
	{ "Right 1", MENU_RIGHTALIGN },
};

void os_init()
{
	// TODO: Initialize OS…
	console_init();
	console_setmenu(mainmenu, 8);
	
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
				if(c == '\n')
					printf("  ");
				else
					printf("%c ", c);
			}
		
		}
		printf("\n");
	}
	
	printf("a\nb\nc\nd\n");
	
	while(true);
}