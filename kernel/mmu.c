#include "mmu.h"

/*
 * 31    24 23    16 15     8 7      0
 * AAAAAAAA AAAAAAAA AAAAXXXG S0ADWURP
 */
uint32_t bootp_page_dir[PAGE_DIR_SIZE] __attribute__((aligned(0x1000))) = {
  [0] = 0x00000083, // Present, Huge, RW
  [0xc0000000 >> 22] = 0x00000083 // Present, Huge, RW
};
