#include "fat16.h"
#include "ata/ata.h"
#include "kernel/kernel.h"
#include "libc/assert.h"
#include "libc/string.h"

static fat16_boot_t boot;
static uint32_t fat;
static uint32_t data;
static uint32_t root_dir_sectors;
static uint32_t root;

static uint8_t* fat16_decode_name(uint8_t *src, char *dst)
{
  memcpy(dst, src, 11);
  int i = 7;
  while (i > 0 && dst[i - 1] == ' ')
    i--;
  dst[i++] = '.';
  memcpy(dst + i, src + 8, 3);
  i += 3;
  while (dst[i - 1] == ' ')
    i--;
  if (dst[i - 1] == '.')
    i--;
  dst[i] = 0x00;
  return dst + i;
}

#define FILES_PER_SECTOR (2 * ATA_SECTOR_SIZE / sizeof(fat16_dir_entry_t))

void fat16_list_all_files()
{
  for (int i = 0; i < root_dir_sectors; i++)
    {
      fat16_dir_entry_t dir[FILES_PER_SECTOR];
      printk("Reading sector %d", root + i);
      ata_read_one(root + i, (uint16_t*)dir);
      for (int j = 0; j < FILES_PER_SECTOR; j++)
        {
          if (dir[j].name[0] == 0)
            {
              printk("No more files in root dir");
              return;
            }
          if (dir[j].name[0] == 0xe5)
            {
              printk("%dth file was deleted", j);
              continue;
            }
          char name[13];
          fat16_decode_name(dir[j].name, name);
          printk("File! name: <%s>", name);
        }
    }
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
