#include "catalog.h"
#include "io.h"
#include "standard.h"
#include "console/utils.h"

#include <stddef.h>

typedef struct cataloginfo
{
	char const * paste;
	char const * synopsis;
	char const * documentation;
} cataloginfo_t;

static cataloginfo_t entries[] = {
	{ "Output(", "Output(x, y, value)",   "This function..." },
	{ "Input(",  "Input(x, y, variable)", "This function blabla..." },
	
	{ "a", "Bla", "Blabla" },
	{ "b", "Bla", "Blabla" },
	{ "c", "Bla", "Blabla" },
	{ "d", "Bla", "Blabla" },
	{ "e", "Bla", "Blabla" },
	{ "f", "Bla", "Blabla" },
	{ "g", "Bla", "Blabla" },
	{ "h", "Bla", "Blabla" },
	{ "i", "Bla", "Blabla" },
	{ "j", "Bla", "Blabla" },
	{ "k", "Bla", "Blabla" },
	{ "l", "Bla", "Blabla" },
	{ "m", "Bla", "Blabla" },
	{ "n", "Bla", "Blabla" },
	{ "o", "Bla", "Blabla" },
	{ "p", "Bla", "Blabla" },
	{ "q", "Bla", "Blabla" },
	{ "r", "Bla", "Blabla" },
	{ "s", "Bla", "Blabla" },
	{ "t", "Bla", "Blabla" },
	{ "Print(", "Print(value)", "Prints the given value and appends a newline." },
	{ "u", "Bla", "Blabla" },
	{ "v", "Bla", "Blabla" },
	{ "w", "Bla", "Blabla" },
	{ "x", "Bla", "Blabla" },
	{ "y", "Bla", "Blabla" },
	{ "z", "Bla", "Blabla" },
	{ "A", "Bla", "Blabla" },
	{ "B", "Bla", "Blabla" },
	{ "C", "Bla", "Blabla" },
	{ "D", "Bla", "Blabla" },
	{ "E", "Bla", "Blabla" },
	{ "F", "Bla", "Blabla" },
	{ "G", "Bla", "Blabla" },
	{ "I", "Bla", "Blabla" },
	{ "J", "Bla", "Blabla" },
	{ "K", "Bla", "Blabla" },
	{ "L", "Bla", "Blabla" },
	{ "M", "Bla", "Blabla" },
	{ "N", "Bla", "Blabla" },
	{ "O", "Bla", "Blabla" },
	{ "P", "Bla", "Blabla" },
	{ "Q", "Bla", "Blabla" },
	{ "R", "Bla", "Blabla" },
	{ "S", "Bla", "Blabla" },
	{ "T", "Bla", "Blabla" },
	{ "U", "Bla", "Blabla" },
	{ "V", "Bla", "Blabla" },
	{ "W", "Bla", "Blabla" },
	{ "X", "Bla", "Blabla" },
	{ "Y", "Bla", "Blabla" },
	{ "Z", "Bla", "Blabla" },

};

#define CATALOG_SIZE (sizeof(entries) / sizeof(entries[0]))

static int selection = -1;

static int leftside_width = 0;

static console_t *console;

char const *catalog_get()
{
	if(selection < 0) return NULL;
	char const * entry = entries[selection].paste;
	selection = -1;
	return entry;
}

void catalog_init()
{
	console = console_new();
	console->flags &= ~CON_AUTOREFRESH;
	console->flags |=  CON_NOCURSOR;
	selection = -1;
	
	for(int i = 0; i < CATALOG_SIZE; i++)
	{
		leftside_width = max(leftside_width, str_len(entries[i].paste));
	}
};

void catalog_open()
{
	console_t *oldcon = stdcon;
	
	console_set(console);
	
	int cursor = 0;
	int scroll = 0;
	
	while(true)
	{
		// render
		cls();
		
		console_box(stdcon, 0, 0, leftside_width + 4, stdcon->height);
		
		// Synopsis box
		console_box(
			stdcon, 
			leftside_width + 4, 
			0, 
			stdcon->width - leftside_width - 4,
		 3);
		
		console_box(stdcon, 0, 0, stdcon->width, stdcon->height);
		
		console_setc(stdcon, leftside_width + 3, 0, 0xC2);
		console_setc(stdcon, leftside_width + 3, stdcon->height - 1, 0xC1);
		
		console_setc(stdcon, leftside_width + 3, 2, 0xC3);
		console_setc(stdcon, stdcon->width - 1,  2, 0xB4);
		
		for(int i = 0; i < (stdcon->height - 2); i++)
		{
			int idx = scroll + i;
			if(idx >= CATALOG_SIZE)
				break;
		
			setcursor(2, 1 + i);
			puts(entries[idx].paste);
			
			if(idx == cursor) {
				for(int j = 1; j <= leftside_width + 2; j++) {
					console->data[(i + 1) * console->width + j].attribs = CHA_HIGHLIGHT;
				}
			}
		}
		
		setcursor(leftside_width + 5, 1);
		puts(entries[cursor].synopsis);
		
		setcursor(leftside_width + 5, 3);
		puts(entries[cursor].documentation);
		
		if(scroll > 0)
		{
			setc(0, 1, 0x18);
		}
		if(scroll <= (CATALOG_SIZE - stdcon->height + 1))
		{
			setc(0, stdcon->height - 2, 0x19);
		}
		
		console_refresh();
		
		// update
		keyhit_t hit = getkey(true);
		if((hit.flags & khfKeyPress) == 0)
			continue;
		
		switch(hit.key)
		{
			case VK_ESCAPE:
				selection = -1;
				console_set(oldcon);
				return;
			case VK_UP:
				if(cursor > 0) cursor--;
				break;
			case VK_DOWN:
				if(cursor < (CATALOG_SIZE - 1)) cursor++;
				break;
		}
		
		while(scroll <= (CATALOG_SIZE - stdcon->height + 1) && (cursor - scroll) > (stdcon->height - 4)) {
			scroll++;
		}
		while(scroll > 0 && scroll > (cursor - 1)) {
			scroll--;
		}
	}
}
