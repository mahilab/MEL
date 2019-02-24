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

#pragma once

#include <MEL/Communications/Packet.hpp>
#include <MEL/Communications/UdpSocket.hpp>
#include <MEL/Core/NonCopyable.hpp>
#include <string>
#include <vector>

namespace mel {

//==============================================================================
// CLASS DECLARATION
//==============================================================================

/// High-level communication class that simplifies UDP communication
class MelNet : NonCopyable {
public:
    /// Default constructor. Use ports in the range of 49152 to 65535.
    MelNet(unsigned short local_port,
           unsigned short remote_port,
           IpAddress remote_address,
           bool blocking = true);

    /// Sends a vector of doubles to the remote host
    void send_data(const std::vector<double>& data);

    /// Receives a vector of doubles from the remote host
    std::vector<double> receive_data();

    /// Sends a string message to the remote host
    void send_message(const std::string& message);

    /// Receives a string message from the remote host
    std::string receive_message();

    /// Requests that new information be sent
    void request();

    /// Returns true if information is being requested, false otherwise
    bool check_request();

    /// Set the blocking state of the MelNet
    void set_blocking(bool blocking);

    /// Tell whether the MelNet is in blocking or non-blocking mode
    bool is_blocking() const;

private:
    unsigned short local_port_;  ///< The port to receive data on on the local host
    unsigned short remote_port_;  ///< The port to send data to on the remote host

    IpAddress remote_address_;  ///< The remote IP address to send data too

    UdpSocket socket_;       ///< The underlying UDP socket
    Packet packet_send_;     ///< The packet used to send data
    Packet packet_receive_;  ///< The packet used to receive data
};

}  // namespace mel

//==============================================================================
// CLASS DOCUMENTATION
//==============================================================================
