#pragma once

#include <MEL/Daq//Quanser/QDaq.hpp>
#include <MEL/Daq/Quanser/QOptions.hpp>
#include <MEL/Daq/Quanser/QAnalogInput.hpp>
#include <MEL/Daq/Quanser/QAnalogOutput.hpp>
#include <MEL/Daq/Quanser/QDigitalInput.hpp>
#include <MEL/Daq/Quanser/QDigitalOutput.hpp>
#include <MEL/Daq/Quanser/QEncoder.hpp>
#include <MEL/Daq/Quanser/QWatchdog.hpp>

namespace mel {

//==============================================================================
// CLASS DECLARATION
//==============================================================================

/// Quanser Q2 USB class
class Q2Usb : public QDaq {

public:

    /// Default constructor. Creates Q2 USB with all channels enabled and default QOptions
    Q2Usb(QOptions options = QOptions(), uint32 id = next_id_);

    /// Default destructor. First calls disable() if the Q2Usb is enabled
    /// then close() if the Q2Usb is open.
    ~Q2Usb();

    /// Enables the Q2Usb by sequentially calling the enable() function
    /// on all I/O modules. Consult the documentation for each module for
    /// details on what the enable functions do. This function also stops
    /// and clears the watchdog if it is running or expired, and will open
    /// the Q2Usb if it is not currently open.
    bool enable() override;

    /// Disables the Q2Usb by sequentially calling the disable() function
    /// on all I/O modules. Consult the documentation for each module for
    /// details on what the enable functions do. This function also stops
    /// and clears the watchdog if it is running or expired.
    bool disable() override;

    /// Updates all Input Modules simultaneously. It is generally more
    /// efficient to call this once per loop, than to call the update()
    /// function on each module separately.
    bool update_input() override;

    /// Updates all Output Modules simultaneously. It is generally more
    /// efficient to call this once per loop, than to call the update()
    /// function on each module separately.
    bool update_output() override;

    /// Quanser provides no software-based method of differntiating Q2 USBs
    /// when more than one is connected to the host. This funcion provides a
    /// physical means by checking for a digital loopback on same numbered
    /// digital input and output channels. For example, if you connect two Q2
    /// USBs to the host, one which has DO0 connected to DI0 (arbitraily chosen),
    /// and the MEL Q2Usb object with id = 0 returns identify(0) = true, you
    /// know Q2Usb(id = 0) is the Q2 USB with the loopback, and Q2Usb(id = 1)
    /// is the Q2 USB without a loopback. If identify(0) = false, you know
    /// Q2Usb(id = 0) is the Q2 USB without the loopback, and Q2Usb(id = 1)
    /// can be infered as the Q2 USB with a loopback, and subsequently checked
    /// by calling indentify(0) on it.
    bool identify(uint32 channel_number);

    /// This is an alternate version of identify() that checks for a loopback
    /// connection on all digital channel pairs, and returns the channel number
    /// of the first connection found. If no connection is found, it returns -1.
    int identify();

public:

    /// Determines how many Q2 USBs are currently connected to host.
    /// Only call this function before enabling any Q2 USBs
    static std::size_t get_q2_usb_count();

public:

    QAnalogInput     analog_input;  ///< The analog input Module
    QAnalogOutput   analog_output;  ///< The analog output Module
    QDigitalInput   digital_input;  ///< The digital input Module
    QDigitalOutput digital_output;  ///< The digital output Module
    QEncoder              encoder;  ///< The encoder Module
    QWatchdog            watchdog;  ///< The watchdog timer of this Q2 USB

private:

    static uint32 next_id_;      ///< Static counter that determines the next ID# to use

};


} // namespace mel

//==============================================================================
// CLASS DOCUMENTATION
//==============================================================================

/// http://quanser-update.azurewebsites.net/quarc/documentation/Q2_usb.html
