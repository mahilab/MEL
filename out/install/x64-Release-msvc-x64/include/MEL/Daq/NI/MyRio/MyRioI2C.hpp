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
// Author(s): Evan Pezent (epezent@rice.edu)

#pragma once
#include <MEL/Core/NonCopyable.hpp>
#include <MEL/Core/Types.hpp>
#include <MEL/Core/Device.hpp>
#include <bitset>

namespace mel {

class MyRioMxp;

class MyRioI2C : public NonCopyable, public Device {
public:

    void set_slave(const std::string& address);
    void set_slave(std::bitset<7> address);

    void send(const void* data, std::size_t size);

private:

    friend class MyRioMxp;

    MyRioI2C(MyRioMxp& connector);

    virtual bool on_enable() override;
    virtual bool on_disable() override;
    void sync();

    void execute();

private:

    MyRioMxp& connector_;

    // NI FPGA Registers
    uint32_t sysselect_; ///< system select register
    uint32_t cnfg_;      ///< configuration register
    uint32_t addr_;      ///< slave address register
    uint32_t cntr_;      ///< counter register
    uint32_t dato_;      ///< data-out reigster
    uint32_t dati_;      ///< data-in register
    uint32_t stat_;      ///< status register
    uint32_t ctrl_;      ///< control reigster
    uint32_t go_;        ///< execute register

};

} // namespace mel