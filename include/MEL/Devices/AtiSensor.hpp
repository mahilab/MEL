// MIT License
//
// MEL - Mechatronics Engine & Library
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
// Author(s): Evan Pezent (epezent@rice.edu)

#ifndef MEL_ATI_SENSOR_HPP
#define MEL_ATI_SENSOR_HPP

#include <MEL/Mechatronics/ForceSensor.hpp>
#include <MEL/Mechatronics/TorqueSensor.hpp>
#include <MEL/Daq/Input.hpp>
#include <array>

namespace mel {

//==============================================================================
// CLASS DECLARATION
//==============================================================================

/// Implements an ATI force/torque transducer
class MEL_API AtiSensor : public ForceSensor, public TorqueSensor {

public:

    /// Calibration matrix data obtained from "UserAxis" elements
    /// in ATI supplied calibration file (e.g. "FTXXXXX.cal")
    struct Calibration {
        std::array<double, 6> Fx;
        std::array<double, 6> Fy;
        std::array<double, 6> Fz;
        std::array<double, 6> Tx;
        std::array<double, 6> Ty;
        std::array<double, 6> Tz;
    };

public:

    /// Constructor
    AtiSensor();

    /// Constructor
    AtiSensor(std::vector<Input<Voltage>::Channel> channels, Calibration calibration);

    /// Sets the voltages channels associated with this ATI sensor
    void set_channels(std::vector<Input<Voltage>::Channel> channels);

    /// Load sensor calibration from explicit calibration matrix
    void set_calibration(Calibration calibration);

    /// Sets the bias vector for offset correction from current voltages
    void offset();

    /// Returns force along speficied axis
    double get_force(Axis axis) override;

    /// Returns forces along X, Z, and Z axes
    std::vector<double> get_forces() override;

    /// Returns force along speficied axis
    double get_torque(Axis axis) override;

    /// Returns forces along X, Z, and Z axes
    std::vector<double> get_torques() override;    

private:

    /// Updates biased voltages
    void update_biased_voltages();

private:

    Calibration calibration_;                       ///< calibration matrix
    std::vector<Input<Voltage>::Channel> channels_; ///< raw voltage channels
    std::array<double, 6> bias_;                    ///< bias vector
    std::array<double, 6> bSTG_;                    ///< biased strain gauge voltages
};

} // namespace mel

#endif // MEL_ATI_SENSOR_HPP
