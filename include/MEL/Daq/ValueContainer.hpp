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

#ifndef MEL_VALUE_CONTAINER_HPP
#define MEL_VALUE_CONTAINER_HPP

#include <MEL/Config.hpp>
#include <MEL/Utility/Types.hpp>
#include <vector>
#include <iostream>

namespace mel {

class ModuleBase;

//==============================================================================
// BASE DECLARATION
//==============================================================================

class MEL_API ValueContainerBase {
public:
    /// Constructor
    ValueContainerBase(ModuleBase* module);

protected:

    friend class ModuleBase;

    /// Called by ModuleBase when channel numbers change
    virtual void resize(std::size_t channel_count) = 0;

    /// Returns vector index associated with channel number
    std::size_t index(uint32 channel_number) const;

protected:

    ModuleBase* module_; ///< pointer to module
};

//==============================================================================
// CLASS DECLARATION
//==============================================================================

template <typename T>
class ValueContainer : public ValueContainerBase {
public:

    /// Constructor
    ValueContainer(ModuleBase* module) : ValueContainerBase(module) {
        resize(module_->get_channel_count());
    }

    /// Read access
    const T& operator[](uint32 channel_number) const {
        return values_[index(channel_number)];
    }

    /// Write access
    T& operator[](uint32 channel_number) {
        return values_[index(channel_number)];
    }

    /// Gets size of container
    std::size_t size() const {
        return values_.size();
    }

    /// Gets reference to raw values array
    std::vector<T>& get() {
        return values_;
    }

    /// Sets raw values array
    void set(const std::vector<T>& values) {
        if (module_->validate_channel_count(values.size()))
            values_ = values;
    }

    /// Overload stream operator
    template <typename U>
    friend std::ostream& operator<<(std::ostream& os, const ValueContainer<U>& container);

private:

    /// Called by ModuleBase when channel numbers change
    void resize(std::size_t channel_count) override {
        values_.assign(channel_count, T());
    }

    std::vector<T> values_;  ///< raw values
};

//==============================================================================
// FUNCTIOINS
//==============================================================================

/// Overload stream operator
template <typename T>
std::ostream& operator<<(std::ostream& os, const ValueContainer<T>& container) {
    if (container.size() > 0) {
        os << "{";
        for (std::size_t i = 0; i < container.size() - 1; i++) {
            uint32 ch = container.module_->get_channel_numbers()[i];
            os << "CH[" << ch << "]:" << container[ch] << ", ";
        }
        uint32 ch = container.module_->get_channel_numbers()[container.size() - 1];
        os << "CH[" << ch << "]:" << container[ch] << "}";
    }
    else {
        os << "{}";
    }
    return os;
}

}

#endif // MEL_VALUE_CONTAINER_HPP
