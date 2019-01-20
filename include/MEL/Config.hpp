// MIT License
//
// MEL - Mechatronics Engine and Library
// Copyright (c) 2019 Mechatronics and Haptic Interfaces Lab - Rice University
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// Author(s): Evan Pezent (epezent@rice.edu)

#ifndef MEL_CONFIG_HPP
#define MEL_CONFIG_HPP

//==============================================================================
// IDENTIFY OPERATING SYSTEM
//==============================================================================

#if (defined (_WIN32) || defined(__WIN32__))
    #define MEL_SYSTEM_WINDOWS
    #ifndef NOMINMAX
        #define NOMINMAX
    #endif
#elif defined(__APPLE__) && defined(__MACH__)
    #define MEL_SYSTEM_MACOS
#else
    #define MEL_SYSTEM_UNIX
#endif


//==============================================================================
// IMPORT/EXPORT MACROS
//==============================================================================

// Define DLL import/export macros
#if !defined(MEL_STATIC)
    #if defined(MEL_SYSTEM_WINDOWS)
        // Export dynamic link library (from DLL side)
        #ifdef MEL_EXPORTS
            #define MEL_API __declspec(dllexport)
        // Import dynamic link library (from client side)
        #else
            #define MEL_API __declspec(dllimport)
        #endif // MEL_EXPORTS
        // Disable annoying MSVC++ warning
        #ifdef _MSC_VER
            #pragma warning(disable: 4251)
        #endif // _MSC_VER
    #else // MacOS, Linux
        #if __GNUC__ >= 4
            #define MEL_API __attribute__ ((__visibility__ ("default")))
        #else
            #define MEL_API
        #endif
    #endif
#else
    #define MEL_API
#endif

// Version of the library
#define MEL_VERSION_MAJOR 1
#define MEL_VERSION_MINOR 0

#endif // MEL_CONFIG_HPP
