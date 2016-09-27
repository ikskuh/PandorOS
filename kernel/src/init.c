#include <stddef.h>
#include <stdbool.h>

#include "console.h"
#include "menu.h"
#include "input.h"
#include "stdlib.h"
#include "pmm.h"
#include "alloc.h"
#include "interpreter.h"
#include "string.h"

int ticks = 0;

void os_tick()
{
	ticks++;
	// int_to_string(mainmenu[5].label, ticks, 10);
	// console_refresh();
}


static void catalog(menuitem_t *);
static char const *catalog_result = NULL;

static void select_shell(int shellId);

static void select_shell0() { select_shell(0); }
static void select_shell1() { select_shell(1); }
static void select_shell2() { select_shell(2); }

menu_t mainmenu = {
	3,
	(menuitem_t[]) {
		{ "Catalog", MENU_DEFAULT, &catalog, 0, NULL },
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
				{ "Network Settings",  MENU_DEFAULT, NULL, 0, NULL },
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
				catalog_result = NULL;
				// menuitem_t *selection = menu_open(&mainmenu);
				menu_open(&mainmenu);
				if(catalog_result != NULL)
				{
					char const * str = catalog_result;
					while(*str) {
						putc(*str);
						currshell.input[currshell.cursor++] = *str++;
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
	
	if(false) {  // Allocator test
		allocator_t *menuAlloc = allocator_new(sizeof(menuitem_t));
		
		void *a = allocator_alloc(menuAlloc);
		void *b = allocator_alloc(menuAlloc);
		allocator_free(menuAlloc, a);
		void *c = allocator_alloc(menuAlloc);
		void *d = allocator_alloc(menuAlloc);
		
		printf("%d, %d, %d, %d\n", a, b, c, d);
		
		allocator_delete(menuAlloc);
	}
	
	variable_t *var_a    = var_get("A");
	variable_t *var_str0 = var_get("STR0");
	
	printf("%d, %d\n", var_type(var_a), var_type(var_str0));
	
	int i = 10;
	var_setval(var_a, &i);
	var_setval(var_str0, "hallo!");
	
	int j = 55;
	var_getval(var_a, &j);
	
	char str0[64];
	var_getval(var_str0, str0);
	
	printf("%d, %s\n", j, str0);
	
	
	shell_main();
	
	while(true);
}

/*

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
*/
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

static void catalog(menuitem_t *this)
{
	(void)this;
	
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
				return;
			case VK_ESCAPE:
				catalog_result =  NULL;
				console_set(prev);
				console_delete(catcon);
				return;
		}
		if(cursor < offset)
			offset = cursor;
		if(cursor > (offset + catcon->height - 2))
			offset = cursor - catcon->height + 2;
	}
}

//*/