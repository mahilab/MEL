#pragma once

#include <MEL/Utility/Types.hpp>

namespace mel {

//==============================================================================
// CLASS DECLARATION
//==============================================================================

template <typename T, typename M>
class ChannelBase {

public:

    /// Default constructor. Creates invalid channel
    ChannelBase() : module_(nullptr), channel_number_(-1) {}

    /// Creates a valid channel.
    ChannelBase(M* module, uint32 channel_number) :
        module_(module),
        channel_number_(channel_number) { }

    virtual ~ChannelBase() {}

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

    /// Sets the current value of the channel
    void set_value(T value) {
        module_->set_value(channel_number_, value);
    }

    /// Sets the current value of the channel
    void operator()(T value) {
        set_value(value);
    }

    /// Gets the channel number
    uint32 get_channel_number() const {
        return channel_number_;
    }

protected:

    M* module_;                    ///< Pointer to the module this channel is on
    const uint32 channel_number_;  ///< The channel number of this channel

};

} // namespace mel

//==============================================================================
// CLASS DOCUMENTATION
//==============================================================================
