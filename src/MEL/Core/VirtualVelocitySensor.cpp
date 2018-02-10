#include <MEL/Core/VirtualVelocitySensor.hpp>   

namespace mel {

VirtualVelocitySensor::VirtualVelocitySensor(const std::string& name, 
    PositionSensor& position_sensor, 
    const Filter& filter,
    const Differentiator& diff) :
    VelocitySensor("VirtualVelocitySensor::" + name),
    position_sensor_(position_sensor_),
    filter_(filter),
    diff_(diff)
{

}

bool VirtualVelocitySensor::enable() {
    diff_.reset();
    filter_.reset();    
    clock_.restart();
    return VelocitySensor::enable();
}

bool VirtualVelocitySensor::disable() {
    diff_.reset();
    filter_.reset();
    clock_.restart();
    return VelocitySensor::disable();
}

double VirtualVelocitySensor::get_velocity() {
    if (is_enabled()) {
        velocity_ = filter_.update(diff_.update(position_sensor_.get_position(), clock_.get_elapsed_time()));
        return velocity_;
    }
    return 0.0;
}

} // namespace mel