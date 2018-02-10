#include <MEL/Communications/Windows/MelShare.hpp>
#include <MEL/Core/Motor.hpp>
#include <MEL/Core/Robot.hpp>
#include <MEL/Core/VirtualVelocitySensor.hpp>
#include <MEL/Daq/Quanser/Q8Usb.hpp>
#include <MEL/Logging/Log.hpp>
#include <MEL/Math/Constants.hpp>
#include <MEL/Math/Differentiator.hpp>
#include <MEL/Math/Filter.hpp>
#include <MEL/Utility/Console.hpp>
#include <MEL/Utility/Timer.hpp>
#include <MEL/Utility/Options.hpp>
#include <MEL/Math/Functions.hpp>
#include <conio.h>
#include <fstream>

using namespace mel;

//==============================================================================
// HALL EFFECT SENSOR
//==============================================================================

class HallEffectSensor : public PositionSensor {
public:
    /// Constructor
    HallEffectSensor(const std::string& name,
                     AnalogInput::Channel ai,
                     double gain,
                     double offset)
        : PositionSensor(name), ai_(ai), gain_(gain), offset_(offset) {}

    /// Gets the position of the hall effect sensor in [rad]
    double get_position() override {
        position_ = ai_.get_value() * gain_ + offset_;
        return position_;
    }

public:
    AnalogInput::Channel ai_;  ///< voltage read from hall effect sensor
    double gain_;              ///< calibration gain   [rad/V]
    double offset_;            ///< calibration offset [rad]
};

//==============================================================================
// HAPTIC PADDLE
//==============================================================================

class HapticPaddle : public Robot {

public:
    /// Constructor
    HapticPaddle(DigitalOutput::Channel d_o,
                 AnalogOutput::Channel ao,
                 AnalogInput::Channel ai,
                 double gain,
                 double offset) :
          // Robot constructor
          Robot("haptic_paddle"), 
          // init amplifier
          amp_("amc_12a8", High, d_o, 1.3, ao),
          // init motor
          motor_("pitman_9434",
                 0.0229,
                 amp_,
                 Limiter(1.8821, 12.0, seconds(1))),
          // init position sensor
          position_sensor_("honeywell_ss49et", ai, gain, offset),
          // init virtual velocity sensor
          velocity_sensor_(
              "honeywell_ss49et",
              position_sensor_,
              //  4nd order Butterworth, low-pass filter, 25 Hz cuttoff @ 1000 Hz sample rate
              Filter({0.031238976917092e-3, 0.124955907668367e-3, 0.187433861502551e-3, 0.124955907668367e-3, 0.031238976917092e-3},
                     {1.000000000000000,   -3.589733887112174,    4.851275882519412,   -2.924052656162454,    0.663010484385890})) 
    {
        // create joint
        add_joint(Joint("paddle_joint_0", motor_, 6.0, position_sensor_, 1.0,
                        velocity_sensor_, 1.0, {-50 * DEG2RAD, 50 * DEG2RAD},
                        400 * DEG2RAD, 2.0));
    }

private:
    Amplifier amp_;
    Motor motor_;
    HallEffectSensor position_sensor_;
    VirtualVelocitySensor velocity_sensor_;
};

//==============================================================================
// CALIBRATE HALL
//==============================================================================

std::vector<double> calibrate_hall() {
    print("\nRotate the pendulum to the indicated angle [deg] and press Enter\n", Color::Yellow);
    print("Angle : Voltage");
    std::vector<double> theta{ -30,-25,-20,-15,-10,-5,0,5,10,15,20,25,30 };
    std::vector<double> volts(theta.size());
    for (std::size_t i = 0; i < theta.size(); ++i) {
        std::cout << std::setfill(' ') << std::setw(4) << theta[i] << "  :  ";
        _getch();
        std::cout << i;        
        std::cout << "\n";
        volts[i] = i;
    }
    std::vector<double> mb = linear_regression(volts, theta);
    set_text_color(Color::Green);
    std::cout << "\n  gain = " << mb[0] << "\n";
    std::cout << "offset = " << mb[1] << "\n\n";
    reset_text_color();
    std::ofstream file;
    file.open("calibration.txt");
    file << mb[0] << "\n" << mb[1];
    return mb;
}

//==============================================================================
// MISC
//==============================================================================

// create global stop variable CTRL-C handler function
ctrl_bool stop(false);
bool handler(CtrlEvent event) {
    print("Ctrl+C Pressed");
    stop = true;
    return true;
}

//==============================================================================
// MAIN
//==============================================================================

int main(int argc, char* argv[]) {

    // intialize logger
    init_logger();

    // register CTRL-C handler
    register_ctrl_handler(handler);

    // make and parse console options
    Options options("haptic_paddle.exe", "MEL Haptic Paddle Example");
    options.add_options()
        ("c,calibrate", "Calibrates Hall Effect Sensor")
        ("h,help", "Prints this Help Message");
    auto input = options.parse(argc, argv);

    // print help message if requested
    if (input.count("h") > 0) {
        print(options.help());
        return 0;
    }

    // attempt to open calibration file
    std::ifstream file;
    file.open("calibration.txt");

    // run calibration if requested
    double gain, offset;
    if (input.count("c") > 0 || !file.is_open()) {
        file.close();
        std::vector<double> mb = calibrate_hall();
        gain = mb[0];
        offset = mb[1];
    }
    else {
        file >> gain >> offset;
        file.close();
        LOG(Info) << "Imported hall effect sensor gain and offset from calibration.txt";
    }

    // create MELShare for MELScope
    std::vector<double> data(3);
    MelShare ms("haptic_paddle");

    // create Q8 USB
    Q8Usb q8;

    // create Haptic Paddle
    HapticPaddle happy(q8.digital_output[0], 
                       q8.analog_output[0], 
                       q8.analog_input[0],
                       -30.292 * DEG2RAD,
                       86.32 * DEG2RAD);

    // enable Q8 Usb
    q8.enable();

    // create control loop timer
    Timer timer(milliseconds(1), Timer::Hybrid);

    // enter control loop
    while (!stop) {
        q8.update_input();
        data[0] = happy[0].get_position();
        data[1] = happy[1].get_velocity();
        ms.write_data(data);
        timer.wait();
    }

    return 0;
}
