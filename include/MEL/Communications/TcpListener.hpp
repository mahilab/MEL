#pragma once

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
