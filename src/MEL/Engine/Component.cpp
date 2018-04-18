#include <MEL/Engine/Component.hpp>
#include <MEL/Utility/Console.hpp>
#include <MEL/Engine/Engine.hpp>
#include <MEL/Logging/Log.hpp>
#include <algorithm>

namespace mel {

//=============================================================================
// INTERFACE FUNCTIONS
//=============================================================================

    Component::~Component() {}

    void Component::on_start() {}

    void Component::on_update() {}

    void Component::on_late_update() {}

    void Component::on_stop() {}

    void Component::on_reset() {}

//=============================================================================
// PRIVATE FUNCTIONS
//=============================================================================

    void Component::add_requirement(std::type_index type) {
        if (std::find(requirements_.begin(), requirements_.end(), type) != requirements_.end()) {
            LOG(Warning) << "Component of type " << type.name() << " already required";
        }
        else {
            requirements_.push_back(type);
        }
    }

    bool Component::enforce_requirements() {
        bool requirements_met = true;
        for (std::size_t i = 0; i < requirements_.size(); ++i) {
            if (object_->components_map_.count(requirements_[i]) == 0) {
                LOG(Error) << "Component of type " << get_type_name() << " on Object " << object_->name << " requires Component of type " << requirements_[i].name();
                requirements_met = false;
            }
        }
        return requirements_met;
    }

    std::string Component::get_type_name() {
        std::string name = typeid(*this).name();
        return name.erase(0, 6);
    }

} // namespace mel
