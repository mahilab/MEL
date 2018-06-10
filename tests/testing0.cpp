#include <MEL/Core/Clock.hpp>
#include <MEL/Utility/System.hpp>
#include <MEL/Utility/Console.hpp>
#include <MEL/Core/Timer.hpp>
#include <chrono>

using namespace mel;
using namespace std;

int main()
{
    Time t;
    Timer timer(hertz(2000));
    Clock clock;
    auto start = std::chrono::high_resolution_clock::now();
    clock.restart();
    timer.restart();
    //sleep(microseconds(500));
    timer.wait();
    auto end = std::chrono::high_resolution_clock::now();
    t = clock.get_elapsed_time();
    print(t);
    cout << "Elapsed time in microseconds : "
            << chrono::duration_cast<chrono::microseconds>(end - start).count()
            << " µs" << endl;
    return 0;
}
