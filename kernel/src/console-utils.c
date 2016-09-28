#include "console/utils.h"

void console_box(console_t *con, int _x, int _y, int width, int height)
{
	for(int x = _x; x < (_x + width); x++)
	{
		int y = (_y + height - 1);
		if(x == 0) {
			console_setc(con, x, 0, 0xDA);
			console_setc(con, x, y, 0xC0);
		} else if(x == (_x + width - 1)) {
			console_setc(con, x, 0, 0xBF);
			console_setc(con, x, y, 0xD9);
		} else {
			console_setc(con, x, 0, 0xC4);
			console_setc(con, x, y, 0xC4);
		}
	}
	for(int y = _y + 1; y < (_y + height - 1); y++)
	{
		int x = (_x + width - 1);
		console_setc(con, 0, y, 0xB3);
		console_setc(con, x, y, 0xB3);
	}
}