#include "Daq.h"

namespace mel {

    Daq::Daq(std::string name,
        channel_vec ai_channels,
        channel_vec ao_channels,
        channel_vec di_channels,
        channel_vec do_channels,
        channel_vec encoder_channels,
        channel_vec encrate_channels) :
        name_(name),
        data_log_filename_(log_dir_ + "\\" + name + "_" + get_current_date_time() + ".csv"),
        // sort channel numbers from smallest to largest and delete duplicates and save
        ai_channels_nums_(sort_and_reduce_channels(ai_channels)),
        ao_channels_nums_(sort_and_reduce_channels(ao_channels)),
        di_channels_nums_(sort_and_reduce_channels(di_channels)),
        do_channels_nums_(sort_and_reduce_channels(do_channels)),
        encoder_channels_nums_(sort_and_reduce_channels(encoder_channels)),
        encrate_channels_nums_(sort_and_reduce_channels(encrate_channels)),
        // save number of unique channels being use
        num_ai_channels_(ai_channels_nums_.size()),
        num_ao_channels_(ao_channels_nums_.size()),
        num_di_channels_(di_channels_nums_.size()),
        num_do_channels_(do_channels_nums_.size()),
        num_encoder_channels_(encoder_channels_nums_.size()),
        num_encrate_channels_(encrate_channels_nums_.size())
    {
        // initialize state variables sizes and set values to zero (if this is not done now, a nullptr exception will be thrown!) 
        ai_voltages_ = voltage_vec(num_ai_channels_, 0.0);
        ao_voltages_ = voltage_vec(num_ao_channels_, 0.0);
        di_signals_ = dsignal_vec(num_di_channels_, 0);
        do_signals_ = dsignal_vec(num_do_channels_, 0);
        enc_counts_ = int32_vec(num_encoder_channels_, 0);
        enc_rates = double_vec(num_encrate_channels_, 0.0);

        // create data log specifically for this DAQ
        boost::filesystem::path dir(log_dir_.c_str());
        boost::filesystem::create_directory(dir);
        data_log_.open(data_log_filename_, std::ofstream::out | std::ofstream::trunc); // change trunc to app to append;

        // print a header to the top of the data log
        data_log_ << "Timestamp" << "\t";
        for (auto it = ai_channels_nums_.begin(); it != ai_channels_nums_.end(); ++it)
            data_log_ << "AI_" + std::to_string(*it) << "\t";
        for (auto it = ao_channels_nums_.begin(); it != ao_channels_nums_.end(); ++it)
            data_log_ << "AO_" + std::to_string(*it) << "\t";
        for (auto it = di_channels_nums_.begin(); it != di_channels_nums_.end(); ++it)
            data_log_ << "DI_" + std::to_string(*it) << "\t";
        for (auto it = do_channels_nums_.begin(); it != do_channels_nums_.end(); ++it)
            data_log_ << "DO_" + std::to_string(*it) << "\t";
        for (auto it = encoder_channels_nums_.begin(); it != encoder_channels_nums_.end(); ++it)
            data_log_ << "ENC_COUNT_" + std::to_string(*it) << "\t";
        for (auto it = encrate_channels_nums_.begin(); it != encrate_channels_nums_.end(); ++it)
            data_log_ << "ENC_RATE_" + std::to_string(*it) << "\t";
        data_log_ << std::endl;

    }

    voltage_vec Daq::get_analog_voltages() {
        return ai_voltages_;
    }

    voltage Daq::get_analog_voltage(channel channel_number) {
        return ai_voltages_[channel_number_to_index(ai_channels_nums_, channel_number)];
    }

    void Daq::set_analog_voltages(voltage_vec new_voltages) {
        new_voltages.resize(num_ao_channels_, 0.0); // ensures same size; pads with zeros if too short
        ao_voltages_ = new_voltages;
    }

    void Daq::set_analog_voltage(channel channel_number, voltage new_voltage) {
        ao_voltages_[channel_number_to_index(ao_channels_nums_, channel_number)] = new_voltage;
    }

    dsignal_vec Daq::get_digital_signals() {
        return di_signals_;
    }

    dsignal Daq::get_digital_signal(channel channel_number) {
        return di_signals_[channel_number_to_index(di_channels_nums_, channel_number)];
    }

    void Daq::set_digital_signals(dsignal_vec new_signals) {
        new_signals.resize(num_do_channels_, 0); // ensures same size; pads with zeros if too short
        do_signals_ = new_signals;
    }

    void Daq::set_digital_signal(channel channel_number, dsignal new_state) {
        do_signals_[channel_number_to_index(do_channels_nums_, channel_number)] = new_state;
    }

    int32_vec Daq::get_encoder_counts() {
        return enc_counts_;
    }

    int32 Daq::get_encoder_count(channel channel_number) {
        return enc_counts_[channel_number_to_index(encoder_channels_nums_, channel_number)];
    }

    double_vec Daq::get_encoder_rates() {
        return enc_rates;
    }

    double Daq::get_encoder_rate(channel channel_number) {
        return enc_rates[channel_number_to_index(encrate_channels_nums_, channel_number)];
    }

    uint32_vec Daq::get_encoder_quadrature_factors() {
        return encoder_quadrature_factors_;
    }

    uint32 Daq::get_encoder_quadrature_factor(channel channel_number) {
        return encoder_quadrature_factors_[channel_number_to_index(encoder_channels_nums_, channel_number)];
    }

    void Daq::log_data(double timestamp) {

        data_log_ << timestamp << "\t";

        for (auto it = ai_voltages_.begin(); it != ai_voltages_.end(); ++it)
            data_log_ << *it << "\t";
        for (auto it = ao_voltages_.begin(); it != ao_voltages_.end(); ++it)
            data_log_ << *it << "\t";
        for (auto it = di_signals_.begin(); it != di_signals_.end(); ++it)
            data_log_ << (int)*it << "\t";
        for (auto it = do_signals_.begin(); it != do_signals_.end(); ++it)
            data_log_ << (int)*it << "\t";
        for (auto it = enc_counts_.begin(); it != enc_counts_.end(); ++it)
            data_log_ << *it << "\t";
        for (auto it = enc_rates.begin(); it != enc_rates.end(); ++it)
            data_log_ << *it << "\t";
        data_log_ << std::endl;

    }

    // HELPER FUNCTIONS

    channel_vec::size_type Daq::channel_number_to_index(const channel_vec& channels, const channel channel_number) {
        auto result = std::find(channels.begin(), channels.end(), channel_number);
        if (result != channels.end()) {
            return result - channels.begin();
        }
        else {
            std::cout << "FATAL ERROR: Attempted to access invalid channel number " << channel_number << "." << std::endl;
        }
    }

    channel_vec Daq::sort_and_reduce_channels(channel_vec channels) {
        std::sort(channels.begin(), channels.end());
        channels.erase(std::unique(channels.begin(), channels.end()), channels.end());
        return channels;
    }

}