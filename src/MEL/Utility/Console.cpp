#include <MEL/Utility/Console.hpp>
#include <csignal>

namespace mel {

//==============================================================================
// CONSOLE OUTPUT
//==============================================================================

void print(Time t, bool end_line) {
    if (t.as_seconds() >= 1.0) {
        print(t.as_seconds(), false);
        print(" s", false);
    } else {
        if (t.as_milliseconds() > 1) {
            print(t.as_milliseconds(), false);
            print(" ms", false);
        } else {
            print(t.as_microseconds(), false);
            print(" us", false);
        }
    }
    if (end_line)
        std::cout << std::endl;
}

//==============================================================================
// CONSOLE INPUT
//==============================================================================

void prompt(std::string message) {
    print(message, false);
    getchar();
}

void register_ctrl_c_handler(void (*function)(int)) {
    signal(SIGINT, function);
    // signal(SIGBREAK, function);
}

}
