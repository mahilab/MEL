#include <MEL/Core/Actuator.hpp>

namespace mel {

Actuator::Actuator() :
    Device("invalid_actuator"),
    torque_(0.0),
    torque_sense_(0.0),
    enable_mode_(EnableMode::None),
	has_torque_sense_(true)
{ }

Actuator::Actuator(std::string name, EnableMode enable_mode) :
    Device(name),
    torque_(0.0),
    torque_sense_(0.0),
    enable_mode_(enable_mode),
	has_torque_sense_(true)
{ }

} // namespace mel
