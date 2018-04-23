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

    std::string Component::get_type_name() {
        std::string name = typeid(*this).name();
        return name.erase(0, 6);
    }

} // namespace mel
