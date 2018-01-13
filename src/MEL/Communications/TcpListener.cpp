#include <MEL/Communications/TcpListener.hpp>
#include <MEL/Communications/TcpSocket.hpp>
#include <iostream>

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
        std::cout << "Failed to bind listener socket to port " << port << std::endl;
        return Error;
    }

    // Listen to the bound port
    if (::listen(get_handle(), 0) == -1)
    {
        // Oops, socket is deaf
        std::cout << "Failed to listen to port " << port << std::endl;
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
        std::cout << "Failed to accept a new connection, the socket is not listening" << std::endl;
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

