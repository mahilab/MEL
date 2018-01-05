#include <MEL/Daq/Quanser/Q8Usb.hpp>
#include <MEL/Utility/System.hpp>
#include <MEL/Utility/Console.hpp>
#include <vector>

using namespace mel;

static bool stop = false;
static void handler(int var) {
    stop = true;
}

int main() {

    // register ctrl-c handler
    register_ctrl_c_handler(handler);

    Q8Usb q8;
    q8.enable();

    q8.digital_output[0](HIGH);
    q8.update_output();

    while (!stop) {
        q8.update_input();
        print(q8.encoder[0]());
    }

    prompt("Press Enter to exit ...");

    return 0;
}