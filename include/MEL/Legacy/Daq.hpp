#pragma once

#include <MEL/Utility/Console.hpp>
#include <MEL/Utility/Types.hpp>
#include <MEL/Core/Device.hpp>

namespace mel {

//==============================================================================
// CLASS DECLARATION
//==============================================================================

class Daq : public Device {

public:

    /// Channel struct containing a pointer to a Daq and a channel number.
    struct Channel {
        Daq* daq_;
        channel channel_number_;
        Channel() : daq_(nullptr), channel_number_(-1) {};
        Channel(Daq* daq, channel channel_number) : daq_(daq), channel_number_(channel_number) {}
    };

public:

    Daq(std::string name,
        channel_vec ai_channels,
        channel_vec ao_channels,
        channel_vec di_channels,
        channel_vec do_channels,
        channel_vec encoder_channels,
        channel_vec encrate_channels);

    virtual ~Daq() { };

    /// This function should enable the specific DAQ by opening a communication line, declaring channels, setting ranges, etc.
    virtual void enable() override = 0;

    /// This function should disable the specific DAQ by running any shutdown procedures and closing the commication line.
    virtual void disable() override = 0;

    /// This function should stop and/or clear the watchdog and reset the DAQ outputs to their initial values.
    virtual void reset();

    /// This function should read from all DAQ input channels simultenously and save the values to the corresponding data vectors.
    virtual void read_all();

    /// This function should write to all DAQ output channels simultaenously from the corresponding data vectors.
    virtual void write_all();

    /// This function should start the watchdog timer if the DAQ supports one.
    virtual void start_watchdog(double watchdog_timeout);

    /// This function should reload the watchdog timer if the DAQ supports one and return true if successful.
    virtual void reload_watchdog();

    /// This function should stop and/or clear the watchdog timer if the DAQ supports one.
    virtual void stop_watchdog();

    /// This function should return true if the watchdog timer has expired.
    virtual bool is_watchdog_expired();

protected:

    /// Returns index of a channel number in a channel numbers vector.
    int channel_number_to_index(const channel_vec& channels, const channel channel_number);

    /// Sorts and reduces a channel numbers vector such as {3, 1, 1, 2} to {1, 2, 3}.
    channel_vec sort_and_reduce_channels(channel_vec channels);

    //==========================================================================
    // ANALOG INPUT
    //==========================================================================

public:

    /// This function should read all analog input channels and save the values to #ai_voltages_ .
    virtual void read_analogs();

    /// Returns a vector of the most recent analog input vloltages (a copy of #ai_voltages_).
    virtual voltage_vec get_analog_voltages();

    /// Returns the most recent analog input voltage from a given channel number.
    virtual voltage get_analog_voltage(channel channel_number);

    /// This function should set the DAQ's analog input ranges and store the values in #ai_min_voltages_ and #ai_max_voltages_ .
    virtual void set_ai_voltage_ranges(voltage_vec min_voltages, voltage_vec max_voltages);

    struct Ai : Channel {
        Ai() : Channel() {}
        Ai(Daq* daq, channel channel_number) : Channel(daq, channel_number) {}
        voltage get_voltage() {
            return daq_->get_analog_voltage(channel_number_);
        }
    };

    virtual Ai ai_(channel channel_number);

protected:

    voltage_vec       ai_voltages_;        ///< vector for which analog inputs should be read in to
    const channel_vec ai_channel_nums_;    ///< vector of analog input channels numbers being used
    const std::size_t ai_channels_count_;  ///< vector of analog input channels being used
    voltage_vec       ai_min_voltages_;    ///< vector of minimum input voltages allowed by the board
    voltage_vec       ai_max_voltages_;    ///< vector of maximum input voltages allowed by the board

    //==========================================================================
    // ANALOG OUTPUT
    //==========================================================================

public:

    virtual void write_analogs();

    virtual void set_analog_voltages(voltage_vec new_voltages);

    virtual void set_analog_voltage(channel channel_number, voltage new_voltage);

    virtual void set_ao_voltage_ranges(voltage_vec min_voltages, voltage_vec max_voltages);

    virtual void set_ao_initial_voltages(voltage_vec initial_voltages);

    virtual void set_ao_final_voltages(voltage_vec final_voltages);

    virtual void set_ao_expire_voltages(voltage_vec expire_voltages);

    struct Ao : Channel {
        Ao() : Channel() {}
        Ao(Daq* daq, channel channel_number) : Channel(daq, channel_number) {}
        void set_voltage(voltage new_voltage) {
            daq_->set_analog_voltage(channel_number_, new_voltage);
        }
    };

    virtual Ao ao_(channel channel_number);

protected:

    voltage_vec       ao_voltages_;          ///< vector for which analog outputs should be written out from
    const channel_vec ao_channel_nums_;      ///< vector of analog output channels numbers being used
    const std::size_t      ao_channels_count_;    ///< vector of analog output channels being used
    voltage_vec       ao_min_voltages_;      ///< vector of minimum output voltages allowed by the board
    voltage_vec       ao_max_voltages_;      ///< vector of maximum output voltages allowed by the board
    voltage_vec       ao_initial_voltages_;  ///< vector of voltages analog outputs will go to when the program starts
    voltage_vec       ao_final_voltages_;    ///< vector of voltages analog outputs will go to when the program finishes
    voltage_vec       ao_expire_voltages_;   ///< vector of voltages analog outputs will go to if the watchdog expires

    //==========================================================================
    // DIGITAL INPUT
    //==========================================================================

public:

    virtual void read_digitals();

    virtual dsignal_vec get_digital_signals();

    virtual dsignal get_digital_signal(channel channel_number);

    struct Di : Channel {
        Di() : Channel() {}
        Di(Daq* daq, channel channel_number) : Channel(daq, channel_number) {}
        dsignal get_signal() {
            return daq_->get_digital_signal(channel_number_);
        }
    };

    virtual Di di_(channel channel_number);

protected:

    dsignal_vec       di_signals_;         ///< vector for which digital inputs should be read in to
    const channel_vec di_channel_nums_;    ///< vector of digital input channels numbers being used
    const std::size_t      di_channels_count_;  ///< vector of digital input channels being used

    //==========================================================================
    // DIGITAL OUTPUT
    //==========================================================================

public:

    virtual void write_digitals();

    virtual void set_digital_signals(dsignal_vec new_signals);

    virtual void set_digital_signal(channel channel_number, dsignal new_signal);

    virtual void set_do_initial_signals(dsignal_vec initial_signals);

    virtual void set_do_final_signals(dsignal_vec final_signals);

    virtual void set_do_expire_signals(dsignal_vec expire_signals);

    struct Do : Channel {
        Do() : Channel() {}
        Do(Daq* daq, channel channel_number) : Channel(daq, channel_number) {}
        void set_signal(dsignal new_signal) { daq_->set_digital_signal(channel_number_, new_signal); }
    };

    virtual Do do_(channel channel_number);

protected:

    dsignal_vec do_signals_;               ///< vector for which digital outputs should be written out from
    const channel_vec   do_channel_nums_;  ///< vector of digital output channels numbers being used
    const std::size_t   do_channels_count_;     ///< vector of digital output channels being used
    dsignal_vec do_initial_signals_;       ///< vector of signals digital outputs will go to when the program starts
    dsignal_vec do_final_signals_;         ///< vector of signals digital outputs will go to when the program finishes
    dsignal_vec do_expire_signals_;        ///< vector of signals digital outputs will go to if the watchdog expires

    //==========================================================================
    // ENCODERS
    //==========================================================================

public:

    virtual void read_encoders();

    virtual void read_encrates();

    virtual void zero_encoders();

    virtual void offset_encoders(std::vector<int32> offset_counts);

    virtual std::vector<int32> get_encoder_counts();

    virtual int32 get_encoder_count(channel channel_number);

    virtual std::vector<double> get_encoder_rates();

    virtual double get_encoder_rate(channel channel_number);

    virtual void set_encoder_quadrature_factors(std::vector<uint32> quadrature_factors);

    virtual std::vector<uint32> get_encoder_quadrature_factors();

    virtual uint32 get_encoder_quadrature_factor(channel channel_number);

    struct Encoder : Channel {
        Encoder() : Channel() {}
        Encoder(Daq* daq, channel channel_number) : Channel(daq, channel_number) {}
        int32 get_count() {
            return daq_->get_encoder_count(channel_number_);
        }
        uint32 get_quadrature_factor() {
            return daq_->get_encoder_quadrature_factor(channel_number_);
        }
    };

    struct EncRate : Channel {
        EncRate() : Channel() {}
        EncRate(Daq* daq, channel channel_number) : Channel(daq, channel_number) {}
        double get_rate() {
            return daq_->get_encoder_rate(channel_number_);
        }
        uint32 get_quadrature_factor() {
            return daq_->get_encoder_quadrature_factor(channel_number_);
        }
    };

    virtual Encoder encoder_(channel channel_number);

    virtual EncRate encrate_(channel channel_number);

protected:

    std::vector<int32>  enc_counts_;                 ///< vector for which encoder counts should be read in to
    std::vector<double> enc_rates;                   ///< vector for which encoder rates should be read in to
    const channel_vec   encoder_channel_nums_;       ///< vector of encoder channels being numbers used
    const channel_vec   encrate_channel_nums_;       ///< vector of encoder velocity channels numbers being used
    const std::size_t   encoder_channels_count_;     ///< vector of encoder channels being used
    const std::size_t   encrate_channels_count_;     ///< vector of encoder velocity channels being used
    std::vector<uint32> encoder_quadrature_factors_; ///< vector of encoder quadrature factors, e.g. 1, 2, or 4

};

} // namespace mel

//==============================================================================
// CLASS DOCUMENTATION
//==============================================================================