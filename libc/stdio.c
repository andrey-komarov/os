#include "stdio.h"

#include "string.h"

#include <stdarg.h>

static const char* digits = "0123456789abcdef";

static char* uitoa(char* str, unsigned long long x, int radix)
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

static char* itoa(char* str, long long x, int radix)
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
  va_list args;
  va_start(args, format);
  return vsprintf(str, format, args);
}

int vsprintf(char *str, const char *format, va_list args)
{
  char ch;
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
	  if (modifier == 'l' && format[2] == 'l' && format[3] == 'x')
	    {
	      long long arg = va_arg(args, long long);
	      str = itoa(str, arg, 16);
	      format += 4;
	    }
	  else if (modifier == 'd')
	    {
	      int arg = va_arg(args, int);
	      str = itoa(str, arg, 10);
	      format += 2;
	    }
	  else if (modifier == 'u')
	    {
	      unsigned arg = va_arg(args, unsigned);
	      str = uitoa(str, arg, 10);
	      format += 2;
	    }
	  else if (modifier == 's')
	    {
	      char* arg = va_arg(args, char*);
	      size_t len = strlen(arg);
	      memcpy(str, arg, strlen(arg));
	      str += len;
	      format += 2;
	    }
	  else if (modifier == 'p')
	    {
	      void* arg = va_arg(args, void*);
	      str[0] = '0';
	      str[1] = 'x';
	      str = uitoa(str + 2, (unsigned)arg, 16);
	      format += 2;
	    }
	  else if (modifier == '%')
	    {
	      str[0] = '%';
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
  return str - start_str;
}

