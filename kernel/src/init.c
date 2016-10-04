#include <stddef.h>
#include <stdbool.h>

#include "menu.h"
#include "standard.h"
#include "io.h"
#include "options.h"
#include "pmm.h"
#include "alloc.h"
#include "interpreter.h"
#include "catalog.h"
#include "malloc.h"
#include "shell.h"
#include "file.h"
#include "blockdev.h"

int ticks = 0;

void os_tick()
{
	ticks++;
	// int_to_string(mainmenu[5].label, ticks, 10);
	// console_refresh();
}

void os_init()
{
	{ // Initialize pmm-dependent
		console_init();
		options_init();
		catalog_init();
		
		basic_init();
		stdlib_init();
		
		var_init();
	}
	
	malloc_init();
	
	{ // Initialize malloc-dependent
		blockdev_init();
		
		file_init();
	
		shell_init(4);
	}
	
	while(true)
	{
		shell_main();
	}
	
	while(true);
}