#include "shell.h"
#include "catalog.h"
#include "options.h"
#include "menu.h"
#include "interpreter.h"
#include "standard.h"
#include "io.h"

static void select_shell(shell_t * shell);

shell_t *currentShell = NULL;

#define currshell (*currentShell)

menu_t mainmenu = {
	3,
	(menuitem_t[]) {
		{ "Catalog", MENU_DEFAULT, &catalog_open, NULL, 0, NULL },
		{ "Screen ?", MENU_DEFAULT, NULL, NULL, 0, NULL },
		{ "System", MENU_DEFAULT | MENU_RIGHTALIGN, NULL, NULL,
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

static void select_shell(shell_t * shell)
{
	if(shell == NULL) return;
	currentShell = shell;
	
	str_copy(mainmenu.items[1].label, shell->name);
	
	console_set(currshell.console);
}

void shell_init(int shellCount)
{
	mainmenu.items[1].length = shellCount;
	mainmenu.items[1].items = malloc(shellCount * sizeof(menuitem_t));
	for(int i = 0; i  < shellCount; i++)
	{
		shell_t *shell = malloc(sizeof(shell_t));
		
		str_printf(shell->name, "Shell %d", i);
		str_copy(shell->prompt, "#> ");
		shell->console = console_new();
		shell->input[0] = 0;
		shell->cursor = 0;
		shell->flags = SHELL_ECHO;

		str_copy(mainmenu.items[1].items[i].label, shell->name);
		mainmenu.items[1].items[i].callback = (menucallback_f)select_shell;
		mainmenu.items[1].items[i].userdata = shell;
		
		if(i == 0) {
			select_shell(shell);
		}
		
		console_printf(shell->console, "%s", shell->prompt);
	}
	
	menu_render(&mainmenu);
}


void shell_main()
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
				
				if(basic_lasterror() != ERR_SUCCESS)
				{
					printf("ERROR: %s\n", basic_err_to_string(basic_lasterror()));
				}
				else
				{
					if(basic_isnull(result) == false && (currshell.flags & SHELL_ECHO)) {
						printf("= %v\n", result);
					}
				}
				printf("%s", currshell.prompt);
				break;
			default:
				currshell.input[currshell.cursor++] = c;
				putc(c);
				break;
		}
	}
}