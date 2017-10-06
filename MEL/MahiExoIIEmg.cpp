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

        MahiExoIIEmg::EmgDataBuffer::EmgDataBuffer(size_t num_channels, size_t length) :
            num_channels_(num_channels),
            length_(length)
        {
            for (size_t i = 0; i < num_channels_; ++i) {
                data_buffer_.push_back(boost::circular_buffer<double>(length_, 0.0));
            }
        }

        void MahiExoIIEmg::EmgDataBuffer::push_back(double_vec data_vec) {
            if (data_vec.size() != num_channels_) {
                util::print("ERROR: Incorrect number of rows when calling EmgDataBuffer::push_back().");
            }
            for (int i = 0; i < num_channels_; ++i) {
                data_buffer_[i].push_back(data_vec[i]);
            }
        }

        void MahiExoIIEmg::EmgDataBuffer::flush() {
            for (int i = 0; i < num_channels_; ++i) {
                data_buffer_[i].insert(data_buffer_[i].end(), length_, 0.0);
            }
        }

        double_vec MahiExoIIEmg::EmgDataBuffer::at(int index) const {
            double_vec data_vec(num_channels_);
            for (int i = 0; i < num_channels_; ++i) {
                data_vec[i] = data_buffer_[i][index];
            }
            return data_vec;
        }

        double_vec MahiExoIIEmg::EmgDataBuffer::get_channel(int index) const {
            double_vec channel_vec(length_);
            for (int i = 0; i < length_; ++i) {
                channel_vec[i] = data_buffer_[index][i];
            }
            return channel_vec;
        }

        MahiExoIIEmg::TeagerKaiserOperator::TeagerKaiserOperator() :
            //y_(double_vec(length_, 0.0)),
            tko_implementations_(std::vector<TeagerKaiserOperatorImplementation>(length_))
        { }

        void MahiExoIIEmg::TeagerKaiserOperator::tkeo(const double& x, double& y) {
            if (length_ != 1) {
                std::cout << "ERROR: Input vector does not match size of TeagerKaiserOperator." << std::endl;
                return;
            }
            tko_implementations_[0].tkeo(x, y);
        }

        void MahiExoIIEmg::TeagerKaiserOperator::tkeo(const double_vec& x, double_vec& y) {
            if (x.size() != length_) {
                std::cout << "ERROR: Input vector does not match size of TeagerKaiserOperator." << std::endl;
                return;
            }
            for (int i = 0; i < length_; ++i) {
                tko_implementations_[i].tkeo(x[i], y[i]);
            }
        }

        void MahiExoIIEmg::TeagerKaiserOperator::reset() {
            for (int i = 0; i < length_; ++i) {
                tko_implementations_[i].reset();
            }
        }

        

        MahiExoIIEmg::TeagerKaiserOperator::TeagerKaiserOperatorImplementation::TeagerKaiserOperatorImplementation() :
            //y_(0.0)
            s_(double_vec(n_, 0.0))        
        { }

        void MahiExoIIEmg::TeagerKaiserOperator::TeagerKaiserOperatorImplementation::tkeo(const double& x, double& y) {
            y = s_[0] * s_[0] - x * s_[1];
            s_[0] = x;
            s_[1] = s_[0];
        }

        void MahiExoIIEmg::TeagerKaiserOperator::TeagerKaiserOperatorImplementation::reset() {
            s_ = double_vec(n_, 0.0);
        }
    }

}