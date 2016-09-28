#pragma once

#include "keyboard.h"

#define MAX_TEXTFIELD_LENGTH 256

/**
 * Opens a text input field on stcon at the given position with the given length.
 *
 * If the editing is cancelled by pressing escape, no modification will be done to the string pointer.
 *
 * @param str The string to be edited.
 * @param x   The x-coordinate of the text field.
 * @param y   The y-coordinate of the text field.
 * @param len The length of the text field.
 * @return    The virtual key code that terminated the editing. One of the following: VK_ESCAPE, VK_TAB or VK_RETURN.
 */
int input_textfield(char *str, int x, int y, int len);