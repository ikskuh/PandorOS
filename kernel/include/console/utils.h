#pragma once

#include "console.h"

#define TEXTBOX_DEFAULT  0
#define TEXTBOX_MARKDOWN (1<<0)
#define TEXTBOX_WORDWRAP (1<<1)

/**
 * Renders a simple box.
 */
void console_box(console_t *con, int x, int y, int width, int height);

/**
 * Renders a box with text and word wrapping.
 * @remarks Does not render the box border!
 */
void console_textbox(console_t *con, int x, int y, int width, int height, char const * text, int flags);