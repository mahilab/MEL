#include <MEL/Core/VirtualVelocitySensor.hpp> 
#include <MEL/Utility/Console.hpp>

namespace mel {

VirtualVelocitySensor::VirtualVelocitySensor(const std::string& name, 
    PositionSensor& position_sensor, 
    const Differentiator& diff) :
    VelocitySensor("VirtualVelocitySensor::" + name),
    position_sensor_(position_sensor),
    diff_(diff)
{
}

bool VirtualVelocitySensor::enable() {
    diff_.reset();
    clock_.restart();
    return VelocitySensor::enable();
}

bool VirtualVelocitySensor::disable() {
    diff_.reset();
    clock_.restart();
    return VelocitySensor::disable();
}

void VirtualVelocitySensor::update() {
    if (is_enabled())
        velocity_ = diff_.update(position_sensor_.get_position(), clock_.get_elapsed_time());
    else
        velocity_ = 0.0;
}

double VirtualVelocitySensor::get_velocity() {
    return velocity_;
}

} // namespace mel