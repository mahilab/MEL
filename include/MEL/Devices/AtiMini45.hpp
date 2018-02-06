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

#ifndef MEL_ATIMINI45_HPP
#define MEL_ATIMINI45_HPP

#include <MEL/Core/ForceSensor.hpp>
#include <vector>

namespace mel {

//==============================================================================
// CLASS DECLARATION
//==============================================================================

class AtiMini45 : public ForceSensor {

public:

    //AtiMini45();

    AtiMini45(std::string name, std::vector<Input<Voltage>::Channel> ai_channels, array_2D<double, 6, 6> calib_mat);

    std::vector<double> get_forces() override;

    bool enable() override;

    bool disable() override;

private:

    std::vector<AnalogInput::Channel> ai_channels_;
    array_2D<double, 6, 6> calib_mat_;



};

} // namespace mel

#endif // MEL_ATIMINI45_HPP

//==============================================================================
// CLASS DOCUMENTATION
//==============================================================================
