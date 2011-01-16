#include "assert.h"
#include "printf.h"

void __assert_func(const char* filename, int lineno,
                   const char* funcname, const char* msg)
{
    printf("ASSERT FAILED! In %s (%s:%d): %s\n", funcname, filename, lineno, msg);
}
