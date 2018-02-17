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
#include <MEL/Core/PdController.hpp>

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
        : PositionSensor(name), ai_(ai), gain_(gain), offset_(offset) { }

    /// Gets the position of the hall effect sensor in [rad]
    double get_position() override {
        position_ = ai_.get_value() * gain_ + offset_;
        return position_;
    }

    /// Interactively calibrates the hall effect sensor in the console
    void calibrate(const std::vector<double>& positions) {
        print("\nRotate to the indicated position when prompted and hold. Press ENTER to begin calibration.\n", Color::Yellow);
        std::vector<double> volts(positions.size());
        for (std::size_t i = 0; i < positions.size(); ++i) {
            std::cout << std::setfill(' ') << std::setw(4) << positions[i] << "  :  ";
            sleep(seconds(3));
            ai_.update();
            volts[i] = ai_.get_value();
            std::cout << volts[i] << " V \n";
        }
        std::vector<double> mb = linear_regression(volts, positions);
        gain_ = mb[0];
        offset_ = mb[1];
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
          amp_("amc_12a8", High, d_o, -1.065, ao),
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
        add_joint(Joint("paddle_joint_0", motor_, 0.713 / 6.250, position_sensor_, 1.0,
                        velocity_sensor_, 1.0, {-40 * DEG2RAD, 40 * DEG2RAD},
                        400 * DEG2RAD, 0.5));
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

std::vector<double> calibrate_hall(AnalogInput::Channel ai_ch) {
    std::ofstream file;
    file.open("calibration.txt");
    file << mb[0] << "\n" << mb[1];
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

    // create MELShare for MELScope
    std::vector<double> from_ms(1);
    std::vector<double> to_ms(5);
    MelShare ms1("ms1");
    MelShare ms2("ms2");
    ms1.write_data(from_ms);

    // create Q8 USB
    Q8Usb q8;

    // create Haptic Paddle
    HapticPaddle happy(q8.digital_output[7],
                       q8.analog_output[0],
                       q8.analog_input[2],
                       -30.292 * DEG2RAD,
                       86.32 * DEG2RAD);

    // create PD controller
    PdController pd(5, 0.001);

    // enable Q8 Usb
    q8.enable();

    // attempt to open calibration file
    std::ifstream file;
    file.open("calibration.txt");

    // run calibration if requested
    double gain, offset;
    if (input.count("c") > 0 || !file.is_open()) {
        file.close();
        std::vector<double> mb = calibrate_hall(q8.analog_input[2]);
        gain = mb[0];
        offset = mb[1];
    }
    else {
        file >> gain >> offset;
        file.close();
        LOG(Info) << "Imported hall effect sensor gain and offset from calibration.txt";
    }

    // enable haptic paddle
    prompt("Press Enter to Enable Haptic Paddle");
    happy.enable();

    // create control loop timer
    Timer timer(milliseconds(1), Timer::Hybrid);

    // enter control loop
    print("Enter Control Loop");
    while (!stop) {
        q8.update_input();
        happy[0].get_velocity_sensor<VirtualVelocitySensor>().update();
        from_ms = ms1.read_data();
        q8.analog_output[0].set_value(from_ms[0]);
        to_ms[0] = happy[0].get_position() * RAD2DEG;
        to_ms[1] = happy[0].get_velocity() * RAD2DEG;
        happy[0].set_torque(pd.calculate(0, happy[0].get_position(), 0, happy[0].get_velocity()));
        ms2.write_data(to_ms);
       // q8.update_output();
        timer.wait();
    }

    return 0;
}
