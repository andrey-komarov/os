#include "fat16.h"
#include "ata/ata.h"
#include "kernel/kernel.h"
#include "libc/assert.h"

static fat16_boot_t boot;
static uint16_t fat;
static uint16_t data;
static uint16_t root_dir_sectors;
static uint16_t root;

void fat16_list_all_files()
{
  printk("Not implemented yet");

}

void fat16_init()
{
  printk("Starting FAT16 init");
  ata_read(0, 1, (uint16_t*)&boot);
  assert(boot.magic == 0xaa55);
  assert(boot.bytes_per_sector == 512);
  printk("%d sectors", boot.sectors_per_fat);
  printk("%d reserved_sectors, %d root dir sectors", boot.reserved_sectors, boot.root_directory_entries);
  root_dir_sectors = (boot.root_directory_entries * 32 + boot.bytes_per_sector - 1) / boot.bytes_per_sector;
  fat = boot.reserved_sectors;
  data = fat + boot.fats * boot.sectors_per_fat + root_dir_sectors;
  root = data - root_dir_sectors;
  printk("fat = %d, root = %d, data = %d", fat, root, data);
}
