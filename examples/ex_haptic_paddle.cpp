#include <MEL/Core/Motor.hpp>
#include <MEL/Core/Robot.hpp>
#include <MEL/Daq/Quanser/Q8Usb.hpp>
#include <MEL/Utility/Console.hpp>
#include <MEL/Logging/Log.hpp>
#include <MEL/Utility/Timer.hpp>
#include <MEL/Communications/Windows/MelShare.hpp>
#include <MEL/Math/Differentiator.hpp>
#include <MEL/Math/Filter.hpp>

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
public:
    /// Constructor
    HapticPaddle(DigitalOutput::Channel d_o,
                 AnalogOutput::Channel ao,
                 AnalogInput::Channel ai,
                 double gain,
                 double offset)
        : Robot("haptic_paddle"),
          amp_("amc_12a8", High, d_o, 1.3, ao),
          motor_("pitman_9434",
                 0.0229,
                 amp_,
                 Limiter(1.8821, 12.0, seconds(1))),
          sensor_("honeywell_ss49et", ai, gain, offset) {
        // joints_.emplace_back("axis0", motor_, )
    }

private:
    Amplifier amp_;
    Motor motor_;
    HallEffectSensor sensor_;
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

int main(int argc, char const* argv[]) {

    // intialize logger
    init_logger();

    // register CTRL-C handler
    register_ctrl_handler(handler);

    // create MEL Share
    std::vector<double> data(3);
    MelShare ms("haptic_paddle");

    // create Q8 USB
    Q8Usb q8;

    // enable Q8 Usb
    q8.enable();

    // create HallEffectSensor
    HallEffectSensor sensor("haptic_paddle_hall", q8.analog_input[0], -30.292, 86.32);

    // create control loop timer
    Timer timer(milliseconds(1), Timer::Hybrid);

    // create Differentiator
    Differentiator diff(Differentiator::Technique::CentralDifference);

    // create filter -- 2nd order, Butterworth, low-pass filter, 50 Hz cuttoff "MATLAB: [b,a] = butter(2, 50/500)"
    Filter filter({ 0.031238976917092e-3, 
        0.124955907668367e-3,   
        0.187433861502551e-3,   
        0.124955907668367e-3,  
        0.031238976917092e-3}, 
        { 1.000000000000000, 
        -3.589733887112174,   
        4.851275882519412,
        -2.924052656162454,  
        0.663010484385890 }); 

    // enter control loop
    while (!stop) {
        q8.update_input();

        data[0] = sensor.get_position();
        data[1] = diff.update(data[0], timer.get_elapsed_time());
        data[2] = filter.update(data[1]);

        ms.write_data(data);
        timer.wait();
    }

    return 0;
}
