// MIT License
//
// MEL - Mechatronics Library
// Copyright (c) 2018 Mechatronics and Haptic Interfaces Lab - Rice University
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// Author(s): Evan Pezent (epezent@rice.edu)

#ifndef MEL_QDAQ_HPP
#define MEL_QDAQ_HPP

#include <MEL/Daq/Daq.hpp>
#include <MEL/Daq/Quanser/QOptions.hpp>
#include <MEL/Utility/NonCopyable.hpp>

//==============================================================================
// FORWARD DECLARATIONS
//==============================================================================

/// Handle for Quanser devices. Must be in global namespace.
typedef struct tag_card* t_card;

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
    QDaq(const std::string& card_type,
         uint32 id,
         QOptions options = QOptions());

    /// Default destructor
    virtual ~QDaq();

public:
    /// Opens the QDaq and sets Options
    virtual bool open() override;

    /// Closes the QDaq
    virtual bool close() override;

    /// Overloaded version of set_options() that allows passing in new options
    bool set_options(const QOptions& options);

    /// Gets a copy of the QOptions of this QDaq
    QOptions get_options();

public:
    /// Determines how many of a specifc QDaq are currently connected to the
    /// host
    static std::size_t get_qdaq_count(const std::string& card_type);

    /// Gets the string message corresponding to a Quanser error number
    static std::string get_quanser_error_message(int error, bool format = true);

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

}  // namespace mel

#endif  // MEL_QDAQ_HPP
