#pragma once
#include <vector>
#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <iostream>

namespace mel {

    // CONSTANTS

    const double PI         = 3.14159265358979E+00;  // constant pi
    const double DEG2RAD    = 1.74532925199433E-02;  // multiply degrees by this value to convert to radians
    const double RAD2DEG    = 5.72957795130824E+01;  // multiply radians by this value to convert to degrees

    const double INCH2METER = 0.0254;                // multiply inches by this value to convert to meters
    const double METER2INCH = 1.0 / 0.0254;          // multiply meters by this value to convert to inches

    // TYPEDEF BASIC TYPES

    typedef             char  int8;   //           -127 to +127
    typedef unsigned    char uint8;   //              0 to +255
    typedef            short  int16;  //        -32,767 to +32,767
    typedef unsigned   short uint16;  //              0 to +65,535
    typedef              int  int32;  // -2,147,483,648 to +2,147,483,647
    typedef unsigned     int uint32;  //              0 to +4,294,967,295

    typedef std::vector<int8>   int8_vec;
    typedef std::vector<uint8>  uint8_vec;
    typedef std::vector<int16>  int16_vec;
    typedef std::vector<uint16> uint16_vec;
    typedef std::vector<int32>  int32_vec;
    typedef std::vector<uint32> uint32_vec;
    typedef std::vector<double> double_vec;


    // TYPDEF MEL TYPES BASED ON DAQ MANUFACTUERER

#define QUANSER
#ifdef QUANSER

    typedef double voltage;
    typedef int8   dsignal;
    typedef uint32 channel;

    typedef std::vector<voltage> voltage_vec;
    typedef std::vector<dsignal> dsignal_vec;
    typedef std::vector<channel> channel_vec;

#endif

#ifdef NI

#endif

    // UTILITY FUNCTIONS

    const std::string get_current_date_time();  // Get current date/time, format is YYYY-MM-DD.HH:mm:ss
    void print_uint_vec(uint32_vec vector);
    void print_int_vec(int32_vec vector);
    void print_double_vec(double_vec vector);

    // GENERIC FUNCTIONS
    double sin_trajectory(double amplitude, double frequency, double time);
}