#pragma once
#include <vector>
#include <stdio.h>
#include <time.h>

#define PI      3.14159265358979E+00  /* constant pi */
#define DEG2RAD 1.74532925199433E-02  /* multiply degrees by this value to convert to radians */
#define RAD2DEG 5.72957795130824E+01  /* multiply radians by this value to convert to degrees */

/* macro for find array length */
#define ARRAY_LENGTH(array) (sizeof(array)/sizeof((array)[0]))

/* a better version of ARRAY_LENGTH */
#define COUNT_OF(x) ((sizeof(x)/sizeof(0[x])) / ((size_t)(!(sizeof(x) % sizeof(0[x])))))

typedef unsigned int              uint;
typedef std::vector<int>          int_vec;
typedef std::vector<unsigned int> uint_vec;
typedef std::vector<float>        float_vec;
typedef std::vector<double>       double_vec;
typedef std::vector<char>         char_vec;

// Get current date/time, format is YYYY-MM-DD.HH:mm:ss
const std::string get_current_data_time();