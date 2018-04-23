#include <MEL/Engine/Component.hpp>
#include <MEL/Utility/Console.hpp>
#include <MEL/Engine/Engine.hpp>
#include <MEL/Logging/Log.hpp>
#include <algorithm>
#include <MEL/Engine/Object.hpp>

namespace mel {

//=============================================================================
// INTERFACE FUNCTIONS
//=============================================================================

    Component::~Component() {}

    void Component::start() {}

    void Component::update() {}

    void Component::late_update() {}

    void Component::stop() {}

    void Component::reset() {}

//=============================================================================
// PRIVATE FUNCTIONS
//=============================================================================

    std::string Component::get_type_name() {
        std::string name = typeid(*this).name();
        return name.erase(0, 6);
    }

} // namespace mel
