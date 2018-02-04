// MIT License
//
// MEL - MAHI Exoskeleton Library
// Copyright (c) 2018 Mechatronics and Haptic Interfaces Lab - Rice University
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

#ifndef MEL_TYPES_HPP
#define MEL_TYPES_HPP

#include <vector>
#include <array>

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
// DAQ TYPES
//==============================================================================

/// Represents a voltage in [V]
typedef double voltage;

/// Represents digital logic values
typedef int8 logic;
const        logic  LOW = 0;
const        logic HIGH = 1;

/// Represents encoder quadrature factors
enum QuadFactor {
    X0 = 0,  ///< no quadrature
    X1   = 1,  ///< 1X counts per revolution
    X2   = 2,  ///< 2X counts per revolution,
    X4   = 4,  ///< 4X counts per revolution
};

//==============================================================================
// STRUCTS
//==============================================================================

/// TODO: THIS NEEDS TO GO SOMEWHERE ELSE
/// Template 2D array for storing 2D data arrays and doing simple arithmetic
template <typename T, std::size_t N, std::size_t M>
struct array_2D {
public:
    array_2D() {};
    array_2D(T val) {
        for (auto it_row = array_.begin(); it_row != array_.end(); ++it_row) {
            for (auto it_col = (*it_row).begin(); it_col != (*it_row).end(); ++it_col) {
                *it_col = val;
            }
        }
    };
    std::array<T, M> operator [](int i) const { return array_[i]; }
    std::array<T, M>& operator [](int i) { return array_[i]; }
    std::array<T, N> multiply(std::array<T, M> b) {
        std::array<T, N> c;
        c.fill(0);
        for (auto it_row = array_.begin(); it_row != array_.end(); ++it_row) {
            for (auto it_col = (*it_row).begin(); it_col != (*it_row).end(); ++it_col) {
                c[it_row - array_.begin()] += *it_col * b[it_col - (*it_row).begin()];
            }
        }
        return c;
    };
private:
    std::array<std::array<T, M>, N> array_;
};

} // namespace mel

#endif // MEL_TYPES_HPP
