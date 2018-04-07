#include <MEL/Communications/UdpSocket.hpp>
#include <MEL/Communications/IpAddress.hpp>
#include <MEL/Communications/Packet.hpp>
#include <MEL/Utility/Console.hpp>
#include <MEL/Core/Clock.hpp>
#include <MEL/Utility/Options.hpp>
#include <MEL/Utility/System.hpp>
#include <MEL/Math/Functions.hpp>

// To run this example, open two terminals (same or different computers) and
// run the following:
//
// Terminal 1 (the "sever"):    udp.exe -s
// Terminal 2 (the "client"):   udp.exe -c -r 123.456.7.8
//
// where 123.456.7.8 is the IP address of the server (printed by Terminal 1)

using namespace mel;

void server() {
    std::cout << "Starting UDP server on port 55002@" << IpAddress::get_local_address() << std::endl;
    while(true) {
        // Create a socket and bind it to the port 55002
        UdpSocket socket;
        socket.bind(55002);
        // Receive a message from anyone
        IpAddress sender;
        unsigned short port;
        // Perform ping test
        std::string msg;
        Packet packet;
        socket.receive(packet, sender, port);
        packet >> msg;
        if (msg != "start")
            break;
        std::cout << "Start request received from client: " << port << "@" << sender << std::endl;
        while (msg != "done") {
            socket.receive(packet, sender, port);
            socket.send(packet, sender, port);
            packet >> msg;
        }
        sleep(seconds(1.0));
    }
}

void client(int iterations, int bytes, const IpAddress& remote_address) {
    std::cout << "Sending start request server: 55002@" << remote_address << std::endl;
    // Create a socket and bind it to any port
    UdpSocket socket;
    socket.bind(Socket::AnyPort);
    // Receive a message from anyone
    IpAddress sender;
    unsigned short port;
    // Send start message
    std::string msg = "start";
    Packet packet;
    packet << msg;
    socket.send(packet, remote_address, 55002);
    std::cout << "Performing ping test with " << bytes << " bytes of data and " << iterations << " iterations ... ";
    // Make dummy packet of size bytes
    packet.clear();
    msg = std::string(bytes, '0');
    packet << msg;
    std::vector<double> pings;
    pings.reserve(iterations);
    Clock clock;
    // Perform ping test
    for (int i = 0; i < iterations; ++i) {
        clock.restart();
        socket.send(packet, remote_address, 55002);
        socket.receive(packet, sender, port);
        pings.push_back((double)clock.get_elapsed_time().as_microseconds());
    }
    packet.clear();
    msg = "done";
    packet << msg;
    socket.send(packet, remote_address, 55002);
    print("Done");
    // Print results
    int mean_ping = (int)mean(pings);
    int std_ping = (int)stddev_p(pings);
    int min_ping = (int)min(pings);
    int max_ping = (int)max(pings);
    print("Avg Ping: " + stringify(mean_ping) + " +/- " + stringify(std_ping) + " usec");
    print("Min Ping: " + stringify(min_ping) + " usec");
    print("Max Ping: " + stringify(max_ping) + " usec");

}

int main(int argc, char *argv[]) {

    // Setup program options
    Options options("udp.exe", "TCP/IP Ping Test");
    options.add_options()
    ("s", "Sever Mode")
    ("c", "Client Mode")
    ("r", "Remote Server Address", value<std::string>())
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
    int i = 1000;
    if (input.count("i") > 0)
        i = input["i"].as<int>();

    // Set remote IP address
    IpAddress remote_address = IpAddress::get_local_address();
    if (input.count("r") > 0)
        remote_address = IpAddress(input["r"].as<std::string>());

    // Run TCP server/client code
    if (input.count("s") > 0)
        server();
    else if (input.count("c") > 0)
        client(i, bytes, remote_address);
    else
        print("Choose server [tcp.exe -s] or client [tcp.exe -c]");

    return 0;
}
