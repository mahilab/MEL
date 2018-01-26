#include <MEL/Daq/Quanser/Q8Usb.hpp>
#include <MEL/Math/Waveform.hpp>
#include <MEL/Utility/System.hpp>
#include <MEL/Utility/Timer.hpp>
#include <MEL/Utility/Windows/Keyboard.hpp>

using namespace mel;

// create global stop variable CTRL-C handler function
static bool stop = false;
static void handler(int var) {
    stop = true;
}

int main() {

    // register CTRL-C handler
    register_ctrl_c_handler(handler);

    //==============================================================================
    // CONSTUCT/OPEN/CONFIGURE
    //==============================================================================

    // create default Q8 USB object
    // (all channels enabled, auto open on, sanity check on)
    Q8Usb q8;
    // override default enable/disable/expiration states
    q8.digital_output.set_enable_values(
        std::vector<logic>(8, HIGH));  // default is LOW
    q8.digital_output.set_disable_values(
        std::vector<logic>(8, HIGH));  // default is LOW
    q8.digital_output.set_expire_values(
        std::vector<logic>(8, HIGH));  // default is LOW
    q8.analog_output.set_enable_values(
        std::vector<voltage>(8, 1.0));  // default is 0.0
    q8.analog_output.set_disable_values(
        std::vector<voltage>(8, 2.0));  // default is 0.0
    q8.analog_output.set_expire_values(
        std::vector<voltage>(8, 3.0));  // default is 0.0

    //==============================================================================
    // ENABLE
    //==============================================================================

    // ask for user input
    prompt("Press ENTER to enable Q8 USB.");
    // enable Q8 USB
    q8.enable();

    //==============================================================================
    // ENCODER
    //==============================================================================

    // ask for user input
    prompt(
        "Connect an encoder to channel 0 then press ENTER to Encoder start "
        "test.");
    // create 10 Hz Timer
    Timer timer(milliseconds(100));
    // start encoder loop
    while (timer.get_elapsed_time() < seconds(5) && !stop) {
        q8.update_input();
        print(q8.encoder.get_value(0));
        timer.wait();
    }

    //==============================================================================
    // ANALOG INPUT/OUTPUT
    //==============================================================================

    // ask for user input
    prompt("Connect AO0 to AI0 then press ENTER to start AIO test.");
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

    //==============================================================================
    // DIGITAL INPUT/OUTPUT
    //==============================================================================

    // ask for user input
    prompt("Connect DI0 to DO0 then press ENTER to start DIO test.");
    logic signal = HIGH;
    // start analog loopback loop
    timer.restart();
    while (timer.get_elapsed_time() < seconds(5) && !stop) {
        q8.update_input();
        print(q8.digital_input.get_value(0));
        signal = !signal;
        q8.digital_output.set_value(0, signal);
        q8.update_output();
        timer.wait();
    }

    //==============================================================================
    // WATCHDOG
    //==============================================================================

    // ask for user input
    prompt(
        "Press Enter to start the watchdog test. Press W to simulate a missed "
        "deadline, or do nothing to allow normal operation.");
    // set watchdog timeout (default value is 100ms)
    q8.watchdog.set_timeout(milliseconds(10));
    // make a timer faster than our watchdog time out
    timer = Timer(milliseconds(1));
    // start watchdog
    q8.watchdog.start();
    while (timer.get_elapsed_time() < seconds(5) && !stop) {
        // simulate a missed deadline if W pressed
        if (Keyboard::is_key_pressed(Key::W)) {
            print("The program missed it's deadline!");
            sleep(milliseconds(11));
        }
        // kick the watchdog so it doesn't expire and check if it's still
        // watching
        if (!q8.watchdog.kick())
            break;  // watchdog timeout out
        // wait the control loop timer
        timer.wait();
    }
    // check the state of the watchdog
    if (q8.watchdog.is_expired()) {
        print("Watchdog did expire. Stopping and clearing it.");
        q8.watchdog.stop();
        q8.watchdog.clear();
    } else {
        print("Watchdog did not expire. Stopping it.");
        q8.watchdog.stop();
    }

    //==============================================================================
    // DISABLE
    //==============================================================================

    prompt("Press Enter to disable the Q8 USB.");
    // disable Q8 USB
    q8.disable();
    // close Q8 USB
    q8.close();

    return 0;
}
