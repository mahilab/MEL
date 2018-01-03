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

                EmgDataBuffer(size_t num_channels, size_t length);

                void push_back(double_vec data_vec);

                void flush();

                double_vec at(int index) const;

                double_vec get_channel(int index) const;

                size_t num_channels_;
                size_t length_;
                std::vector<boost::circular_buffer<double>> data_buffer_;
            };

            class TeagerKaiserOperator {

                class TeagerKaiserOperatorImplementation {

                public:

                    TeagerKaiserOperatorImplementation();

                    void tkeo(const double& x, double& y);

                    void reset(); /// sets the internal states s_ to all be zero

                private:

                    double_vec s_;
                };

            public:

                TeagerKaiserOperator();

                void tkeo(const double& x, double& y);
                void tkeo(const double_vec& x, double_vec& y);

                void reset();

                bool tkeo_signal_monitor_ = false;
                const double tkeo_max_ = 5.0;

            private:
                
                static const int length_ = N_emg_;
                static const int n_ = 2;

                std::vector<TeagerKaiserOperatorImplementation> tko_implementations_;
            };


            // CONSTRUCTOR
            MahiExoIIEmg(Config configuration, Params parameters = Params());


            // PUBLIC FUNCTIONS
            double_vec get_emg_voltages();
          

            // PUBLIC VARIABLES
            std::vector<core::EmgElectrode> emg_electrodes_ = std::vector<core::EmgElectrode>(N_emg_);


            // SIGNAL CHECKING PARAMETERS
            bool emg_signal_monitor_ = false;
            const double emg_voltage_max_ = 5.0;


            // EMG FILTERING
            math::Filter butter_hp_ = math::Filter(N_emg_, { 0.814254556886246, - 3.257018227544984,   4.885527341317476, - 3.257018227544984,   0.814254556886246 }, { 1.000000000000000, - 3.589733887112175,   4.851275882519415, - 2.924052656162457,   0.663010484385890 });
            TeagerKaiserOperator tko_;
           // math::Filter tkeo_butter_lp_ = math::Filter(N_emg_, { 0.020083365564211, 0.040166731128423, 0.020083365564211 }, { 1.000000000000000, -1.561018075800718, 0.641351538057563 });
            math::Filter tkeo_butter_lp_ = math::Filter(N_emg_, { 0.002898194633721,   0.008694583901164,   0.008694583901164,   0.002898194633721 }, { 1.000000000000000, -2.374094743709352,   1.929355669091215, -0.532075368312092 });
            
            

        private:

            // PRIVATE VARIABLES

            double_vec emg_voltages_;

        };

    }

}
