#pragma once

#include <MEL/Core/Device.hpp>

namespace mel {

//==============================================================================
// CLASS DECLARATION
//==============================================================================

class PositionSensor : public Device {

public:

    /// Default constructor
    PositionSensor(std::string name);

    /// This function should return the position of the PositionSensor
    virtual double get_position() = 0;

protected:

    double position_; ///< stores the PositionSensor position since the last call to get_position()

};

} // namespace mel

//==============================================================================
// CLASS DOCUMENTATION
//==============================================================================
