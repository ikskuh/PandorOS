#include "file.h"
#include "malloc.h"
#include "string.h"
#include "hal.h"
#include "memory.h"
#include "debug.h"
#include "interpreter.h"

#include <stdint.h>

#define filename_eq(a,b) str_eqi((a), (b))

struct file
{
	char name[128];
	int type;
	
	void * data;
	int size;
	
	storage_t const * origin;
	
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
					f->origin = storage_bootrom;
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
	mem_set(file->data, 0, 1);
	file->size = 0;
	file->next = NULL;
	file->origin = storage_inram;
	
	if(first == NULL) {
		first = file;
	} else {
		file_t *it = first;
		while(it->next) it = it->next;
		it->next = file;
	}
	
	return file;
}

int _file_type_by_extension(char const * name)
{
	while(*name)
	{
		if(filename_eq(name, ".PRG"))
			return FILE_PROGRAM;
		if(filename_eq(name, ".DAT"))
			return FILE_DATA;
		if(filename_eq(name, ".TXT"))
			return FILE_TEXT;
		if(filename_eq(name, ".BAS"))
			return FILE_BASIC;
		if(*name == '.') {
			return FILE_UNKNOWN;
		}
		name++;
	}
	return FILE_INVALID;
}

int file_type_by_extension(char const * name)
{
	int type = _file_type_by_extension(name);
	debug("'%s' has type %d.\n", name, type);
	return type;
}

file_t * file_get(char const * fileName, int flags)
{
	int type = file_type_by_extension(fileName);
	if(type == FILE_INVALID)
		return NULL;

	for(file_t *it = first; it != NULL; it = it->next)
	{
		if(filename_eq(it->name, fileName) == false || it->type != type) {
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
			case FILE_BASIC:
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
	char * new = realloc(file->data, size + 1);
	
	if(new == NULL)
		return;
	
	new [size] = 0;
	file->data = new;
	file->size = size;
}

void file_rename(file_t * file, char const * newName)
{
	if(file == NULL)
		return;
	if(file_type_by_extension(newName) == FILE_INVALID)
		basic_error(ERR_INVALID_ARG);
	str_copy(file->name, newName);
	file->type = file_type_by_extension(file->name);
}

/**
 * Deletes the given file.
 */
void file_delete(file_t * file)
{
	if(file == NULL)
		return;
	
	if(file == first) {
		first = file->next;
	} else {
		for(file_t *it = first; it != NULL; it = it->next)
		{
			if(it->next != file)
				continue;
			// surgically remove the file from the file list.
			it->next = it->next->next;
			// after surgery, leave the operating room
			break;
		}
	}
	
	free(file->data);
	free(file);
}

storage_t const * file_storage(file_t *file)
{
	if(file == NULL)
		return NULL;
	else
		return file->origin;
}

void file_move(file_t * file, storage_t const * storage)
{
	if(file == NULL)
		return;
	file->origin = storage;
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
	int blocksize = hal_blocksize(sr->storage->device);
	while(len > 0)
	{
		if(sr->cursor >= blocksize)
		{
			sr->cursor = 0;
			sr->lba++;
			
			hal_read_block(sr->storage->device, sr->lba, sr->buffer);
			
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
	int blocksize = hal_blocksize(sr->storage->device);
	while(len > 0)
	{
		sr->buffer[sr->cursor++] = *buf++;
		len--;
		
		if(sr->cursor >= blocksize)
		{
			hal_write_block(sr->storage->device, sr->lba, sr->buffer);
			
			mem_set(sr->buffer, 0xFF, blocksize);
			
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
		if((size > 0) && (size != 0xFFFFFFFF))
		{
			char name[16];
			sio_read(&read, name, 0x10);
			
			file_t *f = file_get(name, FILE_NEW);
			f->origin = storage;
			file_resize(f, size);
			sio_read(&read, file_data(f), size);
		}
	} while((size != 0) && (size != 0xFFFFFFFF));
}

/**
 * Stores the current inram FS to a storage device.
 */
void file_savefs(storage_t const * storage, bool filter)
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
		if(filter && it->origin != storage)
			continue;
		char name[16];
		str_copy(name, it->name);
		size = it->size;
		sio_write(&write, &size, sizeof(size));
		sio_write(&write, name, 0x10);
		sio_write(&write, file_data(it), size);
	} 
	
	// Write end marker
	size = 0xFFFFFFFF;
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