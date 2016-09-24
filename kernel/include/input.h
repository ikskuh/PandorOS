#pragma once

#define VK_ESCAPE 0
#define VK_LEFT 1
#define VK_RIGHT 2
#define VK_UP 3
#define VK_DOWN 4
#define VK_TAB '\t'
#define VK_ENTER '\n'


/**
 * Blocks until a character is available at the keyboard.
 * @returns The character code that was beeing input.
 */
int getchar();

/**
 * Puts a character into the getchar queue.
 * @param c The character code.
 */
void putchar(int c);