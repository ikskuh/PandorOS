#pragma once

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