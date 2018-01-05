#include <MEL/Core/Device.hpp>

namespace mel {

//==============================================================================
// CLASS DEFINITIONS
//==============================================================================

Device::Device() :
    name_("invalid_device"),
    enabled_(false)
{ }

Device::Device(const std::string& name) :
    name_(name),
    enabled_(false)
{
}

Device::~Device() {

}

bool Device::enable() {
    enabled_ = true;
    return true;
}

bool Device::disable() {
    enabled_ = false;
    return true;
}

bool Device::is_enabled() const {
    return enabled_;
}

const std::string& Device::get_name() const {
    return name_;
}

void Device::set_name(const std::string& name) {
    name_ = name;
}

} // namespace mel

