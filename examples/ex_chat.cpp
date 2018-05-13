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
#ifdef _WIN32
#include <conio.h>
#else
#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#endif
#include <cstdio>
#include <thread>
#include <MEL/Utility/Mutex.hpp>

#ifndef _WIN32
int _getch( ) {
    struct termios oldt,
    newt;
    int ch;
    tcgetattr( STDIN_FILENO, &oldt );
    newt = oldt;
    newt.c_lflag &= ~( ICANON | ECHO );
    tcsetattr( STDIN_FILENO, TCSANOW, &newt );
    ch = getchar();
    tcsetattr( STDIN_FILENO, TCSANOW, &oldt );
    return ch;
}
#endif

using namespace mel;

ctrl_bool stop(false);
bool handler(CtrlEvent event);

//==============================================================================
// SERVER
//==============================================================================
class Server {
public:
    Server();
    ~Server();
    void run();
private:
    void broadcast();
private:
    UdpSocket udp;
    TcpListener listener;
    SocketSelector selector;
    std::list<TcpSocket*> clients;
};

//==============================================================================
// CLIENT
//==============================================================================
class Client {
public:
    Client(const std::string& username);
    ~Client();
    bool connect();
    void run();
private:
    void clear_line();
    void resume_line();
    void helper_thread_func();
private:
    std::string username;
    std::string message;
    TcpSocket tcp;
    Mutex mutex;
    bool connected;
};

//==============================================================================
// MAIN
//==============================================================================
int main(int argc, char *argv[]) {
    // Set up command line options
	Options options("chat.exe", "MEL Chat");
	options.add_options()
		("s,server","Starts MEL Chat in Server Mode")
		("u,user",  "Starts MEL Chat in User Mode and sets username", value<std::string>());
	auto input = options.parse(argc, argv);

    init_logger();
    register_ctrl_handler(handler);

    if (input.count("server")){
        Server server;
        server.run();
    }
    else if (input.count("user")) {
        Client client(input["user"].as<std::string>());
        if (client.connect())
            client.run();
    }
    else {
        print(options.help());
    }

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
// SERVER IMPL
//==============================================================================

Server::Server() {
    udp.bind(55001);
    listener.listen(55003);
    selector.add(listener);
}

Server::~Server() {
    LOG(Info) << "Stopping Chat Sever at " << IpAddress::get_local_address();
    for (auto it = clients.begin(); it != clients.end(); ++it) {
        TcpSocket& client = **it;
        selector.remove(client);
        client.disconnect();
        delete *it;
    }
}

void Server::run() {
    LOG(Info) << "Starting Chat Sever at " << IpAddress::get_local_address();
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
        sleep(milliseconds(10));
        broadcast();
    }
}

void Server::broadcast() {
    Packet empty_packet;
    udp.send(empty_packet, IpAddress::Broadcast, 55002);
}

//==============================================================================
// CLIENT IMPL
//==============================================================================

Client::Client(const std::string& username) :
    username(username)
{
    LOG(Info) << "Starting Chat as User \"" << username << "\"";
}

Client::~Client() {
    if (stop) {
        LOG(Info) << "Disconnecting from Chat Sever at "
                  << tcp.get_remote_address() << "@" << tcp.get_remote_port();
        tcp.disconnect();
    }
}

bool Client::connect() {
    UdpSocket udp;
    if (udp.bind(55002) == Socket::Error)
        return (connected = false);
    IpAddress server_address;
    unsigned short server_port;
    Packet packet;
    udp.receive(packet, server_address, server_port);
    LOG(Info) << "Found Chat Sever at " << server_address.to_string();
    udp.unbind();
    tcp.connect(server_address, 55003);
    LOG(Info) << "Connected to Chat Sever at " << tcp.get_remote_address()
              << "@" << tcp.get_remote_port();
    tcp.set_blocking(false);
    return (connected = true);
}

void Client::run() {
    resume_line();
    // start helper thread
    std::thread helper_thread( [this] { this->helper_thread_func(); });
    // messaging loop
    Packet outgoing_packet;
    while(connected && !stop) {
        // process user input
        char ch = _getch();
        {
            Lock lock(mutex);
            if (!connected)
                break;
            if (ch == '\n') {
                // Ctrl+Enter pressed, exit
                std::cout << "\n";
                stop = true;
            }
            else if (ch == '\r') {
                // Enter pressed, send message
                if (message.length() > 0) {
                    std::cout << "\n";
                    outgoing_packet.clear();
                    outgoing_packet << username << message;
                    tcp.send(outgoing_packet);
                    message = "";
                    resume_line();
                }
            }
            else if (ch == '\b') {
                // Backspace pressed, remove character
                if (message.length() > 0) {
                    message.pop_back();
                    std::cout << ch << " \b";
                }
            }
            else {
                // Add character to message
                message += ch;
                std::cout << ch;
            }
        }
    }

    // joint helper thread
    helper_thread.join();
}

void Client::clear_line() {
    for (int i = 0; i < 100; ++i)
        std::cout << "\b \b";
}

void Client::resume_line() {
    set_text_color(Color::Green);
    std::cout << username << ": ";
    set_text_color(Color::None);
    std::cout << message;
}

void Client::helper_thread_func() {
    bool cleared = false;
    std::string other_username, other_message;
    Packet incomming_packet;
    while (connected && !stop) {
        // receive message packets
        {
            Lock lock(mutex);
            while (tcp.receive(incomming_packet) == Socket::Done) {
                incomming_packet >> other_username >> other_message;
                if (other_username != username) {
                    // clear line to print messages
                    if (!cleared) {
                        clear_line();
                        cleared = true;
                    }
                    // print messages
                    set_text_color(Color::Cyan);
                    std::cout << other_username << ": ";
                    set_text_color(Color::None);
                    std::cout << other_message << std::endl;
                }
            }
            // check for disconnection
            if (tcp.receive(incomming_packet) == Socket::Disconnected) {
                std::cout << "\n";
                LOG(Error) << "Lost connection to Chat Sever at "
                           << tcp.get_remote_address() << "@"
                           << tcp.get_remote_port();
                connected = false;
            }
            // if cleared, resume line
            if (cleared)
                resume_line();
            cleared = false;
        }
        sleep(milliseconds(10));
    }
}
