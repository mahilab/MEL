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
// This particular source file includes code which has been adapted from the
// following open-source projects (all external licenses attached at bottom):
//     SFML - Simple and Fast Multimedia Library
//
// Author(s): Evan Pezent (epezent@rice.edu)

#ifndef MEL_TCPSOCKET_HPP
#define MEL_TCPSOCKET_HPP

#include <MEL/Communications/Socket.hpp>
#include <MEL/Utility/Time.hpp>

namespace mel {

//==============================================================================
// FORWARD DECLARATIONS
//==============================================================================

class TcpListener;
class IpAddress;
class Packet;

//==============================================================================
// CLASS DECLARATION
//==============================================================================

/// Specialized socket using the TCP protocol
class TcpSocket : public Socket {
public:
    /// Default constructor
    TcpSocket();

    /// Get the port to which the socket is bound locally
    ///
    /// If the socket is not connected, this function returns 0.
    unsigned short get_local_port() const;

    /// Get the address of the connected peer
    ///
    /// It the socket is not connected, this function returns
    /// sf::IpAddress::None.
    IpAddress get_remote_address() const;

    /// Get the port of the connected peer to which the socket is connected
    ///
    /// If the socket is not connected, this function returns 0.
    unsigned short get_remote_port() const;

    /// Connect the socket to a remote peer
    ///
    /// In blocking mode, this function may take a while, especially
    /// if the remote peer is not reachable. The last parameter allows
    /// you to stop trying to connect after a given timeout.
    /// If the socket was previously connected, it is first disconnected.
    Status connect(const IpAddress& remote_address,
                   unsigned short remote_port,
                   Time timeout = Time::Zero);

    /// Disconnect the socket from its remote peer
    ///
    /// This function gracefully closes the connection. If the
    /// socket is not connected, this function has no effect.
    void disconnect();

    /// Send raw data to the remote peer
    ///
    /// To be able to handle partial sends over non-blocking
    /// sockets, use the send(const void*, std::size_t, std::size_t&)
    /// overload instead.
    /// This function will fail if the socket is not connected.
    Status send(const void* data, std::size_t size);

    /// Send raw data to the remote peer
    ///
    /// This function will fail if the socket is not connected.
    Status send(const void* data, std::size_t size, std::size_t& sent);

    /// Receive raw data from the remote peer
    ///
    /// In blocking mode, this function will wait until some
    /// bytes are actually received.
    /// This function will fail if the socket is not connected.
    Status receive(void* data, std::size_t size, std::size_t& received);

    /// Send a formatted packet of data to the remote peer
    ///
    /// In non-blocking mode, if this function returns sf::Socket::Partial,
    /// you \em must retry sending the same unmodified packet before sending
    /// anything else in order to guarantee the packet arrives at the remote
    /// peer uncorrupted.
    /// This function will fail if the socket is not connected.
    Status send(Packet& packet);

    /// Receive a formatted packet of data from the remote peer
    ///
    /// In blocking mode, this function will wait until the whole packet
    /// has been received.
    /// This function will fail if the socket is not connected.
    Status receive(Packet& packet);

private:
    friend class TcpListener;

    /// Structure holding the data of a pending packet
    struct PendingPacket {
        PendingPacket();

        uint32 Size;               ///< Data of packet size
        std::size_t SizeReceived;  ///< Number of size bytes received so far
        std::vector<char> Data;    ///< Data of the packet
    };

    PendingPacket pending_packet_;  ///< Temporary data of the packet currently
                                    ///< being received
};

}  // namespace mel

#endif  // MEL_TCPSOCKET_HPP

//==============================================================================
// CLASS DOCUMENTATION
//==============================================================================

/// \class mel::TcpSocket
/// \ingroup Communications
///
/// TCP is a connected protocol, which means that a TCP
/// socket can only communicate with the host it is connected
/// to. It can't send or receive anything if it is not connected.
///
/// The TCP protocol is reliable but adds a slight overhead.
/// It ensures that your data will always be received in order
/// and without errors (no data corrupted, lost or duplicated).
///
/// When a socket is connected to a remote host, you can
/// retrieve informations about this host with the
/// get_remote_address and get_remote_port functions. You can
/// also get the local port to which the socket is bound
/// (which is automatically chosen when the socket is connected),
/// with the get_local_port function.
///
/// Sending and receiving data can use either the low-level
/// or the high-level functions. The low-level functions
/// process a raw sequence of bytes, and cannot ensure that
/// one call to Send will exactly match one call to Receive
/// at the other end of the socket.
///
/// The high-level interface uses packets (see mel::Packet),
/// which are easier to use and provide more safety regarding
/// the data that is exchanged. You can look at the mel::Packet
/// class to get more details about how they work.
///
/// The socket is automatically disconnected when it is destroyed,
/// but if you want to explicitly close the connection while
/// the socket instance is still alive, you can call disconnect.
///
/// Usage example:
/// \code
/// // ----- The client -----
///
/// // Create a socket and connect it to 192.168.1.50 on port 55001
/// mel::TcpSocket socket;
/// socket.connect("192.168.1.50", 55001);
///
/// // Send a message to the connected host
/// std::string message = "Hi, I am a client";
/// socket.send(message.c_str(), message.size() + 1);
///
/// // Receive an answer from the server
/// char buffer[1024];
/// std::size_t received = 0;
/// socket.receive(buffer, sizeof(buffer), received);
/// std::cout << "The server said: " << buffer << std::endl;
///
/// // ----- The server -----
///
/// // Create a listener to wait for incoming connections on port 55001
/// mel::TcpListener listener;
/// listener.listen(55001);
///
/// // Wait for a connection
/// mel::TcpSocket socket;
/// listener.accept(socket);
/// std::cout << "New client connected: " << socket.get_remote_address() <<
/// std::endl;
///
/// // Receive a message from the client
/// char buffer[1024];
/// std::size_t received = 0;
/// socket.receive(buffer, sizeof(buffer), received);
/// std::cout << "The client said: " << buffer << std::endl;
///
/// // Send an answer
/// std::string message = "Welcome, client";
/// socket.send(message.c_str(), message.size() + 1);
/// \endcode
///
/// \see mel::Socket, mel::UdpSocket, mel::Packet

//==============================================================================
// LICENSES
//==============================================================================

// SFML - Simple and Fast Multimedia Library
// Copyright (C) 2007-2017 Laurent Gomila (laurent@sfml-dev.org)
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from the
// use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented;
//    you must not claim that you wrote the original software.
//    If you use this software in a product, an acknowledgment
//    in the product documentation would be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such,
//    and must not be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source distribution.
