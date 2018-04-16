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

namespace mel {

class Component {

protected:

    friend class Object;

    /// Virtual Destructor
    virtual ~Component();

    /// Called once when the Engine starts running
    virtual void on_start();

    /// Called once on every iteration of the Engine loop
    virtual void on_update();

    /// Called once on every iteration of the Engine loop after update()
    virtual void on_late_update();

    /// Called once when the Engine stops
    virtual void on_stop();

    /// Gets a Component attached to the same Object as this Component
    template <typename T>
    T* get_component() {
        return object->get_component<T>();
    }

protected:

    Object* object;  ///< Object this Component is attached to
    
};

} // namespace mel

#endif // MEL_COMPONENT_HPP
