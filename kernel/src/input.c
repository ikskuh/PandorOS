#include "input.h"
#include "console.h"
#include "standard.h"

#include <stdbool.h>

int input_textfield(char *str, int x, int y, int len, int flags)
{
	char *work = malloc(MAX_TEXTFIELD_LENGTH);
	str_copy(work, str);
	
	int cursor = str_len(str);
	
	int prevFlags = stdcon->flags;
	stdcon->flags &= ~CON_NOCURSOR;
	
	while(true)
	{
		for(int i = 0; i < len; i++) {
			int idx = stdcon->width * y + x + i;
			if(i < cursor)
				stdcon->data[idx].c = work[i];
			else
				stdcon->data[idx].c = ' ';
			stdcon->data[idx].attribs = CHA_HIGHLIGHT;
		}
		stdcon->cursor.x = x + cursor;
		stdcon->cursor.y = y;
		console_refresh();
	
		keyhit_t hit = getkey(true);
		if((hit.flags & (khfKeyPress | khfCharInput)) == 0)
			continue;
		
		if(hit.flags & khfKeyPress)
		{
			switch(hit.key)
			{
				case VK_BACKSPACE:
					if(cursor <= 0)
						continue;
					work[--cursor] = 0;
					continue;
				case VK_RETURN:
				case VK_TAB:
					str_copy(str, work);
					// Pass through
				case VK_ESCAPE:
					if(flags & TEXTFIELD_NOCANCEL)
						str_copy(str, work);
					stdcon->flags = prevFlags;
					free(work);
					for(int i = 0; i < len; i++) {
						int idx = stdcon->width * y + x + i;
						stdcon->data[idx].attribs = CHA_DEFAULT;
					}
					return hit.key;
			}
		}
		
		if(hit.flags & khfCharInput)
		{
			// TODO: Input text here
			work[cursor++] = hit.codepoint;
			work[cursor] = 0;
		}
	}
	
	
}