#include "io.h"
#include "hal.h"
#include "string.h"
#include "interrupts.h"
#include <stdint.h>

#define ERR (1<<0)
#define DRQ (1<<3)
#define SRV (1<<4)
#define DF (1<<5)
#define RDY (1<<6)
#define BSY (1<<7)

static inline void wait400NS(uint16_t p) 
{
	inb(p);
	inb(p);
	inb(p);
	inb(p);
}

struct cpu *ata_isr(struct cpu *cpu)
{
	// hal_debug("ATA isr!\n");
	return cpu;
}

static int devcnt = 0;

typedef struct ataports
{
	uint16_t data;
	uint16_t error;
	uint16_t sectors;
	uint16_t lbaLow;
	uint16_t lbaMid;
	uint16_t lbaHigh;
	uint16_t devSelect;
	uint16_t status;
	uint16_t cmd;
	uint16_t control;
} ataports_t;

struct dev {
	char name[8];
	int blocksize;
	int sectorcount;
	bool master;
	int baseport;
	struct ataports ports;
	bool present;
};

static struct dev devs[8];

static uint8_t status(struct dev *dev)
{
	wait400NS(dev->ports.status);
	return inb(dev->ports.status);
}

static bool isfloating(struct dev *dev)
{
	return (status(dev) == 0xFF);
}

static bool initialize_dev(struct dev *dev)
{
#define ports (dev->ports)

	if(isfloating(dev)) {
		return false;
	}
	// To use the IDENTIFY command, select a target drive by sending
	// 0xA0 for the master drive, or
	// 0xB0 for the slave, to the "drive select" IO port.
	if(dev->master)
		outb(ports.devSelect, 0xA0); // Select Master
	else
		outb(ports.devSelect, 0xB0); // Select Master
	
	// Then set the Sectorcount, LBAlo, LBAmid, and LBAhi IO ports to 0
	outb(ports.sectors, 0);
	outb(ports.lbaLow, 0);
	outb(ports.lbaMid, 0);
	outb(ports.lbaHigh, 0);
	
	// Then send the IDENTIFY command (0xEC) to the Command IO port.
	outb(ports.cmd, 0xEC);
	
	// Then read the Status port again. If the value read is 0, the drive does not 
	// exist.
	if(status(dev) == 0x00) {
		hal_debug("IDENTIFY failed with STATUS = 0.\n");
		return false;
	}
	
	// For any other value: poll the Status port (0x1F7) until bit 7 (BSY, value = 0x80)
	// clears. Because of some ATAPI drives that do not follow spec, at this point you
	// need to check the LBAmid and LBAhi ports (0x1F4 and 0x1F5) to see if they are
	// non-zero. If so, the drive is not ATA, and you should stop polling. Otherwise,
	// continue polling one of the Status ports until bit 3 (DRQ, value = 8) sets,
	// or until bit 0 (ERR, value = 1) sets.
	while(status(dev) & 0x80) {
		hal_debug("devbusy\n");
	}
	
	if((inb(ports.lbaMid) != 0) || (inb(ports.lbaHigh) != 0)) {
		hal_debug("%d, %d\n", (uint32_t)inb(ports.lbaMid), (uint32_t)inb(ports.lbaHigh));
		hal_debug("IDENTIFY failed with INVALID ATA DEVICE.\n");
		return false;
	}
	
	uint8_t stat;
	while(((stat = status(dev)) & 0x9) == 0) {
		hal_debug("devbusy2\n");
	}
	if(stat & 0x01) {
		hal_debug("IDENTIFY failed with ATA ERR.\n");
		return false;
	}

	// At that point, if ERR is clear, the data is ready to read from the Data port 
	// (0x1F0).
	// Read 256 16-bit values, and store them.
	
	uint16_t ataData[256];
	for(uint32_t i = 0; i < 256; i++) {
		ataData[i] = inw(ports.data);
	}
	
	dev->sectorcount = ((uint32_t)ataData[61] << 16 | ataData[60]);
	
#undef ports
	return true;
}

void x86_init_ata()
{
	int baseports[] = {
		0x1F0,
		0x170,
		0x1E8,
		0x168,
	};
	for(int i = 0; i < 8; i++)
	{
		static char name[8];
		str_copy(name, "ATA");
		name[3] = '0' + i;
		name[4] = 0;
		str_copy(devs[i].name, name);
		
		devs[i].blocksize = 512;
		devs[i].baseport = baseports[i / 2];
		devs[i].master = (i % 2) == 0;
		devs[i].ports = (ataports_t) {
			/*data     =*/(uint16_t)(devs[i].baseport + 0), 
			/*error    =*/(uint16_t)(devs[i].baseport + 1),
			/*sectors  =*/(uint16_t)(devs[i].baseport + 2),
			/*lbaLow   =*/(uint16_t)(devs[i].baseport + 3),
			/*lbaMid   =*/(uint16_t)(devs[i].baseport + 4),
			/*lbaHigh  =*/(uint16_t)(devs[i].baseport + 5),
			/*devSelect=*/(uint16_t)(devs[i].baseport + 6),
			/*status   =*/(uint16_t)(devs[i].baseport + 7),
			/*cmd      =*/(uint16_t)(devs[i].baseport + 7),
			/*control  =*/(uint16_t)(devs[i].baseport + 518)
		};
		
		devs[i].present = initialize_dev(&devs[i]);
	}
	
	int i;
	for(i = 0, devcnt = 0; i < 4; i++)
	{
		if(devs[i].present) {
			if(devcnt != i) {
				devs[devcnt] = devs[i];
			}
			devcnt++;
		}
	}
}

int hal_devcount()
{
	return devcnt;
}

char const * hal_devname(int i)
{
	return devs[i].name;
}

int hal_blocksize(int i)
{
	return devs[i].blocksize; 
}
 
bool hal_read_block(int devid, int lba, void * buffer)
{
	int num = 1; // TODO: Maybe replace with parameter some time
	struct dev * dev = &devs[devid];
	if(dev->present == false) {
		hal_debug("Invalid read from blockid %d: Device not preset!\n", devid);
		return false;
	}
	
#define ports (dev->ports)
	
	if(dev->sectorcount <= 0) {
		return false;
	}
	if(num <= 0) {
		return false;
	}
	uint16_t * ptr = (uint16_t*)buffer;
	
	if(dev->master)
		outb(ports.devSelect, 0xE0);
	else
		outb(ports.devSelect, 0xF0);
	outb(ports.sectors, num);
	outb(ports.lbaLow, lba);
	outb(ports.lbaMid, lba >> 8);
	outb(ports.lbaHigh, lba >> 16);
	outb(ports.cmd, 0x20);
	
	uint32_t off = 0;
	while(num-- > 0) {
		uint8_t stat;
		while(((stat = status(dev)) & 0x9) == 0);
		if(stat & 0x01) {
			hal_debug("Failed to read from %s.\n", dev->name);
			return false;
		}
		
		for(uint32_t i = 0; i < 256; i++) {
			ptr[off++] = inw(ports.data);
		}
	}

#undef ports
	return true;
}

bool hal_write_block(int devid, int lba, void const * buffer)
{
	int num = 1; // TODO: Maybe replace with parameter some time
	struct dev * dev = &devs[devid];
	if(dev->present == false) {
		hal_debug("Invalid read from blockid %d: Device not preset!\n", devid);
		return false;
	}
	
#define ports (dev->ports)
	
	if(dev->sectorcount <= 0) {
		return false;
	}
	if(num <= 0) {
		return false;
	}
	uint16_t * ptr = (uint16_t*)buffer;
	
	if(dev->master)
		outb(ports.devSelect, 0xE0);
	else
		outb(ports.devSelect, 0xF0);
	outb(ports.sectors, num);
	outb(ports.lbaLow, lba);
	outb(ports.lbaMid, lba >> 8);
	outb(ports.lbaHigh, lba >> 16);
	outb(ports.cmd, 0x30);
	
	hal_debug("HAL|Write %d\n", lba);
	
	uint32_t off = 0;
	while(num-- > 0) {
		uint8_t stat;
		while(((stat = status(dev)) & (ERR | DRQ)) == 0);
		if(stat & 0x01) {
			hal_debug("Failed to write to %s.\n", dev->name);
			return false;
		}
		
		for(uint32_t i = 0; i < 256; i++) {
			outw(ports.data, ptr[off++]);
			asm volatile ("nop");
			asm volatile ("nop");
			asm volatile ("nop");
			asm volatile ("nop");
		}
		
		outb(ports.cmd, 0xE7); // Flush
		while(((stat = status(dev)) & (ERR | RDY)) == 0);
		if(stat & 0x01) {
			hal_debug("Failed to read from %s.\n", dev->name);
			return false;
		}
	}

#undef ports
	return true;
}