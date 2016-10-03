#pragma once

typedef struct console console_t;

void hal_console_init(int *w, int *h);

void hal_set_cursor(int x, int y);

void hal_render_raw(int x, int y, char c, int attribs);

void hal_render_console(console_t const * con, int x, int y, int w, int h);

void hal_debug(char const *fmt, ...);


/**
 * Returns the number of block devices.
 */
int hal_devcount();

/**
 * Returns the name of the block device.
 */
char const * hal_devname(int devid);

/**
 * Returns the block size of the given device.
 */
int hal_blocksize(int devid);
 
/**
 * Reads a block from the block device.
 */
void hal_read_block(int devid, int block, void * data);

/**
 * Writes a block to the block device.
 */
void hal_write_block(int devid, int block, void const * data);