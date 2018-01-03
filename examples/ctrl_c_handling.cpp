#include <MEL/Utility/Console.hpp>

using namespace mel;

// Method 1: Member function
class MyClass {
public:
    // function and data affected by ctrl+c handler must be static
    static bool class_block;
    static void class_handler(int param) {
        class_block = false;
    }
};

bool MyClass::class_block(true); // must define static member data outside class

// Method 2: Global function
static bool global_block = true;
static void global_handler(int param) {
    global_block = false;
}

int main(int argc, char *argv[]) {
    print("Press Ctrl+C to continue.");
    // Method 1
    register_ctrl_c(MyClass::class_handler);
    while (MyClass::class_block) {}
    print("Since we made it this far, the class Ctrl+C handler was invoked.");
    // Method 2
    register_ctrl_c(global_handler);
    while (global_block) {}
    print("Since we made it this far, the global Ctrl+C handler was invoked.");
}
