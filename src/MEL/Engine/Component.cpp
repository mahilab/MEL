#include <MEL/Engine/Component.hpp>
#include <MEL/Utility/Console.hpp>
#include <MEL/Engine/Engine.hpp>
#include <MEL/Logging/Log.hpp>
#include <algorithm>
#include <MEL/Engine/Object.hpp>
#include <MEL/Utility/Types.hpp>

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

    std::string Component::get_type() {
        return type(*this);
    }

} // namespace mel
