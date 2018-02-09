#include <MEL/Communications/Socket.hpp>
#include <MEL/Logging/Log.hpp>
#include <cstring>
#include <iostream>

#ifdef __linux__
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#elif _WIN32
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
#endif
namespace mel
{

//==============================================================================
// CLASS DEFINITIONS
//==============================================================================

Socket::Socket(Type type) :
    type_      (type),
    socket_    (invalid_socket()),
    is_blocking_(true)
{
}

Socket::~Socket() {
    // Close the socket before it gets destructed
    close();
}

void Socket::set_blocking(bool blocking) {
    // Apply if the socket is already created
    if (socket_ != invalid_socket())
        set_blocking(socket_, blocking);
    is_blocking_ = blocking;
}

bool Socket::is_blocking() const {
    return is_blocking_;
}

SocketHandle Socket::get_handle() const {
    return socket_;
}

void Socket::create()
{
    // Don't create the socket if it already exists
    if (socket_ == invalid_socket()) {
        SocketHandle handle = socket(PF_INET, type_ == Tcp ? SOCK_STREAM : SOCK_DGRAM, 0);
        if (handle == invalid_socket())         {
            LOG(mel::Error) << "Failed to create socket";
            return;
        }
        create(handle);
    }
}

void Socket::create(SocketHandle handle) {
    // Don't create the socket if it already exists
    if (socket_ == invalid_socket()) {
        // Assign the new handle
        socket_ = handle;

        // Set the current blocking state
        set_blocking(is_blocking_);

        if (type_ == Tcp) {
            // Disable the Nagle algorithm (i.e. removes buffering of TCP packets)
            int yes = 1;
            if (setsockopt(socket_, IPPROTO_TCP, TCP_NODELAY, reinterpret_cast<char*>(&yes), sizeof(yes)) == -1) {
                LOG(Warning) << "Failed to set socket option \"TCP_NODELAY\" ; " << "all your TCP packets will be buffered";
            }
        }
        else {
            // Enable broadcast by default for UDP sockets
            int yes = 1;
            if (setsockopt(socket_, SOL_SOCKET, SO_BROADCAST, reinterpret_cast<char*>(&yes), sizeof(yes)) == -1) {
                LOG(mel::Error) << "Failed to enable broadcast on UDP socket";
            }
        }
    }
}

void Socket::close() {
    // Close the socket
    if (socket_ != invalid_socket())
    {
        close(socket_);
        socket_ = invalid_socket();
    }
}

//==============================================================================
// LINUX IMPLEMENTATION
//==============================================================================

#ifdef __linux__

sockaddr_in Socket::create_address(uint32 address, unsigned short port) {
    sockaddr_in addr;
    std::memset(&addr, 0, sizeof(addr));
    addr.sin_addr.s_addr = htonl(address);
    addr.sin_family      = AF_INET;
    addr.sin_port        = htons(port);

#if defined(SFML_SYSTEM_MACOS)
    addr.sin_len = sizeof(addr);
#endif

    return addr;
}

SocketHandle Socket::invalid_socket() {
    return -1;
}

void Socket::close(SocketHandle sock) {
    ::close(sock);
}

void Socket::set_blocking(SocketHandle sock, bool block) {
    int status = fcntl(sock, F_GETFL);
    if (block)
    {
        if (fcntl(sock, F_SETFL, status & ~O_NONBLOCK) == -1) {
            LOG(mel::Error) << "Failed to set file status flags: " << errno;
        }
    }
    else
    {
        if (fcntl(sock, F_SETFL, status | O_NONBLOCK) == -1) {
           LOG(mel::Error) << "Failed to set file status flags: " << errno;
        }

    }
}

Socket::Status Socket::get_error_status() {
    // The followings are sometimes equal to EWOULDBLOCK,
    // so we have to make a special case for them in order
    // to avoid having double values in the switch case
    if ((errno == EAGAIN) || (errno == EINPROGRESS))
        return Socket::NotReady;

    switch (errno)
    {
        case EWOULDBLOCK:  return Socket::NotReady;
        case ECONNABORTED: return Socket::Disconnected;
        case ECONNRESET:   return Socket::Disconnected;
        case ETIMEDOUT:    return Socket::Disconnected;
        case ENETRESET:    return Socket::Disconnected;
        case ENOTCONN:     return Socket::Disconnected;
        case EPIPE:        return Socket::Disconnected;
        default:           return Socket::Error;
    }
}

//==============================================================================
// WINDOWS IMPLEMENTATION
//==============================================================================

#elif _WIN32

// Windows needs some initialization and cleanup to get sockets working
// properly... so let's create a class that will do it automatically
struct SocketInitializer {
    SocketInitializer() {
        WSADATA init;
        WSAStartup(MAKEWORD(2, 2), &init);
    }

    ~SocketInitializer() {
        WSACleanup();
    }
};

SocketInitializer globalInitializer;

sockaddr_in Socket::create_address(uint32 address, unsigned short port) {
    sockaddr_in addr;
    std::memset(&addr, 0, sizeof(addr));
    addr.sin_addr.s_addr = htonl(address);
    addr.sin_family      = AF_INET;
    addr.sin_port        = htons(port);

    return addr;
}

SocketHandle Socket::invalid_socket() {
    return INVALID_SOCKET;
}

void Socket::close(SocketHandle sock) {
    closesocket(sock);
}


void Socket::set_blocking(SocketHandle sock, bool block) {
    u_long blocking = block ? 0 : 1;
    ioctlsocket(sock, FIONBIO, &blocking);
}

Socket::Status Socket::get_error_status() {
    switch (WSAGetLastError()) {
        case WSAEWOULDBLOCK:  return Socket::NotReady;
        case WSAEALREADY:     return Socket::NotReady;
        case WSAECONNABORTED: return Socket::Disconnected;
        case WSAECONNRESET:   return Socket::Disconnected;
        case WSAETIMEDOUT:    return Socket::Disconnected;
        case WSAENETRESET:    return Socket::Disconnected;
        case WSAENOTCONN:     return Socket::Disconnected;
        case WSAEISCONN:      return Socket::Done; // when connecting a non-blocking socket
        default:              return Socket::Error;
    }
}

#endif

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
