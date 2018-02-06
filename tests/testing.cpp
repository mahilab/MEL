#include <MEL/Utility/Console.hpp>
#include <MEL/Utility/System.hpp>
#include <thread>
#include <mutex>
#include <MEL/Utility/Clock.hpp>
#include <MEL/Utility/Mutex.hpp>
#include <MEL/Utility/Windows/NamedMutex.hpp>
#include <MEL/Utility/Spinlock.hpp>

using namespace mel;

int main() {

    Spinlock mutex;

    Clock clock;
    int j = 0;
    for (int i = 0; i < 1000000; ++i) {
        mutex.lock();
        ++j;
        mutex.unlock();
    }
    print(j);
    print(clock.get_elapsed_time());

    return 0;
}
