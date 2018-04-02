#include <MEL/Core/EmgElectrode.hpp>
#include <MEL/Logging/Log.hpp>
#include <MEL/Utility/Console.hpp>

namespace mel {

EmgElectrode::EmgElectrode( AnalogInput::Channel ai_channel, std::size_t mes_buffer_capacity ) :
    ai_channel_(ai_channel),
    mes_raw_(0.0),
    mes_demean_(0.0),
    mes_envelope_(0.0),
    mes_tkeo_envelope_(0.0),
    mes_buffer_capacity_(mes_buffer_capacity),
    mes_dm_buffer_(mes_buffer_capacity_),
    mes_dm_window_(mes_buffer_capacity_),
    mes_env_buffer_(mes_buffer_capacity_),
    mes_env_window_(mes_buffer_capacity_),
    mes_tkeo_env_buffer_(mes_buffer_capacity_),
    mes_tkeo_env_window_(mes_buffer_capacity_),
    rms_features_count_(1),
    hudgins_td_features_count_(4),
    ar_features_count_(4),
    all_features_count_(rms_features_count_ + hudgins_td_features_count_ + ar_features_count_),
    all_features_(all_features_count_, 0.0),
    rms_features_(rms_features_count_, 0.0),
    hudgins_td_features_(hudgins_td_features_count_, 0.0),
    ar_features_(ar_features_count_, 0.0),
    hp_filter_(Butterworth(4, 0.05, Butterworth::Highpass, 100)), // 4th-order Butterworth High-Pass at 0.05 normalized cutoff frequency
    lp_filter_(Butterworth(4, 0.01, Butterworth::Lowpass, 100)), // 4th-order Butterworth Low-Pass at 0.01 normalized cutoff frequency
    tkeo_lp_filter_(Butterworth(4, 0.01, Butterworth::Lowpass, 100)) // 4th-order Butterworth Low-Pass at 0.01 normalized cutoff frequency
    { }

void EmgElectrode::update() {
    mes_raw_ = ai_channel_.get_value();  
    mes_demean_ = hp_filter_.update(mes_raw_);
    mes_envelope_ = lp_filter_.update(rect_.update(mes_demean_));
    mes_tkeo_envelope_ = tkeo_lp_filter_.update(tkeo_rect_.update(tkeo_.update(mes_demean_)));
    
}

void EmgElectrode::update_and_buffer() {
    update();
    push_mes_buffer();
}

void EmgElectrode::clear_mes_buffer() {
    mes_dm_buffer_.clear();
    mes_env_buffer_.clear();
    mes_tkeo_env_buffer_.clear();
}

void EmgElectrode::resize_mes_buffer(std::size_t capacity) {
    mes_buffer_capacity_ = capacity;
    mes_dm_buffer_.resize(mes_buffer_capacity_);
    mes_env_buffer_.resize(mes_buffer_capacity_);
    mes_tkeo_env_buffer_.resize(mes_buffer_capacity_);
}

bool EmgElectrode::is_buffer_full() {
    return mes_dm_buffer_.full();
}

void EmgElectrode::reset_signal_processing() {
    hp_filter_.reset();
    rect_.reset();
    lp_filter_.reset();
    tkeo_.reset();
    tkeo_rect_.reset();
    tkeo_lp_filter_.reset();
}

double EmgElectrode::get_mes_raw() const {
    return mes_raw_;
}

double EmgElectrode::get_mes_demean() const {
    return mes_demean_;
}

double EmgElectrode::get_mes_envelope() const {
    return mes_envelope_;
}

double EmgElectrode::get_mes_tkeo_envelope() const {
    return mes_tkeo_envelope_;
}

const std::vector<double>& EmgElectrode::get_mes_dm_buffer_data(std::size_t window_size) {
    if (mes_dm_buffer_.size() < window_size) {
        LOG(Warning) << "EMG electrode on channel " << ai_channel_.get_channel_number() << " cannot return requested amount of buffer data until buffer is of size " << window_size << ". Returning vector of size " << mes_dm_buffer_.size() << ".";
        window_size = mes_dm_buffer_.size();
    }   
    mes_dm_window_.resize(window_size);
    for (std::size_t i = 0; i < window_size; ++i) {
        mes_dm_window_[i] = mes_dm_buffer_[i + mes_dm_buffer_.size() - window_size];
    }
    return mes_dm_window_;
}

const std::vector<double>& EmgElectrode::get_mes_env_buffer_data(std::size_t window_size) {
    if (mes_env_buffer_.size() < window_size) {
        LOG(Warning) << "EMG electrode on channel " << ai_channel_.get_channel_number() << " cannot return requested amount of buffer data until buffer is of size " << window_size << ". Returning vector of size " << mes_env_buffer_.size() << ".";
        window_size = mes_env_buffer_.size();
    }   
    mes_env_window_.resize(window_size);
    for (std::size_t i = 0; i < window_size; ++i) {
        mes_env_window_[i] = mes_env_buffer_[i + mes_env_buffer_.size() - window_size];
    }
    return mes_env_window_;
}

const std::vector<double>& EmgElectrode::get_mes_tkeo_env_buffer_data(std::size_t window_size) {
    if (mes_tkeo_env_buffer_.size() < window_size) {
        LOG(Warning) << "EMG electrode on channel " << ai_channel_.get_channel_number() << " cannot return requested amount of buffer data until buffer is of size " << window_size << ". Returning vector of size " << mes_tkeo_env_buffer_.size() << ".";
        window_size = mes_tkeo_env_buffer_.size();
    }   
    mes_tkeo_env_window_.resize(window_size);
    for (std::size_t i = 0; i < window_size; ++i) {
        mes_tkeo_env_window_[i] = mes_tkeo_env_buffer_[i + mes_tkeo_env_buffer_.size() - window_size];
    }
    return mes_tkeo_env_window_;
}


std::size_t EmgElectrode::get_mes_buffer_capacity() const {
    return mes_buffer_capacity_;
}

void EmgElectrode::push_mes_buffer() {
    mes_dm_buffer_.push_back(mes_demean_);
    mes_env_buffer_.push_back(mes_envelope_);
    mes_tkeo_env_buffer_.push_back(mes_tkeo_envelope_);
}

void EmgElectrode::compute_all_features(std::size_t window_size) {
    if (mes_dm_buffer_.size() < window_size) {
        LOG(Warning) << "EMG electrode on channel " << ai_channel_.get_channel_number() << " cannot compute features until buffer is of size " << window_size << ".";
        return;
    }
    mes_dm_window_.resize(window_size);
    for (std::size_t i = 0; i < window_size; ++i) {
        mes_dm_window_[i] = mes_dm_buffer_[i + mes_dm_buffer_.size() - window_size];
    }
    rms_features_[0] = mean_rms(mes_dm_window_);
    hudgins_td_features_[0] = mean_absolute_value(mes_dm_window_);
    hudgins_td_features_[1] = wavelength(mes_dm_window_);
    hudgins_td_features_[2] = zero_crossings(mes_dm_window_);
    hudgins_td_features_[3] = slope_sign_changes(mes_dm_window_);
    auto_regressive_coefficients(ar_features_, mes_dm_window_);

}

void EmgElectrode::compute_rms_features(std::size_t window_size) {
    if (mes_dm_buffer_.size() < window_size) {
        LOG(Warning) << "EMG electrode on channel " << ai_channel_.get_channel_number() << " cannot compute features until buffer is of size " << window_size << ".";
        return;
    }
    mes_dm_window_.resize(window_size);
    for (std::size_t i = 0; i < window_size; ++i) {
        mes_dm_window_[i] = mes_dm_buffer_[i + mes_dm_buffer_.size() - window_size];
    }
    rms_features_[0] = mean_rms(mes_dm_window_);
}

void EmgElectrode::compute_hudgins_td_features(std::size_t window_size) {
    if (mes_dm_buffer_.size() < window_size) {
        LOG(Warning) << "EMG electrode on channel " << ai_channel_.get_channel_number() << " cannot compute features until buffer is of size " << window_size << ".";
        return;
    }
    mes_dm_window_.resize(window_size);
    for (std::size_t i = 0; i < window_size; ++i) {
        mes_dm_window_[i] = mes_dm_buffer_[i + mes_dm_buffer_.size() - window_size];
    }
    hudgins_td_features_[0] = mean_absolute_value(mes_dm_window_);
    hudgins_td_features_[1] = wavelength(mes_dm_window_);
    hudgins_td_features_[2] = zero_crossings(mes_dm_window_);
    hudgins_td_features_[3] = slope_sign_changes(mes_dm_window_);
}

void EmgElectrode::compute_ar_features(std::size_t window_size) {
    if (mes_dm_buffer_.size() < window_size) {
        LOG(Warning) << "EMG electrode on channel " << ai_channel_.get_channel_number() << " cannot compute features until buffer is of size " << window_size << ".";
        return;
    }
    mes_dm_window_.resize(window_size);
    for (std::size_t i = 0; i < window_size; ++i) {
        mes_dm_window_[i] = mes_dm_buffer_[i + mes_dm_buffer_.size() - window_size];
    }
    auto_regressive_coefficients(ar_features_, mes_dm_window_);
}

const std::vector<double>& EmgElectrode::get_all_features() {
    auto it = std::copy(rms_features_.begin(), rms_features_.end(), all_features_.begin());
    it = std::copy(hudgins_td_features_.begin(), hudgins_td_features_.end(), it);
    it = std::copy(ar_features_.begin(), ar_features_.end(), it);
    return all_features_;
}

const std::vector<double>& EmgElectrode::get_rms_features() const {
    return rms_features_;
}

const std::vector<double>& EmgElectrode::get_hudgins_td_features() const {
    return hudgins_td_features_;
}

const std::vector<double>& EmgElectrode::get_ar_features() const {
    return ar_features_;
}

std::size_t EmgElectrode::get_all_features_count() const {
    return all_features_count_;
}

std::size_t EmgElectrode::get_rms_features_count() const {
    return rms_features_count_;
}

std::size_t EmgElectrode::get_hudgins_td_features_count() const {
    return hudgins_td_features_count_;
}

std::size_t EmgElectrode::get_ar_features_count() const {
    return ar_features_count_;
}

double EmgElectrode::mean_rms(const std::vector<double>& mes_window) const {
    double sum_squares = 0.0;
    for (int i = 0; i < mes_window.size(); ++i) {
        sum_squares += std::pow(mes_window[i], 2);
    }
    return std::sqrt(sum_squares / mes_window.size());
}

double EmgElectrode::mean_absolute_value(const std::vector<double>& mes_window) const {
    double sum_abs = 0.0;
    for (int i = 0; i < mes_window.size(); ++i) {
        sum_abs += std::abs(mes_window[i]);
    }
    return sum_abs / mes_window.size();
}

double EmgElectrode::wavelength(const std::vector<double>& mes_window) const {
    double sum_abs_diff = 0.0;
    for (int i = 0; i < mes_window.size() - 1; ++i) {
        sum_abs_diff += std::abs(mes_window[i + 1] - mes_window[i]);
    }
    return sum_abs_diff;
}

double EmgElectrode::zero_crossings(const std::vector<double>& mes_window) const {
    double sum_abs_diff_sign = 0.0;
    for (int i = 0; i < mes_window.size() - 1; ++i) {
        sum_abs_diff_sign += std::abs(std::copysign(1.0, mes_window[i + 1]) - std::copysign(1.0, mes_window[i]));
    }
    return sum_abs_diff_sign / 2.0;
}

double EmgElectrode::slope_sign_changes(const std::vector<double>& mes_window) const {
    double sum_abs_diff_sign_diff = 0.0;
    for (int i = 0; i < mes_window.size() - 2; ++i) {
        sum_abs_diff_sign_diff += std::abs(std::copysign(1.0, (mes_window[i + 2] - mes_window[i + 1])) - std::copysign(1.0, (mes_window[i + 1] - mes_window[i])));
    }
    return sum_abs_diff_sign_diff / 2.0;
}

void EmgElectrode::auto_regressive_coefficients(std::vector<double>& coeffs, const std::vector<double>& mes_window) {

    // Algorithm taken from Cedrick Collomb, "Burg's Method, Algorithm and Recursion," November 8, 2009

    // initialize
    std::size_t N = mes_window.size() - 1;
    std::size_t m = coeffs.size();
    std::vector<double> A_k(m + 1, 0.0);
    A_k[0] = 1.0;
    std::vector<double> f = mes_window;
    std::vector<double> b = mes_window;

    double D_k = 0;
    for (std::size_t j = 0; j <= N; ++j) {
        D_k += 2.0 * f[j] * f[j];
    }
    D_k -= f[0] * f[0] + b[N] * b[N];


    // Burg recursion
    for (std::size_t k = 0; k < m; ++k) {

        // compute mu
        double mu = 0.0;
        for (std::size_t n = 0; n <= N - k - 1; ++n) {
            mu += f[n + k + 1] * b[n];
        }
        mu *= -2.0 / D_k;

        // update A_k
        for (std::size_t n = 0; n <= (k + 1) / 2; ++n) {
            double t1 = A_k[n] + mu * A_k[k + 1 - n];
            double t2 = A_k[k + 1 - n] + mu * A_k[n];
            A_k[n] = t1;
            A_k[k + 1 - n] = t2;
        }

        // update f and b
        for (std::size_t n = 0; n <= N - k - 1; ++n) {
            double t1 = f[n + k + 1] + mu * b[n];
            double t2 = b[n] + mu * f[n + k + 1];
            f[n + k + 1] = t1;
            b[n] = t2;
        }

        // update D_k
        D_k = (1.0 - mu * mu) * D_k - f[k + 1] * f[k + 1] - b[N - k - 1] * b[N - k - 1];
    }

    // assign coefficients
    coeffs.assign(++A_k.begin(), A_k.end());
}


} // namespace mel