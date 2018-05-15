#include <MEL/Utility/Console.hpp>
#include <MEL/Utility/System.hpp>

using namespace mel;

int main(int argc, char* argv[]) {
    while(true) {
        print(get_ch());
        sleep(milliseconds(1));
    }
    return 0;
}
