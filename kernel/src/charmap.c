#include "charmap.h"
#include "io.h"
#include "standard.h"

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
		
		for(int i = 0; i < 16; i++) {
			setc(
				26 + 2 * i, 
				3, 
				("0123456789ABCDEF")[i]);
			setc(
				24, 
				4 + i, 
				("0123456789ABCDEF")[i]);
		}
		
		for(int y = 0; y < 16; y++) {
			for(int x = 0; x < 16; x++) {
				setc(
					26 + 2*x, 
					4 + y, 
					(y << 4) | x);
				if(cx == x && cy == y) {
					console_seta(chcon, 26 + 2 * x, 4 + y, CHA_HIGHLIGHT);
				}
			}
		}
		
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