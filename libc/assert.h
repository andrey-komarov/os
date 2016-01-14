#ifndef _ASSERT_H
#define _ASSERT_H

#include "panic.h"

#define assert(x) if(!(x)) { panic("Assert:" #x); }

#endif // _ASSERT_H
