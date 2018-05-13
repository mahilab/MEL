#include <MEL/Communications/Packet.hpp>
#include <MEL/Communications/SocketSelector.hpp>
#include <MEL/Communications/TcpListener.hpp>
#include <MEL/Communications/TcpSocket.hpp>
#include <MEL/Communications/UdpSocket.hpp>
#include <MEL/Logging/Log.hpp>
#include <MEL/Utility/Console.hpp>
#include <MEL/Utility/Options.hpp>
#include <map>
#include <string>
#ifdef _WIN32
#include <conio.h>
#else
#include <fcntl.h>
#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#endif
#include <MEL/Utility/Mutex.hpp>
#include <cstdio>
#include <thread>
#include <iomanip>

#ifndef _WIN32
int _getch() {
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

// Socket Ports
unsigned short SERVER_UDP = 55001;
unsigned short CLIENT_UDP = 55002;
unsigned short SERVER_TCP = 55003;

std::size_t MAX_USERNAME_LENGTH = 16;
std::size_t MAX_MESSAGE_LENGTH = 64;

// CTRL-C Handler
ctrl_bool STOP(false);
bool handler(CtrlEvent event);

//==============================================================================
// SERVER
//==============================================================================

class Server {
public:
    Server();
    ~Server();
    bool init();
    void run();
private:
    void connect_client();
    void disconnect_client(const std::string& name);
    void broadcast_address();
    void broadcast_message(const std::string& username, const std::string& message);
    void process_packets(std::vector<Packet>& packets);
    void process_disconnections(std::vector<std::string>& disconnections);
private:
    UdpSocket udp;
    TcpListener listener;
    SocketSelector selector;
    std::map<std::string, TcpSocket*> clients;
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
    void print_border();
    void clear_input();
    void resume_input();
    void message_thread_func();
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
        if (server.init())
            server.run();
    }
    else if (input.count("user")) {
        Client client(input["user"].as<std::string>());
        if (client.connect())
            client.run();
    }
    else {
        print(options.help());
        print("  Example: ./chat.exe -u epezent\n");
        print(IpAddress::get_public_address());
    }

    return 0;
}

//==============================================================================
// CONTROL-C HANDLER
//==============================================================================

bool handler(CtrlEvent event) {
    if (event == CtrlEvent::CtrlC)
        STOP = true;
    return true;
}

//==============================================================================
// SERVER IMPLEMENTATION
//==============================================================================

Server::Server() { }

bool Server::init() {
    if (udp.bind(SERVER_UDP) == Socket::Error)
        return false;

    listener.listen(SERVER_TCP);
    selector.add(listener);
    return true;
}

Server::~Server() {
    LOG(Info) << "Stopping Server at " << IpAddress::get_local_address();
    for (auto it = clients.begin(); it != clients.end(); ++it) {
        TcpSocket* client = it->second;
        selector.remove(*client);
        client->disconnect();
        delete client;
    }
}

void Server::run() {
    LOG(Info) << "Starting Server at " << IpAddress::get_local_address();
    while (!STOP) {
        if (selector.wait(seconds(1))) {
            // check for connections
            if (selector.is_ready(listener))
                connect_client();
            else {
                // check for packets and disconnections
                std::vector<Packet> packets;
                std::vector<std::string> disconnections;
                for (auto it = clients.begin(); it != clients.end(); ++it) {
                    TcpSocket& client = *(it->second);
                    if (selector.is_ready(client)) {
                        Packet packet;
                        if (client.receive(packet) == Socket::Done)
                            packets.push_back(packet);
                        else if (client.receive(packet) == Socket::Disconnected)
                            disconnections.push_back(it->first);
                    }
                }
                process_disconnections(disconnections);
                process_packets(packets);
            }
        }
        sleep(milliseconds(10));
        broadcast_address();
    }
}

void Server::connect_client() {
    TcpSocket* client = new TcpSocket;
    if (listener.accept(*client) == Socket::Done) {
        Packet packet;
        std::string username;
        client->receive(packet);
        packet >> username;
        packet.clear();
        if (clients.count(username) == 0 && username != "Server") {
            broadcast_message("Server", ("\"" + username + "\" joined the Chat"));
            clients[username] = client;
            selector.add(*client);
            packet << "OK";
            client->send(packet);
            LOG(Info) << "Connected to \"" << username << "\" at "
                      << client->get_remote_address()
                      << "@" << client->get_remote_port();
        }
        else {
            packet << ("Username \"" + username + "\" already taken");
            client->send(packet);
            LOG(Info) << "Rejected duplicate \"" << username << "\" at "
                      << client->get_remote_address()
                      << "@" << client->get_remote_port();
            delete client;
        }
    }
    else
        delete client;
}

void Server::disconnect_client(const std::string& username) {
    LOG(Info) << "Disconnected from \"" << username << "\" at "
              << clients[username]->get_remote_address()
              << "@" << clients[username]->get_remote_port();
    selector.remove(*clients[username]);
    delete clients[username];
    clients.erase(username);
}

void Server::broadcast_address() {
    Packet empty_packet;
    udp.send(empty_packet, IpAddress::Broadcast, 55002);
}

void Server::broadcast_message(const std::string& username, const std::string& message) {
    Packet packet;
    packet << username << message;
    for (auto it = clients.begin(); it != clients.end(); ++it) {
        TcpSocket& client = *(it->second);
        client.send(packet);
    }
}

void Server::process_packets(std::vector<Packet>& packets) {
    for (std::size_t i = 0; i < packets.size(); ++i) {
        std::string username, message;
        packets[i] >> username >> message;
        LOG(Info) << username << ": " << message;
        broadcast_message(username, message);
    }
    packets.clear();
}

void Server::process_disconnections(std::vector<std::string>& disconnections) {
    for (std::size_t i = 0; i < disconnections.size(); ++i)
        disconnect_client(disconnections[i]);
    for (std::size_t i = 0; i < disconnections.size(); ++i) {
        broadcast_message("Server", ("\"" + disconnections[i] + "\" left the Chat"));
    }
}

//==============================================================================
// CLIENT IMPLEMENTATION
//==============================================================================

Client::Client(const std::string& username) :
    username(username)
{
    message.reserve(MAX_MESSAGE_LENGTH);
}

Client::~Client() {
    if (connected) {
        LOG(Info) << "Disconnecting from Server at "
                  << tcp.get_remote_address() << "@"
                  << tcp.get_remote_port();
        tcp.disconnect();
    }
}

bool Client::connect() {
    if (username.length() > MAX_USERNAME_LENGTH) {
        LOG(Warning) << "Username must be " << MAX_USERNAME_LENGTH
                     << " or fewer characters long";
        return (connected = false);
    }
    LOG(Info) << "Joining Chat as \"" << username << "\"";
    UdpSocket udp;
    if (udp.bind(CLIENT_UDP) == Socket::Error)
        return (connected = false);
    IpAddress server_address;
    unsigned short server_port;
    Packet packet;
    udp.receive(packet, server_address, server_port);
    LOG(Info) << "Found Server at " << server_address.to_string();
    udp.unbind();
    tcp.connect(server_address, SERVER_TCP);
    LOG(Info) << "Connected to Server at " << tcp.get_remote_address()
              << "@" << tcp.get_remote_port();
    // register username
    packet.clear();
    packet << username;
    tcp.send(packet);
    std::string response;
    tcp.receive(packet);
    packet >> response;
    if (response != "OK") {
        LOG(Warning) << response;
        return (connected = false);
    }
    tcp.set_blocking(false);
    return (connected = true);
}

void Client::run() {
    print_border();
    resume_input();
    // start helper thread
    std::thread message_thread( [this] { this->message_thread_func(); });
    // messaging loop
    Packet outgoing_packet;
    while(connected && !STOP) {
        // process user input
        char ch = (char)_getch();
        {
            Lock lock(mutex);
            if (!connected)
                break;
            if (ch == '\n') {
                // Ctrl+Enter pressed, exit
                clear_input();
                STOP = true;
            }
            else if (ch == '\r') {
                // Enter pressed, send message
                if (message.length() > 0) {
                    std::cout << "\n";
                    outgoing_packet.clear();
                    outgoing_packet << username << message;
                    tcp.send(outgoing_packet);
                    message = "";
                    resume_input();
                }
            }
            else if (ch == '\b') {
                // Backspace pressed, remove character
                if (message.length() > 0) {
                    message.pop_back();
                    std::cout << "\b \b";
                }
            }
            else if (message.length() < MAX_MESSAGE_LENGTH) {
                // Add character to message
                message += ch;
                std::cout << ch;
            }
        }
    }
    message_thread.join();
    print_border();
}

void Client::clear_input() {
    std::size_t n = username.length() + message.length() + 2;
    for (std::size_t i = 0; i < n; ++i)
        std::cout << "\b \b";
}

void Client::resume_input() {
    set_text_color(Color::Green);
    std::cout << username << ": ";
    set_text_color(Color::None);
    std::cout << message;
}

void Client::print_border() {
    for (int i = 0; i < 80; ++i)
        std::cout << "-";
    std::cout << std::endl;
}

void Client::message_thread_func() {
    bool cleared = false;
    std::string other_username, other_message;
    Packet packet;
    while (connected && !STOP) {
        // receive message packets
        {
            Lock lock(mutex);
            while (tcp.receive(packet) == Socket::Done) {
                packet >> other_username >> other_message;
                if (other_username != username) {
                    // clear line to print messages
                    if (!cleared) {
                        clear_input();
                        cleared = true;
                    }
                    // print messages
                    if (other_username == "Server")
                        set_text_color(Color::Yellow);
                    else
                        set_text_color(Color::Cyan);
                    std::cout << other_username << ": ";
                    set_text_color(Color::None);
                    std::cout << other_message << std::endl;
                }
            }
            // check for disconnection
            if (tcp.receive(packet) == Socket::Disconnected) {
                std::cout << std::endl;
                LOG(Warning) << "Lost connection to Server at "
                             << tcp.get_remote_address() << "@"
                             << tcp.get_remote_port();
                connected = false;
            }
            // if cleared, resume input
            if (cleared)
                resume_input();
            cleared = false;
        }
        sleep(milliseconds(10));
    }
}
