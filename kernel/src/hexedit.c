#include "editor.h"
#include "io.h"
#include "standard.h"
#include "debug.h"
#include "mainmenu.h"
#include "charmap.h"

#include <stdbool.h>

void hexedit_open(void * buffer, int size, int flags)
{
	console_t *prevcon = stdcon;
	console_t *edcon = console_new();
	console_set(edcon);
	edcon->flags &= ~CON_AUTOREFRESH;
	
	mainmenu_shellenable(false);
	mainmenu_render();
	
	int cursor = 0;
	int digit = 0;
	int mode = 0;
	uint8_t * data = buffer;
	while(true)
	{
		cursor = clamp(cursor, 0, size - 1);
		
		cls();
		int colwidth = edcon->width / 4;
		for(int y = 0; y < edcon->height - 2; y++)
		{
			for(int x = 0; x < colwidth; x++)
			{
				int idx = colwidth * y + x;
				if(idx >= size)
					break;
				char const * hex = "0123456789ABCDEF";
				uint8_t byte = data[idx];
				setc(3 * x + 0, y, hex[(byte >> 4) & 0xF]);
				setc(3 * x + 1, y, hex[(byte >> 0) & 0xF]);
				
				setc(3 * colwidth + x, y, byte);
			}
		}
		
		console_seta(
			edcon,
			3 * (cursor % colwidth) + 0,
			cursor / colwidth,
			CHA_HIGHLIGHT);
		console_seta(
			edcon,
			3 * (cursor % colwidth) + 1,
			cursor / colwidth,
			CHA_HIGHLIGHT);
		console_seta(
			edcon,
			3 * colwidth + (cursor % colwidth),
			cursor / colwidth,
			CHA_HIGHLIGHT);
		
		switch(mode)
		{
			case 0:
				edcon->cursor.x = 3 * (cursor % colwidth) + digit;
				break;
			case 1:
				edcon->cursor.x = 3 * colwidth + (cursor % colwidth);
				break;
		}
		edcon->cursor.y = cursor / colwidth;
		
		console_refresh();
		
		keyhit_t hit = getkey(true);
		if((hit.flags & khfKeyPress) == 0 && (hit.flags & khfCharInput) == 0)
			continue;
		if(hit.flags & khfKeyPress)
		{
			switch(hit.key)
			{
				case VK_ESCAPE:
					console_delete(edcon);
					console_set(prevcon);
					return;
				case VK_LEFT:
					if(digit != 0) break;
					if(cursor > 0) cursor--;
					continue;
				case VK_RIGHT:
					if(digit != 0) break;
					if(cursor < size - 1) cursor++;
					continue;
				case VK_UP:
					if(digit != 0) break;
					cursor = max(cursor - colwidth, 0);
					continue;
				case VK_DOWN:
					if(digit != 0) break;
					cursor = min(cursor + colwidth, size - 1);
					continue;
				case VK_TAB:
					if(digit != 0) break;
					mode = !mode;
					continue;
			}
		}
		if(hit.flags & khfCharInput)
		{
			switch(mode)
			{
				case 0:
				{
					int val = -1;
					if(hit.codepoint >= '0' && hit.codepoint <= '9')
						val = hit.codepoint - '0';
					else if(hit.codepoint >= 'a' && hit.codepoint <= 'f')
						val = hit.codepoint - 'a' + 10;
					else if(hit.codepoint >= 'A' && hit.codepoint <= 'F')
						val = hit.codepoint - 'A' + 10;
					if(val >= 0)
					{
						if(digit == 0)
						{
							uint8_t byte = data[cursor];
							byte &= 0x0F;
							byte |= (val<<4);
							data[cursor] = byte;
							
							digit = 1;
						}
						else
						{
							uint8_t byte = data[cursor];
							byte &= 0xF0;
							byte |= (val&0xF);
							data[cursor] = byte;
						
							digit = 0;
							if(cursor < size - 1) cursor++;
						}
					}
					break;
				}
				case 1:
					data[cursor] = hit.codepoint;
					if(cursor < size - 1) cursor++;
					break;
			}
		}
	}
}