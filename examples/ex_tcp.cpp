#include <MEL/Communications/TcpSocket.hpp>
#include <MEL/Communications/TcpListener.hpp>
#include <MEL/Communications/Packet.hpp>
#include <MEL/Utility/Console.hpp>
#include <MEL/Utility/Clock.hpp>
#include <MEL/Utility/Options.hpp>
#include <MEL/Utility/System.hpp>
#include <MEL/Math/Functions.hpp>

// To run this example, open two terminals (same or different computers) and
// run the following:
//
// Terminal 1 (the "sever"):    tcp.exe -s
// Terminal 2 (the "client"):   tcp.exe -c -r 123.456.7.8
//
// where 123.456.7.8 is the IP address of the server (printed by Terminal 1)

using namespace mel;

void server(const IpAddress& local_address) {
    std::cout << "Starting TCP server on port 55001@" << local_address << std::endl;
    while (true) {
        // Listen for connections
        TcpListener listener;
        listener.listen(55001, local_address);
        // Accept connection
        TcpSocket socket;
        listener.accept(socket);
        std::cout << "Connected to client: " << socket.get_remote_port() << "@" << socket.get_remote_address() << std::endl;
        // Perform ping test
        std::string msg;
        Packet packet;
        while (msg != "done") {
            socket.receive(packet);
            socket.send(packet);
            packet >> msg;
        }
        sleep(seconds(1.0));
    }
}

void client(int iterations, int bytes, const IpAddress& remote_address) {
    std::cout << "Connecting to server: 55001@" << remote_address << " ... ";
    TcpSocket socket;
    socket.connect(remote_address, 55001, seconds(10));
    print("Done");
    std::cout << "Performing ping test with " << bytes << " bytes of data and " << iterations << " iterations ... ";
    // Make dummy packet of size bytes
    std::string msg(bytes, '0');
    Packet packet;
    packet << msg;
    std::vector<double> pings;
    pings.reserve(iterations);
    Clock clock;
    // Perform ping test
    for (int i = 0; i < iterations; ++i) {
        clock.restart();
        socket.send(packet);
        socket.receive(packet);
        pings.push_back(clock.get_elapsed_time().as_microseconds());
    }
    packet.clear();
    msg = "done";
    packet << msg;
    socket.send(packet);
    print("Done");
    int mean_ping = mean(pings);
    int std_ping = stddev_p(pings);
    int min_ping = min(pings);
    int max_ping = max(pings);
    print("Avg Ping: " + stringify(mean_ping) + " +/- " + stringify(std_ping) + " usec");
    print("Min Ping: " + stringify(min_ping) + " usec");
    print("Max Ping: " + stringify(max_ping) + " usec");
}

int main(int argc, char *argv[]) {

    // Setup program options
    Options options("udp.exe", "UDP Ping Test");
    options.add_options()
    ("s", "Sever Mode")
    ("c", "Client Mode")
    ("r", "Remote Server Address", value<std::string>())
    ("l", "Local Address", value<std::string>())
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

    IpAddress local_address = IpAddress::Any;
    if(input.count("l") > 0)
        local_address = IpAddress(input["l"].as<std::string>());

    // Run TCP server/client code
    if (input.count("s") > 0)
        server(local_address);
    else if (input.count("c") > 0)
        client(i, bytes, remote_address);
    else
        print("Choose server [tcp.exe -s] or client [tcp.exe -c]");

    return 0;
}
