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
//            Craig McDonald (craig.g.mcdonald@gmail.com)

#ifndef MEL_EXO_HPP
#define MEL_EXO_HPP

#include <MEL/Core/Robot.hpp>

namespace mel {

//==============================================================================
// CLASS DECLARATION
//==============================================================================

class Exo : public Robot {

public:

    Exo(const std::string& name);

    virtual ~Exo() {};

    virtual std::vector<double> get_anatomical_joint_positions() const;

    virtual double get_anatomical_joint_position(int index) const;

    virtual std::vector<double> get_anatomical_joint_velocities() const;

    virtual double get_anatomical_joint_velocity(int index) const;

    virtual void set_anatomical_joint_torques(std::vector<double> new_torques) {}

protected:

    std::vector<double> anatomical_joint_positions_;
    std::vector<double> anatomical_joint_velocities_;
    std::vector<double> anatomical_joint_torques_;

};

} // namespace mel

#endif // MEL_EXO_HPP

//==============================================================================
// CLASS DOCUMENTATION
//==============================================================================
