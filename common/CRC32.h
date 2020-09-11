#include "stdafx.h"
#include <windows.h>
#include <stdint.h>
#ifdef _WIN32
// include windows first.
// the type used by this header for windows.
typedef unsigned long long u_int64_t;
typedef long long int64_t;
typedef unsigned int u_int32_t;
typedef int int32_t;
typedef unsigned char u_int8_t;
//typedef char int8_t;
typedef unsigned short u_int16_t;
typedef short int16_t;
typedef int64_t ssize_t;
#endif
u_int32_t crc32(const void* buf, int size);
