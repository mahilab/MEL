#include <MEL/Core/Motor.hpp>
#include <MEL/Core/Robot.hpp>

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
// MAIN
//==============================================================================

int main(int argc, char const* argv[]) {
    /* code */
    return 0;
}
