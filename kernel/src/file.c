#include "file.h"
#include "malloc.h"
#include "string.h"
#include "hal.h"
#include "memory.h"
#include "debug.h"
#include "interpreter.h"

#include <stdint.h>

struct file
{
	char name[128];
	int type;
	
	void * data;
	int size;
	
	struct file * next;
};

static file_t * first = NULL;

file_t * file_first() { return first; }

file_t * file_next(file_t * it) { return it->next; }

char const * file_getname(file_t *it) { return it->name; }

void file_init()
{
	first = NULL;
	
	// Load Root-FS:
	rootfs_t const * rootfs = hal_rootfs();
	if(rootfs != NULL)
	{
		char const * ptr = rootfs->data;
		
		uint32_t magic = *((uint32_t * const)ptr);
		if(magic != 0xD05E4ABC) {
			debug("Invalid rootfs magic: %x\n", magic);
		} else {
			ptr += 0x04;
			uint32_t size = 0;
			do
			{
				size = *((uint32_t * const)ptr);
				ptr += 0x04;
				if(size > 0)
				{
					char name[16];
					mem_copy(name, ptr, 0x10);
					ptr += 0x10;
					
					file_t *f = file_get(name, FILE_NEW);
					file_resize(f, size);
					mem_copy(file_data(f), ptr, size);
					
					ptr += size;
				}
			} while(size != 0);
		}
	}
}

static file_t * file_new(char const * fileName, int type)
{
	file_t *file = malloc(sizeof(file_t));
	str_copy(file->name, fileName);
	file->type = type;
	file->data = malloc(1);
	file->size = 0;
	file->next = NULL;
	
	if(first == NULL) {
		first = file;
	} else {
		file_t *it = first;
		while(it->next) it = it->next;
		it->next = file;
	}
	
	return file;
}

int file_type_by_extension(char const * name)
{
	while(*name)
	{
		if(str_eq(name, ".PRG"))
			return FILE_PROGRAM;
		if(str_eq(name, ".DAT"))
			return FILE_DATA;
		if(str_eq(name, ".TXT"))
			return FILE_TEXT;
		name++;
	}
	return FILE_INVALID;
}

file_t * file_get(char const * fileName, int flags)
{
	int type = file_type_by_extension(fileName);
	if(type == FILE_INVALID)
		return NULL;

	for(file_t *it = first; it != NULL; it = it->next)
	{
		if(str_eq(it->name, fileName) == false || it->type != type) {
			continue;
		}
		return it;
	}
	if(flags & FILE_NEW) {
		switch(type)
		{
			case FILE_PROGRAM:
			case FILE_DATA:
			case FILE_TEXT:
				return file_new(fileName, type);
			default:
				return NULL;
		}
	} else {
		return NULL;
	}
}

int file_type(file_t * file)
{
	if(file == NULL)
		return FILE_INVALID;
	else
		return file->type;
}

int file_size(file_t * file)
{
	if(file == NULL)
		return 0;
	else
		return file->size;
}

void * file_data(file_t * file)
{
	if(file == NULL)
		return NULL;
	else
		return file->data;
}

void file_resize(file_t * file, int size)
{
	if(file == NULL)
		return;
	void * new = realloc(file->data, size);
	
	if(new == NULL)
		return;
	file->data = new;
	file->size = size;
}





void file_clearfs()
{
	while(first != NULL)
	{
		file_t *k = first;
		first = first->next;
		
		if(k->data)
			free(k->data);
		free(k);
	}
}

typedef struct storeio
{
	storage_t const * storage;
	uint8_t * buffer;
	int cursor;
	uint32_t lba;
} storeio_t;

static storeio_t sio_init(storage_t const * storage, void * buffer)
{
	storeio_t sr;
	sr.storage = storage;
	sr.buffer= buffer;
	sr.lba = sr.storage->partition.lba;
	sr.cursor = 0;
	// When initializing, read the whole block in, so sio_flush won't override shit.
	hal_read_block(sr.storage->device, sr.lba, sr.buffer);
	return sr;
}

static void sio_read(storeio_t * sr, void * buffer, int len)
{
	uint8_t * buf = buffer;
	while(len > 0)
	{
		if(sr->cursor >= hal_blocksize(sr->storage->device))
		{
			hal_read_block(sr->storage->device, sr->lba, sr->buffer);
			
			sr->cursor = 0;
			sr->lba++;
			if(sr->lba > (sr->storage->partition.lba + sr->storage->partition.size)) {
				// TODO: Do some error handling here!
				return;
			}
		}
		*buf++ = sr->buffer[sr->cursor++];
		len--;
	}
}

static void sio_write(storeio_t * sr, void * buffer, int len)
{
	if(sr->lba > (sr->storage->partition.lba + sr->storage->partition.size)) {
		// TODO: Do some error handling here!
		return;
	}
	
	uint8_t * buf = buffer;
	while(len > 0)
	{
		sr->buffer[sr->cursor++] = *buf++;
		len--;
		
		if(sr->cursor >= hal_blocksize(sr->storage->device))
		{
			hal_write_block(sr->storage->device, sr->lba, sr->buffer);
			
			sr->cursor = 0;
			sr->lba++;
			if(sr->lba > (sr->storage->partition.lba + sr->storage->partition.size)) {
				// TODO: Do some error handling here!
				return;
			}
		}
	}
}

static void sio_flush(storeio_t * sr)
{
	hal_write_block(sr->storage->device, sr->lba, sr->buffer);
}

/**
 * Appends the given file system from a storage device.
 */
void file_loadfs(storage_t const * storage)
{
	if(storage == NULL)
		return;
	int blocksize = hal_blocksize(storage->device);
	
	char buffer[blocksize];
	
	storeio_t read = sio_init(storage, buffer);
	
	uint32_t magic;
	sio_read(&read, &magic, sizeof(magic));
	if(magic != 0xD05E4ABC) {
		debug("Invalid rootfs magic: %x\n", magic);
		basic_error(ERR_INVALID_STORAGE);
	}
	
	uint32_t size = 0;
	do
	{
		sio_read(&read, &size, sizeof(size));
		if(size > 0)
		{
			char name[16];
			sio_read(&read, name, 0x10);
			
			file_t *f = file_get(name, FILE_NEW);
			file_resize(f, size);
			sio_read(&read, file_data(f), size);
		}
	} while(size != 0);
}

/**
 * Stores the current inram FS to a storage device.
 */
void file_savefs(storage_t const * storage)
{
	if(storage == NULL)
		return;
	int blocksize = hal_blocksize(storage->device);
	
	char buffer[blocksize];
	
	storeio_t write = sio_init(storage, buffer);
	
	uint32_t magic;
	sio_read(&write, &magic, sizeof(magic));
	if(magic != 0xD05E4ABC) {
		debug("Invalid rootfs magic: %x\n", magic);
		basic_error(ERR_INVALID_STORAGE);
	}
	
	uint32_t size = 0;
	for(file_t *it = first; it != NULL; it = it->next)
	{
		if(it->size == 0) 
			continue;
		char name[16];
		str_copy(name, it->name);
		size = it->size;
		
		sio_write(&write, &size, sizeof(size));
		sio_write(&write, name, 0x10);
		sio_write(&write, file_data(it), size);
			
	} 
	
	// Write end marker
	size = 0;
	sio_write(&write, &size, sizeof(size));
	sio_flush(&write);
}

void file_initfs(storage_t const * storage)
{

	if(storage == NULL)
		return;
	int blocksize = hal_blocksize(storage->device);
	
	char buffer[blocksize];
	
	storeio_t write = sio_init(storage, buffer);
	
	uint32_t magic = 0xD05E4ABC;
	uint32_t size = 0;
	sio_write(&write, &magic, sizeof(magic));
	sio_write(&write, &size, sizeof(size));
	sio_flush(&write);
}