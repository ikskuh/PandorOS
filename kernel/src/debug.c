#include "debug.h"
#include <stdint.h>

void hexdump(char const * name, void const * mem, int size)
{
	debug("Dump of '%s':\n", name);
	uint8_t const * data = mem;
	for(int i = 0; i < size; i++)
	{
		if(i > 0 && (i % 16) == 0) {
			debug("\n");
		}
		char const * hex = "0123456789ABCDEF";
		debug("%c%c ",
			hex[(data[i]&0xF0) >> 4],
			hex[(data[i]&0x0F) >> 0]);
	}
	debug("\n");
}