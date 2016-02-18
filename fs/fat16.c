#include <stddef.h>
#include "fat16.h"
#include "ata/ata.h"
#include "kernel/kernel.h"
#include "libc/assert.h"
#include "libc/string.h"
#include "tty/tty.h"

static fat16_boot_t boot;
static uint32_t fat;
static uint32_t data;
static uint32_t root_dir_sectors;
static uint32_t root;

#define FAT_CACHE_SIZE 100

static struct
{
  uint16_t cur;
  uint16_t next;
} cache[FAT_CACHE_SIZE];

uint16_t fat16_next(uint16_t cluster)
{
  size_t bucket = cluster % FAT_CACHE_SIZE;
  if (cache[bucket].cur != cluster)
    {
      uint32_t addr = fat * boot.bytes_per_sector + 2 * cluster;
      uint32_t lba = addr / boot.bytes_per_sector;
      uint16_t sect[ATA_SECTOR_SIZE];
      ata_read_one(lba, sect);
      cache[bucket].cur = cluster;
      cache[bucket].next = sect[addr % ATA_SECTOR_SIZE / 2];
    }
  return cache[bucket].next;
}

void fat16_cat(fat16_fd_t *fd)
{
  uint32_t cluster = fd->cluster;
  
  uint32_t size = fd->file_size;
  uint8_t sum = 0;
  while (size > 0)
    {
      for (int s = 0; s < boot.sectors_per_cluster; s++)
        {
          uint8_t sect[ATA_SECTOR_SIZE * 2];
          uint32_t lba = data + (cluster - 2) * boot.sectors_per_cluster + s;
          ata_read_one(lba, (uint16_t*)sect);
          for (int i = 0; i < ATA_SECTOR_SIZE * 2 && size > 0; i++)
            {
              sum += sect[i];
              size--;
            }
        }
      cluster = fat16_next(cluster);
    }
  printk("Cat done, checksum = %d", sum);
}

static char* fat16_decode_name(char *dst, uint8_t *src)
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
  for (uint32_t i = 0; i < root_dir_sectors; i++)
    {
      fat16_dir_entry_t dir[FILES_PER_SECTOR];
      ata_read_one(root + i, (uint16_t*)dir);
      for (uint32_t j = 0; j < FILES_PER_SECTOR; j++)
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
          if (dir[j].attributes == 0x0f)
            {
              printk("This is LFN, skipping");
              continue;
            }
          char name[13];
          fat16_decode_name(name, dir[j].name);
          printk("File! name: <%s>", name);
          if (!(dir[j].attributes & FAT_ATTR_DIR))
            {
              fat16_fd_t fd;
              fd.file_size = dir[j].file_size;
              fd.cluster = dir[j].first_cluster_low;
              fat16_cat(&fd);
            }
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
  for (int i = 0; i < FAT_CACHE_SIZE; i++)
    cache[i].cur = 0xffff;
}

static char *first_path_part(char* dest, char *path)
{
  while (*path == '/' && *path != 0)
    path++;
  if (*path == 0)
    return NULL;
  while (*path != '/' && *path != 0)
    *dest++ = *path++;
  *dest = 0;
  return path;
}

static fat16_dir_entry_t *find_file_in_root_dir(char *name, fat16_dir_entry_t *ent)
{
  for (uint32_t i = 0; i < root_dir_sectors; i++)
    {
      fat16_dir_entry_t dir[FILES_PER_SECTOR];
      ata_read_one(root + i, (uint16_t*)dir);
      for (uint32_t j = 0; j < FILES_PER_SECTOR; j++)
        {
          if (dir[j].name[0] == 0)
            return NULL;
          if (dir[j].name[0] == 0xe5)
            continue;
          if (dir[j].attributes == 0x0f) // LFN entry, skip
            continue;
          char name_here[13];
          fat16_decode_name(name_here, dir[j].name);
          if (!strcmp(name, name_here))
            {
              memcpy(ent, &dir[j], sizeof(*ent));
              return ent;
            }
        }
    }
  return NULL;
}

fat16_fd_t *fat16_open(char *path, fat16_fd_t *fd)
{
  printk("Path: <%s>", path);
  char name[13];
  char *p = first_path_part(name, path);
  fat16_dir_entry_t ent;
  if (find_file_in_root_dir(name, &ent) == NULL)
    return NULL;
  if (first_path_part(name, p) != NULL)
    panic("Nested directories are not supported yet :(");
  fd->cluster = ent.first_cluster_low;
  fd->file_size = ent.file_size;
  printk("Open success");
  return fd;
}

#define min(a, b) ((a) < (b) ? (a) : (b))

int fat16_read(fat16_fd_t *fd, uint8_t *dst, uint32_t offset, uint32_t length)
{
  uint32_t cluster = fd->cluster;
  uint32_t size = fd->file_size;

  if (offset > size || offset + length > size)
    return EFAULT;

  uint32_t length2 = length;
  while (length > 0)
    {
      for (int s = 0; s < boot.sectors_per_cluster && length > 0; s++)
        {
          if (offset >= ATA_SECTOR_SIZE * 2)
            {
              offset -= ATA_SECTOR_SIZE * 2;
            }
          else
            {
              uint32_t right = min(offset + length, ATA_SECTOR_SIZE * 2);
              uint8_t sect[ATA_SECTOR_SIZE * 2];
              uint32_t lba = data + (cluster - 2) * boot.sectors_per_cluster + s;
              ata_read_one(lba, (uint16_t*)sect);
              memcpy(dst, sect + offset, right - offset);
              dst += right - offset;
              length -= right - offset;
              offset = 0;
            }
        }
      cluster = fat16_next(cluster);
    }
  return length2;
}
