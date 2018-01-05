#pragma once

#include <MEL/Daq/Module.hpp>

namespace mel {

//==============================================================================
// CLASS DEClARATION
//==============================================================================

/// Encapsulates an Input only Module
template <typename T>
class Input : public Module<T> {

public:

    class Channel;

    /// Default constructor
    Input(const std::string& name, const std::vector<uint32>& channel_numbers) :
        Module<T>(name, IoType::Input, channel_numbers)
    {
    }

    /// Default destructor
    virtual ~Input() { }

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

    /// Encapsulates a Module channel
    class Channel {
    public:

        /// Default constructor. Creates invalid channel
        Channel() : module_(nullptr), channel_number_(-1) {}

        /// Creates a valid channel.
        Channel(Input* module, uint32 channel_number) :
            module_(module),
            channel_number_(channel_number) { }

        /// Synchronizes the channel with the real-world
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

        /// Gets the channel number
        uint32 get_channel_number() const {
            return channel_number_;
        }

    protected:

        Input* module_;                ///< The Module this channel is on
        const uint32 channel_number_;  ///< The physical channel number

    };

};

} // namespace mel
