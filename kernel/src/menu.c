#include "menu.h"
#include "hal.h"
#include "input.h"
#include <stddef.h>

// Export from console.c
extern int screenwidth, screenheight;

static int menucolor(menuitem_t * const item)
{
	if(item->flags & MENU_SELECTED)
		return CHA_HIGHLIGHT;
	else
		return CHA_DEFAULT;
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

typedef struct renderinfo
{
	menuitem_t *items;
	int count;
	int offset; // offset to the left
	int spacex;  // offset of the blank space
	int spacew;
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

static void render_menu(renderinfo_t *ri)
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
	if(ri->items == NULL || ri->count <= 0)
		return;
		
	int leftpad = 1;

	// MENU_DEFAULT, MENU_SELECTED ,MENU_RED, MENU_RIGHTALIGN
	for(int i = 0; i < ri->count; i++)
	{
		menuitem_t *item = &ri->items[i];
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
	for(int i = (ri->count - 1); i >= 0; i--)
	{
		menuitem_t *item = &ri->items[i];
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

static void render_dropdown(renderinfo_t *ri)
{
// #define CHA_BORDER (CHA_RED | CHA_HIGHLIGHT)
#define CHA_BORDER CHA_DEFAULT
	int width = submenuwidth(ri->items, ri->count);
	
	if(width < ri->spacew) width = ri->spacew;
	
	if(ri->offset == ri->spacex)
	{        // ┤
		// Normal mode
		hal_render_raw(ri->offset, 1, 0xB4, CHA_BORDER);
	} else { // ┬
		// Right padded with overflow to the left
		hal_render_raw(ri->offset, 1, 0xC2, CHA_BORDER);
		hal_render_raw(ri->spacex, 1, 0xD9, CHA_BORDER);
	}
	
	for(int i = 1; i < ri->spacew + 1; i++) {
		hal_render_raw(ri->spacex + i, 1, ' ', CHA_BORDER);
	}
	
	if(ri->offset + width + 2 == screenwidth) {
		if(ri->spacex + ri->spacew + 2 == screenwidth) {
			// Rightmost with rightalign
			hal_render_raw(screenwidth -1, 1, 0xB3, CHA_BORDER);
		} else {
			// Rightalign, but not rightmost
			hal_render_raw(ri->spacex + ri->spacew + 1, 1, 0xC0, CHA_BORDER);
			hal_render_raw(screenwidth - 1, 1, 0xB4, CHA_BORDER);
		}
	} else if(ri->offset + width == ri->spacex + ri->spacew) {
		// Equidistant
		hal_render_raw(ri->offset + width + 1, 1, 0xC3, CHA_BORDER);
	} else {
		// Normal state
		hal_render_raw(ri->offset + width + 1, 1, 0xC2, CHA_BORDER);
		hal_render_raw(ri->spacex + ri->spacew + 1, 1, 0xC0, CHA_BORDER);
	}
	
	hal_render_raw(ri->offset,             2 + ri->count, 0xC0, CHA_BORDER);
	hal_render_raw(ri->offset + width + 1, 2 + ri->count, 0xD9, CHA_BORDER);
	for(int x = 1; x <= width; x++)
	{
		// hal_render_raw(ri->offset + x, 1,         0xC4, CHA_DEFAULT);
		hal_render_raw(ri->offset + x, 2 + ri->count, 0xC4, CHA_BORDER);
	}
	for(int y = 1; y <= ri->count; y++)
	{
		int col = menucolor(&ri->items[y - 1]);
		
		hal_render_raw(ri->offset,             1 + y, 0xB3, CHA_BORDER);
		hal_render_raw(ri->offset + 1,         1 + y, ' ',  col);
		
		int x = 2;
		char const *str = ri->items[y - 1].label;
		while(*str)
		{
			hal_render_raw(ri->offset + (x++), 1 + y, *str++, col);
		}
		for(; x <= width; x++) {
			hal_render_raw(ri->offset + x,       1 + y, ' ',  col);
		}
		hal_render_raw(ri->offset + width + 1, 1 + y, 0xB3, CHA_BORDER);
	}
}

static menuitem_t *menu_subopen(
	void (*render)(renderinfo_t *),
	renderinfo_t *ri,
	bool isVertical)
{
	hal_set_cursor(0, screenheight);
	int cursor = 0;
	while(true)
	{
		menu_select(ri->items, ri->count, cursor);
		render(ri);
		
		keyhit_t hit = getkey(true);
		if((hit.flags & khfKeyPress) == 0)
			continue;
		
		switch(hit.key)
		{
			case VK_LEFT:
				if(!isVertical && cursor > 0) cursor--;
				break;
			case VK_RIGHT:
				if(!isVertical && cursor < (ri->count- 1)) cursor++;
				break;
			case VK_UP:
				if(isVertical && cursor > 0) cursor--;
				break;
			case VK_DOWN:
				if(isVertical && cursor < (ri->count- 1)) cursor++;
				break;
			case VK_ESCAPE:
				return NULL;
			case VK_RETURN:
			{
				menuitem_t *sel = &ri->items[cursor];
				if(sel->length == 0)
					return sel;
				
				renderinfo_t subri = {
					sel->items,
					sel->length,
					999,
					999,
					strlen(ri->items[cursor].label),
				};
				if(sel->flags & MENU_RIGHTALIGN)
				{
					subri.spacex = screenwidth - 1;
					hal_debug("Start: %d\n", subri.spacex);
					for(int i = ri->count-1; i >= cursor; i--) {
						if((ri->items[i].flags & MENU_RIGHTALIGN) == 0)
							continue;
						int w = strlen(ri->items[i].label);
						hal_debug("Segment: %d, %d, '%s'\n", subri.spacex, w, ri->items[i].label);
						subri.spacex -= w;
						subri.spacex -= 1;
					}
					hal_debug("End: %d\n", subri.spacex);
				}
				else
				{
					subri.spacex = 0;
					for(int i = 0; i < cursor; i++) {
						if(ri->items[i].flags & MENU_RIGHTALIGN)
							continue;
						subri.spacex += strlen(ri->items[i].label) + 1;
					}
				}
				
				// Adjust offset to spacex with boundary checks
				{
					int _w = submenuwidth(sel->items, sel->length);
					subri.offset = subri.spacex;
					if(subri.offset + _w >= (screenwidth - 1)) {
						subri.offset = screenwidth - _w - 2;
					}
				}
				menuitem_t *subsel = menu_subopen(
					render_dropdown, 
					&subri,
					true);
				console_refresh();
				hal_set_cursor(0, screenheight);
				if(subsel != NULL) {
					return subsel;
				}
				break;
			}
		}
	}
}

menuitem_t *menu_open(menu_t const * menu)
{
	renderinfo_t ri = {
		menu->items,
		menu->length,
		0,
		0,
		0
	};
	
	menuitem_t *result = menu_subopen(render_menu, &ri, false);
	
	menu_select(menu->items, menu->length, -1);
	
	// Render menu
	render_menu(&ri);
	
	console_refresh();
	
	if(result != NULL && result->callback != NULL) {
		result->callback(result->userdata);
	}
	
	// Rerender menu
	render_menu(&ri);
					
	return result;
}

void menu_render(menu_t const * menu)
{
	renderinfo_t ri = {
		menu->items,
		menu->length,
		0,
		0,
		0
	};
	render_menu(&ri);
}