#include <MEL/Core/Device.hpp>
#include <MEL/Logging/Log.hpp>

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
    if (enabled_) {
        LOG(Warning) << "Ignored attempt to enable " << name_ << " since it is already enabled";
        return true;
    }
    LOG(Info) << "Enabled " << name_;
    enabled_ = true;
    return true;
}

bool Device::disable() {
    if (!enabled_) {
        LOG(Warning) << "Ignored attempt to disable " << name_ << " since it is already disabled";
        return true;
    }
    LOG(Info) << "Disabled " << name_;
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

