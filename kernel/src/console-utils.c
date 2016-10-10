#include "console/utils.h"
#include "hal.h"
#include <stdbool.h>

void console_box(console_t *con, int _x, int _y, int width, int height)
{
	for(int x = _x; x < (_x + width); x++)
	{
		int y = (_y + height - 1);
		if(x == _x) {
			console_setc(con, x, _y, 0xDA);
			console_setc(con, x, y, 0xC0);
		} else if(x == (_x + width - 1)) {
			console_setc(con, x, _y, 0xBF);
			console_setc(con, x, y, 0xD9);
		} else {
			console_setc(con, x, _y, 0xC4);
			console_setc(con, x, y, 0xC4);
		}
	}
	for(int y = _y + 1; y < (_y + height - 1); y++)
	{
		int x = (_x + width - 1);
		console_setc(con, _x, y, 0xB3);
		console_setc(con, x, y, 0xB3);
	}
}

void console_textbox(
	console_t *con, 
	int sx, 
	int sy, 
	int width, 
	int height, 
	char const *text, int flags)
{
	int x = 0;
	int y = 0;
	int chattr = CHA_DEFAULT;
	bool inWord = false;
#define CHAR(_x,_y) con->data[(sy + _y) * con->width + (sx + _x)]
	while(*text)
	{
		char c = *text++;
		
		if(c == '*' && (flags & TEXTBOX_MARKDOWN)) {
			if(*text == '*') {
				text++;
			} else {
				if(chattr & CHA_BOLD) {
					chattr &= ~CHA_BOLD;
				} else {
					chattr |= CHA_BOLD;
				}
				continue;
			}
		}
		
		if(flags & TEXTBOX_WORDWRAP)
		{
			if(c == ' ') {
				inWord = false;
			} else {
				if(inWord == false) {
					int peek = 0;
					char const * ptr = text - 1;
					while(*ptr && *ptr != ' ') {
						if(!((flags & TEXTBOX_MARKDOWN) && (ptr[0] == '*') && (ptr[1] != '*')))
							peek++;
						ptr++;
					}
					if(x + peek > width) {
						x = 0;
						y += 1;
						if(y >= height) {
							return;
						}
					}
				}
				inWord = true;
			}
		}
		
		if(c == '\n') {
			// Trigger a newline
			x = width;
		} else {
			CHAR(x, y).c = c;
			CHAR(x, y).attribs = chattr;
			x += 1;
		}
		if(x >= width) {
			x = 0;
			y += 1;
		}
		if(y >= height) {
			return;
		}
	}
}