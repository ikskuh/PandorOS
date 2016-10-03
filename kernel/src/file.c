#include "file.h"
#include "malloc.h"
#include "string.h"

struct file
{
	char name[128];
	int type;
	
	void * data;
	int size;
	
	struct file * next;
};

file_t * first = NULL;

void file_init()
{
	first = NULL;
}

static file_t * file_new(char const * fileName, int type)
{
	file_t *file = malloc(sizeof(file_t));
	str_copy(file->name, fileName);
	file->type = type;
	file->data = NULL;
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

static int gettype(char const * name)
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
	int type = gettype(fileName);
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
	file->data = realloc(file->data, file->size);
}