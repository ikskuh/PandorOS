#include "shell.h"
#include "interpreter.h"
#include "standard.h"
#include "io.h"
#include "catalog.h"
#include "charmap.h"
#include "mainmenu.h"
#include "debug.h"

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
	shell_t * shells = malloc(shellCount * sizeof(shell_t));
	for(int i = 0; i  < shellCount; i++)
	{
		shell_t *shell = &shells[i];
		
		str_printf(shell->name, "Shell %d", i);
		str_copy(shell->prompt, "#> ");
		shell->console = console_new();
		shell->input[0] = 0;
		shell->cursor = 0;
		shell->flags = SHELL_ECHO;

		if(i == 0) {
			select_shell(shell);
		}
	}
	
	mainmenu_initshell(shells, shellCount);
	
	mainmenu_render();
}

static void shell_readprompt()
{
	while(true)
	{
		keyhit_t hit = getkey(true);
		
		if(hit.flags & khfKeyPress)
		{
			switch(hit.key)
			{
				case VK_META:
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
					continue;
				}
				case VK_TAB:
				{
					// Do autocompletion here!
					continue;
				}
			}
		}
		
		if((hit.flags & khfCharInput) == 0)
			continue;
		
		int c = hit.codepoint;
		switch(c)
		{
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
				
				return;
			default:
				currshell.input[currshell.cursor++] = c;
				putc(c);
				break;
		}
	}
}

static void shell_execute()
{
	mainmenu_shellenable(false);
				
	value_t result = basic_execute(currshell.input);
	
	mainmenu_shellenable(true);
	
	if(basic_isnull(result) == false && (currshell.flags & SHELL_ECHO)) {
		printf("= %v\n", result);
	}
}

void shell_main()
{
	mainmenu_shellenable(true);
	while(true)
	{
		// Start by printing the prompt.
		printf("%s", currshell.prompt);
	
		shell_readprompt();
		
		shell_execute();
	}
}