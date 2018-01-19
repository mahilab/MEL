#pragma once

#include <MEL/Daq/Daq.hpp>
#include <MEL/Daq/Quanser/QOptions.hpp>
#include <MEL/Utility/NonCopyable.hpp>

//==============================================================================
// FORWARD DECLARATIONS
//==============================================================================

/// Handle for Quanser devices. Must be in global namespace.
typedef struct tag_card * t_card;

namespace mel {

class QWatchdog;
class QAnalogInput;
class QAnalogOutput;
class QDigitalInput;
class QDigitalOutput;
class QDigitalInputOutput;
class QEncoder;
class QVelocity;

//==============================================================================
// TYPDEFS
//==============================================================================

/// Typedef for Quanser device handle
typedef t_card QHandle;

//==============================================================================
// CLASS DECLARATION
//==============================================================================

/// Encapsulates a generic Quanser DAQ
class QDaq : public Daq, NonCopyable {

public:

    /// Default constructor
    QDaq(const std::string& card_type, uint32 id, QOptions options);

    /// Default destructor
    virtual ~QDaq();

public:

    bool open() override;

    bool close() override;

    /// Use this function to set the QOptions. The function will immediately
    /// apply the options to the Q8 USB and return true if successful.
    bool set_options();

    /// Overloaded version of set_options() that allows passing in new options
    bool set_options(const QOptions& options);

    /// Gets a copy of the QOptions of this QDaq
    QOptions get_options();

public:

    /// Determines how many of a specifc QDaq are currently connected to the host
    static std::size_t get_qdaq_count(const std::string& card_type);

    /// Gets the string message corresponding to a Quanser error number
    static std::string get_quanser_error_message(int error);

protected:

    friend class QWatchdog;
    friend class QAnalogInput;
    friend class QAnalogOutput;
    friend class QDigitalInput;
    friend class QDigitalOutput;
    friend class QDigitalInputOutput;
    friend class QEncoder;
    friend class QVelocity;

    std::string card_type_;  ///< The card type string, per Quarc API
    uint32 id_;              ///< The ID# of this Q8 USB
    QHandle handle_;         ///< Internal handle to the Quanser device
    QOptions options_;       ///< The board specific options of this Q8 USB

};

} // namespace mel

//==============================================================================
// CLASS DOCUMENTATION
//==============================================================================
