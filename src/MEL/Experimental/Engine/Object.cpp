#include <MEL/Experimental/Engine/Engine.hpp>
#include <MEL/Experimental/Engine/Object.hpp>
#include <MEL/Utility/Console.hpp>
#include <MEL/Experimental/Engine/Component.hpp>
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
    std::cout << name << " [";
    for (std::size_t i = 0; i < components_.size(); ++i) {
        std::cout << components_[i]->get_type();
        if (i < (components_.size() - 1))
            std::cout << ", ";
    }
    std::cout << "]" << std::endl;
    for (std::size_t i = 0; i < children_.size(); ++i)
        children_[i]->print_family_tree(level + 1);
}

//=============================================================================
// PRIVATE FUNCTIONS
//=============================================================================

void Object::add(Component* component, std::type_index type) {
    component->object_ = this;
    components_.push_back(component);
    LOG(Verbose) << "Added Component of type " << demangle(type.name()) << " to Object " << name;
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
        components_[i]->start();
    // call children
    for (std::size_t i = 0; i < children_.size(); ++i)
        children_[i]->start_all();
}

void Object::update_all() {
    // call components
    for (std::size_t i = 0; i < components_.size(); ++i)
        components_[i]->update();
    // call children
    for (std::size_t i = 0; i < children_.size(); ++i)
        children_[i]->update_all();
}

void Object::late_update_all() {
    // call children in reverse order
    for (auto it = children_.rbegin(); it != children_.rend(); ++it)
        (*it)->late_update_all();
    // call components in reverse order
    for (auto it = components_.rbegin(); it != components_.rend(); ++it)
        (*it)->late_update();
}

void Object::stop_all() {
    // call children in reverse order
    for (auto it = children_.rbegin(); it != children_.rend(); ++it)
        (*it)->stop_all();
    // call components in reverse order
    for (auto it = components_.rbegin(); it != components_.rend(); ++it)
        (*it)->stop();
}

void Object::reset_all() {
    // call components
    for (std::size_t i = 0; i < components_.size(); ++i)
        components_[i]->reset();
    // call children
    for (std::size_t i = 0; i < children_.size(); ++i)
        children_[i]->reset_all();
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
