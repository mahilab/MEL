#include <MEL/Engine/Engine.hpp>
#include <MEL/Engine/Object.hpp>
#include <MEL/Utility/Console.hpp>
#include <MEL/Engine/Component.hpp>
#include <MEL/Logging/Log.hpp>

namespace mel {

//=============================================================================
// CONSTRUCTOR / DESTRUCTOR
//=============================================================================

Object::Object(const std::string& name, Object* parent) :
    name(name),
    parent_(nullptr)
{
    if (register_object(this) && parent) {
        parent->add_child(this);
    }
}

Object::~Object() {
    // delete components
    for (std::size_t i = 0; i < components_.size(); ++i) {
        delete components_[i];
    }
    unregister_object(this);
}

//=============================================================================
// PUBLIC FUNCTIONS
//=============================================================================

void Object::add_child(Object* child) {
    // check if child already has a parent
    if (child->parent_) {
        LOG(Error) << "Object " << child->name << " already a child of Object " << child->parent_->name;
        return;
    }
    if (children_map_.count(child->name) == 0) {
        children_map_.insert({ child->name, children_.size() });
        children_.push_back(child);
        child->parent_ = this;
        LOG(Verbose) << "Object " << child->name << " added to Object " << name;
    }
    else {
        LOG(Warning) << "Object " << child->name << " already exists in Object " << name;
    }
}

void Object::remove_child(Object* child_object) {
    remove_child(child_object->name);
}

void Object::remove_child(const std::string& child_name) {
    if (children_map_.count(child_name)) {
        // nullify child Engine
        children_[children_map_[child_name]]->set_engine(nullptr);
        // remove child from children vector
        children_.erase(children_.begin() + children_map_[child_name]);
        // update children map
        children_map_.erase(child_name);
        for (std::size_t i = 0; i < children_.size(); ++i) {
            children_map_[children_[i]->name] = i;
        }
        LOG(Verbose) << "Child Object " << child_name << " removed from Object " << name;
    }
    else {
        LOG(Warning) << "Child Object " << child_name << " does not exist in Object " << name;
    }
}

void Object::remove_all_children() {
    std::vector<std::string> children_names;
    for (auto it = children_map_.begin(); it != children_map_.end(); ++it)
        children_names.push_back(it->first);
    for (std::size_t i = 0; i < children_names.size(); ++i)
        remove_child(children_names[i]);
}

void Object::set_parent(Object* parent_object) {
    parent_object->add_child(this);
}

void Object::print_family_tree(int level) {
    for (int i = 0; i < level; ++i) {
        std::cout << "    ";
    }
    std::cout << name << std::endl;
    for (std::size_t i = 0; i < children_.size(); ++i)
        children_[i]->print_family_tree(level + 1);
}

//=============================================================================
// PRIVATE FUNCTIONS
//=============================================================================

void Object::add_component(Component* component, std::type_index type) {
    if (components_map_.count(type)) {
        LOG(Error) << "Object " << "name already has a Component of type " << type.name();
        delete component;
        return;
    }
    component->object_ = this;
    components_map_[type] = components_.size();
    components_.push_back(component);
    LOG(Verbose) << "Added Component of type " << type.name() << " to Object " << name;
}

Component* Object::get_component(std::type_index type) {
    if (components_map_.count(type)) {
        return components_[components_map_[type]];
    }
    LOG(Error) << "Object " << name << " has no Component of type " << type.name();
    return nullptr;
}

Object* Object::get_child(const std::string& child_name) {
    if (children_map_.count(child_name)) {
        return children_[children_map_[child_name]];
    }
    else {
        LOG(Error) << "Object " << name << " has no child Object " << child_name;
        return nullptr;
    }
}

Object* Object::get_global_object_(const std::string& object_name) {
    if (object_registry_.count(object_name)) {
        return object_registry_[object_name];
    }
    else {
        LOG(Error) << "No Object with name " << object_name << " exists";
        return nullptr;
    }
}

//=============================================================================
// RECURSIVE FUNCTIONS
//=============================================================================

void Object::set_engine(Engine* engine) {
    for (std::size_t i = 0; i < children_.size(); ++i)
        children_[i]->set_engine(engine);
    engine_ = engine;
}

void Object::start_all() {
    // call components
    for (std::size_t i = 0; i < components_.size(); ++i)
        components_[i]->on_start();
    // call children
    for (std::size_t i = 0; i < children_.size(); ++i)
        children_[i]->start_all();
}

void Object::update_all() {
    // call components
    for (std::size_t i = 0; i < components_.size(); ++i)
        components_[i]->on_update();
    // call children
    for (std::size_t i = 0; i < children_.size(); ++i)
        children_[i]->update_all();
}

void Object::late_update_all() {
    // call components
    for (std::size_t i = 0; i < components_.size(); ++i)
        components_[i]->on_late_update();
    // call children
    for (std::size_t i = 0; i < children_.size(); ++i)
        children_[i]->late_update_all();
}

void Object::stop_all() {
    // call components
    for (std::size_t i = 0; i < components_.size(); ++i)
        components_[i]->on_stop();
    // call children
    for (std::size_t i = 0; i < children_.size(); ++i)
        children_[i]->stop_all();
}

void Object::reset_all() {
    // call components
    for (std::size_t i = 0; i < components_.size(); ++i)
        components_[i]->on_reset();
    // call children
    for (std::size_t i = 0; i < children_.size(); ++i)
        children_[i]->reset_all();
}

bool Object::enforce_requirements() {
    bool componet_req_met = true;
    bool children_req_met = true;
    // call components
    for (std::size_t i = 0; i < components_.size(); ++i)
        if (!components_[i]->enforce_requirements())
            componet_req_met = false;
    // call children
    for (std::size_t i = 0; i < children_.size(); ++i)
        if (!children_[i]->enforce_requirements())
            children_req_met = false;
    return componet_req_met && children_req_met;
}

//=============================================================================
// STATIC FUNCTIONS
//=============================================================================

std::unordered_map<std::string, Object*> Object::object_registry_;

bool Object::register_object(Object* object) {
    if (Object::object_registry_.count(object->name)) {
        LOG(Error) << "Object named " << object->name << " already exists";
        return false;
    }
    else {
        Object::object_registry_[object->name] = object;
        return true;
    }
}

bool Object::unregister_object(Object* object) {
    if (Object::object_registry_.count(object->name)) {
        Object::object_registry_.erase(object->name);
        return true;
    }
    else {
        return false;
    }
}

}  // namespace mel
