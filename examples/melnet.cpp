#include <MEL/Communications/MelNet.hpp>
#include <MEL/Utility/Console.hpp>
#include <MEL/Utility/Clock.hpp>
#include <MEL/Utility/System.hpp>
#include <MEL/Utility/Timer.hpp>
#include <vector>
#include <string>

// To run this example, open two terminals are run the following:
//
// Terminal 1: melnet.exe blocking A <IP B> (e.g. melnet.exe blocking A 169.254.67.6)
// Terminal 2: melnet.exe blocking B <IP A> (e.g. melnet.exe blocking B 169.254.105.30)
//
// or
//
// Terminal 1: melnet.exe nonblocking A <IP B> (e.g. melnet.exe nonblocking A 169.254.67.6)
// Terminal 2: melnet.exe nonblocking B <IP A> (e.g. melnet.exe nonblocking B 169.254.105.30)
//
// The two terminals can be on the same computer (IP A = IP B) or separate
// computers on the same network (IP A =/= IP B).

using namespace mel;

void blocking_A(std::string remote_host) {
    // make MelNet A
    MelNet melnetA(55001, 55002, IpAddress(remote_host));
    // wait to receive data
    std::vector<double> data = melnetA.receive_data();
    print(data);
    // send new data
    data = {5,6,7,8,9};
    melnetA.send_data(data);
    // wait to receive message
    std::string message = melnetA.receive_message();
    print(message);
    // send new message
    message += ", World!";
    melnetA.send_message(message);
}

void blocking_B(std::string remote_host) {
    // make MelNet B
    MelNet melnetB(55002, 55001, IpAddress(remote_host));
    // send data
    std::vector<double> data = {0,1,2,3,4};
    melnetB.send_data(data);
    // wait to receive new data
    data = melnetB.receive_data();
    print(data);
    // send message
    std::string message = "Hello";
    Clock clock;
    melnetB.send_message(message);
    // wait to receive new message
    message = melnetB.receive_message();
    print(clock.get_elapsed_time());
    print(message);
}

void nonblocking_A(std::string remote_host) {
    // make MelNet A
    MelNet melnetA(55001, 55002, IpAddress(remote_host), false);
    while (melnetA.receive_message() != "Feed Me!") {
        print("Waiting to Feed B");
    }
    print("Feeding B");
    std::vector<double> data = {0,1,2,3,4};
    melnetA.send_data(data);
    while (melnetA.receive_message() != "More!") {
        print("Waiting to Feed B");
    }
    print("Feeding B again, he's really hungy!");
    data = {5,6,7,8,9};
    melnetA.send_data(data);
}

void nonblocking_B(std::string remote_host) {
    // make MelNet B
    MelNet melnetB(55002, 55001, IpAddress(remote_host), true);
    std::string message("Feed Me!");
    melnetB.send_message(message);
    std::vector<double> data = melnetB.receive_data();
    print(data);
    prompt("Press ENTER for more!");
    message = "More!";
    melnetB.send_message(message);
    data = melnetB.receive_data();
    print(data);
}

int main(int argc, char *argv[]) {

    if (argc > 3) {
        std::string mode = argv[1];
        std::string id = argv[2];
        std::string remote_host = argv[3];
        print("Mode:        " + mode);
        print("ID:          " + id);
        print("Remote Host: " + remote_host);
        if (mode == "blocking") {
            if (id == "A")
                blocking_A(remote_host);
            else if (id == "B")
                blocking_B(remote_host);
        } else if (mode == "nonblocking") {
            if (id == "A")
                nonblocking_A(remote_host);
            else if (id == "B")
                nonblocking_B(remote_host);
        }
    }
    return 0;
}
