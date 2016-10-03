#include "blockdev.h"
#include "debug.h"

void blockdev_init()
{
	int cnt = hal_devcount();
	for(int i = 0; i < cnt; i++)
	{
		debug("Blockdev[%d] = %s\n", i, hal_devname(i));
	}
}
