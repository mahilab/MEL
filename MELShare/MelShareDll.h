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
    MELSHARE_API int read_char_map(const char* name, char* buffer, const int size);
}

extern "C" {
    MELSHARE_API int read_int_map(const char* name, int* buffer, const int size);
}

extern "C" {
    MELSHARE_API int read_double_map(const char* name, double* buffer, const int size);
}

extern "C" {
    MELSHARE_API int write_char_map(const char* name, const char* buffer, const int size);
}

extern "C" {
    MELSHARE_API int write_int_map(const char* name, const int* buffer, const int size);
}

extern "C" {
    MELSHARE_API int write_double_map(const char* name, const double* buffer, const int size);
}