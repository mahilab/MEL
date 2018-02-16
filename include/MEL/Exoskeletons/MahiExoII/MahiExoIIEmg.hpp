#include <MEL/Exoskeletons/MahiExoII/MahiExoII.hpp>
#include <MEL/Core/EmgElectrode.hpp>
#include <MEL/Utility/RingBuffer.hpp>
#include <vector>

namespace mel {

    class MahiExoIIEmg : public virtual MahiExoII {

    public:

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

        

        /// Constructor
        MahiExoIIEmg(MeiiConfiguration configuration, MeiiParameters parameters = MeiiParameters());

        void update_emg();

        const std::vector<double>& get_mes_raw() const;

        const std::vector<double>& get_mes_demean() const;

        const std::vector<double>& get_mes_env() const;

        const std::vector<double>& get_mes_tkeo_env() const;

        const std::vector<uint32>& get_emg_channel_numbers() const;

        size_t get_emg_channel_count() const;

        


        bool emg_signal_monitor_ = false;
        const double emg_voltage_max_ = 5.0;
      

    private:

        const std::vector<uint32> emg_channel_numbers_;
        const size_t emg_channel_count_;
        std::vector<EmgElectrode> emg_electrodes_;
        std::vector<double> mes_raw_;
        std::vector<double> mes_demean_;
        std::vector<double> mes_envelope_;
        std::vector<double> mes_tkeo_envelope_;

    };

}
