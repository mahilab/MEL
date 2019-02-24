// MIT License
//
// MEL - Mechatronics Engine & Library
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
//            Craig McDonald (craig.g.mcdonald@gmail.com)

#pragma once
#include <string>
#include <typeinfo>
#include <vector>
#include <map>


namespace mel {

//==============================================================================
// FIXED TYPES
//==============================================================================

typedef               char  int8;   ///<                       -127 to +127
typedef unsigned      char uint8;   ///<                          0 to +255
typedef              short  int16;  ///<                    -32,767 to +32,767
typedef unsigned     short uint16;  ///<                          0 to +65,535
typedef                int  int32;  ///<             -2,147,483,648 to +2,147,483,647
typedef unsigned       int uint32;  ///<                          0 to +4,294,967,295
#if defined(_MSC_VER)
typedef signed     __int64  int64;  ///< -9,223,372,036,854,775,808 to +9,223,372,036,854,775,807
typedef unsigned   __int64 uint64;  ///<                          0 to 18,446,744,073,709,551,615
#else
typedef signed   long long int64;   ///< -9,223,372,036,854,775,808 to +9,223,372,036,854,775,807
typedef unsigned long long uint64;  ///<                          0 to 18,446,744,073,709,551,615
#endif

//==============================================================================
// FILESYSTEM TYPES
//==============================================================================

/// Represents mode in which process resources can be opened or created
enum OpenMode {
    OpenOrCreate,  ///< create the object if it does not exit
    OpenOnly       ///< only attempt to open an existing object
};

/// Represents mode in which a file is written when it is opened
enum WriteMode {
    Truncate = 0, ///< file will be truncated if it exists
    Append = 1    ///< file will be not be truncated on open
};

//==============================================================================
// DAQ TYPES
//==============================================================================

/// Default type for a channel number
typedef uint32 ChanNum;

/// An array of channel numbers
typedef std::vector<ChanNum> ChanNums;

/// Maps a channel number to an array index
// typdef channel number(s)
typedef std::map<uint32, std::size_t> ChanMap;

/// Represents a voltage in [V]
typedef double Voltage;

/// Represents a digital TTL logic level
enum Logic {
    Low  = 0, ///< Low TTL level  (typically 0V)
    High = 1   ///< High TTL level (typically 5V)
};

/// Represents a digital pin direction
enum Direction {
    In,  ///< Digital input
    Out  ///< Digital output
};

/// Represents encoder quadrature factors
enum QuadFactor {
    X0   = 0,  ///< no quadrature factor
    X1   = 1,  ///< 1X counts per revolution
    X2   = 2,  ///< 2X counts per revolution
    X4   = 4,  ///< 4X counts per revolution
};

//==============================================================================
// MECHATRONIC TYPES
//==============================================================================

/// Represents an orthoganal axis
enum Axis {
    AxisX = 0,
    AxisY = 1,
    AxisZ = 2
};

//==============================================================================
// UTILITY FUNCTIONS
//==============================================================================

std::string demangle(const char* name);

template <class T>
std::string type(const T& t) {
    return demangle(typeid(t).name());
}

} // namespace mel
