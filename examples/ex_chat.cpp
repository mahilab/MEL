// MIT License
//
// MEL - Mechatronics Engine & Library
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
// Author(s): Evan Pezent (epezent@rice.edu)

#include <MEL/Communications/Packet.hpp>
#include <MEL/Communications/SocketSelector.hpp>
#include <MEL/Communications/TcpListener.hpp>
#include <MEL/Communications/TcpSocket.hpp>
#include <MEL/Communications/UdpSocket.hpp>
#include <MEL/Logging/Log.hpp>
#include <MEL/Utility/Console.hpp>
#include <MEL/Utility/Mutex.hpp>
#include <MEL/Utility/Options.hpp>
#include <map>
#include <thread>

using namespace std;
using namespace mel;

//==============================================================================
// CONSTANTS AND GLOBALS
//==============================================================================

const unsigned short SERVER_TCP = 55001; // port 55001 (server TCP port) must be
const unsigned short SERVER_UDP = 55002; // port-forwarded on the server machine
const unsigned short CLIENT_UDP = 55003; // to enable internet-based chat

const size_t MAX_USERNAME_LENGTH = 16;
const size_t MAX_MESSAGE_LENGTH  = 64;

ctrl_bool STOP(false);

//==============================================================================
// FUNCTIONS
//==============================================================================

bool handler(CtrlEvent event);

//==============================================================================
// SERVER
//==============================================================================

class Server {
public:
    ~Server();
    bool init();
    void run();
private:
    void connect_client();
    void disconnect_client(const string& name);
    void broadcast_address();
    void broadcast_message(const string& username, const string& message);
    void process_packets(vector<Packet>& packets);
    void process_disconnections(vector<string>& disconnections);
private:
    UdpSocket udp;
    TcpListener listener;
    SocketSelector selector;
    map<string, TcpSocket*> clients;
};

//==============================================================================
// CLIENT
//==============================================================================

class Client {
public:
    Client(const string& username);
    ~Client();
    bool connect();
    bool connect(IpAddress server_address);
    void run();
private:
    void print_border();
    void clear_input();
    void resume_input();
    void messaging_thread_func();
private:
    string username;
    string message;
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
		("s,server","Starts Chat as Server")
		("u,user",  "Starts Chat as User and sets username", value<string>())
        ("i,ip",    "IP dddress of the Sever",               value<string>());
	auto input = options.parse(argc, argv);

    init_logger();
    register_ctrl_handler(handler);
    cls();

    if (input.count("server")){
        Server server;
        if (server.init())
            server.run();
    }
    else if (input.count("user")) {
        Client client(input["user"].as<string>());
        if (input.count("ip")) {
            if (client.connect(input["ip"].as<string>()))
                client.run();
        }
        else {
            if (client.connect())
                client.run();
        }
    }
    else {
        print(options.help());
        print("  Examples:");
        print("    Server:     ./chat.exe -s");
        print("    User (LAN): ./chat.exe -u epezent");
        print("    User (www): ./chat.exe -u epezent -i 68.225.108.149\n");
    }

    return 0;
}

//==============================================================================
// FUNCTIONS
//==============================================================================

bool handler(CtrlEvent event) {
    if (event == CtrlEvent::CtrlC)
        STOP = true;
    return true;
}

//==============================================================================
// SERVER IMPLEMENTATION
//==============================================================================

bool Server::init() {
    if (udp.bind(SERVER_UDP) == Socket::Error)
        return false;
    if (listener.listen(SERVER_TCP) == Socket::Error)
        return false;
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
    LOG(Info) << "Starting Server";
    LOG(Info) << "Server Local IP Address:  " << IpAddress::get_local_address();
    LOG(Info) << "Server Public IP Address: " << IpAddress::get_public_address();
    while (!STOP) {
        if (selector.wait(seconds(1))) {
            // check for connections
            if (selector.is_ready(listener))
                connect_client();
            else {
                // check for packets and disconnections
                vector<Packet> packets;
                vector<string> disconnections;
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
        string username;
        client->receive(packet);
        packet >> username;
        packet.clear();
        if (username == "Server") {
            packet << "Username Server is reserved";
            client->send(packet);
            LOG(Info) << "Rejected user attemping to join as Server";
            delete client;
        }
        else if (clients.count(username)) {
            packet << ("Username " + username + " is unavailable");
            client->send(packet);
            LOG(Info) << "Rejected duplicate user " << username << " at "
                      << client->get_remote_address();
            delete client;
        }
        else {
            broadcast_message("Server", (username + " joined the Chat"));
            clients[username] = client;
            selector.add(*client);
            packet << "OK";
            client->send(packet);
            LOG(Info) << "Connected to " << username << " at "
                      << client->get_remote_address();
        }
    }
    else
        delete client;
}

void Server::disconnect_client(const string& username) {
    LOG(Info) << "Disconnected from " << username << " at "
              << clients[username]->get_remote_address();
    selector.remove(*clients[username]);
    delete clients[username];
    clients.erase(username);
}

void Server::broadcast_address() {
    Packet empty_packet;
    udp.send(empty_packet, IpAddress::Broadcast, CLIENT_UDP);
}

void Server::broadcast_message(const string& username,
                               const string& message)
{
    Packet packet;
    packet << username << message;
    for (auto it = clients.begin(); it != clients.end(); ++it) {
        if (it->first != username) {
            TcpSocket& client = *(it->second);
            client.send(packet);
        }
    }
}

void Server::process_packets(vector<Packet>& packets) {
    for (size_t i = 0; i < packets.size(); ++i) {
        string username, message;
        packets[i] >> username >> message;
        LOG(Info) << username << ": " << message;
        broadcast_message(username, message);
    }
    packets.clear();
}

void Server::process_disconnections(vector<string>& disconnections) {
    for (size_t i = 0; i < disconnections.size(); ++i)
        disconnect_client(disconnections[i]);
    for (size_t i = 0; i < disconnections.size(); ++i) {
        broadcast_message("Server", (disconnections[i] + " left the Chat"));
    }
}

//==============================================================================
// CLIENT IMPLEMENTATION
//==============================================================================

Client::Client(const string& username) :
    username(username)
{
    message.reserve(MAX_MESSAGE_LENGTH);
}

Client::~Client() {
    if (connected) {
        LOG(Info) << "Disconnecting from Server at "<< tcp.get_remote_address();
        tcp.disconnect();
    }
}

bool Client::connect() {
    LOG(Info) << "Searching for Server on LAN";
    UdpSocket udp;
    if (udp.bind(CLIENT_UDP) == Socket::Error)
        return (connected = false);
    IpAddress server_address;
    unsigned short server_port;
    Packet packet;
    udp.receive(packet, server_address, server_port);
    LOG(Info) << "Found Server at " << server_address.to_string();
    udp.unbind();
    return connect(server_address);
}

bool Client::connect(IpAddress server_address) {
    if (username.length() > MAX_USERNAME_LENGTH) {
        LOG(Warning) << "Username must be " << MAX_USERNAME_LENGTH
                     << " or fewer characters long";
        return (connected = false);
    }
    LOG(Info) << "Joining Chat as " << username;
    if (tcp.connect(server_address, SERVER_TCP, seconds(10)) == Socket::Error) {
        LOG(Warning) << "Attempt to connect to Server at " << server_address
                     << " timed out";
        return (connected = false);
    }
    LOG(Info) << "Connected to Server at " << tcp.get_remote_address();
    // register username
    Packet packet;
    packet << username;
    tcp.send(packet);
    string response;
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
    // start messaging thread
    thread messaging_thread( [this] { this->messaging_thread_func(); });
    // messaging loop
    while(connected && !STOP) {
        // process user input
        char ch = static_cast<char>(get_key());
        {
            Lock lock(mutex);
            if (!connected)
                break;
            if (ch == KEY_ESCAPE)
                STOP = true;
            else if (ch == KEY_ENTER) {
                if (message.length() > 0) {
                    if (message == "cls" || message == "clear") {
                        cls();
                        message = "";
                        resume_input();
                    }
                    else if (message == "exit" || message == "quit")
                        STOP = true;
                    else {
                        cout << "\n";
                        Packet packet;
                        packet << username << message;
                        tcp.send(packet);
                        message = "";
                        resume_input();
                    }
                }
            }
            else if (ch == KEY_BACKSPACE) {
                if (message.length() > 0) {
                    message.pop_back();
                    cout << "\b \b";
                }
            }
            else if (message.length() < MAX_MESSAGE_LENGTH) {
                // Add character to message
                message += ch;
                cout << ch;
            }
        }
    }
    messaging_thread.join();
    clear_input();
    if (STOP)
        print_border();
}

void Client::clear_input() {
    size_t n = username.length() + message.length() + 2;
    for (size_t i = 0; i < n; ++i)
        cout << "\b \b";
}

void Client::resume_input() {
    set_text_color(Color::Green);
    cout << username << ": ";
    set_text_color(Color::None);
    cout << message;
}

void Client::print_border() {
    for (int i = 0; i < 80; ++i)
        cout << "-";
    cout << endl;
}

void Client::messaging_thread_func() {
    bool cleared = false;
    string other_username, other_message;
    Packet packet;
    while (connected && !STOP) {
        // receive message packets
        {
            Lock lock(mutex);
            while (tcp.receive(packet) == Socket::Done) {
                packet >> other_username >> other_message;
                // clear line to print messages
                if (!cleared) {
                    clear_input();
                    cleared = true;
                }
                // print messages
                if (other_username == "Server")
                    set_text_color(Color::Magenta);
                else
                    set_text_color(Color::Cyan);
                cout << other_username << ": ";
                set_text_color(Color::None);
                cout << other_message << endl;
                beep();
            }
            // check for disconnection
            if (tcp.receive(packet) == Socket::Disconnected) {
                cout << endl;
                print_border();
                LOG(Warning) << "Lost connection to Server at "
                             << tcp.get_remote_address();
                connected = false;
            }
            // if cleared, resume input
            if (cleared)
                resume_input();
            std::cout << "";
            cleared = false;
        }
        sleep(milliseconds(10));
    }
}
