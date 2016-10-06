#include "blockdev.h"
#include "debug.h"
#include <stdint.h>

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

void blockdev_init()
{
	int cnt = hal_devcount();
	for(int i = 0; i < cnt; i++)
	{
		debug("Blockdev[%d] = %s\n", i, hal_devname(i));
		if(hal_blocksize(i) != 512) {
			debug("Unsupported block size: %d\n", hal_blocksize(i));
		}
		
		mbr_t mbr;
		hal_read_block(i, 0, &mbr);
		
		debug("Signature: %X\n", mbr.signature);
		for(int j = 0; j < 4; j++)
		{
			debug("Partition %d:\n", j + 1);
			debug("\tFlags = %x\n", mbr.partitions[j].flags);
			debug("\tID    = %x\n", mbr.partitions[j].id);
			debug("\tLBA   = %x\n", mbr.partitions[j].lba);
			debug("\tSize  = %x\n", mbr.partitions[j].size);
			debug("\tStart = %dC %dH %dS\n", 
				mbr.partitions[j].start.cylinder,
				mbr.partitions[j].start.head,
				mbr.partitions[j].start.sector);
			debug("\tEnd = %dC %dH %dS\n", 
				mbr.partitions[j].end.cylinder,
				mbr.partitions[j].end.head,
				mbr.partitions[j].end.sector);
		}
		
		debug("Boot loader:\n");
		for(int j = 0; j < 436; j++)
		{
			if(j > 0 && (j % 16) == 0) debug("\n");
			if((j % 16) == 0) debug("\t");
			debug("%X ",mbr.bootloader[j]);
		}
		debug("\n");
	}
}
