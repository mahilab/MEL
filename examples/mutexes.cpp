#include <MEL/Utility/Mutex.hpp>
#include <MEL/Utility/Console.hpp>

#include <csignal>

using namespace mel;

int main(int argc, char *argv[]) {
    if (argc > 1) {
        std::string id = argv[1];
        if (id == "A") {
            Mutex mutex("my_mutex");
            mutex.try_lock();
            prompt("Press ENTER to release the mutex");
            mutex.release();
        } else if (id == "B") {
            Mutex mutex("my_mutex");
            mutex.try_lock();
            print("B can continue now");
            mutex.release();
        }
    }
    return 0;
}
