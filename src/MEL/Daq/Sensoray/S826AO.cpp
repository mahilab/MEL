#include <MEL/Daq/Sensoray/S826.hpp>
#include <MEL/Daq/Sensoray/S826AO.hpp>
#include <MEL/Logging/Log.hpp>
#include <windows.h>
#include <826api.h> 

namespace mel {

S826AO::S826AO(S826& s826) : 
    AnalogOutput({0,1,2,3,4,5,6,7}),
    s826_(s826)
{ 
    set_name(s826_.get_name() + "_AO");
    // program all channels to be -10 to 10 V
    int result;
    for (auto& c : get_channel_numbers()) {
        result = S826_DacRangeWrite(s826_.board_, c, 3, 0);
        if (result != S826_ERR_OK) {
            LOG(Error) << "Failed to set " << get_name() << " channel number " << c << " output range (" << S826::get_error_message(result) << ")";
        }
    }
}

bool S826AO::update_channel(ChanNum channel_number) {
    uint32 setpoint = (uint)(values_[channel_number] * 0xFFFF / 20) + 0x8000; // -10V to +10V
    int result = S826_DacDataWrite(s826_.board_, channel_number, setpoint, 0);
    if (result != S826_ERR_OK) {
        LOG(Error) << "Failed to update " << get_name() << " channel number " << channel_number << " (" << S826::get_error_message(result) << ")";
        return false;
    }
    return true;
}


}