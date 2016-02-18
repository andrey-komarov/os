#include "tty/tty.h"
#include "libc/string.h"

static uint16_t* const VIDEO = (uint16_t*)(0xB8000 + 0xC0000000);
static const size_t ROWS = 25;
static const size_t COLUMNS = 80;

static size_t row, column;
static uint8_t color;
static uint8_t default_color;
static uint16_t empty;

void init_tty()
{
  default_color = tty_make_color(COLOR_LIGHT_GREY, COLOR_BLACK);
  color = default_color;
  empty = tty_make_entry(' ', color);
  tty_cls();
}

uint8_t tty_make_color(enum vga_color fg, enum vga_color bg)
{
  return fg | (bg << 4);
}

uint16_t tty_make_entry(char ch, uint8_t color)
{
  uint16_t ch16 = ch;
  uint16_t color16 = color;
  return ch16 | (color16 << 8);
}

void tty_cls()
{
  row = column = 0;
  size_t i;
  color = tty_make_color(COLOR_LIGHT_GREY, COLOR_BLACK);
  uint16_t empty = tty_make_entry(' ', color);
  for (i = 0; i < ROWS * COLUMNS; i++)
    VIDEO[i] = empty;
}

void tty_put(char ch, uint8_t color, size_t r, size_t c)
{
  size_t index = COLUMNS * r + c;
  VIDEO[index] = tty_make_entry(ch, color);
}

void tty_putchar(char ch)
{
  if (ch == '\n')
    {
      column = 0;
      if (++row == ROWS)
        row = 0;
      for (size_t i = 0; i < COLUMNS; i++)
        tty_put(' ', default_color, row, i);
      return;
    }
  tty_put(ch, color, row, column);
  if (++column == COLUMNS)
    {
      if (++row == ROWS)
        row = 0;
    }
}

void tty_putstr(char* s)
{
  size_t len = strlen(s);
  size_t i;
  for (i = 0; i < len; i++)
    tty_putchar(s[i]);
}
