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

#ifndef MEL_TCPLISTENER_HPP
#define MEL_TCPLISTENER_HPP

#include <MEL/Communications/Socket.hpp>
#include <MEL/Communications/IpAddress.hpp>


namespace mel
{

//==============================================================================
// FORWARD DECLARATIONS
//==============================================================================

class TcpSocket;

//==============================================================================
// CLASS DECLARATION
//==============================================================================

/// Socket that listens to new TCP connections
class TcpListener : public Socket
{
public:

    /// Default constructor
    TcpListener();

    /// Get the port to which the socket is bound locally
    ///
    /// If the socket is not listening to a port, this function
    /// returns 0.
    unsigned short get_local_port() const;

    /// Start listening for connections
    ///
    /// This functions makes the socket listen to the specified
    /// port, waiting for new connections.
    /// If the socket was previously listening to another port,
    /// it will be stopped first and bound to the new port.
    Status listen(unsigned short port, const IpAddress& address = IpAddress::Any);

    /// Stop listening and close the socket
    ///
    /// This function gracefully stops the listener. If the
    /// socket is not listening, this function has no effect.
    void close();

    /// Accept a new connection
    ///
    /// If the socket is in blocking mode, this function will
    /// not return until a connection is actually received.
    Status accept(TcpSocket& socket);
};

} // namespace mel

#endif // MEL_TCPLISTENER_HPP

//==============================================================================
// CLASS DOCUMENTATIOIN
//==============================================================================

/// \class mel::TcpListener
/// \ingroup Communications
///
/// A listener socket is a special type of socket that listens to
/// a given port and waits for connections on that port.
/// This is all it can do.
///
/// When a new connection is received, you must call accept and
/// the listener returns a new instance of mel::TcpSocket that
/// is properly initialized and can be used to communicate with
/// the new client.
///
/// Listener sockets are specific to the TCP protocol,
/// UDP sockets are connectionless and can therefore communicate
/// directly. As a consequence, a listener socket will always
/// return the new connections as mel::TcpSocket instances.
///
/// A listener is automatically closed on destruction, like all
/// other types of socket. However if you want to stop listening
/// before the socket is destroyed, you can call its close()
/// function.
///
/// Usage example:
/// \code
/// // Create a listener socket and make it wait for new
/// // connections on port 55001
/// mel::TcpListener listener;
/// listener.listen(55001);
///
/// // Endless loop that waits for new connections
/// while (running)
/// {
///     mel::TcpSocket client;
///     if (listener.accept(client) == mel::Socket::Done)
///     {
///         // A new client just connected!
///         std::cout << "New connection received from " << client.getRemoteAddress() << std::endl;
///         doSomethingWith(client);
///     }
/// }
/// \endcode
///
/// \see mel::TcpSocket, mel::Socket

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
