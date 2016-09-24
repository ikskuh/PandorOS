#include <stdbool.h>

#include "console.h"
#include "input.h"

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

void shell_start();

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
	
	putchar('H');
	putchar('a');
	putchar('l');
	putchar('l');
	putchar('o');
	putchar('W');
	putchar('\b');
	putchar(' ');
	putchar('W');
	putchar('e');
	putchar('l');
	putchar('t');
	putchar('!');
	putchar('\n');
	
	shell_start();
	
	while(true);
}

int readline(char *buffer)
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
		int len = readline(line);
		printf("You entered %d chars:\n\"%s\"\n", len, line);
	
	}
}