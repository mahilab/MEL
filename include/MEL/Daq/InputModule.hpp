#pragma once

#include <MEL/Daq/Module.hpp>

namespace mel {

//==============================================================================
// CLASS DEClARATION
//==============================================================================

/// Encapsulates an Input only Module
template <typename T>
class InputModule : public Module<T> {

public:

    /// Default constructor
    InputModule(const std::string& name, const std::vector<uint32>& channel_numbers) :
        Module<T>(name, IoType::Input, channel_numbers)
    {
    }

    /// Default destructor
    virtual ~InputModule() { }

};

}
