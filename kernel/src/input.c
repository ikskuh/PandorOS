#include "input.h"
#include "console.h"

#define BUFFER_LEN 64

static int buffer[BUFFER_LEN];

volatile static int rcursor = 0;
volatile static int wcursor = 0;

int getchar()
{
	while(wcursor <= rcursor);
	int result = buffer[rcursor++];
	if(rcursor == wcursor) {
		rcursor = 0;
		wcursor = 0;
	}
	return result;
}

void putchar(int c)
{
	buffer[wcursor++] = c;
}