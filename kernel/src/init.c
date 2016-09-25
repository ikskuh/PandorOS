#include <stddef.h>
#include <stdbool.h>

#include "console.h"
#include "input.h"
#include "stdlib.h"
#include "pmm.h"

int ticks = 0;

void os_tick()
{
	ticks++;
	// int_to_string(mainmenu[5].label, ticks, 10);
	// console_refresh();
}

menuitem_t mainmenu_contents[] = {
	{ "Screen 0", MENU_DEFAULT },
	{ "Screen 1", MENU_DEFAULT },
	{ "Screen 2", MENU_DEFAULT },
	{ "Catalog",  MENU_RIGHTALIGN },
	{ "System",   MENU_RIGHTALIGN },
};
menu_t mainmenu = {
	sizeof(mainmenu_contents) / sizeof(mainmenu_contents[0]),
	mainmenu_contents,
};

static struct {
	console_t *console;
	char input[128];
	int cursor;
} shells[3];

int currentShell = 0;
#define currshell shells[currentShell]
static char const * shell_prompt = "#> ";

static bool catalog();
static char const *catalog_result = NULL;


static void menu_select(menu_t *menu, int index)
{
	for(int i = 0; i < menu->length; i++) {
		if(i == index) {
			menu->items[i].flags |= MENU_SELECTED;
		} else {
			menu->items[i].flags &= ~MENU_SELECTED;
		}
		if(i == currentShell) {
			menu->items[i].flags |= MENU_RED;
		} else {
			menu->items[i].flags &= ~MENU_RED;
		}
	}
	console_menu(menu);
}

static void menu_mark(menu_t *menu)
{
	for(int i = 0; i < menu->length; i++) {
		if(i == currentShell) {
			menu->items[i].flags |= MENU_RED;
		} else {
			menu->items[i].flags &= ~MENU_RED;
		}
	}
	console_menu(menu);
}

static void select_shell(int shellId)
{
	if(shellId < 0 || shellId > 2)
		return;
	currentShell = shellId;
	
	console_set(currshell.console);
	menu_mark(&mainmenu);
}


static int menu_loop()
{
	// Start with the current shell selected.
	int cursor = currentShell;
	while(true)
	{
		menu_select(&mainmenu, cursor);
	
		keyhit_t hit = getkey(true);
		if((hit.flags & khfKeyPress) == 0)
			continue;
		switch(hit.key)
		{
			case VK_LEFT: 
				if(cursor > 0) cursor--;
				break;
			case VK_RIGHT:
				if(cursor < mainmenu.length - 1) cursor++;
				break;
			case VK_ESCAPE:
			case VK_TAB:
				menu_select(&mainmenu, -1);
				return 0;
			case VK_RETURN:
			case VK_SPACE:
				switch(cursor)
				{
					case 0:
					case 1:
					case 2:
						select_shell(cursor);
						menu_select(&mainmenu, -1);
						return 1; // shell change
					case 3: {
						int result = catalog();
						menu_select(&mainmenu, -1);
						if(result)
							return 2; // catalog
						else
							return 0; // nothing
					}
				}
				break;
		}
	}
	return 0;
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
				int selection = menu_loop();
				switch(selection)
				{
					case 1:   // Shell changed.
						break;
					case 2: { // Catalog selection
						char *str = catalog_result;
						while(*str) {
							putc(*str);
							currshell.input[currshell.cursor++] = *str++;
						}
						break;
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
				currshell.input[currshell.cursor] = 0;
				currshell.cursor = 0;
				int len = 0;
				char *str = currshell.input;
				while(*str++) len++;
				printf("\nYou entered: \"%s\" (Length=%d)\n", currshell.input, len);
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
	
	for(int i = 0; i < 3; i++)
	{
		shells[i].console = console_new();
		shells[i].input[0] = 0;
		shells[i].cursor = 0;
		
		console_printf(shells[i].console, "%s", shell_prompt);
	}
	
	console_menu(&mainmenu);
	
	menu_select(&mainmenu, -1);

	select_shell(0);
	
	shell_main();
	
	shell_main();
	
	while(true);
}

static void test_printf()
{
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
	
	printf("PMM Testing:\n");
	{
		page_t p0, p1;
		printf("pmm_alloc[0] = %d\n", pmm_alloc());
		printf("pmm_alloc[1] = %d\n", p0 = pmm_alloc());
		printf("pmm_alloc[2] = %d\n", pmm_alloc());
		printf("pmm_alloc[3] = %d\n", p1 = pmm_alloc());
		pmm_free(p0);
		printf("pmm_alloc[4] = %d\n", pmm_alloc());
	}
}

static char const * catalog_selections[] = {
	"Hallo",
	"Welt",
	"Teststring",
	"Something Wicked!",
	"Output(",
	"Real(",
	"Imag(",
	"Conj(",
	"SetPixel(",
	"GetPixel(",
	"ClearScreen(",
	"ClearHome",
	"a",
	"b",
	"c",
	"d",
	"e",
	"f",
	"g",
	"h",
	"i",
	"j",
	"k",
	"l",
	"m",
	"n",
	"o",
	"p",
	"q",
	"r",
	"s",
	NULL,
};

static bool catalog()
{
	console_t *prev = stdcon;
	console_t *catcon = console_new();
	
	console_set(catcon);
	
	int offset = 0;
	int cursor = 0;
	while(true)
	{
		// Render catalog
		cls();
		for(int i = 0; i < (catcon->height - 1); i++)
		{
			if(catalog_selections[offset + i] == NULL)
				break;
			if((offset + i) == cursor)
				printf("->");
			else
				printf("  ");
			printf(" %s\n", catalog_selections[offset + i]);
		}
		
		keyhit_t hit;
		do {
			hit = getkey(true);
		} while((hit.flags & khfKeyPress) == 0);
		
		switch(hit.key)
		{
			case VK_UP:
				if(cursor > 0)
					cursor--;
				break;
			case VK_DOWN:
				if(catalog_selections[cursor + 1] != NULL)
					cursor++;
				break;
			case VK_LEFT:
				for(int i = 0; i < (catcon->height - 2); i++) {
					if(cursor > 0) cursor--;
				}
				break;
			case VK_RIGHT:
				for(int i = 0; i < (catcon->height - 2); i++) {
					if(catalog_selections[cursor + 1] != NULL)
						cursor++;
				}
				break;
			case VK_RETURN:
				catalog_result = catalog_selections[cursor];
				console_set(prev);
				console_delete(catcon);
				return true;
			case VK_ESCAPE:
				catalog_result =  NULL;
				console_set(prev);
				console_delete(catcon);
				return false;
		}
		if(cursor < offset)
			offset = cursor;
		if(cursor > (offset + catcon->height - 2))
			offset = cursor - catcon->height + 2;
	}
}