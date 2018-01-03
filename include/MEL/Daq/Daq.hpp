#pragma once

#include <MEL/Core/Device.hpp>

namespace mel {

//==============================================================================
// FORWARD DECLARATIONS
//==============================================================================

class Time;

//==============================================================================
// CLASS DECLARATION
//==============================================================================

class Daq : public Device {

public:

    /// The default constructor
    Daq(const std::string& name);

    /// The default destructor
    virtual ~Daq();

    /// This function should open communication with the DAQ, either through
    /// an API, socket communication, etc. It should not perform any other task
    /// than opening the communication channel; start up procedures should be
    /// implemented in enable(). This function should return true if the open
    ///was successful, false otherwise, and set #open_ accordingly
    virtual bool open() = 0;

    /// This function should close communication with the device. It should
    /// return true if successful, false otherwise, and set #open_ accordingly
    virtual bool close() = 0;

    /// The function should call the DAQ's API to reset the device back to its
    /// default or factory settings. It should return true if successful, false
    /// otherwise.
    virtual bool reset() = 0;

    /// This function should call the update function on all Input Modules
    /// contained on this DAQ. Alternatively, this function may call an API
    /// function that updates all input values simultaneously, but care should
    /// be taken in making sure each Input module's values get updated as well.
    /// It should return true if successful, false otherwise.
    virtual bool update_input() = 0;

    /// This function should call the update function on all Output Modules
    /// contained on this DAQ. Alternatively, this function may call an API
    /// function that updates all input values simultaneously, but care should
    /// be taken in making sure each Input module's values get updated as well.
    /// It should return true if successful, false otherwise.
    virtual bool update_output() = 0;

public:

    /// Returns true if communication with the device is open, false if closed.
    bool is_open() const;

protected:

    bool open_;  ///< The Daq open status

};

} // namespace mel

//==============================================================================
// CLASS DOCUMENTATION
//==============================================================================
