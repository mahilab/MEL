#include "MahiExoIIEmg.h"

MahiExoIIEmg::MahiExoIIEmg(Config configuration, Params parameters) :
    MahiExoII(configuration,parameters),
    emg_voltages_(mel::double_vec(8,0))
{
    for (int i = 0; i < 8; ++i) {

        std::string num = std::to_string(i);

        emg_electrodes_.push_back(new mel::EmgElectrode("emg_electrode_" + num, configuration.emg_[i]));
    }
}

mel::double_vec MahiExoIIEmg::get_emg_voltages() {
    mel::double_vec emg_voltages;
    emg_voltages.reserve(emg_electrodes_.size());
    for (auto it = emg_electrodes_.begin(); it != emg_electrodes_.end(); ++it) {
        emg_voltages.push_back((*it)->get_voltage());
    }
    return emg_voltages_ = emg_voltages;
}