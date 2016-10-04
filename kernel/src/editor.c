#include "editor.h"
#include "io.h"
#include "standard.h"
#include "debug.h"
#include "file.h"
#include "interpreter.h"
#include "mainmenu.h"
#include "catalog.h"
#include "charmap.h"

#include <stdbool.h>

#define LINE_LENGTH 250

typedef struct line
{
	char text[LINE_LENGTH];
	int length;
} line_t;

void editor_open(char const * fileName)
{
	switch(file_type_by_extension(fileName))
	{
		case FILE_INVALID:
		case FILE_TEXT:
		case FILE_PROGRAM:
			break;
		default:
			basic_error(ERR_INVALID_FILE);
			return;
	}
	
	console_t *prevcon = stdcon;
	console_t *edcon = console_new();
	console_set(edcon);
	edcon->flags &= ~CON_AUTOREFRESH;
	
	int lineOffset = 0;
	int lineCount = 1;
	
	file_t *file = file_get(fileName, FILE_DEFAULT);
	if(file_type(file) != FILE_INVALID)
	{
		char * data = file_data(file);
		for(int i = 0; i < file_size(file); i++)
		{
			if(data[i] == '\n')
				lineCount++;
		}
	}
	line_t *lines = zalloc(lineCount * sizeof(line_t));
	if(file_type(file) != FILE_INVALID)
	{
		char * data = file_data(file);
		
		int line = 0;
		for(int i = 0; i < file_size(file); i++)
		{
			char c = data[i];
			if(c == '\n')
			{
				line++;
			}
			else
			{
				lines[line].text[lines[line].length++] = c;
			}
		}
	}
	
	int cx = 0;
	int cy = 0;

#define PUTC(c) do { \
	if(lines[cy].length >= LINE_LENGTH || cx >= LINE_LENGTH) \
		break; \
	for(int i = lines[cy].length; i > cx; i--) { \
		lines[cy].text[i] = lines[cy].text[i - 1]; \
	} \
	lines[cy].text[cx] = c; \
	lines[cy].length += 1; \
	cx += 1; \
} while(false)

	mainmenu_shellenable(false);
	mainmenu_render();
	
	while(true)
	{
		cy = clamp(cy, 0, lineCount - 1);
		cx = clamp(cx, 0, lines[cy].length);
					
		while(cy < lineOffset) {
			lineOffset--;
		}
		
		// TODO: Improve scrolling down
		while(cy >= lineOffset + edcon->height) {
			lineOffset ++;
		}
		
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
				case VK_K:
					if(kbd_is_pressed(VK_CONTROL) == false)
						break;
					if(lineCount <= 1)
					{
						lines[0].length = 0;
						continue;
					}
					for(int i = cy + 1; i < lineCount; i++)
					{
						lines[i - 1] = lines[i];
					}
					lineCount--;
					continue;
				case VK_LEFT:
					if(cx > 0) {
						cx -= 1;
						continue;
					}
					cx = LINE_LENGTH; // This will be clamped at the start of the loop.
					// Intelligent fallthrough
				case VK_UP:
					if(cy == 0)
						continue;
					cy -= 1;
					continue;
				case VK_RIGHT:
					if(cx < lines[cy].length && cx < LINE_LENGTH) {
						cx += 1;
						continue;
					}
					cx = 0;
					// Intelligent fallthrough
				case VK_DOWN:
					if(cy >= lineCount - 1)
						continue;
					cy += 1;
					continue;
				case VK_END:
					cx = lines[cy].length;
					continue;
				case VK_HOME:
					cx = 0;
					continue;
				case VK_TAB:
				{
					mainmenu_open(false);
				
					char const * insertion = catalog_get();
					if(insertion != NULL)
					{
						while(*insertion) {
							PUTC(*insertion);
							insertion++;
						}
					}
					int cmap = charmap_last();
					if(cmap >= 0) {
						PUTC(cmap);
					}
					
					continue;
				}
				case VK_S:
				{
					if(kbd_is_pressed(VK_CONTROL) == false)
						break;
					
					int total = 0;
					for(int i = 0; i < lineCount; i++)
					{
						total += lines[i].length;
						total += 1; // '\n'
					}
					
					file = file_get(fileName, FILE_NEW);
					if(file_type(file) == FILE_INVALID) {
						debug("Invalid file: %s\n", fileName);
						continue;
					}
					
					file_resize(file, total);
					
					char * ptr = file_data(file);
					
					for(int i = 0; i < lineCount; i++)
					{
						mem_copy(ptr, lines[i].text, lines[i].length);
						ptr += lines[i].length;
						*ptr = '\n';
						ptr += 1;
					}
				
					continue;
				}
				case VK_ESCAPE:
					console_delete(edcon);
					console_set(prevcon);
					return;
			}
		}
		if(hit.flags & khfCharInput)
		{
			PUTC(hit.codepoint);
		}
	}
}