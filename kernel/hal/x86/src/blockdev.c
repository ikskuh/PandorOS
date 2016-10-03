#include "io.h"
#include "hal.h"
#include "string.h"

#include <stdint.h>

static inline void wait400NS(uint16_t p) 
{
	inb(p);
	inb(p);
	inb(p);
	inb(p);
}

int hal_devcount()
{
	return 4;
}

char const * hal_devname(int devid)
{
	static char names[4][8];
	str_copy(names[devid], "ATA");
	names[devid][3] = '0' + devid;
	names[devid][4] = 0;
	return names[devid];
}

int hal_blocksize(int devid) { return 512; }
 
void hal_read_block(int devid, int block, void * data)
{
	hal_debug("hal_read_block not implemented yet.\n");
	while(1);
}

void hal_write_block(int devid, int block, void const * data)
{
	hal_debug("hal_write_block not implemented yet.\n");
	while(1);
}