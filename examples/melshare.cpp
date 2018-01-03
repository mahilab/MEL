#include <MEL/Communications/MelShare.hpp>
#include <MEL/Utility/Console.hpp>
#include <vector>

// To run this example, open two terminals and run the following:
//
// Terminal 1: shared_memory.exe A
// Terminal 2: shared_memory.exe B

using namespace mel;

int main(int argc, char *argv[]) {
    if (argc > 1) {
        std::string id = argv[1];
        if (id == "A") {
            MelShare ms("melshare");
            ms.write_data({1.0, 2.0, 3.0, 4.0, 5.0});
            prompt("Press Enter after running B");
            print(ms.read_message());

        } else if (id == "B") {
            MelShare ms("melshare");
            auto data = ms.read_data();
            print(data);
            ms.write_message("Hello, I'm a string from C++!");
        }
    }
    return 0;
}
