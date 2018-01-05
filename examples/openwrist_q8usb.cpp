#include <MEL/Daq/Quanser/Q8Usb.hpp>
#include <MEL/Exoskeletons/OpenWrist/OpenWrist.hpp>
#include <MEL/Utility/System.hpp>
#include <vector>

using namespace mel;

static bool stop = false;
static void handler(int var) {
    stop = true;
}

int main() {

    // register ctrl-c handler
    register_ctrl_c_handler(handler);

    // enable Windows realtime
    enable_realtime();

    // make Q8 USB that's configured for current control with VoltPAQ-X4
    QOptions options;
    options.set_update_rate(QOptions::Fast);
    options.set_analog_output_mode(0, QOptions::AoMode::CurrentMode1, 0, 2.0, 20.0, 0, -1, 0, 1000);
    options.set_analog_output_mode(1, QOptions::AoMode::CurrentMode1, 0, 2.0, 20.0, 0, -1, 0, 1000);
    options.set_analog_output_mode(2, QOptions::AoMode::CurrentMode1, 0, 2.0, 20.0, 0, -1, 0, 1000);
    Q8Usb q8(options);

    // create OpenWrist and bind Q8 channels to it
    OwConfiguration config(
        q8,
        q8.watchdog,
        q8.digital_output[{ 0, 1, 2 }],
        q8.analog_output[{ 0, 1, 2 }],
        q8.analog_input[{ 0, 1, 2 }],
        q8.encoder[{ 0, 1, 2 }],
        q8.velocity[{ 0, 1, 2 }],
        { 1.0, 1.0, 1.0 }
    );
    OpenWrist ow(config);

    prompt("Press ENTER to calibrate Openwrist");
    ow.calibrate(stop);

    prompt("Press Enter to enable OpenWrist transparency mode.");
    ow.transparency_mode(stop);

    disable_realtime();
    return 0;
}

