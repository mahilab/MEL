#include <MEL/Utility/Windows/NamedMutex.hpp>
#include <MEL/Utility/Mutex.hpp>
#include <MEL/Utility/Console.hpp>
#include <MEL/Utility/System.hpp>
#include <thread>

using namespace mel;

Mutex mutex;

void thread1_func() {
    print("Thread 1: Entering. Aquiring lock!");
    mutex.lock();
    sleep(milliseconds(10));
    prompt("Thread 1: Press enter to allow Thread 2 to continue.");
    mutex.unlock();
    print("Thread 1: Exiting");
}

void thread2_func() {
    print("Thread 2: Entering. Can't continue, reached lock!");
    mutex.lock();
    sleep(milliseconds(10));
    prompt("Thread 2: Press enter to allow Main to continue.");
    mutex.unlock();
    print("Thread 2: Exiting");
}

int main(int argc, char *argv[]) {
    if (argc > 1) {
        std::string id = argv[1];
        if (id == "A") {
            NamedMutex named_mutex("my_named_mutex");
            named_mutex.lock();
            prompt("Press ENTER to unlock the named_mutex");
            named_mutex.unlock();
        } else if (id == "B") {
            NamedMutex named_mutex("my_named_mutex");
            named_mutex.lock();
            print("B can continue now");

            named_mutex.unlock();
        }
        else if (id == "C") {
            print("Main: Spawning Threads");
            std::thread thread1(thread1_func);
            sleep(milliseconds(1));
            std::thread thread2(thread2_func);
            thread1.join();   // we will wait for thread1 to complete
            thread2.detach(); // but continue despite thread2 completing or not
            print("Main: Can't continue, reached lock!");
            mutex.lock();
            sleep(milliseconds(10));
            print("Main: Thanks!");
            mutex.unlock();
        }
    }
    return 0;
}
