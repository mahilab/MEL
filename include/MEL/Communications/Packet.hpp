// MIT License
//
// MEL - Mechatronics Library
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

#ifndef MEL_PACKET_HPP
#define MEL_PACKET_HPP

#include <MEL/Utility/Types.hpp>
#include <string>
#include <vector>

namespace mel {

//==============================================================================
// FORARD DECLARATIONS
//==============================================================================

class TcpSocket;
class UdpSocket;
class SharedMemory;

//==============================================================================
// CLASS DECLARATION
//==============================================================================

/// Utility class to build blocks of data to transfer over the network
class Packet {
    // A bool-like type that cannot be converted to integer or pointer types
    typedef bool (Packet::*BoolType)(std::size_t);

public:
    /// Default constructor. Creates an empty packet.
    Packet();

    /// Virtual destructor
    virtual ~Packet();

    /// Append data to the end of the packet
    ///
    /// \param data        Pointer to the sequence of bytes to append
    /// \param send_in_bytes Number of bytes to append
    void append(const void* data, std::size_t send_in_bytes);

    /// Clear the packet. After calling Clear, the packet is empty.
    void clear();

    /// Get a pointer to the data contained in the packet
    ///
    /// Warning: the returned pointer may become invalid after
    /// you append data to the packet, therefore it should never
    /// be stored.
    /// The return pointer is NULL if the packet is empty.
    const void* get_data() const;

    /// Get the size of the data contained in the packet. Returns the number of
    //  bytes pointed to by what get_data returns.
    std::size_t get_data_size() const;

    /// Tell if the reading position has reached the end of the packet. True if
    /// all data was read, false otherwise
    bool end_of_packet() const;

public:
    ////////////////////////////////////////////////////////////
    /// Test the validity of the packet, for reading
    ///
    /// This operator allows to test the packet as a boolean
    /// variable, to check if a reading operation was successfully.
    ///
    /// A packet will be in an invalid state if it has no more
    /// data to read.
    ///
    /// This behavior is the same as standard C++ streams.
    ///
    /// Usage example:
    /// \code
    /// float x;
    /// packet >> x;
    /// if (packet)
    /// {
    ///    // ok, x was extracted successfully
    /// }
    ///
    /// // -- or --
    ///
    /// float x;
    /// if (packet >> x)
    /// {
    ///    // ok, x was extracted successfullyly
    /// }
    /// \endcode
    ///
    /// Don't focus on the return type, it's equivalent to bool but
    /// it disallows unwanted implicit conversions to integer or
    /// pointer types.
    ///
    /// \return True if last data extraction from packet was successfully
    ///
    /// \see end_of_packet
    ///
    ////////////////////////////////////////////////////////////
    operator BoolType() const;

    /// Overloads of operator >> to read data from the packet
    Packet& operator>>(bool& data);
    Packet& operator>>(int8& data);
    Packet& operator>>(uint8& data);
    Packet& operator>>(int16& data);
    Packet& operator>>(uint16& data);
    Packet& operator>>(int32& data);
    Packet& operator>>(uint32& data);
    Packet& operator>>(int64& data);
    Packet& operator>>(uint64& data);
    Packet& operator>>(float& data);
    Packet& operator>>(double& data);
    Packet& operator>>(char* data);
    Packet& operator>>(std::string& data);
    Packet& operator>>(wchar_t* data);
    Packet& operator>>(std::wstring& data);
    template <typename T>  Packet& operator>>(std::vector<T>& data);

    /// Overloads of operator << to write data into the packet
    Packet& operator<<(bool data);
    Packet& operator<<(int8 data);
    Packet& operator<<(uint8 data);
    Packet& operator<<(int16 data);
    Packet& operator<<(uint16 data);
    Packet& operator<<(int32 data);
    Packet& operator<<(uint32 data);
    Packet& operator<<(int64 data);
    Packet& operator<<(uint64 data);
    Packet& operator<<(float data);
    Packet& operator<<(double data);
    Packet& operator<<(const char* data);
    Packet& operator<<(const std::string& data);
    Packet& operator<<(const wchar_t* data);
    Packet& operator<<(const std::wstring& data);
    template <typename T> Packet& operator<<(const std::vector<T>& data);

protected:
    friend class TcpSocket;
    friend class UdpSocket;
    friend class SharedMemory;

    /// Called before the packet is sent over the network
    ///
    /// This function can be defined by derived classes to
    /// tranform the data before it is sent; this can be
    /// used for compression, encryption, etc.
    /// The function must return a pointer to the modified data,
    /// as well as the number of bytes pointed.
    /// The default implementation provides the packet's data
    /// without tranmelorming it.
    ///
    /// \param size Variable to fill with the size of data to send
    ///
    /// \return Pointer to the array of bytes to send
    virtual const void* on_send(std::size_t& size);

    /// Called after the packet is received over the network
    ///
    /// This function can be defined by derived classes to
    /// tranmelorm the data after it is received; this can be
    /// used for decompression, decryption, etc.
    /// The function receives a pointer to the received data,
    /// and must fill the packet with the tranmelormed bytes.
    /// The default implementation fills the packet directly
    /// without tranmelorming the data.
    ///
    /// \param data Pointer to the received bytes
    /// \param size Number of bytes
    virtual void on_receive(const void* data, std::size_t size);

private:
    /// Disallow comparisons between packets
    bool operator==(const Packet& right) const;
    bool operator!=(const Packet& right) const;

    /// Check if the packet can extract a given number of bytes This function
    /// updates accordingly the state of the packet.
    bool check_size(std::size_t size);

    std::vector<char> data_;  ///< Data stored in the packet
    std::size_t read_pos_;    ///< Current reading position in the packet
    std::size_t send_pos_;    ///< Current send position in the packet (for
                              ///< handling partial sends)
    bool is_valid_;           ///< Reading state of the packet
};

template <typename T>
Packet& Packet::operator>>(std::vector<T>& data) {
    uint32 length = 0;
    *this >> length;
    if (length > 0) { // should call check_size, but cannot with templates
        data.resize(static_cast<std::size_t>(length));
        for (std::size_t i = 0; i < data.size(); ++i) {
            T value;
            *this >> value;
            data[i] = value;
        }
    }
    return *this;
}

template <typename T>
Packet& Packet::operator<<(const std::vector<T>& data) {
    uint32 length = static_cast<uint32>(data.size());
    *this << length;
    if (length > 0) {
        for (std::size_t i = 0; i < data.size(); ++i)
            *this << data[i];
    }
    return *this;
}

}  // namespace mel

#endif  // MEL_PACKET_HPP

//==============================================================================
// CLASS DOCUMENTATION
//==============================================================================

/// \class mel::Packet
/// \ingroup network
///
/// Packets provide a safe and easy way to serialize data,
/// in order to send it over the network using sockets
/// (mel::TcpSocket, mel::UdpSocket).
///
/// Packets solve 2 fundamental problems that arise when
/// tranmelerring data over the network:
/// \li data is interpreted correctly according to the endianness
/// \li the bounds of the packet are preserved (one send == one receive)
///
/// The mel::Packet class provides both input and output modes.
/// It is designed to follow the behavior of standard C++ streams,
/// using operators >> and << to extract and insert data.
///
/// It is recommended to use only fixed-size types (like mel::int32, etc.),
/// to avoid possible differences between the sender and the receiver.
/// Indeed, the native C++ types may have different sizes on two platforms
/// and your data may be corrupted if that happens.
///
/// Usage example:
/// \code
/// mel::uint32 x = 24;
/// std::string s = "hello";
/// double d = 5.89;
///
/// // Group the variables to send into a packet
/// mel::Packet packet;
/// packet << x << s << d;
///
/// // Send it over the network (socket is a valid mel::TcpSocket)
/// socket.send(packet);
///
/// ----------------------------------------------------------------------------
///
/// // Receive the packet at the other end
/// mel::Packet packet;
/// socket.receive(packet);
///
/// // Extract the variables contained in the packet
/// mel::uint32 x;
/// std::string s;
/// double d;
/// if (packet >> x >> s >> d)
/// {
///     // Data extracted successfullyly...
/// }
/// \endcode
///
/// Packets have built-in operator >> and << overloads for
/// standard types:
/// \li bool
/// \li fixed-size integer types (mel::int8/16/32, mel::uint8/16/32)
/// \li floating point numbers (float, double)
/// \li string types (char*, wchar_t*, std::string, std::wstring, mel::String)
///
/// Like standard streams, it is also possible to define your own
/// overloads of operators >> and << in order to handle your
/// custom types.
///
/// \code
/// struct MyStruct
/// {
///     float       number;
///     mel::int8    integer;
///     std::string str;
/// };
///
/// mel::Packet& operator <<(mel::Packet& packet, const MyStruct& m)
/// {
///     return packet << m.number << m.integer << m.str;
/// }
///
/// mel::Packet& operator >>(mel::Packet& packet, MyStruct& m)
/// {
///     return packet >> m.number >> m.integer >> m.str;
/// }
/// \endcode
///
/// Packets also provide an extra feature that allows to apply
/// custom tranmelormations to the data before it is sent,
/// and after it is received. This is typically used to
/// handle automatic compression or encryption of the data.
/// This is achieved by inheriting from mel::Packet, and overriding
/// the on_send and on_receive functions.
///
/// Here is an example:
/// \code
/// class ZipPacket : public mel::Packet
/// {
///     virtual const void* on_send(std::size_t& size)
///     {
///         const void* srcData = get_data();
///         std::size_t srcSize = get_data_size();
///
///         return MySuperZipFunction(srcData, srcSize, &size);
///     }
///
///     virtual void on_receive(const void* data, std::size_t size)
///     {
///         std::size_t dstSize;
///         const void* dstData = MySuperUnzipFunction(data, size, &dstSize);
///
///         append(dstData, dstSize);
///     }
/// };
///
/// // Use like regular packets:
/// ZipPacket packet;
/// packet << x << s << d;
/// ...
/// \endcode
///
/// \see mel::TcpSocket, mel::UdpSocket

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
