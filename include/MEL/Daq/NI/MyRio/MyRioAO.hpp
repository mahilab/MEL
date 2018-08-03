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

#ifndef MEL_MYRIOAO_HPP
#define MEL_MYRIOAO_HPP

#include <MEL/Daq/Output.hpp>
#include <MEL/Utility/NonCopyable.hpp>

//==============================================================================
// FORWARD DECLARATIONS
//==============================================================================

namespace mel {

class MyRio;
enum MyRioConnectorType : int;

//==============================================================================
// CLASS DECLARATION
//==============================================================================

class MEL_API MyRioAO : public AnalogOutput, NonCopyable {
public:

    MyRioAO(MyRio& daq, MyRioConnectorType type, const std::vector<uint32>& channel_numbers);

    bool enable() override;

    bool disable() override;

    bool update_channel(uint32 channel_number) override;

private:

    MyRio& daq_;
    MyRioConnectorType type_;

};

}  // namespace mel

#endif  // MEL_MYRIOAO_HPP