#include <MEL/Daq/NI/MyRio/MyRioConnector.hpp>
#include <MEL/Daq/NI/MyRio/MyRio.hpp>

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
                            const std::vector<uint32>& ai_channels,
                            const std::vector<uint32>& ao_channels,
                            const std::vector<uint32>& dio_channels,
                            const std::vector<uint32>& enc_channels) :
    DaqBase(myrio.get_name() + "_" + connectorName(_type)),
    type(_type),
    AI(*this, ai_channels),
    AO(*this, ao_channels),
    DIO(*this, dio_channels),
    encoder(*this, enc_channels),
    myrio_(myrio)
{}

bool MyRioConnector::update_input() {
    return AI.update() && DIO.update() && encoder.update();
}

bool MyRioConnector::update_output() {
    return AO.update() && DIO.update();
}

bool MyRioConnector::on_open() {
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
