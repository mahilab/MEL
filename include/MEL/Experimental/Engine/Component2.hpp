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
//            Craig McDonald (craig.g.mcdonald@gmail.com)

#ifndef MEL_COMPONENT2_HPP
#define MEL_COMPONENT2_HPP

#include <array>
#include <string>
#include <unordered_map>
#include <vector>

namespace mel {

class Engine;

class Component2 {
protected:
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
    /// Virtual function enums
    enum Function {
        Start      = 0,
        Update     = 1,
        LateUpdate = 2,
        Stop       = 3,
        Reset      = 4
    };

    /// Function call ordering options
    enum Order {
        BeforeChildrenForward = 0,  ///< this, then children in order added
        BeforeChildrenReverse = 1,  ///< this, then children in reverse order
        AfterChildrenForward  = 2,  ///< children in order added, then this
        AfterChildrenReverse  = 3   ///< children in reverse order, then this
    };

public:
    /// Default Constructor
    Component2(const std::string& name);

    /// Virtual Destructor
    virtual ~Component2();

    /// Sets the call order of a Component function
    void order(Function function, Order order);

    /// Adds a new child Component (ownership maintained)
    template <typename T, typename... Args>
    void add(Args... args);

    /// Adds an existing child Component (ownership not maintained)
    void add(Component2* component);

    /// Removes a child Component by type and instance
    template <typename T>
    void remove(std::size_t i = 0);

    /// Removes all child Components
    void remove_all();

    /// Gets a child Component by type and instance
    template <typename T>
    T* get(std::size_t i = 0);

    /// Gets a global Component by name
    template <typename T = Component2>
    static T* get(const std::string& name);

    /// Returns true if an Object has a Component
    template <typename T>
    bool has();

    /// Gets a count of child Components by type
    template <typename T>
    std::size_t count();

    /// Gets the count of all child Components
    std::size_t count();

    /// Sets the Component name
    void name(const std::string& name);

    /// Gets the Component name
    const std::string& name();

    /// Gets the Component type name
    std::string type();

    /// Prints Component family tree to console
    std::string family_tree();

// private:
    friend class Engine;

    /// Recursively sets the Engine of this Object and all child Objects
    void set_engine(Engine* engine);

    /// Recursively calls a function of Component and all children
    void call(Function function);

    /// Adds an Component to the Component registry
    static bool register_component(Component2* component);

    /// Removes an Component from the Component registry
    static bool unregister_component(Component2* component);

private:
    /// Component instance name
    std::string name_;

    /// Engine pointer
    Engine* engine_;

    /// Parent Component
    Component2* parent_;

    /// Children Components
    std::vector<Component2*> children_;

    /// Component ownership mask
    std::vector<bool> ownership_mask_;

    /// Virtual functions
    std::array<void(Component2::*)(), 5> functions_;

    /// Function call orderings
    std::array<Order, 5> orderings_;

    /// Registry of all Components
    static std::unordered_map<std::string, Component2*> component_registry_;
};

//==============================================================================
// IMPLEMENTATION
//==============================================================================

template <typename T, typename... Args>
void Component2::add(Args... args) {
    Component2* component = new T(args...);
    component->parent_    = this;
    children_.push_back(component);
    ownership_mask_.push_back(true);
}

template <typename T>
void Component2::remove(std::size_t i) {}

template <typename T>
T* Component2::get(std::size_t i) {
    for (std::size_t c = 0; c < children_.size(); ++c) {
        T* component = dynamic_cast<T*>(children_[c]);
        if (component && (i-- == 0))
            return component;
    }
    return nullptr;
}

template <typename T>
T* Component2::get(const std::string& name) {
    auto it = component_registry_.find(name);
    if (it != component_registry_.end()) {
        T* component = dynamic_cast<T*>(it->second);
        if (component)
            return component;
    }
    return nullptr;
}

template <typename T>
bool Component2::has() {
    for (std::size_t c = 0; c < children_.size(); ++c) {
        if (dynamic_cast<T*>(children_[c]))
            return true;
    }
    return false;
}

/// Gets a count of child Components by type
template <typename T>
std::size_t Component2::count() {
    std::size_t count = 0;
    for (std::size_t c = 0; c < children_.size(); ++c) {
        if (dynamic_cast<T*>(children_[c]))
            count++;
    }
    return count;
}

}  // namespace mel

#endif  // MEL_COMPONENT2_HPP
