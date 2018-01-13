#include <MEL/Communications/TcpSocket.hpp>
#include <MEL/Communications/TcpListener.hpp>
#include <MEL/Communications/Packet.hpp>
#include <MEL/Utility/Console.hpp>
#include <MEL/Utility/Clock.hpp>
#include <MEL/Utility/Options.hpp>

// To run this example, open two terminals and run the following:

// Terminal 1 (Server): tcp.exe -s
// Terminal 2 (Client): tcp.exe -c
//
// client will send "Hello" to sever, who will append ", World!"
// and send back the full string "Hello, World!" to client, where
// it will be printed.

using namespace mel;

void server(int iterations) {
    TcpListener listener;
    listener.listen(55001);
    // Wait for a connection
    TcpSocket socket;
    listener.accept(socket);
    std::cout << "Connected to client: " << socket.get_remote_port() << "@" << socket.get_remote_address() << std::endl;
    // Perform ping test
    std::string msg;
    Packet packet;
    for (int i = 0; i < iterations; ++i) {
        socket.receive(packet);
        socket.send(packet);
    }
}

void client(int iterations, int bytes, const IpAddress& remote_address) {
    std::cout << "Connecting to server: 55001@" << remote_address << std::endl;
    TcpSocket socket;
    socket.connect(remote_address, 55001, seconds(10));
    // Make dummy packet of size bytes
    std::string msg(bytes, '0');
    Packet packet;
    packet << msg;
    Clock clock;
    // Perform ping test
    for (int i = 0; i < iterations; ++i) {
        clock.restart();
        socket.send(packet);
        socket.receive(packet);
        Time t = clock.get_elapsed_time();
        print("Ping: ", false);
        print(t);
    }
}

int main(int argc, char *argv[]) {

    // Setup program options
    Options options("udp.exe", "UDP Ping Test");
    options.add_options()
        ("s", "Sever")
        ("c", "Client")
        ("r", "Remote Address", value<std::string>())
        ("i", "Ping Iterations", value<int>())
        ("b", "Message Size in Bytes", value<int>())
        ("h,help", "Help Message");
    auto input = options.parse(argc, argv);
    if (input.count("h") > 0) {
        print(options.help());
        return 0;
    }

    // Set ping packet size
    int bytes = 256;
    if (input.count("b") > 0)
        bytes = input["b"].as<int>();

    // Set iterations
    int i = 100;
    if (input.count("i") > 0)
        i = input["i"].as<int>();

    // Set remote IP address
    IpAddress remote_address = IpAddress::get_local_address();
    if (input.count("r") > 0)
        remote_address = IpAddress(input["r"].as<std::string>());

    // Run TCP server/client code
    if (input.count("s") > 0)
        server(i);
    else if (input.count("c") > 0)
        client(i, bytes, remote_address);
    else
        print("Choose server [tcp.exe -s] or client [tcp.exe -c]");

    return 0;
}
