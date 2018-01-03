#pragma once

#include <string>

namespace mel {

//==============================================================================
// CLASS DECLARATION
//==============================================================================

/// Abstract base class from which all physical devices should derive.
class Device {

public:

    /// Default constructor. Every device in MEL should have a unique name.
    Device(const std::string& name);

    /// Default destructor.
    virtual ~Device();

    /// The function should enable the device, performing start up and
    /// initialization procedures. It should return true if successful,
    /// false otherwise, and set #enabled_ accordingly
    virtual bool enable() = 0;

    /// The function should disable the device, performing necessary shutdown
    /// procedures. It should  return true if successful, false otherwise,
    /// and set #enabled_ accordingly
    virtual bool disable() = 0;

public:

    /// Returns true if the device is enabled, false if disabled.
    bool is_enabled() const;

    /// Returns the Device's string name
    const std::string& get_name() const;

    /// Sets the Device's string name
    void set_name(const std::string& name);

protected:

    std::string name_;  ///< The Device name
    bool enabled_;      ///< The Device enabled status

};

} // namespace mel

//==============================================================================
// CLASS DOCUMENTATION
//==============================================================================
