#include "MEL/Exoskeletons/MahiExoII/MahiExoIIEmg.hpp"

namespace mel {

MahiExoIIEmg::MahiExoIIEmg(MeiiConfiguration configuration, MeiiParameters parameters) :
    MahiExoII(configuration, parameters),
    emg_voltages_(std::vector<double>(N_emg_, 0.0))
{
    //emg_electrodes_.reserve(N_emg_);
    //
    //for (int i = 0; i < N_emg_; ++i) {
    //    emg_electrodes_.push_back(EmgSignal(config_.ai_channels_[i]));
    //}
}

std::vector<double> MahiExoIIEmg::get_emg_voltages() {
    double emg_voltage;
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
    return emg_voltages_;
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

MahiExoIIEmg::TeagerKaiserOperator::TeagerKaiserOperator() :
    tko_implementations_(std::vector<TeagerKaiserOperatorImplementation>(length_))
{ }

void MahiExoIIEmg::TeagerKaiserOperator::tkeo(const double& x, double& y) {
    if (length_ != 1) {
        std::cout << "ERROR: Input vector does not match size of TeagerKaiserOperator." << std::endl;
        return;
    }
    tko_implementations_[0].tkeo(x, y);
}

void MahiExoIIEmg::TeagerKaiserOperator::tkeo(const std::vector<double>& x, std::vector<double>& y) {
    if (x.size() != length_) {
        std::cout << "ERROR: Input vector does not match size of TeagerKaiserOperator." << std::endl;
        return;
    }
    for (int i = 0; i < length_; ++i) {
        tko_implementations_[i].tkeo(x[i], y[i]);
        if (tkeo_signal_monitor_) {
            if (y[i] < -std::abs(tkeo_max_) || y[i] > std::abs(tkeo_max_)) {
                std::cout << "WARNING: TKEO value outside of expected range for channel " + std::to_string(i) + " with a value of " + std::to_string(y[i]) << std::endl;
            }
        }
    }
}

void MahiExoIIEmg::TeagerKaiserOperator::reset() {
    for (int i = 0; i < length_; ++i) {
        tko_implementations_[i].reset();
    }
}

        

MahiExoIIEmg::TeagerKaiserOperator::TeagerKaiserOperatorImplementation::TeagerKaiserOperatorImplementation() :
    s_(std::vector<double>(n_, 0.0))        
{ }

void MahiExoIIEmg::TeagerKaiserOperator::TeagerKaiserOperatorImplementation::tkeo(const double& x, double& y) {
    y = s_[0] * s_[0] - x * s_[1];
    s_[0] = x;
    s_[1] = s_[0];
}

void MahiExoIIEmg::TeagerKaiserOperator::TeagerKaiserOperatorImplementation::reset() {
    s_ = std::vector<double>(n_, 0.0);
}


} // namespace MEL