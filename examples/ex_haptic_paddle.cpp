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
#include <MEL/Math/Butterworth.hpp>
#include <MEL/Math/Waveform.hpp>

using namespace mel;

//==============================================================================
// HALL EFFECT SENSOR
//==============================================================================

class HallEffectSensor : public PositionSensor {
public:
    /// Constructor
    HallEffectSensor(const std::string& name,
                     AnalogInput::Channel ai,
                     double gain = 0.0,
                     double offset = 0.0)
        : PositionSensor(name), ai_(ai), gain_(gain), offset_(offset) { }

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
                 AnalogInput::Channel ai) :
          // Robot constructor
          Robot("haptic_paddle"),
          // init amplifier
          amp_("amc_12a8", High, d_o, -1.065, ao),
          // init motor
          motor_("pitman_9434", 0.0229, amp_, Limiter(1.8821, 12.0, seconds(1))),
          // init position sensor
          position_sensor_("honeywell_ss49et", ai),
          // init virtual velocity sensor
          velocity_sensor_("honeywell_ss49et", position_sensor_)
    {
        // create joint
        add_joint(Joint("paddle_joint_0", motor_, 0.713 / 6.250, position_sensor_, 1.0,
                        velocity_sensor_, 1.0, {-40 * DEG2RAD, 40 * DEG2RAD},
                        400 * DEG2RAD, 0.5));
    }

    /// Overrides the default Robot::enable function with some custom logic
    bool enable() override {
        // load calibration
        std::ifstream file;
        file.open("calibration.txt");
        if (file.is_open()) {
            // read in previous calibration
            double gain, offset;
            file >> gain >> offset;
            position_sensor_.offset_ = offset * DEG2RAD;
            position_sensor_.gain_ = gain * DEG2RAD;
            LOG(Info) << "Imported Haptic Paddle hall effect sensor calibration";
            file.close();
        }
        else {
            file.close();
            calibrate();
        }
        return Robot::enable();
    }

    /// Interactively calibrates the hall effect sensor in the console
    void calibrate() {
        std::ofstream file;
        file.open("calibration.txt");
        std::vector<double> positions = { -30, -25, -20, -15, -10, -5, 0, 5, 10, 15, 20, 25, 30 };
        print("\nRotate to the indicated position when prompted and hold. Press ENTER to begin calibration.\n", Color::Yellow);
        std::vector<double> volts(positions.size());
        for (std::size_t i = 0; i < positions.size(); ++i) {
            beep();
            std::cout << std::setfill(' ') << std::setw(4) << positions[i] << "  :  ";
            sleep(seconds(3));
            position_sensor_.ai_.update();
            volts[i] = position_sensor_.ai_.get_value();
            std::cout << volts[i] << " V \n";
        }
        std::cout << "\n";
        std::vector<double> mb = linear_regression(volts, positions);
        position_sensor_.gain_ = mb[0];
        position_sensor_.offset_ = mb[1];
        file << mb[0] << "\n";
        file << mb[1] << "\n";
        LOG(Info) << "Calibrated Haptic Paddle hall effect sensor";
        file.close();
    }

private:
    Amplifier amp_;
    Motor motor_;
    HallEffectSensor position_sensor_;
    VirtualVelocitySensor velocity_sensor_;
};

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
    std::vector<double> to_ms_data(4);
    MelShare to_ms("to_ms");
    std::vector<double> from_ms_data = { 0, 0 };
    MelShare from_ms("from_ms");
    from_ms.write_data(from_ms_data);

    // create Q8 USB
    Q8Usb q8;

    // create Haptic Paddle
    HapticPaddle happy(q8.digital_output[7],
                       q8.analog_output[0],
                       q8.analog_input[2]);

    // create PD controller
    PdController pd(1.0, 0.01);

    // enable Q8 Usb
    q8.enable();

    // enable haptic paddle
    prompt("Press ENTER to enable Haptic Paddle");
    happy.enable();

    // perform calibration if requested
    if (input.count("c") > 0)
        happy.calibrate();

    Waveform sinwave(Waveform::Sin, seconds(2), 30.0);

    // create control loop timer
    Timer timer(hertz(1000), Timer::Hybrid);
    

    Butterworth buttpos(2, hertz(10), timer.get_frequency());
    Butterworth buttvel(4, hertz(5), timer.get_frequency());

    // enter control loop
    prompt("Press ENTER to start control loop");
    while (!stop) {
        q8.update_input();
        happy[0].get_velocity_sensor<VirtualVelocitySensor>().update();
        from_ms_data = from_ms.read_data();
        to_ms_data[0] = happy[0].get_position() * RAD2DEG;
        to_ms_data[1] = buttpos.update(happy[0].get_position()) * RAD2DEG;
        to_ms_data[2] = happy[0].get_velocity() * RAD2DEG;
        to_ms_data[3] = buttvel.update(happy[0].get_velocity()) * RAD2DEG;

        double x_ref = sinwave.evaluate(timer.get_elapsed_time()) * DEG2RAD;
        double torque = pd.calculate(x_ref, happy[0].get_position(), 0, happy[0].get_velocity());

        //double torque = from_ms_data[0] * mel::tanh(10.0 * buttvel.update(happy[0].get_velocity()));

        happy[0].set_torque(torque);

        to_ms.write_data(to_ms_data);
        q8.update_output();
        timer.wait();
    }

    return 0;
}
