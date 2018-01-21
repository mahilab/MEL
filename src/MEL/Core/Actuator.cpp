#include <MEL/Core/Actuator.hpp>

namespace mel {

Actuator::Actuator() :
    Device("invalid_actuator"),
    enable_mode_(EnableMode::None),
    torque_(0.0),
    torque_sense_(0.0),
	has_torque_sense_(true)
{ }

Actuator::Actuator(std::string name, EnableMode enable_mode) :
    Device(name),
    enable_mode_(enable_mode),
    torque_(0.0),
    torque_sense_(0.0),
	has_torque_sense_(true)
{ }

} // namespace mel
