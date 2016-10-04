#pragma once

#include <stdbool.h>

#define MAINMENU_CATALOG 0
#define MAINMENU_CHARMAP 1
#define MAINMENU_SHELL   2
#define MAINMENU_SYSTEM  3

typedef struct shell shell_t;

void mainmenu_open(bool fromShell);

void mainmenu_render();

void mainmenu_initshell(shell_t * shells, int count);

void mainmenu_setshell(char const * name);

void mainmenu_shellenable(bool enable);