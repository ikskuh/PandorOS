#pragma once

#include <stdint.h>

#define STORAGE_BOOTROM -2
#define STORAGE_INRAM   -1

typedef struct chs
{
	unsigned head      :  8;
	unsigned sector    :  6;
	unsigned cylinder  : 10;
} __attribute__((packed)) chs_t;

typedef struct partition
{
	uint8_t flags;
	chs_t start;
	uint8_t id;
	chs_t end;
	uint32_t lba;
	uint32_t size;
} __attribute__((packed)) partition_t;

typedef struct mbr
{
	uint8_t bootloader[436];
	uint8_t uniqueID[10];
	partition_t partitions[4];
	uint16_t signature;
} __attribute__((packed)) mbr_t;

typedef struct storage
{
	char name[64];
	int device;
	partition_t partition;
	struct storage * next;
} storage_t;

extern storage_t const * storage_inram;
extern storage_t const * storage_bootrom;

void storage_init();

storage_t const * storage_first();

storage_t const * storage_get(char const * name);