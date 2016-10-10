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
#include "storage.h"
#include "longjmp.h"
#include "hal.h"

#define AUTORUN_FILE "AUTORUN.BAS"

int ticks = 0;

void os_tick()
{
	ticks++;
	// int_to_string(mainmenu[5].label, ticks, 10);
	// console_refresh();
}

jmp_buf errorhandler;

static error_t lastError = ERR_SUCCESS;

error_t basic_lasterror()
{
	return lastError;
}

void os_init()
{
	// This allows us to catch all basic_errors that will happen during initiliazation.
	lastError = (error_t)setjmp(errorhandler);
	if(lastError != ERR_SUCCESS)
	{
		hal_debug("Error while initialization: %s\n", basic_err_to_string(basic_lasterror()));
		while(1);
	}
	
	{ // Initialize pmm-dependent
		
		basic_init();
		stdlib_init();
		
		var_init();
	}
	
	malloc_init();
	
	{ // Initialize malloc-dependent
		console_init();
		
		options_init();
		catalog_init();
		
		storage_init();
		file_init();
	
		shell_init(4);
	}
	
	// Make some error wrapper for init script:
	lastError = (error_t)setjmp(errorhandler);
	if(lastError == ERR_SUCCESS)
	{
		file_t *autorun = file_get(AUTORUN_FILE, FILE_DEFAULT);
		if(autorun != NULL)
		{
			dynmem_t bytecode = basic_compile(
				file_data(autorun), 
				file_size(autorun));
			// TODO: Fix memory leak on error
			basic_execute2(bytecode.ptr, bytecode.cursor);
			dynmem_free(&bytecode);
		}
		
		console_refresh();
	}
	else
	{
		printf("Failed to run " AUTORUN_FILE ": %s\n", basic_err_to_string(lastError));
	}
	
	while(true)
	{
		// This will override the initialization error handler and will loop the OS forever :)
		lastError = (error_t)setjmp(errorhandler);
		if(lastError == ERR_SUCCESS)
		{
			shell_main();
		}
		else
		{
			printf("ERROR: %s\n", basic_err_to_string(lastError));
		}
	}
	
	while(true);
}