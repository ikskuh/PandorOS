#pragma once

#include "console.h"

#include <stdbool.h>

#define SHELL_DEFAULT 0
#define SHELL_ECHO    (1<<0)

typedef struct shell
{
	char name[64];
	char prompt[64];
	console_t *console;
	char input[128];
	int cursor;
	int flags;
} shell_t;

/**
 * Initializes the shell system.
 **/
void shell_init(int shells);

/**
 * Starts the shell interface with mainmenu and command line.
 */
void shell_main();

/**
 * Selects another shell.
 */
void select_shell(shell_t * shell);

extern shell_t *currentShell;