#pragma once

#include <MEL/Daq//Quanser/QDaq.hpp>
#include <MEL/Daq/Quanser/QOptions.hpp>
#include <MEL/Daq/Quanser/QAnalogInput.hpp>
#include <MEL/Daq/Quanser/QAnalogOutput.hpp>
#include <MEL/Daq/Quanser/QDigitalInput.hpp>
#include <MEL/Daq/Quanser/QDigitalOutput.hpp>
#include <MEL/DAQ/Quanser/QEncoder.hpp>
#include <MEL/Daq/Quanser/QVelocity.hpp>
#include <MEL/Daq/Quanser/QWatchdog.hpp>

namespace mel {

//==============================================================================
// CLASS DECLARATION
//==============================================================================

/// Encapsulates Quanser Analog Input functionality
class Q8Usb : public QDaq {

public:

    /// Default constructor. Creates Q8 USB with all channels enabled and default
    /// QOptions
    Q8Usb(QOptions options = QOptions(), bool perform_sanity_check = true, uint32 id = next_id_);

    /// Overloaded constructor for defining specific channels to enable.
    Q8Usb(std::vector<uint32> ai_channels,
          std::vector<uint32> ao_channels,
          std::vector<uint32> di_channels,
          std::vector<uint32> do_channels,
          std::vector<uint32> enc_channels,
          QOptions options = QOptions(),
          bool perform_sanity_check = true,
          uint32 id = next_id_);

    /// Default destructor. First calls disable() if the Q8Usb is enabled
    /// then close() if the Q8Usb is open.
    ~Q8Usb();

    /// Enables the Q8Usb by sequentially calling the enable() function
    /// on all I/O modules. Consult the documentation for each module for
    /// details on what the enable functions do. This function also stops 
    /// and clears the watchdog if it is running or expired, and will open
    /// the Q8Usb if it is not currently open.
    bool enable() override;

    /// Disables the Q8Usb by sequentially calling the disable() function
    /// on all I/O modules. Consult the documentation for each module for
    /// details on what the enable functions do. This function also stops 
    /// and clears the watchdog if it is running or expired.
    bool disable() override;

    bool reset() override;

    /// Updates all Input Modules simultaneously. It is generally more
    /// efficient to call this once per loop, than to call the update()
    /// function on each module separately.
    bool update_input() override;

    /// Updates all Output Modules simultaneously. It is generally more
    /// efficient to call this once per loop, than to call the update()
    /// function on each module separately.
    bool update_output() override;

    /// Quanser provides no software-based method of differntiating Q8 USBs 
    /// when more than one is connected to the host. This funcion provides a 
    /// physical means by checking for a digital loopback on same numbered
    /// digital input and output channels. For example, if you connect two Q8
    /// USBs to the host, one which has DO5 connected to DI5 (arbitraily chosen), 
    /// and the MEL Q8Usb object with id = 0 returns identify(5) = true, you
    /// know Q8Usb(id = 0) is the Q8 USB with the loopback, and Q8Usb(id = 1)
    /// is the Q8 USB without a loopback. If identify(5) = false, you know
    /// Q8Usb(id = 0) is the Q8 USB without the loopback, and Q8Usb(id = 1)
    /// can be infered as the Q8 USB with a loopback, and subsequently checked 
    /// by calling indentify(5) on it.
    bool identify(uint32 channel_number);

    /// This is an alternate version of identify() that checks for a loopback
    /// connection on all digital channel pairs, and returns the channel number
    /// of the first connection found. If no connection is found, it returns -1.
    int identify();

public:

    /// Determines how many Q8 USBs are currently connected to host.
    /// Only call this function before enabling any Q8 USBs
    static std::size_t get_q8_usb_count();

public:

    QAnalogInput     analog_input;  ///< The analog input Module
    QAnalogOutput   analog_output;  ///< The analog output Module
    QDigitalInput   digital_input;  ///< The digital input Module
    QDigitalOutput digital_output;  ///< The digital output Module
    QEncoder              encoder;  ///< The encoder Module
    QVelocity            velocity;  ///< The encoder velocity Module
    QWatchdog            watchdog;  ///< The watchdog timer of this Q8 USB

private:


    /// Quarc can sometimes fail to properly intialize a Q8 USB even if it says
    /// otherwise. In these cases, velocity readings are widely incorrect and can 
    /// result in catstrophic behavior. This function performs a sanity check by
    /// checking that all velocities are zero on startup if #perform_sanity_check_
    /// is true (default).
    bool sanity_check();

private:

    bool perform_sanity_check_;  ///< If true, the sanity check will be performed on enable
    static uint32 next_id_;      ///< Static counter that determines the next ID# to use

};


} // namespace mel

//==============================================================================
// CLASS DOCUMENTATION
//==============================================================================

/// http://quanser-update.azurewebsites.net/quarc/documentation/q8_usb.html