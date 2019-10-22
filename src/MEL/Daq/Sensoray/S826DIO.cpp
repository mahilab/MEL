#include <MEL/Daq/Sensoray/S826.hpp>
#include <MEL/Daq/Sensoray/S826DIO.hpp>
#include <MEL/Logging/Log.hpp>
#include <windows.h>
#include <826api.h> 
#include <bitset>

namespace mel {
    S826DIO::S826DIO(S826& s826) : 
        DigitalInputOutput({ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
                            10,11,12,13,14,15,16,17,18,19,
                            20,21,22,23,24,25,26,27,28,29,
                            30,31,32,33,34,35,36,37,38,39,
                            40,41,42,43,44,45,46,47}),
        s826_(s826_) 
    {

    }

    bool S826DIO::update() {
        return true;
    }

    bool S826DIO::update_channel(ChanNum channel_number) {
        return true;
    }

    bool S826DIO::set_direction(ChanNum channel_number, Direction direction) {
        return true;
    }

    bool S826DIO::on_open() {
        return true;
    }
}