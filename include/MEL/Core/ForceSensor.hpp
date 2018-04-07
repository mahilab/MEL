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

#ifndef MEL_FORCESENSOR_HPP
#define MEL_FORCESENSOR_HPP

#include <MEL/Core/Device.hpp>
#include <MEL/Daq/Input.hpp>

namespace mel {

//==============================================================================
// CLASS DECLARATION
//==============================================================================

class ForceSensor : public Device {
public:
    /// Default constructor
    ForceSensor();

    /// Prefferred constructor, giving it a unique name and assigning the
    /// associated DAQ channel
    ForceSensor(std::string name,
                std::vector<Input<Voltage>::Channel> ai_channels);

    /// This function should return the forces of the FroceSensor
    virtual std::vector<double> get_forces() = 0;

protected:
    /// Reads the voltages from the associated DAQ channels and returns them
    std::vector<double> get_voltages();

protected:
    std::vector<Input<Voltage>::Channel>
        ai_channels_;   ///< the DAQ analog input channels bound to this sensor
    int num_channels_;  ///< number of DAQ analog input channels bound to this
                        ///< sensor
    std::vector<double> forces_;  ///< stores all the forces read by the sensor
                                  ///< at one sample time
    std::vector<double> voltages_;  ///< stores all the voltages read by the
                                    ///< sensor at one sample time
};

}  // namespace mel

#endif  // MEL_FORCESENSOR_HPP