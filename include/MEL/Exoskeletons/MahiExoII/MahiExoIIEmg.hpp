

#ifndef MEL_MAHIEXOIIEMG_HPP
#define MEL_MAHIEXOIIEMG_HPP

#include <MEL/Exoskeletons/MahiExoII/MahiExoII.hpp>
#include <MEL/Core/EmgElectrode.hpp>
#include <MEL/Utility/RingBuffer.hpp>
#include <vector>

namespace mel {

class MahiExoIIEmg : public virtual MahiExoII {

public:

    struct EmgDataBuffer {

        EmgDataBuffer(std::size_t num_channels, std::size_t length);

        void push_back(std::vector<double> data_vec);

        void clear();

        std::vector<double> at(int index) const;

        std::vector<double> get_channel(int index) const;


        std::size_t num_channels_;
        std::size_t length_;
        std::vector<RingBuffer<double>> data_buffer_;
    };

        

    /// Constructor
    MahiExoIIEmg(MeiiConfiguration configuration, MeiiParameters parameters = MeiiParameters());

    /// Update the voltage reading for all EMG electrodes attached to the robot, and apply all signal processing and update associated signals
    void update_emg();

    std::vector<double> get_mes_raw() const;

    std::vector<double> get_mes_demean() const;

    std::vector<double> get_mes_env() const;

    std::vector<double> get_mes_tkeo_env() const;

    std::vector<uint32> get_emg_channel_numbers() const;

    /// Resize the MES ring buffer on all EMG electrodes attached to the robot to a new capacity
    void resize_mes_buffer(std::size_t capacity);

    std::size_t get_emg_channel_count() const;

    void reset_emg_signal_processing();


    bool emg_signal_monitor_ = false;
    const double emg_voltage_max_ = 5.0;
      

private:

    const std::vector<uint32> emg_channel_numbers_;
    const std::size_t emg_channel_count_;
    std::vector<EmgElectrode> emg_electrodes_;
    std::vector<double> mes_raw_;
    std::vector<double> mes_demean_;
    std::vector<double> mes_envelope_;
    std::vector<double> mes_tkeo_envelope_;

};

} // namespace mel

#endif // MEL_MAHIEXOIIEMG_HPP
