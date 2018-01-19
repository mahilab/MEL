#pragma once

#include <MEL/Daq/Module.hpp>
#include <MEL/Daq/ChannelBase.hpp>

namespace mel {



//==============================================================================
// CLASS DEClARATION
//==============================================================================

/// Encapsulates an InputOutput only Module, with added functionality
template <typename T>
class InputOutput : public Module<T> {

public:

    /// Represents an InputOutput direction
    enum Direction {
        Input,
        Output
    };

    class Channel;

    /// Default constructor
    InputOutput(const std::string& name,
                const std::vector<uint32>& channel_numbers,
                const std::vector<Direction>& directions) :
        Module<T>(name, IoType::InputOutput, channel_numbers),
        directions_(directions),
        enable_values_(Module<T>::channel_count_),
        disable_values_(Module<T>::channel_count_),
        expire_values_(Module<T>::channel_count_)
    {
        sort_input_output_channel_numbers();
    }

    /// Default destructor
    virtual ~InputOutput() { }

    /// Sets the directions of all channels
    virtual bool set_directions(const std::vector<Direction>& directions) {
        if (validate_channel_count(directions)) {
            directions_ = directions;
            sort_input_output_channel_numbers();
            return true;
        }
        return false;
    }

    /// Sets the direction of a single channel
    virtual bool set_direction(uint32 channel_number, Direction direction) {
        if (Module<T>::validate_channel_number(channel_number)) {
            directions_[Module<T>::channel_map_.at(channel_number)] = direction;
            sort_input_output_channel_numbers();
            return true;
        }
        return false;
    }

    /// Sets the expire values of all channels
    virtual bool set_expire_values(const std::vector<T>& expire_values) {
        if (Module<T>::validate_channel_count(expire_values)) {
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

    void sort_input_output_channel_numbers() {
        input_channel_numbers_.clear();
        output_channel_numbers_.clear();
        for (std::size_t i = 0; i < channel_numbers_.size(); ++i) {
            if (directions_[i] == Direction::Input)
                input_channel_numbers_.push_back(channel_numbers_[i]);
            else if (directions_[i] == Direction::Output)
                output_channel_numbers_.push_back(channel_numbers_[i]);
        }
    }

protected:

    std::vector<Direction> directions_;           ///< The I/O directions of each channel
    std::vector<uint32> input_channel_numbers_;   ///< the channel numbers that are inputs
    std::vector<uint32> output_channel_numbers_;  ///< the channel numbers that are outputs
    std::vector<T> enable_values_;                ///< The initial values set when the Module is enabled
    std::vector<T> disable_values_;               ///< The final values set when the Module is disabled
    std::vector<T> expire_values_;                ///< The expire values when the Module expires

public:

    /// Encapsulates a Module channel
    class Channel : public ChannelBase<T, InputOutput<T>> {

    public:

        /// Default constructor. Creates invalid channel
        Channel() : ChannelBase<T, InputOutput<T>>() {}

        /// Creates a valid channel.
        Channel(InputOutput* module, uint32 channel_number) :
            ChannelBase<T, InputOutput<T>>(module, channel_number) { }

        /// Sets the direction of the channel
        void set_direction(Direction direction) {
            ChannelBase<T, InputOutput<T>>::module_->set_direction(
                ChannelBase<T, InputOutput<T>>::channel_number_, direction);
        }

        /// Sets the enable value of the channel
        void set_enable_value(T enable_value) {
            ChannelBase<T, InputOutput<T>>::module_->set_intial_value(
                ChannelBase<T, InputOutput<T>>::channel_number_, enable_value);
        }

        /// Sets the disable value of the channel
        void set_disable_value(T disable_value) {
            ChannelBase<T, InputOutput<T>>::module_->set_disable_value(
                ChannelBase<T, InputOutput<T>>::channel_number_, disable_value);
        }

        /// Sets the expiration value of the channel
        bool set_expire_value(T expire_value) {
            return ChannelBase<T, InputOutput<T>>::module_->set_expire_value(
                ChannelBase<T, InputOutput<T>>::channel_number_, expire_value);
        }

    };

};

} // namespace mel
