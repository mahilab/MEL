#pragma once

#ifdef _WIN32
#ifdef MELSHARE_EXPORTS
#	define MELSHARE_API __declspec(dllexport)
#else
#	define MELSHARE_API __declspec(dllimport)
#endif
#elif
#	define MELSHARE_API
#endif

extern "C" {
    MELSHARE_API int read_char_map(char* name, char* buffer, int size);
}

extern "C" {
    MELSHARE_API int read_int_map(char* name, int* buffer, int size);
}

extern "C" {
    MELSHARE_API int read_double_map(char* name, double* buffer, int size);
}

extern "C" {
    MELSHARE_API int write_char_map(char* name, char* buffer, int size);
}

extern "C" {
    MELSHARE_API int write_int_map(char* name, int* buffer, int size);
}

extern "C" {
    MELSHARE_API int write_double_map(char* name, double* buffer, int size);
}