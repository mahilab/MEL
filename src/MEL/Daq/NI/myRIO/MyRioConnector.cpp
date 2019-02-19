#include <MEL/Daq/NI/MyRio/MyRioConnector.hpp>
#include <MEL/Daq/NI/MyRio/MyRio.hpp>
#include "Detail/MyRioUtil.hpp"
#include "Detail/MyRioFpga60/MyRio.h"
#include <MEL/Logging/Log.hpp>

namespace mel
{

namespace {
std::string connectorName(MyRioConnector::Type type) {
    if (type == MyRioConnector::Type::MxpA)
        return "MxpA";
    else if (type == MyRioConnector::Type::MxpB)
        return "MxpB";
    else if (type == MyRioConnector::Type::MspC)
        return "MspC";
    else
        return "Unkown";
}

}
    
MyRioConnector::MyRioConnector(MyRio& myrio,
                            Type _type,
                            const ChanNums& ai_channels,
                            const ChanNums& ao_channels,
                            const ChanNums& dio_channels,
                            const ChanNums& enc_channels) :
    DaqBase(myrio.get_name() + "_" + connectorName(_type)),
    type(_type),
    AI(*this, ai_channels),
    AO(*this, ao_channels),
    DIO(*this, dio_channels),
    encoder(*this, enc_channels),
    myrio_(myrio)
{
    
}

bool MyRioConnector::update_input() {
    return AI.update() && DIO.update() && encoder.update();
}

bool MyRioConnector::update_output() {
    return AO.update() && DIO.update();
}

void MyRioConnector::reconfigure_dios() {
    ChanNums channels;
    auto num_encoders = encoder.get_channel_count();
    if (type == Type::MxpA || type == Type::MxpB) {
        if (num_encoders == 1)
            channels = {0,1,2,3,4,5,6,7,8,9,10,13,14,15};
        else
            channels = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
    }
    else if (type == Type::MspC) {
        if (num_encoders == 2)
            channels = {1,3,5,7};
        else if (num_encoders == 1)
            channels = {1,3,4,5,6,7};
        else
            channels = {0,1,2,3,4,5,6,7};
    }
    DIO.set_channel_numbers(channels);
}

bool MyRioConnector::on_open() {
    encoder.sync_from_myrio();
    reconfigure_dios();
    return true;
}

bool MyRioConnector::on_close() {
    return true;
}

bool MyRioConnector::on_enable() {
    if (AI.enable() && AO.enable() && DIO.enable() && encoder.enable())
        return true;
    else
        return false;
}

bool MyRioConnector::on_disable() {
    if (AI.disable() && AO.disable() && DIO.disable() && encoder.disable())
        return true;
    else
        return false;
}

} // mel