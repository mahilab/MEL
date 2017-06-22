#include "Daq.h"

Daq::Daq(std::string type, std::string id, 
    uint_vec ai_channels,
    uint_vec ao_channels,
    uint_vec di_channels,
    uint_vec do_channels,
    uint_vec enc_channels,
    uint_vec vel_channels) :
    type_(type),
    id_(id),
    log_filename_("daq_logs\\" + type + "_" + id + "_" + get_current_date_time() + ".dat"),
    ai_channels_(ai_channels),
    ao_channels_(ao_channels),
    di_channels_(di_channels),
    do_channels_(di_channels),
    enc_channels_(enc_channels),
    vel_channels_(vel_channels)
{
    // sort channel numbers from smallest to largest and delete duplicates
    sort_and_reduce_channels(ai_channels);
    sort_and_reduce_channels(ao_channels);
    sort_and_reduce_channels(di_channels);
    sort_and_reduce_channels(di_channels);
    sort_and_reduce_channels(enc_channels);
    sort_and_reduce_channels(vel_channels);

    /* initialize state variables sizes and set values to zero (if this is not done now, a nullptr exception will be thrown!)  */
    ai_voltages_ = double_vec(ai_channels_.size(), 0.0);
    ao_voltages_ = double_vec(ao_channels_.size(), 0.0);
    di_states_ = char_vec(di_channels_.size(), 0);
    do_states_ = char_vec(do_channels_.size(), 0);
    enc_counts_ = int_vec(enc_channels_.size(), 0);
    enc_counts_per_sec_ = double_vec(enc_channels_.size(), 0.0);

    /* create data log specifically for this DAQ */
    std::string daq_log_dir_ = "daq_logs";
    boost::filesystem::path dir(daq_log_dir_.c_str());
    boost::filesystem::create_directory(dir);
    data_log_.open(log_filename_, std::ofstream::out | std::ofstream::trunc); // change trunc to app to append;
}

double_vec Daq::get_analog_voltages() {
    return ai_voltages_;
}

double Daq::get_analog_voltage(int channel_number) {
    return ai_voltages_[channel_number_to_index(ai_channels_, channel_number)];
}

void Daq::set_analog_voltages(double_vec new_voltages) {
    ao_voltages_ = new_voltages;
}

void Daq::set_analog_voltage(int channel_number, double new_voltage) {
    ao_voltages_[channel_number_to_index(ao_channels_, channel_number)] = new_voltage;
}

char_vec Daq::get_digital_states() {
    return di_states_;
}

char Daq::get_digital_state(int channel_number) {
    return di_states_[channel_number_to_index(di_channels_, channel_number)];
}

void Daq::set_digital_states(char_vec new_states) {
    do_states_ = new_states;
}

void Daq::set_digital_state(int channel_number, char new_state) {
    do_states_[channel_number_to_index(do_channels_, channel_number)] = new_state;
}

int_vec Daq::get_encoder_counts() {
    return enc_counts_;
}

int Daq::get_encoder_count(int channel_number) {
    return enc_counts_[channel_number_to_index(enc_channels_, channel_number)];
}

double_vec Daq::get_encoder_velocities() {
    return enc_counts_per_sec_;
}

double Daq::get_encoder_velocity(int channel_number) {
    return enc_counts_per_sec_[channel_number_to_index(vel_channels_, channel_number)];
}

// HELPER FUNCTIONS

uint Daq::channel_number_to_index(const uint_vec& channels, const uint channel_number) {
    return std::find(channels.begin(), channels.end(), channel_number) - channels.begin();
}

void Daq::sort_and_reduce_channels(uint_vec& channels) {
    channels.erase(std::unique(channels.begin(), channels.end()), channels.end());
    std::sort(channels.begin(), channels.end());
}