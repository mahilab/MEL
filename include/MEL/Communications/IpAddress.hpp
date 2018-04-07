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

#ifndef MEL_IPADDRESS_HPP
#define MEL_IPADDRESS_HPP

#include <MEL/Utility/Time.hpp>
#include <MEL/Utility/Types.hpp>
#include <istream>
#include <ostream>
#include <string>

namespace mel {

//==============================================================================
// CLASS DECLARATION
//==============================================================================

/// Encapsulates an IPv4 network address
class IpAddress {
public:
    /// Default constructor. Creates an empty (invalid) address/
    IpAddress();

    /// Construct the address from a string
    ///
    /// Here \a address can be either a decimal address
    /// (ex: "192.168.1.56") or a network name (ex: "localhost").
    IpAddress(const std::string& address);

    /// Construct the address from a string
    ///
    /// Here \a address can be either a decimal address
    /// (ex: "192.168.1.56") or a network name (ex: "localhost").
    /// This is equivalent to the constructor taking a std::string
    /// parameter, it is defined for convenience so that the
    /// implicit conversions from literal strings to IpAddress work.
    IpAddress(const char* address);

    /// Construct the address from 4 bytes
    ///
    /// Calling IpAddress(a, b, c, d) is equivalent to calling
    /// IpAddress("a.b.c.d"), but safer as it doesn't have to
    /// parse a string to get the address components.
    IpAddress(uint8 byte0, uint8 byte1, uint8 byte2, uint8 byte3);

    /// Construct the address from a 32-bits integer
    ///
    /// This constructor uses the internal representation of
    /// the address directly. It should be used for optimization
    /// purposes, and only if you got that representation from
    /// IpAddress::to_integer().
    explicit IpAddress(uint32 address);

    /// Get a string representation of the address
    ///
    /// The returned string is the decimal representation of the
    /// IP address (like "192.168.1.56"), even if it was constructed
    /// from a host name.
    std::string to_string() const;

    /// Get an integer representation of the address
    ///
    /// The returned number is the internal representation of the
    /// address, and should be used for optimization purposes only
    /// (like sending the address through a socket).
    /// The integer produced by this function can then be converted
    /// back to a mel::IpAddress with the proper constructor.
    uint32 to_integer() const;

    /// Get the computer's local address
    ///
    /// The local address is the address of the computer from the
    /// LAN point of view, i.e. something like 192.168.1.56. It is
    /// meaningful only for communications over the local network.
    /// Unlike get_public_address, this function is fast and may be
    /// used safely anywhere.
    static IpAddress get_local_address();

    /// Get the computer's public address
    ///
    /// The public address is the address of the computer from the
    /// internet point of view, i.e. something like 89.54.1.169.
    /// It is necessary for communications over the world wide web.
    /// The only way to get a public address is to ask it to a
    /// distant website; as a consequence, this function depends on
    /// both your network connection and the server, and may be
    /// very slow. You should use it as few as possible. Because
    /// this function depends on the network connection and on a distant
    /// server, you may use a time limit if you don't want your program
    /// to be possibly stuck waiting in case there is a problem; this
    /// limit is deactivated by default.
    // static IpAddress get_public_address(Time timeout = Time::Zero);

    static const IpAddress
        None;  ///< Value representing an empty/invalid address
    static const IpAddress Any;  ///< Value representing any address (0.0.0.0)
    static const IpAddress LocalHost;  ///< The "localhost" address (for
                                       ///< connecting a computer to itself
                                       ///< locally)
    static const IpAddress Broadcast;  ///< The "broadcast" address (for sending
                                       ///< UDP messages to everyone on a local
                                       ///< network)

private:
    friend bool operator<(const IpAddress& left, const IpAddress& right);

    /// Resolves the given address string
    void resolve(const std::string& address);

    uint32 address_;  ///< Address stored as an unsigned 32 bits integer
    bool is_valid_;   ///< Is the address valid?
};

//==============================================================================
// OPERATOR OVERLOADS
//==============================================================================

/// Overload of == operator to compare two IP addresses are equal
bool operator==(const IpAddress& left, const IpAddress& right);

/// Overload of != operator to compare two IP addresses are not equal
bool operator!=(const IpAddress& left, const IpAddress& right);

/// Overload of < operator to compare two IP addresses
bool operator<(const IpAddress& left, const IpAddress& right);

/// Overload of > operator to compare two IP addresses
bool operator>(const IpAddress& left, const IpAddress& right);

/// Overload of <= operator to compare two IP addresses
bool operator<=(const IpAddress& left, const IpAddress& right);

/// Overload of >= operator to compare two IP addresses
bool operator>=(const IpAddress& left, const IpAddress& right);

/// Overload of >> operator to extract an IP address from an input stream
std::istream& operator>>(std::istream& stream, IpAddress& address);

/// Overload of << operator to print an IP address to an output stream
std::ostream& operator<<(std::ostream& stream, const IpAddress& address);

}  // namespace mel

#endif  // MEL_IPADDRESS_HPP

//==============================================================================
// CLASS DOCUMENTATION
//==============================================================================

/// \class mel::IpAddress
/// \ingroup network
///
/// mel::IpAddress is a utility class for manipulating network
/// addresses. It provides a set a implicit constructors and
/// conversion functions to easily build or tranmelorm an IP
/// address from/to various representations.
///
/// Usage example:
/// \code
/// mel::IpAddress a0;                                       // an invalid
/// address mel::IpAddress a1 = mel::IpAddress::None;                 // an
/// invalid address (same as a0) mel::IpAddress a2("127.0.0.1");
/// // the local host address mel::IpAddress a3 = mel::IpAddress::Broadcast;
/// // the broadcast address mel::IpAddress a4(192, 168, 1, 56);
/// // a local address mel::IpAddress a5("my_computer");
/// // a local address created from a network name mel::IpAddress
/// a6("89.54.1.169");                        // a distant address
/// mel::IpAddress a7("www.google.com");                     // a distant
/// address created from a network name mel::IpAddress a8 =
/// mel::IpAddress::get_local_address();  // my address on the local network
/// mel::IpAddress a9 = mel::IpAddress::get_public_address(); // my address on
/// the internet \endcode
///
/// Note that mel::IpAddress currently doesn't support IPv6
/// nor other types of network addresses.

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
