#include <MEL/Math/Functions.hpp>
#include <MEL/Utility/Frequency.hpp>
#include <MEL/Utility/Console.hpp>
#include <MEL/Utility/Timer.hpp>

using namespace mel;

int main(int argc, char const* argv[]) {

    Timer timer(hertz(1000));
    print(timer.get_period());
    while(true) {
        timer.wait();
    }

    return 0;
}
