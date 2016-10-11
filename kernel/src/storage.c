#include "storage.h"
#include "standard.h"
#include "debug.h"
#include <stdint.h>

static storage_t *storages = NULL;

storage_t const * storage_inram;
storage_t const * storage_bootrom;

void storage_init()
{
	{ // Setup inram storage
		storage_t * store = malloc(sizeof(storage_t));
		str_copy(store->name, "RAM");
		store->device = STORAGE_INRAM;
		store->next = storages;
		storages = store;
		storage_inram = store;
	}
	{ // Setup bootrom storage
		storage_t *store = malloc(sizeof(storage_t));
		str_copy(store->name, "ROM");
		store->device = STORAGE_BOOTROM;
		store->next = storages;
		storages = store;
		storage_bootrom = store;
	}

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
		
		uint32_t * blockMagic = (void*)&mbr;
		if(*blockMagic == 0xD05E4ABC)
		{
			debug("Device is non-partitioned data.\n");
			storage_t *store = malloc(sizeof(storage_t));
				
			// Setup
			str_copy(store->name, hal_devname(i));
			store->device = i;
			store->partition = (partition_t) {
				0x00,
				{ 1, 1, 1 }, // CHS Start
				0x7F,
				{ 10, 10, 10 }, // CHS End?
				1, // LBA
				2880, // TODO: Set correct device size. Currently, its a 3.5" Floppy with 1.44 MB
			};
			
			// Enlist
			store->next = storages;
			storages = store;
			
			continue;
		}
		
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

storage_t const * storage_get(char const * name)
{
	for(storage_t const * it = storage_first(); it != NULL; it = it->next)
	{
		if(str_eq(it->name, name)) {
			return it;
		}
	}
	return NULL;
}

storage_t const * storage_first()
{
	return storages;
}
