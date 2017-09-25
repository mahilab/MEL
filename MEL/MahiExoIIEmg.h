#pragma once
#include "MahiExoII.h"
#include "EmgElectrode.h"
#include "Filter.h"
#include <boost/circular_buffer.hpp>

namespace mel {

    namespace exo {

        class MahiExoIIEmg : public virtual MahiExoII {

        public:

            static const int N_emg_ = 8; // number of EMG electrodes


            struct Config : public virtual MahiExoII::Config {
                std::array<core::Daq::Ai, N_emg_> emg_; // analog input channels that measure EMG voltages
            };

            struct Params : public virtual MahiExoII::Params {

            };

            struct EmgDataBuffer {
                EmgDataBuffer(size_t num_channels, size_t length) :
                    num_channels_(num_channels),
                    length_(length)
                {
                    for (size_t i = 0; i < num_channels_; ++i) {
                        data_buffer_.push_back(boost::circular_buffer<double>(length_, 0.0));
                    }
                }
                void push_back(double_vec data_vec) {
                    if (data_vec.size() != num_channels_) {
                        util::print("ERROR: Incorrect number of rows when calling EmgDataBuffer::push_back().");
                    }
                    for (int i = 0; i < num_channels_; ++i) {
                        data_buffer_[i].push_back(data_vec[i]);
                    }
                }
                double_vec at(int index) {
                    double_vec data_vec;
                    for (int i = 0; i < num_channels_; ++i) {
                        data_vec.push_back(data_buffer_[i][index]);
                    }
                    return data_vec;
                }
                double_vec get_channel(int index) {
                    double_vec channel_vec;
                    for (int i = 0; i < length_; ++i) {
                        channel_vec.push_back(data_buffer_[index][i]);
                    }
                    return channel_vec;
                }
                size_t num_channels_;
                size_t length_;
                std::vector<boost::circular_buffer<double>> data_buffer_;
            };

            // CONSTRUCTOR
            MahiExoIIEmg(Config configuration, Params parameters = Params());


            // PUBLIC FUNCTIONS

            double_vec get_emg_voltages();
            

            // PUBLIC VARIABLES

            std::vector<core::EmgElectrode*> emg_electrodes_;

            // EMG FILTERING
            math::Filter butter_hp_ = math::Filter(N_emg_, 4, { 0.814254556886246, - 3.257018227544984,   4.885527341317476, - 3.257018227544984,   0.814254556886246 }, { 1.000000000000000, - 3.589733887112175,   4.851275882519415, - 2.924052656162457,   0.663010484385890 });

        private:

            // PRIVATE VARIABLES

            double_vec emg_voltages_;

        };

    }

}
