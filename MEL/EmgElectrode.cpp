#include "EmgElectrode.h"

namespace mel {

    namespace core {

        EmgElectrode::EmgElectrode() :
            Device("emg_electrode"),
            voltage_(0.0)
        { }

        EmgElectrode::EmgElectrode(std::string name, Daq::Ai ai_channel) :
            Device(name),
            ai_channel_(ai_channel),
            voltage_(0.0)
        { }

        double EmgElectrode::get_voltage() {

            voltage_ = ai_channel_.get_voltage();

            return voltage_;
        }

    }
}