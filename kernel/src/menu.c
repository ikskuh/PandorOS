#include "menu.h"
#include "hal.h"
#include "input.h"
#include <stddef.h>

// Export from console.c
extern int screenwidth, screenheight;

static int menucolor(menuitem_t * const item)
{
	if(item->flags & MENU_RED)
	{
		if(item->flags & MENU_SELECTED)
			return CHA_RED | CHA_HIGHLIGHT;
		else
			return CHA_RED;
	}
	else
	{
		if(item->flags & MENU_SELECTED)
			return CHA_HIGHLIGHT;
		else
			return CHA_DEFAULT;
	}
}

static void menu_select(menuitem_t *items, int count, int index)
{
	for(int i = 0; i < count; i++) {
		if(i == index) {
			items[i].flags |= MENU_SELECTED;
		} else {
			items[i].flags &= ~MENU_SELECTED;
		}
	}
}

struct renderinfo
{
	menuitem_t *items;
	int count;
	int offset; // offset to the left
	int spacex;  // offset of the blank space
	int specew;
} renderinfo_t;

static int strlen(char const *str) {
	int c = 0;
	while(*str++) c++;
	return c;
}

static int submenuwidth(menuitem_t *items, int count)
{
	int width = 2;
	for(int i = 0; i < count; i++) {
		int w = strlen(items[i].label) + 2;
		if(w > width) width = w;
	}
	return width;
}

static void render_menu(menuitem_t *items, int count, int offset)
{
	for(int x = 0; x < screenwidth; x++) {
		char c = ' ';
		if(x == 0)
			c = 0xB3;
		if(x == (screenwidth - 1))
			c = 0xB3;
		hal_render_raw(x, 0, c, CHA_DEFAULT);
		
		c = 0xC4;
		if(x == 0)
			c = 0xC0;
		if(x == (screenwidth - 1))
			c = 0xD9;
		hal_render_raw(x, 1, c, CHA_DEFAULT);
	}
	if(items == NULL || count <= 0)
		return;
		
	int leftpad = 1;

	// MENU_DEFAULT, MENU_SELECTED ,MENU_RED, MENU_RIGHTALIGN
	for(int i = 0; i < count; i++)
	{
		menuitem_t *item = &items[i];
		if((item->flags & MENU_RIGHTALIGN) != 0) continue;
		
		int col = menucolor(item);
		char const *str = item->label;
		while(*str) {
			hal_render_raw(leftpad++, 0, *str++, col);
		}
		hal_render_raw(leftpad, 1, 0xC1, CHA_DEFAULT);
		hal_render_raw(leftpad++, 0, 0xB3, CHA_DEFAULT);
	}
	
	int rightpad = screenwidth - 2;
	// MENU_DEFAULT, MENU_SELECTED ,MENU_RED, MENU_RIGHTALIGN
	for(int i = (count - 1); i >= 0; i--)
	{
		menuitem_t *item = &items[i];
		if((item->flags & MENU_RIGHTALIGN) == 0) continue;

		int col = menucolor(item);
		char const *str = item->label;
		int len = 0;
		while(*str++) len++;
		str = item->label;
		for(int i = len - 1; i >= 0; i--) {
			hal_render_raw(rightpad--, 0, str[i], col);
			len--;
		}
		hal_render_raw(rightpad, 1, 0xC1, CHA_DEFAULT);
		hal_render_raw(rightpad--, 0, 0xB3, CHA_DEFAULT);
	}
}

static void render_dropdown(menuitem_t *items, int count, int offset)
{
	int width = submenuwidth(items, count);
	
	hal_render_raw(offset,             1,         0xC2, CHA_DEFAULT);
	hal_render_raw(offset + width + 1, 1,         0xC2, CHA_DEFAULT);
	hal_render_raw(offset,             2 + count, 0xC0, CHA_DEFAULT);
	hal_render_raw(offset + width + 1, 2 + count, 0xD9, CHA_DEFAULT);
	for(int x = 1; x <= width; x++)
	{
		hal_render_raw(offset + x, 1,         0xC4, CHA_DEFAULT);
		hal_render_raw(offset + x, 2 + count, 0xC4, CHA_DEFAULT);
	}
	for(int y = 1; y <= count; y++)
	{
		int col = menucolor(&items[y - 1]);
		
		hal_render_raw(offset,             1 + y, 0xB3, CHA_DEFAULT);
		hal_render_raw(offset + 1,         1 + y, ' ',  col);
		
		int x = 2;
		char const *str = items[y - 1].label;
		while(*str)
		{
			hal_render_raw(offset + (x++), 1 + y, *str++, col);
		}
		for(; x <= width; x++) {
			hal_render_raw(offset + x,       1 + y, ' ',  col);
		}
		hal_render_raw(offset + width + 1, 1 + y, 0xB3, CHA_DEFAULT);
	}
}

static menuitem_t *menu_subopen(
	menuitem_t *items, int count, 
	void (*render)(menuitem_t *, int, int),
	int offset,
	bool isVertical)
{
	hal_set_cursor(0, screenheight);
	int cursor = 0;
	while(true)
	{
		menu_select(items, count, cursor);
		render(items, count, offset);
		
		keyhit_t hit = getkey(true);
		if((hit.flags & khfKeyPress) == 0)
			continue;
		
		switch(hit.key)
		{
			case VK_LEFT:
				if(!isVertical && cursor > 0) cursor--;
				break;
			case VK_RIGHT:
				if(!isVertical && cursor < (count- 1)) cursor++;
				break;
			case VK_UP:
				if(isVertical && cursor > 0) cursor--;
				break;
			case VK_DOWN:
				if(isVertical && cursor < (count- 1)) cursor++;
				break;
			case VK_ESCAPE:
				return NULL;
			case VK_RETURN:
			{
				menuitem_t *sel = &items[cursor];
				if(sel->length > 0)
				{
					int _off;
					if(sel->flags & MENU_RIGHTALIGN)
					{
						_off = screenwidth - 1;
						hal_debug("Start: %d\n", _off);
						for(int i = count-1; i >= cursor; i--) {
							if((items[i].flags & MENU_RIGHTALIGN) == 0)
								continue;
							int w = strlen(items[i].label);
							hal_debug("Segment: %d, %d, '%s'\n", _off, w, items[i].label);
							_off -= w;
							_off -= 1;
						}
						hal_debug("End: %d\n", _off);
					}
					else
					{
						_off = 0;
						for(int i = 0; i < cursor; i++) {
							if(items[i].flags & MENU_RIGHTALIGN)
								continue;
							_off += strlen(items[i].label) + 1;
						}
					}
					int _w = submenuwidth(sel->items, sel->length);
					if(_off + _w >= (screenwidth - 1)) {
						_off = screenwidth - _w - 2;
					}
					menuitem_t *subsel = menu_subopen(
						sel->items, 
						sel->length, 
						render_dropdown, 
						_off,
						true);
					console_refresh();
					hal_set_cursor(0, screenheight);
					if(subsel != NULL) {
						return subsel;
					}
				}
				else
				{
					if(sel->callback != NULL) {
						sel->callback(sel);
					}
					return sel;
				}
				break;
			}
		}
	}
}

menuitem_t *menu_open(menu_t const * menu)
{
	menuitem_t *result = menu_subopen(menu->items, menu->length, render_menu, 0, false);
	
	menu_select(menu->items, menu->length, -1);
	render_menu(menu->items, menu->length, 0);
	console_refresh();
					
	return result;
}

void menu_render(menu_t const * menu)
{
	render_menu(menu->items, menu->length, 0);
}