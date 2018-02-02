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

#ifndef MEL_UDPSOCKET_HPP
#define MEL_UDPSOCKET_HPP

#include <MEL/Communications/Socket.hpp>
#include <MEL/Communications/IpAddress.hpp>
#include <vector>

namespace mel {

//==============================================================================
// FORWAD DECLARATION
//==============================================================================

class Packet;

//==============================================================================
// CLASS DECLARATION
//==============================================================================

/// Specialized socket using the UDP protocol
class UdpSocket : public Socket {

public:

    enum {
        MaxDatagramSize = 65507 ///< The maximum number of bytes that can be sent in a single UDP datagram
    };

    /// Default constructor
    UdpSocket();

    /// Get the port to which the socket is bound locally
    /// If the socket is not bound to a port, this function returns 0.
    unsigned short get_local_port() const;

    /// Bind the socket to a specific port
    ///
    /// Binding the socket to a port is necessary for being
    /// able to receive data on that port.
    /// You can use the special value Socket::AnyPort to tell the
    /// system to automatically pick an available port, and then
    /// call get_local_port to retrieve the chosen port.
    Status bind(unsigned short port, const IpAddress& address = IpAddress::Any);

    /// Unbind the socket from the local port to which it is bound
    ///
    /// The port that the socket was previously bound to is immediately
    /// made available to the operating system after this function is called.
    /// This means that a subsequent call to bind() will be able to re-bind
    /// the port if no other process has done so in the mean time.
    /// If the socket is not bound to a port, this function has no effect.
    void unbind();

    /// Send raw data to a remote peer
    ///
    /// Make sure that \a size is not greater than
    /// UdpSocket::MaxDatagramSize, otherwise this function will
    /// fail and no data will be sent.
    ///
    /// \param data           Pointer to the sequence of bytes to send
    /// \param size           Number of bytes to send
    /// \param remote_address Address of the receiver
    /// \param remote_port    Port of the receiver to send the data to
    Status send(const void* data, std::size_t size, const IpAddress& remote_address, unsigned short remote_port);

    /// Receive raw data from a remote peer
    ///
    /// In blocking mode, this function will wait until some
    /// bytes are actually received.
    /// Be careful to use a buffer which is large enough for
    /// the data that you intend to receive, if it is too small
    /// then an error will be returned and *all* the data will
    /// be lost.
    ///
    /// \param data           Pointer to the array to fill with the received bytes
    /// \param size           Maximum number of bytes that can be received
    /// \param received       This variable is filled with the actual number of bytes received
    /// \param remote_address Address of the peer that sent the data
    /// \param remote_port    Port of the peer that sent the data
    Status receive(void* data, std::size_t size, std::size_t& received, IpAddress& remote_address, unsigned short& remote_port);

    /// Send a formatted packet of data to a remote peer
    ///
    /// Make sure that the packet size is not greater than
    /// UdpSocket::MaxDatagramSize, otherwise this function will
    /// fail and no data will be sent.
    ///
    /// \param packet         Packet to send
    /// \param remote_address Address of the receiver
    /// \param remote_port    Port of the receiver to send the data to
    Status send(Packet& packet, const IpAddress& remote_address, unsigned short remote_port);

    /// Receive a formatted packet of data from a remote peer
    ///
    /// In blocking mode, this function will wait until the whole packet
    /// has been received.
    ///
    /// \param packet         Packet to fill with the received data
    /// \param remote_address Address of the peer that sent the data
    /// \param remote_port    Port of the peer that sent the data
    Status receive(Packet& packet, IpAddress& remote_address, unsigned short& remote_port);

private:

    std::vector<char> buffer_; ///< Temporary buffer holding the received data in Receive(Packet)

};

} // namespace mel

#endif // MEL_UDPSOCKET_HPP

//==============================================================================
// CLASS DOCUMENTATION
//==============================================================================

/// \class mel::UdpSocket
/// \ingroup Communications
///
/// A UDP socket is a connectionless socket. Instead of
/// connecting once to a remote host, like TCP sockets,
/// it can send to and receive from any host at any time.
///
/// It is a datagram protocol: bounded blocks of data (datagrams)
/// are tranmelered over the network rather than a continuous
/// stream of data (TCP). Therefore, one call to send will always
/// match one call to receive (if the datagram is not lost),
/// with the same data that was sent.
///
/// The UDP protocol is lightweight but unreliable. Unreliable
/// means that datagrams may be duplicated, be lost or
/// arrive reordered. However, if a datagram arrives, its
/// data is guaranteed to be valid.
///
/// UDP is generally used for real-time communication
/// (audio or video streaming, real-time games, etc.) where
/// speed is crucial and lost data doesn't matter much.
///
/// Sending and receiving data can use either the low-level
/// or the high-level functions. The low-level functions
/// process a raw sequence of bytes, whereas the high-level
/// interface uses packets (see mel::Packet), which are easier
/// to use and provide more safety regarding the data that is
/// exchanged. You can look at the mel::Packet class to get
/// more details about how they work.
///
/// It is important to note that UdpSocket is unable to send
/// datagrams bigger than MaxDatagramSize. In this case, it
/// returns an error and doesn't send anything. This applies
/// to both raw data and packets. Indeed, even packets are
/// unable to split and recompose data, due to the unreliability
/// of the protocol (dropped, mixed or duplicated datagrams may
/// lead to a big mess when trying to recompose a packet).
///
/// If the socket is bound to a port, it is automatically
/// unbound from it when the socket is destroyed. However,
/// you can unbind the socket explicitly with the Unbind
/// function if necessary, to stop receiving messages or
/// make the port available for other sockets.
///
/// Usage example:
/// \code
/// // ----- The client -----
///
/// // Create a socket and bind it to the port 55001
/// mel::UdpSocket socket;
/// socket.bind(55001);
///
/// // Send a message to 192.168.1.50 on port 55002
/// std::string message = "Hi, I am " + mel::IpAddress::getLocalAddress().toString();
/// socket.send(message.c_str(), message.size() + 1, "192.168.1.50", 55002);
///
/// // Receive an answer (most likely from 192.168.1.50, but could be anyone else)
/// char buffer[1024];
/// std::size_t received = 0;
/// mel::IpAddress sender;
/// unsigned short port;
/// socket.receive(buffer, sizeof(buffer), received, sender, port);
/// std::cout << sender.ToString() << " said: " << buffer << std::endl;
///
/// // ----- The server -----
///
/// // Create a socket and bind it to the port 55002
/// mel::UdpSocket socket;
/// socket.bind(55002);
///
/// // Receive a message from anyone
/// char buffer[1024];
/// std::size_t received = 0;
/// mel::IpAddress sender;
/// unsigned short port;
/// socket.receive(buffer, sizeof(buffer), received, sender, port);
/// std::cout << sender.ToString() << " said: " << buffer << std::endl;
///
/// // Send an answer
/// std::string message = "Welcome " + sender.toString();
/// socket.send(message.c_str(), message.size() + 1, sender, port);
/// \endcode
///
/// \see mel::Socket, mel::TcpSocket, mel::Packet

//==============================================================================
// LICENSES
//==============================================================================

// SFML - Simple and Fast Multimedia Library
// Copyright (C) 2007-2017 Laurent Gomila (laurent@sfml-dev.org)
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it freely,
// subject to the following restrictions:
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
