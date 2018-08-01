#include <MEL/Daq/Quanser/Q8Usb.hpp>
#include <MEL/Math/Waveform.hpp>
#include <MEL/Utility/System.hpp>
#include <MEL/Core/Timer.hpp>
#include <MEL/Utility/Windows/Keyboard.hpp>
#include <MEL/Utility/Console.hpp>
#include <MEL/Logging/Log.hpp>
#include <MEL/Logging/Writers/ColorConsoleWriter.hpp>

using namespace mel;

// create global stop variable CTRL-C handler function
ctrl_bool stop(false);
bool handler(CtrlEvent event) {
    if (event == CtrlEvent::CtrlC)
        stop = true;
    return true;
}

int main() {

    // register CTRL-C handler
    register_ctrl_handler(handler);

    //==============================================================================
    // CONSTUCT/OPEN/CONFIGURE
    //==============================================================================

    // create default Q8 USB object
    // (all channels enabled, auto open on, sanity check on)
    Q8Usb q8;

    // override default enable/disable/expiration states
    q8.DO.set_enable_values(std::vector<Logic>(8, High));  // default is LOW
    q8.DO.set_disable_values(std::vector<Logic>(8, High)); // default is LOW
    q8.DO.set_expire_values(std::vector<Logic>(8, High));  // default is LOW
    q8.AO.set_enable_values(std::vector<Voltage>(8, 1.0));  // default is 0.0
    q8.AO.set_disable_values(std::vector<Voltage>(8, 2.0)); // default is 0.0
    q8.AO.set_expire_values(std::vector<Voltage>(8, 3.0));  // default is 0.0

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
    while (timer.get_elapsed_time_actual() < seconds(5) && !stop) {
        q8.update_input();
        print(q8.encoder.get_value(0));
        timer.wait();
    }
    stop = false;


    //==============================================================================
    // ANALOG INPUT/OUTPUT
    //==============================================================================

    // ask for user input
    prompt("Connect AO0 to AI0 then press ENTER to start AIO test.");
    // create Waveform
    Waveform wave(Waveform::Sin, seconds(0.25), 5.0);
    // start analog loopback loop
    timer.restart();
    while (timer.get_elapsed_time_actual() < seconds(5) && !stop) {
        q8.update_input();
        print(q8.AI.get_value(0));
        double voltage = wave.evaluate(timer.get_elapsed_time_actual());
        q8.AO.set_value(0, voltage);
        q8.update_output();
        timer.wait();
    }
    stop = false;

    //==============================================================================
    // DIGITAL INPUT/OUTPUT
    //==============================================================================

    // ask for user input
    prompt("Connect DI0 to DO0 then press ENTER to start DIO test.");
    Logic signal = High;
    // start analog loopback loop
    timer.restart();
    while (timer.get_elapsed_time_actual() < seconds(5) && !stop) {
        q8.update_input();
        print(q8.DI.get_value(0));
        signal = (Logic)(High - signal);
        q8.DO.set_value(0, signal);
        q8.update_output();
        timer.wait();
    }
    stop = false;


    //==============================================================================
    // WATCHDOG
    //==============================================================================

    // ask for user input
    prompt(
        "Press Enter to start the watchdog test. Press W to simulate a missed "
        "deadline, or do nothing to allow normal operation.");
    // make a timer faster than our watchdog time out
    timer = Timer(milliseconds(1));
    // set watchdog timeout (default value is 100ms)
    q8.watchdog.set_timeout(milliseconds(10));
    // start watchdog
    q8.watchdog.start();
    timer.restart();
    while (timer.get_elapsed_time_actual() < seconds(5) && !stop) {
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
    stop = false;

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
