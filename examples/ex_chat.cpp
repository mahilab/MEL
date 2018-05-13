#include <MEL/Communications/Packet.hpp>
#include <MEL/Communications/SocketSelector.hpp>
#include <MEL/Communications/TcpListener.hpp>
#include <MEL/Communications/TcpSocket.hpp>
#include <MEL/Communications/UdpSocket.hpp>
#include <MEL/Logging/Log.hpp>
#include <MEL/Utility/Console.hpp>
#include <MEL/Utility/Options.hpp>
#include <string>
#include <list>

using namespace mel;

ctrl_bool stop(false);
bool handler(CtrlEvent event);
void server();
void client(const std::string& username);

//==============================================================================
// MAIN
//==============================================================================
int main(int argc, char *argv[])
{
    // Set up command line options
	Options options("chat.exe", "MEL Chat");
	options.add_options()
		("s,server","Starts MEL Chat in Server Mode")
		("u,user",  "Starts MEL Chat in User Mode and sets username", value<std::string>());
	auto input = options.parse(argc, argv);

    init_logger();
    register_ctrl_handler(handler);

    if (input.count("server"))
        server();
    else if (input.count("user"))
        client(input["user"].as<std::string>());
    else
        print(options.help());

    return 0;
}

//==============================================================================
// CONTROL-C HANDLER
//==============================================================================
bool handler(CtrlEvent event) {
    if (event == CtrlEvent::CtrlC)
        stop = true;
    return true;
}

//==============================================================================
// SERVER
//==============================================================================
void server() {
    LOG(Info) << "Starting Chat Sever at " << IpAddress::get_local_address();
    // UDP socket for broadcasting IP address to clients (users)
    UdpSocket udp;
    udp.bind(55001);
    Packet empty_packet;
    // setup TCP network
    TcpListener listener;
    listener.listen(55003);
    std::list<TcpSocket*> clients;
    SocketSelector selector;
    selector.add(listener);
    // packets
    std::vector<Packet> prev_packets, next_packets;
    // run the server
    while (!stop) {
        if (selector.wait(seconds(1))) {
            // check for connecting clients
            if (selector.is_ready(listener)) {
                TcpSocket* client = new TcpSocket;
                if (listener.accept(*client) == Socket::Done) {
                    clients.push_back(client);
                    selector.add(*client);
                    LOG(Info) << "Connected to User at "
                              << client->get_remote_address()
                              << "@" << client->get_remote_port();

                }
                else {
                    delete client;
                }
            }
            else {
                // receive packets from clients and check for disconnections
                std::vector<Packet> packets;
                for (auto it = clients.begin(); it != clients.end(); ++it) {
                    TcpSocket& client = **it;
                    if (selector.is_ready(client)) {
                        Packet packet;
                        if (client.receive(packet) == Socket::Done) {
                            packets.push_back(packet);
                        }
                        else if (client.receive(packet) == Socket::Disconnected) {
                            LOG(Info) << "Disconnected from User at "
                                      << client.get_remote_address()
                                      << "@" << client.get_remote_port();
                            selector.remove(client);
                            delete *it;
                            it = clients.erase(it);
                        }
                    }

                }
                // process packets
                for (std::size_t i = 0; i < packets.size(); ++i) {
                    std::string username, message;
                    packets[i] >> username >> message;
                    LOG(Info) << username << ": " << message;
                    for (auto it = clients.begin(); it != clients.end(); ++it) {
                        TcpSocket& client = **it;
                        client.send(packets[i]);
                    }
                }

            }
        }
        sleep(milliseconds(100));
        // broadcast over UDP so Users can get Sever address
        udp.send(empty_packet, IpAddress::Broadcast, 55002);
    }

    // cleanup
    LOG(Info) << "Stopping Chat Sever at " << IpAddress::get_local_address();
    for (auto it = clients.begin(); it != clients.end(); ++it) {
        TcpSocket& client = **it;
        selector.remove(client);
        client.disconnect();
        delete *it;
    }
}

//==============================================================================
// CLIENT
//==============================================================================
void client(const std::string& username) {
    LOG(Info) << "Starting Chat as User \"" << username << "\"";
    // listen for Chat Sever UDP broadcast port 55002 so we can get its address
    LOG(Info) << "Searching for Chat Server";
    UdpSocket udp;
    if (udp.bind(55002) == Socket::Error)
        return;
    IpAddress server_address;
    unsigned short server_port;
    Packet empty_packet;
    udp.receive(empty_packet, server_address, server_port);
    LOG(Info) << "Found Chat Sever at " << server_address.to_string();
    udp.unbind();
    // connect to Chat Server over TCP
    TcpSocket tcp;
    tcp.connect(server_address, 55003);
    LOG(Info) << "Connected to Chat Sever at " << tcp.get_remote_address()
              << "@" << tcp.get_remote_port();
    tcp.set_blocking(false);
    Packet packet;
    std::string message, other_username;
    while(!stop) {
        set_text_color(Color::Green);
        std::cout << username << ": ";
        set_text_color(Color::None);
        std::getline(std::cin, message);
        if (message != "") {
            // send packet
            packet.clear();
            packet << username << message;
            tcp.send(packet);
        }
        // receive packets
        while (tcp.receive(packet) == Socket::Done) {
            packet >> other_username >> message;
            if (other_username != username) {
                set_text_color(Color::Cyan);
                std::cout << other_username << ": ";
                set_text_color(Color::None);
                std::cout << message << std::endl;
            }
        }
        // check for disconnection
        if (tcp.receive(packet) == Socket::Disconnected) {
            LOG(Error) << "Lost connection to Chat Sever at "
                       << tcp.get_remote_address() << "@"
                       << tcp.get_remote_port();
            break;
        }

    }
    // disconnect from Chat Server if stopped
    if (stop) {
        LOG(Info) << "Disconnecting from Chat Sever at " << tcp.get_remote_address() << "@" << tcp.get_remote_port();
        tcp.disconnect();
    }
}
