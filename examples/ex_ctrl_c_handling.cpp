#include <MEL/Utility/Console.hpp>

using namespace mel;

ctrl_bool flag(false);
bool my_handler(CtrlEvent event) {
    if (event == CtrlEvent::CtrlC)
        print("Ctrl-C Pressed");
    else if (event == CtrlEvent::CtrlQuit) {
        print("Ctrl-Break or Ctrl-\\ Pressed");
        flag = true;
    }
    // ... check other Ctrl values as needed
    return 1;
}

int main() {
    print("Press Ctrl+C to continue, or Ctrl+Break to exit");
    register_ctrl_handler(my_handler);
    while (!flag) {}
    return 0;
}

