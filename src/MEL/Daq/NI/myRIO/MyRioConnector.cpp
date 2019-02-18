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

static const std::vector<uint32_t> SYSSELECT({SYSSELECTA, SYSSELECTB, SYSSELECTC});

// encoder configure registers
static const std::vector<std::vector<uint32_t>> ENCCNFG ({
    {ENCACNFG},
    {ENCBCNFG},
    {ENCC_0CNFG, ENCC_1CNFG}
});

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
{
    
}

bool MyRioConnector::update_input() {
    return AI.update() && DIO.update() && encoder.update();
}

bool MyRioConnector::update_output() {
    return AO.update() && DIO.update();
}

bool MyRioConnector::configure_encoders(std::size_t encoder_count) {
    if (!is_open()) {
        LOG(Error) << "myRIO must be open to configure Encoders";
        return false; 
    }
    // MXP Connectors
    if (type == Type::MxpA || type == Type::MxpB) {
        // check requested count
        if (encoder_count > 1) {
            LOG(Warning) << "myRIO MXP connectors support a maximum of 1 encoders (" << encoder_count << " requested)";
            encoder_count = 1;
        }
        // enable encoder
        if (encoder_count == 1) {
            set_register_bit(SYSSELECT[type], 5);
            encoder.set_channel_numbers({0});
        }
        // disable encoder
        else if (encoder_count == 0) {
            clr_register_bit(SYSSELECT[type], 5);
            encoder.set_channel_numbers({});
        }
    }
    // MSP Connector
    else if (type == Type::MspC) {
        // check requested count
        if (encoder_count > 2) {
            LOG(Warning) << "myRIO MSP connectors support a maximum of 2 encoders (" << encoder_count << " requested)";
            encoder_count = 2;
        }
        // enable 2 encoders
        if (encoder_count == 2) {
            set_register_bit(SYSSELECT[type], 0);
            set_register_bit(SYSSELECT[type], 2);
            encoder.set_channel_numbers({0,1});
        }
        else if (encoder_count == 1) {
            set_register_bit(SYSSELECT[type], 0);
            clr_register_bit(SYSSELECT[type], 2);
            encoder.set_channel_numbers({0});
        }
        else if (encoder_count == 0) {
            clr_register_bit(SYSSELECT[type], 0);
            clr_register_bit(SYSSELECT[type], 2);
            encoder.set_channel_numbers({});
        }
    }

    // configure and enable each encoder
    for (auto &c : encoder.get_channel_numbers()) {
        clr_register_bit(ENCCNFG[type][c], 2); // set to quadrature mode
        set_register_bit(ENCCNFG[type][c], 0); // enable encoder
    }

    LOG(Verbose) << "Configured myRIO Connector " << get_name() << " for Encoders channels [" << encoder.get_channel_numbers() << "]"; 
    // reconfigure DIO channels
    reconfigure_dios();
    return true;
}

void MyRioConnector::reconfigure_dios() {
    std::vector<uint32> channels;
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
    LOG(Verbose) << "Configured myRIO Connector " << get_name() << " for DIO channels [" << DIO.get_channel_numbers() << "]";
}

bool MyRioConnector::on_open() {
    // determine how many encoders are enabled and reconfigure
    if (type == Type::MxpA || type == Type::MxpB) {
        if (get_register_bit(SYSSELECT[type], 5))
            encoder.set_channel_numbers({0});
    }
    else if (type == Type::MspC) {
        if (get_register_bit(SYSSELECT[type], 0) && get_register_bit(SYSSELECT[type], 2))
            encoder.set_channel_numbers({0,1});
        else if (get_register_bit(SYSSELECT[type], 0))
            encoder.set_channel_numbers({0});
        else
            encoder.set_channel_numbers({});
    }
    // configure and enable each encoder
    for (auto &c : encoder.get_channel_numbers()) {
        clr_register_bit(ENCCNFG[type][c], 2); // set to quadrature mode
        set_register_bit(ENCCNFG[type][c], 0); // enable encoder
    }
    // reconfigure DIO channels
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