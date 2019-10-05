#include <MEL/Daq/NI/MyRio/MyRioI2C.hpp>
#include <MEL/Daq/NI/MyRio/MyRioConnector.hpp>
#include "Detail/MyRioUtil.hpp"
#include "Detail/MyRioFpga60/MyRio.h"
#include <MEL/Logging/Log.hpp>

extern NiFpga_Session myrio_session;

namespace mel
{

MyRioI2C::MyRioI2C(MyRioMxp& connector) :
    Device(connector.get_name() + "_i2c"),
    connector_(connector)
{
    // get registers
    if (connector_.type == MyRioConnector::MxpA) {
        sysselect_ = SYSSELECTA;
        cnfg_ = I2CACNFG;
        addr_ = I2CAADDR;
        cntr_ = I2CACNTR;
        dato_ = I2CADATO;
        dati_ = I2CADATI;
        stat_ = I2CASTAT;
        ctrl_ = I2CACNTL;
        go_   = I2CAGO;
    }
    else if (connector_.type == MyRioConnector::MxpB) {
        sysselect_ = SYSSELECTB;
        cnfg_ = I2CBCNFG;
        addr_ = I2CBADDR;
        cntr_ = I2CBCNTR;
        dato_ = I2CBDATO;
        dati_ = I2CBDATI;
        stat_ = I2CBSTAT;
        ctrl_ = I2CBCNTL;
        go_   = I2CBGO;
    }  
}

void MyRioI2C::set_slave(const std::string& address) {
    if (address.length() != 7)
        LOG(Error) << "I2C slave address must be a 7 digit binary value (e.g. \"0110010\")";
    else {
        std::bitset<7> bits(address);
        set_slave(bits);
    }
}

void MyRioI2C::set_slave(std::bitset<7> address) {
    auto bits = read_register(addr_);
    for (std::size_t i = 0; i < 7; ++i)
        bits[i+1] = address[i];
    write_register(addr_, bits);    
}

void MyRioI2C::send(const void* data, std::size_t size) {
    // set the first address bit to zero to indicate next operatin will be "send"
    clr_register_bit(addr_, 0);
    const unsigned char* bytes = static_cast<const unsigned char*>(data);
    for (std::size_t b = 0; b < size; ++b) {
        write_register(dato_, bytes[b]);
    }
}

void MyRioI2C::execute() {
    NiFpga_WriteBool(myrio_session, go_, 1);
    auto status = read_register(stat_);
    bool bsy = true;
    while (bsy) {
        status = read_register(stat_);
        bsy = status[0];
    }
    bool err = status[1];
    if (err)
        LOG(Error) << "Error occured during MyRio I2C execute operation.";
}

bool MyRioI2C::on_enable() {
    set_register_bit(sysselect_, 7);
    connector_.DIO.sync();
    return true;
}

bool MyRioI2C::on_disable() {
    clr_register_bit(sysselect_, 7);
    connector_.DIO.sync();
    return true;
}

void MyRioI2C::sync() {
    if (get_register_bit(sysselect_, 7))
        enabled_  = true;
    else
        enabled_  = false;
}

} // namespace mel
