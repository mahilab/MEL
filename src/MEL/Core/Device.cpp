#include <MEL/Core/Device.hpp>
#include <MEL/Logging/Log.hpp>

namespace mel {

//==============================================================================
// CLASS DEFINITIONS
//==============================================================================

Device::Device() :
    enabled_(false),
    name_("UNNAMED")
{ }

Device::Device(const std::string& name) :
    enabled_(false),
    name_(name)
{ }

Device::~Device() { }

bool Device::enable() {
    if (enabled_) {
        LOG(Warning) << get_name() << " already enabled";
        return true;
    }
    if (on_enable()) {
        LOG(Verbose) << "Enabled " << get_name();
        enabled_ = true;
        return true;
    }
    else {
        LOG(Error) << "Failed to enabled " << get_name();
        enabled_ = false;
        return false;
    }
}

bool Device::disable() {
    if (!enabled_) {
        LOG(Warning) << get_name() << " already disabled";
        return true;
    }
    if (on_disable()) {
        LOG(Verbose) << "Disabled " << get_name();
        enabled_ = false;
        return true;
    }
    else {
        LOG(Error) << "Failed to disable " << get_name();
        enabled_ = true;
        return false;
    }
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

