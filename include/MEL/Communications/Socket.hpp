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
// This particular source file includes code which has been adapted from the
// following open-source projects (all external licenses attached at bottom):
//     SFML - Simple and Fast Multimedia Library
//
// Author(s): Evan Pezent (epezent@rice.edu)

#pragma once

#include <MEL/Core/NonCopyable.hpp>
#include <MEL/Core/Types.hpp>
#include <vector>

#ifdef _WIN32
#include <basetsd.h>
#else
#include <sys/socket.h>
#endif

struct sockaddr_in;

namespace mel {

//==============================================================================
// TYPEDEFS
//==============================================================================

// Low-level socket handle type, specific to each platform
#if defined(_WIN32)
typedef UINT_PTR SocketHandle;
#else
typedef int SocketHandle;
#endif

//==============================================================================
// FORWARD DECLARATIONS
//==============================================================================

class TcpSocket;
class TcpListener;
class UdpSocket;
class IpAddress;
class Packet;
class SocketSelector;

//==============================================================================
// CLASS DECLARATION
//==============================================================================

/// Base class for all the socket types
class Socket : NonCopyable {
public:
    /// Types of protocols that the socket can use
    enum Type {
        Tcp,  ///< TCP protocol
        Udp   ///< UDP protocol
    };

    /// Status codes that may be returned by socket functions
    enum Status {
        Done,          ///< The socket has sent / received the data
        NotReady,      ///< The socket is not ready to send / receive data yet
        Partial,       ///< The socket sent a part of the data
        Disconnected,  ///< The TCP socket has been disconnected
        Error          ///< An unexpected error happened
    };

    /// Some special values used by sockets
    enum {
        AnyPort = 0  ///< Special value that tells the system to pick any
                     ///< available port
    };

#ifdef _WIN32
    typedef int AddrLength;
#else
    typedef socklen_t AddrLength;
#endif

public:
    /// Destructor
    virtual ~Socket();

    /// Set the blocking state of the socket
    ///
    /// In blocking mode, calls will not return until they have
    /// completed their task. For example, a call to Receive in
    /// blocking mode won't return until some data was actually
    /// received.
    /// In non-blocking mode, calls will always return immediately,
    /// using the return code to signal whether there was data
    /// available or not.
    /// By default, all sockets are blocking.
    void set_blocking(bool blocking);

    /// Tell whether the socket is in blocking or non-blocking mode
    bool is_blocking() const;

protected:
    /// Default constructor.
    /// This constructor can only be accessed by derived classes.
    Socket(Type type);

    /// Return the internal handle of the socket
    ///
    /// The returned handle may be invalid if the socket
    /// was not created yet (or already destroyed).
    /// This function can only be accessed by derived classes.
    SocketHandle get_handle() const;

    /// Create the internal representation of the socket
    /// This function can only be accessed by derived classes.
    void create();

    /// Create the internal representation of the socket from a socket handle
    /// This function can only be accessed by derived classes.
    void create(SocketHandle handle);

    /// Close the socket gracefully
    /// This function can only be accessed by derived classes.
    void close();

private:
    friend class TcpSocket;
    friend class TcpListener;
    friend class UdpSocket;
    friend class IpAddress;
    friend class Packet;
    friend class SocketSelector;

    /// Create an internal sockaddr_in address
    static sockaddr_in create_address(uint32 address, unsigned short port);

    /// Return the value of the invalid socket
    static SocketHandle invalid_socket();

    /// Close and destroy a socket
    static void close(SocketHandle sock);

    /// Set a socket as blocking or non-blocking
    static void set_blocking(SocketHandle sock, bool block);

    /// Get the last socket error status
    static Status get_error_status();

    // Member data
    Type type_;            ///< Type of the socket (TCP or UDP)
    SocketHandle socket_;  ///< Socket descriptor
    bool is_blocking_;     ///< Current blocking mode of the socket
};

}  // namespace mel

//==============================================================================
// CLASS DOCUMENTATION
//==============================================================================

/// \class mel::Socket
/// \ingroup Communications
///
/// This class mainly defines internal stuff to be used by
/// derived classes.
///
/// The only public features that it defines, and which
/// is therefore common to all the socket classes, is the
/// blocking state. All sockets can be set as blocking or
/// non-blocking.
///
/// In blocking mode, socket functions will hang until
/// the operation completes, which means that the entire
/// program (well, in fact the current thread if you use
/// multiple ones) will be stuck waiting for your socket
/// operation to complete.
///
/// In non-blocking mode, all the socket functions will
/// return immediately. If the socket is not ready to complete
/// the requested operation, the function simply returns
/// the proper status code (Socket::NotReady).
///
/// The default mode, which is blocking, is the one that is
/// generally used, in combination with threads or selectors.
/// The non-blocking mode is rather used in real-time
/// applications that run an endless loop that can poll
/// the socket often enough, and cannot afford blocking
/// this loop.
///
/// \see mel::TcpListener, mel::TcpSocket, mel::UdpSocket

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
