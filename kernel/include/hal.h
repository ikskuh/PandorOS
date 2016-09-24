#pragma once


// Console Functions:

enum vchar_color
{
	vcDefault,
	vcHighlight,
	vcRed,
	vcRedHighlight,
};

struct vchar
{
	char c;
	enum vchar_color color;
};

typedef struct vchar vchar;

void hal_console_init(int *w, int *h);
void hal_movecursor(int x, int y);
void hal_setchar(int x, int y, vchar c);
vchar hal_getchar(int x, int y);