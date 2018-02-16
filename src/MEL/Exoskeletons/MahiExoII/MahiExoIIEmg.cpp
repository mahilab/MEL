#include "MEL/Exoskeletons/MahiExoII/MahiExoIIEmg.hpp"

namespace mel {

MahiExoIIEmg::MahiExoIIEmg(MeiiConfiguration configuration, MeiiParameters parameters) :
    MahiExoII(configuration, parameters),
    emg_channel_numbers_(configuration.daq_.get_module<AnalogInput>().get_channel_numbers()),
    emg_channel_count_(configuration.daq_.get_module<AnalogInput>().get_channel_count()),
    mes_raw_(emg_channel_count_, 0.0),
    mes_demean_(emg_channel_count_, 0.0),
    mes_envelope_(emg_channel_count_, 0.0),
    mes_tkeo_envelope_(emg_channel_count_, 0.0)
{
    for (int i = 0; i < emg_channel_count_; ++i) {
        emg_electrodes_.emplace_back(config_.ai_channels_[emg_channel_numbers_[i]]);
    }
}

void MahiExoIIEmg::update_emg() {
    for (size_t i = 0; i < emg_channel_count_; ++i) {
        emg_electrodes_[i].update();
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

const std::vector<double>& MahiExoIIEmg::get_mes_demean() const {
    return mes_demean_;
}

const std::vector<double>& MahiExoIIEmg::get_mes_env() const {
    return mes_envelope_;
}

const std::vector<double>& MahiExoIIEmg::get_mes_tkeo_env() const {
    return mes_tkeo_envelope_;
}

size_t MahiExoIIEmg::get_emg_channel_count() const {
    return emg_channel_count_;
}

const std::vector<uint32>& MahiExoIIEmg::get_emg_channel_numbers() const {
    return emg_channel_numbers_;
}




MahiExoIIEmg::EmgDataBuffer::EmgDataBuffer(size_t num_channels, size_t length) :
    num_channels_(num_channels),
    length_(length)
{
    for (size_t i = 0; i < num_channels_; ++i) {
        data_buffer_.push_back(RingBuffer<double>(length_));
    }
}

void MahiExoIIEmg::EmgDataBuffer::push_back(std::vector<double> data_vec) {
    if (data_vec.size() != num_channels_) {
        print("ERROR: Incorrect number of rows when calling EmgDataBuffer::push_back().");
    }
    for (int i = 0; i < num_channels_; ++i) {
        data_buffer_[i].push_back(data_vec[i]);
    }
}

void MahiExoIIEmg::EmgDataBuffer::clear() {
    for (int i = 0; i < num_channels_; ++i) {
        data_buffer_[i].clear();
    }
}

std::vector<double> MahiExoIIEmg::EmgDataBuffer::at(int index) const {
    std::vector<double> data_vec(num_channels_);
    for (int i = 0; i < num_channels_; ++i) {
        data_vec[i] = data_buffer_[i][index];
    }
    return data_vec;
}

std::vector<double> MahiExoIIEmg::EmgDataBuffer::get_channel(int index) const {
    std::vector<double> channel_vec(length_);
    for (int i = 0; i < length_; ++i) {
        channel_vec[i] = data_buffer_[index][i];
    }
    return channel_vec;
}




} // mel