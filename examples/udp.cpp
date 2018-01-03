#include <string>
#include <MEL/Communications/UdpSocket.hpp>
#include <MEL/Communications/IpAddress.hpp>
#include <MEL/Communications/Packet.hpp>
#include <MEL/Utility/Console.hpp>

// To run this example, open two terminals are run the following:
// Terminal 1: udp.exe server
// Terminal 2: udp.exe client
//
// client will send "Hello" to sever, who will append ", World!"
// and send back the full string "Hello, World!" to client, where
// it will be printed.

using namespace mel;

void server() {
    // Create a socket and bind it to the port 55002
    UdpSocket socket;
    socket.bind(55002);
    // Receive a message from anyone
    IpAddress sender;
    unsigned short port;
    // Make a Packet to receive data
    std::string s;
    Packet packet;
    // Receive Packet from whoever
    socket.receive(packet, sender, port);
    // Print Packet
    packet >> s;
    // Clear Packet
    packet.clear();
    // Repack Packet
    s += ", World!";
    packet << s;
    socket.send(packet, sender, 55001);
}

void client() {
    // Create a socket and bind it to the port 55001
    UdpSocket socket;
    socket.bind(55001);
    // Create Packet and pack it
    std::string s = "Hello";
    Packet packet;
    packet << s;
    // Send Packet
    socket.send(packet, IpAddress("169.254.67.6"), 55002);
    // Recieve Packet from whoever
    IpAddress sender;
    unsigned short port;
    socket.receive(packet, sender, port);
    // Print Packet
    packet >> s;
    print(s);
}

int main(int argc, char *argv[]) {
    if (argc > 1) {
        std::string input = argv[1];
        if (input == "server")
            server();
        else if (input == "client")
            client();
    }
    return 0;
}
