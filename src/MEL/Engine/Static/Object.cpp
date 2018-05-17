#include <MEL/Engine/Static/Component.hpp>
#include <MEL/Engine/Static/Engine.hpp>
#include <MEL/Engine/Static/Object.hpp>
#include <MEL/Logging/Log.hpp>
#include <MEL/Utility/Console.hpp>

namespace mel {

//=============================================================================
// CONSTRUCTOR / DESTRUCTOR
//=============================================================================

BaseObject::BaseObject() : name(""), parent_(nullptr) {}

BaseObject::BaseObject(const std::string& object_name, BaseObject* parent)
    : name(object_name), parent_(nullptr) {
    if (register_object(this) && parent) {
        parent->add_child(*this);
    }
}

//=============================================================================
// PUBLIC FUNCTIONS
//=============================================================================

void BaseObject::set_parent(BaseObject& parent_object) {
    parent_object.add_child(*this);
}

void BaseObject::add_child(BaseObject& child) {
    // check if child already has a parent
    if (child.parent_) {
        LOG(Error) << "Object " << child.name << " already a child of Object "
                   << child.parent_->name;
        return;
    }
    if (&child == this) {
        LOG(Error) << "Object " << name << " cannot be made a child of itself";
        return;
    }
    if (children_map_.count(child.name) == 0) {
        children_map_.insert({child.name, children_.size()});
        children_.push_back(&child);
        child.parent_ = this;
        LOG(Verbose) << "Object " << child.name << " added to Object " << name;
    } else {
        LOG(Warning) << "Object " << child.name << " already exists in Object "
                     << name;
    }
}

void BaseObject::remove_child(BaseObject* child_object) {
    remove_child(child_object->name);
}

void BaseObject::remove_child(const std::string& child_name) {
    if (children_map_.count(child_name)) {
        // nullify child Engine
        // children_[children_map_[child_name]]->set_engine(nullptr);
        // remove child from children vector
        children_.erase(children_.begin() + children_map_[child_name]);
        // update children map
        children_map_.erase(child_name);
        for (std::size_t i = 0; i < children_.size(); ++i) {
            children_map_[children_[i]->name] = i;
        }
        LOG(Verbose) << "Child Object " << child_name << " removed from Object "
                     << name;
    } else {
        LOG(Warning) << "Child Object " << child_name
                     << " does not exist in Object " << name;
    }
}

void BaseObject::remove_all_children() {
    std::vector<std::string> children_names;
    for (auto it = children_map_.begin(); it != children_map_.end(); ++it)
        children_names.push_back(it->first);
    for (std::size_t i = 0; i < children_names.size(); ++i)
        remove_child(children_names[i]);
}

void BaseObject::print_family_tree(int level) {
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

//=============================================================================
// RECURSIVE FUNCTIONS
//=============================================================================

//=============================================================================
// STATIC FUNCTIONS
//=============================================================================

std::unordered_map<std::string, BaseObject*> BaseObject::object_registry_;

bool BaseObject::register_object(BaseObject* object) {
    if (BaseObject::object_registry_.count(object->name)) {
        LOG(Error) << "Object named " << object->name << " already exists";
        return false;
    } else {
        BaseObject::object_registry_[object->name] = object;
        return true;
    }
}

bool BaseObject::unregister_object(BaseObject* object) {
    if (BaseObject::object_registry_.count(object->name)) {
        BaseObject::object_registry_.erase(object->name);
        return true;
    } else {
        return false;
    }
}

}  // namespace mel
