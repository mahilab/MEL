#include <MEL/Communications/MelNet.hpp>   
#include <MEL/Utility/Timer.hpp>
#include <MEL/Utility/Console.hpp>
#include <MEL/Utility/System.hpp>
#include <MEL/Utility/Mutex.hpp>
#include <thread>
#include <atomic>

using namespace mel;

// Global data
Mutex mutex;
std::vector<double> data = { 0 };
std::atomic<bool> stop = false;

// CTRL+C handler
static void handler(int var) {
    stop = true;
}

// Communicatins thread function
void comm_thread_func() {
    print("Starting communications thread");
    MelNet mnet(55001, 55002, "169.254.208.230", false);
    while (!stop) {
        Lock lock(mutex);
        if (mnet.receive_message() == "update")
            mnet.send_data(data);
        sleep(milliseconds(10)); // ~100 Hz
    }
    print("Terminating communications thread");
}

// Main thread
int main(int argc, char *argv[]) {
    print("Starting main thread");
    // register ctrl-c handler
    register_ctrl_c_handler(handler);
    // start communications loop
    std::thread comm_thread(comm_thread_func);
    // start control loop
    Timer timer(milliseconds(1), Timer::Hybrid);
    while (!stop && timer.get_elapsed_time() < seconds(1)) {
        Lock lock(mutex);
        data[0] += 1.0;
        timer.wait();
    }
    // join comm thread (not really necessary, but whatevs)
    comm_thread.join();
    print("Terminating main thread");
    return 0;
}


