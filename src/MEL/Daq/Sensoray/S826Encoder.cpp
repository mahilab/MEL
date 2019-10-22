#include <MEL/Daq/Sensoray/S826.hpp>
#include <MEL/Daq/Sensoray/S826Encoder.hpp>
#include <MEL/Logging/Log.hpp>
#include <windows.h>
#include <826api.h> 

namespace mel {

S826Encoder::S826Encoder(S826& s826) : 
    Encoder({ 0,1,2,3,4,5 }), 
    s826_(s826), 
    timestamps_(this),
    values_per_sec_(this),
    velocities_(this)
{
    set_name(s826.get_name() + "_encoder");
}

bool S826Encoder::on_open() {
    // program all counters to be incremental encoders
    int result;
    bool success = true;
    for (auto& c : get_channel_numbers()) {
        result = S826_CounterModeWrite(s826_.board_, c, S826_CM_K_QUADX4);
        if (result != S826_ERR_OK) {
            LOG(Error) << "Failed to writer counter mode of " << get_name() << " channel number " << c << " (" << S826::get_error_message(result) << ")";
            return false;
        }
        S826_CounterStateWrite(s826_.board_, c, 1);
        if (result != S826_ERR_OK) {
            LOG(Error) << "Failed to write counter state of " << get_name() << " channel number " << c << " (" << S826::get_error_message(result) << ")";
            return false;
        }
    }
    return success;
}

bool S826Encoder::update_channel(ChanNum channel_number) {
    uint32 count;
    uint32 timestamp;
    int result;
    result = S826_CounterSnapshot(s826_.board_, channel_number);
    if (result != S826_ERR_OK) {
        LOG(Error) << "Failed to trigger snapshot on " << get_name() << " channel number " << channel_number << " (" << S826::get_error_message(result) << ")";
        return false;
    }
    result = S826_CounterSnapshotRead(s826_.board_, channel_number, &count, &timestamp, NULL, 0);
    if (result != S826_ERR_OK) {
        LOG(Error) << "Failed to update " << get_name() << " channel number " << channel_number << " (" << S826::get_error_message(result) << ")";
        return false;
    }
    int32 last_count = values_[channel_number];
    int32 this_count = static_cast<int32>(count);
    Time last_time = timestamps_[channel_number];
    Time this_time = microseconds(timestamp);
    values_[channel_number] = this_count;
    timestamps_[channel_number] = this_time;
    values_per_sec_[channel_number] = static_cast<double>(this_count - last_count) / (this_time - last_time).as_seconds();
    return true;
}

bool S826Encoder::reset_count(ChanNum channel_number, int32 count) {
    uint32 ucount = (uint32)count;
    int result;
    result = S826_CounterPreloadWrite(s826_.board_, channel_number, 0, count);
    if (result != S826_ERR_OK) {
        LOG(Error) << "Failed to preload counts in to Preload0 register (" << S826::get_error_message(result) << ")";
        return false;
    }
    result = S826_CounterPreload(s826_.board_, channel_number, 1, 0);
    if (result != S826_ERR_OK) {
        LOG(Error) << "Failed to reset counts of " << get_name() << " channel number " << channel_number << " (" << S826::get_error_message(result) << ")";
        return false;
    }
    values_[channel_number] = count;
    return true;
}

bool S826Encoder::set_quadrature_factor(ChanNum channel_number, QuadFactor factor) {
    if (factor == QuadFactor::X1) {
        int result = S826_CounterModeWrite(s826_.board_, channel_number, S826_CM_K_QUADX1);
        if (result != S826_ERR_OK) {
            LOG(Error) << "Failed to set quadrature mode of " << get_name() << " channel number " << channel_number << " (" << S826::get_error_message(result) << ")";
            return false;
        }
    }
    else if (factor == QuadFactor::X2) {
        int result = S826_CounterModeWrite(s826_.board_, channel_number, S826_CM_K_QUADX2);
        if (result != S826_ERR_OK) {
            LOG(Error) << "Failed to set quadrature mode of " << get_name() << " channel number " << channel_number << " (" << S826::get_error_message(result) << ")";
            return false;
        }
    }
    else if (factor == QuadFactor::X4) {
        int result = S826_CounterModeWrite(s826_.board_, channel_number, S826_CM_K_QUADX4);
        if (result != S826_ERR_OK) {
            LOG(Error) << "Failed to set quadrature mode of " << get_name() << " channel number " << channel_number << " (" << S826::get_error_message(result) << ")";
            return false;
        }
    }
    else {
        LOG(Error) << "Unsupported quadrature factor requested of " << get_name();
        return false;
    }
    return true;
}

std::vector<double>& S826Encoder::get_values_per_sec() {
    return values_per_sec_.get();
}

double S826Encoder::get_value_per_sec(ChanNum channel_number) {
    if (validate_channel_number(channel_number))
        return values_per_sec_[channel_number];
    else
        return double();
}

const std::vector<double>& S826Encoder::get_velocities() {
    for (auto const& ch : get_channel_numbers())
        velocities_[ch] = values_per_sec_[ch] * conversions_[ch];
    return velocities_.get();
}

double S826Encoder::get_velocity(ChanNum channel_number) {
    if (validate_channel_number(channel_number)) {
        return values_per_sec_[channel_number] * conversions_[channel_number];
    }
    else
        return double();
}

Time S826Encoder::get_timestamp(ChanNum channel_number) {
    if (validate_channel_number(channel_number))
        return timestamps_[channel_number];
    return Time::Zero;
}

std::vector<Time>& S826Encoder::get_timestamps() {
    return timestamps_.get();
}

//=============================================================================

S826Encoder::Channel S826Encoder::get_channel(ChanNum channel_number) {
    if (validate_channel_number(channel_number))
        return Channel(this, channel_number);
    else
        return Channel();
}

std::vector<S826Encoder::Channel> S826Encoder::get_channels(const ChanNums& channel_numbers) {
    std::vector<Channel> channels;
    for (std::size_t i = 0; i < channel_numbers.size(); ++i)
        channels.push_back(get_channel(channel_numbers[i]));
    return channels;
}

S826Encoder::Channel S826Encoder::operator[](ChanNum channel_number) {
    return get_channel(channel_number);
}

std::vector<S826Encoder::Channel> S826Encoder::operator[](const ChanNums& channel_numbers) {
    return get_channels(channel_numbers);
}

S826Encoder::Channel::Channel() :
    Encoder::Channel()
{ }

S826Encoder::Channel::Channel(S826Encoder* module, ChanNum channel_number) :
    Encoder::Channel(module, channel_number)
{ }

double S826Encoder::Channel::get_value_per_sec() {
    return static_cast<S826Encoder*>(module_)->get_value_per_sec(channel_number_);
}

double S826Encoder::Channel::get_velocity() {
    velocity_ = static_cast<S826Encoder*>(module_)->get_velocity(channel_number_);
    return velocity_;
}

Time S826Encoder::Channel::get_timestamp() {
    return static_cast<S826Encoder*>(module_)->get_timestamp(channel_number_);
}

}