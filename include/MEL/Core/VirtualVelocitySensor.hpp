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
// Author(s): Evan Pezent (epezent@rice.edu)

#ifndef MEL_VIRTUALVELOCITYSENSOR_HPP
#define MEL_VIRTUALVELOCITYSENSOR_HPP

#include <MEL/Core/VelocitySensor.hpp>
#include <MEL/Core/PositionSensor.hpp>
#include <MEL/Math/Differentiator.hpp>
#include <MEL/Math/Filter.hpp>
#include <MEL/Utility/Clock.hpp>

namespace mel {

//==============================================================================
// CLASS DECLARATION
//==============================================================================

class VirtualVelocitySensor : public VelocitySensor {

public:

    /// Constructor
    VirtualVelocitySensor(const std::string& name, 
        PositionSensor& position_sensor, 
        const Filter& filter = Filter({ 1,0 }, { 1, 0 }),
        const Differentiator& diff = Differentiator(Differentiator::CentralDifference));

    /// Enables the VirtualVelocitySensor
    bool enable() override;

    /// Disables teh VirtualVelocitySensor
    bool disable() override;

    void update();

    /// Returns the differentiated velocity
    double get_velocity() override;

private:

    PositionSensor& position_sensor_;  ///< the position sensor from which velocity will be derived
    Filter filter_;                    ///< the velocity filter
    Differentiator diff_;              ///< the differentiator
    Clock clock_;                      ///< clock that tracks time between calls to get_velocity
};

} // mel

#endif // MEL_VIRTUALVELOCITYSENSOR_HPP

//==============================================================================
// CLASS DOCUMENTATION
//==============================================================================
