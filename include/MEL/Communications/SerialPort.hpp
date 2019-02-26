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
#include <MEL/Core/NonCopyable.hpp>
#include <MEL/Communications/SerialSettings.hpp>
#include <string>

namespace mel {

/// Interface to RS-232 Serial Port
class SerialPort : NonCopyable {
public:

    /// Constructor
    SerialPort(std::size_t port, std::size_t baudrate = 9600, std::string mode = "8N1");

    /// Destructor
    ~SerialPort();

    /// Opens communication on specified port
    bool open();

    /// Closes communication on specified port
    bool close();

    /// Sends data
    bool send_data(unsigned char* data, std::size_t size);

    /// Receives data
    int receive_data(unsigned char* data, std::size_t size);

    /// Returns true if SerialPort is open
    bool is_open() const;

private:

    bool is_open_;
    std::size_t port_;
    std::size_t baudrate_;
    std::string mode_;
};

} // namespace mel
