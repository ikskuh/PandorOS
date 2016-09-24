#include "hal.h"
#include "io.h"
#include <stdint.h>

#define WIDTH 80
#define HEIGHT 25

struct _vchar
{
	char c;
	uint8_t color;
};

static struct _vchar * const VIDEO = (struct _vchar *)0xB8000;

static void displaycursor(uint8_t col, uint8_t row)
{
  uint16_t tmp;
  tmp = row * WIDTH + col;
  outb(0x3D4,14);
  outb(0x3D5,tmp >> 8);
  outb(0x3D4,15);
  outb(0x3D5,tmp);
}
 

void hal_console_init(int *w, int *h)
{
	if(w) *w = WIDTH;
	if(h) *h = HEIGHT;
}

void hal_movecursor(int x, int y)
{
	displaycursor(x, y);
}

static inline uint8_t mapColor(enum vchar_color color)
{
	switch(color)
	{
		case vcRedHighlight:
			return 0xCF;
		case vcRed:
			return 0x0C;
		case vcHighlight:
			return 0x8F;
		case vcDefault:
		default:
			return 0x0F;
	}
}

static inline enum vchar_color backmapColor(uint8_t color)
{
	switch(color)
	{
		case 0xCF: return vcRedHighlight;
		case 0xFC: return vcRed;
		case 0x7F: return vcHighlight;
		default:   return vcDefault;
	}
}

void hal_setchar(int x, int y, vchar c)
{
	VIDEO[y * WIDTH + x] = (struct _vchar) {
		c.c,
		mapColor(c.color),
	};
}

vchar hal_getchar(int x, int y)
{
	struct _vchar pt = VIDEO[y * WIDTH + x];
	return (vchar) { pt.c, backmapColor(pt.color) };
}