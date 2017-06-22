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
    log_filename_("daq_logs\\" + type + "_" + id + "_" + get_current_data_time() + ".dat")
{
    std::string daq_log_dir_ = "daq_logs";
    boost::filesystem::path dir(daq_log_dir_.c_str());
    boost::filesystem::create_directory(dir);
    data_log_.open(log_filename_, std::ofstream::out | std::ofstream::trunc); // change trunc to app to append;

    ai_channels_ = ai_channels;
    ao_channels_ = ao_channels;
    di_channels_ = di_channels;
    do_channels_ = do_channels;
    enc_channels_ = enc_channels;
    vel_channels_ = vel_channels;
}

double_vec Daq::get_analog_voltages() {
    return ai_voltages_;
}

double Daq::get_analog_voltage(int channel_number) {
    
}

void Daq::set_analog_voltages(double_vec new_voltages) {
    ao_voltages_ = new_voltages;
}

void Daq::set_analog_voltage(int channel_number, double new_voltage) {

}

char_vec Daq::get_digital_states() {
    return di_states_;
}

char Daq::get_digital_state(int channel_number) {

}

void Daq::set_digital_states(char_vec new_states) {
    do_states_ = new_states;
}

void Daq::set_digital_state(int channel_numner, char new_state) {

}

int_vec Daq::get_encoders_counts() {
    return enc_counts_;
}

int Daq::get_encoder_counts(int channel_number) {

}

double_vec Daq::get_encoders_counts_per_sec() {
    return enc_counts_per_sec_;
}

double Daq::get_encoder_counts_per_sec() {

}
