#include <MEL/Utility/Windows/NamedMutex.hpp>
#include <MEL/Utility/Mutex.hpp>
#include <MEL/Utility/Spinlock.hpp>
#include <MEL/Utility/Console.hpp>
#include <MEL/Utility/System.hpp>
#include <thread>

using namespace mel;

void thread1_func(Lockable& lockable) {
    print("Thread 1: Entering. Aquiring lock!");
    Lock lock(lockable);
    sleep(milliseconds(10));
    prompt("Thread 1: Press enter to allow Thread 2 to continue.");
    print("Thread 1: Exiting");
}

void thread2_func(Lockable& lockable) {
    print("Thread 2: Entering. Can't continue, reached lock!");
    Lock lock(lockable);
    sleep(milliseconds(10));
    prompt("Thread 2: Press enter to allow Main to continue.");
    print("Thread 2: Exiting");
}

int main(int argc, char *argv[]) {
    if (argc > 1) {
        std::string id = argv[1];
        if (id == "named_mutex_A") {
            NamedMutex named_mutex("my_named_mutex");
            // lock the mutex using a Lock RAII wrapper
            Lock lock(named_mutex);
            prompt("Press ENTER to unlock the named_mutex");
        } else if (id == "named_mutex_B") {
            NamedMutex named_mutex("my_named_mutex", NamedMutex::OpenOnly);
            // you *can* lock and unlock a mutex like this, but this is not
            // exception safe and you run the risk of unlock never getting called.
            named_mutex.lock();
            print("B can continue now");
            named_mutex.unlock();
        }
        else if (id == "mutex") {
            print("Main: Spawning Threads");
            Mutex mutex;
            std::thread thread1(thread1_func, std::ref(mutex));
            sleep(milliseconds(1));
            std::thread thread2(thread2_func, std::ref(mutex));
            thread1.join();   // main will wait for thread1 to complete
            thread2.detach(); // but continue despite thread2 completing or not
            print("Main: Can't continue, reached lock!");
            Lock lock(mutex);
            sleep(milliseconds(10));
            print("Main: Thanks!");
        }
        else if (id == "spinlock") {
            print("Main: Spawning Threads");
            Spinlock spinlock;
            std::thread thread1(thread1_func, std::ref(spinlock));
            sleep(milliseconds(1));
            std::thread thread2(thread2_func, std::ref(spinlock));
            thread1.join();   // main will wait for thread1 to complete
            thread2.detach(); // but continue despite thread2 completing or not
            print("Main: Can't continue, reached lock!");
            Lock lock(spinlock);
            sleep(milliseconds(10));
            print("Main: Thanks!");
        }
    }
    return 0;
}
