#include <MEL/Communications/SharedMemory.hpp>
#include <MEL/Utility/Console.hpp>
#include <MEL/Communications/Packet.hpp>
#include <vector>

using namespace mel;

int main() {
    SharedMemory map("my_map");
    std::vector<double> data = {1.2, 3.4, 5.6, 7.8};
    map.write(&data[0], 32);
    prompt("Press Enter to exit");
    return 0;
}

