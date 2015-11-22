#include "stdio.h"

#include "string.h"

#include <stdarg.h>

static const char* digits = "0123456789abcdef";

static char* uitoa(char* str, unsigned x, int radix)
{
  if (x == 0)
    {
      *str = '0';
      return str + 1;
    }
  size_t len = 0;
  while (x)
    {
      str[len++] = digits[x % radix];
      x /= radix;
    }
  size_t i = 0;
  for (i = 0; i < len / 2; i++)
    {
      char tmp = str[i];
      str[i] = str[len - i - 1];
      str[len - i - 1] = tmp;
    }
  return str + len;
}

static char* itoa(char* str, int x, int radix)
{
  if (x < 0)
    {
      *str = '-';
      str++;
      x = -x;
    }
  return uitoa(str, x, radix);
}

int sprintf(char *str, const char *format, ...)
{
  char ch;
  va_list args;
  va_start(args, format);
  char* start_str = str;
  while ((ch = *format) != 0)
    {
      if (ch != '%')
	{
	  *str = ch;
	  str++;
	  format++;
	}
      else
	{
	  char modifier = format[1];
	  if (modifier == 'd')
	    {
	      int arg_d = va_arg(args, int);
	      str = itoa(str, arg_d, 10);
	      format += 2;
	    }
	  else if (modifier == 's')
	    {
	      char* arg_s = va_arg(args, char*);
	      size_t len = strlen(arg_s);
	      memcpy(str, arg_s, strlen(arg_s));
	      str += len;
	      format += 2;
	    }
	  else if (modifier == 'p')
	    {
	      void* arg_p = va_arg(args, void*);
	      str[0] = '0';
	      str[1] = 'x';
	      str = uitoa(str + 2, (unsigned)arg_p, 16);
	      format += 2;
	    }
	  else
	    {
	      *str = '%';
	      str++;
	      format++;
	    }
	}
    }
  *str = 0;
  return start_str - str;
}

