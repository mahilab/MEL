#include <MEL/Core/Device.hpp>
#include <MEL/Logging/Log.hpp>

namespace mel {

//==============================================================================
// CLASS DEFINITIONS
//==============================================================================

Device::Device() :
    enabled_(false),
    long_name_("Device::"),
    name_("")
{ }

Device::Device(const std::string& name) :
    enabled_(false),
    long_name_("Device::" + name),
    name_(long_name_.substr(long_name_.find_last_of("::") + 1))
{
}

Device::~Device() {

}

bool Device::enable() {
    if (enabled_) {
        LOG(Warning) << "Ignored attempt to enable " << long_name_ << " since it is already enabled";
        return true;
    }
    LOG(Verbose) << "Enabled " << long_name_;
    enabled_ = true;
    return true;
}

bool Device::disable() {
    if (!enabled_) {
        LOG(Warning) << "Ignored attempt to disable " << long_name_ << " since it is already disabled";
        return true;
    }
    LOG(Verbose) << "Disabled " << long_name_;
    enabled_ = false;
    return true;
}

bool Device::is_enabled() const {
    return enabled_;
}

const std::string& Device::get_name() const {
    return name_;
}

const std::string& Device::get_long_name() const {
    return long_name_;
}

void Device::set_name(const std::string& name) {
    name_ = name;
    std::string lineage = long_name_.substr(0, long_name_.find_last_of("::"));
    long_name_ = lineage + name;
}

} // namespace mel

