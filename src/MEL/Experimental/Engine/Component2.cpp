#include <MEL/Experimental/Engine/Component2.hpp>
#include <MEL/Experimental/Engine/Engine.hpp>
#include <MEL/Logging/Log.hpp>
#include <MEL/Utility/Console.hpp>
#include <MEL/Utility/Types.hpp>
#include <algorithm>
#include <sstream>

namespace mel {

//=============================================================================
// INTERFACE FUNCTIONS
//=============================================================================

void Component2::start() {}

void Component2::update() {}

void Component2::late_update() {}

void Component2::stop() {}

void Component2::reset() {}

//==============================================================================
// CONSTRUCTOR / DESTRUCTOR
//==============================================================================

Component2::Component2(const std::string& name)
    : name_(name), parent_(nullptr) {
    // create function array
    functions_[Start]      = &Component2::start;
    functions_[Update]     = &Component2::update;
    functions_[LateUpdate] = &Component2::late_update;
    functions_[Stop]       = &Component2::stop;
    functions_[Reset]      = &Component2::reset;
    // create default orderings
    orderings_[Start]      = BeforeChildrenForward;
    orderings_[Update]     = BeforeChildrenForward;
    orderings_[LateUpdate] = AfterChildrenReverse;
    orderings_[Stop]       = AfterChildrenReverse;
    orderings_[Reset]      = BeforeChildrenForward;
    // regster component in global registry
    register_component(this);
}

Component2::~Component2() {
    remove_all();
    unregister_component(this);
}

//==============================================================================
// PUBLIC FUNCTIONS
//==============================================================================

void Component2::order(Function function, Order order) {
    orderings_[function] = order;
}

void Component2::add(Component2* component) {
    component->parent_ = this;
    children_.push_back(component);
    ownership_mask_.push_back(false);
}

void Component2::remove_all() {
    for (std::size_t i = 0; i < children_.size(); ++i) {
        if (ownership_mask_[i])
            delete children_[i];
    }
    children_.clear();
}

std::size_t Component2::count() {
    return children_.size();
}

void Component2::name(const std::string& name) {
    name_ = name;
}

const std::string& Component2::name() {
    return name_;
}

std::string Component2::type() {
    return mel::type(*this);
}

void recurse_family_tree(Component2* component, int level, std::stringstream& ss) {
    for (int i = 0; i < level - 1; ++i)
        ss << "|   ";
    if (level > 0)
        ss << "|---";
    ss << component->name() << " <" << component->type() << ">" << "\n";
    for (std::size_t i = 0; i < component->count(); ++i)
        recurse_family_tree(component->get<Component2>(i), level + 1, ss);
}


std::string Component2::family_tree() {
    std::stringstream ss;
    recurse_family_tree(this, 0, ss);
    std::string family_tree = ss.str();
    family_tree.pop_back();
    return family_tree;
}

//==============================================================================
// PRIVATE FUNCTIONS
//==============================================================================

void Component2::set_engine(Engine* engine) {
    for (std::size_t i = 0; i < children_.size(); ++i)
        children_[i]->set_engine(engine);
    engine_ = engine;
}

void Component2::call(Function function) {
    switch (orderings_[function]) {
        case BeforeChildrenForward:
            (this->*functions_[function])();
            for (auto it = children_.begin(); it != children_.end(); ++it)
                (*it)->call(function);
            break;
        case BeforeChildrenReverse:
            (this->*functions_[function])();
            for (auto it = children_.rbegin(); it != children_.rend(); ++it)
                (*it)->call(function);
            break;
        case AfterChildrenForward:
            for (auto it = children_.begin(); it != children_.end(); ++it)
                (*it)->call(function);
            (this->*functions_[function])();
            break;
        case AfterChildrenReverse:
            for (auto it = children_.rbegin(); it != children_.rend(); ++it)
                (*it)->call(function);
            (this->*functions_[function])();
            break;
    };
}

//=============================================================================
// STATIC FUNCTIONS
//=============================================================================

std::unordered_map<std::string, Component2*> Component2::component_registry_;

bool Component2::register_component(Component2* component) {
    if (Component2::component_registry_.count(component->name())) {
        LOG(Error) << "Component named " << component->name()
                   << " already exists";
        return false;
    } else {
        Component2::component_registry_[component->name()] = component;
        return true;
    }
}

bool Component2::unregister_component(Component2* component) {
    if (Component2::component_registry_.count(component->name())) {
        Component2::component_registry_.erase(component->name());
        return true;
    } else {
        return false;
    }
}

}  // namespace mel
