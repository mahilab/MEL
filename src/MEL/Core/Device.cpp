#include <MEL/Core/Device.hpp>
#include <MEL/Utility/Log.hpp>

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
        LOG(WARNING) << "Device <" << name_ << "> already enabled";
        return true;
    }
    LOG(INFO) << "Enabled Device <" << name_ << ">";
    enabled_ = true;
    return true;
}

bool Device::disable() {
    if (!enabled_) {
        LOG(WARNING) << "Device <" << name_ << "> already disabled";
        return true;
    }
    LOG(INFO) << "Disabled Device <" << name_ << ">";
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

