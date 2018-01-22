#include <MEL/Daq/Quanser/Q8Usb.hpp>
#include <MEL/Utility/Timer.hpp> 
#include <MEL/Math/Waveform.hpp> 

using namespace mel;

// create global stop variable CTRL-C handler function
static bool stop = false;
static void handler(int var) {
    stop = true;
}

int main() {

    // register CTRL-C handler
    register_ctrl_c_handler(handler);

    // create default Q8 USB object (all channels enabled)
    Q8Usb q8;

    // ask for user input
    prompt("Press ENTER to open and enable Q8 USB.");

    // enable Q8 USB
    q8.enable();

    // ask for user input
    prompt("Connect an encoder to channel 0 then press ENTER to start test.");

    // create 10 Hz Timer
    Timer timer(milliseconds(100));

    // start encoder loop 
    while (timer.get_elapsed_time() < seconds(5) && !stop) {
        q8.update_input();
        print(q8.encoder.get_value(0));
        timer.wait();
    }

    stop = false;

    // ask for user input
    prompt("Connect AO0 to AI0 then press ENTER to start test.");

    // create Waveform
    Waveform wave(Waveform::Sin, seconds(0.25), 5.0);

    // start analog loopback loop
    timer.restart();
    while (timer.get_elapsed_time() < seconds(5) && !stop) {
        q8.update_input();
        print(q8.analog_input.get_value(0));
        double voltage = wave.evaluate(timer.get_elapsed_time());
        q8.analog_output.set_value(0, voltage);
        q8.update_output();
        timer.wait();
    }

    stop = false;

    // ask for user input
    prompt("Connect DO0 to DI0 then press ENTER to start test.");

    logic signal = HIGH;

    // start analog loopback loop
    timer.restart();
    while (timer.get_elapsed_time() < seconds(5) && !stop) {
        q8.update_input();
        print((int)(q8.digital_input.get_value(0)));
        signal = !signal;
        q8.digital_output.set_value(0, signal);
        q8.update_output();
        timer.wait();
    }

    // disable Q8 USB
    q8.disable();

    // close Q8 USB
    q8.close();
}
