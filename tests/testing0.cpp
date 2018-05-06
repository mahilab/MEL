#include <MEL/Communications/Windows/MelShare.hpp>
#include <MEL/Core/Clock.hpp>
#include <MEL/Utility/Console.hpp>

using namespace mel;
int main() {

    MelShare ms("benchmark");

    std::vector<double> data(5);
    Clock clock;
    for (std::size_t i = 0; i < 1000000; ++i) {
        data[0] = 1.0;
        data[1] = 2.0;
        data[2] = 3.0;
        data[3] = 4.0;
        data[4] = 5.0;

        ms.write_data(data);
    }
    print(clock.get_elapsed_time());


    return 0;
}

