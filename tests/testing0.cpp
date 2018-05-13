#include <MEL/Communications/Packet.hpp>
#include <MEL/Utility/Console.hpp>
#include <array>

using namespace mel;
int main() {

    Packet packet;
    std::vector<double> x = {1,2,3,4,5};
    std::vector<double> y;

    std::vector<std::string> a = {"evan","amy","krystin"};
    std::vector<std::string> b;

    packet << x << a;
    packet >> y >> b;

    print(y);
    print(b);

    return 0;
}

