#include <MEL/Communications/TcpListener.hpp>
#include <MEL/Communications/TcpSocket.hpp>
#include <MEL/Logging/Log.hpp>
#include <iostream>

#ifdef _WIN32
#include <basetsd.h>
#ifdef _WIN32_WINDOWS
    #undef _WIN32_WINDOWS
#endif
#ifdef _WIN32_WINNT
    #undef _WIN32_WINNT
#endif
#define _WIN32_WINDOWS 0x0501
#define _WIN32_WINNT   0x0501
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#endif

namespace mel
{

TcpListener::TcpListener() :
Socket(Tcp)
{

}

unsigned short TcpListener::get_local_port() const
{
    if (get_handle() != Socket::invalid_socket())
    {
        // Retrieve informations about the local end of the socket
        sockaddr_in address;
        Socket::AddrLength size = sizeof(address);
        if (getsockname(get_handle(), reinterpret_cast<sockaddr*>(&address), &size) != -1)
        {
            return ntohs(address.sin_port);
        }
    }

    // We failed to retrieve the port
    return 0;
}
Socket::Status TcpListener::listen(unsigned short port, const IpAddress& address)
{
    /// Close the socket if it is already bound
    close();
    // Create the internal socket if it doesn't exist
    create();
    // Check if the address is valid
    if ((address == IpAddress::None) || (address == IpAddress::Broadcast))
        return Error;

    // Bind the socket to the specified port
    sockaddr_in addr = Socket::create_address(address.to_integer(), port);
    if (bind(get_handle(), reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) == -1)
    {
        // Not likely to happen, but...
        LOG(mel::Error) << "Failed to bind listener socket to port " << port;
        return Error;
    }

    // Listen to the bound port
    if (::listen(get_handle(), SOMAXCONN) == -1) // backlog = 0 failed on Linux
    {
        // Oops, socket is deaf
        LOG(mel::Error) << "Failed to listen to port " << port;
        return Error;
    }
    return Done;
}

void TcpListener::close()
{
    // Simply close the socket
    Socket::close();
}

Socket::Status TcpListener::accept(TcpSocket& socket)
{
    // Make sure that we're listening
    if (get_handle() == Socket::invalid_socket())
    {
        LOG(mel::Error) << "Failed to accept a new connection, the socket is not listening";
        return Error;
    }

    // Accept a new connection
    sockaddr_in address;
    Socket::AddrLength length = sizeof(address);
    SocketHandle remote = ::accept(get_handle(), reinterpret_cast<sockaddr*>(&address), &length);

    // Check for errors
    if (remote == Socket::invalid_socket())
        return Socket::get_error_status();

    // Initialize the new connected socket
    socket.close();
    socket.create(remote);

    return Done;
}

} // namespace mel

//==============================================================================
// APAPTED FROM: SFML (https://www.sfml-dev.org/)
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
//==============================================================================
