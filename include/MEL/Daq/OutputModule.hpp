#pragma once

#include <MEL/Daq/Module.hpp>

namespace mel {

//==============================================================================
// CLASS DEClARATION
//==============================================================================

/// Encapsulates an Output only Module, with added functionality
template <typename T>
class OutputModule : public Module<T> {

public:

    /// Encapsulates and Output channel
    class Channel : public Module<T>::Channel {

    public:

        Channel() :
            Module<T>::Channel(),
            module_(nullptr) { }

        Channel(OutputModule* module, uint32 channel_number) :
            Module<T>::Channel(module, channel_number),
            module_(module) { }

        void set_value(T value) {
            module_->set_value(Module<T>::Channel::channel_number_, value);
        }

        void set_enable_value(T enable_value) {
            module_->set_intial_value(channel_number_, enable_value);
        }

        void set_disable_value(T disable_value) {
            module_->set_disable_value(channel_number_, disable_value);
        }

        bool set_expire_value(T expire_value) {
            return module_->set_expire_value(channel_number_, expire_value);
        }

    private:

        OutputModule* module_;  ///< The Output Module this channel is on

    };

public:

    /// Default constructor
    OutputModule(const std::string& name, const std::vector<uint32>& channel_numbers) :
        Module<T>(name, IoType::Output, channel_numbers),
        enable_values_(Module<T>::channel_count_),
        disable_values_(Module<T>::channel_count_),
        expire_values_(Module<T>::channel_count_),
        invalid_channel_(OutputModule::Channel(this,-1))
    {
        OutputModule::make_channels();
    }

    /// Default destructor
    virtual ~OutputModule() { }

    /// This function should call the DAQ's API to set watchdog expire values if
    /// the functionality exists. The vector of expire values must correspond to
    /// the enabled channel numbers in acending order. The base implementation
    /// below should be called in the derived implementation.
    virtual bool set_expire_values(const std::vector<T>& expire_values) {
        if (validate_channel_count(expire_values)) {
            expire_values_ = expire_values;
            return true;
        }
        return false;
    }

    /// Sets the expire value of a single channel
    virtual bool set_expire_value(uint32 channel_number, T expire_value) {
        if (validate_channel_number(channel_number)) {
            expire_values_[channel_map_.at(channel_number)] = expire_value;
            return true;
        }
        return false;
    }

public:

    /// Sets the current channel values of this Module. If the incorrect number
    /// of values is pass, no values are set.
    void set_values(const std::vector<T>& values) {
        if (validate_channel_count(values))
            values_ = values;
    }

    /// Sets the current value of a single channel. If an invalid channel number
    /// is passed, non value is set
    void set_value(uint32 channel_number, T value) {
        if (validate_channel_number(channel_number))
            values_[channel_map_.at(channel_number)] = value;
    }

    /// Sets the initial values to be written on enable
    void set_enable_values(const std::vector<T>& enable_values) {
        if (validate_channel_count(enable_values))
            enable_values_ = enable_values;
    }

    /// Sets the intial value of a single channel to be written on enable
    void set_enable_value(uint32 channel_number, T enable_value) {
        if (validate_channel_number(channel_number))
            enable_values_[channel_map_.at(channel_number)] = enable_value;
    }

    /// Sets the final values to be written on disable
    void set_disable_values(const std::vector<T>& disable_values) {
        if (validate_channel_count(disable_values))
            disable_values_ = disable_values;
    }

    /// Sets the final value of a single channel to be written on disable
    void set_disable_value(uint32 channel_number, T disable_value) {
        if (validate_channel_number(channel_number))
            disable_values_[channel_map_.at(channel_number)] = disable_value;
    }

    /// Overrides get_channel() so that it returns an Output::Channel
    const Channel& get_channel(uint32 channel_number) override {
        if (validate_channel_number(channel_number))
            return OutputModule::channels_[channel_map_.at(channel_number)];
        else
            return OutputModule::invalid_channel_;
    }

 private:

     /// Overrides make_channels() so that channels_ is filled with Encoder::Channels
     void make_channels() override {
         for (std::size_t i = 0; i < channel_count_; ++i)
             OutputModule::channels_.push_back(OutputModule::Channel(this, channel_numbers_[i]));
     }

protected:

    std::vector<T> enable_values_;   ///< The initial values set when the Module is enabled
    std::vector<T> disable_values_;  ///< The final values set when the Module is disabled
    std::vector<T> expire_values_;   ///< The expire values when the Module expires

    Channel invalid_channel_;        ///< Represents an invalid channel
    std::vector<Channel> channels_;  ///< The channels contained on this Module

};

}
