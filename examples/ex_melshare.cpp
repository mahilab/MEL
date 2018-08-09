#include <MEL/Communications/MelShare.hpp>
#include <MEL/Logging/Log.hpp>
#include <MEL/Utility/Console.hpp>
#include <MEL/Communications/Packet.hpp>
#include <vector>

// Usage:
// To run this example, open two terminals and run the following:
//
// Terminal 1: melshare.exe 1A
// Terminal 2: melshare.exe 1B

using namespace mel;

int main(int argc, char* argv[]) {
    static ColorConsoleWriter<TxtFormatter> consoleAppender;

    if (argc > 1) {
        MelShare ms("melshare");
        std::string id = argv[1];
        if (id == "1A") {
            ms.write_message("Hello from C++! Please send me some data.");
            prompt("Press Enter after running 1B ...");
            print(ms.read_data());
        }
        else if (id == "1B") {
            print(ms.read_message());
            ms.write_data({1.0, 2.0, 3.0});
        }
        else if (id == "2A") {
            Packet packet;
            packet << 3.0f << "evan";
            ms.write(packet);
            prompt("Press Enter after running 2B ...");
            print(ms.read_data());
        }
        else if (id == "2B") {
            Packet packet;
            float three;
            std::string evan;
            ms.read(packet);
            packet >> three >> evan;
            print(three);
            print(evan);
            packet.clear();
            packet << 1.0 << 2.0 << 3.0 << 4.0 << 5.0;
            ms.write(packet);
        }

    }
    return 0;
}