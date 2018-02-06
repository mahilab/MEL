#pragma once
#include "MEL/Exoskeletons/MahiExoII/MahiExoII.hpp"
#include "MEL/Core/EmgSignal.hpp"
#include "MEL/Math/Filter.hpp"
#include "MEL/Utility/RingBuffer.hpp"

namespace mel {

    class MahiExoIIEmg : public virtual MahiExoII {

    public:

        static const int N_emg_ = 8; // number of EMG electrodes

        struct EmgDataBuffer {

            EmgDataBuffer(size_t num_channels, size_t length);

            void push_back(std::vector<double> data_vec);

            void clear();

            std::vector<double> at(int index) const;

            std::vector<double> get_channel(int index) const;


            size_t num_channels_;
            size_t length_;
            std::vector<RingBuffer<double>> data_buffer_;
        };

        class TeagerKaiserOperator {

            class TeagerKaiserOperatorImplementation {

            public:

                TeagerKaiserOperatorImplementation();

                void tkeo(const double& x, double& y);

                void reset(); // sets the internal states s_ to all be zero

            private:

                std::vector<double> s_;
            };

        public:

            TeagerKaiserOperator();

            void tkeo(const double& x, double& y);
            void tkeo(const std::vector<double>& x, std::vector<double>& y);

            void reset();

            bool tkeo_signal_monitor_ = false;
            const double tkeo_max_ = 5.0;

        private:
                
            static const int length_ = N_emg_;
            static const int n_ = 2;

            std::vector<TeagerKaiserOperatorImplementation> tko_implementations_;
        };


        /// Constructor
        MahiExoIIEmg(MeiiConfiguration configuration, MeiiParameters parameters = MeiiParameters());


        // PUBLIC FUNCTIONS
        std::vector<double> get_emg_voltages();
          

        // PUBLIC VARIABLES
        std::vector<EmgSignal> emg_electrodes_;


        // SIGNAL CHECKING PARAMETERS
        bool emg_signal_monitor_ = false;
        const double emg_voltage_max_ = 5.0;


        // EMG FILTERING
        Filter butter_hp_ = Filter({ 0.814254556886246, - 3.257018227544984,   4.885527341317476, - 3.257018227544984,   0.814254556886246 }, { 1.000000000000000, - 3.589733887112175,   4.851275882519415, - 2.924052656162457,   0.663010484385890 }, N_emg_); // 4th-order Butterworth High-Pass at 0.05 normalized cutoff frequency
        TeagerKaiserOperator tko_;
        Filter tkeo_butter_lp_ = Filter({ 0.002898194633721,   0.008694583901164,   0.008694583901164,   0.002898194633721 }, { 1.000000000000000, -2.374094743709352,   1.929355669091215, -0.532075368312092 }, N_emg_);
            
            

    private:

        // PRIVATE VARIABLES

        std::vector<double> emg_voltages_;

    };

}
