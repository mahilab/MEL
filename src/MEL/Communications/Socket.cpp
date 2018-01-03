#include <MEL/Communications/Socket.hpp>
// #include <SFML/System/Err.hpp>
#include <cstring>
#include <iostream>

#ifdef __linux__
    #include <errno.h>
    #include <fcntl.h>
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
            std::cout << "Failed to create socket" << std::endl;
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
                std::cout << "Failed to set socket option \"TCP_NODELAY\" ; " << "all your TCP packets will be buffered" << std::endl;
            }
        }
        else {
            // Enable broadcast by default for UDP sockets
            int yes = 1;
            if (setsockopt(socket_, SOL_SOCKET, SO_BROADCAST, reinterpret_cast<char*>(&yes), sizeof(yes)) == -1) {
                std::cout << "Failed to enable broadcast on UDP socket" << std::endl;
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
            std::cout << "Failed to set file status flags: " << errno << std::endl;
        }
    }
    else
    {
        if (fcntl(sock, F_SETFL, status | O_NONBLOCK) == -1) {
           std::cout << "Failed to set file status flags: " << errno << std::endl;
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
