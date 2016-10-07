#pragma once

#define HEXEDIT_DEFAULT  0
#define HEXEDIT_READONLY (1<<0)

/**
 * Opens the hex editor
 */
void hexedit_open(void * buf, int size, int flags);