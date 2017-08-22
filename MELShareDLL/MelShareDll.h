#pragma once
#include "MelShare.h"

#ifdef MELSHARE_EXPORTS
#	define MELSHARE_API __declspec(dllexport)
#else
#	define MELSHARE_API __declspec(dllimport)
#endif

//-----------------------------------------------------------------------------
// ERROR CODES
//-----------------------------------------------------------------------------
// >0 = successful read/write, return value is current map size
//  0   map size is zero
// -1 = failed to open shared memory map
// -2 = failed to open mutex
// -3 = wait on mutex abandoned
// -4 = wait on mutex timed out
// -5 = wait on mutex failed
// -6 = failed to release mutex
// -7 = failed to close mutex handle
//-----------------------------------------------------------------------------

extern "C" {
    MELSHARE_API int get_map_size(char* name);
}

extern "C" {
    MELSHARE_API int read_char_map(char* name, char* buffer, int buffer_size);
}

extern "C" {
    MELSHARE_API int read_int_map(char* name, int* buffer, int buffer_size);
}

extern "C" {
    MELSHARE_API int read_float_map(char* name, float* buffer, int buffer_size);
}

extern "C" {
    MELSHARE_API int read_double_map(char* name, double* buffer, int buffer_size);
}

extern "C" {
    MELSHARE_API int write_char_map(char* name, char* buffer, int buffer_size);
}

extern "C" {
    MELSHARE_API int write_int_map(char* name, int* buffer, int buffer_size);
}

extern "C" {
    MELSHARE_API int write_float_map(char* name, float* buffer, int buffer_size);
}

extern "C" {
    MELSHARE_API int write_double_map(char* name, double* buffer, int buffer_size);
}