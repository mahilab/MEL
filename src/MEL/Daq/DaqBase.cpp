#include <MEL/Daq/DaqBase.hpp>
#include <MEL/Utility/Console.hpp>
#include <MEL/Core/Time.hpp>
#include <MEL/Logging/Log.hpp>

namespace mel {

//==============================================================================
// CLASS DEFINITIONS
//==============================================================================

DaqBase::DaqBase(const std::string& name) :
    Device(name),
    open_(false)
{

}

DaqBase::~DaqBase() {
    if (is_enabled())
        disable();
    if (is_open()) 
        close();
}

bool DaqBase::open() {
    if (open_) {
        LOG(Warning) << "DAQ " << get_name() << " already open";
        return false;
    } 
    if (on_open()) {
        LOG(Info) << "Opened DAQ " << get_name();
        open_ = true;
        return true;
    }
    else {
        LOG(Error) << "Failed to open DAQ " << get_name();
        open_ = false;
        return false;
    }
}

bool DaqBase::close() {
    if (!open_) {
        LOG(Warning) << "DAQ " << get_name() << " already closed";
        return false;
    }
    if (on_close()) {
        LOG(Info) << "Closed DAQ " << get_name();
        open_ = false;
        return true;
    }
    else {
        LOG(Error) << "Failed to close DAQ " << get_name();
        open_ = true;
        return false;
    }
}

bool DaqBase::update_input() {
    // TODO
    return true;
}

bool DaqBase::update_output() {
    // TODO
    return true;
}

bool DaqBase::is_open() const {
    return open_;
}


} // namespace mel
