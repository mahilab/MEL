#pragma once
#include <vector>
#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <iostream>

namespace mel {

    const double PI = 3.14159265358979E+00;  // constant pi
    const double DEG2RAD = 1.74532925199433E-02;  // multiply degrees by this value to convert to radians
    const double RAD2DEG = 5.72957795130824E+01;  // multiply radians by this value to convert to degrees
    const double INCH2METER = 0.0254; // multiply inches by this value to convert to meters
    const double METER2INCH = 1.0 / 0.0254; // multiply meters by this value to convert to inches

    /*
    // macro for find array length
    #define ARRAY_LENGTH(array) (sizeof(array)/sizeof((array)[0]))

    // a better version of ARRAY_LENGTH 
    #define COUNT_OF(x) ((sizeof(x)/sizeof(0[x])) / ((size_t)(!(sizeof(x) % sizeof(0[x])))))
    */

    // TYPEDEFS

    typedef   signed    char  int8;
    typedef unsigned    char uint8;
    typedef            short  int16;
    typedef unsigned   short uint16;
    typedef              int  int32;
    typedef unsigned     int uint32;
    typedef          __int64  int64;
    typedef unsigned __int64 uint64;

    typedef std::vector<int8>   int8_vec;
    typedef std::vector<uint8>  uint8_vec;
    typedef std::vector<int16>  int16_vec;
    typedef std::vector<uint16> uint16_vec;
    typedef std::vector<int32>  int32_vec;
    typedef std::vector<uint32> uint32_vec;
    typedef std::vector<int64>  int64_vec;
    typedef std::vector<uint64> uint64_vec;
    typedef std::vector<double> double_vec;

    // Get current date/time, format is YYYY-MM-DD.HH:mm:ss
    const std::string get_current_date_time();
    void print_uint_vec(uint32_vec vector);
    void print_int_vec(int32_vec vector);
    void print_double_vec(double_vec vector);
}