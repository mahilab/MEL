#include <MEL/Daq/Sensoray/S826.hpp>
#include <MEL/Core/Console.hpp>
#include <bitset>

using namespace mel;

int main(int argc, char const *argv[])
{
    // S826 s826(0);
    // s826.open();

    for (int x = 0; x < 8; ++x) {
        std::bitset<32> bX(x);
        print(bX.to_string());
    }
    
    return 0;
}
