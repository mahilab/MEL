#pragma once

#include <MEL/Core/Device.hpp>
#include <MEL/Utility/Types.hpp>
#include <MEL/Utility/Console.hpp>
#include <vector>
#include <map>
#include <algorithm>

namespace mel {

//==============================================================================
// TYPES
//==============================================================================

/// The Module/Channel type
enum class IoType {
    Input,          ///< Module that reads in real-world values
    Output,         ///< Module that writes out real-world values
    Bidirectional   ///< Module that reads or writes real-world values
};

//==============================================================================
// CLASS DECLARATION
//==============================================================================

/// Template class that encapsulates a set of same-type DAQ channels
template <typename T>
class Module : public Device {

public:

    /// Encapsulates a Module channel
    class Channel {
    public:

        /// Default constructor. Creates invalid channel
        Channel() : module_(nullptr), channel_number_(-1) {}

        /// Overloaded constructor. Creates channel on module.
        Channel(Module<T>* module, uint32 channel_number) :
            module_(module),
            channel_number_(channel_number) { }

        /// Updates this channel with the real-world
        bool update() {
            return module_->update_channel(channel_number_);
        }

        /// Returns the current value of the channel
        T get_value() const {
            return module_->get_value(channel_number_);
        }

        /// Returns reference to the channel's module
        const Module<T>& get_module() const {
            return *module_;
        }

        /// Gets the channel number of this Channel
        uint32 get_channel_number() const {
            return channel_number_;
        }

    protected:

        Module<T>* module_;             ///< The Module this channel is on
        const uint32 channel_number_;  ///< The physical channel number
    };

public:

    /// Default constructor
    Module(const std::string& name, IoType type, const std::vector<uint32>& channel_numbers) :
        Device(name),
        type_(type),
        channel_numbers_(sort_and_reduce_channels(channel_numbers)),
        channel_count_(channel_numbers_.size()),
        channel_map_(make_channel_map(channel_numbers_)),
        values_(channel_count_),
        min_values_(channel_count_),
        max_values_(channel_count_),
        invalid_channel_(Channel(this,-1))
    {
        make_channels();
    }

    /// Default destructor
    virtual ~Module() { }

    /// This function should call the DAQ's API to update all channels with the
    /// real-world. If the Module is an input, it should read in current values
    /// to #values_. If the Module is an output, it should write out current values
    /// from #values_. It should update all values at once.
    virtual bool update() = 0;

    /// This function should call the DAQ's API to update a single channel with
    /// the real-world. It should not update any other channels.
    virtual bool update_channel(uint32 channel_number) = 0;

    /// Sets the min and max possible values of each channel
    virtual bool set_ranges(const std::vector<T>& min_values, const std::vector<T>& max_values) {
        if (validate_channel_count(min_values) && validate_channel_count(max_values)) {
            min_values_ = min_values;
            max_values_ = max_values;
            return true;
        }
        return false;
    }

    /// Sets the min and max possible value of a single channel
    virtual bool set_range(uint32 channel_number, T min_value, T max_value) {
        if (validate_channel_number(channel_number)) {
            min_values_[channel_map_.at(channel_number)] = min_value;
            max_values_[channel_map_.at(channel_number)] = max_value;
            return true;
        }
        return false;
    }

public:

    /// Gets non-const reference to the current channel values of this Module
    std::vector<T>& get_values() {
        return values_;
    }

    /// Gets the current value of a a single channel. If an invalid channel number
    /// is passed, the default value of the underlying channel type is returned.
    T get_value(uint32 channel_number) const {
        if(validate_channel_number(channel_number))
            return values_[channel_map_.at(channel_number)];
        return T();
    }


    /// Gets the vector of channel numbers this Module maintains
    const std::vector<uint32>& get_channel_numbers() const {
        return channel_numbers_;
    }

    /// Returns the number of channels on this Module
    std::size_t get_channel_count() const {
        return channel_count_;
    }

    /// Returns the IoType of this Module
    IoType get_type() const {
        return type_;
    }

    /// Gets a Channel object given a channel number. If an invalid channel number
    /// is passed, an invalid Channel is returned.
    virtual const Channel& get_channel(uint32 channel_number) {
        if (validate_channel_number(channel_number))
            return channels_[channel_map_.at(channel_number)];
        else
            return invalid_channel_;
    }

protected:

    /// Makes vector of the Channels contained on this Module
    virtual void make_channels() {
        for (std::size_t i = 0; i < channel_count_; ++i) {
            channels_.push_back(Channel(this, channel_numbers_[i]));
        }
    }

    /// Checks if a channel number is a number defined on this Module
    bool validate_channel_number(uint32 channel_number) const {
        if (channel_map_.count(channel_number) > 0)
            return true;
        std::string message = "Invalid channel number " + stringify(channel_number) +
            " not declared in channel numbers {" + stringify(channel_numbers_) + "}.";
        print(message);
        return false;
    }

    /// Checks if the size of a vector equals the number of channels
    template <typename V>
    bool validate_channel_count(const std::vector<V>& v) const {
        if (channel_count_ == v.size())
            return true;
        std::string message = "Invalid number of elements in vector {" + stringify(v) +
            "} not equal to channel count of " + stringify(channel_count_) + ".";
        print(message);
        return false;
    }

    /// Sorts and reduces a channel numbers vector such as {3, 5, 5, 2} to {2, 3, 5}
    static std::vector<uint32> sort_and_reduce_channels(const std::vector<uint32>& channels) {
        std::vector<uint32> sorted_channels = channels;
        std::sort(sorted_channels.begin(), sorted_channels.end());
        sorted_channels.erase(std::unique(sorted_channels.begin(), sorted_channels.end()), sorted_channels.end());
        return sorted_channels;
    }

    /// Makes an associative channel map mapping channel number to vector index
    /// (e.g. if channel_numbers = {2,3,5} then index = {0,1,2})
    static std::map<uint32, std::size_t> make_channel_map(const std::vector<uint32>& channel_numbers) {
        std::map<uint32, std::size_t> channel_map;
        for (std::size_t i = 0; i < channel_numbers.size(); ++i)
            channel_map[channel_numbers[i]] = i;
        return channel_map;
    }

protected:

    const IoType type_ ;                               ///< The IoType of this Module
    const std::vector<uint32> channel_numbers_;        ///< The channel numbers used by this Module
    const std::size_t channel_count_;                  ///< The total number of channels used by this Module
    const std::map<uint32, std::size_t> channel_map_;  ///< Associates a channel number with a vector index position

    std::vector<T> values_;          ///< The real-world values of the channels in this Module
    std::vector<T> min_values_;      ///< The minimum possible values of each channel
    std::vector<T> max_values_;      ///< The maximum possible values of each channel

    Channel invalid_channel_;        ///< Represents an invalid channel
    std::vector<Channel> channels_;  ///< The channels contained on this Module

public:

};

} // namespace mel

//==============================================================================
// CLASS DOCUMENTATION
//==============================================================================
