#include "hal.h"
#include "io.h"
#include <stdint.h>

#define WIDTH 80
#define HEIGHT 25

struct vchar
{
	char c;
	uint8_t color;
};

static uint8_t COLOR = 0x0F;

static struct vchar * const VIDEO = (struct vchar *)0xB8000;

static int cx, cy;

static void displaycursor(uint8_t col, uint8_t row)
{
  uint16_t tmp;
  tmp = row * WIDTH + col;
  outb(0x3D4,14);
  outb(0x3D5,tmp >> 8);
  outb(0x3D4,15);
  outb(0x3D5,tmp);
}
 

void hal_console_init()
{
	cx = 0;
	cy = 0;
}

void hal_movecursor(int x, int y)
{
	if(x >= 0 && x < WIDTH)
		cx = x;
	if(y >= 0 && y < HEIGHT)
		cy = y;
	displaycursor(cx, cy);
}

void hal_cls()
{
	for(int y = 0; y < HEIGHT; y++)
	{
		for(int x = 0; x < WIDTH; x++)
		{
			VIDEO[WIDTH * y + x] = (struct vchar){ ' ', COLOR };
		}
	}
	cx = 0;
	cy = 0;
}

static void newline()
{
	cx = 0;
	cy += 1;
	
	// Scoll here
	if(cy >= HEIGHT)
	{
		for(int y = 1; y < HEIGHT; y++)
		{
			for(int x = 0; x < WIDTH; x++)
			{
				VIDEO[WIDTH * (y - 1) + x] = VIDEO[WIDTH * y + x];
			}
		}
		cy -= 1;
	}
}

void hal_putc(char c)
{
	switch(c)
	{
		case '\r':
			break;
		case '\n':
			newline();
			break;
		default:
			VIDEO[WIDTH * cy + cx] = (struct vchar){ c, COLOR };
			cx++;
			if(cx >= WIDTH) {
				newline();
			}
			break;
	}
	displaycursor(cx, cy);
}