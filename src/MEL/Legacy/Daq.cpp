#include <MEL/Core/Daq.hpp>
#include <algorithm>
#include <iostream>
#include <limits>

namespace mel {

//==============================================================================
// CLASS DEFINITIONS
//==============================================================================

Daq::Daq(std::string name,
    channel_vec ai_channels,
    channel_vec ao_channels,
    channel_vec di_channels,
    channel_vec do_channels,
    channel_vec encoder_channels,
    channel_vec encrate_channels) :
    Device(name),
    // sort channel numbers from smallest to largest and delete duplicates and save
    ai_channel_nums_(sort_and_reduce_channels(ai_channels)),
    ao_channel_nums_(sort_and_reduce_channels(ao_channels)),
    di_channel_nums_(sort_and_reduce_channels(di_channels)),
    do_channel_nums_(sort_and_reduce_channels(do_channels)),
    encoder_channel_nums_(sort_and_reduce_channels(encoder_channels)),
    encrate_channel_nums_(sort_and_reduce_channels(encrate_channels)),
    // save number of unique channels being use
    ai_channels_count_(ai_channel_nums_.size()),
    ao_channels_count_(ao_channel_nums_.size()),
    di_channels_count_(di_channel_nums_.size()),
    do_channels_count_(do_channel_nums_.size()),
    encoder_channels_count_(encoder_channel_nums_.size()),
    encrate_channels_count_(encrate_channel_nums_.size())
{
    // initialize state variables sizes and set values to zero (if this is not done now, a nullptr exception will be thrown!)
    ai_voltages_ = voltage_vec(ai_channels_count_, 0.0);
    ao_voltages_ = voltage_vec(ao_channels_count_, 0.0);
    di_signals_ = dsignal_vec(di_channels_count_, 0);
    do_signals_ = dsignal_vec(do_channels_count_, 0);
    enc_counts_ = std::vector<int32>(encoder_channels_count_, 0);
    enc_rates = std::vector<double>(encrate_channels_count_, 0.0);
}

void Daq::reset() {
    print("WARNING: DAQ <" + name_ + "> does not implement " + __FUNCTION__);
}

void Daq::read_all() {
    print("WARNING: DAQ <" + name_ + "> does not implement " + __FUNCTION__);
}

void Daq::write_all() {
    print("WARNING: DAQ <" + name_ + "> does not implement " + __FUNCTION__);
}

void Daq::start_watchdog(double watchdog_timeout) {
    print("WARNING: DAQ <" + name_ + "> does not implement " + __FUNCTION__);
}

void Daq::reload_watchdog() {
    print("WARNING: DAQ <" + name_ + "> does not implement " + __FUNCTION__);
}

void Daq::stop_watchdog() {
    print("WARNING: DAQ <" + name_ + "> does not implement " + __FUNCTION__);
}

bool Daq::is_watchdog_expired() {
    print("WARNING: DAQ <" + name_ + "> does not implement " + __FUNCTION__);
    return false;
}

int Daq::channel_number_to_index(const channel_vec& channels, const channel channel_number) {
    auto result = std::find(channels.begin(), channels.end(), channel_number);
    if (result != channels.end()) {
        int index = result - channels.begin();
        if (index >= 0 && index < channels.size()) {
            return index;
        }
        else {
            std::cout << "FATAL ERROR: Channel index " << index << " out of bounds." << std::endl;
            return -1;
        }
    }
    else {
        std::cout << "FATAL ERROR: Attempted to access invalid channel number " << channel_number << "." << std::endl;
        return -1;
    }
}

channel_vec Daq::sort_and_reduce_channels(channel_vec channels) {
    std::sort(channels.begin(), channels.end());
    channels.erase(std::unique(channels.begin(), channels.end()), channels.end());
    return channels;
}

//==============================================================================
// ANALOG INPUT
//==============================================================================

void Daq::read_analogs() {
    print("WARNING: DAQ <" + name_ + "> does not implement " + __FUNCTION__);
}

voltage_vec Daq::get_analog_voltages() {
    return ai_voltages_;
}

voltage Daq::get_analog_voltage(channel channel_number) {
    int index = channel_number_to_index(ai_channel_nums_, channel_number);
    if (index >= 0)
        return ai_voltages_[index];
    else
        return std::numeric_limits<voltage>::quiet_NaN();
}

void Daq::set_ai_voltage_ranges(voltage_vec min_voltages, voltage_vec max_voltages) {
    ai_min_voltages_ = min_voltages;
    ai_max_voltages_ = max_voltages;
}

Daq::Ai Daq::ai_(channel channel_number) {
    return Ai(this, channel_number);
}

//==============================================================================
// ANALOG OUTPUT
//==============================================================================

void Daq::write_analogs() {
    print("WARNING: DAQ <" + name_ + "> does not implement " + __FUNCTION__);
}


void Daq::set_analog_voltages(voltage_vec new_voltages) {
    new_voltages.resize(ao_channels_count_, 0.0); // ensures same size; pads with zeros if too short
    ao_voltages_ = new_voltages;
}

void Daq::set_analog_voltage(channel channel_number, voltage new_voltage) {
    int index = channel_number_to_index(ao_channel_nums_, channel_number);
    if (index >= 0)
        ao_voltages_[index] = new_voltage;
}

void Daq::set_ao_voltage_ranges(voltage_vec min_voltages, voltage_vec max_voltages) {
    ao_min_voltages_ = min_voltages;
    ao_max_voltages_ = max_voltages;
}

void Daq::set_ao_initial_voltages(voltage_vec initial_voltages) {
    ao_initial_voltages_ = initial_voltages;
}

void Daq::set_ao_final_voltages(voltage_vec final_voltages) {
    ao_final_voltages_ = final_voltages;
}

void Daq::set_ao_expire_voltages(voltage_vec expire_voltages) {
    ao_expire_voltages_ = expire_voltages;
}

Daq::Ao Daq::ao_(channel channel_number) {
    return Ao(this, channel_number);
}

//==============================================================================
// DIGITAL INPUT
//==============================================================================

void Daq::read_digitals() {
    print("WARNING: DAQ <" + name_ + "> does not implement " + __FUNCTION__);
}

dsignal_vec Daq::get_digital_signals() {
    return di_signals_;
}

dsignal Daq::get_digital_signal(channel channel_number) {
    int index = channel_number_to_index(di_channel_nums_, channel_number);
    if (index >= 0)
        return di_signals_[index];
    else
        return std::numeric_limits<dsignal>::quiet_NaN();
}

Daq::Di Daq::di_(channel channel_number) {
    return Di(this, channel_number);
}

//==============================================================================
// DIGITAL OUTPUT
//==============================================================================

void Daq::write_digitals() {
    print("WARNING: DAQ <" + name_ + "> does not implement " + __FUNCTION__);
}

void Daq::set_digital_signals(dsignal_vec new_signals) {
    new_signals.resize(do_channels_count_, 0); // ensures same size; pads with zeros if too short
    do_signals_ = new_signals;
}

void Daq::set_digital_signal(channel channel_number, dsignal new_state) {
    int index = channel_number_to_index(do_channel_nums_, channel_number);
    if (index >= 0)
        do_signals_[index] = new_state;
}

void Daq::set_do_initial_signals(dsignal_vec initial_signals) {
    do_initial_signals_ = initial_signals;
}

void Daq::set_do_final_signals(dsignal_vec final_signals) {
    do_final_signals_ = final_signals;
}

void Daq::set_do_expire_signals(dsignal_vec expire_signals) {
    do_expire_signals_ = expire_signals;
}

Daq::Do Daq::do_(channel channel_number) {
    return Do(this, channel_number);
}

//==============================================================================
// ENCODERS
//==============================================================================

void Daq::read_encoders() {
    print("WARNING: DAQ <" + name_ + "> does not implement " + __FUNCTION__);
}

void Daq::read_encrates() {
    print("WARNING: DAQ <" + name_ + "> does not implement " + __FUNCTION__);
}

void Daq::zero_encoders() {
    print("WARNING: DAQ <" + name_ + "> does not implement " + __FUNCTION__);
}

void Daq::offset_encoders(std::vector<int32> offset_counts) {
    print("WARNING: DAQ <" + name_ + "> does not implement " + __FUNCTION__);
}

std::vector<int32> Daq::get_encoder_counts() {
    return enc_counts_;
}

int32 Daq::get_encoder_count(channel channel_number) {
    int index = channel_number_to_index(encoder_channel_nums_, channel_number);
    if (index >= 0)
        return enc_counts_[index];
    else
        return std::numeric_limits<int32>::quiet_NaN();
}

std::vector<double> Daq::get_encoder_rates() {
    return enc_rates;
}

double Daq::get_encoder_rate(channel channel_number) {
    int index = channel_number_to_index(encrate_channel_nums_, channel_number);
    if (index >= 0)
        return enc_rates[index];
    else
        return std::numeric_limits<double>::quiet_NaN();
}

void Daq::set_encoder_quadrature_factors(std::vector<uint32> quadrature_factors) {
    encoder_quadrature_factors_ = quadrature_factors;
}

std::vector<uint32> Daq::get_encoder_quadrature_factors() {
    return encoder_quadrature_factors_;
}

uint32 Daq::get_encoder_quadrature_factor(channel channel_number) {
    int index = channel_number_to_index(encoder_channel_nums_, channel_number);
    if (index >= 0)
        return encoder_quadrature_factors_[index];
    else
        return std::numeric_limits<uint32>::quiet_NaN();
}

Daq::Encoder Daq::encoder_(channel channel_number) {
    return Encoder(this, channel_number);
}

Daq::EncRate Daq::encrate_(channel channel_number) {
    return EncRate(this, channel_number);
}


} // namespace mel
