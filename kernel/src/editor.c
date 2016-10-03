#include "editor.h"
#include "io.h"
#include "standard.h"
#include "debug.h"

#include <stdbool.h>

#define LINE_LENGTH 250

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
	int lineCount = 3;
	line_t *lines = malloc(lineCount * sizeof(line_t));
	lines[0].length = 0;
	lines[1].length = 0;
	lines[2].length = 0;
	
	#define LINEINIT(n, t) str_copy(lines[n].text, t); lines[n].length = str_len(t)
	
	LINEINIT(0, "Hallo Editor!");
	LINEINIT(1, "Dies ist ein Editor mit Zeilenumbruch. Die Implementierung des Zeilenumbruch ist aber im Moment noch ziemlich schlecht. Trotzdem kann man super tippen, vorallem weil der Zeilenumbruch funktionieren sollte.");
	LINEINIT(2, "Von daher sollte man sich überlegen, ein Word-Wrapping einzubauen.");
	
	int cx = 0;
	int cy = 0;
	
	while(true)
	{
		cls();
		
		int top = lineOffset;
		int bottom = 0;

		edcon->cursor.x = 0;
		edcon->cursor.y = edcon->height;
		
		int y = 0;
		for(int l = top; l < lineCount; l++)
		{
			line_t *line = &lines[l];
			bottom = l;
	
			setc(0, y, ':');

			int x = 1;
			bool inWord = false;
			for(int i = 0; i < line->length; i++) {
				
				char c = line->text[i];
				
				if(c == ' ') {
					inWord = false;
				} else {
					if(inWord == false) {
						int peek = 0;
						for(int j = 0; i+j < line->length && (line->text[i + j] != ' '); j++) {
							peek++;
						}
						if(x + peek > edcon->width) {
							x = 1;
							y ++;
							if(y >= edcon->height) {
								break;
							}
						}
					}
					inWord = true;
				}
				
				if(cy == l && cx == i) {
					edcon->cursor.x = x;
					edcon->cursor.y = y;
				}
				
				setc(x++, y, c);
				if(x >= edcon->width) {
					x = 1;
					y++;
					if(y >= edcon->height) {
						break;
					}
				}
			}
			if(cy == l && cx == line->length) {
				edcon->cursor.x = x;
				edcon->cursor.y = y;
			}
			
			y++;
			if(y >= edcon->height) {
				break;
			}
		}
		
		console_refresh();
		
		keyhit_t hit = getkey(true);
		if((hit.flags & khfKeyPress) == 0 && (hit.flags & khfCharInput) == 0)
			continue;
		if(hit.flags & khfKeyPress)
		{
			switch(hit.key)
			{
				case VK_BACKSPACE:
					if(cx <= 0) {
						// TODO: User wants to delete 
						continue;
					}
					for(int i = cx - 1; i < lines[cy].length; i++) {
						lines[cy].text[i] = lines[cy].text[i + 1];
					}
					lines[cy].length -= 1;
					
					cx -= 1;
				
					continue;
				case VK_DELETE:
					if(cx >= lines->length)
						continue;
					for(int i = cx; i < lines[cy].length; i++) {
						lines[cy].text[i] = lines[cy].text[i + 1];
					}
					lines[cy].length -= 1;
				
					continue;
				case VK_RETURN:
					// Append Line in any case:
					lines = realloc(lines, (++lineCount) * sizeof(line_t));
					for(int i = lineCount - 1; i > cy; i--)
					{
						lines[i] = lines[i - 1];
					}
					lines[cy + 1].length = 0;
					for(int i = cx; i < lines[cy].length; i++) {
						lines[cy + 1].text[i - cx] = lines[cy].text[i];
						lines[cy + 1].length++;
					}
					lines[cy].length = cx;
					cy++;
					cx = 0;
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
				case VK_END:
					cx = lines[cy].length;
					continue;
				case VK_HOME:
					cx = 0;
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