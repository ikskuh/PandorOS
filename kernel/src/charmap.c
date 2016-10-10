#include "charmap.h"
#include "io.h"
#include "standard.h"
#include "debug.h"
#include "console/utils.h"

static int last = -1;

static int cx = 0;
static int cy = 0;

void charmap_open()
{
	console_t *prevcon = stdcon;
	console_t *chcon = console_new();
	chcon->flags &= ~CON_AUTOREFRESH;
	chcon->flags |= CON_NOCURSOR;
	console_set(chcon);
	
	last = -1;
	
	while(true)
	{
		cls();
		
		int left = (chcon->width - 34) / 2;
		int top = (chcon->height - 17) / 2 - 1;
		
		for(int i = 0; i < 16; i++) {
			setc(
				left + 2 * i + 2,
				top, 
				("0123456789ABCDEF")[i]);
			setc(
				left, 
				top + i + 1,
				("0123456789ABCDEF")[i]);
		}
		
		for(int y = 0; y < 16; y++) {
			for(int x = 0; x < 16; x++) {
				setc(
					left + 2 * x + 2, 
					top + y + 1, 
					(y << 4) | x);
				if(cx == x && cy == y) {
					console_seta(chcon, left + 2 * x + 2, top + y + 1, CHA_HIGHLIGHT);
				}
			}
		}
		
		console_box(chcon, left - 1, top - 1, 35, 18);
		
		console_refresh();
	
		keyhit_t hit = getkey(true);
		if((hit.flags & khfKeyPress) == 0)
			continue;
		
		switch(hit.key)
		{
			case VK_LEFT:
				if(cx > 0)
					cx--;
				else
					cx = 15;
				break;
			case VK_RIGHT:
				if(cx < 15)
					cx++;
				else
					cx = 0;
				break;
			case VK_UP:
				if(cy > 0)
					cy--;
				else
					cy = 15;
				break;
			case VK_DOWN:
				if(cy < 15)
					cy++;
				else
					cy = 0;
				break;
			case VK_RETURN:
				if(cx == 0 && cy == 0) // Disallow \0
					break;
				last = (cy << 4) | cx;
			case VK_ESCAPE:
				console_delete(chcon);
				console_set(prevcon);
				console_refresh();
				return;
		}
	}
}

int charmap_last()
{
	int l = last;
	last = -1;
	return l;
}