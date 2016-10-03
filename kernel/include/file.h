#pragma once

#define FILE_INVALID 0
#define FILE_PROGRAM 1
#define FILE_DATA    2
#define FILE_TEXT    3


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
 * Gets or creates a new file.
 * @param fileName The name of the file.
 * @param type     The required type of the file.
 * @param flags    The flags which affect the file opening.
 */
file_t * file_get(char const * fileName, int flags);

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