#include <MEL/Utility/Console.hpp>
#include <thread>
#include <MEL/Utility/Timer.hpp>
#include <MEL/Utility/Mutex.hpp>
#include <MEL/Utility/System.hpp>
#include <MEL/Utility/Winidows/Keyboard.hpp>

using namespace mel;

std::string STR = "";
Mutex mutex;


void async_console() {
    while(true) {
        if (STR != "") {
            print(STR);
            STR = "";
        }
        sleep(milliseconds(1));
    }
}

void print_async(const std::string& str) {
    STR = str;
}

int main() {
    std::thread t(async_console);
    t.detach();
    while(true) {

    }

    return 0;
}
