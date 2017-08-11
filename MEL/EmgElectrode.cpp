#include "EmgElectrode.h"

namespace mel {

    EmgElectrode::EmgElectrode() :
        name_("no_name"),
        voltage_(0.0)
    { }

    EmgElectrode::EmgElectrode(std::string name, Daq::Ai ai_channel) :
        name_(name),
        ai_channel_(ai_channel),
        voltage_(0.0)
    { }

    double EmgElectrode::get_voltage() {

        voltage_ = ai_channel_.get_voltage();

        return voltage_;
    }
}