#include <MEL/Communications/IpAddress.hpp>
#include <MEL/Communications/Socket.hpp>
#include <cstring>
#include <utility>

namespace mel {

//==============================================================================
// CLASS DEFINITIONS
//==============================================================================

const IpAddress IpAddress::None;
const IpAddress IpAddress::Any(0, 0, 0, 0);
const IpAddress IpAddress::LocalHost(127, 0, 0, 1);
const IpAddress IpAddress::Broadcast(255, 255, 255, 255);

IpAddress::IpAddress() :
    address_(0),
    is_valid_(false)
{
}

IpAddress::IpAddress(const std::string& address) :
    address_(0),
    is_valid_(false)
{
    resolve(address);
}


////////////////////////////////////////////////////////////
IpAddress::IpAddress(const char* address) :
    address_(0),
    is_valid_(false)
{
    resolve(address);
}

IpAddress::IpAddress(uint8 byte0, uint8 byte1, uint8 byte2, uint8 byte3) :
    address_(htonl((byte0 << 24) | (byte1 << 16) | (byte2 << 8) | byte3)),
    is_valid_(true)
{
}

IpAddress::IpAddress(uint32 address) :
    address_(htonl(address)),
    is_valid_(true)
{
}

std::string IpAddress::to_string() const {
    in_addr address;
    address.s_addr = address_;

    return inet_ntoa(address);
}

uint32 IpAddress::to_integer() const {
    return ntohl(address_);
}

IpAddress IpAddress::get_local_address() {
    // The method here is to connect a UDP socket to anyone (here to localhost),
    // and get the local socket address with the getsockname function.
    // UDP connection will not send anything to the network, so this function won't cause any overhead.

    IpAddress local_address;

    // Create the socket
    SocketHandle sock = socket(PF_INET, SOCK_DGRAM, 0);
    if (sock == Socket::invalid_socket())
        return local_address;

    // Connect the socket to localhost on any port
    sockaddr_in address = Socket::create_address(ntohl(INADDR_LOOPBACK), 9);
    if (connect(sock, reinterpret_cast<sockaddr*>(&address), sizeof(address)) == -1) {
        Socket::close(sock);
        return local_address;
    }

    // Get the local address of the socket connection
    Socket::AddrLength size = sizeof(address);
    if (getsockname(sock, reinterpret_cast<sockaddr*>(&address), &size) == -1) {
        Socket::close(sock);
        return local_address;
    }

    // Close the socket
    Socket::close(sock);

    // Finally build the IP address
    local_address = IpAddress(ntohl(address.sin_addr.s_addr));

    return local_address;
}


/*
IpAddress IpAddress::get_public_address(Time timeout)
{
    // The trick here is more complicated, because the only way
    // to get our public IP address is to get it from a distant computer.
    // Here we get the web page from http://www.sfml-dev.org/ip-provider.php
    // and parse the result to extract our IP address
    // (not very hard: the web page contains only our IP address).

    Http server("www.sfml-dev.org");
    Http::Request request("/ip-provider.php", Http::Request::Get);
    Http::Response page = server.sendRequest(request, timeout);
    if (page.getStatus() == Http::Response::Ok)
        return IpAddress(page.getBody());

    // Something failed: return an invalid address
    return IpAddress();
}
*/

void IpAddress::resolve(const std::string& address) {
    address_ = 0;
    is_valid_ = false;

    if (address == "255.255.255.255") {
        // The broadcast address needs to be handled explicitly,
        // because it is also the value returned by inet_addr on error
        address_ = INADDR_BROADCAST;
        is_valid_ = true;
    }
    else if (address == "0.0.0.0") {
        address_ = INADDR_ANY;
        is_valid_ = true;
    }
    else {
        // Try to convert the address as a byte representation ("xxx.xxx.xxx.xxx")
        uint32 ip = inet_addr(address.c_str());
        if (ip != INADDR_NONE) {
            address_ = ip;
            is_valid_ = true;
        }
        else {
            // Not a valid address, try to convert it as a host name
            addrinfo hints;
            std::memset(&hints, 0, sizeof(hints));
            hints.ai_family = AF_INET;
            addrinfo* result = NULL;
            if (getaddrinfo(address.c_str(), NULL, &hints, &result) == 0) {
                if (result) {
                    ip = reinterpret_cast<sockaddr_in*>(result->ai_addr)->sin_addr.s_addr;
                    freeaddrinfo(result);
                    address_ = ip;
                    is_valid_ = true;
                }
            }
        }
    }
}

//==============================================================================
// OPERATOR OVERLOADS
//==============================================================================

bool operator ==(const IpAddress& left, const IpAddress& right) {
    return !(left < right) && !(right < left);
}

bool operator !=(const IpAddress& left, const IpAddress& right) {
    return !(left == right);
}

bool operator <(const IpAddress& left, const IpAddress& right) {
    return std::make_pair(left.is_valid_, left.address_) < std::make_pair(right.is_valid_, right.address_);
}

bool operator >(const IpAddress& left, const IpAddress& right) {
    return right < left;
}

bool operator <=(const IpAddress& left, const IpAddress& right) {
    return !(right < left);
}

bool operator >=(const IpAddress& left, const IpAddress& right) {
    return !(left < right);
}

std::istream& operator >>(std::istream& stream, IpAddress& address) {
    std::string str;
    stream >> str;
    address = IpAddress(str);

    return stream;
}

std::ostream& operator <<(std::ostream& stream, const IpAddress& address) {
    return stream << address.to_string();
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
