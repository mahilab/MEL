#pragma once
#include "MahiExoII.h"
#include "EmgElectrode.h"
#include <boost/circular_buffer.hpp>

namespace mel {

    namespace exo {

        class MahiExoIIEmg : public virtual MahiExoII {

        public:

            struct Config : public virtual MahiExoII::Config {
                std::array<core::Daq::Ai, 8> emg_; // analog input channels that measure EMG voltages
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

        private:

            // PRIVATE VARIABLES

            double_vec emg_voltages_;

        };

    }

}
