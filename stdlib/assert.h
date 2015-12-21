#ifndef _ASSERT_H
#define _ASSERT_H

#include "unistd.h"

#define assert(x) if(!(x)) { printf("Assertion failed at file " __FILE__ " at line %d: " #x "\n", __LINE__); _exit(EXIT_FAILURE); }

#endif // _ASSERT_H
