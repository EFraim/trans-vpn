#ifndef _DEBUG_H_
#define _DEBUG_H_

#include "printf.h"

#define DBG(fmt, args...) printf(fmt "%s", ##args, "\n")

#endif
