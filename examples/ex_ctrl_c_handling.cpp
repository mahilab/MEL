#include <MEL/Utility/Console.hpp>

using namespace mel;

ctrl_bool flag = false;
int my_handler(unsigned long param) {
    if (param == CTRL_C_EVENT)
        print("Ctrl-C Pressed");
    else if (param = CTRL_BREAK_EVENT) {
        print("Ctrl-Break Pressed");
        flag = true;
    }    
    // ... check other Ctrl values as needed
    return 1;
}

int main(int argc, char *argv[]) {
    print("Press Ctrl+C to continue, or Ctrl+Break to exit");
    register_ctrl_handler(my_handler);
    while (!flag) {}
    return 0;
}

