#pragma once

typedef struct console console_t;

void hal_console_init(int *w, int *h);

void hal_set_cursor(int x, int y);

void hal_render_raw(int x, int y, char c, int attribs);

void hal_render_console(console_t const * con, int x, int y, int w, int h);

void hal_debug(char const *fmt, ...);