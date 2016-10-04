#include "shell.h"
#include "interpreter.h"
#include "standard.h"
#include "io.h"
#include "catalog.h"
#include "charmap.h"
#include "mainmenu.h"

shell_t *currentShell = NULL;

#define currshell (*currentShell)

void select_shell(shell_t * shell)
{
	if(shell == NULL) return;
	currentShell = shell;
	
	mainmenu_setshell(shell->name);
	
	console_set(currshell.console);
}

void shell_init(int shellCount)
{
	shell_t shells[shellCount];
	for(int i = 0; i  < shellCount; i++)
	{
		shell_t *shell = malloc(sizeof(shell_t));
		
		str_printf(shell->name, "Shell %d", i);
		str_copy(shell->prompt, "#> ");
		shell->console = console_new();
		shell->input[0] = 0;
		shell->cursor = 0;
		shell->flags = SHELL_ECHO;

		if(i == 0) {
			select_shell(shell);
		}
		
		console_printf(shell->console, "%s", shell->prompt);
	}
	
	mainmenu_initshell(shells, shellCount);
	
	mainmenu_render();
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
				mainmenu_open(true);
				char const * insertion = catalog_get();
				if(insertion != NULL)
				{
					while(*insertion) {
						putc(*insertion);
						currshell.input[currshell.cursor++] = *insertion++;
					}
				}
				int cmap = charmap_last();
				if(cmap >= 0) {
					currshell.input[currshell.cursor++] = cmap;
					putc(cmap);
				}
				
				mainmenu_shellenable(true);
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