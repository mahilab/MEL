#include <MEL/Communications/IpAddress.hpp>
#include <MEL/Utility/Console.hpp>

using namespace mel;

int main(int argc, char const *argv[]) {

    print(IpAddress::get_local_address());
    return 0;
}
