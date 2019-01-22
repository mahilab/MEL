// MIT License
//
// MEL - Mechatronics Engine & Library
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

#pragma once

#if defined(__linux__) || defined(__FreeBSD__)

enum Port {
    ttyS0 = 0,
    ttyS1, 
    ttyS2,
    ttyS3,
    ttyS4, 
    ttyS5, 
    ttyS6,
    ttyS7,
    ttyS8,
    ttyS9,
    ttyS10,
    ttyS11,
    ttyS12,
    ttyS13,
    ttyS14,
    ttyS15,
    ttyUSB0,
    ttyUSB1,
    ttyUSB2,
    ttyUSB3,
    ttyUSB4,
    ttyUSB5,
    ttyAMA0,
    ttyAMA1,
    ttyACM0,
    ttyACM1,
    rfcomm0,
    rfcomm1,
    ircomm0,
    ircomm1,
    cuau0,
    cuau1,
    cuau2,
    cuau3,
    cuaU0,
    cuaU1,
    cuaU2,
    cuaU3,
};

#else

enum Port {
    COM1  = 0,
    COM2  = 1,
    COM3  = 2,
    COM4  = 3,
    COM5  = 4,
    COM6  = 5,
    COM7  = 6,
    COM8  = 7,
    COM9  = 8,
    COM10 = 9,
    COM11 = 10,
    COM12 = 11,
    COM13 = 12,
    COM14 = 13,
    COM15 = 14,
    COM16 = 15
};

#endif

