// MIT License
//
// MEL - MAHI Exoskeleton Library
// Copyright (c) 2018 Mechatronics and Haptic Interfaces Lab - Rice University
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// Author(s): Craig McDonald (craig.g.mcdonald@gmail.com)

#ifndef MEL_EMGELECTRODE_HPP
#define MEL_EMGELECTRODE_HPP

#include <MEL/Daq/Input.hpp>
#include <MEL/Math/Filter.hpp>
#include <MEL/Math/Rectifier.hpp>
#include <MEL/Math/TeagerKaiserEnergyOperator.hpp>
#include <MEL/Utility/RingBuffer.hpp>
#include <vector>

namespace mel {

    //==============================================================================
    // CLASS DECLARATION
    //==============================================================================

    class EmgElectrode {

    public:

        /// Constructor
        EmgElectrode( AnalogInput::Channel ai_channel);

        /// Default destructor
        ~EmgElectrode() {};

        /// Update the voltage reading from the associated analog input channel on the DAQ, and apply all signal processing and update associated signals
        void update();

        /// Update and push to the buffer
        void update_and_buffer();

        /// Clear the buffer
        void clear_mes_buffer();

        /// Check to see if the buffer is full
        bool is_buffer_full();

        /// Get raw MES
        double get_mes_raw() const;

        /// Get demeaned MES
        double get_mes_demean() const;

        /// Get processed MES
        double get_mes_envelope() const;

        /// Get processed MES using TKEO
        double get_mes_tkeo_envelope() const;

        /// Get the current contents of the MES buffer
        std::vector<double> get_mes_buffer_data();

        /// Get the size of the current MES buffer
        size_t get_mes_buffer_capacity() const;

        /// Get the most recently computed features from the MES buffer
        std::vector<double> get_all_features() const;

        /// Get the most recently computed root-mean-square (RMS) features from the MES buffer
        std::vector<double> get_rms_features() const;

        /// Get the most recently computed Hudgins time-domain features from the MES buffer
        std::vector<double> get_hudgins_td_features() const;

        /// Get the most recently computed autoregressive features from the MES buffer
        std::vector<double> get_ar_features() const;

        /// Get the number of features returned by get_all_features()
        size_t get_all_feautures_count() const;

        /// Get the number of features returned by get_rms_features()
        size_t get_rms_feautures_count() const;

        /// Get the number of features returned by get_hudgins_td_features()
        size_t get_hudgins_td_feautures_count() const;

        /// Get the number of features returned by get_ar_features()
        size_t get_ar_feautures_count() const;

        /// Compute all possible features from data currently in the MES buffer
        void compute_all_features(); 

        /// Compute root-mean-square (RMS) features from data currently in the MES buffer
        void compute_rms_features();

        /// Compute Hudgins time-domain features from data currently in the MES buffer
        void compute_hudgins_td_features();

        /// Compute autoregressive features from data currently in the MES buffer
        void compute_ar_features();




    private:

        /// Put most recently updated signal in the buffer
        void push_mes_buffer();

        AnalogInput::Channel ai_channel_; ///< analog input of the associated DAQ

        double mes_raw_; ///< raw MES [V]
        double mes_demean_; ///< MES after high-pass filtering to remove mean and motion artifacts
        double mes_envelope_; ///< MES evnelope from rectification and low-pass filtering
        double mes_tkeo_envelope_; ///< MES envelope from TKEO, rectification, and low-pass filtering

        size_t mes_buffer_capacity_ = 200; ///< capacity of MES buffer
        RingBuffer<double> mes_demean_buffer_; ///< signal buffer to hold time history of demeaned MES for computing features
        std::vector<double> mes_demean_window_; ///< time window of MES same length as buffer

        size_t rms_feautures_count_ = 1; ///< number of features returned by get_rms_features()
        size_t hudgins_td_feautures_count_ = 4; ///< number of features returned by get_hudgins_td_features()
        size_t ar_feautures_count_ = 4; ///< number of features returned by get_ar_features()
        size_t all_feautures_count_; ///< number of features returned by get_all_features()
        std::vector<double> rms_features_; ///< most recently computed root-mean-square (RMS) features from the MES buffer 
        std::vector<double> hudgins_td_features_; ///< most recently computed Hudgins time-domain features from the MES buffer
        std::vector<double> ar_features_; ///< most recently computed autoregressive features from the MES buffer

        Filter hp_filter_; ///< high-pass filter for first phase of standard MES processing
        Rectifier rect_; ///< full-wave rectifier for second phase of standard MES processing
        Filter lp_filter_; ///< low-pass filter for third and final phase of standard MES processing
        TeagerKaiserEnergyOperator tkeo_; ///< teager-kaiser energy operator for second phase of TKEO MES processing
        Rectifier tkeo_rect_; ///< full-wave rectifier for third phase of standard MES processing
        Filter tkeo_lp_filter_; ///< low-pass filter for fourth and final phase of TKEO MES processing
        
        /// returns mean root-mean-square (RMS) for a window of MES
        double mean_rms(const std::vector<double>& mes_window) const;

        /// returns mean absolute value for a window of MES
        double mean_absolute_value(const std::vector<double>& mes_buffer) const;

        /// returns wavelength for a window of MES
        double wavelength(const std::vector<double>& mes_buffer) const;

        /// returns number of zero crossings for a window of MES
        double zero_crossings(const std::vector<double>& mes_buffer) const;

        /// returns number of slope sign changes for a window of MES
        double slope_sign_changes(const std::vector<double>& mes_buffer) const;

        /// returns autoregressive coefficients for a window of MES
        void auto_regressive_coefficients(std::vector<double>& coeffs, const std::vector<double>& mes_buffer);

    };


} // mel

#endif // MEL_EMGELECTRODE_HPP
