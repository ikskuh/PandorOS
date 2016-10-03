#include "editor.h"
#include "io.h"
#include "standard.h"
#include "debug.h"

#define LINE_LENGTH 80

typedef struct line
{
	char text[LINE_LENGTH];
	int length;
} line_t;

void editor_open(char const * fileName)
{
	console_t *prevcon = stdcon;
	console_t *edcon = console_new();
	console_set(edcon);
	edcon->flags &= ~CON_AUTOREFRESH;
	
	int lineOffset = 0;
	int lineCount = 1;
	line_t *lines = malloc(lineCount * sizeof(line_t));
	lines[0].length = 0;
	
	
	int cx = 0;
	int cy = 0;
	
	while(true)
	{
		cls();
		
		int top = lineOffset;
		int bottom = min(lineCount, lineOffset + edcon->height);

		for(int l = top; l < bottom; l++)
		{
			line_t *line = &lines[l];
	
			for(int i = 0; i < line->length; i++) {
				setc(i, l - lineOffset, line->text[i]);
			}
		}
		
		edcon->cursor.x = cx;
		edcon->cursor.y = cy - lineOffset;
		
		console_refresh();
		
		keyhit_t hit = getkey(true);
		if((hit.flags & khfKeyPress) == 0 && (hit.flags & khfCharInput) == 0)
			continue;
		if(hit.flags & khfKeyPress)
		{
			switch(hit.key)
			{
				case VK_BACKSPACE:
					if(cx <= 0)
						continue;
					
					debug("key unput: (%d -> %d)\n", cx, lines[cy].length);
					
					for(int i = cx - 1; i < lines[cy].length; i++) {
						lines[cy].text[i] = lines[cy].text[i + 1];
					}
					lines[cy].length -= 1;
					
					cx -= 1;
				
					continue;
				case VK_RETURN:
					continue;
				case VK_UP:
					if(cy == 0)
						continue;
					cy -= 1;
					cx = min(cx, lines[cy].length);
					continue;
				case VK_DOWN:
					if(cy >= lineCount - 1)
						continue;
					cy += 1;
					cx = min(cx, lines[cy].length);
					continue;
				case VK_RIGHT:
					if(cx >= lines[cy].length)
						continue;
					if(cx >= LINE_LENGTH)
						continue;
					cx += 1;
					continue;
				case VK_LEFT:
					if(cx <= 0)
						continue;
					cx -= 1;
					continue;
			}
		}
		if(hit.flags & khfCharInput)
		{
			if(lines[cy].length >= LINE_LENGTH || cx >= LINE_LENGTH)
				continue;
			
			debug("key input: %c, (%d -> %d)\n", hit.codepoint, cx, lines[cy].length);
			
			for(int i = lines[cy].length; i > cx; i--) {
				lines[cy].text[i] = lines[cy].text[i - 1];
			}
			lines[cy].text[cx] = hit.codepoint;
			lines[cy].length += 1;
			
			cx += 1;
		}
	}
}