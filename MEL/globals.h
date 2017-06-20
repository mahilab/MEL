#pragma once

#include <vector>

#define PI      3.14159265358979E+00 /* constant pi to double precision */
#define DEG2RAD 1.74532925199433E-02 /* constant to multiply by to convert degrees to radians */
#define RAD2DEG 5.72957795130824E+01 /* constant to multiply by to convert radians to degrees */

#define TEST 1

#define TEST2 4

typedef unsigned int              uint;
typedef std::vector<int>          int_vec;
typedef std::vector<unsigned int> uint_vec;
typedef std::vector<float>        float_vec;
typedef std::vector<double>       double_vec;
typedef std::vector<char>         char_vec;

/* macro for find array length */
#define ARRAY_LENGTH(array) (sizeof(array)/sizeof((array)[0]))

/* a better version of ARRAY_LENGTH */
#define COUNT_OF(x) ((sizeof(x)/sizeof(0[x])) / ((size_t)(!(sizeof(x) % sizeof(0[x])))))