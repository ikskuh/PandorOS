#include <stdbool.h>

#include "console.h"

void os_init()
{
	// TODO: Initialize OS…
	console_init();
	
	printf("Starting the OS...\n");
	
	printf("String: %s\n", "Hello World!");
	printf("Numbers: %b %d %x\n", 14, 15, 13);
	printf("Numbers: %b %d %x\n", -14, -15, -13);
	
	while(true);
}