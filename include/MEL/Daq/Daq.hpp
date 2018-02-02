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

#ifndef MEL_DAQ_HPP
#define MEL_DAQ_HPP

#include <MEL/Core/Device.hpp>
#include <MEL/Daq/Module.hpp>
#include <typeindex>
#include <unordered_map>

namespace mel {

//==============================================================================
// CLASS DECLARATION
//==============================================================================

class Daq : public Device {
public:
    /// The default constructor
    Daq(const std::string& name);

    /// The default destructor
    virtual ~Daq();

    /// This function should open communication with the DAQ, either through
    /// an API, socket communication, etc. It should not perform any other task
    /// than opening the communication channel; start up procedures should be
    /// implemented in enable(). This function should return true if the open
    /// was successful, false otherwise, and set #open_ accordingly
    virtual bool open() = 0;

    /// This function should close communication with the device. It should
    /// return true if successful, false otherwise, and set #open_ accordingly
    virtual bool close() = 0;

    /// This function should call the update function on all Input Modules
    /// contained on this DAQ. Alternatively, this function may call an API
    /// function that updates all input values simultaneously, but care should
    /// be taken in making sure each Input module's values get updated as well.
    /// It should return true if successful, false otherwise.
    virtual bool update_input() = 0;

    /// This function should call the update function on all Output Modules
    /// contained on this DAQ. Alternatively, this function may call an API
    /// function that updates all input values simultaneously, but care should
    /// be taken in making sure each Input module's values get updated as well.
    /// It should return true if successful, false otherwise.
    virtual bool update_output() = 0;

public:
    /// Returns true if communication with the device is open, false if closed.
    bool is_open() const;

    /// Gets a Module from the Daq
    template <class T>
    T& get_module() {
        return *static_cast<T*>(modules_[std::type_index(typeid(T))]);
    }

protected:

    /// Adds a a Module to the Daq
    template <class T>
    void add_module(T* module) {
        modules_[std::type_index(typeid(T))] = module;
    }

protected:
    bool open_;  ///< The Daq open status
    std::unordered_map<std::type_index, ModuleBase*> modules_;

};

}  // namespace mel

#endif  // MEL_DAQ_HPP

//==============================================================================
// CLASS DOCUMENTATION
//==============================================================================
