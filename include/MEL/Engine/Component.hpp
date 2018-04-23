// MIT License
//
// MEL - Mechatronics Engine and Library
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

#ifndef MEL_COMPONENT_HPP
#define MEL_COMPONENT_HPP

#include <MEL/Engine/Object.hpp>
#include <typeindex>

namespace mel {

// Component interface
class Component {

protected:

    friend class Object;

    /// Default Constructor
    Component() {}

    /// Virtual Destructor
    virtual ~Component();

    /// Called when this Component's Object starts running
    virtual void start();

    /// Called every time this Component's Object updates
    virtual void update();

    /// Called every time this Component's Object late updates
    virtual void late_update();

    /// Called this Component's Object stops running
    virtual void stop();

    /// Called when this Component's Object resets
    virtual void reset();

public:

    /// Gets the Object this Component is attached to
    template <typename T = Object>
    T* get_object() { return dynamic_cast<T*>(object_); }

    /// Gets a Component attached to the same Object as this Component
    template <typename T>
    T* get() { return object_->get<T>(); }

    /// Gets the Component type name
    std::string get_type_name();

private:

    friend class Object;

    Object* object_;  ///< Object this Component is attached to
    std::vector<std::type_index> requirements_;  ///< Required Components

};

} // namespace mel

#endif // MEL_COMPONENT_HPP
