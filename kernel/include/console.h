#pragma once

/**
 * Initializes the text console.
 */
void console_init();

/**
 * Moves the cursor if possible.
 */
void console_move(int x, int y);

/**
 * Clears the screen.
 */
void cls();

/**
 * Puts a string to the console.
 */
void puts(char const *string);

/**
 * Puts a character on the screen.
 */
void putc(char c);

/**
 * Classic printf function for debugging.
 */ 
int printf(char const *fmt, ...);