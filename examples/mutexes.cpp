#include <MEL/Utility/Windows/NamedMutex.hpp>
#include <MEL/Utility/Console.hpp>

#include <csignal>

using namespace mel;

int main(int argc, char *argv[]) {
    if (argc > 1) {
        std::string id = argv[1];
        if (id == "A") {
            NamedMutex mutex("my_mutex");
            mutex.lock();
            prompt("Press ENTER to unlock the mutex");
            mutex.unlock();
        } else if (id == "B") {
            NamedMutex mutex("my_mutex");
            mutex.lock();
            print("B can continue now");
            mutex.unlock();
        }
    }
    return 0;
}
