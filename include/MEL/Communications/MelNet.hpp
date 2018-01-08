#pragma once

#include <MEL/Communications/UdpSocket.hpp>
#include <MEL/Communications/Packet.hpp>
#include <MEL/Utility/NonCopyable.hpp>
#include <vector>
#include <string>

namespace mel {

//==============================================================================
// CLASS DECLARATION
//==============================================================================

/// High-level communication class that simplifies UDP communication
class MelNet : NonCopyable {

public:

    /// Default constructor. Use ports in the range of 49152 to 65535.
    MelNet(unsigned int local_port, unsigned int remote_port,
           IpAddress remote_address, bool blocking = true);

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

    unsigned int local_port_;   ///< The port to receive data on on the local host
    unsigned int remote_port_;  ///< The port to send data to on the remote host

    IpAddress remote_address_;  ///< The remote IP address to send data too

    UdpSocket socket_;          ///< The underlying UDP socket
    Packet packet_send_;        ///< The packet used to send data
    Packet packet_receive_;     ///< The packet used to receive data

};

} // namespace mel

//==============================================================================
// CLASS DOCUMENTATION
//==============================================================================
