#include <MEL/Daq/Daq.hpp>
#include <MEL/Utility/Console.hpp>
#include <MEL/Utility/Time.hpp>

namespace mel {

//==============================================================================
// CLASS DEFINITIONS
//==============================================================================

Daq::Daq(const std::string& name) :
    Device("Daq::" + name),
    open_(false)
{
}

Daq::~Daq() {

}

bool Daq::open() {
    open_ = true;
    return true;
}

bool Daq::close() {
    open_ = false;
    return true;
}

bool Daq::is_open() const {
    return open_;
}


} // namespace mel
