#ifndef _ATA_H
#define _ATA_H

#include <stdint.h>

// Device Control Registers
#define ATA_DCR 0x3f6
// Alternate Status Ports
#define ATA_ASP 0x376

// Primary
#define ATA_BASE1 0x1f0
// Secondary
#define ATA_BASE2 0x170

// Registers
#define ATA_RG_DATA 0
#define ATA_RG_SEC_COUNT 2
#define ATA_RG_LBALO 3
#define ATA_RG_LBAMID 4
#define ATA_RG_LBAHI 5
#define ATA_RG_DRIVE 6
#define ATA_RG_CMD 7
#define ATA_RG_STATUS 7
#define ATA_RG_CONTROL 0x206

// Status byte
#define ATA_ST_ERR (1 << 0)
#define ATA_ST_DRQ (1 << 3)
#define ATA_ST_SRV (1 << 4)
#define ATA_ST_DF (1 << 5)
#define ATA_ST_RDY (1 << 6)
#define ATA_ST_BSY (1 << 7)

// Commands
#define ATA_CMD_IDENTIFY 0xec
// Flush caches
#define ATA_CMD_FLUSH 0xe7
#define ATA_CMD_READ 0x20

// Floating Bus, no drive connected
#define ATA_ST_FLT 0xff

typedef struct ata_identify_t
{
  uint16_t values[256];
} ata_identify_t;

void init_ata();
void ata_identify();
void ata_read(int lba28, uint16_t *buf);
#endif // _ATA_H
