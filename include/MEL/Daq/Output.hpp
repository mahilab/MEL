#pragma once

#include <MEL/Daq/Module.hpp>

namespace mel {

//==============================================================================
// CLASS DEClARATION
//==============================================================================

/// Encapsulates an Output only Module, with added functionality
template <typename T>
class Output : public Module<T> {

public:

    class Channel;

    /// Default constructor
    Output(const std::string& name, const std::vector<uint32>& channel_numbers) :
        Module<T>(name, IoType::Output, channel_numbers),
        enable_values_(Module<T>::channel_count_),
        disable_values_(Module<T>::channel_count_),
        expire_values_(Module<T>::channel_count_)
    {
    }

    /// Default destructor
    virtual ~Output() { }

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
        if (Module<T>::validate_channel_number(channel_number)) {
            expire_values_[Module<T>::channel_map_.at(channel_number)] = expire_value;
            return true;
        }
        return false;
    }

public:

    /// Sets the current channel values of this Module. If the incorrect number
    /// of values is pass, no values are set.
    void set_values(const std::vector<T>& values) {
        if (validate_channel_count(values))
            Module<T>::values_ = values;
    }

    /// Sets the current value of a single channel. If an invalid channel number
    /// is passed, non value is set
    void set_value(uint32 channel_number, T value) {
        if (Module<T>::validate_channel_number(channel_number))
            Module<T>::values_[Module<T>::channel_map_.at(channel_number)] = value;
    }

    /// Sets the initial values to be written on enable
    void set_enable_values(const std::vector<T>& enable_values) {
        if (validate_channel_count(enable_values))
            enable_values_ = enable_values;
    }

    /// Sets the intial value of a single channel to be written on enable
    void set_enable_value(uint32 channel_number, T enable_value) {
        if (Module<T>::validate_channel_number(channel_number))
            enable_values_[Module<T>::channel_map_.at(channel_number)] = enable_value;
    }

    /// Sets the final values to be written on disable
    void set_disable_values(const std::vector<T>& disable_values) {
        if (validate_channel_count(disable_values))
            disable_values_ = disable_values;
    }

    /// Sets the final value of a single channel to be written on disable
    void set_disable_value(uint32 channel_number, T disable_value) {
        if (Module<T>::validate_channel_number(channel_number))
            disable_values_[Module<T>::channel_map_.at(channel_number)] = disable_value;
    }

    Channel get_channel(uint32 channel_number) {
        if (Module<T>::validate_channel_number(channel_number))
            return Channel(this, channel_number);
        else
            return Channel();
    }

    std::vector<Channel> get_channels(const std::vector<uint32>& channel_numbers) {
        std::vector<Channel> channels;
        for (std::size_t i = 0; i < channel_numbers.size(); ++i)
            channels.push_back(get_channel(channel_numbers[i]));
        return channels;
    }

    Channel operator[](uint32 channel_number) {
        return get_channel(channel_number);
    }

    std::vector<Channel> operator[](const std::vector<uint32>& channel_numbers) {
        return get_channels(channel_numbers);
    }

protected:

    std::vector<T> enable_values_;   ///< The initial values set when the Module is enabled
    std::vector<T> disable_values_;  ///< The final values set when the Module is disabled
    std::vector<T> expire_values_;   ///< The expire values when the Module expires

public:

    /// Encapsulates and Output channel
    class Channel {

    public:

        /// Default constructor. Creates invalid channel
        Channel() : module_(nullptr), channel_number_(-1) {}

        /// Creates a valid channel
        Channel(Output* module, uint32 channel_number) :
            module_(module),
            channel_number_(channel_number)
        { }

        /// Updates this channel with the real-world
        bool update() {
            return module_->update_channel(channel_number_);
        }

        /// Returns the current value of the channel
        T get_value() const {
            return module_->get_value(channel_number_);
        }

        /// Returns the current value of the channel
        T operator()() {
            return get_value();
        }

        /// Sets the current value of the channel
        void set_value(T value) {
            module_->set_value(channel_number_, value);
        }

        /// Sets the current value of the channel
        void operator()(T value) {
            set_value(value);
        }

        /// Sets the enable value of the channel
        void set_enable_value(T enable_value) {
            module_->set_intial_value(channel_number_, enable_value);
        }

        /// Sets the disable value of the channel
        void set_disable_value(T disable_value) {
            module_->set_disable_value(channel_number_, disable_value);
        }

        /// Sets the expiration value of the channel
        bool set_expire_value(T expire_value) {
            return module_->set_expire_value(channel_number_, expire_value);
        }

        /// Gets the channel number
        uint32 get_channel_number() const {
            return channel_number_;
        }

    private:

        Output* module_;               ///< The Output Module this channel is on
        const uint32 channel_number_;  ///< The physical channel number

    };

};

} // namespace mel
