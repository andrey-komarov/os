#ifndef _FAT16_H
#define _FAT16_H

#include <stdint.h>

typedef struct fat16_boot
{
  uint8_t jmp[3];
  uint8_t oem_id[8];
  uint16_t bytes_per_sector;
  uint8_t sectors_per_cluster;
  uint16_t reserved_sectors;
  uint8_t fats;
  uint16_t root_directory_entries;
  uint16_t total_sectors;
  uint8_t media_descriptor_type;
  uint16_t sectors_per_fat;
  uint16_t sectors_per_track;
  uint16_t heads;
  uint32_t hidden_sectors;
  uint32_t total_sectors_32;
  uint8_t drive_number;
  uint8_t win_nt_flags;
  uint8_t signature;
  uint32_t serial;
  uint8_t label[11];
  uint8_t id[8];
  uint8_t code[448];
  uint16_t magic;
} __attribute__((packed)) fat16_boot_t;

#define FAT_ATTR_RO 0x01
#define FAT_ATTR_HIDDEN 0x02
#define FAT_ATTR_SYSTEM 0x04
#define FAT_ATTR_VOL_ID 0x08
#define FAT_ATTR_DIR 0x10
#define FAT_ATTR_ARCHIVE 0x20
#define FAT_ATTR_LFN (FAT_ATTR_RO | FAT_ATTR_HIDDEN | FAT_ATTR_SYSTEM | FAT_ATTR_VOL_ID)

typedef struct fat16_dir_entry
{
  uint8_t name[11];
  uint8_t attributes;
  uint8_t reserved;
  uint8_t created_seconds;
  uint16_t created_time;
  uint16_t created_date;
  uint16_t accessed_date;
  uint16_t first_cluster_high;
  uint16_t modification_time;
  uint16_t modification_date;
  uint16_t first_cluster_low;
  uint32_t file_size;
} __attribute__((packed)) fat16_dir_entry_t;

typedef struct fat16_fd
{
  uint16_t cluster;
  uint32_t file_size;
} fat16_fd_t;

void fat16_init();

fat16_fd_t *fat16_open(char *path, fat16_fd_t *fd);

void fat16_list_all_files();

#endif // _FAT16_H
