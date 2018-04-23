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

#ifndef MEL_OBJECT_HPP
#define MEL_OBJECT_HPP

#include <string>
#include <vector>
#include <typeindex>
#include <unordered_map>
#include <MEL/Engine/Detail/Detail.hpp>
#include <string>

namespace mel {

/// BaseObject class
class BaseObject {
public:

    BaseObject(const std::string& name, BaseObject* parent = nullptr);

    virtual void update() {}

    /// Sets the parent of this Object
    void set_parent(BaseObject* parent);

    /// Adds a child Object to this Object
    void add_child(BaseObject* child);

    /// Removes a child Object from this Object
    void remove_child(BaseObject* child_object);

    /// Removes a child Object from this Object by name
    void remove_child(const std::string& child_name);

    /// Removes all child Objects from tis Object
    void remove_all_children();

    /// Prints Object familiy tree
    void print_family_tree(int level = 0);

public:

    const std::string name;

//private:

    /// Parent Object of this Object
    BaseObject* parent_;
    /// Children Objects of this Object
    std::vector<BaseObject*> children_;
    /// Map of childlren names to children_ indices
    std::unordered_map<std::string, std::size_t> children_map_;

private:

    /// Adds an Object to the Object registry
    static bool register_object(BaseObject* object);

    /// Removes an Object from the Object registry
    static bool unregister_object(BaseObject* object);

private:

    /// Registry of all Objects
    static std::unordered_map<std::string, BaseObject*> object_registry_;

};


/// Object Class
template <typename ... TComponents>
class Object : public BaseObject {
public:

    /// Constructor
    Object(const std::string& name, 
           const TComponents& ... args, 
           BaseObject* parent = nullptr) : 
        BaseObject(name, parent), components(args...) { }

    /// Updates all Components attached to this Object
    void update() {
        for_each_in_tuple(components, update_functor());
    }

    /// Gets a Component attached to this Object
    template <typename TComponent>
    TComponent* get() {
        return &std::get<TComponent>(components);
    }

private:

    /// Componets attached to this Object
    std::tuple<TComponents ...> components;
};

}  // namespace mel

#endif  // MEL_OBJECT_HPP

//template <typename TObject> 
//void set_references(TObject& object) {
//    t = &object.template get<T>();
//}
//T* t;
//for_each_in_tuple(components, set_references_functor<Object>(this)); //< in ctor