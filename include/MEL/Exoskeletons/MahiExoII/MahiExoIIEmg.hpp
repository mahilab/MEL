

#ifndef MEL_MAHIEXOIIEMG_HPP
#define MEL_MAHIEXOIIEMG_HPP

#include <MEL/Exoskeletons/MahiExoII/MahiExoII.hpp>
#include <MEL/Core/EmgElectrode.hpp>
#include <MEL/Utility/RingBuffer.hpp>
#include <vector>

namespace mel {

class MahiExoIIEmg : public virtual MahiExoII {

public:

    /// Constructor
    MahiExoIIEmg(MeiiConfiguration configuration, MeiiParameters parameters = MeiiParameters());

    /// Update the voltage reading for all EMG electrodes attached to the robot, and apply all signal processing and update associated signals
    void update_emg();

    /// Returns the raw MES voltage at last update for all EMG electrodes attached to the robot  
    const std::vector<double>& get_mes_raw() const;

    /// Returns the MES demeaned signal at last update for all EMG electrodes attached to the robot
    const std::vector<double>& get_mes_dm() const;

    /// Returns the MES envelope at last update for all EMG electrodes attached to the robot
    const std::vector<double>& get_mes_env() const;

    /// Returns the MES TKEO envelope at last update for all EMG electrodes attached to the robot
    const std::vector<double>& get_mes_tkeo_env() const;

    /// Get the last window_size elements pushed to the MES TKEO envelope buffer
    const std::vector<std::vector<double>>& get_mes_tkeo_env_buffer_data(std::size_t window_size);

    /// Returns the channel numbers on the Q8 Analog Input module associated with the EMG electrodes attached to the robot
    std::vector<uint32> get_emg_channel_numbers() const;

    /// Returns the number of EMG channels/electrodes attached to the robot
    std::size_t get_emg_channel_count() const;

    /// Resize the MES ring buffer on all EMG electrodes attached to the robot to a new capacity
    void resize_mes_buffer(std::size_t capacity);

    /// Check to see if the buffer is full
    bool is_mes_buffer_full();

    /// Calls the reset function of all signal processing for EMG
    void reset_emg_signal_processing();

    /// Compute all possible features from data currently in the MES buffer
    void compute_all_features();

    /// Get the most recently computed features from the MES buffer
    const std::vector<double>& get_all_features();

    /// Get the number of features returned by get_all_features()
    std::size_t get_all_features_count() const;

    /// Get the capacity of the current MES buffer
    std::size_t get_mes_buffer_capacity() const;

    
public:

    bool emg_signal_monitor_ = false;
    const double emg_voltage_max_ = 5.0;
      

private:

    const std::vector<uint32> emg_channel_numbers_;
    const std::size_t emg_channel_count_;

    std::vector<EmgElectrode> emg_electrodes_;
    size_t mes_buffer_capacity_;
    size_t mes_feature_window_size_;

    std::vector<double> mes_raw_;
    std::vector<double> mes_demean_;
    std::vector<double> mes_envelope_;
    std::vector<double> mes_tkeo_envelope_;

    std::vector<std::vector<double>> mes_tkeo_env_window_;

    std::vector<double> all_features_;

};

} // namespace mel

#endif // MEL_MAHIEXOIIEMG_HPP
