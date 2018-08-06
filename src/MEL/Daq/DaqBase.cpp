#include <MEL/Daq/DaqBase.hpp>
#include <MEL/Utility/Console.hpp>
#include <MEL/Core/Time.hpp>

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

}

bool DaqBase::open() {
    open_ = true;
    return true;
}

bool DaqBase::close() {
    open_ = false;
    return true;
}

bool DaqBase::is_open() const {
    return open_;
}


} // namespace mel
