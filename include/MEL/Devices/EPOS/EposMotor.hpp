// MIT License
//
// MEL - Mechatronics Engine & Library
// Copyright (c) 2019 Mechatronics and Haptic Interfaces Lab - Rice University
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
// Author(s): Zane Zook (gadzooks@rice.edu)

#pragma once

#include <MEL/Core/Device.hpp>
#include <MEL/Core/NonCopyable.hpp>

namespace mel {

class MEL_API EposMotor : public mel::Device, mel::NonCopyable {

public:

    EposMotor(const std::string& name);
    ~EposMotor();

private:

    bool on_enable() override;
    bool on_disable() override;

private:

};

} // namespace mel
