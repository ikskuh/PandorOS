#pragma once

#include "storage.h"

#define FILE_INVALID 0
#define FILE_PROGRAM 1
#define FILE_DATA    2
#define FILE_TEXT    3
#define FILE_BASIC   4
#define FILE_UNKNOWN 5


#define FILE_DEFAULT    0
#define FILE_NEW        (1<<0)

/**
 * A file structure.
 */
typedef struct file file_t;

/**
 * Initializes the inram file system.
 */
void file_init();

/**
 * Deletes all files from the inram FS.
 */
void file_clearfs();

/**
 * Initializes a new file system.
 */
void file_initfs(storage_t const * storage);

/**
 * Appends the given file system from a storage device.
 */
void file_loadfs(storage_t const * storage);

/**
 * Stores the current inram FS to a storage device.
 */
void file_savefs(storage_t const * storage);

/** 
 * Gets or creates a new file.
 * @param fileName The name of the file.
 * @param type     The required type of the file.
 * @param flags    The flags which affect the file opening.
 */
file_t * file_get(char const * fileName, int flags);

/**
 * Renames the given file.
 */
void file_rename(file_t * file, char const * newName);

/**
 * Deletes the given file.
 */
void file_delete(file_t * file);

/**
 * Returns the file type determined by the given file name.
 */
int file_type_by_extension(char const * fileName);

/**
 * Returns the type of the given file.
 */
int file_type(file_t * file);

/*
 * Returns the size of the file in bytes.
 */
int file_size(file_t * file);

/**
 * Returns a pointer to the file data.
 * @remark If file_resize is called, the pointer may be invalidated.
 */
void * file_data(file_t * file);

/**
 * Resizes the file to the given size.
 * @param file The file that should be resized.
 * @param size The new size of the file.
 */
void file_resize(file_t * file, int size);


file_t * file_first();

file_t * file_next(file_t * it);

char const * file_getname(file_t *f);