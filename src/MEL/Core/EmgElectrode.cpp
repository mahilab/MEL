#include <MEL/Core/EmgElectrode.hpp>

namespace mel {

EmgElectrode::EmgElectrode(std::string name, InputModule<voltage>::Channel ai_channel) :
    Device(name),
    ai_channel_(ai_channel),
    voltage_(0.0)
{ }

double EmgElectrode::get_voltage() {
    voltage_ = ai_channel_.get_value();
    return voltage_;
}

} // namespace mel
