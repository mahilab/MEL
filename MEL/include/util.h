#pragma once
#include <vector>
#include <stdio.h>
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

    typedef unsigned int              uint;
    typedef std::vector<int>          int_vec;
    typedef std::vector<unsigned int> uint_vec;
    typedef std::vector<double>       double_vec;
    typedef std::vector<char>         char_vec;


    // Get current date/time, format is YYYY-MM-DD.HH:mm:ss
    const std::string get_current_date_time();
    void print_uint_vec(uint_vec vector);
    void print_int_vec(int_vec vector);
    void print_double_vec(double_vec vector);
}