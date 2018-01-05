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

    /// Default constructor
    Module(const std::string& name, IoType type, const std::vector<uint32>& channel_numbers) :
        Device(name),
        type_(type),
        channel_numbers_(sort_and_reduce_channels(channel_numbers)),
        channel_count_(channel_numbers_.size()),
        channel_map_(make_channel_map(channel_numbers_)),
        values_(channel_count_),
        min_values_(channel_count_),
        max_values_(channel_count_)
    {
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

protected:

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

};

} // namespace mel

//==============================================================================
// CLASS DOCUMENTATION
//==============================================================================
