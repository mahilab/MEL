#include <MEL/Utility/Mutex.hpp>
#include <mutex>
#include <thread>
#include <MEL/Utility/Clock.hpp>
#include <MEL/Utility/Console.hpp>

using namespace mel;

int main(int argc, char const *argv[]) {

    Clock clock;
    std::mutex mutex;
    for (std::size_t i = 0; i < 1000000; ++i) {
        mutex.lock();
        mutex.unlock();
    }

    print(clock.get_elapsed_time());

    return 0;
}
