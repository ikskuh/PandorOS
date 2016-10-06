#include "storage.h"
#include "standard.h"
#include "debug.h"
#include <stdint.h>

static storage_t *storages = NULL;

void storage_init()
{
	int cnt = hal_devcount();
	for(int i = 0; i < cnt; i++)
	{
		debug("Blockdev[%d] = %s\n", i, hal_devname(i));
		if(hal_blocksize(i) != 512) {
			debug("Unsupported block size: %d\n", hal_blocksize(i));
			continue;
		}
		
		mbr_t mbr;
		hal_read_block(i, 0, &mbr);
		
		if(mbr.signature != 0xAA55) {
			debug("Invalid MBR signature: %X\n", mbr.signature);
			continue;
		}
		
		// debug("Signature: %X\n", mbr.signature);
		for(int j = 0; j < 4; j++)
		{
			// 0x7F Partition Type
			if(mbr.partitions[j].id == 0x7F) {
				storage_t *store = malloc(sizeof(storage_t));
				
				// Setup
				str_printf(store->name, "%s.%d", hal_devname(i), j);
				store->device = i;
				store->partition = mbr.partitions[j];
				
				// Enlist
				store->next = storages;
				storages = store;
			}
		
			//debug("Partition %d:\n", j + 1);
			//debug("\tFlags = %x\n", mbr.partitions[j].flags);
			//debug("\tID    = %x\n", mbr.partitions[j].id);
			//debug("\tLBA   = %x\n", mbr.partitions[j].lba);
			//debug("\tSize  = %x\n", mbr.partitions[j].size);
			//debug("\tStart = %dC %dH %dS\n", 
				//mbr.partitions[j].start.cylinder,
				//mbr.partitions[j].start.head,
				//mbr.partitions[j].start.sector);
			//debug("\tEnd = %dC %dH %dS\n", 
				//mbr.partitions[j].end.cylinder,
				//mbr.partitions[j].end.head,
				//mbr.partitions[j].end.sector);
		}
		
		//debug("Boot loader:\n");
		//for(int j = 0; j < 436; j++)
		//{
			//if(j > 0 && (j % 16) == 0) debug("\n");
			//if((j % 16) == 0) debug("\t");
			//debug("%X ",mbr.bootloader[j]);
		//}
		//debug("\n");
	}
	
	
	debug("Storage devices:\n");
	for(storage_t const * it = storage_first(); it != NULL; it = it->next)
	{
		debug("%s\n", it->name);
	}
}

storage_t const * storage_first()
{
	return storages;
}
