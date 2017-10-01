#include "MahiExoIIEmg.h"

namespace mel {

    namespace exo {

        MahiExoIIEmg::MahiExoIIEmg(Config configuration, Params parameters) :
            MahiExoII(configuration, parameters),
            emg_voltages_(double_vec(N_emg_, 0.0))
        {
            for (int i = 0; i < N_emg_; ++i) {
                std::string num = std::to_string(i);
                emg_electrodes_[i] = core::EmgElectrode("emg_electrod_" + num, configuration.emg_[i]);
            }
        }

        double_vec MahiExoIIEmg::get_emg_voltages() {
            for (auto it = emg_electrodes_.begin(); it != emg_electrodes_.end(); ++it) {
                emg_voltages_[std::distance(emg_electrodes_.begin(), it)] = it->get_voltage();
            }
            return emg_voltages_;
        }

    }

}