#include <MEL/Communications/Windows/MelShare.hpp>
#include <MEL/Logging/Log.hpp>
#include <MEL/Utility/Console.hpp>
#include <vector>

// Usage:
// To run this example, open two terminals and run the following:
//
// Terminal 1: melshare.exe A
// Terminal 2: melshare.exe B

using namespace mel;

int main(int argc, char* argv[]) {
    static ColorConsoleWriter<TxtFormatter> consoleAppender;
    init_logger();

    if (argc > 1) {
        MelShare ms("melshare");
        std::string id = argv[1];
        if (id == "A") {
            ms.write_message("Hello from C++! Please send me some data.");
            prompt("Press Enter after running B ...");
            print(ms.read_data());
        } else if (id == "B") {
            print(ms.read_message());
            ms.write_data({1.0, 2.0, 3.0});
        }
    }
    return 0;
}
