#include <MEL/Daq/Sensoray/S826.hpp>
#include <MEL/Daq/Sensoray/S826Encoder.hpp>
#include <MEL/Logging/Log.hpp>
#include <windows.h>
#include <826api.h> 

namespace mel {

S826Encoder::S826Encoder(S826& s826) : Encoder({ 0,1,2,3,4,5 }), s826_(s826), timestamps_(this)
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
    //int result = S826_CounterRead(s826_.board_, channel_number, &count);
    if (result != S826_ERR_OK) {
        LOG(Error) << "Failed to update " << get_name() << " channel number " << channel_number << " (" << S826::get_error_message(result) << ")";
        return false;
    }
    values_[channel_number] = static_cast<int32>(count);
    timestamps_[channel_number] = microseconds(timestamp);
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

}