#pragma once

#ifdef MELSHARE_EXPORTS
#	define MELSHARE_API __declspec(dllexport)
#else
#	define MELSHARE_API __declspec(dllimport)
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

namespace MathLibrary
{
    // This class is exported from the MathLibrary.dll  
    class Functions
    {
    public:
        // Returns a + b  
        static MELSHARE_API double Add(double a, double b);

        // Returns a * b  
        static MELSHARE_API double Multiply(double a, double b);

        // Returns a + (a * b)  
        static MELSHARE_API double AddMultiply(double a, double b);
    };
}