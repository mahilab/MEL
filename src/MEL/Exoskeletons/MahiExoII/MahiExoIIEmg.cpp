#include <MEL/Exoskeletons/MahiExoII/MahiExoIIEmg.hpp>
#include <MEL/Utility/Console.hpp>

namespace mel {

MahiExoIIEmg::MahiExoIIEmg(MeiiConfiguration configuration, MeiiParameters parameters) :
    MahiExoII(configuration, parameters),
    emg_channel_numbers_(configuration.daq_.get_module<AnalogInput>().get_channel_numbers()),
    emg_channel_count_(configuration.daq_.get_module<AnalogInput>().get_channel_count()),
    mes_raw_(emg_channel_count_, 0.0),
    mes_demean_(emg_channel_count_, 0.0),
    mes_envelope_(emg_channel_count_, 0.0),
    mes_tkeo_envelope_(emg_channel_count_, 0.0),
    mes_tkeo_env_window_(emg_channel_count_),
    mes_buffer_capacity_(500),
    mes_feature_window_size_(200)
{
    
    for (int i = 0; i < emg_channel_count_; ++i) {
        emg_electrodes_.emplace_back(config_.ai_channels_[i]);
        emg_electrodes_[i].resize_mes_buffer(mes_buffer_capacity_);      
    }
    all_features_ = std::vector<double>(emg_channel_count_ * emg_electrodes_[0].get_all_features_count(), 0.0);
}

void MahiExoIIEmg::update_emg() {
    
    for (std::size_t i = 0; i < emg_channel_count_; ++i) {       
        emg_electrodes_[i].update_and_buffer();        
        mes_raw_[i] = emg_electrodes_[i].get_mes_raw();
        mes_demean_[i] = emg_electrodes_[i].get_mes_demean();
        mes_envelope_[i] = emg_electrodes_[i].get_mes_envelope();
        mes_tkeo_envelope_[i] = emg_electrodes_[i].get_mes_tkeo_envelope();

    }
    //uint16 i;
    //for (auto it = emg_electrodes_.begin(); it != emg_electrodes_.end(); ++it) {
    //    emg_voltage = (double) it->get_raw_voltage();
    //    i = std::distance(emg_electrodes_.begin(), it);
    //    if (emg_signal_monitor_) {
    //        if (emg_voltage < -std::abs(emg_voltage_max_) || emg_voltage > std::abs(emg_voltage_max_)) {
    //            std::cout << "WARNING: EMG voltage outside of expected range for channel " + std::to_string(i) + " with a value of " + std::to_string(emg_voltage) << std::endl;
    //        }
    //    }
    //    emg_voltages_[i] = emg_voltage;               
    //}
}

const std::vector<double>& MahiExoIIEmg::get_mes_raw() const {
    return mes_raw_;
}

const std::vector<double>& MahiExoIIEmg::get_mes_dm() const {
    return mes_demean_;
}

const std::vector<double>& MahiExoIIEmg::get_mes_env() const {
    return mes_envelope_;
}

const std::vector<double>& MahiExoIIEmg::get_mes_tkeo_env() const {
    return mes_tkeo_envelope_;
}

const std::vector<std::vector<double>>& MahiExoIIEmg::get_mes_tkeo_env_buffer_data(std::size_t window_size) {
    for (std::size_t i = 0; i < emg_channel_count_; ++i) {
        mes_tkeo_env_window_[i] = emg_electrodes_[i].get_mes_tkeo_env_buffer_data(window_size);
    }
    return mes_tkeo_env_window_;
}

std::vector<uint32> MahiExoIIEmg::get_emg_channel_numbers() const {
    return emg_channel_numbers_;
}

std::size_t MahiExoIIEmg::get_emg_channel_count() const {
    return emg_channel_count_;
}

void MahiExoIIEmg::resize_mes_buffer(std::size_t capacity) {
    mes_buffer_capacity_ = capacity;
    for (std::size_t i = 0; i < emg_channel_count_; ++i) {
        emg_electrodes_[i].resize_mes_buffer(mes_buffer_capacity_);
    }
}

bool MahiExoIIEmg::is_mes_buffer_full() {
    bool buffer_full = true;
    for (std::size_t i = 0; i < emg_channel_count_; ++i) {
        if (!emg_electrodes_[i].is_buffer_full())
            buffer_full = false;
    }
    return buffer_full;
}

void MahiExoIIEmg::reset_emg_signal_processing() {
    for (std::size_t i = 0; i < emg_channel_count_; ++i) {
        emg_electrodes_[i].reset_signal_processing();
    }
}

void MahiExoIIEmg::compute_all_features() {
    for (std::size_t i = 0; i < emg_channel_count_; ++i) {
        emg_electrodes_[i].compute_all_features(mes_feature_window_size_);
    }
}

const std::vector<double>& MahiExoIIEmg::get_all_features() {
    std::vector<double> rms_features(emg_channel_count_ * emg_electrodes_[0].get_rms_features_count());
    std::vector<double> hudgins_td_features(emg_channel_count_ * emg_electrodes_[0].get_hudgins_td_features_count());
    std::vector<double> ar_features(emg_channel_count_ * emg_electrodes_[0].get_ar_features_count());
    for (std::size_t i = 0; i < emg_channel_count_; ++i) {
        for (std::size_t j = 0; j < emg_electrodes_[0].get_rms_features_count(); ++j) {
            rms_features[j*emg_channel_count_ + i] = emg_electrodes_[i].get_rms_features()[j];
        }
        for (std::size_t j = 0; j < emg_electrodes_[0].get_hudgins_td_features_count(); ++j) {
            hudgins_td_features[j*emg_channel_count_ + i] = emg_electrodes_[i].get_hudgins_td_features()[j];
        }
        for (std::size_t j = 0; j < emg_electrodes_[0].get_ar_features_count(); ++j) {
            ar_features[j*emg_channel_count_ + i] = emg_electrodes_[i].get_ar_features()[j];
        }
    }
    auto it = std::copy(rms_features.begin(), rms_features.end(), all_features_.begin());
    it = std::copy(hudgins_td_features.begin(), hudgins_td_features.end(), it);
    it = std::copy(ar_features.begin(), ar_features.end(), it);
    return all_features_;
}

std::size_t MahiExoIIEmg::get_all_features_count() const {
    return emg_electrodes_[0].get_all_features_count();
}

std::size_t MahiExoIIEmg::get_mes_buffer_capacity() const {
    return mes_buffer_capacity_;
}

} // namespace mel