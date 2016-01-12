#include "ata.h"
#include "kernel/ioports.h"
#include "kernel/kernel.h"
#include "libc/panic.h"

void identify()
{
  printk("ATA identify");
  outb(ATA_BASE1 + ATA_RG_DRIVE, 0xa0);
  outb(ATA_BASE1 + ATA_RG_SEC_COUNT, 0);
  outb(ATA_BASE1 + ATA_RG_LBALO, 0);
  outb(ATA_BASE1 + ATA_RG_LBAMID, 0);
  outb(ATA_BASE1 + ATA_RG_LBAHI, 0);
  outb(ATA_BASE1 + ATA_RG_CMD, ATA_CMD_IDENTIFY);
  uint8_t status = inb(ATA_BASE1 + ATA_RG_STATUS);
  if (status == 0)
    panic("ATA Primary drive does not exist");
  for (int i = 0; i < 4 && (status & ATA_ST_BSY); i++)
    status = inb(ATA_BASE1 + ATA_RG_STATUS);
  printk("ATA identify status: %p", status);
  if (status & ATA_ST_ERR)
    panic("ATA Primary is in ERR state");
  if (!(status & ATA_ST_DRQ))
    panic("ATA Primary DRQ is not set");
  ata_identify_t id;
  for (int i = 0; i < 256; i++)
    id.values[i] = inw(ATA_BASE1 + ATA_RG_DATA);
  if (id.values[83] & (1 << 10))
    printk("LBA48 is supported");
  else
    printk("LBA48 is not supported");
  uint32_t sectors28 = (((uint32_t)id.values[61]) << 16) | id.values[60];
  uint64_t sectors48 = id.values[103];
  sectors48 = (sectors48 << 8) | id.values[102];
  sectors48 = (sectors48 << 8) | id.values[101];
  sectors48 = (sectors48 << 8) | id.values[100];
  printk("%u LBA28 sectors", sectors28);
  printk("%lld LBA48 sectors", sectors48);
}
