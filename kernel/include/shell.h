#pragma once

#define SHELL_DEFAULT 0
#define SHELL_ECHO    (1<<0)

typedef struct
{
	console_t *console;
	char input[128];
	int cursor;
	bool flags;
} shell_t;