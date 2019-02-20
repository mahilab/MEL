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

static const std::vector<ChanNums> AI_CHANNELS({
    {0,1,2,3},
    {0,1,2,3},
    {0,1}
});

static const std::vector<ChanNums> AO_CHANNELS({
    {0,1},
    {0,1},
    {0,1}
});

static const std::vector<ChanNums> DIO_CHANNELS({
    {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15},
    {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15},
    {0,1,2,3,4,5,6,7}
});

} // namespace
    
MyRioConnector::MyRioConnector(MyRio& myrio,Type _type) :
    DaqBase(myrio.get_name() + "_" + connectorName(_type)),
    type(_type),
    AI(*this, AI_CHANNELS[type]),
    AO(*this, AO_CHANNELS[type]),
    DIO(*this, DIO_CHANNELS[type]),
    encoder(*this, {}),
    myrio_(myrio)
{   

}

bool MyRioConnector::update_input() {
    return AI.update() && DIO.update_input() && encoder.update();
}

bool MyRioConnector::update_output() {
    return AO.update() && DIO.update_output();
}

void MyRioConnector::reset() {
    for (auto& ch : encoder.get_channel_numbers())
        encoder.disable_channel(ch);
    DIO.sync();
}

bool MyRioConnector::on_open() {
    encoder.sync();
    DIO.sync();
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